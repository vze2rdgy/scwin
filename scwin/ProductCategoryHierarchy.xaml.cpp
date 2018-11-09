//
// ProductCategoryHierarchy.xaml.cpp
// Implementation of the ProductCategoryHierarchy class
//

#include "pch.h"
#include "ProductCategoryHierarchy.xaml.h"


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
using namespace Windows::UI::Popups;

using namespace TreeViewControl;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

DependencyProperty^ ProductCategoryHierarchy::isInEditProperty = nullptr;


void scwin::ProductCategoryHierarchy::RegisterDependencyProperties()
{
	isInEditProperty = DependencyProperty::Register(
		"IsInEdit",
		bool::typeid,
		ProductCategoryHierarchy::typeid,
		ref new PropertyMetadata(false, ref new PropertyChangedCallback(&ProductCategoryHierarchy::IsInEditPropertyChanged)
		));
}

void scwin::ProductCategoryHierarchy::IsInEditPropertyChanged(DependencyObject ^ d, DependencyPropertyChangedEventArgs ^ prop)
{
	auto thisobj = safe_cast<ProductCategoryHierarchy^>(d);
	thisobj->UpdateButtonStates();
}

void scwin::ProductCategoryHierarchy::BindData()
{
	auto tn = safe_cast<TreeNode^>(treeCategories->SelectedItem);
	if (tn)
	{
		auto data = safe_cast<ProductCategory^>(safe_cast<TreeNode^>(treeCategories->SelectedItem)->Data);
		txtPCatName->Text = data->Name;
		txtPCatDesc->Text = data->Desc;
	}
}

ProductCategoryHierarchy::ProductCategoryHierarchy()
{
	InitializeComponent();

	InitializeCommands();

	//DataContext = this;



	Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::ProductCategoryHierarchy::OnLoaded);

	treeCategories->ContainerContentChanging += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Xaml::Controls::ListViewBase ^, Windows::UI::Xaml::Controls::ContainerContentChangingEventArgs ^>(this, &scwin::ProductCategoryHierarchy::OnContainerContentChanging);

	this->SizeChanged += ref new Windows::UI::Xaml::SizeChangedEventHandler(this, &scwin::ProductCategoryHierarchy::OnSizeChanged);
	Windows::Graphics::Display::DisplayInformation::GetForCurrentView()->OrientationChanged += 
		ref new Windows::Foundation::TypedEventHandler<Windows::Graphics::Display::DisplayInformation ^, Platform::Object ^>(this, &scwin::ProductCategoryHierarchy::OnOrientationChanged);
}


void scwin::ProductCategoryHierarchy::OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	// simply load first level at loading.
	WeakReference wr(this);
	scLocalDataService::Instance()->GetCategoryChildren(nullptr)->Completed = ref new AsyncOperationWithProgressCompletedHandler<IObservableVector<ProductCategory^>^, int>([wr](IAsyncOperationWithProgress<IObservableVector<ProductCategory^>^, int>^ progress, AsyncStatus status)
	{
		if (status == AsyncStatus::Completed)
		{
			auto v = progress->GetResults();
			auto ths = wr.Resolve<ProductCategoryHierarchy>();
			ths->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([ths, v]()
			{
				ths->LoadNodeChildren(ths->treeCategories->RootNode, v);
				if (ths->treeCategories->RootNode && ths->treeCategories->RootNode->Size)
				{
					ths->treeCategories->SelectedItem = ths->treeCategories->RootNode->GetAt(0);
				}
				else
				{
					ths->btnCatEdit->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
					ths->btnCatDelete->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
				}
				ths->IsInEdit = false;
			}));
		}
		else
		{

		}
	});
}

