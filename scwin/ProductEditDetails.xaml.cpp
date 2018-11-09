//
// ProductEditDetails.xaml.cpp
// Implementation of the ProductEditDetails class
//

#include "pch.h"
#include "ProductEditDetails.xaml.h"
#include "cpplinq.hpp"

using namespace scwin;

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

ProductEditDetails::ProductEditDetails()
{
	InitializeComponent();

	Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::ProductEditDetails::OnLoaded);
	Unloaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::ProductEditDetails::OnUnloaded);
	SizeChanged += ref new Windows::UI::Xaml::SizeChangedEventHandler(this, &scwin::ProductEditDetails::OnSizeChanged);

}
void scwin::ProductEditDetails::OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	tokViewModelPropChanged = ViewModel->PropertyChanged += ref new Windows::UI::Xaml::Data::PropertyChangedEventHandler(this, &scwin::ProductEditDetails::OnPropertyChanged);
	ViewModel->LoadProductDataForPanel(0);
}



void scwin::ProductEditDetails::OnEditModeChanged(UIEditMode mode)
{

}

void scwin::ProductEditDetails::StartEditing()
{
	if (ViewModel->EditPanel == 0)
	{
		VisualStateManager::GoToState(this, "EditViewState", true);
		// tell view model that this panel is being edited
		ViewModel->BeginEdit();
		// by default enable first panel for editing
		ViewModel->ProductPanels->GetAt(0)->IsExpanded = true;

		auto ctl = (Control^)FocusManager::FindFirstFocusableElement(gridproductEditable);
		//assert(ctl != nullptr);
		if(ctl)
			ctl->Focus(Windows::UI::Xaml::FocusState::Programmatic);
	}
}

void scwin::ProductEditDetails::EndEditing()
{
	if (ViewModel->EditPanel == 0)
	{
		VisualStateManager::GoToState(this, "ViewViewState", true);
	}
	ViewModel->EditPanel = -1;
}


void scwin::ProductEditDetails::OnApplyTemplate()
{
	UserControl::OnApplyTemplate();
}

void scwin::ProductEditDetails::btnEdit_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	ViewModel->EditPanel = 0;
	StartEditing();
}


void scwin::ProductEditDetails::btnSave_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	WeakReference wr(this);
	create_task(ViewModel->Save())
		.then([wr](int retCode)
	{
		auto ths = wr.Resolve<ProductEditDetails>();
		if ((MessageIds)retCode == MessageIds::Success)
		{
			ths->ViewModel->CommitEdit();
			ths->EndEditing();
			ths->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler(
				[ths]()
			{
				if (VisualStateManager::GoToState(ths, "ValidationSuccessState", true))
				{
					OutputDebugString(L"ValidationSuccessState called");

				}
			}
			));
		}
		else
		{
			ths->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler(
				[ths]()
			{
				if (VisualStateManager::GoToState(ths, "ValidationFailedState", true))
				{
					OutputDebugString(L"ValidationFailedState called");

				}
			}
			));
			PubSubService::Instance->Publish(
				ths,
				PubSubMessageIds::ApplicationErrorStatusMessage,
				ref new String(GetMessage((MessageIds)retCode))
			);
		}
	});

}


void scwin::ProductEditDetails::btnCancel_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	this->ViewModel->RollbackEdit();
	EndEditing();
	WeakReference wr(this);
	Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler(
		[wr]()
	{
		auto ths = wr.Resolve<ProductEditDetails>();
		if (VisualStateManager::GoToState(ths, "ValidationSuccessState", true))
		{
			OutputDebugString(L"ValidationSuccessState called");

		}
	}
	));
}



void scwin::ProductEditDetails::OnSizeChanged(Platform::Object ^sender, Windows::UI::Xaml::SizeChangedEventArgs ^e)
{
	// stretch the grid to parent's size
	if (ViewModel != nullptr)
	{
		if (ViewModel->EditPanel == 0)
			gridproductEditable->Width = parentPanel->ActualWidth - buttonPanel->ActualWidth;
		else
			gridproductViewOnly->Width = parentPanel->ActualWidth - buttonPanel->ActualWidth;
	}
}


void scwin::ProductEditDetails::treeCategories_TreeViewItemClick(TreeViewControl::TreeView^ sender, TreeViewControl::TreeViewItemClickEventArgs^ args)
{
	if (args->ClickedItem)
	{
		auto tn = safe_cast<TreeNode^>(args->ClickedItem);
		auto data = safe_cast<ProductCategory^>(tn->Data);
		WeakReference wrthis(this);
		scLocalDataService::Instance()->GetCache().GetCategoryChildrenCountAsync(data)
			.then([wrthis, tn, data](int count)
		{
			if (count != tn->Size)
			{
				// get children, and then in the dispatcher look, reload treenode's children.
				scLocalDataService::Instance()->GetCache().GetCategoryChildrenAsync(data)
					.then([wrthis, tn, data](ProductCategoryVector^ children)
				{
					wrthis.Resolve<ProductEditDetails>()->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([wrthis, tn, children]()
					{
						tn->Clear();
						auto thisObj = wrthis.Resolve<ProductEditDetails>();
						thisObj->LoadNodeChildren(tn, children);
					}));
				});
			}
		});
	}
}


