#include "pch.h"
#include "AdminViewModel.h"
#include "scLocalDataService.h"
#include "UserServices.h"
#include <assert.h>
#include "cpplinq.hpp"
#include "AdminDataBuilder.h"

using namespace scwin::services;
using namespace Windows::UI::Popups;

String^ USERPROFILEVIEWTYPE = "UserProfileViewType";

scwin::AdminViewModel::AdminViewModel(scMainViewModel ^ parentModel)
{
	this->parentVm = parentModel;
	// TODO: The user and role models must lazily initialize data (see constructors of those classes. move the constructor initializations to a inititiaze method.
	userVm = ref new UserViewModel();
	rolesVm = ref new RoleViewModel();

}

void scwin::AdminViewModel::InvokePropertyChanged(String ^ propName)
{
	PropertyChanged(this, ref new PropertyChangedEventArgs(propName));
}

UserViewModel^ AdminViewModel::UsersViewModel::get()
{
	return userVm;
}

RoleViewModel^ AdminViewModel::RolesViewModel::get()
{
	return rolesVm;
}


//////////////////////



UserViewModel::UserViewModel()
{
	Initialize();
}

void scwin::UserViewModel::Initialize()
{
	AddCommand = ref new Command(ref new ExecuteAction(this, &UserViewModel::ExecuteAdd), ref new CanExecuteFunc(this, &UserViewModel::CanExecuteAdd));
	
	EditCommand = ref new Command(ref new ExecuteAction(this, &UserViewModel::ExecuteEdit), ref new CanExecuteFunc(this, &UserViewModel::CanExecuteEdit));
	DeleteCommand = ref new Command(ref new ExecuteAction(this, &UserViewModel::ExecuteDelete), ref new CanExecuteFunc(this, &UserViewModel::CanExecuteDelete));
	SaveCommand = ref new Command(ref new ExecuteAction(this, &UserViewModel::ExecuteSave), ref new CanExecuteFunc(this, &UserViewModel::CanExecuteSave));
	CancelCommand = ref new Command(ref new ExecuteAction(this, &UserViewModel::ExecuteCancel), ref new CanExecuteFunc(this, &UserViewModel::CanExecuteCancel));
	ImportCommand = ref new Command(ref new ExecuteAction(this, &UserViewModel::ExecImportCommand), ref new CanExecuteFunc(this, &UserViewModel::CanExecImportCommand));
	RefreshCommand = ref new Command(ref new ExecuteAction(this, &UserViewModel::ExecRefreshCommand), ref new CanExecuteFunc(this, &UserViewModel::CanExecRefreshCommand));
	ResetPwdCommand = ref new Command(ref new ExecuteAction(this, &UserViewModel::ExecResetPwdCommand), ref new CanExecuteFunc(this, &UserViewModel::CanExecResetPwdCommand));

	LoadData();
}

void scwin::UserViewModel::OnRoleFilterChanged()
{
	auto roleId = comboSelectedRole != nullptr ? comboSelectedRole->Id : "allroles";
	auto prop = ref new AppProperty("UserProfile_LastSelectedRoleFilter", roleId, true);
	scLocalDataService::Instance()->SetProperty(prop);
}

void scwin::UserViewModel::RefreshCommands()
{
	SaveCommand->Refresh();
	CancelCommand->Refresh();
}

void UserViewModel::LoadData()
{
	auto service = scLocalDataService::Instance();
	WeakReference wr(this);
	task<IVector<UserRole^>^> getRoles = create_task(service->ListRoles());
	getRoles.then([wr](IVector<UserRole^>^ roles)
	{
		auto ths = wr.Resolve<UserViewModel>();
		ths->AllRoles = (IObservableVector<UserRole^>^)roles;
		ths->RolesForCombo = ref new Vector<UserRole^>(to_vector(roles));
		auto ur = ref new UserRole("allroles");
		ur->Name = "All Roles";
		ths->RolesForCombo->InsertAt(0, ur);
		ths->RolesComboSelectedItem = ur;
	});

	task<IUserProfileList^> getUsers = create_task(service->ListUsers());
	getUsers.then([wr](IUserProfileList^ users)
	{
		auto ths = wr.Resolve<UserViewModel>();
		ths->Users = users;
		ths->allusers = users;
		AdminDataBuilder::roles = nullptr;
	});
}

void scwin::UserViewModel::IsPaneOpen::set(bool val)
{
	if (ispaneOpen != val)
	{
		ispaneOpen = val;
		RaisePropertyChanged(PROPERTYNAME(IsPaneOpen));
		RefreshCommands();
	}
}

