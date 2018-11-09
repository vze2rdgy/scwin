#include "pch.h"
#include "ValidationMask.h"
#include "ColorHelpers.h"
#include "NativeTypeWrapper.h"

using namespace scwin;

/*
private static readonly DependencyPropertyKey _maskExpressionPropertyKey = DependencyProperty.RegisterAttachedReadOnly("MaskExpression",
typeof(Regex),
typeof(ColorMasking),
new FrameworkPropertyMetadata());

/// <summary>
/// Identifies the <see cref="Mask"/> dependency property.
/// </summary>
///
public static readonly DependencyProperty PassColorProperty = DependencyProperty.RegisterAttached("PassColor",
typeof(string),
typeof(ColorMasking),
new PropertyMetadata("#99FF99"));

public static readonly DependencyProperty FailColorProperty = DependencyProperty.RegisterAttached("FailColor",
typeof(string),
typeof(ColorMasking),
new PropertyMetadata("#FFCCFF"));

public static readonly DependencyProperty MaskProperty = DependencyProperty.RegisterAttached("Mask",
typeof(string),
typeof(ColorMasking),
new FrameworkPropertyMetadata(OnMaskChanged));


/// <summary>
/// Identifies the <see cref="MaskExpression"/> dependency property.
/// </summary>
public static readonly DependencyProperty MaskExpressionProperty = _maskExpressionPropertyKey.DependencyProperty;

*/


DependencyProperty^ ColorMasking::passColorProperty = nullptr;
DependencyProperty^ ColorMasking::failColorProperty = nullptr;
DependencyProperty^ ColorMasking::maskProperty = nullptr;
DependencyProperty^ ColorMasking::maskExprProperty = nullptr;
DependencyProperty^ ColorMasking::failedMessageProperty = nullptr;
EventRegistrationToken ColorMasking::keyUpEventToken;

void scwin::ColorMasking::RegisterDependencyProperties()
{
	if (passColorProperty == nullptr)
		passColorProperty =
			DependencyProperty::RegisterAttached("PassColor", String::typeid, ColorMasking::typeid, ref new PropertyMetadata(L"#99FF99"));
	if (failColorProperty == nullptr)
		failColorProperty =
			DependencyProperty::RegisterAttached("FailColor", String::typeid, ColorMasking::typeid, ref new PropertyMetadata(L"#FFCCFF"));
	if (maskProperty == nullptr)
		maskProperty =
			DependencyProperty::RegisterAttached("Mask", String::typeid, ColorMasking::typeid, ref new PropertyMetadata(L"", ref new PropertyChangedCallback(&ColorMasking::OnMaskChanged)));
	if (maskExprProperty == nullptr)
		maskExprProperty =
			DependencyProperty::RegisterAttached("MaskExpression", RGWrapper::typeid, ColorMasking::typeid, nullptr);
	if (failedMessageProperty == nullptr)
		failedMessageProperty =
			DependencyProperty::RegisterAttached("FailedMessage", String::typeid, ColorMasking::typeid, ref new PropertyMetadata(L"Not valid entry for this input box"));
}

void scwin::ColorMasking::UnregisterDependencyProperties()
{

}

ColorMasking::ColorMasking()
{

}

ColorMasking::~ColorMasking()
{
}

void scwin::ColorMasking::SetMaskExpression(DependencyObject ^ obj, RGWrapper^ maskExpression)
{
	auto textBox = (TextBox^)obj;
	textBox->SetValue(maskExprProperty, maskExpression);
}

RGWrapper^ scwin::ColorMasking::GetMaskExpression(DependencyObject ^ obj)
{
	auto textBox = (TextBox^)obj;
	auto wrapper = (RGWrapper^)textBox->GetValue(maskExprProperty);
	return wrapper;
}

