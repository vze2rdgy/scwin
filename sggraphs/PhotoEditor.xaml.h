//
// PhotoEditor.xaml.h
// Declaration of the PhotoEditor class
//

#pragma once

#include "PhotoEditor.g.h"

namespace sggraphs
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class PhotoEditor sealed
	{
	public:
		PhotoEditor();
	private:
		void Slider_DragStarting(Windows::UI::Xaml::UIElement^ sender, Windows::UI::Xaml::DragStartingEventArgs^ args);
	};
}
