//
// App.xaml.h
// Declaration of the App class.
//

#pragma once

#include "CountToVisibilityConverter.h"
#include "VisualStateTriggerConverter.h"
#include "PropertyNotSetConverter.h"
#include "BooleanToStringConverter.h"
#include "BooleanToVisibilityConverter.h"
#include "StringLengthConverters.h"
#include "NullVisibilityConverter.h"
#include "NullToBooleanConverter.h"
#include "ReverseBooleanConverter.h"
#include "NavMenuListView.h"
#include "DateTimeFormatConverter.h"
#include "CurrencyConverter.h"
#include "PricePlanAllInOneSelector.h"
#include "Enums.h"
#include "PictureSelection.xaml.h"
#include "ExpandPanel.h"
#include "ViewToEditorView.h"
#include "ViewLifeTimeController.h"
#include "PhoneFormatConverter.h"
#include "StringToUriConverter.h"
#include "ValidationMask.h"
#include "DataStateVisibilityTrigger.h"
#include "DeviceFamilyStateTrigger.h"
#include "UserInteractionStateTrigger.h"
#include "DataStateEnableConverter.h"
#include "Themes\AppDataTemplates.xaml.h"
#include "App.g.h"
#include "StartupEventArgs.h"
#include "IsTenFootTrigger.h"
#include "BreadcrumbControl.h"
#include "GridOrListView.h"


namespace scwin
{
	/// <summary>
	/// Provides application-specific behavior to supplement the default Application class.
	/// </summary>
	ref class App sealed
	{
	private:
		Windows::ApplicationModel::Background::BackgroundTaskRegistration^ taskReg;
	public:
		property Windows::Foundation::Collections::IVector<ViewLifeTimeController^>^ SecondaryViews;

	protected:
		virtual void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e) override;
		virtual void OnBackgroundActivated(Windows::ApplicationModel::Activation::BackgroundActivatedEventArgs^ args) override;
		virtual void OnActivated(Windows::ApplicationModel::Activation::IActivatedEventArgs^ args) override;

	internal:
		App();

	public:
		virtual ~App();

	private:
		void ExtendAcrylicIntoTitleBar();
		void OnResuming(Object^ sender, Object^ args);
		void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ e);
		void OnEnteredBackground(Platform::Object^ sender, Windows::ApplicationModel::EnteredBackgroundEventArgs^ e);
		void OnLeavingBackground(Platform::Object^ sender, Windows::ApplicationModel::LeavingBackgroundEventArgs^ e);
		void OnNavigationFailed(Platform::Object ^sender, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs ^e);
		void OnUnhandledException(Platform::Object^ sender, Windows::UI::Xaml::UnhandledExceptionEventArgs^ e);
		void OnViewModelPropertyChanged(Object^ sender, PropertyChangedEventArgs^ eargs);

		void SetupView();
		void LoadWaitView();
		void LoadRootView();
		void LoadSignInView();
		void LoadSignUpView();

	private:
		// register background task
		void SetupInprocBackgroundTasks();
	};
}
