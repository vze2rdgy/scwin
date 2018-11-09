//
// ViewToEditorView.h
// Declaration of the ViewToEditorView class.
//

#pragma once

namespace scwin
{
	public ref class ViewToEditorView sealed : public Windows::UI::Xaml::Controls::ContentControl
	{
		Button^ btnEdit;
		Button^ btnCancel;
		Button^ btnSave;

		static DependencyProperty^ editModeProperty;

	public:
		ViewToEditorView();
		static void RegisterDependencyProperties();

		static property DependencyProperty^ EditModeProperty
		{
			DependencyProperty^ get() { return editModeProperty; }
		}

		property UIEditMode EditMode
		{
			UIEditMode get() { return (UIEditMode)GetValue(editModeProperty); }
			void set(UIEditMode val) { SetValue(editModeProperty, val); }
		}

	protected:
		virtual void OnApplyTemplate() override;
		void OnClick(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
	};
}
