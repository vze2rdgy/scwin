#include "pch.h"
#include "SettingsViewModel.h"
#include "Address.h"

#define LASTSELECTIONPROPERTY "Settings_LastSelection"

SettingsViewModel::SettingsViewModel()
{
	// get the last selection
	auto ls = scLocalDataService::Instance();
	create_task(ls->GetPropertyAsync(LASTSELECTIONPROPERTY))
		.then([this](AppProperty^ property)
	{
		if (property != nullptr && property->HasValue())
		{
			int converted = std::wcstol(property->Value->Data(), nullptr, 10);
			if (converted >= 0)
				LastSelection = converted;
			else
				LastSelection = 0;
		}
		else
			LastSelection = 0; // making individual setting for each if condition to avoid duplicate property notifications.
	});
}

void scwin::SettingsViewModel::LoadState(int PivotIndex)
{
	WeakReference wr(this);
	switch (PivotIndex)
	{
	case 0: // Organization pivot
		create_task(scLocalDataService::Instance()->GetAppBusinessOrgAsync())
			.then([wr](AppBusinessOrg^ org)
		{
			auto ths = wr.Resolve<SettingsViewModel>();
			ths->BusinessDetails = org;
			ths->IsBusinessDetailsAvailable = !org->IsEmpty;
		});
		break;
	case 1:	// Options
		break;
	case 2: // About me
		break;
	}

	// Save the pivot selection
	LastSelection = PivotIndex; // because binding is set to OneTime
	auto prop = ref new AppProperty(LASTSELECTIONPROPERTY, ref new String(std::to_wstring(PivotIndex).c_str()), true);
	scLocalDataService::Instance()->SetPropertyAsync(prop);
}

void scwin::SettingsViewModel::SelectPanelContent(int panelType)
{
	_selectedPanelId = panelType;
	switch (panelType)
	{
	case 0:
		ActiveDataContext = ref new AppBusinessOrgFacade(_busdetails);
		DisplayEditOrgPanel = true;
		DisplayEditOrgMorePanel = false;
		break;
	case 1:
		ActiveDataContext = ref new AppBusinessOrgFacade(_busdetails);
		DisplayEditOrgPanel = false;
		DisplayEditOrgMorePanel = true;
		break;
	}
}

IAsyncOperation<bool>^ scwin::SettingsViewModel::CancelAsync()
{
	return create_async([=]()
	{
		//_selectedPanelId
		return true;
	});
}

IAsyncOperation<bool>^  scwin::SettingsViewModel::SaveAsync()
{
	return create_async([=]()
	{
		//_selectedPanelId
		switch (_selectedPanelId)
		{
		case 0: // org details
		{
			auto busdetfacade = (AppBusinessOrgFacade^)ActiveDataContext;
			if (!busdetfacade->IsValid)
			{
				PubSubService::Instance->Publish(
					this,
					PubSubMessageIds::ApplicationErrorStatusMessage,
					"Error saving information. Please ensure that all required fields are provided."
				);
				return false;
			}
			auto lds = scLocalDataService::Instance();
			try
			{
				lds->StartTransaction();
				auto org = busdetfacade->Org;
				org->ObjectState = _isbusdetavailable ? DataState::Edited : DataState::Added;
				auto msgId = lds->UpdateAddress(busdetfacade->Address);
				if (msgId != MessageIds::Success)
				{
					lds->CancelTransaction();
					PubSubService::Instance->Publish(
						this,
						PubSubMessageIds::ApplicationErrorStatusMessage,
						ref new String(GetMessage(msgId))
					);
					return false;
				}
				else
					org->AddressId = busdetfacade->Address->Id;
				org->TechContact = busdetfacade->TechContact;
				msgId = lds->SaveAppBusinessOrg(org);
				if (msgId != MessageIds::Success)
				{
					lds->CancelTransaction();
					PubSubService::Instance->Publish(
						this,
						PubSubMessageIds::ApplicationErrorStatusMessage,
						"Failed to save application organization information. Please retry."
					);
					return false;
				}
				lds->CommitTransaction();
				IsBusinessDetailsAvailable = true;
			}
			catch (Exception^ ex)
			{
				lds->CancelTransaction();
				PubSubService::Instance->Publish(
					this,
					PubSubMessageIds::ApplicationErrorStatusMessage,
					"Failed to save application organization information. Reported error: " + ex->Message + "."
				);
				return false;
			}
		}
		case 1: // Org additional details
		{
			auto busdetfacade = (AppBusinessOrgFacade^)ActiveDataContext;
			if (StringHelpers::IsNullOrEmpty(busdetfacade->Org->Website) || 
				(StringHelpers::IsNullOrEmpty(busdetfacade->HelpDesk->Phone) &&
					StringHelpers::IsNullOrEmpty(busdetfacade->HelpDesk->Email) &&
					StringHelpers::IsNullOrEmpty(busdetfacade->HelpDesk->Url)
					)
				)
			{
				PubSubService::Instance->Publish(
					this,
					PubSubMessageIds::ApplicationErrorStatusMessage,
					"Error saving information. Please ensure that all required fields are provided."
				);
				return false;
			}
			auto org = busdetfacade->Org;
			org->ObjectState = DataState::Edited ;
			org->HelpDesk = busdetfacade->HelpDesk;
			auto lds = scLocalDataService::Instance();
			try
			{
				lds->StartTransaction();
				auto msgId = lds->SaveAppBusinessOrg(org);
			if (msgId != MessageIds::Success)
			{
				lds->CancelTransaction();
				PubSubService::Instance->Publish(
					this,
					PubSubMessageIds::ApplicationErrorStatusMessage,
					"Failed to save additional details about the application organization. Please retry."
				);
				return false;
			}
			lds->CommitTransaction();
			}
			catch (Exception^ ex)
			{
				lds->CancelTransaction();
				PubSubService::Instance->Publish(
					this,
					PubSubMessageIds::ApplicationErrorStatusMessage,
					"Failed to save application organization information. Reported error: " + ex->Message + "."
				);
				return false;
			}
			break;
		}
		}
		return true;
	});
}

