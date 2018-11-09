//
// CompositionShadow.xaml.h
// Declaration of the CompositionShadow class
//

#pragma once

#include "CompositionShadow.g.h"

namespace scwin
{
	[Windows::Foundation::Metadata::WebHostHidden]
	[Windows::UI::Xaml::Markup::ContentProperty(Name = "CastingElement")]
	public ref class CompositionShadow sealed
	{
		static Windows::UI::Xaml::DependencyProperty^ blurRadiusProperty;
		static Windows::UI::Xaml::DependencyProperty^ colorProperty;
		static Windows::UI::Xaml::DependencyProperty^ offsetXProperty;
		static Windows::UI::Xaml::DependencyProperty^ offsetYProperty;
		static Windows::UI::Xaml::DependencyProperty^ offsetZProperty;
		static Windows::UI::Xaml::DependencyProperty^ shadowOpacityProperty;

		FrameworkElement^ castingElement;
		Windows::UI::Composition::DropShadow^ dropShadow;
		Windows::UI::Composition::SpriteVisual^ shadowVisual;
		EventRegistrationToken castingelementSizeChangedEventToken;

	public:

		static void Register();

		CompositionShadow();

		property FrameworkElement^ CastingElement
		{
			FrameworkElement^ get();
			void set(FrameworkElement^);
		}

		static property Windows::UI::Xaml::DependencyProperty^ BlurRadiusProperty
		{
			Windows::UI::Xaml::DependencyProperty^ get() { return blurRadiusProperty; }
		}

		static property Windows::UI::Xaml::DependencyProperty^ ColorProperty
		{
			Windows::UI::Xaml::DependencyProperty^ get() { return colorProperty; }
		}

		static property Windows::UI::Xaml::DependencyProperty^ OffsetXProperty
		{
			Windows::UI::Xaml::DependencyProperty^ get() { return offsetXProperty; }
		}

		static property Windows::UI::Xaml::DependencyProperty^ OffsetYProperty
		{
			Windows::UI::Xaml::DependencyProperty^ get() { return offsetYProperty; }
		}

		static property Windows::UI::Xaml::DependencyProperty^ OffsetZProperty
		{
			Windows::UI::Xaml::DependencyProperty^ get() { return offsetZProperty; }
		}

		static property Windows::UI::Xaml::DependencyProperty^ ShadowOpactityProperty
		{
			Windows::UI::Xaml::DependencyProperty^ get() { return shadowOpacityProperty; }
		}

		property double BlurRadius
		{
			double get() 
			{ 
				return (double)GetValue(blurRadiusProperty); 
			}
			void set(double val) 
			{ 
				SetValue(blurRadiusProperty, val); 
			}
		}

		property Windows::UI::Color Color
		{
			Windows::UI::Color get() 
			{ 
				return (Windows::UI::Color)GetValue(colorProperty); 
			}
			void set(Windows::UI::Color val) { SetValue(colorProperty, val); }
		}

		property double OffsetX
		{
			double get() 
			{ 
				return (double)GetValue(offsetXProperty); 
			}
			void set(double val) { SetValue(offsetXProperty, val); }
		}

		property double OffsetY
		{
			double get() 
			{ 
				return (double)GetValue(offsetYProperty); 
			}
			void set(double val) { SetValue(offsetYProperty, val); }
		}

		property double OffsetZ
		{
			double get() 
			{ 
				return (double)GetValue(offsetZProperty); 
			}
			void set(double val) { SetValue(offsetZProperty, val); }
		}

		property double ShadowOpacity
		{
			double get()
			{
				auto boxed = dynamic_cast<IBox<double>^>(GetValue(shadowOpacityProperty));
				return boxed != nullptr ? boxed->Value : 0.;
			}
			void set(double val)
			{
				SetValue(shadowOpacityProperty, val);
			}
		}

		property Windows::UI::Composition::CompositionBrush^ Mask
		{
			Windows::UI::Composition::CompositionBrush^ get() { return dropShadow->Mask; }
			void set(Windows::UI::Composition::CompositionBrush^ val) { dropShadow->Mask = val; }
		}

		property Windows::UI::Composition::DropShadow^ DropShadow
		{
			Windows::UI::Composition::DropShadow^ get() { return dropShadow; }
		}

		property Windows::UI::Composition::Visual^ Visual
		{
			Windows::UI::Composition::Visual^ get(){ return shadowVisual; }
		}

	private:
		static void OnBlurRadiusChanged(DependencyObject^ obj, DependencyPropertyChangedEventArgs^ e);
		static void OnColorChanged(DependencyObject^ obj, DependencyPropertyChangedEventArgs^ e);
		static void OnOffsetXChanged(DependencyObject^ obj, DependencyPropertyChangedEventArgs^ e);
		static void OnOffsetYChanged(DependencyObject^ obj, DependencyPropertyChangedEventArgs^ e);
		static void OnOffsetZChanged(DependencyObject^ obj, DependencyPropertyChangedEventArgs^ e);
		static void OnShadowOpacityChanged(DependencyObject^ obj, DependencyPropertyChangedEventArgs^ e);

		void OnSizeChanged(Object^ sender, SizeChangedEventArgs^ e);
		void OnCastingElementSizeChanged(Object^ sender, SizeChangedEventArgs^ e);

		void ConfigureShadowVisualForCastingElement();

		void UpdateShadowMask();

		void UpdateShadowOffset(float32 x, float32 y, float32 z);

		void UpdateShadowSize();
	};
}
