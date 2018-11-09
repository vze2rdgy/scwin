//
// AppNavigator.xaml.cpp
// Implementation of the AppNavigator class
//

#include "pch.h"
#include "AppNavigator.xaml.h"
#include "XamlHelper.h"
#include "CompositionShadow.xaml.h"
#include "MainDashboard.xaml.h"
#include "Products.xaml.h"
#include "Customers.xaml.h"
#include "Orders.xaml.h"
#include "UserAdminView.xaml.h"
#include "ProductInventories.xaml.h"


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

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

DependencyProperty^ scwin::AppNavigator::selectedSubmenuItemsProperty = nullptr;
DependencyProperty^ scwin::AppNavigator::selectedSubmenuItemProperty = nullptr;

void scwin::AppNavigator::Register()
{
	if (selectedSubmenuItemsProperty == nullptr)
		selectedSubmenuItemsProperty = DependencyProperty::Register(
			"SelectedSubmenuItems",
			IObservableVector<NavMenuItem^>::typeid,
			AppNavigator::typeid,
			ref new PropertyMetadata(nullptr)
		);
	if (selectedSubmenuItemProperty == nullptr)
		selectedSubmenuItemProperty = DependencyProperty::Register(
			"SelectedSubmenuItem",
			NavMenuItem::typeid,
			AppNavigator::typeid,
			ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback([](DependencyObject^ obj, DependencyPropertyChangedEventArgs^ eargs)
			{
				auto navp = (AppNavigator^)obj;
				auto navMenu = (NavMenuItem^)eargs->NewValue;
				if (navMenu)
				{
					auto lvs = scLocalDataService::Instance();
					auto lastMenuItem = ref new AppProperty(String::Concat("LastSelectedAppMenuItem_", navMenu->ParentLabel), navMenu->Label);
					lastMenuItem->IsRoaming = true;
					lvs->SetProperty(lastMenuItem);
				}
	}
		)));
}

AppNavigator::AppNavigator()
{
	InitializeComponent();

	KeyDown += ref new KeyEventHandler(this, &AppNavigator::KeyDownHandler);

	AppNavIcons = ref new Vector<NavMenuItem^>();
	auto icon = ref new NavMenuItem("Home");
	icon->ImageSource = "Assets/Home@2x.png";
	AppNavIcons->Append(icon);

	icon = ref new NavMenuItem("Catalog");
	icon->ImageSource = "Assets/Catalog@2x.png";
	AppNavIcons->Append(icon);

	icon = ref new NavMenuItem("Customers");
	icon->ImageSource = "Assets/Customers@2x.png";
	AppNavIcons->Append(icon);

	icon = ref new NavMenuItem("Orders");
	icon->ImageSource = "Assets/Orders@2x.png";
	AppNavIcons->Append(icon);

	icon = ref new NavMenuItem("Admin");
	icon->ImageSource = "Assets/Admin@2x.png";
	AppNavIcons->Append(icon);



	auto lvs = scLocalDataService::Instance();
	auto lastMenuItem = lvs->GetProperty("LastSelectedAppMenuItem");
	String^ menuName;
	if (lastMenuItem != nullptr)
		menuName = lastMenuItem->Value;
	if (StringHelpers::IsNullOrEmpty(menuName))
		menuName = "Home";

	BuildSubmenus(menuName);
}

IObservableVector<NavMenuItem^>^ AppNavigator::SelectedSubmenuItems::get()
{
	return (IObservableVector<NavMenuItem^>^)GetValue(selectedSubmenuItemsProperty);
}

void AppNavigator::SelectedSubmenuItems::set(IObservableVector<NavMenuItem^>^ val)
{
	SetValue(selectedSubmenuItemsProperty, val);
}

NavMenuItem^ AppNavigator::SelectedSubmenuItem::get()
{
	return (NavMenuItem^)GetValue(selectedSubmenuItemProperty);
}

void AppNavigator::SelectedSubmenuItem::set(NavMenuItem^ val)
{
	SetValue(selectedSubmenuItemProperty, val);
}


void scwin::AppNavigator::OnKeyDown(KeyRoutedEventArgs ^ e)
{
	auto key = e->Key;
	switch (key)
	{
	case Windows::System::VirtualKey::Enter:
		OutputDebugStringA("Enter key pressed\n");
		break;
	default:
		UserControl::OnKeyDown(e);
	}
}

void scwin::AppNavigator::LogoutNavPaneButton_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	using namespace Windows::UI::Popups;
	MessageDialog^ dlg = ref new MessageDialog(L"Are you sure you want to signout?", L"Signout");
	dlg->Commands->Append(ref new UICommand("Yes"));
	dlg->Commands->Append(ref new UICommand("No"));
	dlg->CancelCommandIndex = 1;
	dlg->DefaultCommandIndex = 1;
	task<IUICommand^> showTask(dlg->ShowAsync());
	showTask.then([=](IUICommand^ cmd)
	{
		if (cmd->Label == L"Yes")
		{
			mainViewModel->Logout();
		}
	});
}


void scwin::AppNavigator::SettingsNavPaneButton_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	// original coding 
	//for (NavMenuItem^ i : navlist)
	//{
	//	i->IsSelected = false;
	//}
	//AppFrame->Navigate(TypeName(Account::typeid));
}


void scwin::AppNavigator::gvNavIcons_ItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto item = dynamic_cast<NavMenuItem^>(e->ClickedItem);
	if (item != nullptr)
	{
		auto name = item->Label;
		BuildSubmenus(name);
		auto lvs = scLocalDataService::Instance();
		auto lastMenuItem = lvs->GetProperty("LastSelectedAppMenuItem");
		lastMenuItem->Value = name;
		lastMenuItem->IsRoaming = true;
		lvs->SetProperty(lastMenuItem);
		CompositionShadow^ shadow;
		if (XamlHelper::GetAncester(this, &shadow))
		{
			shadow->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		}
	}
}


