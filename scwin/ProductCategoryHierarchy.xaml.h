//
// ProductCategoryHierarchy.xaml.h
// Declaration of the ProductCategoryHierarchy class
//

#pragma once

#include "GlyphConverter.h"
#include "BooleanToVisibilityConverter.h"
#include "CommandStateToVisibilityConverter.h"
#include "ProductCategory.h"
#include "ProductCategoryHierarchy.g.h"

namespace scwin
{
	[Bindable]
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class ProductCategoryHierarchy sealed 
	{
		static DependencyProperty^ isInEditProperty;

	public:
		static void RegisterDependencyProperties();

		ProductCategoryHierarchy();

		void OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);

		property Command^ AddCategory;
		property Command^ EditCategory;
		property Command^ DeleteCategory;
		property Command^ SaveCategory;
		property Command^ CancelCategory;
		
		static property DependencyProperty^ IsInEditProperty
		{
			DependencyProperty^ get()
			{
				return isInEditProperty;
			}
		}
		
		property bool IsInEdit
		{
		public:
			bool get()
			{
				auto boxed = (IBox<Boolean>^)GetValue(isInEditProperty);
				if (boxed != nullptr) 
					return boxed->Value;
				return false;
			}
		private:
			void set(bool val)
			{
				SetValue(isInEditProperty, val);
			}
		}

	private:
		void InitializeCommands();

		void FetchProductInventories();

		static void IsInEditPropertyChanged(DependencyObject^ d, DependencyPropertyChangedEventArgs^ prop);

		void BindData();

		void ExcecuteAddCategory(Object^ p);
		bool CanExecuteAddCategory(Object^ p);
		void ExcecuteEditCategory(Object^ p);
		bool CanExecuteEditCategory(Object^ p);
		void ExcecuteDeleteCategory(Object^ p);
		void DeleteCommandAction(Windows::UI::Popups::IUICommand^ cmd);
		bool CanExecuteDeleteCategory(Object^ p);
		void ExcecuteSaveCategory(Object^ p);
		bool CanExecuteSaveCategory(Object^ p);
		void ExcecuteCancelCategory(Object^ p);
		bool CanExecuteCancelCategory(Object^ p);

		void LoadNodeChildren(TreeViewControl::TreeNode^ owner, IObservableVector<ProductCategory^>^ categories);
		void HideShowNodesWhenSearched(String^ searched, Windows::Foundation::Collections::IVector<Platform::Object^>^ items);

		void OnContainerContentChanging(Windows::UI::Xaml::Controls::ListViewBase ^sender, Windows::UI::Xaml::Controls::ContainerContentChangingEventArgs ^args);
		void OnSizeChanged(Platform::Object ^sender, Windows::UI::Xaml::SizeChangedEventArgs ^e);
		void OnOrientationChanged(Windows::Graphics::Display::DisplayInformation ^sender, Platform::Object ^args);
		void treeCategories_TreeViewItemClick(TreeViewControl::TreeView^ sender, TreeViewControl::TreeViewItemClickEventArgs^ args);
		void categoryTreeViewItemContextMenu_Opening(Platform::Object^ sender, Platform::Object^ e);
		void treeCategories_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void txtSearchCategory_TextChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e);

		void UpdateButtonStates();
		void cmenuAddCategory_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void cmenuDeleteCategory_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void cmenuEditCategory_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
