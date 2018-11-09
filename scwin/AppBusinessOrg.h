#pragma once

#include "Picture.h"

namespace scwin
{
	ref class TechContact;
	ref class HelpDesk;

	/// Application Business Owner Organization.
	public ref class AppBusinessOrg sealed : public IDTOBase
	{
		String^ _name;
		String^ _addressId;
		String^ _logoImageId; // Picture instance is created from logo
		TechContact^ _techContact;
		String^ _website;
		HelpDesk^ _helpdesk;

	public:
		AppBusinessOrg();

		AppBusinessOrg(String^ id);

		property String^ Id;
		
		property String^ Name
		{
			String^ get();
			void set(String^ val);
		}

		property String^ AddressId
		{
			String^ get();
			void set(String^ val);
		}

		property scwin::Address^ Address
		{
			scwin::Address^ get();
		}

		property String^ LogoImageId
		{
			String^ get();
			void set(String^ val);
		}

		property scwin::TechContact^ TechContact
		{
			scwin::TechContact^ get();
			void set(scwin::TechContact^ val);
		}

		property String^ Website
		{
			String^ get();
			void set(String^ val);
		}

		property scwin::HelpDesk^ HelpDesk
		{
			scwin::HelpDesk^ get();
			void set(scwin::HelpDesk^ val);
		}

		// Inherited via IDTOBase
		virtual property bool CanRaiseEvents;
		
		virtual property DataState ObjectState;
		
		virtual property bool IsEmpty
		{
			bool get();
		}

		virtual property bool IsValid
		{
			bool get();
		}
		
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

	private:
		DECLARERAISEPROPCHANGED;
	};

	/// This is stored as an embedded object in AppBusinessOrg object.
	public ref class TechContact sealed 
	{
	public:
		property String^ Name;
		property String^ Phone;
		property String^ Email;
		bool Equals(TechContact^ obj);
	};

	/// Another embedded object inside AppBussinessOrg object.
	public ref class HelpDesk sealed
	{
	public:
		property String^ Phone;
		property String^ Email;
		property String^ Url;
		bool Equals(HelpDesk^ obj);
	};
}