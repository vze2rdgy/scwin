#include "pch.h"
#include "BreadcrumbControl.h"
#include "cpplinq.hpp"

ResourceDictionary^ BreadcrumbControl::_styleResources = nullptr;
DependencyProperty^ BreadcrumbControl::_HomeIconProperty = nullptr;
DependencyProperty^ BreadcrumbControl::_ItemsSourceProperty = nullptr;
DependencyProperty^ BreadcrumbControl::_ItemsProperty = nullptr;
DependencyProperty^ BreadcrumbControl::_HomeTemplateProperty = nullptr;
DependencyProperty^ BreadcrumbControl::_ItemTemplateProperty = nullptr;
DependencyProperty^ BreadcrumbControl::_SeperatorTemplateProperty = nullptr;
DependencyProperty^ BreadcrumbControl::_OverFlowTemplateProperty = nullptr;
DependencyProperty^ BreadcrumbControl::_DisplayMemberPathProperty = nullptr;
DependencyProperty^ BreadcrumbControl::_SeperatorProperty = nullptr;
DependencyProperty^ BreadcrumbControl::_HomeTextProperty = nullptr;
DependencyProperty^ BreadcrumbControl::_OverFlowProperty = nullptr;
DependencyProperty^ BreadcrumbControl::_ItemStyleProperty = nullptr;
DependencyProperty^ BreadcrumbControl::_HomeItemStyleProperty = nullptr;
DependencyProperty^ BreadcrumbControl::_ItemCommandProperty = nullptr;
DependencyProperty^ BreadcrumbControl::_HomeCommandProperty = nullptr;

void scwin::BreadcrumbControl::Register()
{
	if (_HomeIconProperty == nullptr)
	{
		_HomeIconProperty = DependencyProperty::Register("HomeIcon", IconElement::typeid, BreadcrumbControl::typeid,
			ref new PropertyMetadata(ref new SymbolIcon(Symbol::Home)));
	}
	if (_ItemsSourceProperty == nullptr)
	{
		_ItemsSourceProperty = DependencyProperty::Register("ItemsSource", 
			Object::typeid, 
			Object::typeid,
			ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(&BreadcrumbControl::OnItemsSourcePropertyChanged)));
	}
	if (_ItemsProperty == nullptr)
	{
		_ItemsProperty = DependencyProperty::Register(
			"Items", 
			IObservableVector<Object^>::typeid,
			BreadcrumbControl::typeid,
			ref new PropertyMetadata(ref new Vector<Object^>()));
	}
	if (_HomeTemplateProperty == nullptr)
	{
		_HomeTemplateProperty = DependencyProperty::Register("HomeTemplate",
			DataTemplate::typeid,
			BreadcrumbControl::typeid,
			ref new PropertyMetadata(nullptr));
	}

	if (_ItemTemplateProperty == nullptr)
	{
		_ItemTemplateProperty = DependencyProperty::Register("ItemTemplate",
			DataTemplate::typeid,
			BreadcrumbControl::typeid,
			ref new PropertyMetadata(nullptr));
	}

	if (_SeperatorTemplateProperty == nullptr)
	{
		_SeperatorTemplateProperty = DependencyProperty::Register("SeperatorTemplate",
			DataTemplate::typeid,
			BreadcrumbControl::typeid,
			ref new PropertyMetadata(nullptr));
	}

	if (_OverFlowTemplateProperty == nullptr)
	{
		_OverFlowTemplateProperty = DependencyProperty::Register("OverFlowTemplate",
			DataTemplate::typeid,
			BreadcrumbControl::typeid,
			ref new PropertyMetadata(nullptr));
	}

	if (_DisplayMemberPathProperty == nullptr)
	{
		_DisplayMemberPathProperty = DependencyProperty::Register("DisplayMemberPath",
			String::typeid,
			BreadcrumbControl::typeid,
			ref new PropertyMetadata(nullptr));
	}

	if (_SeperatorProperty == nullptr)
	{
		_SeperatorProperty = DependencyProperty::Register("Seperator",
			String::typeid,
			BreadcrumbControl::typeid,
			ref new PropertyMetadata(nullptr));
	}

	if (_HomeTextProperty == nullptr)
	{
		_HomeTextProperty = DependencyProperty::Register("HomeText",
			String::typeid,
			BreadcrumbControl::typeid,
			ref new PropertyMetadata(nullptr));
	}

	if (_OverFlowProperty == nullptr)
	{
		_OverFlowProperty = DependencyProperty::Register("OverFlow",
			String::typeid,
			BreadcrumbControl::typeid,
			ref new PropertyMetadata(nullptr));
	}

	if (_ItemStyleProperty == nullptr)
	{
		_ItemStyleProperty = DependencyProperty::Register("ItemStyle",
			Style::typeid,
			BreadcrumbControl::typeid,
			ref new PropertyMetadata(nullptr));
	}

	if (_HomeItemStyleProperty == nullptr)
	{
		_HomeItemStyleProperty = DependencyProperty::Register("HomeItemStyle",
			Style::typeid,
			BreadcrumbControl::typeid,
			ref new PropertyMetadata(nullptr));
	}

	if (_ItemCommandProperty == nullptr)
	{
		_ItemCommandProperty = DependencyProperty::Register("ItemCommand",
			ICommand ::typeid,
			BreadcrumbControl::typeid,
			ref new PropertyMetadata(nullptr));
	}

	if (_HomeCommandProperty == nullptr)
	{
		_HomeCommandProperty = DependencyProperty::Register("HomeCommand",
			ICommand::typeid,
			BreadcrumbControl::typeid,
			ref new PropertyMetadata(nullptr));
	}
}