void scwin::ProductCategoryHierarchy::InitializeCommands()
{
	AddCategory = ref new Command(
		ref new ExecuteAction(this, &ProductCategoryHierarchy::ExcecuteAddCategory),
		ref new CanExecuteFunc(this, &ProductCategoryHierarchy::CanExecuteAddCategory)
	);
	EditCategory = ref new Command(
		ref new ExecuteAction(this, &ProductCategoryHierarchy::ExcecuteEditCategory),
		ref new CanExecuteFunc(this, &ProductCategoryHierarchy::CanExecuteEditCategory)
	);
	DeleteCategory = ref new Command(
		ref new ExecuteAction(this, &ProductCategoryHierarchy::ExcecuteDeleteCategory),
		ref new CanExecuteFunc(this, &ProductCategoryHierarchy::CanExecuteDeleteCategory)
	);
	SaveCategory = ref new Command(
		ref new ExecuteAction(this, &ProductCategoryHierarchy::ExcecuteSaveCategory),
		ref new CanExecuteFunc(this, &ProductCategoryHierarchy::CanExecuteSaveCategory)
	);
	CancelCategory = ref new Command(
		ref new ExecuteAction(this, &ProductCategoryHierarchy::ExcecuteCancelCategory),
		ref new CanExecuteFunc(this, &ProductCategoryHierarchy::CanExecuteCancelCategory)
	);
}

void scwin::ProductCategoryHierarchy::ExcecuteAddCategory(Object ^ p)
{
	this->IsInEdit = true;

	txtPCatDesc->Text = L"";
	txtPCatName->Text = L"";
	txtPCatName->Focus(Windows::UI::Xaml::FocusState::Programmatic);
	txtPCatName->Tag = 0;
	if (treeCategories->SelectedItem == nullptr)
		treeCategories->SelectedItem = treeCategories->RootNode->Data;
}

bool scwin::ProductCategoryHierarchy::CanExecuteAddCategory(Object ^ p)
{
	if (IsInEdit) return false;
	return treeCategories->RootNode != nullptr || treeCategories->SelectedItem != nullptr;
	return true;
}

void scwin::ProductCategoryHierarchy::ExcecuteEditCategory(Object ^ p)
{
	this->IsInEdit = true;
	txtPCatName->Tag = 1;
}

bool scwin::ProductCategoryHierarchy::CanExecuteEditCategory(Object ^ p)
{
	if (IsInEdit) 
		return false;
	
	if (treeCategories->SelectedItem == nullptr)
		return false;
	
	return true;
}

void scwin::ProductCategoryHierarchy::ExcecuteDeleteCategory(Object ^ p)
{
	auto tn = (TreeNode^)treeCategories->SelectedItem;
	String^ msg = L"Do you want to delete this category ?\n";
	if (tn->Size)
		msg += "This category has sub-categories defined and they will be lost as well.\n";
	msg += "If the category has products assigned, the delete operation is ignored. \nPlease read the status message for more information.";
	auto box = ref new MessageDialog(msg, "Delete Category");
	box->Commands->Append(ref new UICommand("Yes", ref new UICommandInvokedHandler(this, &ProductCategoryHierarchy::DeleteCommandAction), 0));
	box->Commands->Append(ref new UICommand("No", ref new UICommandInvokedHandler(this, &ProductCategoryHierarchy::DeleteCommandAction), 1));
	box->DefaultCommandIndex = 1;
	box->CancelCommandIndex = 1;
	box->ShowAsync();
}

void scwin::ProductCategoryHierarchy::DeleteCommandAction(IUICommand ^ cmd)
{
	if ((int)cmd->Id == 1) // cancel
	{
		PubSubService::Instance->Publish(this, PubSubMessageIds::ApplicationInfoStatusMessage, "Category delete operation is cancelled.");
	}
	else
	{
		auto tn = (TreeNode^)treeCategories->SelectedItem;
		auto data = tn ? safe_cast<ProductCategory^>(tn->Data) : nullptr;
		if (data)
		{
			scLocalDataService::Instance()->GetCache().DeleteCategoryAsync(data->Id);
		}
	}
}

bool scwin::ProductCategoryHierarchy::CanExecuteDeleteCategory(Object ^ p)
{
	if (IsInEdit) return false;
	return treeCategories->SelectedItem != nullptr;
}

