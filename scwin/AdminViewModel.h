#pragma once
#include "scMainViewModel.h"
#include "ProductViewModel.h"

namespace scwin
{

	public ref class UserRoleSelection sealed : INotifyPropertyChanged
	{
	private:
		UserRole^ role;
		bool checked = false;

	public:

		UserRoleSelection(UserRole^ role);

		property UserRole^ Role
		{
			UserRole^ get(){ return role; }
		}

		property IBox<bool>^ IsSelected
		{
			IBox<bool>^ get() 
			{ 
				return ref new Box<bool>(checked);
			}

			void set(IBox<bool>^ val)
			{ 
				if (val)
				{
					checked = val->Value;
				}
				else
				{
					checked = false;
				}
				PropertyChanged(this, ref new PropertyChangedEventArgs(L"IsSelected"));
			}
		}

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;
	};

	typedef IObservableVector<UserRoleSelection^> ISelectedUserRoles;
	typedef IObservableVector<UserProfile^> UserList;

	[Bindable]
	public ref class UserViewModel sealed : public INotifyPropertyChanged
	{
	private:
		UserList^ allusers;
		UserList^ filteredusers;
		bool isEditState = false;
		IObservableVector<UserRole^>^ roles;
		IObservableVector<UserRole^>^ rolesForDropdown;
		ISelectedUserRoles^ selectedUserRoles;
		String^ invalidMesage;
		String^ searchText;
		bool ispaneOpen;
		int selectedIndex;
		UserRole^ comboSelectedRole;
		UserProfile^ profileBeingEdited;
		String^ emailConfirm;
		UserProfile^ selected;

	public:

		UserViewModel();

		void Initialize();

		property Command^ AddCommand;
		property Command^ EditCommand;
		property Command^ DeleteCommand;
		property Command^ SaveCommand;
		property Command^ CancelCommand;

		property Command^ ImportCommand;
		property Command^ RefreshCommand;
		property Command^ ResetPwdCommand;

		property String^ UserPwd;

		property IObservableVector<UserProfile^>^ Users
		{
		public:
			IObservableVector<UserProfile^>^ get() 
			{ 
				return this->filteredusers; 
			}

		private:
			void set(IObservableVector<UserProfile^>^ val)
			{
				if (filteredusers != val)
				{
					filteredusers = val;
					RaisePropertyChanged(PROPERTYNAME(Users));
				}
			}
		}

		property bool IsPaneOpen
		{
			bool get() { return ispaneOpen; }
			void set(bool val);
		}

		property int SelectedIndex
		{
			int get()
			{
				return selectedIndex;
			}
			void set(int val)
			{
				selectedIndex = val;
			}
		}

		property String^ EmailConfirm
		{
			String^ get() { return emailConfirm; }
			void set(String^ val)
			{
				emailConfirm = val;
				RaisePropertyChanged(PROPERTYNAME(EmailConfirm));
			}
		}

		property UserProfile^ SelectedProfile
		{
			UserProfile^ get() 
			{
				return selected;
			}
			void set(UserProfile^ val)
			{
				if (selected != val)
				{
					selected = val;
					RaisePropertyChanged(PROPERTYNAME(SelectedProfile));
				}
			}
		}

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		property bool IsInvalidFullName
		{
			bool get()
			{
				return !StringHelpers::IsNullOrEmpty(this->invalidMesage);
			}
		}
		property bool IsInvalidPwd
		{
			bool get()
			{
				return !StringHelpers::IsNullOrEmpty(this->invalidMesage);
			}
		}
		property bool IsInvalidRole
		{
			bool get()
			{
				return !StringHelpers::IsNullOrEmpty(this->invalidMesage);
			}
		}

		property bool IsValidState
		{
			bool get() { return StringHelpers::IsNullOrEmpty(invalidMesage); }
		}

		property String^ SearchText
		{
			String^ get() { return searchText; }
			void set(String^ val);
		}

		property IObservableVector<UserRole^>^ AllRoles
		{
			IObservableVector<UserRole^>^ get() { return roles; }
		private:
			void set(IObservableVector<UserRole^>^ val)
			{
				if (roles != val)
				{
					roles = val;
					RaisePropertyChanged(PROPERTYNAME(AllRoles));
				}
			}
		}

		property IObservableVector<UserRole^>^ RolesForCombo
		{
			IObservableVector<UserRole^>^ get() { return rolesForDropdown; }
		private:
			void set(IObservableVector<UserRole^>^ val)
			{
				if (rolesForDropdown != val)
				{
					rolesForDropdown = val;
					RaisePropertyChanged(PROPERTYNAME(RolesForCombo));
				}
			}
		}

		property UserRole^ RolesComboSelectedItem
		{
			UserRole^ get() { return comboSelectedRole; }
			void set(UserRole^ val)
			{
				if (comboSelectedRole != val)
				{
					comboSelectedRole = val;
					RaisePropertyChanged(PROPERTYNAME(RolesComboSelectedItem));
					OnRoleFilterChanged();
				}
			}
		}

		property ISelectedUserRoles^ SelectedUserRoles
		{
		public:
			ISelectedUserRoles^ get()
			{
				return this->selectedUserRoles;
			}
			void set(ISelectedUserRoles^ val)
			{
				if (selectedUserRoles != val)
				{
					selectedUserRoles = val;
					RaisePropertyChanged(PROPERTYNAME(SelectedUserRoles));
				}
			}
		}

		property String^ InvalidMessage
		{
			String^ get() { return invalidMesage; }
			void set(String^ val)
			{
				if (val != invalidMesage)
				{
					invalidMesage = val;
					RaisePropertyChanged(PROPERTYNAME(InvalidMessage));
				}
			}
		}

		property UserProfile^ EditedUserProfile
		{
			UserProfile^ get() { return profileBeingEdited; }
			void set(UserProfile^ val)
			{
				if (profileBeingEdited != val)
				{
					profileBeingEdited = val;
					RaisePropertyChanged(PROPERTYNAME(EditedUserProfile));
					if (val != nullptr)
						UpdateSelectedRolesOfSelectedUser();
				}
			}
		}

	private:
		
		DECLARERAISEPROPCHANGED;

		void OnRoleFilterChanged();

		void RefreshCommands();

		void LoadData();

		void ApplySearch(String^ text);

		void UpdateSelectedRolesOfSelectedUser();

		void ExecuteAdd(Object^ data);
		bool CanExecuteAdd(Object^ data);

		void ExecuteEdit(Object^ data);
		bool CanExecuteEdit(Object^ data);

		void ExecuteDelete(Object^ data);
		bool CanExecuteDelete(Object^ data);

		void ExecuteSave(Object^ data);
		bool CanExecuteSave(Object^ data);

		void ExecuteCancel(Object^ data);
		bool CanExecuteCancel(Object^ data);
	
		void ExecRefreshCommand(Object^ data);
		bool CanExecRefreshCommand(Object^ data);

		void ExecImportCommand(Object^ data);
		bool CanExecImportCommand(Object^ data);

		void ExecResetPwdCommand(Object^ data);
		bool CanExecResetPwdCommand(Object^ data);

		void UserProfileOnPropertyChanged(Platform::Object ^sender, Windows::UI::Xaml::Data::PropertyChangedEventArgs ^e);
};

