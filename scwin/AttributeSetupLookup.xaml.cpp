//
// AttributeSetupLookup.xaml.cpp
// Implementation of the AttributeSetupLookup class
//

#include "pch.h"
#include "AttributeSetupLookup.xaml.h"
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

// The Content Dialog item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

scwin::AttributeSetupLookup::AttributeSetupLookup(AttributeViewModelFacade^ facade)
{
	InitializeComponent();

	this->facade = facade;

	attrTypes = scMainViewModel::GetProductAttributeTypes();

	Unloaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::AttributeSetupLookup::OnUnloaded);

	WeakReference wr(this);
	//auto currView = ApplicationView::GetForCurrentView();
	create_task(scLocalDataService::Instance()->GetAttributesAsync())
		.then([wr](task<IVector<ProductAttribute^>^> tlist)
	{
		try 
		{
			tlist.wait();

			auto ths = wr.Resolve<AttributeSetupLookup>();
			auto result = tlist.get();
			ths->Attributes = result;
		}
		catch (COMException^ ex)

		{
			LogException(STRCONV(ex->Message).c_str());
		}
	});
}



void scwin::AttributeSetupLookup::btnClose_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	this->Hide();
}


void scwin::AttributeSetupLookup::btnAccept_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	// validate
	if (currentAttrib == nullptr || !currentAttrib->IsValid || !currentAttribVal->IsValid)
	{
		using namespace Windows::UI::Popups;
		auto msgdlg = ref new MessageDialog("No enough data entered for the attribute or its value.", "Validation Error");
		msgdlg->ShowAsync();
	}

	WeakReference wr(this);
	auto type = currentAttrib->Type;
	// TODO: if type is quantitative, ensure that user enters only numerical data
	// We have to achieve this through control validation.
	auto datatype = LookupAttributeValueType(type);

	currentAttribVal->DataType = datatype;
	create_task(scLocalDataService::Instance()->UpdateProductAttributeAsync(facade->SelectedProduct->Id, currentAttrib, currentAttribVal))
		.then([wr](MessageIds ret)
	{
		auto ths = wr.Resolve<AttributeSetupLookup>();
		if (HASFAILED(ret))
		{
			ths->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, 
				ref new DispatchedHandler([]()
			{
				using namespace Windows::UI::Popups;
				auto msgdlg = ref new MessageDialog("Failed to save attribute/value. Please retry.", "Save Error");
				msgdlg->ShowAsync();
			}));
		}
		else
		{
			// pass the new attribute info to the calling view.
			ths->Dispatcher->RunAsync(CoreDispatcherPriority::Normal,
				ref new DispatchedHandler([wr]()
			{
				auto ths = wr.Resolve<AttributeSetupLookup>();
				// check if already exists
				using namespace cpplinq;
				auto hasAny = from(to_vector(ths->facade->ProductAttributes))
					>> where([wr](ProductAttributeAndValue^ x)
				{
					auto ths = wr.Resolve<AttributeSetupLookup>();
					
					if (x->Value == nullptr || ths->CurrentAttributeValue == nullptr)
					{
						// ignore;
						return true;
					}

					if (StringHelpers::Trim(x->Value->Value->ToString()) == StringHelpers::Trim(ths->CurrentAttributeValue->Value->ToString()))
					{
						return true;
					}

					return false;
				}) >> any();

				if (!hasAny)
				{
					auto sz = ths->facade->ProductAttributes->Size + 1;
					ths->facade->ProductAttributes->Append(ref new ProductAttributeAndValue(sz, ths->CurrentAttribute, ths->CurrentAttributeValue));
				}
				ths->Hide();
			}));
		}
	});
}


void scwin::AttributeSetupLookup::btnAdd_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	CurrentAttribute = ref new ProductAttribute();
	CurrentAttributeValue = ref new GeneralAttributeValue();
	cmbAttribTypes->SelectedIndex = 1;
	txtAttribName->Focus(Windows::UI::Xaml::FocusState::Programmatic);
}



void scwin::AttributeSetupLookup::OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	currentAttrib = nullptr;
	currentAttribVal = nullptr;
}

void scwin::AttributeSetupLookup::txtAttributeValue_GotFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (this->currentAttrib == nullptr || currentAttrib->ObjectState == DataState::Added)
		return;

	auto asb = safe_cast<AutoSuggestBox^>(sender);
	if (!StringHelpers::IsNullOrEmpty(asb->Text))
		return;

	create_task(scLocalDataService::Instance()->GetProductAttributeValuesDistinctAsync(currentAttrib->Id, nullptr))
		.then([asb](IVector<String^>^ vect)
	{
		asb->ItemsSource = vect;
	});
	
}