void scwin::ProductCategoryHierarchy::ExcecuteSaveCategory(Object ^ p)
{
	String^ validMsg = nullptr;
	if (StringHelpers::IsNullOrEmpty(txtPCatName->Text))
	{
		VisualStateManager::GoToState(txtPCatName, L"DataInvalid", false);
		validMsg += L"Category Name is required";
	}
	if (StringHelpers::IsNullOrEmpty(txtPCatDesc->Text))
	{
		VisualStateManager::GoToState(txtPCatDesc, L"DataInvalid", false);
		validMsg += validMsg ? L"|" : L"";
		validMsg += L"Category Description is required";
	}
	if (!StringHelpers::IsNullOrEmpty(validMsg))
	{
		PubSubService::Instance->Publish(this, PubSubMessageIds::ApplicationErrorStatusMessage, validMsg);
		return;
	}

	VisualStateManager::GoToState(txtPCatName, L"DataValid", false);
	VisualStateManager::GoToState(txtPCatDesc, L"DataValid", false);

	// validate document
	auto tn = safe_cast<TreeNode^>(treeCategories->SelectedItem);
	if (tn == nullptr)
	{
		tn = treeCategories->RootNode;
		treeCategories->SelectedItem = tn;
	}

	if (tn->Data == nullptr && tn == treeCategories->RootNode)
	{
		tn->Data = ref new ProductCategory("rootcategory");
	}

	ProductCategory^ data = (ProductCategory^)tn->Data;

	

	// search of the name cat name among nodes children
	for (int i = 0; i<tn->Size; i++)
	{
		auto n = safe_cast<TreeNode^>(tn->GetAt(i));
		auto d = safe_cast<ProductCategory^>(n->Data);
		if (StringHelpers::Trim(txtPCatName->Text) == StringHelpers::Trim(d->Name))
		{
			VisualStateManager::GoToState(txtPCatName, L"DataInvalid", false);
			PubSubService::Instance->Publish(this, PubSubMessageIds::ApplicationErrorStatusMessage, L"Category Name is already available.");
			return;
		}
	}

	String^ pid = nullptr;
	// make sure the category name is not already part of the node list 
	bool isInsert = (int)txtPCatName->Tag == 0;
	ProductCategory^ d = nullptr;
	if (isInsert) // insert
	{
		d = ref new ProductCategory();
		pid = data->Id;
	}
	else // edit
	{
		d = data;
	}
	d->Name = txtPCatName->Text;
	d->Desc = txtPCatDesc->Text;
	WeakReference wr(this);
	scLocalDataService::Instance()->GetCache().SaveCategoryLocalAsync(d, pid)
		.then([wr, d, isInsert, tn](bool status)
	{
		auto ths = wr.Resolve<ProductCategoryHierarchy>();
		ths->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([wr, d, status, isInsert, tn]()
		{
			auto ths = wr.Resolve<ProductCategoryHierarchy>();
			if (status)
			{
				ths->txtPCatName->Tag = nullptr;
				ths->IsInEdit = false;
				if (isInsert)
				{
					TreeNode^ newNode = ref new TreeNode();
					newNode->Data = d;
					tn->Append(newNode);
					ths->treeCategories->SelectedItem = newNode;
				}
				else
				{
					tn->Data = d;
				}
			}
			else
			{
				PubSubService::Instance->Publish(ths, PubSubMessageIds::ApplicationErrorStatusMessage, L"Product category was not saved correctly. Please retry. Otherwise Cancel and continue.");
			}
		}));
	});
}

bool scwin::ProductCategoryHierarchy::CanExecuteSaveCategory(Object ^ p)
{
	if (IsInEdit)
	{
		if (treeCategories->RootNode->Size == 0)
			return true;
		if (treeCategories->SelectedItem != nullptr)
			return true;
	}
	return false;
}

void scwin::ProductCategoryHierarchy::ExcecuteCancelCategory(Object ^ p)
{
	VisualStateManager::GoToState(txtPCatName, L"DataValid", false);
	VisualStateManager::GoToState(txtPCatDesc, L"DataValid", false);
	txtPCatName->Tag = nullptr;
	this->IsInEdit = false;
	BindData();
}

