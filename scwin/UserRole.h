#pragma once

#include "dto.h"
#include "StringHelpers.h"
#include "Enums.h"

namespace scwin
{

	[Bindable]
	public ref class UserRole sealed : IDTOBase
	{
	private:
		String^ name;
		uint64 rights;
		String^ sfuncRights;

	public:

		property bool IsSelected; // used for gui purposes only

	public:
		UserRole();

		UserRole(String^ id);

		UserRole^ Clone();

		property String^ Id;

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		void RaisePropertyChanged(String^ propName);

		property String^ Name 
		{
			String^ get() 
			{ 
				return name; 
			}
			void set(String^ val) 
			{ 
				this->name = val; 
				RaisePropertyChanged(L"Name");
			}
		}

		
		property uint64 Rights
		{
			uint64 get();
			void set(uint64 val);
		}


		// Inherited via IDTOBase
		virtual property DataState ObjectState;


		// Inherited via IDTOBase
		virtual property bool IsEmpty
		{
			bool get() {
				return scwin::utils::StringHelpers::IsNullOrEmpty(name) || rights == 0;
			}
		}

		virtual property bool IsValid
		{
			bool get()
			{
				return true;
			}
		}

		property String^ FunctionalRightsAsString
		{
			String^ get();
		private:
			void set(String^);
		}


		// Inherited via IDTOBase
		virtual property bool CanRaiseEvents;

	private:
		void SetRightsAsString();

	};


}