void scwin::UserViewModel::UpdateSelectedRolesOfSelectedUser()
{
	if (selectedUserRoles != nullptr)
		this->selectedUserRoles->Clear();
	this->selectedUserRoles = nullptr;

	auto temp = ref new Vector<UserRoleSelection^>();
	// loop all roles and then loop user's roles and then check
	// selected items.
	if (roles != nullptr)
	{
		for (auto r : roles)
		{
			auto userRoleSelection = ref new UserRoleSelection(r);
			UINT index;
			if (EditedUserProfile->Roles->IndexOf(r->Id, &index))
			{
				userRoleSelection->IsSelected = true;
			}
			temp->Append(userRoleSelection);
		}
	}
	SelectedUserRoles = temp;
}

void scwin::UserViewModel::ExecuteAdd(Object ^ data)
{
	this->EditedUserProfile = ref new UserProfile();
	IsPaneOpen = true;
	RefreshCommands();
}

bool scwin::UserViewModel::CanExecuteAdd(Object ^ data)
{
	return !this->ispaneOpen;
}

void scwin::UserViewModel::ExecuteEdit(Object ^ data)
{
	RefreshCommands();
}

bool scwin::UserViewModel::CanExecuteEdit(Object ^ data)
{
	return !this->ispaneOpen;
}

void scwin::UserViewModel::ExecuteDelete(Object ^ data)
{
	SysInfoHelpers::ShowNotImplementedMessageDialog("Delete User");
}

bool scwin::UserViewModel::CanExecuteDelete(Object ^ data)
{
	return !this->ispaneOpen;
}

void scwin::UserViewModel::ExecuteSave(Object ^ data)
{
	if (SysInfoHelpers::ShowNetworkErrorMessageDialog())
	{

		// verify email, full name and if atleast one role is selected.
		InvalidMessage = L"";
		EditedUserProfile->Roles->Clear();
		for (auto s : this->SelectedUserRoles)
		{
			if (s->IsSelected->Value)
			{
				EditedUserProfile->Roles->Append(s->Role->Id);
			}
		}

		if (EditedUserProfile->ObjectState == DataState::Added)
		{
			if (EditedUserProfile->Email != EmailConfirm)
			{
				PubSubService::Instance->Publish(this, PubSubMessageIds::ApplicationErrorStatusMessage, "Email address entered does match with confirmation email.");
				return;
			}
		}

		if (!EditedUserProfile->IsValid)
		{
			PubSubService::Instance->Publish(this, PubSubMessageIds::ApplicationErrorStatusMessage, "User Profile is incomplete. Not all required information provided.");
			return;
		}

		auto service = scLocalDataService::Instance();
		WeakReference wr(this);
		service->UpsertUser(EditedUserProfile, StringHelpers::IsNullOrEmpty(UserPwd) ? SysInfoHelpers::NewCouchDbId() : UserPwd)
			->Completed = ref new AsyncOperationCompletedHandler<bool>([wr](IAsyncOperation<bool>^ operation, AsyncStatus status)
		{
			auto ths = wr.Resolve<UserViewModel>();
			if (status == AsyncStatus::Completed)
			{
				ths->EditedUserProfile->UpdateRoleStr();
				if (ths->EditedUserProfile->ObjectState == DataState::Added)
				{
					ths->Users->Append(ths->EditedUserProfile);
				}
				else
				{
					ths->Users->SetAt(ths->SelectedIndex, ths->EditedUserProfile);
				}
				ths->EditedUserProfile = nullptr;
				ths->IsPaneOpen = false;
				AdminDataBuilder::roles = nullptr;
			}
			else
			{
				ths->InvalidMessage = ref new String(GetMessage(MessageIds::SaveFailed));
			}
		});
	}
}

bool scwin::UserViewModel::CanExecuteSave(Object ^ data)
{
	return StringHelpers::IsNullOrEmpty(invalidMesage);
}

void scwin::UserViewModel::ExecuteCancel(Object ^ data)
{
	// restore
}

bool scwin::UserViewModel::CanExecuteCancel(Object ^ data)
{
	return true;
}

void scwin::UserViewModel::ExecRefreshCommand(Object ^ data)
{
	if (SysInfoHelpers::ShowNetworkErrorMessageDialog())
	{
		LoadData();
	}
}

bool scwin::UserViewModel::CanExecRefreshCommand(Object ^ data)
{
	return !this->ispaneOpen;
}

void scwin::UserViewModel::ExecImportCommand(Object ^ data)
{
	SysInfoHelpers::ShowNotImplementedMessageDialog("Import Users");
}

