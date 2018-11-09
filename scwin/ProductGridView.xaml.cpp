//
// Products.xaml.cpp
// Implementation of the Products class
//

#include "pch.h"
#include "ProductGridView.xaml.h"
#include "XamlHelper.h"
//#include "AppView.xaml.h"
#include "ViewLifeTimeController.h"
#include "ProductView.xaml.h"

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
using namespace Windows::Security::Cryptography;
using namespace Windows::Graphics::Imaging;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

#define PID L"732BC119A040499DA19856D0D1094033"
#define PIMAGEID L"0C60F192C1E3432D898A6CE3D0F2FF30"

#define ALLPRODUCTS L"All Products"
#define PRODUCTVIEWTYPE		L"ProductViewType"
#define PRODUCTLASTSORTEDBY L"ProductLastSortedBy"
#define PRODUCTBREADCRUMPSEQ L"Products-Breadcrump-Seq"

void scwin::ProductGridView::OnNavigatedTo(NavigationEventArgs ^ e)
{
	ViewModel = dynamic_cast<ProductViewModel^>(e->Parameter);
	OnLoading();
}

ProductGridView::ProductGridView()
{
	InitializeComponent();
	Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::ProductGridView::OnLoaded);
}

void scwin::ProductGridView::OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{

	//// generate binary from Assets\Images\mona-lisa-74050_640.jpg
	// load last stored category breadcrump sequence
}


void scwin::ProductGridView::NotifyPropertyChange(String ^ propertyName)
{
	WeakReference wr(this);
	Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([wr, propertyName]()
	{
		auto ths = wr.Resolve<ProductGridView>();
		ths->PropertyChanged(ths, ref new PropertyChangedEventArgs(propertyName));
	}));
}

String ^ scwin::ProductGridView::ToHex(byte value)
{
	int const arraysize = 3;
	char16 hexValue[arraysize] = { 0 };
	swprintf_s(hexValue, arraysize, L"%02x", (int)value);
	return ref new String(hexValue);
}

String ^ scwin::ProductGridView::GetAddress(int databyte)
{
	int const arraysize = 9;
	char16 address[arraysize] = { 0 };
	swprintf_s(address, arraysize, L"%08x", databyte);
	return ref new String(address);
}

//void scwin::ProductGridView::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
//{
//	Button^ btn = (Button^)sender;
//	if (btn->Content->ToString() == L"Save in Couchdb")
//	{
//		Uri^ uri = ref new Uri("ms-appx:///Assets/Images/mona-lisa-74050_640.jpg");
//		create_task(StorageFile::GetFileFromApplicationUriAsync(uri))
//			.then([](StorageFile^ file)
//		{
//			return file->OpenAsync(FileAccessMode::Read);
//		}).then([=](task<IRandomAccessStream^> inputStream)
//		{
//			try
//			{
//				auto strm = inputStream.get();
//				Buffer^ buff = ref new Buffer(strm->Size);
//				create_task(strm->ReadAsync(buff, strm->Size, InputStreamOptions::None))
//					.then([=](task<IBuffer^> toutbuffer)
//				{
//					auto outbuffer = toutbuffer.get();
//					this->hexdump = CryptographicBuffer::EncodeToBase64String(buff);
//					OnHexDumpCompleted(hexdump);
//				});
//			}
//			catch (Exception^ ex)
//			{
//				throw ex;
//			}
//		}
//		);
//	}
//	else
//	{
//		// first get image
//		create_task(scLocalDataService::Instance()->GetProductImage(ref new String(PIMAGEID)))
//			.then([=](task<ProductImage^> imageLoaded)
//			{
//				try
//				{
//					auto img = imageLoaded.get();
//					if (img != nullptr && !StringHelpers::IsNullOrEmpty(img->Data))
//					{
//						// convert data property to bytes
//						auto buffer = CryptographicBuffer::DecodeFromBase64String(img->Data);
//						InMemoryRandomAccessStream^ outputStream = ref new InMemoryRandomAccessStream();
//						create_task(outputStream->WriteAsync(buffer))
//							.then([=](task<UINT> tread)
//						{
//							auto readSize = tread.get();
//							create_task(BitmapDecoder::CreateAsync(outputStream))
//								.then([=](task<BitmapDecoder^> tdc)
//							{
//								auto dc = tdc.get();
//								create_task(dc->GetSoftwareBitmapAsync())
//									.then([=](SoftwareBitmap^ sbmp)
//								{
//									sbmp = SoftwareBitmap::Convert(sbmp, BitmapPixelFormat::Bgra8, BitmapAlphaMode::Premultiplied);
//									auto src = ref new SoftwareBitmapSource();
//									create_task(src->SetBitmapAsync(sbmp))
//										.then([=]()
//									{
//										thumpnail->Source = src;
//									});
//								});
//							});
//
//						});
//					}
//				}
//				catch (Exception^ ex)
//				{
//					throw ex;
//				}
//			});
//	}
//}

