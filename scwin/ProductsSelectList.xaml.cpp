//
// ProductsSelectList.xaml.cpp
// Implementation of the ProductsSelectList class
//

#include "pch.h"
#include "ProductsSelectList.xaml.h"
#include "XamlHelper.h"

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

DependencyProperty^ ProductsSelectList::selectedProductsProperty = nullptr;
DependencyProperty^ ProductsSelectList::allProductsProperty = nullptr;

void scwin::ProductsSelectList::Register()
{
	if (selectedProductsProperty == nullptr)
		selectedProductsProperty = DependencyProperty::Register("SelectedProducts",
			IVector<Product^>::typeid,
			ProductsSelectList::typeid,
			ref new PropertyMetadata(nullptr)
		);
	if (allProductsProperty == nullptr)
		allProductsProperty = DependencyProperty::Register("AllProducts",
			IVector<ProductSelectable^>::typeid,
			ProductsSelectList::typeid,
			ref new PropertyMetadata(nullptr)
		);
}

ProductsSelectList::ProductsSelectList()
{
	InitializeComponent();
}


void scwin::ProductsSelectList::txtFindProduct_TextChanged(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs^ args)
{
	auto ths = this;
	for (auto psel : ths->AllProducts)
	{
		auto lvi = safe_cast<ListViewItem^>(ths->listOfAllProducts->ContainerFromItem(psel));
		auto visibility = (StringHelpers::Match(psel->Item->Name, sender->Text) ||
			StringHelpers::Match(psel->Item->ShortDescription, sender->Text) ||
			StringHelpers::Match(psel->Item->Description, sender->Text)
			) ? Windows::UI::Xaml::Visibility::Visible : Windows::UI::Xaml::Visibility::Collapsed;
		lvi->Visibility = visibility;
	}
}


void scwin::ProductsSelectList::btnRemoveProduct_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

}


void scwin::ProductsSelectList::btnCancel_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Popup^ popup;
	if (XamlHelper::FindAncestor(this, &popup))
	{
		popup->IsOpen=false;
	}
}


void scwin::ProductsSelectList::CheckBox_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto dc = safe_cast<CheckBox^>(sender)->DataContext;
	// add it to selected products
	auto prodSelectable = dynamic_cast<ProductSelectable^>(dc);
	if (prodSelectable)
	{
		auto p = prodSelectable->Item;
		auto v = to_vector(SelectedProducts);
		auto fitr = find_if(v.cbegin(), v.cend(), [p](Product^ p1)
		{
			return p1->Id == p->Id;
		});
		if (fitr == v.cend())
		{
			SelectedProducts->Append(p);
		}
	}
}


void scwin::ProductsSelectList::CheckBox_Unchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto dc = safe_cast<CheckBox^>(sender)->DataContext;
	// add it to selected products
	auto prodSelectable = dynamic_cast<ProductSelectable^>(dc);
	if (prodSelectable)
	{
		auto p = prodSelectable->Item;
		auto v = to_vector(SelectedProducts);
		auto fitr = find_if(v.cbegin(), v.cend(), [p](Product^ p1)
		{
			return p1->Id == p->Id;
		});
		if (fitr != v.cend())
		{
			SelectedProducts->RemoveAt(fitr - v.cbegin());
		}
	}
}
