#pragma once

#include "dto.h"

namespace scwin
{

	[Bindable]
	public ref class Address sealed : IDTOBase
	{
		String^ id;
		String^ line1;
		String^ line2;
		String^ city;
		String^ province;
		String^ country;
		String^ zip;

	private:
		void RaisePropertyChanged(String^ propName);

	public:
		Address();
		Address(String^ id);
		virtual ~Address();

		property String^ Id
		{
			String^ get() { return id; }
		}

		property String^ Line1
		{
			String^ get() { return line1; }
			void set(String^ val)
			{
				if (val != line1)
				{
					line1 = val;
					RaisePropertyChanged("Line1");
				}
			}
		}
		property String^ Line2
		{
			String^ get() { return line2; }
			void set(String^ val)
			{
				if (val != line2)
				{
					line2 = val;
					RaisePropertyChanged("Line2");
				}
			}
		}
		property String^ City
		{
			String^ get() { return city; }
			void set(String^ val)
			{
				if (val != city)
				{
					city = val;
					RaisePropertyChanged("City");
				}
			}
		}
		property String^ Province
		{
			String^ get() { return province; }
			void set(String^ val)
			{
				if (val != province)
				{
					province = val;
					RaisePropertyChanged("Province");
				}
			}
		}
		property String^ Country
		{
			String^ get() { return country; }
			void set(String^ val)
			{
				if (val != country)
				{
					country = val;
					RaisePropertyChanged("Country");
				}
			}
		}
		property String^ PostalCode
		{
			String^ get() { return zip; }
			void set(String^ val)
			{
				if (val != zip)
				{
					zip = val;
					RaisePropertyChanged("PostalCode");
				}
			}
		}

		String^ ToString() override;

		// Inherited via IDTOBase
		virtual property bool CanRaiseEvents;
		virtual property DataState ObjectState;
		
		virtual property bool IsEmpty { bool get() { return false; }}
		virtual property bool IsValid {bool get(); }
		
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;
	};


}