void scwin::ProductEditDetails::treeCategories_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{

}


void scwin::ProductEditDetails::foSelectCategory_Opening(Platform::Object^ sender, Platform::Object^ e)
{
	if (treeCategories->RootNode && treeCategories->RootNode->Size) return;

	// simply load first level at loading.
	WeakReference wr(this);
	create_task(scLocalDataService::Instance()->GetCategoryChildren(nullptr))
		.then([wr](IObservableVector<ProductCategory^>^ v)
	{
		auto ths = wr.Resolve<ProductEditDetails>();
		ths->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([ths, v]()
		{
			ths->LoadNodeChildren(ths->treeCategories->RootNode, v);
			if (ths->treeCategories->RootNode && ths->treeCategories->RootNode->Size)
			{
				ths->treeCategories->SelectedItem = ths->treeCategories->RootNode->GetAt(0);
			}
		}));
	});
}


void scwin::ProductEditDetails::LoadNodeChildren(TreeNode^ owner, IObservableVector<ProductCategory^>^ categories)
{
	using namespace cpplinq;

	for (auto c : categories)
	{
		c->Parent = (ProductCategory^)owner->Data;
		OutputDebugString(c->Name->Data());
		OutputDebugString(L"\n");
		auto tn = ref new TreeNode();
		tn->Data = c;
		if (ViewModel->CurrentProductCategories->Size)
		{
			auto isSelected = from(to_vector(ViewModel->CurrentProductCategories)) >> where([](IVectorView<ProductCategory^>^ path)
			{
				return path->Size;
			}) >> select([](IVectorView<ProductCategory^>^ path)
			{
				return path->GetAt(path->Size - 1);
			}) >> where([c](ProductCategory^ pc)
			{
				return pc->Id == c->Id;
			}) >> first_or_default() != nullptr;
			c->Tag = isSelected;
		}
		owner->Append(tn);
	}

}


void scwin::ProductEditDetails::CheckBox_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	using namespace cpplinq;
	auto chk = (CheckBox^)sender;
	// get the itemcontainer
	auto isChecked = chk->IsChecked;
	auto data = (ProductCategory^)(chk)->Tag;

	if (data)
	{

		// if checked add the category the viewmodel if not already added.
		if (ViewModel->CurrentProductCategories != nullptr)
		{
			int index = -1;
			for (int i = 0; i < ViewModel->CurrentProductCategories->Size; i++)
			{
				auto x = ViewModel->CurrentProductCategories->GetAt(i);
				auto v = to_vector(x);
				if (from(v) >> any([data](ProductCategory^ y) {
					return y->Id == data->Id;
				}))
				{
					// if already contains and checkbox is checked, then nothing to do
					if (isChecked->Value)
					{
						return;
					}
					else
					{
						index = i;
					}
				}
			}
			if (index >= 0)
			{
				ViewModel->CurrentProductCategories->RemoveAt(index);
				return;
			}
		}

	}

	using namespace cpplinq;

	// add if checked
	if (data && isChecked->Value)
	{
		auto hasit = from(to_vector(ViewModel->CurrentProductCategories))
			>> select([](IVectorView<ProductCategory^>^ view)
		{
			return view->GetAt(view->Size - 1);
		})
			>> where([data](ProductCategory^ cat)
		{
			return cat->Id == data->Id;
		}) >> any();
		if (!hasit)
		{
			Vector<ProductCategory^>^ path =
				ref new Vector<ProductCategory^>();
			auto o = data;
			while (o != nullptr)
			{
				path->InsertAt(0, o);
				o = o->Parent;
			}

			ViewModel->CurrentProductCategories->Append(path->GetView());

		}
	}
}


void scwin::ProductEditDetails::btnFlyoutClose_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	foSelectCategory->Hide();
}


void scwin::ProductEditDetails::OnPropertyChanged(Platform::Object ^sender, Windows::UI::Xaml::Data::PropertyChangedEventArgs ^e)
{
	if (e->PropertyName == "EditPanel")
	{
		if (ViewModel->EditPanel == 0)
		{
			StartEditing();
		}

	}
}


void scwin::ProductEditDetails::CategoryItemDelete_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	auto tag = ((Button^)sender)->Tag;
	// remove this from local collection
	auto path = dynamic_cast<IVectorView<ProductCategory^>^>(tag);
	UINT index;
	if (ViewModel->CurrentProductCategories->IndexOf(path, &index))
	{
		ViewModel->CurrentProductCategories->RemoveAt(index);
	}
}


void scwin::ProductEditDetails::OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	ViewModel->PropertyChanged -= tokViewModelPropChanged;
}
