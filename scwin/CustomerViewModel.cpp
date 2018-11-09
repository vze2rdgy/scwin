#include "pch.h"
#include "CustomerViewModel.h"
#include "cpplinq.hpp"

#define CUSTOMERVIEWMODEL_LASTVIEWTYPE L"Admin_Customer_LastGridOrListViewType"

CustomerViewModel::CustomerViewModel()
{
	create_task([=]() {CustomerViewModel::RestoreState(); });
}

scwin::CustomerViewModel::~CustomerViewModel()
{
	SaveState();
}


void scwin::CustomerViewModel::RestoreState()
{
	// restore from settings table any saved properties.
	_bIsCommited = false;

	using namespace cpplinq;
	// load organizations
	auto lds = scLocalDataService::Instance();
	auto orgs = lds->GetCustomerOrganizations();
	Organizations = ref new Vector<CustomerDtosGroupedBindable^>(
		from(to_vector(orgs)) >> select([](CustomerOrg^ org)
	{
		return ref new CustomerDtosGroupedBindable(org);
	}) >> to_vector() );

	create_task(lds->GetPropertyAsync(CUSTOMERVIEWMODEL_LASTVIEWTYPE))
		.then([this](AppProperty^ prop)
	{
		if (prop != nullptr && prop->Value != nullptr)
		{
			int val = wcstol(prop->Value->Data(), nullptr, 10);
			LastViewType = (GridOrListViewViewType)val;
		}
	});
}

void scwin::CustomerViewModel::SaveState()
{
	if (!_bIsCommited)
	{
		// save any restorable properties in settings table.
		auto lds = scLocalDataService::Instance();
		auto prop = ref new AppProperty(CUSTOMERVIEWMODEL_LASTVIEWTYPE, StringHelpers::ConvertToString(to_string((int)LastViewType).c_str()), true);
		lds->SetProperty(prop);
		_bIsCommited = true;
	}
}

Platform::Object^ CustomerViewModel::ActiveDataContext::get()
{
	return _activeDataContext;
}

void CustomerViewModel::ActiveDataContext::set(Platform::Object^ val)
{
	if (val != _activeDataContext)
	{
		_activeDataContext = val;
		RaisePropertyChanged(PROPERTYNAME(ActiveDataContext));
	}
}

Platform::String^ CustomerViewModel::ActiveDataTemplateName::get()
{
	return _activeDataTemplateName;
}

void CustomerViewModel::ActiveDataTemplateName::set(Platform::String^ val)
{
	if (_activeDataTemplateName != val)
	{
		_activeDataTemplateName = val;
		RaisePropertyChanged(PROPERTYNAME(ActiveDataTemplateName));
	}
}


DEFINERAISEPROPCHANGEDBASIC(CustomerViewModel);
