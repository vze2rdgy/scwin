//
// Triangle.xaml.h
// Declaration of the Triangle class
//

#pragma once

#include "Triangle.g.h"

namespace sggraphs
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Triangle sealed
	{
		OpenGLES& openGles;
		EGLSurface renderSurface;
		concurrency::critical_section csForRenderSurface;

	public:
		Triangle();

	private:
		void OnVisibilityChanged(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::VisibilityChangedEventArgs ^args);
		void OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
		void OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
		void RecoverFromLostDevice();

	public:
		void MakeCurrent(bool yes);

	};
}
