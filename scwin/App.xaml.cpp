//
// App.xaml.cpp
// Implementation of the App class.
//

#include "pch.h"
#include "StreetcatAppTrigger.h"
#include "AppTabView.xaml.h"
#include "MobView.xaml.h"
#include "scMainViewModel.h"
#include "MainDashboard.xaml.h"
#include "MainPage.xaml.h"
#include "SignIn.xaml.h"
#include "SignUp.xaml.h"
#include "SuspensionManager.h"
#include "LayoutAwarePage.h"
#include "ReplicationService.h"
#include "ProductFamilies.xaml.h"
#include "ProductsSelectList.xaml.h"
#include "ChoooseACustomer.xaml.h"
#include "AddressView.xaml.h"
#include "CustomerViewControl.xaml.h"
#include "ChooseAProduct.xaml.h"
#include "CompositionShadow.xaml.h"
#include "AppNavigator.xaml.h"
#include "WaitView.xaml.h"
#include "AddressEditControl.xaml.h"
#include "ProductCategoryHierarchy.xaml.h"
#include "GridOrListView.h"
#include "ContentControlRevised.h"
#include "FileLogging.h"

using namespace scwin;

using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Security::Credentials;
using namespace Windows::UI::Notifications;

scMainViewModel^ mainViewModel;

CoreDispatcher^ AppDispatcher = nullptr;

/// <summary>
/// Initializes the singleton application object.  This is the first line of authored code
/// executed, and as such is the logical equivalent of main() or WinMain().
/// </summary>
App::App()
{

#ifdef _DEBUG
	// The following code is used for detecting memory leaks. If a memory leak is detected when the application exits,
	// the information will be printed out to the debug output window. Change the -1 in _CrtSetBreakAlloc( -1 ) to the
	// allocation number and run the code again... easy to tell if bits and pieces aren't being cleaned up
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);

	_CrtSetBreakAlloc(-1);
#endif


	Sg::SuspensionManager::RegisterDependencyProperties();
	scwin::LayoutAwarePage::RegisterDependencyProperties();
	scwin::ExpandPanel::RegisterDependencyProperties();
	scwin::ViewToEditorView::RegisterDependencyProperties();
	ColorMasking::RegisterDependencyProperties();
	//StatusMessageTrigger::RegisterDps();
	DataStateVisibilityTrigger::Register();
	ProductFamilies::Register();
	ProductsSelectList::Register();
	ChoooseACustomer::Register();
	AddressView::Register();
	CustomerViewControl::Register();
	ChooseAProduct::Register();
	DeviceFamilyStateTrigger::Register();
	UserInteractionStateTrigger::Register();
	CompositionShadow::Register();
	AppNavigator::Register();
	//AppView::Register();
	AddressEditControl::Register();
	OrientationStateTrigger::RegisterDps();
	ProductCategoryHierarchy::RegisterDependencyProperties();
	BreadcrumbControl::Register();
	GridOrListView::Register();
	ContentControlRevised::Register();

    InitializeComponent();

    Suspending += ref new SuspendingEventHandler(this, &App::OnSuspending);
	Resuming += ref new EventHandler<Object^>(this, &App::OnResuming);
	EnteredBackground += ref new EnteredBackgroundEventHandler(this, &App::OnEnteredBackground);
	LeavingBackground += ref new LeavingBackgroundEventHandler(this, &App::OnLeavingBackground);

#ifndef _DEBUG
	UnhandledException += ref new UnhandledExceptionEventHandler(this, &App::OnUnhandledException);
#endif

	SecondaryViews = ref new Platform::Collections::Vector<ViewLifeTimeController^>();
	FileLoggingSessionScenario::Instance->ToggleLoggingEnabledDisabledAsync();
}

scwin::App::~App()
{
}

/// Extend acrylic into the title bar. 
void scwin::App::ExtendAcrylicIntoTitleBar()
{
	using namespace Windows::ApplicationModel::Core;
	CoreApplication::GetCurrentView()->TitleBar->ExtendViewIntoTitleBar = true;
	ApplicationViewTitleBar^ titleBar = ApplicationView::GetForCurrentView()->TitleBar;
	titleBar->ButtonBackgroundColor = Colors::Transparent;
	titleBar->ButtonInactiveBackgroundColor = Colors::Transparent;
}


