//
// CompositionShadow.xaml.cpp
// Implementation of the CompositionShadow class
//

#include "pch.h"
#include "CompositionShadow.xaml.h"

using namespace scwin;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Composition;
using namespace Windows::UI::Xaml::Hosting;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

Windows::UI::Xaml::DependencyProperty^ scwin::CompositionShadow::blurRadiusProperty = nullptr;
Windows::UI::Xaml::DependencyProperty^ scwin::CompositionShadow::colorProperty = nullptr;
Windows::UI::Xaml::DependencyProperty^ scwin::CompositionShadow::offsetXProperty = nullptr;
Windows::UI::Xaml::DependencyProperty^ scwin::CompositionShadow::offsetYProperty = nullptr;
Windows::UI::Xaml::DependencyProperty^ scwin::CompositionShadow::offsetZProperty = nullptr;
Windows::UI::Xaml::DependencyProperty^ scwin::CompositionShadow::shadowOpacityProperty = nullptr;

void scwin::CompositionShadow::Register()
{
	if (blurRadiusProperty == nullptr)
		blurRadiusProperty = DependencyProperty::Register("BlurRadius", double::typeid, CompositionShadow::typeid,
			ref new PropertyMetadata(9.0, ref new PropertyChangedCallback(&CompositionShadow::OnBlurRadiusChanged)));
	if (colorProperty == nullptr)
		colorProperty = DependencyProperty::Register("Color", Color::typeid, CompositionShadow::typeid,
			ref new PropertyMetadata(Colors::Black, ref new PropertyChangedCallback(&CompositionShadow::OnColorChanged)));
	if (offsetXProperty == nullptr)
		offsetXProperty = DependencyProperty::Register("OffsetX", double::typeid, CompositionShadow::typeid,
			ref new PropertyMetadata(0.0, ref new PropertyChangedCallback(&CompositionShadow::OnOffsetXChanged)));
	if (offsetYProperty == nullptr)
		offsetYProperty = DependencyProperty::Register("OffsetY", double::typeid, CompositionShadow::typeid,
			ref new PropertyMetadata(0.0, ref new PropertyChangedCallback(&CompositionShadow::OnOffsetYChanged)));
	if (offsetZProperty == nullptr)
		offsetZProperty = DependencyProperty::Register("OffsetZ", double::typeid, CompositionShadow::typeid,
			ref new PropertyMetadata(0.0, ref new PropertyChangedCallback(&CompositionShadow::OnOffsetZChanged)));
	if (shadowOpacityProperty == nullptr)
		shadowOpacityProperty = DependencyProperty::Register("ShadowOpacity", double::typeid, CompositionShadow::typeid,
			ref new PropertyMetadata(1.0, ref new PropertyChangedCallback(&CompositionShadow::OnShadowOpacityChanged)));
}

scwin::CompositionShadow::CompositionShadow()
{
	InitializeComponent();
	DefaultStyleKey = CompositionShadow::typeid;
	SizeChanged += ref new SizeChangedEventHandler(this, &CompositionShadow::OnSizeChanged);
	Loaded += ref new RoutedEventHandler([this](Object^ sender, RoutedEventArgs^ e)
	{
		ConfigureShadowVisualForCastingElement();
	});
	Compositor^ compositor = ElementCompositionPreview::GetElementVisual(this)->Compositor;
	shadowVisual = compositor->CreateSpriteVisual();
	dropShadow = compositor->CreateDropShadow();
	shadowVisual->Shadow = dropShadow;
	// SetElementChildVisual on the control itself ("this") would result in the shadow
	// rendering on top of the content. To avoid this, CompositionShadow contains a Border (see xaml)
	// (to host the shadow) and a ContentPresenter (to host the actual content, "CastingElement").
	ElementCompositionPreview::SetElementChildVisual(ShadowElement, shadowVisual);
}

FrameworkElement^ scwin::CompositionShadow::CastingElement::get()
{
	return castingElement;
}

void scwin::CompositionShadow::CastingElement::set(FrameworkElement^ val)
{
	if (castingElement != nullptr)
		castingElement->SizeChanged -= castingelementSizeChangedEventToken;
	castingElement = val;
	castingelementSizeChangedEventToken = castingElement->SizeChanged += ref new SizeChangedEventHandler(this, &scwin::CompositionShadow::OnCastingElementSizeChanged);
	ConfigureShadowVisualForCastingElement();
}

