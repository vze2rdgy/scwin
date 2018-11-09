//
// Bars.xaml.h
// Declaration of the Bars class
//

#pragma once

#include "Bars.g.h"
#include <angle_windowsstore.h>

namespace sggraphs
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Bars sealed
	{
	private:
		EGLSurface renderSurface;
		concurrency::critical_section csForRenderSurface;
		IAsyncAction^ renderLoopWorker;
		OpenGLES& openGles;

	private:

		void OnPageLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnPageUnloaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		void OnVisibilityChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ args);

		void CreateRenderSurface();

		void DestroyRenderSurface();

		void RecoverFromLostDevice();

		void StartRenderLoop();

		void StopRenderLoop();

	public:
		Bars();
		virtual ~Bars();

		void MakeCurrent(bool yes);
	};
}
