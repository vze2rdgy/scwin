#pragma once

#include "AppBusinessOrg.h"

namespace scwin
{

	public ref class SettingsViewModel sealed : INotifyPropertyChanged
	{
		AppBusinessOrg^ _busdetails;
		int _lastSel;
		bool _isbusdetavailable = false;
		Platform::Object^ _activeDataContext;
		//Platform::String^ _activeDataTemplateName;
		bool _displayEditOrgPanel;
		bool _displayEditOrgMorePanel;
		int _selectedPanelId;

	public:
		SettingsViewModel();

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		void LoadState(int PivotIndex);

		/// <summary>
		/// Provide below values for various Panel Types
		/// 0 : EditAppBusinessDetails
		/// 1 : 
		/// </summary>
		void SelectPanelContent(int panelType);

		property int LastSelection
		{
			int get();
			void set(int val);
		}

		property AppBusinessOrg^ BusinessDetails
		{
			AppBusinessOrg^ get();
		private:
			void set(AppBusinessOrg^ val);
		}

		property bool IsBusinessDetailsAvailable
		{
			bool get();
		private:
			void set(bool val);
		}

		property Platform::Object^ ActiveDataContext
		{
			Platform::Object^ get();
			void set(Platform::Object^ val);
		}

		//property Platform::String^ ActiveDataTemplateName
		//{
		//	Platform::String^ get();
		//private:
		//	void set(Platform::String^ val);
		//}

		property bool DisplayEditOrgPanel
		{
			bool get() { return _displayEditOrgPanel; }
			void set(bool val)
			{
				if (val != _displayEditOrgPanel)
				{
					_displayEditOrgPanel = val;
					RaisePropertyChanged(PROPERTYNAME(DisplayEditOrgPanel));
				}
			}
		}

		property bool DisplayEditOrgMorePanel
		{
			bool get() { return _displayEditOrgMorePanel; }
			void set(bool val)
			{
				if (val != _displayEditOrgMorePanel)
				{
					_displayEditOrgMorePanel = val;
					RaisePropertyChanged(PROPERTYNAME(DisplayEditOrgMorePanel));
				}
			}
		}

		/// <summary>
		/// Save the respectively selected information in the panel
		/// </summary>
		IAsyncOperation<bool>^ SaveAsync();

		IAsyncOperation<bool>^ CancelAsync();

	private:

		DECLARERAISEPROPCHANGED;
	};

	public ref class AppBusinessOrgFacade sealed : INotifyPropertyChanged
	{

	private:
		DECLARERAISEPROPCHANGED;

	public:
		AppBusinessOrgFacade(AppBusinessOrg^ org);

		property AppBusinessOrg^ Org;
		property Address^ Address;
		property TechContact^ TechContact;
		property HelpDesk^ HelpDesk;

		property bool IsValid
		{
			bool get();
		}

		property String^ Error;

	public:
		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

	};
}