void scwin::AppNavigator::btnForNavBar_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	CompositionShadow^ shadow;
	if (XamlHelper::GetAncester(this, &shadow))
	{
		if (shadow->Visibility != Windows::UI::Xaml::Visibility::Visible)
			shadow->Visibility = Windows::UI::Xaml::Visibility::Visible;
		else
			shadow->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	}
}

#define FONTAWESOMETEXT "Assets/Fonts/fa-solid-900.ttf#Font Awesome 5 Free Solid"

void scwin::AppNavigator::BuildSubmenus(String ^ navHeaderName, String^ lastSelectedSubmenuItem)
{
	AppProperty^ lastMenuItem = nullptr;
	auto lvs = scLocalDataService::Instance();
	Vector<NavMenuItem^>^ menuItems = ref new Vector<NavMenuItem^>();
	if (navHeaderName == "Home")
	{
		auto m = ref new NavMenuItem("Dashboard", 0xf108, FONTAWESOMETEXT, TypeName(MainDashboard::typeid));
		m->ParentLabel = navHeaderName;
		menuItems->Append(m);
		lastMenuItem = lvs->GetProperty("LastSelectedAppMenuItem_Home");
	}
	else if (navHeaderName == "Catalog")
	{
		auto m = ref new NavMenuItem("Products", 0xf466, FONTAWESOMETEXT, TypeName(Products::typeid));
		m->ParentLabel = navHeaderName;
		menuItems->Append(m);
		m = ref new NavMenuItem("Analytics", 0xf200, FONTAWESOMETEXT, TypeName(Products::typeid));
		m->ParentLabel = navHeaderName;
		menuItems->Append(m);		
		m = ref new NavMenuItem("Brochures", 0xf02f, FONTAWESOMETEXT, TypeName(MainDashboard::typeid));
		m->ParentLabel = navHeaderName;
		menuItems->Append(m);
		lastMenuItem = lvs->GetProperty("LastSelectedAppMenuItem_Catalog");
	}
	else if (navHeaderName == "Customers")
	{
		auto m = ref new NavMenuItem("Customers", 0xf1ad, FONTAWESOMETEXT, TypeName(Customers::typeid));
		m->ParentLabel = navHeaderName;
		menuItems->Append(m);
		m = ref new NavMenuItem("Contacts", 0xf2b9, FONTAWESOMETEXT, TypeName(Customers::typeid));
		m->ParentLabel = navHeaderName;
		menuItems->Append(m);
		m = ref new NavMenuItem("Analytics", 0xf201, FONTAWESOMETEXT, TypeName(Customers::typeid));
		m->ParentLabel = navHeaderName;
		menuItems->Append(m);		
		lastMenuItem = lvs->GetProperty("LastSelectedAppMenuItem_Customers");
	}
	else if (navHeaderName == "Orders")
	{
		auto m = ref new NavMenuItem("Orders", 0xf1ad, FONTAWESOMETEXT, TypeName(Orders::typeid));
		m->ParentLabel = navHeaderName;
		menuItems->Append(m);
		m = ref new NavMenuItem("Analytics", 0xf080, FONTAWESOMETEXT, TypeName(Orders::typeid));
		m->ParentLabel = navHeaderName;
		menuItems->Append(m);		
		lastMenuItem = lvs->GetProperty("LastSelectedAppMenuItem_Orders");
	}
	else if (navHeaderName == "Admin")
	{
		auto m = ref new NavMenuItem("Manage Users", 0xf0c0, FONTAWESOMETEXT, TypeName(UserAdminView::typeid));
		m->ParentLabel = navHeaderName;
		menuItems->Append(m);
		m = ref new NavMenuItem("Manage Products", 0xf466, FONTAWESOMETEXT, TypeName(Products::typeid));
		m->ParentLabel = navHeaderName;
		menuItems->Append(m); 
		m = ref new NavMenuItem("Manage Inventory", 0xf472, FONTAWESOMETEXT, TypeName(ProductInventories::typeid));
		m->ParentLabel = navHeaderName;
		menuItems->Append(m);
		m = ref new NavMenuItem("Manage Customers", 0xf1ad, FONTAWESOMETEXT, TypeName(Customers::typeid));
		m->ParentLabel = navHeaderName;
		menuItems->Append(m);
		m = ref new NavMenuItem("Manage Orders", 0xf0d1, FONTAWESOMETEXT, TypeName(Orders::typeid));
		m->ParentLabel = navHeaderName;
		menuItems->Append(m);
		lastMenuItem = lvs->GetProperty("LastSelectedAppMenuItem_Admin");
	}

	SelectedSubmenuItems = menuItems;

	String^ menuName;
	if (lastMenuItem != nullptr)
	{
		menuName = lastMenuItem->Value;
		if (!StringHelpers::IsNullOrEmpty(menuName))
		{
			for (auto i = 0; i < menuItems->Size; i++)
			{
				auto mi = menuItems->GetAt(i);
				if (mi->Label == menuName)
				{
					SelectedSubmenuItem = mi;
					return;
				}
			}
		}
	}
	if (menuItems->Size > 0)
		SelectedSubmenuItem = menuItems->First()->Current;
}

void scwin::AppNavigator::KeyDownHandler(Platform::Object ^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs ^ e)
{
	auto key = e->Key;
	switch (key)
	{
	case Windows::System::VirtualKey::Escape:
		CompositionShadow ^ shadow;
		if (XamlHelper::GetAncester(this, &shadow))
		{
			shadow->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		}
		break;
	}
}