void scwin::AttributeSetupLookup::txtUom_GotFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (this->currentAttrib == nullptr || currentAttrib->ObjectState == DataState::Added)
		return;

	auto asb = safe_cast<AutoSuggestBox^>(sender);
	if (!StringHelpers::IsNullOrEmpty(asb->Text))
		return;

	create_task(scLocalDataService::Instance()->GetAttributeValueDistinctUomsAsync(currentAttrib->Id, nullptr))
		.then([asb](IVector<String^>^ vect)
	{
		asb->ItemsSource = vect;
	});

}


void scwin::AttributeSetupLookup::AutoSuggestBox_TextChanged(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs^ args)
{
	if (this->currentAttrib == nullptr || currentAttrib->ObjectState == DataState::Added)
		return;

	if (args->Reason == AutoSuggestionBoxTextChangeReason::UserInput)
	{
		if (sender->Name == "txtAttributeValue")
		{
			delete sender->ItemsSource;
			create_task(scLocalDataService::Instance()->GetProductAttributeValuesDistinctAsync(currentAttrib->Id, sender->Text))
				.then([sender](IVector<String^>^ vect)
			{
				sender->ItemsSource = vect;
			});
		}
		else if (sender->Name == "txtUom")
		{
			delete sender->ItemsSource;
			create_task(scLocalDataService::Instance()->GetAttributeValueDistinctUomsAsync(currentAttrib->Id, sender->Text))
				.then([sender](IVector<String^>^ vect)
			{
				sender->ItemsSource = vect;
			});
		}
	}
}


void scwin::AttributeSetupLookup::AutoSuggestBox_QuerySubmitted(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxQuerySubmittedEventArgs^ args)
{
	// This event gets fired when:
	//     * a user presses Enter while focus is in the TextBox
	//     * a user clicks or tabs to and invokes the query button (defined using the QueryIcon API)
	//     * a user presses selects (clicks/taps/presses Enter) a suggestion
	// We are not using here because no action need to be taken on the string item in the itemssource.

	//if (sender->Name == "txtAttributeValue")
	//{

	//}
	//else if (sender->Name == "txtUom")
	//{

	//}
}


void scwin::AttributeSetupLookup::AutoSuggestBox_SuggestionChosen(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxSuggestionChosenEventArgs^ args)
{
	
	// This event is required only if the item in the ItemsSource (args->SelectedItem) is a object type and 
	// the displayed item on the autosuggestbox is a property from that object.
	//if (sender->Name == "txtAttributeValue")
	//{

	//}
	//else if (sender->Name == "txtUom")
	//{

	//}
}


void scwin::AttributeSetupLookup::txtFind_TextChanged(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs^ args)
{
	if (args->Reason == AutoSuggestionBoxTextChangeReason::UserInput)
	{
		// we will hide and show listview items
		auto items = lstAttributes->Items;
		UINT remainingItems = 0;
		for (UINT i = 0; i < items->Size; i++)
		{
			auto item = safe_cast<ProductAttribute^>(items->GetAt(i));
			auto lvi = safe_cast<ListViewItem^>(lstAttributes->ContainerFromItem(item));
			bool result = StringHelpers::Match(item->Name, sender->Text) || StringHelpers::Match(item->Type, sender->Text);
			if (result)
			{
				lvi->Visibility = Windows::UI::Xaml::Visibility::Visible;
				remainingItems++;
			}
			else
				lvi->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		}
		if (remainingItems)
		{
			lstAttributes->SelectedIndex = 0;
			CurrentAttribute = dynamic_cast<ProductAttribute^>(lstAttributes->Items->GetAt(0));
			delete currentAttribVal;
			CurrentAttributeValue = ref new GeneralAttributeValue();
		}
	}
}


void scwin::AttributeSetupLookup::lstAttributes_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	auto selectedItem = dynamic_cast<ProductAttribute^>(lstAttributes->SelectedItem);
	if (selectedItem != nullptr)
	{
		CurrentAttribute = selectedItem;
		CurrentAttributeValue = ref new GeneralAttributeValue();
	}
}

void scwin::AttributeSetupLookup::lstAttributes_ItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{

	auto selectedItem = dynamic_cast<ProductAttribute^>(e->ClickedItem);
	if (selectedItem != nullptr)
	{
		CurrentAttribute = selectedItem;
		CurrentAttributeValue = ref new GeneralAttributeValue();
	}
}
