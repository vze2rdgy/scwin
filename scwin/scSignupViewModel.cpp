#include "pch.h"
#include "scSignupViewModel.h"
#include "scLocalDataService.h"

using namespace scwin;
using namespace Windows::Security::Credentials;


scSignupViewModel::scSignupViewModel(scMainViewModel^ mainvm)
{
	this->Parent = mainvm;
	WeakReference wr(this);
	cmdSignIn = ref new Command(
		ref new ExecuteAction([wr](Object^ p)
	{
		auto ths = wr.Resolve<scSignupViewModel>();
		ths->Parent->FlagIsAuthenticated(nullptr, LandingContentStage::SignUp);
	}),
		ref new CanExecuteFunc([wr](Object^ p)
	{
		//auto ths = wr.Resolve<scSignupViewModel>();
		return true;
	})
	);

	cmdSignUp = ref new Command(
		ref new ExecuteAction([wr](Object^ p)
	{
		auto ths = wr.Resolve<scSignupViewModel>();
		ths->Parent->ClearMessages();
		if (StringHelpers::IsNullOrEmpty(ths->Email))
		{
			ths->Parent->SetStatusMessage(L"Email address is required.", StatusMessageTypes::Error);
			return;
		}
		if (StringHelpers::IsNullOrEmpty(ths->Password))
		{
			//TODO - Add min password requirements.
			ths->Parent->SetStatusMessage(L"Password is required.", StatusMessageTypes::Error);
			return;
		}
		if (StringHelpers::IsNullOrEmpty(ths->Company))
		{
			ths->Parent->SetStatusMessage(L"Company info is required.", StatusMessageTypes::Error);
			return;
		}
		if (StringHelpers::IsNullOrEmpty(ths->FullName))
		{
			ths->Parent->SetStatusMessage(L"Your full name is required.");
			return;
		}
		auto dataService = scLocalDataService::Instance();
		dataService->Signup(ths->Email, ths->Password, ths->Company, ths->FullName)->Completed =
			ref new AsyncOperationCompletedHandler<scwin::UserContext^>([wr](IAsyncOperation<scwin::UserContext^>^ operation, AsyncStatus status)
		{
			auto ths = wr.Resolve<scSignupViewModel>();
			if (status == AsyncStatus::Error)
			{
				auto code = operation->ErrorCode;
				auto ex = Exception::CreateException(code.Value);
				ths->Parent->SetStatusMessage(ex->Message);
			}
			else
			{
				try
				{
					auto userCtxt = operation->GetResults();
					ths->Parent->FlagIsAuthenticated(userCtxt, LandingContentStage::SignUp);
					operation->Close();
				}
				catch (Exception^ ex)
				{
					operation->Close();
					throw ex;
				}
			}
		});
	}),
		ref new CanExecuteFunc([wr](Object^ p)
	{
		auto ths = wr.Resolve<scSignupViewModel>();
		return !(StringHelpers::IsNullOrEmpty(ths->Email) ||
			StringHelpers::IsNullOrEmpty(ths->Password) ||
			StringHelpers::IsNullOrEmpty(ths->Company) ||
			StringHelpers::IsNullOrEmpty(ths->Email));
	}));

	email = L"Email Address";
}

void scSignupViewModel::NotifyPropertyChanged(String ^ propName)
{
	WeakReference wr(this);
	Windows::UI::Xaml::Window::Current->Dispatcher->RunAsync(
		Windows::UI::Core::CoreDispatcherPriority::Normal,
		ref new Windows::UI::Core::DispatchedHandler([wr, propName]()
	{
		auto ths = wr.Resolve<scSignupViewModel>();
		ths->PropertyChanged(ths, ref new PropertyChangedEventArgs(propName));
		ths->cmdSignUp->Refresh();
	})
	);
}



scSignupViewModel::~scSignupViewModel()
{
}

String^ scSignupViewModel::Email::get()
{
	return email;
}

void scSignupViewModel::Email::set(String^ val)
{
	email = val;
	NotifyPropertyChanged(L"Email");
}


String^ scSignupViewModel::Password::get()
{
	return pwd;
}

void scSignupViewModel::Password::set(String^ val)
{
	//TODO: Very important, password must not be saved in class member variable.
	// 
	pwd = val;
	NotifyPropertyChanged(L"Password");
}

String^ scSignupViewModel::FullName::get()
{
	return fullname;
}

void scSignupViewModel::FullName::set(String^ val)
{
	fullname = val;
	NotifyPropertyChanged(L"FullName");
}

String^ scSignupViewModel::Company::get()
{
	return company;
}

void scSignupViewModel::Company::set(String^ val)
{
	company = val;
	NotifyPropertyChanged(L"Company");
}

ICommand^ scSignupViewModel::SignInCommand::get()
{
	return cmdSignIn;
}

ICommand^ scSignupViewModel::SignUpCommand::get()
{
	return cmdSignUp;
}