bool scwin::UserViewModel::CanExecImportCommand(Object ^ data)
{
	return !this->ispaneOpen;
}

void scwin::UserViewModel::ExecResetPwdCommand(Object ^ data)
{
	if (SysInfoHelpers::ShowNetworkErrorMessageDialog())
	{
		auto cursor = Window::Current->CoreWindow->PointerCursor;
		Window::Current->CoreWindow->PointerCursor =
			ref new Windows::UI::Core::CoreCursor(Windows::UI::Core::CoreCursorType::Wait, 1);
		scLocalDataService::Instance()->ResetUserPassword(SelectedProfile->Id)
			.then([=](MessageIds status)
		{
			AppDispatcher->RunAsync(
				CoreDispatcherPriority::Normal,
				ref new DispatchedHandler([=]() {
				Window::Current->CoreWindow->PointerCursor = cursor;
			})
			);
			if (HASFAILED(status))
			{
				PubSubService::Instance->Publish(
					this, 
					PubSubMessageIds::ApplicationErrorStatusMessage, 
					ref new String(GetMessage(status)));
			}
			else
			{
				PubSubService::Instance->Publish(this, PubSubMessageIds::ApplicationInfoStatusMessage, "Reset Password message is emailed to the user.");
				IsPaneOpen = false;
			}
		});
	}
}

bool scwin::UserViewModel::CanExecResetPwdCommand(Object ^ data)
{
	return true;
}

void scwin::UserViewModel::UserProfileOnPropertyChanged(Platform::Object ^sender, Windows::UI::Xaml::Data::PropertyChangedEventArgs ^e)
{
	if (e->PropertyName == L"Email")
	{
		// check email address
		if (!StringHelpers::IsValidEmail(EditedUserProfile->Email))
		{
			this->InvalidMessage = ref new String(GetMessage(MessageIds::EmailAddressNotValid));
			RaisePropertyChanged(PROPERTYNAME(IsInvalidEmail));
			RefreshCommands();
			return;
		}
	}
	if (e->PropertyName == L"FullName")
	{
		if (StringHelpers::IsNullOrEmpty(EditedUserProfile->FullName))
		{
			this->InvalidMessage = ref new String(GetMessage(MessageIds::UserNameNotProvided));
			RaisePropertyChanged(PROPERTYNAME(IsInvalidFullName));
			RefreshCommands();
			return;
		}
	}
	this->InvalidMessage = L"";
	RaisePropertyChanged(PROPERTYNAME(IsValidState));
	RefreshCommands();
}

void scwin::UserViewModel::SearchText::set(String^ val)
{
	if (val != searchText)
	{
		searchText = val;
		if (val != SEARCHPROMPT)
		{
			ApplySearch(searchText);
		}
		else if (!StringHelpers::IsNullOrEmpty(val))
		{
			RaisePropertyChanged("SearchText");
		}
	}
	OutputDebugString(L"SearchText::set.\n");
}

void scwin::UserViewModel::ApplySearch(String ^ text)
{
	OutputDebugString(L"ApplySearch");
	if (StringHelpers::IsNullOrEmpty(text) || text == SEARCHPROMPT)
	{
		this->Users = allusers;
		return;
	}

	if (allusers != nullptr)
	{
		this->Users = ref new Vector<UserProfile^>();
		for (auto u : allusers)
		{
			if (StringHelpers::Match(u->Email, text) || StringHelpers::Match(u->FullName, text))
			{
				filteredusers->Append(u);
			}
		}
	}
}

DEFINERAISEPROPCHANGEDBASIC(UserViewModel)


//////////////////////////////FunctionalRightsSelection//////////////////////////////////////////

bool FunctionalRightsSelection::Selected::get()
{
	return selected;
}

void FunctionalRightsSelection::Selected::set(bool val)
{
	if (selected != val)
	{
		selected = val;
		RaisePropertyChanged(PROPERTYNAME(Selected));
	}
}

DEFINERAISEPROPCHANGEDBASIC(FunctionalRightsSelection)

////////////////


RoleViewModel::RoleViewModel()
{
	filteredListChangedToken.Value = 0L;
	searchText = SEARCHPROMPT;

	AddCommand = ref new Command(ref new ExecuteAction(this, &RoleViewModel::ExecAddCommand), ref new CanExecuteFunc(this, &RoleViewModel::CanExecAddCommand));
	SaveCommand = ref new Command(ref new ExecuteAction(this, &RoleViewModel::ExecSaveCommand), ref new CanExecuteFunc(this, &RoleViewModel::CanExecSaveCommand));
	DeleteCommand = ref new Command(ref new ExecuteAction(this, &RoleViewModel::ExecDelCommand), ref new CanExecuteFunc(this, &RoleViewModel::CanExecDelCommand));
	RefreshCommand = ref new Command(ref new ExecuteAction(this, &RoleViewModel::ExecRefreshCommand), ref new CanExecuteFunc(this, &RoleViewModel::CanExecRefreshCommand));
	ImportCommand = ref new Command(ref new ExecuteAction(this, &RoleViewModel::ExecImportCommand), ref new CanExecuteFunc(this, &RoleViewModel::CanExecImportCommand));

	LoadData();
}