BreadcrumbControl::BreadcrumbControl()
{
	DefaultStyleKey = TypeName(BreadcrumbControl::typeid);
	if (Windows::ApplicationModel::DesignMode::DesignModeEnabled)
		return;
	Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::BreadcrumbControl::OnLoaded);
}


void scwin::BreadcrumbControl::OnApplyTemplate()
{
	using namespace cpplinq;
	ContentControl::OnApplyTemplate();
	_stackPanel = (StackPanel^)GetTemplateChild("PART_Stack");
	if (_stackPanel == nullptr) return;
	_stackPanel->Children->Clear();
	if (Items == nullptr || !Items->Size)
		return;
	for (auto item : to_vector(Items))
	{
		SetSeperatorItem(_stackPanel->Children, SeperatorTemplate, Seperator);
		SetItem(_stackPanel->Children, item, Items, ItemTemplate, DisplayMemberPath);
	}
	UpdateLayout(); // in order to measure actual width
	Vector<BreadcrumbSeparator^>^ seperators = ref new Vector<BreadcrumbSeparator^>();
	Vector<BreadcrumbItem^>^ items = ref new Vector<BreadcrumbItem^>();
	for (auto uiel : to_vector(_stackPanel->Children))
	{
		auto seperator = dynamic_cast<BreadcrumbSeparator^>(uiel);
		if (seperator != nullptr)
		{
			seperators->Append(seperator);
			continue;
		}
		auto item = dynamic_cast<BreadcrumbItem^>(uiel);
		if (item != nullptr)
			items->Append((BreadcrumbItem^)uiel);
	}
	auto i = 0;
	while (_stackPanel->ActualWidth > ActualWidth)
	{
		if (i < items->Size - 1)
		{
			auto item = items->GetAt(i);
			if (item == nullptr)
				break;
			SetOverflowItem(item, OverFlowTemplate, OverFlow);
			i++;
		}
		else
		{
			auto item = from(to_vector(items)) >> first_or_default([](BreadcrumbItem^ item)
			{
				return item->Visibility == Windows::UI::Xaml::Visibility::Visible;
			});
			auto seperator = from(to_vector(seperators)) >> first_or_default([](BreadcrumbSeparator^ item)
			{
				return item->Visibility == Windows::UI::Xaml::Visibility::Visible;
			});
			if (seperator != nullptr) 
				seperator->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
			if (item != nullptr)
				item->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		}
		UpdateLayout();
	}
}

void scwin::BreadcrumbControl::OnHomeSelected()
{
	auto emptyargs = ref new RoutedEventArgs();
	HomeSelected(this, emptyargs);
	if (HomeCommand == nullptr) return;
	if (HomeCommand->CanExecute(nullptr)) 
		HomeCommand->Execute(emptyargs);
}

void scwin::BreadcrumbControl::OnItemSelected(BreadcrumbEventArgs ^ eargs)
{
	ItemSelected(this, eargs);
	if (ItemCommand == nullptr) return;
	if (ItemCommand->CanExecute(nullptr)) 
		ItemCommand->Execute(eargs);
}

void scwin::BreadcrumbControl::OnItemsSourcePropertyChanged(DependencyObject ^ d, DependencyPropertyChangedEventArgs ^ e)
{
	auto control = static_cast<BreadcrumbControl^>(d);
	auto oldVal = dynamic_cast<IObservableVector<Object^>^>(e->OldValue);
	auto newVal = dynamic_cast<IObservableVector<Object^>^>(e->NewValue);
	if (control != nullptr) 
		control->OnItemsSourceChanged(oldVal, newVal);
}

void scwin::BreadcrumbControl::OnItemsSourceChanged(IObservableVector<Object^>^ oldValue, IObservableVector<Object^>^ newValue)
{
	if (oldValue != nullptr)
		oldValue->VectorChanged -= _itemsSourceNotifyChangedEventToken;
	Items = newValue;
	if (newValue != nullptr)
		_itemsSourceNotifyChangedEventToken = newValue->VectorChanged += ref new VectorChangedEventHandler<Object^>(this, &scwin::BreadcrumbControl::ItemsOnCollectionChanged);
}