/// <summary>
/// Invoked when the application is launched normally by the end user.  Other entry points
/// will be used such as when the application is launched to open a specific file.
/// </summary>
/// <param name="e">Details about the launch request and process.</param>
void App::OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e)
{
	//SetupInprocBackgroundTasks();

	ApplicationView::GetForCurrentView()->SetPreferredMinSize(Size(320, 200));
	ApplicationViewTitleBar^ titleBar = ApplicationView::GetForCurrentView()->TitleBar;
	Windows::ApplicationModel::Core::CoreApplication::GetCurrentView()->TitleBar->ExtendViewIntoTitleBar = false;
	if (titleBar != nullptr)
	{
		Color color = safe_cast<Color>(this->Resources->Lookup(L"SystemChromeMediumColor"));
		titleBar->BackgroundColor = color;
		titleBar->ButtonBackgroundColor = color;
	}

	//ExtendAcrylicIntoTitleBar();

	if (scwin::utils::SysInfoHelpers::IsTenFootExperience())
	{
		ApplicationView::GetForCurrentView()->SetDesiredBoundsMode(ApplicationViewBoundsMode::UseCoreWindow);
		ResourceDictionary^ tenFootDict = ref new ResourceDictionary();
		tenFootDict->Source = ref new Uri("ms-appx:///Themes/TenFeetStylesSheet.xaml");
		this->Resources->MergedDictionaries->Append(tenFootDict);
	}

	mainViewModel = scMainViewModel::Instance;
	// subscripe to events in the vm
	mainViewModel->PropertyChanged += ref new PropertyChangedEventHandler(this, &App::OnViewModelPropertyChanged);
	AppDispatcher = Window::Current->Dispatcher;
}

void App::OnViewModelPropertyChanged(Object^ sender, PropertyChangedEventArgs^ eargs)
{
	if (eargs->PropertyName == L"AuthenticationStage")
	{
		WeakReference wr(this);
		Window::Current->Dispatcher->RunAsync(
			Windows::UI::Core::CoreDispatcherPriority::High, ref new Windows::UI::Core::DispatchedHandler([wr, eargs]()
		{
			auto ths = wr.Resolve<App>();
			switch (mainViewModel->AuthenticationStage)
			{
			case LandingContentStage::SignUp:
				ths->LoadSignUpView();
				break;
			case LandingContentStage::Dashboard:
				ths->LoadRootView();
				break;
			case LandingContentStage::Wait:
				ths->LoadWaitView();
				break;
			case LandingContentStage::SignIn:
			default:
				ths->LoadSignInView();
				break;
			}
			ths->SetupView();
			//else if (eargs->PropertyName == L"Error")
			//{
			//	auto err = ref new TextBlock();
			//	err->Text = mainViewModel->Error;
			//	err->HorizontalAlignment = HorizontalAlignment::Center;
			//	err->VerticalAlignment = VerticalAlignment::Center;
			//	Window::Current->Content = err;
			//	// Ensure the current window is active
			//	SetupView();
			//}
		})
		);
	}
}

void App::SetupView()
{
	auto frame = dynamic_cast<Frame^>(Window::Current->Content);
	if (frame)
	{
		Sg::SuspensionManager::RegisterFrame(safe_cast<Frame^>(Window::Current->Content), L"AppFrame");

		//if (this-> PreviousExecutionState == ApplicationExecutionState::Terminated)
		//{
		//	// TODO: Restore the saved session state only when appropriate, scheduling the
		//	// final launch steps after the restore is complete
		//	try
		//	{
		//		Sg::SuspensionManager::RestoreAsync();
		//	}
		//	catch (Exception^ ex)
		//	{
		//		// TODO log....
		//	}
		//}
	}
	// Ensure the current window is active
	Window::Current->Activate();
}

void scwin::App::LoadWaitView()
{
	Window::Current->Content = ref new WaitView();
}

void App::LoadRootView()
{
	// before starting the root view, we have to ensure that
	// database is ready.
	auto rootPage = dynamic_cast<Page^>(Window::Current->Content);
	// Create a Frame to act as the navigation context and associate it with
	// a SuspensionManager key
	auto deviceFamily = Windows::System::Profile::AnalyticsInfo::VersionInfo->DeviceFamily;
	if (deviceFamily == "Windows.Mobile")
	{
		auto rootView = ref new MobView(mainViewModel);
		rootPage = rootView;
	}
	else
	{
		auto rootView = ref new AppTabView(mainViewModel);
		// Do not repeat app initialization when the Window already has content,
		// just ensure that the window is active
		if (rootView == nullptr)
		{
			rootView->AppFrame->NavigationFailed += ref new Windows::UI::Xaml::Navigation::NavigationFailedEventHandler(this, &App::OnNavigationFailed);
		}
		if (rootView->AppFrame == nullptr)
		{
			rootView->AppFrame->Navigate(
				TypeName(MainDashboard::typeid), nullptr, ref new Windows::UI::Xaml::Media::Animation::SuppressNavigationTransitionInfo());
		}
		rootPage = rootView;
	}
	Window::Current->Content = rootPage;
}

