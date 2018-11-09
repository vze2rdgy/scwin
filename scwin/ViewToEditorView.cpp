//
// ViewToEditorView.cpp
// Implementation of the ViewToEditorView class.
//

#include "pch.h"
#include "ViewToEditorView.h"

using namespace scwin;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Documents;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;

// The Templated Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234235

DependencyProperty^ scwin::ViewToEditorView::editModeProperty = nullptr;

ViewToEditorView::ViewToEditorView()
{
	DefaultStyleKey = "scwin.ViewToEditorView";
}

void scwin::ViewToEditorView::RegisterDependencyProperties()
{
	if (editModeProperty == nullptr)
		editModeProperty = DependencyProperty::Register("EditMode", UIEditMode::typeid, ViewToEditorView::typeid, ref new PropertyMetadata(UIEditMode::Unmodified));
}

void scwin::ViewToEditorView::OnApplyTemplate()
{
	ContentControl::OnApplyTemplate();
	btnEdit = (Button^)GetTemplateChild("btnEdit");
	btnSave = (Button^)GetTemplateChild("btnSave");
	btnCancel = (Button^)GetTemplateChild("btnCancel");

	btnEdit->Click += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::ViewToEditorView::OnClick);
	btnSave->Click += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::ViewToEditorView::OnClick);
	btnCancel->Click += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::ViewToEditorView::OnClick);
}


void scwin::ViewToEditorView::OnClick(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	Button^ btn = safe_cast<Button^>(sender);
	if (ReferenceEquals(btn, btnEdit))
	{
		EditMode = UIEditMode::Edit;
		VisualStateManager::GoToState(this, "EditViewState", true);
	}
	else if (ReferenceEquals(btn, btnSave))
	{
		EditMode = UIEditMode::Save;
		VisualStateManager::GoToState(this, "ViewViewState", true);
	}
	else if (ReferenceEquals(btn, btnCancel))
	{
		EditMode = UIEditMode::Cancel;
		VisualStateManager::GoToState(this, "ViewViewState", true);
	}
}
