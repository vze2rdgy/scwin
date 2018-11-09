//
// AppTabView.xaml.cpp
// Implementation of the AppTabView class
//

#include "pch.h"
#include "StatusMessageTemplateSelector.h"
#include "AppTabView.xaml.h"
#include "MainDashboard.xaml.h"
#include "Products.xaml.h"
#include "Customers.xaml.h"
#include "COrgGridView.xaml.h"
#include "Orders.xaml.h"
#include "UserAdminView.xaml.h"
#include "ProductInventories.xaml.h"
#include "cpplinq.hpp"
#include "SettingsView.xaml.h"

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

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

#define DEFAULTMENUITEM "Home:Dashboard"
#define LASTMENUITEMPROPNAME "LastSelectedAppMenuItem"
#define SETTINGSMENUITEM "Settings:Settings"

AppTabView::AppTabView(scMainViewModel^ vm) : 
	NavItemToPageViewMap(
		{
			{"Home",{{"Dashboard",  TypeName(MainDashboard::typeid)}}},
			{"Catalog",{
				{"Products",  TypeName(Products::typeid)},
				{"Analytics",  TypeName(Products::typeid)},
				{"Brochures",  TypeName(Products::typeid)}
			}},
			{"Customers",{
				{"Customers",  TypeName(Customers::typeid)},
				{"Contacts",  TypeName(Customers::typeid)},
				{"Analytics",  TypeName(Customers::typeid)}
			}},
			{"Orders",{
				{"Orders",  TypeName(Orders::typeid)},
				{"Analytics",  TypeName(Orders::typeid)}
			}},
			{"Admin",{
				{"Users",  TypeName(UserAdminView::typeid)},
				{"Products",  TypeName(Products::typeid)},
				{"Inventory",  TypeName(ProductInventories::typeid)},
				{"Customers",  TypeName(COrgGridView::typeid)},
				{"Orders",  TypeName(Orders::typeid)}
			}},
			{"Settings", {
				{"Settings", TypeName(SettingsView::typeid)}
			}}
		}
	)
{
	ViewModel = vm;
	auto selector = ref new StatusMessageTemplateSelector(Resources);
	Resources->Insert(L"StatusMessageTemplateSelector", selector);
	InitializeComponent();

	InitializeNavigationRequirements();

	// navigate to last selected menu item
	auto lastMenuItem = scLocalDataService::Instance()->GetProperty(LASTMENUITEMPROPNAME);
	String^ lastMenuItemName = DEFAULTMENUITEM;
	if ( lastMenuItem != nullptr && !StringHelpers::IsNullOrEmpty(lastMenuItem->Value))
		if (lastMenuItem->Value != SETTINGSMENUITEM)
			lastMenuItemName = lastMenuItem->Value;
	
	auto mv = to_vector(NavView->MenuItems);
	for (auto obj : mv)
	{
		auto mi = dynamic_cast<NavigationViewItem^>(obj);
		if (mi != nullptr)
		{
			if (mi->Tag->ToString() == lastMenuItemName)
			{
				//mi->IsSelected = true;
				NavView->SelectedItem = mi;
				break;
			}
		}
	}
}

Windows::UI::Xaml::Controls::Frame^ AppTabView::AppFrame::get()
{
	return frame;
}

void scwin::AppTabView::InitializeNavigationRequirements()
{
	Windows::UI::Core::SystemNavigationManager::GetForCurrentView()->
		BackRequested += ref new EventHandler<Windows::UI::Core::BackRequestedEventArgs^>(this, &AppTabView::SystemNavigationManager_BackRequested);
	auto navm = SystemNavigationManager::GetForCurrentView();
	navm->AppViewBackButtonVisibility = AppViewBackButtonVisibility::Visible;
	navm->BackRequested += ref new EventHandler<BackRequestedEventArgs^>(this, &AppTabView::NavManager_BackRequested);
	NavigationCacheMode = Windows::UI::Xaml::Navigation::NavigationCacheMode::Required;
}

