#include "pch.h"
#include "ContentControlRevised.h"


DependencyProperty^ ContentControlRevised::_dataTemplateNameProperty = nullptr;

void scwin::ContentControlRevised::Register()
{
	if (_dataTemplateNameProperty == nullptr)
		_dataTemplateNameProperty = DependencyProperty::Register(
			"DataTemplateName", String::typeid, 
			ContentControlRevised::typeid, 
			ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(ContentControlRevised::OnDataTemplateNameChanged)));
}

ContentControlRevised::ContentControlRevised()
{
	//Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::ContentControlRevised::OnLoaded);
	SizeChanged += ref new Windows::UI::Xaml::SizeChangedEventHandler(this, &scwin::ContentControlRevised::OnSizeChanged);
}


ContentControlRevised::~ContentControlRevised()
{
}

void scwin::ContentControlRevised::OnApplyTemplate()
{
	ContentControl::OnApplyTemplate();
}

void scwin::ContentControlRevised::OnContentTemplateChanged(DataTemplate^ oldContentTemplate, DataTemplate^ newContentTemplate)
{
	ContentControl::OnContentTemplateChanged(oldContentTemplate, newContentTemplate);
}

//void scwin::ContentControlRevised::OnLoaded(Object ^ sender, RoutedEventArgs ^ eargs)
//{
//	Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]() { UpdateLayout(); }));
//}


void scwin::ContentControlRevised::OnSizeChanged(Platform::Object ^sender, Windows::UI::Xaml::SizeChangedEventArgs ^e)
{
	UpdateLayout();
}

void scwin::ContentControlRevised::OnDataTemplateNameChanged(DependencyObject ^ dp, DependencyPropertyChangedEventArgs ^ eargs)
{
	auto strName = (String^)eargs->NewValue;
	if (!StringHelpers::IsNullOrEmpty(strName))
	{
		auto ctl = (ContentControl^)dp;
		ctl->ContentTemplate = (DataTemplate^)ctl->Resources->Lookup(strName);
	}
}
