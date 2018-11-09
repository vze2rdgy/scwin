#pragma once

#include "dto.h"
#include "ObservableList.h"

using namespace Windows::UI::Xaml::Media::Imaging;

namespace scwin
{
	[Bindable]
	public ref class UserProfile sealed : IDTOBase
	{
		String^ id;
		String^ fullName;
		String^ dbKey;
		String^ scode;
		IObservableVector<String^>^ roles;
		String^ memroleAsString;
		DataState datastate;

	public: // Used for GUI purposes only
		property bool IsSelected;
		
	internal:
		void UpdateRoleStr();

	public:
		UserProfile();
		UserProfile(String^ email, String^ fullName, String^ dbKey, IObservableVector<String^>^ roles);

		//property SoftwareImage^ Image;

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		property String^ Id
		{
			String^ get()
			{
				return this->id;
			}
		}

		property String^ Email
		{
			String^ get()
			{ 
				return this->id;
			}
			void set(String^ val)
			{
				id = val;
				RaisePropertyChanged(L"Email");
			}
		}

		property String^ FullName
		{
			String^ get()
			{
				return this->fullName;
			}
			void set(String^ val)
			{
				fullName = val;
				RaisePropertyChanged(L"FullName");
			}
		}

		//property String^ UserCode
		//{
		//	String^ get()
		//	{
		//		return this->scode;
		//	}
		//	void set(String^ val)
		//	{
		//		scode = val;
		//		RaisePropertyChanged(L"UserCode");
		//	}
		//}

		property String^ DbKey // this is readonly property.
		{
			String^ get()
			{
				return this->dbKey;
			}
		}

		property IObservableVector<String^>^ Roles
		{
			IObservableVector<String^>^ get()
			{
				if (roles == nullptr)
				{
					roles = ref new Vector<String^>();
				}
				return roles;
			}
			void set(IObservableVector<String^>^ val)
			{
				roles = val;
			}
		}

		// Inherited via IDTOBase
		virtual property DataState ObjectState
		{
			DataState get() { return datastate; }
			void set(DataState val)
			{
				datastate = val;
				RaisePropertyChanged(PROPERTYNAME(ObjectState));
			}
		}

		virtual property bool IsEmpty
		{
			bool get() 
			{
				return scwin::utils::StringHelpers::IsNullOrEmpty(id) && scwin::utils::StringHelpers::IsNullOrEmpty(fullName);
			}
		}

		virtual property bool IsValid
		{
			bool get();
		}

		property String^ MemberRolesAsString
		{
			String^ get();
		private:
			void set(String^);
		}


		// Inherited via IDTOBase
		virtual property bool CanRaiseEvents;

		UserProfile^ Clone();

	private:
		DECLARERAISEPROPCHANGED;
};

	typedef Vector<UserProfile^> UserProfileList;

	typedef IObservableVector<UserProfile^> IUserProfileList;

}