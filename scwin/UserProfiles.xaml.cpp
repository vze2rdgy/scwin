//
// UserProfiles.xaml.cpp
// Implementation of the UserProfiles class
//

#include "pch.h"
#include "UserProfiles.xaml.h"
#include "cpplinq.hpp"

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

UserProfiles::UserProfiles()
{
	InitializeComponent();

	ViewModel = ref new UserViewModel();

	//String^ s = L"\\A(?=[a-z0-9@\.!#$%&'*+\/=?^_`{|}~-]{6,254}\\z)(?=[a-z0-9\.!#$%&'*+/=?^_`{|}~-]{1,64}@)[a-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*@ (?:(?=[a-z0-9-]{1,63}\\\.)[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\\\.)+(?=[a-z0-9-]{1,63}\\z)[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\\z";
	//ColorMasking::SetMask(txtUserEmail, s);

	this->DataContextChanged += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Xaml::FrameworkElement ^, Windows::UI::Xaml::DataContextChangedEventArgs ^>(this, &scwin::UserProfiles::OnDataContextChanged);

	this->Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::UserProfiles::OnLoaded);

	this->Unloaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::UserProfiles::OnUnloaded);

}


void scwin::UserProfiles::OnDataContextChanged(Windows::UI::Xaml::FrameworkElement ^sender, Windows::UI::Xaml::DataContextChangedEventArgs ^args)
{
	if (args->NewValue != nullptr)
	{
		if (auto vm = dynamic_cast<UserViewModel^>(args->NewValue))
		{
			viewPropChangedToken = this->ViewModel->PropertyChanged += ref new Windows::UI::Xaml::Data::PropertyChangedEventHandler(this, &scwin::UserProfiles::OnPropertyChanged);
		}
	}
}


void scwin::UserProfiles::OnPropertyChanged(Platform::Object ^sender, Windows::UI::Xaml::Data::PropertyChangedEventArgs ^e)
{
}



//void scwin::UserProfiles::listOfUsers_ItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
//{
//	auto element = (ListViewItem^)listOfUsers->ContainerFromItem(e->ClickedItem);
//	auto rootelmnt = element->ContentTemplateRoot;
//	OutputDebugString(rootelmnt->GetType()->FullName->Data());
//	OutputDebugString(L"\n");
//	if (lastTapped)
//	{
//		VisualStateManager::GoToState(lastTapped, L"WhenNotSelected", false);
//	}
//	lastTapped = (Control^)rootelmnt;
//	VisualStateManager::GoToState(lastTapped, L"WhenSelected", false);
//}


void scwin::UserProfiles::OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	Array<PubSubMessageIds>^ messages = { PubSubMessageIds::SearchMessage };
	PubSubService::Instance->Subscribe(this, ref new ReceivePubSubMessageCallback(this, &UserProfiles::OnPubsubMessage), messages);

	//txtSearchUsers->Text = SEARCHPROMPT;
}


//void scwin::UserProfiles::txtSearchUsers_GotFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
//{
//	if (txtSearchUsers->Text == SEARCHPROMPT)
//	{
//		txtSearchUsers->Text = L""; 
//	}
//	else
//	{
//		txtSearchUsers->SelectAll();
//	}
//}
//
//
//void scwin::UserProfiles::txtSearchUsers_LostFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
//{
//	if (StringHelpers::IsNullOrEmpty(txtSearchUsers->Text))
//	{
//		txtSearchUsers->Text = SEARCHPROMPT;
//	}
//}


void scwin::UserProfiles::OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	PubSubService::Instance->Unsubscribe(this);
}

void scwin::UserProfiles::OnPubsubMessage(Object ^ sender, PubSubMessageIds msgId, Object ^ data)
{
	WeakReference wr(this);
	Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([wr, data]()
	{
		auto ths = wr.Resolve<UserProfiles>();
		auto searchInfo = dynamic_cast<SearchData^>(data);
		if (ths->Visibility == Windows::UI::Xaml::Visibility::Visible && !StringHelpers::IsNullOrEmpty(searchInfo->SearchText))
		{
			if (searchInfo != nullptr)
			{
				ths->ViewModel->SearchText = searchInfo->SearchText;
			}
		}
		else
		{
			ths->ViewModel->SearchText = L"";
		}
	}));
}


