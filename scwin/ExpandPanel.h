//
// ExpandPanel.h
// Declaration of the ExpandPanel class.
//

#pragma once

namespace scwin
{
	public ref class ExpandPanel sealed : public Windows::UI::Xaml::Controls::ContentControl
	{
		static DependencyProperty^ headerContentProperty ;
		static DependencyProperty^  isExpandedProperty;
		static DependencyProperty^ cornerRadiusProperty;


		bool useTransitions = false;
		VisualState^ collapsedState;
		Windows::UI::Xaml::Controls::Primitives::ToggleButton^ toggleExpander;
		FrameworkElement^ contentElement;
		event RoutedEventHandler^ expandedEvent;


	public:

		static void RegisterDependencyProperties();

		static property DependencyProperty^ HeaderContentProperty
		{
			DependencyProperty^ get() { return headerContentProperty; }
		}
		static property DependencyProperty^ IsExpandedProperty
		{
			DependencyProperty^ get() { return isExpandedProperty; }
		}
		static property DependencyProperty^ CornerRadiusProperty
		{
			DependencyProperty^ get() { return cornerRadiusProperty; }
		}
		
		ExpandPanel();

		event RoutedEventHandler^ Expanded
		{
			EventRegistrationToken add(RoutedEventHandler^ evnt);
			void remove(EventRegistrationToken);
			void raise(Object^ sender, RoutedEventArgs^ args);
		}

		property Object^ HeaderContent
		{
			Object^ get() { return GetValue(headerContentProperty); }
			void set(Object^ val) { SetValue(headerContentProperty, val); }
		}
		property bool IsExpanded
		{
			bool get() { return (bool)GetValue(isExpandedProperty); }
			void set(bool val) { SetValue(isExpandedProperty, val); }
		}
		property Windows::UI::Xaml::CornerRadius CornerRadius
		{
			Windows::UI::Xaml::CornerRadius get() 
			{ 
				return (Windows::UI::Xaml::CornerRadius)GetValue(cornerRadiusProperty); 
			}
			void set(Windows::UI::Xaml::CornerRadius val)
			{
				SetValue(cornerRadiusProperty, val); 
			}
		}


	private:
		void ChangeVisualState(bool useTransitions);

	protected:
		virtual void OnApplyTemplate() override;

	private:
		void OnClick(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
		void OnCompleted(Platform::Object ^sender, Platform::Object ^args);
	};
}