void scwin::RoleViewModel::RefreshCommands()
{
	AddCommand->Refresh();
	SaveCommand->Refresh();
	DeleteCommand->Refresh();
	RefreshCommand->Refresh();
	ImportCommand->Refresh();
}

void scwin::RoleViewModel::IsPaneOpen::set(bool val)
{
	if (ispaneOpen != val)
	{
		ispaneOpen = val;
		RaisePropertyChanged(PROPERTYNAME(IsPaneOpen));
		RefreshCommands();
	}
}

void scwin::RoleViewModel::SearchText::set(String^ val)
{
	if (val != searchText)
	{
		searchText = val;
		if (val != SEARCHPROMPT)
		{
			ApplySearch(searchText);
		}
		else if (!StringHelpers::IsNullOrEmpty(val))
		{
			RaisePropertyChanged("SearchText");
		}
	}
	OutputDebugString(L"SearchText::set.\n");
}


void scwin::RoleViewModel::LoadData()
{
	auto cursor = Window::Current->CoreWindow->PointerCursor;
	Window::Current->CoreWindow->PointerCursor =
		ref new Windows::UI::Core::CoreCursor(Windows::UI::Core::CoreCursorType::Wait, 1);
	auto service = scLocalDataService::Instance();
	WeakReference wr(this);
	task<IVector<UserRole^>^> getRoles = create_task(service->ListRoles());
	getRoles.then([wr, cursor](IVector<UserRole^>^ roles)
	{
		wr.Resolve<RoleViewModel>()->FilteredList = roles;
		Window::Current->Dispatcher->RunAsync(
			CoreDispatcherPriority::Normal,
			ref new DispatchedHandler([=]() {
			Window::Current->CoreWindow->PointerCursor = cursor;
		})
		);
	});
}

void scwin::RoleViewModel::FilteredListChanged(IUserRoleList ^ sender, IVectorChangedEventArgs ^ eargs)
{

}

void scwin::RoleViewModel::SelectedRole::set(UserRole^ val)
{
	if (val != selectedRole)
	{
		selectedRole = val;
		RaisePropertyChanged("SelectedRole");
		UpdateFunctionRights();
	}
}

void scwin::RoleViewModel::EditedRole::set(UserRole^ val)
{
	if (editedRole != val)
	{
		editedRole = val;
		RaisePropertyChanged(PROPERTYNAME(EditedRole));
		UpdateFunctionRights();
	}
}

void scwin::RoleViewModel::DeleteMessageInvoker(Windows::UI::Popups::IUICommand ^ command)
{
	//if (command->Label == "Yes")
	//{
	//}
	//else if (command->Label == "No")
	//{

	//}
	auto dlg = ref new MessageDialog(L"Not implemented yet.", "Delete User Role");
	dlg->ShowAsync();
}

void scwin::RoleViewModel::ApplySearch(String ^ text)
{
	OutputDebugString(L"ApplySearch");
	if (StringHelpers::IsNullOrEmpty(text) || text == SEARCHPROMPT)
	{
		this->FilteredList = roles;
		return;
	}
	if (roles != nullptr)
	{
		this->FilteredList = ref new Vector<UserRole^>();
		for (auto ur : roles)
		{
			if (StringHelpers::Match(ur->Name, text))
			{
				filteredList->Append(ur);
			}
		}
	}
}

void scwin::RoleViewModel::ExecAddCommand(Object ^ data)
{
	if (!scwin::utils::SysInfoHelpers::ShowNetworkErrorMessageDialog())
	{
		return;
	}
	EditedRole = ref new UserRole();
	UpdateFunctionRights();
	IsPaneOpen = true;
}

bool scwin::RoleViewModel::CanExecAddCommand(Object ^ data)
{

	if (!IsPaneOpen)
		return false;

	return true;

}

