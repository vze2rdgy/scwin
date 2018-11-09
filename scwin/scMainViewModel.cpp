#include "pch.h"
#include "scMainViewModel.h"
#include "scLocalDataService.h"
#include "DTODefaultLessComparer.h"

using namespace Windows::System::Profile;
using namespace scwin;
using namespace Windows::Security::Credentials;

scMainViewModel^ scMainViewModel::singleton = nullptr;

scMainViewModel^ scMainViewModel::Instance::get()
{
	if (singleton == nullptr)
	{
		//critical_section::scoped_lock g(gLock);
		std::lock_guard<std::recursive_mutex> g(gLock);
		if (singleton == nullptr)
		{
			singleton = ref new scMainViewModel();
		}
	}
	return singleton;
}

scMainViewModel::scMainViewModel()
{

	StatusMessages = ref new Vector<StatusMessage^>();

	// subscribe to pubsub
	Array <PubSubMessageIds>^ receivedMessageTypes = { PubSubMessageIds::All };
	PubSubService::Instance->Subscribe(this, ref new ReceivePubSubMessageCallback(this, &scMainViewModel::OnPubSubMessage), receivedMessageTypes);
	auto service = scLocalDataService::Instance();
	service->Connect(this);
	IsFirstTime = true;
	WeakReference wr(this);
	service->VerifySession()->Completed = 
		ref new AsyncOperationCompletedHandler<scwin::UserContext^>([wr](IAsyncOperation<scwin::UserContext^>^ operation, AsyncStatus status)
	{
		auto ths = wr.Resolve<scMainViewModel>();
		if (status == AsyncStatus::Error)
		{
			auto code = operation->ErrorCode;
			ths->SetStatusMessage("Failed to sign in. Please retry.", StatusMessageTypes::Error);
			ths->FlagIsAuthenticated(nullptr, LandingContentStage::SignIn);
			operation->Close();
			return;
		}
		try
		{
			auto userCtxt = operation->GetResults();
			ths->FlagIsAuthenticated(userCtxt, LandingContentStage::SignIn);
			operation->Close();
		}
		catch (Exception^ ex)
		{
			operation->Close();
			throw ex;
		}
	});

	SearchText = SEARCHPROMPT;

	Cart = ref new Vector<SalesOrder^>();
	Cart->VectorChanged += ref new Windows::Foundation::Collections::VectorChangedEventHandler<scwin::SalesOrder ^>(this, &scwin::scMainViewModel::OnVectorChanged);
	Cart->Append(ref new SalesOrder());
}

scMainViewModel::~scMainViewModel()
{
	PubSubService::Instance->Unsubscribe(this);
	auto service = scLocalDataService::Instance();
	service->Disconnect();
}

void scMainViewModel::Logout()
{
	WeakReference wr(this);
	scLocalDataService::Instance()->SignOut()->Completed =
		ref new AsyncOperationCompletedHandler<bool>([wr](IAsyncOperation<bool>^ operation, AsyncStatus status)
	{
		auto ths = wr.Resolve<scMainViewModel>();
		if (status == AsyncStatus::Error)
		{
			auto code = operation->ErrorCode;
			operation->Close();
			throw Exception::CreateException(code.Value);
		}
		try
		{
			ths->FlagIsAuthenticated(nullptr, LandingContentStage::SignIn);
			operation->Close();
		}
		catch (Exception^ ex)
		{
			operation->Close();
			throw ex;
		}
	});
}

String^ scMainViewModel::PageTitle::get()
{
	return pageTitle;
}

void scMainViewModel::PageTitle::set(String^ val)
{
	pageTitle = val;
	NotifyPropertyChanged(PROPERTYNAME(PageTitle));
}

IObservableVector<Object^>^ scMainViewModel::BreadCrumbs::get()
{
	if (breadCrumbs == nullptr)
		std::lock_guard<std::recursive_mutex> g(GlobalMutex);
		if (breadCrumbs == nullptr)
			breadCrumbs = ref new Vector<Object^>();
	return breadCrumbs;
}

void scwin::scMainViewModel::UpdateUser(UserProfile ^ profile)
{
	throw ref new Platform::NotImplementedException();
}

IAsyncOperation<scwin::UserContext^>^ scwin::scMainViewModel::GetUser()
{
	throw ref new NotImplementedException();
}

void scwin::scMainViewModel::ChangePassword(String ^ oldPwd, String ^ newPwd)
{
	throw ref new Platform::NotImplementedException();
}

IVector<String^>^ scwin::scMainViewModel::GetProductAttributeTypes()
{

	auto list = ref new Vector<String^>(
	{ ATTR_CATEGORICAL,
		ATTR_LABEL,
		ATTR_METRIC,
		ATTR_MISC,
		ATTR_ORDINAL,
		ATTR_PERIOD,
		ATTR_QUALITATIVE });

	return list;

}


UserContext^ scMainViewModel::UserContext::get()
{
	return this->userContext;
}

void scwin::scMainViewModel::FlagIsAuthenticated(scwin::UserContext ^ usrCtx, LandingContentStage defaultStage)
{
	if (this->userContext)
	{
		userContext = nullptr;
	}
	this->userContext = usrCtx;
	this->IsAuthorized = usrCtx != nullptr && usrCtx->Name != nullptr && !usrCtx->Name->IsEmpty();
	if (this->IsAuthorized)
	{
		this->AuthenticationStage = LandingContentStage::Wait;
	}
	else
	{
		this->AuthenticationStage = defaultStage;
		switch (defaultStage)
		{
		case scwin::LandingContentStage::SignIn:
			break;
		case scwin::LandingContentStage::SignUp:
			break;
		}
	}
	NotifyPropertyChanged(L"UserContext");
}

