#include "pch.h"
#include "SuspensionManager.h"
#include <functional>

using namespace Windows::Storage::Streams;
using namespace Windows::Data::Json;
using namespace concurrency;

Map<String^, Object^>^ Sg::SuspensionManager::sessionState = ref new Map<String^, Object^>();
Vector<Type^>^ Sg::SuspensionManager::knownTypes = ref new Vector<Type^>();
std::vector<WeakReference> Sg::SuspensionManager::registeredFrames;

DependencyProperty^ Sg::SuspensionManager::frameSessionStateKeyProperty = nullptr;
DependencyProperty^ Sg::SuspensionManager::frameSessionStateProperty = nullptr;

void Sg::SuspensionManager::RegisterDependencyProperties()
{
	if (frameSessionStateKeyProperty == nullptr)
	{
		frameSessionStateKeyProperty =
			DependencyProperty::RegisterAttached(L"_FrameSessionStateKey", String::typeid, SuspensionManager::typeid, nullptr);
	}
	if (frameSessionStateProperty == nullptr)
	{
		frameSessionStateProperty =
			DependencyProperty::RegisterAttached(L"_FrameSessionState", Map<String^, Object^>::typeid, SuspensionManager::typeid, nullptr);
	}
}

DependencyProperty^ Sg::SuspensionManager::FrameSessionStateKeyProperty::get()
{
	return frameSessionStateKeyProperty;
}

DependencyProperty^ Sg::SuspensionManager::FrameSessionStateProperty::get()
{
	return frameSessionStateProperty;
}

Map<String^, Object^>^ Sg::SuspensionManager::SessionState::get()
{
	return sessionState;
}

IAsyncAction ^ Sg::SuspensionManager::SaveAsync()
{
	for (auto weakFrameRef : registeredFrames)
	{
		Frame^ fr = weakFrameRef.Resolve<Frame>();
		if (fr != nullptr)
		{
			SaveFrameNavigationState(fr);
		}
	}

	// serialize the sesison state synchronously to avoid asynchronous access to s hared state.
	//InMemoryRandomAccessStream^ sessionData = ref new InMemoryRandomAccessStream();
	auto asyncresult = create_async([] {

	});
	return asyncresult;
}

IAsyncAction ^ Sg::SuspensionManager::RestoreAsync()
{
	auto asyncresult = create_async([] {

	});
	return asyncresult;
}

void Sg::SuspensionManager::RegisterFrame(Frame ^ frame, String ^ sessionStateKey)
{
	if (frame->GetValue(FrameSessionStateKeyProperty) != nullptr)
	{
		throw Exception::CreateException(E_FAIL, L"Frames can only be registered to one session state key.");
	}

	if (frame->GetValue(FrameSessionStateProperty) != nullptr)
	{
		throw Exception::CreateException(E_FAIL, L"Frames must be either be registered before accessing frame session state or not registered at all.");
	}

	frame->SetValue(FrameSessionStateKeyProperty, sessionStateKey);
	registeredFrames.emplace_back(frame);
	RestoreFrameNavigationState(frame);

}

void Sg::SuspensionManager::UnregisterFrame(Frame ^ frame)
{
	SessionState->Remove((String^)frame->GetValue(FrameSessionStateKeyProperty));

	using namespace std;
	remove_if(registeredFrames.begin(), registeredFrames.end(), [frame](WeakReference& x) 
	{
		auto f = x.Resolve<Frame>();
		return f == nullptr || f == frame;
	});
}

Map<String^, Object^>^ Sg::SuspensionManager::SessionStateForFrame(Frame ^ frame)
{
	auto frameState = dynamic_cast<Map<String^, Object^>^>(frame->GetValue(FrameSessionStateKeyProperty));
	if (!frameState)
	{
		auto frameSessionKey = dynamic_cast<String^>(frame->GetValue(FrameSessionStateKeyProperty));
		if (frameSessionKey)
		{
			if (!sessionState->HasKey(frameSessionKey))
			{
				auto dict = ref new Map<String^, Object^>();
				sessionState->Insert(frameSessionKey, dict);
			}
			frameState = (Map<String^, Object^>^)sessionState->Lookup(frameSessionKey);
		}
		else
		{
			frameState = ref new Map<String^, Object^>();
		}
		frame->SetValue(FrameSessionStateProperty, frameState);
	}
	return frameState;
}

void Sg::SuspensionManager::RestoreFrameNavigationState(Frame ^ frame)
{
	auto frameState = SessionStateForFrame(frame);
	if (frameState->HasKey(L"Navigation"))
	{
		frame->SetNavigationState((String^)frameState->Lookup(L"Navigation"));
	}
}

void Sg::SuspensionManager::SaveFrameNavigationState(Frame ^ frame)
{
	auto frameState = SessionStateForFrame(frame);
	if (frameState->HasKey(L"Navigation"))
	{
		frameState->Remove(L"Navigation");
	}
	frameState->Insert(L"Navigation", frame->GetNavigationState());
}