void scwin::ProductGridView::ReadBytes(DataReader ^ reader)
{
	create_task(reader->LoadAsync(chunkSize))
		.then([=](task<UINT> bytesloaded)
	{
		auto bytecount = bytesloaded.get();
		auto bytes = ref new Array<byte>(bytecount);
		reader->ReadBytes(bytes);
		hexdump += CryptographicBuffer::EncodeToBase64String(CryptographicBuffer::CreateFromByteArray(bytes));
		if (bytecount == chunkSize)
		{
			ReadBytes(reader);
		}
		else
		{
			HexDumpCompleted(this->hexdump);
		}
	});
}


void scwin::ProductGridView::OnHexDumpCompleted(Platform::String ^obj)
{

	// test it
	auto buffer = CryptographicBuffer::DecodeFromBase64String(obj);

	// prepare to save data
	ProductImage^ img = ref new ProductImage(ref new String(PIMAGEID));
	img->Pid = ref new String(PID);
	img->Title = "Product Image Experimental";
	img->ContentType = "image/jpg";
	img->Data = obj;
	img->IsTitleImage = true;
	scLocalDataService::Instance()->SaveProductImageAsync(img);
}


void scwin::ProductGridView::lstCategoryBreadCrump_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	auto selected = e->AddedItems->First()->Current;
	if (selected)
	{
		ProductCategory^ cat = safe_cast<ProductCategory^>(selected);
		if (cat->Id == "0")
			PopulateProducts();
		else
			PopulateProducts(cat->Id);
	}
}


void scwin::ProductGridView::HyperlinkButton_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	// ensure that listviewitem of this button is selected
	auto elmnt = (FrameworkElement^)sender;
	// ensure that listviewitem of this button is selected
	auto lvi =  safe_cast<ListViewItem^>(lstCategoryBreadCrump->ContainerFromItem(elmnt->Tag));
	if (lvi != nullptr && !lvi->IsSelected)
		lvi->IsSelected = true;

	auto flyOut = (MenuFlyout^)FlyoutBase::GetAttachedFlyout(elmnt);
	auto selItem = lstCategoryBreadCrump->SelectedItem;
	if (selItem != nullptr)
	{
		WeakReference wr(this);
		ProductCategory^ cat = selItem == nullptr ? nullptr : (ProductCategory^)selItem;
		create_task(scLocalDataService::Instance()->GetCategoryChildren(cat->Id == L"0" ? nullptr : cat))
			.then([wr, flyOut, elmnt](IObservableVector<ProductCategory^>^ cats)
		{
			auto ctl = wr.Resolve<ProductGridView>();
			flyOut->Items->Clear();
			for (auto pc : cats)
			{
				MenuFlyoutItem^ item = ref new MenuFlyoutItem();
				item->Text = pc->Name;
				item->Command = ctl->cmdSelectMenuItem;
				item->CommandParameter = pc;
				flyOut->Items->Append(item);
			}
			flyOut->ShowAt(elmnt);
		});
	}
}


