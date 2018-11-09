#pragma once

#include <collection.h>
#include <ppltasks.h>

using namespace ::Platform;
using namespace ::Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

namespace Sg
{
	[Windows::Foundation::Metadata::WebHostHidden]
	ref class SuspensionManager sealed
	{
	private:
		static Map<String^, Object^>^ sessionState;
		static Vector<Type^>^ knownTypes;
		const String^ sessionStateFileName = L"sessionState.xml";
		static DependencyProperty^ frameSessionStateKeyProperty;
		static DependencyProperty^ frameSessionStateProperty;
		static std::vector<WeakReference> registeredFrames;

	private:
		static property DependencyProperty^ FrameSessionStateKeyProperty
		{
			DependencyProperty^ get();
		}

		static property DependencyProperty^ FrameSessionStateProperty
		{
			DependencyProperty^ get();
		}

	internal:
		static property Map<String^, Object^>^ SessionState
		{
			Map<String^, Object^>^ get();
		}

		static property Vector<String^>^ KnownTypes
		{
			Vector<String^>^ get();
		}

		static Map<String^, Object^>^ SessionStateForFrame(Frame^ fame);

	public:

		static IAsyncAction^ SaveAsync();

		static IAsyncAction^ RestoreAsync();

		static void RegisterDependencyProperties();

		static void RegisterFrame(Frame^ frame, String^ sessionStateKey);

		static void UnregisterFrame(Frame^ fame);


		static void RestoreFrameNavigationState(Frame^ frame);

		static void SaveFrameNavigationState(Frame^ frame);

	};

}