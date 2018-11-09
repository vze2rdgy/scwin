#include "pch.h"
#include "scSignInViewModel.h"
#include "scLocalDataService.h"

using namespace Windows::Security::Credentials;

scSignInViewModel::scSignInViewModel(scMainViewModel^ mainvm)
{
	Parent = mainvm;
	WeakReference wr(this);
	cmdSignIn = ref new Command(
		ref new ExecuteAction([wr](Object^ p)
		{
			auto ths = wr.Resolve<scSignInViewModel>();
			ths->Parent->ClearMessages();
			if (ths->Email == nullptr ||
				ths->Email->Length() == 0)
			{
				ths->Parent->SetStatusMessage(L"Email address is required.", StatusMessageTypes::Error);
				return;
			}
			if (ths->Password == nullptr ||
				ths->Password->Length() == 0)
			{
				//TODO - Add min password requirements.
				ths->Parent->SetStatusMessage(L"Password is required.", StatusMessageTypes::Error);
				return;
			}
			auto dataService = scLocalDataService::Instance();
			dataService->Signin(ths->Email, ths->Password)->Completed =
				ref new AsyncOperationCompletedHandler<scwin::UserContext^>([wr](IAsyncOperation<scwin::UserContext^>^ operation, AsyncStatus status)
			{
				auto ths = wr.Resolve<scSignInViewModel>();
				if (status == AsyncStatus::Error)
				{
					auto code = operation->ErrorCode;
					auto ex= Exception::CreateException(code.Value, ref new String(Messages[(int)HRESULT_CODE(code.Value)]));
					ths->Parent->SetStatusMessage(ex->Message, StatusMessageTypes::Error);
				}
				else
				{
					try
					{
						auto userCtxt = operation->GetResults();
						ths->Parent->FlagIsAuthenticated(userCtxt, LandingContentStage::SignIn);
						operation->Close();
					}
					catch (Exception^ ex)
					{
						operation->Close();
						throw ex;
					}
				}
			});
		}
	),
		ref new CanExecuteFunc([wr](Object^ p)
		{
			auto ths = wr.Resolve<scSignInViewModel>();
			return !(StringHelpers::IsNullOrEmpty(ths->Email) || StringHelpers::IsNullOrEmpty(ths->Password));
		}
	)
	);

	cmdSignUp = ref new Command(
		ref new ExecuteAction([wr](Object^ p)
	{
		auto ths = wr.Resolve<scSignInViewModel>();
		ths->Parent->FlagIsAuthenticated(nullptr, LandingContentStage::SignUp);
	}),
		ref new CanExecuteFunc([](Object^ p)
	{
		return true;
	})
		);
}


scSignInViewModel::~scSignInViewModel()
{
}

void scSignInViewModel::NotifyPropertyChanged(String ^ propName)
{
	WeakReference wr(this);
	Windows::UI::Xaml::Window::Current->Dispatcher->RunAsync(
		Windows::UI::Core::CoreDispatcherPriority::Normal,
		ref new Windows::UI::Core::DispatchedHandler([wr,propName]()
	{
		auto ths = wr.Resolve<scSignInViewModel>();
		ths->PropertyChanged(ths, ref new PropertyChangedEventArgs(propName));
		ths->cmdSignIn->Refresh();
	})
	);
}


String^ scSignInViewModel::Email::get()
{
	return email;
}

void scSignInViewModel::Email::set(String^ val)
{
	email = val;
	NotifyPropertyChanged("Email");
}

String^ scSignInViewModel::Password::get()
{
	return pwd;
}

void scSignInViewModel::Password::set(String^ val)
{
	pwd = val;
	NotifyPropertyChanged("Password");
}

bool scSignInViewModel::RememberMe::get()
{
	return remMe;
}

void scSignInViewModel::RememberMe::set(bool val)
{
	remMe = val;
	NotifyPropertyChanged("RememberMe");
}

ICommand^ scSignInViewModel::SignInCommand::get()
{
	return cmdSignIn;
}

ICommand^ scSignInViewModel::SignUpCommand::get()
{
	return cmdSignUp;
}