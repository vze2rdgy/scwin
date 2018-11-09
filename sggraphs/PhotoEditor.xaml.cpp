//
// PhotoEditor.xaml.cpp
// Implementation of the PhotoEditor class
//

#include "pch.h"
#include "PhotoEditor.xaml.h"

using namespace sggraphs;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

PhotoEditor::PhotoEditor()
{
	InitializeComponent();
}


void sggraphs::PhotoEditor::Slider_DragStarting(Windows::UI::Xaml::UIElement^ sender, Windows::UI::Xaml::DragStartingEventArgs^ args)
{

}