void scwin::UserProfiles::splitPane_PaneClosing(Windows::UI::Xaml::Controls::SplitView^ sender, Windows::UI::Xaml::Controls::SplitViewPaneClosingEventArgs^ args)
{
	directResetPwdReq = false;
	GOTOVIEWSTATE(this, "UserPwdResetCollapsedViewState");
}


void scwin::UserProfiles::listOfUsers_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	ViewModel->SelectedIndex = listOfUsers->SelectedIndex;
}


void scwin::UserProfiles::hlinkUserName_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	GOTOVIEWSTATE(this, "UserPwdResetCollapsedViewState");
	auto data = ((HyperlinkButton^)sender)->DataContext;
	if (data != nullptr)
	{
		auto lv = (ListViewItem^)listOfUsers->ContainerFromItem(data);
		if (lv != nullptr)
		{
			lv->IsSelected = true;
		}

		if (!splitPane->IsPaneOpen)
			splitPane->IsPaneOpen = true;

		ViewModel->EditedUserProfile = ViewModel->SelectedProfile->Clone();
		ViewModel->EditedUserProfile->ObjectState = DataState::Edited;
		GOTOVIEWSTATE(this, "EditingUserState");
	}
}


void scwin::UserProfiles::btnUserProfilePaneClose_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (UserPwdResetView->Visibility == Windows::UI::Xaml::Visibility::Visible)
	{
		GOTOVIEWSTATE(this, "UserPwdResetCollapsedViewState");
		if (directResetPwdReq)
		{
			ViewModel->EditedUserProfile = nullptr;
			ViewModel->IsPaneOpen = false;
			directResetPwdReq = false;
		}
	}
	else 
	{
		ViewModel->EditedUserProfile = nullptr;
		ViewModel->IsPaneOpen = false;
	}
}


void scwin::UserProfiles::cmbRolesFilter_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	using namespace cpplinq;
	if (e->AddedItems->Size > 0)
	{
		auto selectedItem = (UserRole^)e->AddedItems->First()->Current;
		if (selectedItem != nullptr)
		{
			auto isallroles = selectedItem->Id == "allroles";
			// run through the list view items and hide and show depending on the roles
			auto users = ViewModel->Users;
			if (users != nullptr)
			{
				for (auto i = 0; i < users->Size; i++)
				{
					auto container = (ListViewItem^)listOfUsers->ContainerFromIndex(i);
					if (container == nullptr)
						break;

					if (isallroles)
					{
						container->Visibility = Windows::UI::Xaml::Visibility::Visible;
					}
					else
					{
						auto userMemberOf = from(to_vector(users->GetAt(i)->Roles)) >> any([selectedItem](String^ roleId)
						{
							return selectedItem->Id == roleId;
						});
						container->Visibility = userMemberOf ? Windows::UI::Xaml::Visibility::Visible : Windows::UI::Xaml::Visibility::Collapsed;
					}
				}
			}
		}
	}
}


void scwin::UserProfiles::btnUserProfileEditCancel_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	ViewModel->EditedUserProfile = nullptr;
	ViewModel->IsPaneOpen = false;
}


void scwin::UserProfiles::btnAddUserProfile_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	//GOTOVIEWSTATE(this, "UserPwdResetCollapsedViewState");
	GOTOVIEWSTATE(this, "AddingUserState");
}


void scwin::UserProfiles::btnkResetPassword_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	GOTOVIEWSTATE(this, "UserPwdResetVisibleViewState");
}


void scwin::UserProfiles::appbtnResetPassword_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	directResetPwdReq = true;
	//GOTOVIEWSTATE(this, "UserPwdResetCollapsedViewState");
	if (ViewModel->SelectedProfile != nullptr)
	{
		if (!splitPane->IsPaneOpen)
			splitPane->IsPaneOpen = true;
		ViewModel->EditedUserProfile = ViewModel->SelectedProfile->Clone();
		ViewModel->EditedUserProfile->ObjectState = DataState::Edited;
		GOTOVIEWSTATE(this, "EditingUserState");
		GOTOVIEWSTATE(this, "UserPwdResetVisibleViewState");
	}
}


void scwin::UserProfiles::btnInviteNewUser_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

}