bool scwin::ProductCategoryHierarchy::CanExecuteCancelCategory(Object ^ p)
{
	if (IsInEdit)
	{
		if (treeCategories->RootNode->Size == 0)
			return true;
		if (treeCategories->SelectedItem != nullptr)
			return true;
	}
	return false;
}

void scwin::ProductCategoryHierarchy::LoadNodeChildren(TreeNode^ owner, IObservableVector<ProductCategory^>^ categories)
{

	Vector<Object^>^ tns = nullptr;
	if (!StringHelpers::IsNullOrEmpty(txtSearchCategory->Text))
	{
		tns = ref new Vector<Object^>();
	}

	for (auto c : categories)
	{
		OutputDebugString(c->Name->Data());
		OutputDebugString(L"\n");
		auto tn = ref new TreeNode();
		tn->Data = c;
		tn->IsExpanded = false;
		owner->Append(tn);
		if (tns)
		{
			tns->Append(tn);
		}
	}
	if (tns)
	{
		HideShowNodesWhenSearched(txtSearchCategory->Text, tns);
	}
}

void scwin::ProductCategoryHierarchy::HideShowNodesWhenSearched(String ^ searched, Windows::Foundation::Collections::IVector<Platform::Object^>^ items)
{
	// last item selected
	TreeNode^ lastItem = treeCategories->SelectedItem ? safe_cast<TreeNode^>(treeCategories->SelectedItem) : nullptr;
	bool hideIfMatched = !StringHelpers::IsNullOrEmpty(searched);
	for (int i = 0; i < items->Size; i++)
	{
		auto tn = safe_cast<TreeNode^>(treeCategories->Items->GetAt(i));
		auto data = safe_cast<ProductCategory^>(tn->Data);
		auto uie = safe_cast<FrameworkElement^>(treeCategories->ContainerFromItem(tn));
		if (uie)
		{
			if (hideIfMatched)
			{
				bool hasMatched = StringHelpers::Match(data->Name, txtSearchCategory->Text);
				auto visibility = hasMatched ? Windows::UI::Xaml::Visibility::Visible : Windows::UI::Xaml::Visibility::Collapsed;
				uie->Visibility = visibility;
				// when a child node is shown, then its parent also must be displayed.
				if (visibility == Windows::UI::Xaml::Visibility::Visible)
				{
					TreeNode^ tpar = tn->ParentNode;
					while (tpar != treeCategories->RootNode)
					{
						safe_cast<FrameworkElement^>(treeCategories->ContainerFromItem(tpar))->Visibility = visibility;
						tpar = tpar->ParentNode;
					}
				}
			}
			else
			{
				if (uie->Visibility != Windows::UI::Xaml::Visibility::Visible)
				{
					uie->Visibility = Windows::UI::Xaml::Visibility::Visible;
				}
			}
		}
	}

	WeakReference wr(this);
	Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([lastItem, wr]()
	{
		auto ths = wr.Resolve<ProductCategoryHierarchy>();
		// if lastItem is visible then keep it selected, otherwise, first visible item
		if (lastItem && safe_cast<ListViewItem^>(ths->treeCategories->ContainerFromItem(lastItem))->Visibility == Windows::UI::Xaml::Visibility::Visible)
		{
			ths->treeCategories->SelectedItem = lastItem;
		}
		else
		{
			for (int i = 0; i < ths->treeCategories->Items->Size; i++)
			{
				auto item = ths->treeCategories->Items->GetAt(i);
				auto lv = safe_cast<ListViewItem^>(ths->treeCategories->ContainerFromItem(item));
				if (lv->Visibility == Windows::UI::Xaml::Visibility::Visible)
				{
					ths->treeCategories->SelectedItem = item;
				}
			}
		}
	}));

}


