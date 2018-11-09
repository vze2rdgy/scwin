//
// ProductEditDetails.xaml.h
// Declaration of the ProductEditDetails class
//

#pragma once

#include "GlyphConverter.h"
#include "BooleanToVisibilityConverter.h"
#include "CommandStateToVisibilityConverter.h"
#include "ProductEditDetails.g.h"
#include "SharedViewModels.h"

using namespace TreeViewControl;

namespace scwin
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class ProductEditDetails sealed : INotifyPropertyChanged
	{
		EventRegistrationToken tokViewModelPropChanged;

	public:
		ProductEditDetails();

		property ProductViewModel^ ViewModel;


		void OnEditModeChanged(UIEditMode mode);

		void StartEditing();
		void EndEditing();

		virtual event PropertyChangedEventHandler^ PropertyChanged;

	protected:
		virtual void OnApplyTemplate() override;

	private:
		void btnEdit_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void btnSave_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void btnCancel_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
		void OnSizeChanged(Platform::Object ^sender, Windows::UI::Xaml::SizeChangedEventArgs ^e);
		void treeCategories_TreeViewItemClick(TreeViewControl::TreeView^ sender, TreeViewControl::TreeViewItemClickEventArgs^ args);
		void treeCategories_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void foSelectCategory_Opening(Platform::Object^ sender, Platform::Object^ e);
		void LoadNodeChildren(TreeNode^ owner, IObservableVector<ProductCategory^>^ categories);
		void CheckBox_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void btnFlyoutClose_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnPropertyChanged(Platform::Object ^sender, Windows::UI::Xaml::Data::PropertyChangedEventArgs ^e);
		void CategoryItemDelete_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
	};
}
