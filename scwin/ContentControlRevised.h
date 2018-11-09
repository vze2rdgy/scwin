#pragma once

namespace scwin
{

	public ref class ContentControlRevised sealed : Windows::UI::Xaml::Controls::ContentControl
	{
		static DependencyProperty^ _dataTemplateNameProperty;

	public:

		static void Register();

		ContentControlRevised();

		virtual ~ContentControlRevised();

		static property DependencyProperty^ DataTemplateNameProperty
		{
			DependencyProperty^ get() { return _dataTemplateNameProperty; }
		}

		property String^ DataTemplateName
		{
			String^ get() { return (String^)GetValue(_dataTemplateNameProperty); }
			void set(String^ val) { SetValue(_dataTemplateNameProperty, val); }
		}

	protected:
		virtual void OnApplyTemplate() override;
		virtual void OnContentTemplateChanged(DataTemplate^ oldContentTemplate, DataTemplate^ newContentTemplate) override;

	private:
		//void OnLoaded(Object^ sender, RoutedEventArgs^ eargs);
		void OnSizeChanged(Platform::Object ^sender, Windows::UI::Xaml::SizeChangedEventArgs ^e);
		static void OnDataTemplateNameChanged(DependencyObject^ dp, DependencyPropertyChangedEventArgs^ eargs);
	};

}