void scwin::ProductGridView::OnLoading()
{
	cmdSelectMenuItem = ref new Command(ref new ExecuteAction(this, &ProductGridView::ExecuteSelectMenuCommand), ref new CanExecuteFunc(this, &ProductGridView::CanExecuteSelectMenuCommand));
	CategoryBreadcrump = ref new Vector<ProductCategory^>();


	HexDumpCompleted += ref new scwin::Action<Platform::String ^>(this, &scwin::ProductGridView::OnHexDumpCompleted);
	Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::ProductGridView::OnLoaded);

	ListViewTypes vt = ListViewTypes::Tiles;
	auto prop = scLocalDataService::Instance()->GetProperty(ref new String(PRODUCTVIEWTYPE));
	if (prop && !StringHelpers::IsNullOrEmpty(prop->Value))
	{
		vt = (ListViewTypes)(int)std::wcstol(prop->Value->Data(), nullptr, 10);
	}
	this->lastViewType = vt;

	String^ sortedBy = L"Name";
	prop = scLocalDataService::Instance()->GetProperty(ref new String(PRODUCTLASTSORTEDBY));
	if (prop && !StringHelpers::IsNullOrEmpty(prop->Value))
	{
		sortedBy = prop->Value;
	}
	lastSortedBy = sortedBy;

	SortCommand = ref new Command(
		ref new ExecuteAction(this, &scwin::ProductGridView::ExecuteSortCommand),
		ref new CanExecuteFunc(this, &scwin::ProductGridView::CanExecuteSortCommand)
	);

	//ViewTypeCommand = ref new Command(
	//	ref new ExecuteAction(this, &scwin::ProductGridView::ExecuteViewTypeCommand),
	//	ref new CanExecuteFunc(this, &scwin::ProductGridView::CanExecuteViewTypeCommand)
	//);

	if (ViewModel->IsProductManager)
	{
		ViewTypeCommand = ref new Command(
			ref new ExecuteAction(this, &scwin::ProductGridView::ExecuteViewTypeCommand),
			ref new CanExecuteFunc(this, &scwin::ProductGridView::CanExecuteViewTypeCommand)
		);
	}

	WeakReference wr(this);
	prop = scLocalDataService::Instance()->GetProperty(PRODUCTBREADCRUMPSEQ);
	if (prop && !StringHelpers::IsNullOrEmpty(prop->Value))
	{
		concurrency::create_task([wr, prop]()
		{
			auto ths = wr.Resolve<ProductGridView>();
			ths->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([wr, prop]()
			{
				auto ths = wr.Resolve<ProductGridView>();
				IVector<String^>^ bcarray = StringHelpers::Split(prop->Value, L';');
				if (bcarray->Size)
				{
					if (bcarray->GetAt(0) == ALLPRODUCTS)
					{
						for (auto str : bcarray)
						{
							if (str == (String^)ALLPRODUCTS)
							{
								auto rootCat = ref new ProductCategory(L"0");
								rootCat->Name = ALLPRODUCTS;
								ths->catBreadcrump->Append(rootCat);
							}
							else
							{
								CONDITIONS cont;
								cont.insert(make_pair("name", STRCONV(str)));
								auto cat = scLocalDataService::Instance()->GetCategory(cont);
								if (cat)
								{
									ths->catBreadcrump->Append(cat);
								}
							}
						}
					}
					if (ths->catBreadcrump && ths->catBreadcrump->Size)
						ths->PopulateProducts(ths->catBreadcrump->GetAt(ths->catBreadcrump->Size - 1)->Id);
					return;
				}
				auto view = wr.Resolve<ProductGridView>();
				auto rootCat = ref new ProductCategory(L"0");
				rootCat->Name = ALLPRODUCTS;
				view->catBreadcrump->Append(rootCat);
				view->PopulateProducts();
			}));
		});
	}
	else
	{
		auto rootCat = ref new ProductCategory(L"0");
		rootCat->Name = ALLPRODUCTS;
		catBreadcrump->Append(rootCat);
		PopulateProducts();
	}
}

void scwin::ProductGridView::PopulateProducts(String^ catId)
{
	WeakReference wr(this);
	if (catId)
	{
		CONDITIONS cond;
		cond.insert(make_pair("categoryid", STRCONV(catId)));
		cond.insert(make_pair("orderby", "name"));
		create_task(scLocalDataService::Instance()->GetProductsAsync(cond))
			.then([wr](ProductList^ plist)
		{
			auto ths = wr.Resolve<ProductGridView>();
			ths->displayedProducts = ths->allproducts = plist;
			ths->NotifyPropertyChange("DisplayedProducts");
		});
	}
	else
	{
		create_task(scLocalDataService::Instance()->GetProductsAsync())
			.then([wr](ProductList^ plist)
		{
			auto ths = wr.Resolve<ProductGridView>();
			ths->displayedProducts = ths->allproducts = plist;
			ths->NotifyPropertyChange("DisplayedProducts");
		});
	}
}