void scwin::AppTabView::SystemNavigationManager_BackRequested(Object ^ sender, Windows::UI::Core::BackRequestedEventArgs ^ e)
{
	if (!e->Handled && AppFrame->CanGoBack)
	{
		e->Handled = true;

		auto pageInfo = to_vector(AppFrame->BackStack).back();

		Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, pageInfo]()
		{
			// dispatch to update the menu selection.
			auto tpl = GetHeaderAndPage(pageInfo->SourcePageType);
			// find the menuitem
			String^ concated = String::Concat(String::Concat(std::get<0>(tpl), ":"), std::get<1>(tpl));
			for (auto obj : to_vector(this->NavView->MenuItems))
			{
				auto mitem = static_cast<NavigationViewItemBase^>(obj);
				String^ tag = dynamic_cast<String^>(mitem->Tag);
				if (!StringHelpers::IsNullOrEmpty(tag) && tag == concated)
				{
					mitem->IsSelected = true;
					break;
				}
			}
		}));
		AppFrame->GoBack();
	}
}

void scwin::AppTabView::NavManager_BackRequested(Object ^ sender, BackRequestedEventArgs ^ e)
{
	e->Handled = true;
}

void scwin::AppTabView::NavigateToPage(String^ navItemHeader, String^ navItemTitle)
{
	ViewModel->BreadCrumbs->Clear();
	ViewModel->BreadCrumbs->Append(navItemTitle);
	auto page = NavItemToPageViewMap[navItemHeader][navItemTitle];
	frame->Navigate(page);
}


void scwin::AppTabView::NavView_SelectionChanged(Windows::UI::Xaml::Controls::NavigationView^ sender, Windows::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs^ args)
{
	String^ menuItemName = nullptr;
	if (args->IsSettingsSelected) {

		menuItemName = SETTINGSMENUITEM;
	}
	else
	{
		menuItemName = ((NavigationViewItemBase^)args->SelectedItem)->Tag->ToString();
	}
	if (!StringHelpers::IsNullOrEmpty(menuItemName))
	{
		auto v = StringHelpers::Split(menuItemName, ':');
		String^ headerName = v->GetAt(0);
		String^ menuName = v->GetAt(1);
		ViewModel->PageTitle = headerName + " - " + menuName;
		NavigateToPage(headerName, menuName);
		AppProperty^ lastMenuItem = nullptr;
		lastMenuItem = ref new AppProperty(LASTMENUITEMPROPNAME, menuItemName);
		lastMenuItem->IsRoaming = true;
		scLocalDataService::Instance()->SetProperty(lastMenuItem);
	}
}

void scwin::AppTabView::btnRemoveMesage_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (ViewModel->StatusMessages != nullptr)
	{
		auto toremove = dynamic_cast<StatusMessage^>(((Control^)sender)->DataContext);
		auto v = to_vector(ViewModel->StatusMessages);
		auto pos = find(v.cbegin(), v.cend(), toremove);
		if (pos != v.cend())
		{
			ViewModel->StatusMessages->RemoveAt(pos - v.cbegin());
		}
	}
}


void scwin::AppTabView::frame_Navigating(Platform::Object^ sender, Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs^ e)
{

}

tuple<String^, String^> scwin::AppTabView::GetHeaderAndPage(TypeName typeName)
{
	using namespace cpplinq;
	for (auto kvp0 : NavItemToPageViewMap)
	{
		for (auto kvp1 : kvp0.second)
		{
			if (kvp1.second.Name == typeName.Name /*&& kvp1.second.Kind == typeName.Kind*/)
			{
				//return make_tuple(kvp0.first, kvp1.first);
				return { kvp0.first, kvp1.first };
			}
		}
	}
	//return make_tuple(ref new String(L""), ref new String(L""));
	return { ref new String(L""), ref new String(L"") };
}