#pragma once

#include "StartupEventArgs.h"
#include "scLocalDataService.h"
#include "scSignInViewModel.h"
#include "scSignupViewModel.h"
#include "UserProfile.h"
#include "PubSubService.h"
#include "AdminViewModel.h"

namespace scwin 
{

	[Bindable]
	public ref class scMainViewModel sealed : DependencyObject, INotifyPropertyChanged
	{

		friend ref class scSignInViewModel;
		friend ref class scSignupViewModel;

	private:
		static scMainViewModel^ singleton;
		IBackgroundTrigger^ timedTrigger;
		LandingContentStage authStatus = LandingContentStage::SignIn;
		bool isAuthorized = false;
		scwin::UserContext^ userContext;
		scSignInViewModel^ signInVm;
		scSignupViewModel^ signUpVm;
		AdminViewModel^ adminVm;
		double progressBarProgress = 0.;
		Visibility displayProgress = Visibility::Collapsed;
		String^ searchText;
		bool dataPrepared;
		String^ pageTitle;
		IObservableVector<Object^>^ breadCrumbs;

	private:
		scMainViewModel();

		void FlagIsAuthenticated(scwin::UserContext^ usrCtx, LandingContentStage defaultStage = LandingContentStage::SignIn);

		void OnPubSubMessage(Object^ sender, PubSubMessageIds msgIds, Object^ data);

	internal:
		void SetStatusMessage(String^ message, StatusMessageTypes type = StatusMessageTypes::Information);

		void ClearMessages();

	protected:
		void NotifyPropertyChanged(String^ propName, CoreDispatcherPriority priority = CoreDispatcherPriority::Normal);

	public:
		virtual event PropertyChangedEventHandler^ PropertyChanged;

	public:

		virtual ~scMainViewModel();

		static property scMainViewModel^ Instance
		{
			scMainViewModel^ get();
		}

		property String^ PageTitle
		{
			String^ get();
			void set(String^);
		}

		property IObservableVector<Object^>^ BreadCrumbs
		{
			IObservableVector<Object^>^ get();
		}

		property bool DataPrepared
		{
			bool get() { return dataPrepared; }
			void set(bool val)
			{
				dataPrepared = val;
				this->NotifyPropertyChanged(PROPERTYNAME(DataPrepared), CoreDispatcherPriority::High);
			}
		}

		property bool IsAuthorized
		{
			bool get();
			void set(bool val);
		}

		property bool IsFirstTime;

		property scwin::UserContext^ UserContext
		{
		public:
			scwin::UserContext^ get();
		}

		property scwin::http::ServerStatus ConnectionStatus
		{
			scwin::http::ServerStatus get();
		}

		property LandingContentStage AuthenticationStage
		{
			LandingContentStage get();
			void set(LandingContentStage val);
		}

		property String^ LastError;

		property IVector<StatusMessage^>^ StatusMessages;

		property String^ SearchText
		{
			String^ get();
			void set(String^ text);
		}

		/// Returns real-time instances of signin view model.
		/// When caller stops using it, it is garbage collected.
		property scSignInViewModel^ SignInViewModel
		{
			scSignInViewModel^ get();
		}

		property scSignupViewModel^ SignUpViewModel
		{
			scSignupViewModel^ get();
		}

		property AdminViewModel^ AdminVM
		{
			AdminViewModel^ get() 
			{ 
				if (adminVm == nullptr)
				{
					GUARDEDGLOCK;
					if (adminVm == nullptr)
					{
						adminVm = ref new AdminViewModel(this);
					}
				}
				return this->adminVm; 
			}
		}

		property String^ Version
		{
			String^ get();
		}

		property double ProgressBarProgress
		{
			double get() { return progressBarProgress; }
			void set(double val)
			{
				if (progressBarProgress != val)
				{
					progressBarProgress = val;
					this->NotifyPropertyChanged(L"ProgressBarProgress", CoreDispatcherPriority::High);
				}
			}
		}

		property Visibility DisplayProgress
		{
			Visibility get()
			{
				return this->displayProgress;
			}
			void set(Visibility val);
		}

		void Logout();

		void UpdateUser(UserProfile^ profile);

		IAsyncOperation<scwin::UserContext^>^ GetUser();

		void ChangePassword(String^ oldPwd, String^ newPwd);


		static IVector<String^>^ GetProductAttributeTypes();

		property IObservableVector<SalesOrder^>^ Cart;

		property int CartSize;

		void OnVectorChanged(Windows::Foundation::Collections::IObservableVector<scwin::SalesOrder ^> ^sender, Windows::Foundation::Collections::IVectorChangedEventArgs ^event);
};

}