void scwin::RoleViewModel::ExecSaveCommand(Object ^ data)
{
	if (!scwin::utils::SysInfoHelpers::ShowNetworkErrorMessageDialog())
	{
		return;
	}

	auto ur = EditedRole;
	ur->Rights = FunctionalRights::NoRights;
	for (auto fr : ListRights)
	{
		if (fr->Selected)
			ur->Rights |= fr->Right;
	}
	if (ur)
	{
		if (ur->IsEmpty)
		{
			PubSubService::Instance->Publish(this, PubSubMessageIds::ApplicationErrorStatusMessage, "Not all required information is provided. Role Name and atleast one right must be provided.");
			return;
		}
		WeakReference wr(this);
		scLocalDataService::Instance()->UpsertRole(ur)
			->Completed = ref new AsyncOperationCompletedHandler<bool>([wr, ur](IAsyncOperation<bool>^ operation, AsyncStatus status)
		{
			auto ths = wr.Resolve<RoleViewModel>();
			if (status == AsyncStatus::Error)
			{
				auto dlg = ref new MessageDialog("Unfortunately due to an issue at the server, the user role was not saved. Please retry or contact application administrator.", "Save Error");
				dlg->ShowAsync();
			}
			else
			{
				if (ur->ObjectState == DataState::Added)
				{
					ths->FilteredList->Append(ur);
				}
				else
				{
					// replace
					ths->FilteredList->SetAt(ths->SelectedIndex, ur);
				}
				ur->ObjectState = DataState::Unmodified;
				ths->IsPaneOpen = false;
				ths->RefreshCommands();
			}
		});
	}
}

bool scwin::RoleViewModel::CanExecSaveCommand(Object ^ data)
{
	return IsPaneOpen;
}

void scwin::RoleViewModel::ExecDelCommand(Object ^ data)
{
	/*
	MessageDialog^ dlg = ref new MessageDialog(L"The role will be deleted if not assigned to a user. Continue ?", "Delete User Role");
	dlg->Commands->Append(ref new UICommand("Yes", ref new UICommandInvokedHandler([this](IUICommand^ cmd)
	{
		using namespace cpplinq;
		auto vdel = from(to_vector(filteredList))
			>> where([](UserRole^ role) {
			return role->IsSelected;
		})
			>> to_vector();
		auto list = ref new Vector<UserRole^>(vdel);
		//send it to server
		scLocalDataService::Instance()->DeleteRoles(list);
	})));
	dlg->Commands->Append(ref new UICommand("No"));
	dlg->DefaultCommandIndex = 1;
	dlg->ShowAsync();
	*/
	SysInfoHelpers::ShowNotImplementedMessageDialog("Delete Role");
}

bool scwin::RoleViewModel::CanExecDelCommand(Object ^ data)
{
	return !IsPaneOpen;
}

void scwin::RoleViewModel::ExecRefreshCommand(Object ^ data)
{
	LoadData();
}

bool scwin::RoleViewModel::CanExecRefreshCommand(Object ^ data)
{
	return !IsPaneOpen;
}

void scwin::RoleViewModel::ExecImportCommand(Object ^ data)
{
	SysInfoHelpers::ShowNotImplementedMessageDialog("Import Roles");
}

bool scwin::RoleViewModel::CanExecImportCommand(Object ^ data)
{
	return !IsPaneOpen;
}

void scwin::RoleViewModel::UpdateFunctionRights()
{
	auto role = EditedRole;
	if (role != nullptr)
	{
		WeakReference wr(this);
		create_task([wr, role]()
		{
			Vector<FunctionalRightsSelection^>^ selections =
				ref new Vector<FunctionalRightsSelection^>();
			for (auto rightsDesc : FunctionalRightsDescriptions)
			{
				auto sel = ref new FunctionalRightsSelection();
				sel->Right = rightsDesc.first;
				sel->RightName = StringHelpers::ConvertToString(rightsDesc.second.Name.c_str());
				sel->RightDesc = StringHelpers::ConvertToString(rightsDesc.second.Description.c_str());
				uint64 rightflag = rightsDesc.first;
				sel->Selected = rightflag == (rightflag & role->Rights);
				selections->Append(sel);
			}
			wr.Resolve<RoleViewModel>()->ListRights = selections;
		});
	}
}

IObservableVector<FunctionalRightsSelection^>^ scwin::RoleViewModel::ListRights::get()
{
	return listRights;
}

void scwin::RoleViewModel::ListRights::set(IObservableVector<FunctionalRightsSelection^>^ val)
{
	if (listRights != val)
	{
		listRights = val;
		RaisePropertyChanged(PROPERTYNAME(ListRights));
	}
}

DEFINERAISEPROPCHANGEDBASIC(RoleViewModel)

scwin::UserRoleSelection::UserRoleSelection(UserRole ^ role) : role(role)
{
}