	typedef IObservableVector<UserRole^> IUserRoleList;

	public ref class FunctionalRightsSelection sealed : public INotifyPropertyChanged
	{
		bool selected = false;
	public:
		property String ^ RightName;
		property String^ RightDesc;
		property uint64 Right;

		property bool Selected
		{
			bool get();
			void set(bool);
		}

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		DECLARERAISEPROPCHANGED;
	};


	[Bindable]
	public ref class RoleViewModel sealed : public INotifyPropertyChanged
	{
	private:
		IVector<UserRole^>^ roles;
		IVector<UserRole^>^ filteredList;
		EventRegistrationToken filteredListChangedToken;
		String^ searchText;
		UserRole^ selectedRole;
		UserRole^ editedRole;
		IObservableVector<FunctionalRightsSelection^>^ listRights;
		bool ispaneOpen;
		//bool canExecCommands = true;
		int selectedIndex;


	public:

		RoleViewModel();

	private:

		void LoadData();

		void FilteredListChanged(IUserRoleList^ sender, IVectorChangedEventArgs^ eargs);
		void DeleteMessageInvoker(Windows::UI::Popups::IUICommand^ command);

		void ExecAddCommand(Object^ data);
		bool CanExecAddCommand(Object^ data);
		
		void ExecSaveCommand(Object^ data);
		bool CanExecSaveCommand(Object^ data);
		
		void ExecDelCommand(Object^ data);
		bool CanExecDelCommand(Object^ data);

		void ExecRefreshCommand(Object^ data);
		bool CanExecRefreshCommand(Object^ data);

		void ExecImportCommand(Object^ data);
		bool CanExecImportCommand(Object^ data);

		void UpdateFunctionRights();

		void RefreshCommands();

		DECLARERAISEPROPCHANGED;

	public:

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		void ApplySearch(String^ text);

		property bool IsPaneOpen
		{
			bool get() { return ispaneOpen; }
			void set(bool val);
		}

		property int SelectedIndex 
		{ 
			int get() 
			{ 
				return selectedIndex; 
			} 
			void set(int val) 
			{ 
				selectedIndex = val; 
			}
		}

		property IVector<UserRole^>^ Roles
		{
		public:
			IVector<UserRole^>^ get()
			{
				if (roles == nullptr)
				{
					GUARDEDGLOCK;
					if (roles == nullptr)
					{
						roles = ref new Vector<UserRole^>();
					}
				}
				return this->roles;
			}

		private:
			void set(IVector<UserRole^>^ val)
			{
				if (roles != val)
				{
					roles = val;
					RaisePropertyChanged("Roles");
				}
			}
		}

		property IVector<UserRole^>^ FilteredList
		{
			IVector<UserRole^>^ get() { return this->filteredList; }
			void set(IVector<UserRole^>^ list)
			{
				if (filteredList != list)
				{
					if (filteredList != nullptr)
					{
						if (filteredListChangedToken.Value != 0L)
						{
							auto ov = dynamic_cast<Vector<UserRole^>^>(filteredList);
							if (ov != nullptr)
								ov->VectorChanged -= filteredListChangedToken;
						}
					}
					filteredList = list;
					if (filteredList != nullptr)
					{
						auto ov = dynamic_cast<Vector<UserRole^>^>(filteredList);
						if (ov != nullptr)
							filteredListChangedToken = ov->VectorChanged += ref new VectorChangedEventHandler<UserRole^>(this, &RoleViewModel::FilteredListChanged);
					}
					RaisePropertyChanged("FilteredList");
				}
			}
		}

		property String^ SearchText
		{
			String^ get() { return searchText; }
			void set(String^ val);
		}

		property UserRole^ SelectedRole
		{
			UserRole^ get() { return selectedRole; }
			void set(UserRole^ role);
		}

		property UserRole^ EditedRole
		{
			UserRole^ get() { return editedRole; }
			void set(UserRole^ val);
		}

		property IObservableVector<FunctionalRightsSelection^>^ ListRights
		{
			IObservableVector<FunctionalRightsSelection^>^ get();
		private:
			void set(IObservableVector<FunctionalRightsSelection^>^);
		}

		property Command^ ImportCommand;
		property Command^ AddCommand;
		property Command^ DeleteCommand;
		property Command^ SaveCommand;
		property Command^ RefreshCommand;
	};

	[Bindable]
	public ref class AdminViewModel sealed : INotifyPropertyChanged
	{
	private:
		WeakReference parentVm;
		UserViewModel^ userVm;
		RoleViewModel^ rolesVm;
		ProductViewModel^ prodVm;

	public:

		AdminViewModel(scMainViewModel^ parentModel);

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		void InvokePropertyChanged(String^ propName);

		property scMainViewModel^ Parent
		{
			scMainViewModel^ get()
			{
				return parentVm.Resolve<scMainViewModel>();
			}
		}

		property UserViewModel^ UsersViewModel
		{
			UserViewModel^ get();
		}

		property RoleViewModel^ RolesViewModel
		{
			RoleViewModel^ get();
		}

		property ProductViewModel^ ProductVM
		{
			ProductViewModel^ get() 
			{
				if (prodVm == nullptr)
				{
					GUARDEDGLOCK;
					if (prodVm == nullptr)
					{
						prodVm = ref new ProductViewModel();
					}
				}
				return this->prodVm;
			}
		}
	};
}