void scwin::CompositionShadow::OnBlurRadiusChanged(DependencyObject ^ obj, DependencyPropertyChangedEventArgs ^ e)
{
	auto boxed = dynamic_cast<IBox<double>^>(e->NewValue);
	((scwin::CompositionShadow^)obj)->dropShadow->BlurRadius = boxed != nullptr ? boxed->Value : 0.;
}

void scwin::CompositionShadow::OnColorChanged(DependencyObject ^ obj, DependencyPropertyChangedEventArgs ^ e)
{
	auto boxed = dynamic_cast<IBox<Windows::UI::Color>^>(e->NewValue);
	((scwin::CompositionShadow^)obj)->dropShadow->Color = boxed != nullptr ? boxed->Value : Colors::Black;
}

void scwin::CompositionShadow::OnOffsetXChanged(DependencyObject ^ obj, DependencyPropertyChangedEventArgs ^ e)
{
	scwin::CompositionShadow^ cs = safe_cast<scwin::CompositionShadow^>(obj);
	auto offset = cs->dropShadow->Offset;
	auto xoffset = 0.;
	auto boxed = dynamic_cast<IBox<double>^>(e->NewValue);
	if (boxed != nullptr)
		xoffset = boxed->Value;
	cs->UpdateShadowOffset(xoffset, offset.y, offset.z);
}

void scwin::CompositionShadow::OnOffsetYChanged(DependencyObject ^ obj, DependencyPropertyChangedEventArgs ^ e)
{
	scwin::CompositionShadow^ cs = safe_cast<scwin::CompositionShadow^>(obj);
	auto offset = cs->dropShadow->Offset;
	auto yoffset = 0.;
	auto boxed = dynamic_cast<IBox<double>^>(e->NewValue);
	if (boxed != nullptr)
		yoffset = boxed->Value;
	cs->UpdateShadowOffset(offset.x, yoffset, offset.z);
}

void scwin::CompositionShadow::OnOffsetZChanged(DependencyObject ^ obj, DependencyPropertyChangedEventArgs ^ e)
{
	scwin::CompositionShadow^ cs = safe_cast<scwin::CompositionShadow^>(obj);
	auto offset = cs->dropShadow->Offset;
	auto zoffset = 0.;
	auto boxed = dynamic_cast<IBox<double>^>(e->NewValue);
	if (boxed != nullptr)
		zoffset = boxed->Value;
	cs->UpdateShadowOffset(offset.x, offset.y, zoffset);
}

void scwin::CompositionShadow::OnShadowOpacityChanged(DependencyObject ^ obj, DependencyPropertyChangedEventArgs ^ e)
{
	scwin::CompositionShadow^ cs = safe_cast<scwin::CompositionShadow^>(obj);
	auto boxed = dynamic_cast<IBox<double>^>(e->NewValue);
	if (boxed != nullptr)
		cs->dropShadow->Opacity = boxed->Value;
}

void scwin::CompositionShadow::OnSizeChanged(Object ^ sender, SizeChangedEventArgs ^ e)
{
	UpdateShadowSize();
}

void scwin::CompositionShadow::OnCastingElementSizeChanged(Object ^ sender, SizeChangedEventArgs ^ e)
{
	UpdateShadowSize();
}

void scwin::CompositionShadow::ConfigureShadowVisualForCastingElement()
{
	UpdateShadowMask();
	UpdateShadowSize();
}

void scwin::CompositionShadow::UpdateShadowMask()
{
	using namespace Windows::UI::Xaml::Shapes;
	if (castingElement != nullptr)
	{
		CompositionBrush^ mask = nullptr;
		if (dynamic_cast<Image^>(castingElement) != nullptr)
		{
			mask = ((Image^)castingElement)->GetAlphaMask();
		}
		else if (dynamic_cast<Shape^>(castingElement) != nullptr)
		{
			mask = ((Shape^)castingElement)->GetAlphaMask();
		}
		else if (dynamic_cast<TextBlock^>(castingElement) != nullptr)
		{
			mask = ((TextBlock^)castingElement)->GetAlphaMask();
		}
		dropShadow->Mask = mask;
	}
	else
	{
		dropShadow->Mask = nullptr;
	}
}

void scwin::CompositionShadow::UpdateShadowOffset(float32 x, float32 y, float32 z)
{
	dropShadow->Offset = Windows::Foundation::Numerics::float3(x, y, z);
}

void scwin::CompositionShadow::UpdateShadowSize()
{
	using namespace Windows::Foundation::Numerics;
	float2 newSize(ActualWidth, ActualHeight);
	if (castingElement != nullptr)
	{
		newSize = float2(castingElement->ActualWidth, castingElement->ActualHeight);
	}
	shadowVisual->Size = newSize;
}
