#pragma once

#include "dto.h"

namespace scwin
{
	[Bindable]
	public ref class CustomerContact sealed : public IDTOBase
	{
		String^ id;
		String^ corgccid; // costcenter id
		String^ firstName;
		String^ lastName;
		String^ email;
		String^ phone1;
		String^ phone2;
		String^ cellphone;
		String^ linkedin;
		String^ facebook;
		bool isactive;

	public:
		CustomerContact();
		CustomerContact(String^ id);
		virtual ~CustomerContact();

		property String^ Id
		{
			String^ get() { return id; }
		}

		property String^ CostCenterId
		{
			String^ get() { return corgccid; }
			void set(String^ val)
			{
				if (corgccid != val)
				{
					corgccid = val;
					RaisePropertyChanged("CostCenterId");
				}
			}
		}

		property String^ FirstName
		{
			String^ get() { return firstName; }
			void set(String^ val)
			{
				if (firstName != val)
				{
					firstName = val;
					RaisePropertyChanged("FirstName");
					RaisePropertyChanged(PROPERTYNAME(FullName));
				}
			}
		}

		property String^ LastName
		{
			String^ get() { return lastName; }
			void set(String^ val)
			{
				if (lastName != val)
				{
					lastName = val;
					RaisePropertyChanged("LastName");
					RaisePropertyChanged(PROPERTYNAME(FullName));
				}
			}
		}

		property String^ FullName
		{
			String^ get() { return String::Concat(String::Concat(firstName, " "), lastName); }
		}

		property String^ Email
		{
			String^ get() { return email; }
			void set(String^ val)
			{
				if (email != val)
				{
					email = val;
					RaisePropertyChanged("Email");
				}
			}
		}

		property String^ Phone1
		{
			String^ get() { return phone1; }
			void set(String^ val)
			{
				if (phone1 != val)
				{
					phone1 = val;
					RaisePropertyChanged("Phone1");
				}
			}
		}

		property String^ Phone2
		{
			String^ get() { return phone2; }
			void set(String^ val)
			{
				if (phone2 != val)
				{
					phone2 = val;
					RaisePropertyChanged("Phone2");
				}
			}
		}

		property String^ Cellphone
		{
			String^ get() { return cellphone; }
			void set(String^ val)
			{
				if (cellphone != val)
				{
					cellphone = val;
					RaisePropertyChanged("Cellphone");
				}
			}
		}

		property String^ LinkedIn
		{
			String^ get() { return linkedin; }
			void set(String^ val)
			{
				if (linkedin!= val)
				{
					linkedin = val;
					RaisePropertyChanged("LinkedIn");
				}
			}
		}

		property String^ Facebook
		{
			String^ get() { return facebook; }
			void set(String^ val)
			{
				if (facebook != val)
				{
					facebook = val;
					RaisePropertyChanged("Facebook");
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

		// Inherited via IDTOBase
		virtual property bool CanRaiseEvents;
		virtual property DataState ObjectState;
		virtual property bool IsEmpty
		{
			bool get() { return false; }
		}
		
		virtual property bool IsValid
		{
			bool get() 
			{
				bool b = StringHelpers::IsNullOrEmpty(corgccid) ||
					StringHelpers::IsNullOrEmpty(firstName) ||
					StringHelpers::IsNullOrEmpty(lastName);
				if (b)
					return false;
				if (StringHelpers::IsNullOrEmpty(email) || StringHelpers::IsNullOrEmpty(phone1))
					return false;
				return true; 
			}
		}

		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

	private:
		void RaisePropertyChanged(String ^ propName);

	};

}