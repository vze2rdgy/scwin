//
// GridOrListView.cpp
// Implementation of the GridOrListView class.
//

#include "pch.h"
#include "GridOrListView.h"

using namespace scwin;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Documents;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;

// The Templated Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234235


DependencyProperty^ GridOrListView::_defaultViewTypeProperty = nullptr;
DependencyProperty^ GridOrListView::_currentViewTypeProperty = nullptr;
DependencyProperty^ GridOrListView::_gridViewItemTemplateProperty = nullptr;
DependencyProperty^ GridOrListView::_listViewItemTemplateProperty = nullptr;
DependencyProperty^ GridOrListView::_displayAddButtonProperty = nullptr;
DependencyProperty^ GridOrListView::_addButtonLabelProperty = nullptr;
DependencyProperty^ GridOrListView::_addButtonTooltipProperty = nullptr;

void scwin::GridOrListView::Register()
{
	if (_defaultViewTypeProperty == nullptr)
	{
		_defaultViewTypeProperty = DependencyProperty::Register(
			"DefaultViewType",
			GridOrListViewViewType::typeid,
			GridOrListView::typeid,
			ref new PropertyMetadata(GridOrListViewViewType::Grid)
		);
	}
	if (_currentViewTypeProperty == nullptr)
	{
		_currentViewTypeProperty = DependencyProperty::Register(
			"CurrentViewType",
			GridOrListViewViewType::typeid,
			GridOrListView::typeid,
			ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(GridOrListView::OnCurrentViewTypeChanged))
		);
	}
	if (_listViewItemTemplateProperty == nullptr)
	{
		_listViewItemTemplateProperty = DependencyProperty::Register(
			"ListViewItemTemplate",
			DataTemplate::typeid,
			GridOrListView::typeid,
			ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(GridOrListView::OnItemViewTemplateChange))
		);
	}
	
	if (_gridViewItemTemplateProperty == nullptr)
	{
		_gridViewItemTemplateProperty = DependencyProperty::Register(
			"GridViewItemTemplate",
			DataTemplate::typeid,
			GridOrListView::typeid,
			ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(GridOrListView::OnItemViewTemplateChange))
		);
	}

	if (_displayAddButtonProperty == nullptr)
	{
		_displayAddButtonProperty = DependencyProperty::Register(
			"DisplayAddButton",
			Windows::UI::Xaml::Visibility::typeid,
			GridOrListView::typeid,
			ref new PropertyMetadata(Windows::UI::Xaml::Visibility::Collapsed)
		);
	}

	if (_addButtonLabelProperty == nullptr)
	{
		_addButtonLabelProperty = DependencyProperty::Register(
			"AddButtonLabel",
			String::typeid,
			GridOrListView::typeid,
			ref new PropertyMetadata("Add")
		);
	}

	if (_addButtonTooltipProperty == nullptr)
	{
		_addButtonTooltipProperty = DependencyProperty::Register(
			"AddButtonTooltip",
			String::typeid,
			GridOrListView::typeid,
			ref new PropertyMetadata("Add")
		);
	}
}

GridOrListView::GridOrListView()
{
	DefaultStyleKey = "scwin.GridOrListView";
	this->ItemsSource = nullptr;
}

void scwin::GridOrListView::OnApplyTemplate()
{
	ItemsControl::OnApplyTemplate();
	auto asbSearch = (AutoSuggestBox^)GetTemplateChild("asbSearch");
	asbSearch->TextChanged += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Xaml::Controls::AutoSuggestBox ^, Windows::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs ^>(this, &scwin::GridOrListView::OnAsbSearchChanged);
	_btnSwitch = (AppBarToggleButton^)GetTemplateChild("btnToggleToGridOrList");
	_btnSwitch->Checked += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::GridOrListView::OnbtnToggleToGridOrListChecked);
	_btnSwitch->Unchecked += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::GridOrListView::OnbtnToggleToGridOrListUnchecked);
	auto btnAdd = (AppBarButton^)GetTemplateChild("btnAdd");
	btnAdd->Click += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::GridOrListView::OnAddButtonClicked);
	if (GetValue(_currentViewTypeProperty) == nullptr)
	{
		SetValue(_currentViewTypeProperty, (GridOrListViewViewType)GetValue(_defaultViewTypeProperty));
	}
	else
	{
		UpdateView();
	}
}

void scwin::GridOrListView::OnCurrentViewTypeChanged(DependencyObject ^ dp, DependencyPropertyChangedEventArgs ^ eargs)
{
	auto glv = (GridOrListView^)dp;
	glv->UpdateView();
}

void scwin::GridOrListView::OnItemViewTemplateChange(DependencyObject ^ dp, DependencyPropertyChangedEventArgs ^ eargs)
{
	auto glv = (GridOrListView^)dp;
	glv->UpdateView();
}

void scwin::GridOrListView::OnAsbSearchChanged(AutoSuggestBox ^ sender, AutoSuggestBoxTextChangedEventArgs ^ eargs)
{
	SearchKeyChanged(this, sender->Text);
}

void scwin::GridOrListView::OnbtnToggleToGridOrListChecked(Object ^ sender, RoutedEventArgs ^ eargs)
{
	_btnSwitch->Icon = ref new SymbolIcon(Symbol::List);
	// change template
	ItemsPanel = (ItemsPanelTemplate^)Application::Current->Resources->Lookup("GridOrListViewListPanelTemplate");
	ItemTemplate = (DataTemplate^)GetValue(_listViewItemTemplateProperty);
	_bSuppressNotification = true;
	CurrentViewType = GridOrListViewViewType::List;
	_bSuppressNotification = false;
}

void scwin::GridOrListView::OnbtnToggleToGridOrListUnchecked(Object ^ sender, RoutedEventArgs ^ eargs)
{
	_btnSwitch->Icon = ref new SymbolIcon(Symbol::ViewAll);
	// change template
	ItemsPanel = (ItemsPanelTemplate^)Application::Current->Resources->Lookup("GridOrListViewGridPanelTemplate");
	ItemTemplate = (DataTemplate^)GetValue(_gridViewItemTemplateProperty);
	_bSuppressNotification = true;
	CurrentViewType = GridOrListViewViewType::Grid;
	_bSuppressNotification = false;
}

void scwin::GridOrListView::OnAddButtonClicked(Object ^ sender, RoutedEventArgs ^ eargs)
{
	_addButtonClicked(this, eargs);
}

void scwin::GridOrListView::UpdateView()
{
	if (!_bSuppressNotification)
	{
		if (_btnSwitch != nullptr)
		{
			GridOrListViewViewType viewType;
			auto currentViewType = GetValue(_currentViewTypeProperty);
			if (currentViewType == nullptr)
				viewType = (GridOrListViewViewType)GetValue(_defaultViewTypeProperty);
			else
				viewType = (GridOrListViewViewType)currentViewType;

			if (viewType == GridOrListViewViewType::Grid)
			{
				_btnSwitch->IsChecked = false;
				OnbtnToggleToGridOrListUnchecked(_btnSwitch, nullptr);
			}
			else
			{
				_btnSwitch->IsChecked = true;
				OnbtnToggleToGridOrListChecked(_btnSwitch, nullptr);
			}
		}
	}
}