void scwin::ProductGridView::ExecuteSortCommand(Object ^ data)
{
	FlyoutBase::ShowAttachedFlyout(btnProductSort);
}

bool scwin::ProductGridView::CanExecuteSortCommand(Object ^ data)
{
	return true;
}

void scwin::ProductGridView::ExecuteViewTypeCommand(Object ^ data)
{

}

bool scwin::ProductGridView::CanExecuteViewTypeCommand(Object ^ data)
{
	return true;
	//return displayedProducts != nullptr && displayedProducts->Size > 0;
}

void scwin::ProductGridView::ExecuteSortMenuItemCommand(Object ^ data)
{
}

bool scwin::ProductGridView::CanExecuteSortMenuItemCommand(Object ^ data)
{
	return true;
}

void scwin::ProductGridView::ExecuteSelectMenuCommand(Object ^ data)
{
	// if not already, add this menu item to breadcrump menu listbox.
	ProductCategory^ cat = safe_cast<ProductCategory^>(data);
	ProductCategory^ selectedInBreadCrumb = safe_cast<ProductCategory^>(lstCategoryBreadCrump->SelectedItem);
	UINT index = -1;
	for (UINT i = 0; i < catBreadcrump->Size; ++i)
	{
		auto item = catBreadcrump->GetAt(i);
		if (item->Id == selectedInBreadCrumb->Id)
		{
			index = i;
			break;
		}
	}

	// clear up to one before index or 1
	UINT i = catBreadcrump->Size - 1;
	while (i > index && i > 0)
	{
		catBreadcrump->RemoveAt(i--);
	};
	catBreadcrump->Append(cat);
	WeakReference wr(this);
	create_task([wr]()
	{
		auto ths = wr.Resolve<ProductGridView>();
		String^ joined;
		for (UINT i = 0; i < ths->catBreadcrump->Size; i++)
		{
			joined += ths->catBreadcrump->GetAt(i)->Name;
			if (i < ths->catBreadcrump->Size - 1)
			{
				joined += L";";
			}
		}
		AppProperty^ prop = ref new AppProperty(PRODUCTBREADCRUMPSEQ, joined);
		scLocalDataService::Instance()->SetProperty(prop);

		ths->PopulateProducts();

	});
}

bool scwin::ProductGridView::CanExecuteSelectMenuCommand(Object ^ data)
{
	return true;
}

void scwin::ProductGridView::MenuFlyoutItem_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	
}


void scwin::ProductGridView::listViewProducts_ItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto clicked = e->ClickedItem;
	if (clicked)
	{
		auto prod = safe_cast<Product^>(clicked);
	}
}


void scwin::ProductGridView::btnProductView_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{

	auto relatedProduct = safe_cast<Product^>(((Button^)sender)->Tag);
	if (relatedProduct != selectedProduct)
	{
		listViewProducts->SelectedItem = relatedProduct;
	}
	if (relatedProduct)
	{
		auto pvm = this->ViewModel;
		pvm->Initialize(selectedProduct);
		Frame->Navigate(TypeName(ProductView::typeid), pvm);
	}
}


void scwin::ProductGridView::btnViewProduct_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{

}

//
//void scwin::ProductGridView::ProductItem_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
//{
//	DependencyObject^ productItem = safe_cast<DependencyObject^>(sender);
//
//	OutputDebugString(L"\n");
//	OutputDebugString(productItem->GetType()->FullName->Data());
//	OutputDebugString(L"\n");
//
//	DependencyObject^ btElmnt;
//	if (XamlHelper::GetChildFromTree(productItem, "btnProductView", &btElmnt))
//	{
//		Button^ btn = (Button^)btElmnt;
//		btn->Visibility = Windows::UI::Xaml::Visibility::Visible;
//	}
//}
//
//
//void scwin::ProductGridView::ProductItem_PointerExited(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
//{
//	DependencyObject^ productItem = safe_cast<DependencyObject^>(sender);
//	DependencyObject^ btElmnt;
//	if (XamlHelper::GetChildFromTree(productItem, "btnProductView", &btElmnt))
//	{
//		Button^ btn = (Button^)btElmnt;
//		btn->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
//	}
//}