void scwin::scMainViewModel::OnPubSubMessage(Object ^ sender, PubSubMessageIds msgId, Object ^ data)
{
	switch (msgId)
	{
	case PubSubMessageIds::ApplicationInfoStatusMessage:
	case PubSubMessageIds::ApplicationErrorStatusMessage:
		if (data)
		{
			auto str = dynamic_cast<String^>(data);
			if (str)
			{
				if (PubSubMessageIds::ApplicationErrorStatusMessage == msgId)
					SetStatusMessage(str, StatusMessageTypes::Error);
				else 
					SetStatusMessage(str);
			}
		}
		break;
	}
}

void scwin::scMainViewModel::SetStatusMessage(String ^ message, StatusMessageTypes type)
{
	if (this->StatusMessages == nullptr)
		this->StatusMessages = ref new Vector<StatusMessage^>();
	WeakReference wr(this);
	Dispatcher->RunAsync(CoreDispatcherPriority::Normal, 
		ref new DispatchedHandler([wr, message, type]()
	{
		auto ths = wr.Resolve<scMainViewModel>();

		auto v = to_vector(ths->StatusMessages);
		auto pos = std::find_if(v.cbegin(), v.cend(), [message](StatusMessage^ m)
		{
			return m->Message == message;
		});

		if (pos != v.cend())
		{
			ths->StatusMessages->RemoveAt(pos - v.cbegin());
		}
		ths->StatusMessages->Append(ref new StatusMessage(message, type));
		if (type == StatusMessageTypes::Error)
		{
			ths->LastError = message;
			ths->NotifyPropertyChanged("LastError");
		}
	}));
}

void scwin::scMainViewModel::ClearMessages()
{
	if (this->StatusMessages != nullptr)
	{
		WeakReference wr(this);
		Dispatcher->RunAsync(
			CoreDispatcherPriority::Normal,
			ref new Windows::UI::Core::DispatchedHandler([wr]()
		{
			auto ths = wr.Resolve<scMainViewModel>();
			ths->StatusMessages->Clear();
			ths->LastError = "";
			ths->NotifyPropertyChanged("LastError");
		}));
	}
}

void scMainViewModel::NotifyPropertyChanged(String ^ propName, CoreDispatcherPriority priority)
{
	if (Dispatcher->HasThreadAccess)
	{
		PropertyChanged(this, ref new PropertyChangedEventArgs(propName));
	}
	else
	{
		WeakReference wr(this);
		Dispatcher->RunAsync(
			priority,
			ref new Windows::UI::Core::DispatchedHandler([wr, propName]()
		{
			auto ths = wr.Resolve<scMainViewModel>();
			ths->PropertyChanged(ths, ref new PropertyChangedEventArgs(propName));
		})
		);
	}
}

//void scwin::scMainViewModel::ExecuteSearchCommand(Object ^ data)
//{
//	PubSubMessage^ searchMessage = ref new PubSubMessage(this, PubSubMessageIds::SearchMessage, ref new SearchData(SearchContexts::Contextual, searchText));
//	PubSubService::Instance->Publish(searchMessage);
//}
//
//bool scwin::scMainViewModel::CanExecuteSearchCommand(Object ^ data)
//{
//	return !StringHelpers::IsNullOrEmpty(searchText);
//}

scwin::http::ServerStatus scMainViewModel::ConnectionStatus::get()
{
	return scwin::http::ServerStatus::Disconnected;
}

bool scMainViewModel::IsAuthorized::get()
{
	return this->isAuthorized;
}

void scMainViewModel::IsAuthorized::set(bool val)
{
	this->isAuthorized = val;
	NotifyPropertyChanged("IsAuthorized");
}

LandingContentStage scMainViewModel::AuthenticationStage::get()
{
	return this->authStatus;
}

void scMainViewModel::AuthenticationStage::set(LandingContentStage status)
{
	authStatus = status;
	NotifyPropertyChanged("AuthenticationStage");
}

void scMainViewModel::DisplayProgress::set(Visibility val)
{
	displayProgress = val;
	this->NotifyPropertyChanged(L"DisplayProgress", CoreDispatcherPriority::Low);
}

String^ scMainViewModel::SearchText::get()
{
	return searchText;
}

void scMainViewModel::SearchText::set(String^ text)
{
	searchText = text;
	//SearchCommand->Refresh(searchText);
	//NotifyPropertyChanged("SearchText");
	PubSubMessage^ searchMessage = ref new PubSubMessage(this, PubSubMessageIds::SearchMessage, ref new SearchData(SearchContexts::Contextual, searchText));
	PubSubService::Instance->Publish(searchMessage);
}

///////////// Api Properties ///////////////////////

String^ scMainViewModel::Version::get()
{
	auto pack = Package::Current;
	auto version = pack->Id->Version;
	String^ ver = version.Major + L"." + version.Minor + L"." + version.Build;
	return L"v" + ver;
}

////////////// Nested view models ////////////////////////
scSignInViewModel^ scMainViewModel::SignInViewModel::get()
{
	// initialize signvm with necessary inputs
	if (signInVm == nullptr)
		signInVm = ref new scSignInViewModel(this);
	return signInVm;
}

scSignupViewModel^ scMainViewModel::SignUpViewModel::get()
{
	// initialize signvm with necessary inputs
	if (signUpVm == nullptr)
		signUpVm = ref new scSignupViewModel(this);
	return signUpVm;
}

///////////// Commands ///////////////////////



void scwin::scMainViewModel::OnVectorChanged(Windows::Foundation::Collections::IObservableVector<scwin::SalesOrder ^> ^sender, Windows::Foundation::Collections::IVectorChangedEventArgs ^event)
{
	CartSize = Cart->Size;
	NotifyPropertyChanged("CartSize");
}