void scwin::ColorMasking::textBox_KeyUp(Object ^ obj, KeyRoutedEventArgs ^ e)
{
	auto txt = (TextBox^)obj;
	String^ val = txt->Text;
	auto maskexpr = GetMaskExpression(txt);
	if (maskexpr == nullptr)
	{
		return;
	}

	String^ passHex = dynamic_cast<String^>(txt->GetValue(PassColorProperty));
	String^ failHex = dynamic_cast<String^>(txt->GetValue(FailColorProperty));
	Color passColor = ColorHelpers::ToColorFromHex(passHex);
	Color failColor = ColorHelpers::ToColorFromHex(failHex);
	std::wcmatch m;
	try
	{
		if (std::regex_match(val->Data(), m, *maskexpr->get()) && m.size() > 1)
		{
			// some match exists.
			txt->BorderBrush = ref new Windows::UI::Xaml::Media::SolidColorBrush(Colors::Green);
			txt->BorderThickness = Windows::UI::Xaml::Thickness(0);
			ToolTipService::SetToolTip(txt, L"");
		}
		else
		{
			txt->BorderBrush = ref new Windows::UI::Xaml::Media::SolidColorBrush(Colors::Red);
			txt->BorderThickness = Windows::UI::Xaml::Thickness(1, 1, 20, 1);
			ToolTipService::SetToolTip(txt, (String^)txt->GetValue(FailedMessageProperty));
		}
		ToolTipService::SetPlacement(txt, Windows::UI::Xaml::Controls::Primitives::PlacementMode::Bottom);
	}
	catch (const std::regex_error& /* ex */)
	{
		txt->Background = ref new Windows::UI::Xaml::Media::SolidColorBrush(failColor);
	}
}

void scwin::ColorMasking::OnPassColorChanged(DependencyObject ^ dependencyObject, DependencyPropertyChangedEventArgs ^ e)
{
	auto textBox = (TextBox^)dependencyObject;
	auto color = (String^)e->NewValue;
	textBox->SetValue(PassColorProperty, color);
}

//String ^ scwin::ColorMasking::GetProposedTextBackspace(TextBox ^ textBox)
//{
//	auto text = GetTextWithSelectionRemoved(textBox);
//	if (textBox->SelectionStart > 0)
//	{
//		text = scwin::utils::StringHelpers::Substring(text, textBox->SelectionStart - 1, 1);
//	}
//	return text;
//}
//
//String ^ scwin::ColorMasking::GetProposedText(TextBox ^ textBox, String ^ newText)
//{
//	auto text = GetTextWithSelectionRemoved(textBox);
//	text += newText;
//	return text;
//}
//
//String ^ scwin::ColorMasking::GetTextWithSelectionRemoved(TextBox ^ textBox)
//{
//	String^ text = textBox->Text;
//	if (textBox->SelectionStart != -1)
//	{
//		text = scwin::utils::StringHelpers::Substring(text, textBox->SelectionStart, textBox->SelectionLength);
//	}
//	return text;
//}

void ColorMasking::OnMaskChanged(DependencyObject^ obj, DependencyPropertyChangedEventArgs^ e)
{
	using namespace Windows::UI::Xaml::Input;
	auto textBox = safe_cast<TextBox^>(obj);
	auto mask = dynamic_cast<String^>(e->NewValue);
	textBox->KeyUp -= keyUpEventToken;
	if (mask == nullptr)
	{
		textBox->ClearValue(MaskProperty);
		textBox->ClearValue(MaskExpressionProperty);
	}
	else
	{
		textBox->SetValue(MaskProperty, mask);
		keyUpEventToken = textBox->KeyUp += ref new KeyEventHandler(&ColorMasking::textBox_KeyUp);
		// store regex related data.
		auto rg = new std::wregex(mask->Data(), mask->Length());
		RGWrapper^ wrapper = ref new RGWrapper(rg);
		SetMaskExpression(obj, wrapper);
	}
}

String ^ scwin::ColorMasking::GetMask(TextBox ^ textBox)
{
	if (textBox == nullptr)
	{
		throw ref new InvalidArgumentException("textBox");
	}
	return (String^)textBox->GetValue(MaskProperty);
}

void scwin::ColorMasking::SetMask(TextBox ^ textBox, String ^ mask)
{
	if (textBox == nullptr)
	{
		throw ref new InvalidArgumentException("textBox");
	}
	textBox->SetValue(MaskProperty, mask);
}

