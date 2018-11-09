#pragma once

#include "dto.h"

namespace scwin
{
	[Bindable]
	public ref class CustomerOrg sealed : public IDTOBase
	{
		String^ id;
		String^ code;
		String^ name;
		String^ website;
		String^ linkedIn;
		String^ facebook;
		String^ notes;
		bool isactive;

	public:
		CustomerOrg();
		CustomerOrg(String^ id);
		
		virtual ~CustomerOrg();

		// Inherited via IDTOBase
		virtual property bool CanRaiseEvents;
		virtual property DataState ObjectState;
		virtual property bool IsEmpty 
		{ 
			bool get() 
			{ 
				return false; 
			}
		}
		virtual property bool IsValid 
		{ 
			bool get() 
			{ 
				if (
					StringHelpers::IsNullOrEmpty(name) || 
					StringHelpers::IsNullOrEmpty(code)
					)
					return false;
				return true; 
			}
		}
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		property String^ Id
		{
			String^ get() { return id; }
		}

		property String^ Code
		{
			String^ get() { return code; }
			void set(String^ val)
			{
				if (val != code)
				{
					code = val;
					RaisePropertyChanged("Code");
				}
			}
		}

		property String^ Name
		{
			String^ get()
			{
				return name;
			}

			void set(String^ val)
			{
				if (name != val)
				{
					name = val;
					RaisePropertyChanged("Name");
				}
			}
		}


		property String^ Website
		{
			String^ get() { return website; }
			void set(String^ val)
			{
				if (val != website)
				{
					website = val;
					RaisePropertyChanged("Website");
				}
			}
		}

		property String^ LinkedIn
		{
			String^ get() { return linkedIn; }
			void set(String^ val)
			{
				if (val != linkedIn)
				{
					linkedIn = val;
					RaisePropertyChanged("LinkedIn");
				}
			}
		}

		property String^ Facebook
		{
			String^ get() { return facebook; }
			void set(String^ val)
			{
				if (val !=  facebook)
				{
					facebook = val;
					RaisePropertyChanged("Facebook");
				}
			}
		}

		property String^ Notes
		{
			String^ get() { return notes; }
			void set(String^ val)
			{
				if (val != notes)
				{
					notes = val;
					RaisePropertyChanged("Notes");
				}
			}
		}

		property bool IsActive
		{
			bool get() { return isactive; }
			void set(bool val)
			{
				if (val != isactive)
				{
					isactive = val;
					RaisePropertyChanged("IsActive");
				}
			}
		}

	private:
		void RaisePropertyChanged(String ^ propName);


	};

}