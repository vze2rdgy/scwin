//
// pch.h
// Header for standard system include files.
//

#pragma once

#include <collection.h>
#include <ppltasks.h>
#include <concurrent_vector.h>
#include <WindowsNumerics.h>

#include "App.xaml.h"

#define COUCHDBVER 1

#if COUCHDBVER == 2
#define CDBHOST	"http://streetcat.smartgazellc.com:6000/"
#else
#define CDBHOST	"http://streetcat.smartgazellc.com:5000/"
#endif

#define LOCKTIMEOUT	100	
#define BACKGROUNDTASKTIMEOUT 15	// interval used for verifying the data base connection status.
	// TODO: depending on device type, we may need different interval settings (on battery, powered, etc)
#define REPLCHECKTIMER 1000

using namespace Platform; //https://msdn.microsoft.com/en-us/library/hh710417.aspx
using namespace Platform::Metadata;
using namespace concurrency;	//https://msdn.microsoft.com/en-us/library/dd492819.aspx
//UWP namespace documentation : https://docs.microsoft.com/en-us/uwp/api/
using namespace Windows::Data::Json;
using namespace Windows::Foundation;
using namespace Windows::Web::Http;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::ViewManagement;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Background;
using namespace Windows::ApplicationModel::Activation;
using namespace Concurrency;
using namespace Windows::Foundation::Diagnostics;

//extern critical_section gLock;
extern std::recursive_mutex gLock;

#define GUARDEDGLOCK std::lock_guard<std::recursive_mutex> g(gLock)

#include "ScopeComplete.h"
#include "AppProperty.h"
#include "SysInfoHelpers.h"
#include "Delegates.h"
#include "Command.h"
#include "StringHelpers.h"
#include "HttpHelpers.h"
#include "scMainViewModel.h"
#include "Enums.h"
#include "App.xaml.h"
#include "NavMenuItem.h"
#include "dto.h"
#include "SearchContexts.h"

using namespace scwin;
using namespace scwin::utils;

extern CoreDispatcher^ AppDispatcher;
extern scMainViewModel^ mainViewModel;


/********************** MACROS *********************/

#define DECLARERAISEPROPCHANGED void RaisePropertyChanged(String^ propName)

#define DEFINERAISEPROPCHANGEDBASIC(type) \
	void scwin::##type::RaisePropertyChanged(String ^ propName) \
	{ \
		if (AppDispatcher->HasThreadAccess) \
		{ \
			PropertyChanged(this, ref new PropertyChangedEventArgs(propName)); \
			/*OutputDebugString(L"Property "); OutputDebugString(propName->Data()); OutputDebugStringA(" called in Dispatcher Thread.\n");*/ \
			return ; \
		} \
		WeakReference wr(this); \
		AppDispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([wr, propName]() \
		{ \
			auto ths = wr.Resolve<##type>(); \
			if (ths != nullptr)  \
			{ \
				ths->PropertyChanged(ths, ref new PropertyChangedEventArgs(propName)); \
				/*OutputDebugString(L"Property "); OutputDebugString(propName->Data()); OutputDebugStringA(" routed to Dispatcher Thread.\n");*/ \
			} \
		})); \
	}

#define DEFINERAISEPROPCHANGEDSELF(type) \
	void scwin::##type::RaisePropertyChanged(String ^ propName) \
	{ \
		if (Dispatcher->HasThreadAccess) \
		{ \
			PropertyChanged(this, ref new PropertyChangedEventArgs(propName)); \
			/*OutputDebugString(L"Property "); OutputDebugString(propName->Data()); OutputDebugStringA(" called in Dispatcher Thread.\n");*/ \
			return ; \
		} \
		WeakReference wr(this); \
		Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([wr, propName]() \
		{ \
			auto ths = wr.Resolve<##type>(); \
			if (ths != nullptr)  \
			{ \
				ths->PropertyChanged(ths, ref new PropertyChangedEventArgs(propName)); \
				/*OutputDebugString(L"Property "); OutputDebugString(propName->Data()); OutputDebugStringA(" routed to Dispatcher Thread.\n");*/ \
			} \
		})); \
	}


#define DEFINERAISEPROPCHANGED(type) \
	void scwin::##type::RaisePropertyChanged(String ^ propName) \
	{ \
		if (!CanRaiseEvents) return ; \
		this->ObjectState = this->ObjectState == DataState::Unmodified ? DataState::Edited : this->ObjectState; \
		if (AppDispatcher->HasThreadAccess) \
		{ \
			PropertyChanged(this, ref new PropertyChangedEventArgs(propName)); \
			return ; \
		} \
		WeakReference wr(this); \
		AppDispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([wr, propName]() \
		{ \
			auto ths = wr.Resolve<##type>(); \
			if (ths != nullptr) ths->PropertyChanged(ths, ref new PropertyChangedEventArgs(propName)); \
		})); \
	}

extern char16** const Messages;

extern inline void ThrowException(MessageIds id);
extern inline Exception^ CreateException(MessageIds id);
extern inline char16* GetMessage(MessageIds id);
extern inline void LogException(LPCSTR);
extern inline void LogException(MessageIds id);
extern inline MessageIds LogRetException(MessageIds id);
extern inline MessageIds LogRetException(MessageIds id, LPCSTR message);

extern std::recursive_mutex GlobalMutex;

#define LOGMSG(msg) OutputDebugString(msg);
#define LOGMSGA(msg) OutputDebugStringA(msg);

#define APPTITLE	"Streetcat"

#define SEARCHPROMPT L"Enter text to search..."


#define ADMINPIVOTPRODUCTSINDEX 3


#define GOTOVIEWSTATE(ctl, statename) \
{ \
	auto b = VisualStateManager::GoToState(ctl, statename, true); \
	OutputDebugString(L"VisualStateManager::GoToState(ctl, statename, true)"); \
	OutputDebugString(L"="); \
	OutputDebugString(b.ToString()->Data()); \
	OutputDebugString(L"\n"); \
}

#define ATTR_CATEGORICAL "Categorical"
#define ATTR_QUALITATIVE "Qualitative"
#define ATTR_METRIC "Metric"
#define ATTR_ORDINAL "Ordinal"
#define ATTR_LABEL "Label"
#define ATTR_PERIOD "Period"
#define ATTR_MISC "Misc"



#include "typedefinitions.h"