void scwin::App::LoadSignInView()
{
	Window::Current->Content = ref new MainPage(mainViewModel, LandingContentStage::SignIn);
}

void scwin::App::LoadSignUpView()
{
	Window::Current->Content = ref new MainPage(mainViewModel, LandingContentStage::SignUp);
}

// Called when a registered background timertrigger is invoked.
void scwin::App::OnBackgroundActivated(Windows::ApplicationModel::Activation::BackgroundActivatedEventArgs ^ args)
{
	if (args->TaskInstance->Task == this->taskReg)
	{
	}
}

void scwin::App::OnActivated(Windows::ApplicationModel::Activation::IActivatedEventArgs ^ args)
{
	auto ev = dynamic_cast<ToastNotificationActivatedEventArgs^>(args);
	if (ev)
	{
		auto arg = ev->Argument;
		if (arg == "dismiss")
		{
			// Stop the timer...
		}
	}

}

void scwin::App::OnResuming(Object ^ sender, Object ^ args)
{
}


/// <summary>
/// Invoked when application execution is being suspended.  Application state is saved
/// without knowing whether the application will be terminated or resumed with the contents
/// of memory still intact.
/// </summary>
/// <param name="sender">The source of the suspend request.</param>
/// <param name="e">Details about the suspend request.</param>
void App::OnSuspending(Object^ sender, SuspendingEventArgs^ e)
{
	auto deferral = e->SuspendingOperation->GetDeferral();
	deferral->Complete();
	Sg::SuspensionManager::SaveAsync()->Completed = ref new AsyncActionCompletedHandler([&deferral](IAsyncAction^ action, AsyncStatus status)
	{
		scLocalDataService::Cleanup();

#ifdef _DEBUG
		_CrtMemState state;
		ZeroMemory(&state, sizeof(_CrtMemState));
		_CrtMemDumpStatistics(&state);
		_CrtDumpMemoryLeaks();
#endif
	});
}

void scwin::App::OnEnteredBackground(Platform::Object ^ sender, Windows::ApplicationModel::EnteredBackgroundEventArgs ^ e)
{
	//throw ref new Platform::NotImplementedException();
}

void scwin::App::OnLeavingBackground(Platform::Object ^ sender, Windows::ApplicationModel::LeavingBackgroundEventArgs ^ e)
{
	//throw ref new Platform::NotImplementedException();
}

/// <summary>
/// Invoked when Navigation to a certain page fails
/// </summary>
/// <param name="sender">The Frame which failed navigation</param>
/// <param name="e">Details about the navigation failure</param>
void App::OnNavigationFailed(Platform::Object ^sender, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs ^e)
{
    throw Exception::CreateException(E_FAIL, "Failed to load Page " + e->SourcePageType.Name);
}

void scwin::App::OnUnhandledException(Platform::Object ^ sender, Windows::UI::Xaml::UnhandledExceptionEventArgs ^ e)
{
	String^ msg = "Application reported an unhandled exception:- " + e->Message + ". Continuing to use the application may result in unpredicatble behavior. Please close and reopen the application.\n";
	msg += "HRESULT: " + e->Exception.Value;

	//using namespace Windows::UI::Popups;
	//MessageDialog^ dlg = ref new MessageDialog(
	//	msg,"Unhandled Error");
	//dlg->Commands->Append(ref new UICommand("Close Application"));
	//dlg->Commands->Append(ref new UICommand("Continue to use"));
	//dlg->ShowAsync();
	FileLoggingSessionScenario::Instance->LogCritical(msg);
}

void scwin::App::SetupInprocBackgroundTasks()
{
	auto aop = BackgroundExecutionManager::RequestAccessAsync();
	auto thentask = create_task(aop);
	thentask.then([this](BackgroundAccessStatus status)
	{
		auto builder = ref new BackgroundTaskBuilder();
		builder->Name = "Streetcat Inproc Background Tasks";
		builder->AddCondition(ref new SystemCondition(SystemConditionType::InternetAvailable | SystemConditionType::SessionConnected));
		builder->SetTrigger(ref new TimeTrigger(BACKGROUNDTASKTIMEOUT, false));
		taskReg = builder->Register();
	});

	

}