void scwin::BreadcrumbControl::ItemsOnCollectionChanged(IObservableVector<Object^>^ sender, IVectorChangedEventArgs ^ eargs)
{
	//switch (eargs->CollectionChange)
	//{
	//case CollectionChange::ItemChanged:
	//	break;
	//case CollectionChange::ItemInserted:

	//	break;
	//case CollectionChange::ItemRemoved:
	//	break;
	//case CollectionChange::Reset:
	//	break;
	//}
	OnApplyTemplate();
}

void scwin::BreadcrumbControl::SetSeperatorItem(UIElementCollection ^ collection, DataTemplate ^ seperatorTemplate, String ^ seperator)
{
	auto sep = ref new BreadcrumbSeparator();
	if (seperatorTemplate != nullptr)
	{
		sep->ContentTemplate = seperatorTemplate;
	}
	else
	{
		sep->Content = seperator;
	}
	sep->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Center;
	collection->Append(sep);
}

void scwin::BreadcrumbControl::SetOverflowItem(BreadcrumbItem ^ item, DataTemplate ^ overflowTemplate, String ^ overFlow)
{
	item->Content = nullptr;
	item->ContentTemplate = nullptr;
	if (overflowTemplate != nullptr)
		item->ContentTemplate = overflowTemplate;
	else
		item->Content = overFlow;
}

void scwin::BreadcrumbControl::SetItem(UIElementCollection ^ collection, Object^ item, IObservableVector<Object^>^ items, DataTemplate ^ itemTemplate, String ^ displayMemberPath)
{
	auto content = CreateButton();
	if (itemTemplate != nullptr)
	{
		content->ContentTemplate = itemTemplate;
		content->DataContext = item;
	}
	else
	{
		auto binding = ref new Binding();
		binding->Source = item;
		binding->Path = ref new PropertyPath(displayMemberPath);
		content->SetBinding(ContentProperty, binding);
	}
	content->Click += ref new RoutedEventHandler([this, item, items](Object^ sender, RoutedEventArgs^ eargs)
	{
		unsigned int index;
		if (items->IndexOf(item, &index))
		{
			OnItemSelected(ref new BreadcrumbEventArgs(item, index));
		}
	});
	collection->Append(content);
}

BreadcrumbItem ^ scwin::BreadcrumbControl::CreateButton()
{
	auto btn = ref new BreadcrumbItem();
	btn->Style = (this->ItemStyle == nullptr ? (Windows::UI::Xaml::Style^)Application::Current->Resources->Lookup("BreadcrumbButtonStyle") : this->ItemStyle);
	return btn;
}

void scwin::BreadcrumbControl::SetHomeItem(UIElementCollection ^ collection)
{
	auto home = ref new BreadcrumbHome();
	home->Style = this->HomeItemStyle != nullptr ? this->HomeItemStyle : (Windows::UI::Xaml::Style^)Application::Current->Resources->Lookup("BreadcrumbButtonStyle");
	if (StringHelpers::IsNullOrEmpty(HomeText))
	{
		if (HomeTemplate == nullptr)
		{
			home->ContentTemplate = (Windows::UI::Xaml::DataTemplate^)Application::Current->Resources->Lookup("BreadCrumbHome");
			home->DataContext = HomeIcon;
		}
		else
		{
			home->ContentTemplate = HomeTemplate;
		}
	}
	else
	{
		home->Content = HomeText;
	}
	home->Click += ref new RoutedEventHandler([this](Object^ sender, RoutedEventArgs^ eargs)
	{
		OnHomeSelected();
	});
	collection->Append(home);
}

ResourceDictionary ^ scwin::BreadcrumbControl::GetStyleResources()
{
	if (_styleResources == nullptr)
	{
		_styleResources = ref new ResourceDictionary();
		Application::LoadComponent(_styleResources, ref new Uri("ms-appx:///scwin/BreadcrumbControlResourceDict.xaml"));
	}
	return _styleResources;
}

EventRegistrationToken BreadcrumbControl::HomeSelected::add(RoutedEventHandler^ h)
{
	return _homeSelected += h;
}

void BreadcrumbControl::HomeSelected::remove(EventRegistrationToken token)
{
	_homeSelected -= token;
}

void BreadcrumbControl::HomeSelected::raise(Object^ sender, RoutedEventArgs^ args)
{
	_homeSelected(sender, args);
}


EventRegistrationToken BreadcrumbControl::ItemSelected::add(BreadcrumbEventHandler^ h)
{
	return _itemSelected += h;
}

void BreadcrumbControl::ItemSelected::remove(EventRegistrationToken token)
{
	_itemSelected -= token;
}

void BreadcrumbControl::ItemSelected::raise(BreadcrumbControl^ sender, BreadcrumbEventArgs^ args)
{
	_itemSelected(sender, args);
}


void scwin::BreadcrumbControl::OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{

}
