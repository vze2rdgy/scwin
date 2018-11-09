//
// ChooseAProduct.xaml.cpp
// Implementation of the ChooseAProduct class
//

#include "pch.h"
#include "ChooseAProduct.xaml.h"
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

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238




DependencyProperty^ ChooseAProduct::selectedProductProperty = nullptr;
DependencyProperty^ ChooseAProduct::productlistProperty = nullptr;
DependencyProperty^ ChooseAProduct::filterProperty = nullptr;
DependencyProperty^ ChooseAProduct::displaydetailOptionsProperty = nullptr;

void scwin::ChooseAProduct::Register()
{
	if (selectedProductProperty == nullptr)
	{
		selectedProductProperty = DependencyProperty::Register(
			"SelectedProduct", ProductWithInventoryWrapper::typeid, ChooseAProduct::typeid,
			ref new PropertyMetadata(nullptr)
		);
	}
	if (productlistProperty== nullptr)
	{
		productlistProperty = DependencyProperty::Register(
			"ProductList", IVector<Product^>::typeid, ChooseAProduct::typeid,
			ref new PropertyMetadata(nullptr)
		);
	}
	if (filterProperty == nullptr)
	{
		filterProperty = DependencyProperty::Register(
			"Filter", String::typeid, ChooseAProduct::typeid,
			ref new PropertyMetadata("",
				ref new PropertyChangedCallback(&OnFilterPropertyChanged))
		);
	}
	if (displaydetailOptionsProperty == nullptr)
	{
		displaydetailOptionsProperty = DependencyProperty::Register(
			"Filter", ProductChooseOptions::typeid, ChooseAProduct::typeid,
			ref new PropertyMetadata(ProductChooseOptions::Basic)
		);
	}
}

DEFINERAISEPROPCHANGEDSELF(ChooseAProduct);


ChooseAProduct::ChooseAProduct()
{
	InitializeComponent();

	Loaded += ref new RoutedEventHandler(this, &ChooseAProduct::OnLoad);
}


void scwin::ChooseAProduct::OnFilterPropertyChanged(DependencyObject ^ object, DependencyPropertyChangedEventArgs ^ args)
{

}

void scwin::ChooseAProduct::OnLoad(Object ^ sender, RoutedEventArgs ^ eargs)
{

	//auto tiertempl = Resources->Lookup("TierPricePlanTemplate");

	switch (DisplayDetailOptions)
	{
	case ProductChooseOptions::Basic:
		LoadProductsBasicInfo();
		break;
	case ProductChooseOptions::ProductInfoWithPrice:
		LoadProductsWithPriceInfo();
		break;
	case ProductChooseOptions::ProductInfoWithPriceAndInventory:
		LoadProductsWithPriceAndInventoryInfo();
		break;
	}

}

void scwin::ChooseAProduct::LoadProductsBasicInfo()
{
	if (ProductList == nullptr) return;
	auto tmplist = ref new Vector<ProductWithInventoryWrapper^>();
	for (auto p : ProductList)
	{
		auto pw = ref new ProductWithInventoryWrapper(p);
		tmplist->Append(pw);
	}
	ProductPresentationList = tmplist;
	GOTOVIEWSTATE(this, "BasicState");
}

void scwin::ChooseAProduct::LoadProductsWithPriceInfo()
{
	if (ProductList == nullptr) return;
	auto tmplist = ref new Vector<ProductWithInventoryWrapper^>();
	for (auto p : ProductList)
	{
		auto pw = ref new ProductWithInventoryWrapper(p);
		pw->LoadPriceInfo();
		tmplist->Append(pw);
	}
	ProductPresentationList = tmplist;
	GOTOVIEWSTATE(this, "ProductWithPriceState");
}

void scwin::ChooseAProduct::LoadProductsWithPriceAndInventoryInfo()
{
	if (ProductList == nullptr) return;
	auto tmplist = ref new Vector<ProductWithInventoryWrapper^>();
	for (auto p : ProductList)
	{
		auto pw = ref new ProductWithInventoryWrapper(p);
		pw->LoadPriceInfo();
		pw->LoadInventoryInfo();
		tmplist->Append(pw);
	}
	ProductPresentationList = tmplist;
	GOTOVIEWSTATE(this, "ProductWithPriceAndInventoryState");
}


void scwin::ChooseAProduct::lvProducts_ItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto prodwrap = dynamic_cast<ProductWithInventoryWrapper^>(e->ClickedItem);
	if (prodwrap != nullptr)
	{
		productSelectedEvent(this, prodwrap);
		Popup^ flyo; // note that even flyout is another popup cloaked in a modern look.
		if (XamlHelper::FindAncestor(this, &flyo))
		{
			flyo->IsOpen = false;
		}
	}
}