int SettingsViewModel::LastSelection::get()
{
	return _lastSel;
}

void SettingsViewModel::LastSelection::set(int val)
{
	if (_lastSel != val)
	{
		_lastSel = val;
		RaisePropertyChanged(PROPERTYNAME(LastSelection));
	}
}

AppBusinessOrg^ SettingsViewModel::BusinessDetails::get()
{
	return _busdetails;
}

void SettingsViewModel::BusinessDetails::set(AppBusinessOrg^ val)
{
	if (_busdetails != val)
	{
		_busdetails = val;
		RaisePropertyChanged(PROPERTYNAME(BusinessDetails));
	}
}

bool SettingsViewModel::IsBusinessDetailsAvailable::get()
{
	return _isbusdetavailable;
}

void SettingsViewModel::IsBusinessDetailsAvailable::set(bool val)
{
	if (_isbusdetavailable != val)
	{
		_isbusdetavailable = val;
		RaisePropertyChanged(PROPERTYNAME(IsBusinessDetailsAvailable));
	}
}

Platform::Object^ SettingsViewModel::ActiveDataContext::get()
{
	return _activeDataContext;
}

void SettingsViewModel::ActiveDataContext::set(Platform::Object^ val)
{
	if (val != _activeDataContext)
	{
		_activeDataContext = val;
		RaisePropertyChanged(PROPERTYNAME(ActiveDataContext));
	}
}

//Platform::String^ SettingsViewModel::ActiveDataTemplateName::get()
//{
//	return _activeDataTemplateName;
//}
//
//void SettingsViewModel::ActiveDataTemplateName::set(Platform::String^ val)
//{
//	if (_activeDataTemplateName != val)
//	{
//		_activeDataTemplateName = val;
//		RaisePropertyChanged(PROPERTYNAME(ActiveDataTemplateName));
//	}
//}

DEFINERAISEPROPCHANGEDBASIC(SettingsViewModel);

/////////////////////////////////// AppBusinessOrgFacade ////////////////////////////////

scwin::AppBusinessOrgFacade::AppBusinessOrgFacade(AppBusinessOrg ^ org)
{
	this->Org = org;
	if (org->Address != nullptr)
		Address = org->Address;
	else
		Address = ref new scwin::Address();
	if (org->TechContact != nullptr)
		TechContact = org->TechContact;
	else
		TechContact = ref new scwin::TechContact();
	if (org->HelpDesk != nullptr)
		HelpDesk = org->HelpDesk;
	else
		HelpDesk = ref new scwin::HelpDesk();
}

bool AppBusinessOrgFacade::IsValid::get()
{
	if (Org == nullptr || !Org->IsValid)
		return false;
	if (TechContact == nullptr)
		return false;
	if (StringHelpers::IsNullOrEmpty(TechContact->Name) ||
		StringHelpers::IsNullOrEmpty(TechContact->Phone))
		return false;
	return true;
}