void scwin::ProductCategoryHierarchy::OnContainerContentChanging(Windows::UI::Xaml::Controls::ListViewBase ^sender, Windows::UI::Xaml::Controls::ContainerContentChangingEventArgs ^args)
{
	auto node = dynamic_cast<TreeNode^>(args->Item);
	if (node != nullptr)
	{
		auto data = dynamic_cast<ProductCategory^>(node->Data);
		if (data != nullptr)
		{
			args->ItemContainer->AllowDrop = true;
		}
	}
}


void scwin::ProductCategoryHierarchy::OnSizeChanged(Platform::Object ^sender, Windows::UI::Xaml::SizeChangedEventArgs ^e)
{
	auto size = e->NewSize;
	gridCatTreePart->Width = size.Width * .4;
	gridCategoryDetails->Width = size.Width * .6;
}



void scwin::ProductCategoryHierarchy::OnOrientationChanged(Windows::Graphics::Display::DisplayInformation ^sender, Platform::Object ^args)
{
	using namespace Windows::Graphics::Display;
	if (sender->CurrentOrientation == DisplayOrientations::Landscape || sender->CurrentOrientation == DisplayOrientations::LandscapeFlipped)
	{
		stackCategories->Orientation = Orientation::Horizontal;
	}
	else
	{
		stackCategories->Orientation = Orientation::Vertical;
	}
}


void scwin::ProductCategoryHierarchy::treeCategories_TreeViewItemClick(TreeViewControl::TreeView^ sender, TreeViewControl::TreeViewItemClickEventArgs^ args)
{
	using namespace TreeViewControl;
	if (args->ClickedItem)
	{
		auto tn = safe_cast<TreeNode^>(args->ClickedItem);
		auto data = safe_cast<ProductCategory^>(tn->Data);
		// check size
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
					wrthis.Resolve<ProductCategoryHierarchy>()->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([wrthis, tn, children]()
					{
						tn->Clear();
						auto thisObj = wrthis.Resolve<ProductCategoryHierarchy>();
						thisObj->LoadNodeChildren(tn, children);
						thisObj->UpdateButtonStates();
					}));
				});
			}
		});
	}
}


void scwin::ProductCategoryHierarchy::categoryTreeViewItemContextMenu_Opening(Platform::Object^ sender, Platform::Object^ e)
{
	MenuFlyout^ mfl = safe_cast<MenuFlyout^>(sender);
	for (auto m : mfl->Items)
	{
		if (m->Name == L"cmenuAddCategory")
		{
			m->IsEnabled = AddCategory->CanExecute(treeCategories->SelectedItem);
		}
		else if (m->Name == L"cmenuEditCategory")
		{
			m->IsEnabled = EditCategory->CanExecute(treeCategories->SelectedItem);
		}
		else if (m->Name == L"cmenuDeleteCategory")
		{
			m->IsEnabled = DeleteCategory->CanExecute(treeCategories->SelectedItem);
		}
	}
}

void scwin::ProductCategoryHierarchy::treeCategories_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	// update canexecute states
	BindData();
	UpdateButtonStates();
}


void scwin::ProductCategoryHierarchy::txtSearchCategory_TextChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e)
{
	// use treeCategories->ContainerFromItem(treenode) method to hide and show nodes 
	// in the treeview based on what has satisfied or not.

	HideShowNodesWhenSearched(txtSearchCategory->Text, treeCategories->Items);

}

void scwin::ProductCategoryHierarchy::UpdateButtonStates()
{
	AddCategory->Refresh(treeCategories->SelectedItem);
	EditCategory->Refresh(treeCategories->SelectedItem);
	DeleteCategory->Refresh(treeCategories->SelectedItem);
	SaveCategory->Refresh(treeCategories->SelectedItem);
	CancelCategory->Refresh(treeCategories->SelectedItem);
}


void scwin::ProductCategoryHierarchy::cmenuAddCategory_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	AddCategory->Execute(treeCategories->SelectedItem);
}


void scwin::ProductCategoryHierarchy::cmenuDeleteCategory_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	DeleteCategory->Execute(treeCategories->SelectedItem);
}


void scwin::ProductCategoryHierarchy::cmenuEditCategory_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	EditCategory->Execute(treeCategories->SelectedItem);
}
