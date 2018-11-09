#pragma once

#include "UserRole.h"

using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Media::Imaging;

namespace scwin
{

	[Bindable]
	public ref class UserContext sealed : public INotifyPropertyChanged
	{
	public:
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

	private:
		String^ name;
		String^ fullName;
		String^ emailAddress;
		BitmapImage^ profImg;
		IVector<UserRole^>^ roles;
		String^ dbkey;
		uint64 userRights;

	private:
		void RaisePropertyChanged(String^ propertyName);

	public:
		UserContext();

	public:
		property String^ Name
		{
		public:
			String^ get();
			void set(String^ val);
		}

		property IVector<UserRole^>^ Roles
		{
		public:
			IVector<UserRole^>^ get();
			void set(IVector<UserRole^>^ val);
		}

		property String^ FullName
		{
			String^ get();
			void set(String^ val);
		}

		property String^ Email
		{
			String^ get();
			void set(String^);
		}

		property BitmapImage^ ProfileImage
		{
			BitmapImage^ get();
			void set(BitmapImage^);
		}

	internal:
		property String^ Key
		{
			String^ get() { return this->dbkey; }
			void set(String^ val) { dbkey = val; }
		}

		bool HasRight(FunctionalRights requestedRight);

		void SetRight(FunctionalRights newRight);

		void RemoveRight(FunctionalRights oldRight);

		void ClearRight();

	};

}