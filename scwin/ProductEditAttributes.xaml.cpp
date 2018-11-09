//
// ProductEditAttributes.xaml.cpp
// Implementation of the ProductEditAttributes class
//

#include "pch.h"
#include "ProductEditAttributes.xaml.h"
#include "AttributeSetupLookup.xaml.h"
#include "FamilySetupLookup.xaml.h"

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

ProductEditAttributes::ProductEditAttributes()
{
	InitializeComponent();

	Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::ProductEditAttributes::OnLoaded);
	Unloaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::ProductEditAttributes::OnUnloaded);
}


void scwin::ProductEditAttributes::OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	ViewModel->LoadProductDataForPanel(3);
}


void scwin::ProductEditAttributes::OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	ViewModel->UnloadProductDataForPanel(3);
}


void scwin::ProductEditAttributes::OnApplyTemplate()
{
	UserControl::OnApplyTemplate();
}

void scwin::ProductEditAttributes::btnPricePlanEdit_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	ViewModel->EditPanel = 3;
	ViewModel->Attributes->VisualStateTrigger = VisualStateTriggers::WriteState;
}


void scwin::ProductEditAttributes::btnPricePlanSave_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	//ON complete
	WeakReference wr(this);
	create_task(ViewModel->SaveProductDataForPanel(3))
		.then([wr](int ret)
	{
		auto ths = wr.Resolve<ProductEditAttributes>();
		if (SUCCESS(MessageIds(ret)))
		{
			ths->ViewModel->EditPanel = -1;
			ths->ViewModel->Attributes->UpdateState();
		}
	});
}


void scwin::ProductEditAttributes::btnPricePlanCancel_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	// on complete 
	ViewModel->EditPanel = -1;
	ViewModel->Attributes->Reset();
}



void scwin::ProductEditAttributes::txtFamily_TextChanged(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs^ args)
{
	auto txt = sender->Text;
	if (StringHelpers::IsNullOrEmpty(txt))
	{
		sender->ItemsSource = nullptr;
		return;
	}
	if (args->Reason == AutoSuggestionBoxTextChangeReason::UserInput)
	{

		create_task(ViewModel->Attributes->GetMatchingFamiliesAsync(txt))
			.then([sender, txt](IVector<ProductFamily^>^ list)
		{
			sender->ItemsSource = list;
		});

		ViewModel->Attributes->UpdateFamilyName(txt);
	}
	else //if (args->Reason == AutoSuggestionBoxTextChangeReason::SuggestionChosen)
	{
		ViewModel->Attributes->UpdateFamilyName(txt);
	}
}

void scwin::ProductEditAttributes::txtFamily_LostFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (ViewModel->Attributes->Family == nullptr)
	{
		// double check familyname
		ViewModel->Attributes->UpdateFamilyName(txtFamily->Text);
		if (ViewModel->Attributes->Family == nullptr)
		{
			ViewModel->Attributes->Family = ref new ProductFamily();
			ViewModel->Attributes->Family->Name = txtFamily->Text;
		}
	}
}


void scwin::ProductEditAttributes::txtFamily_QuerySubmitted(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxQuerySubmittedEventArgs^ args)
{
	// This event gets fired when:
	//     * a user presses Enter while focus is in the TextBox
	//     * a user clicks or tabs to and invokes the query button (defined using the QueryIcon API)
	//     * a user presses selects (clicks/taps/presses Enter) a suggestion

	auto txt = args->QueryText;
	create_task(ViewModel->Attributes->GetMatchingFamiliesAsync(txt))
		.then([sender, txt](IVector<ProductFamily^>^ list)
	{
		sender->ItemsSource = list;
	});

}


void scwin::ProductEditAttributes::txtFamily_SuggestionChosen(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxSuggestionChosenEventArgs^ args)
{
	// This event is required only if the item in the ItemsSource (args->SelectedItem) is a object type and 
	// the displayed item on the autosuggestbox is a property from that object.
	auto family = dynamic_cast<ProductFamily^>(args->SelectedItem);
	if (family != nullptr)
	{
		ViewModel->Attributes->UpdateFamily(family);
		sender->Text = family->Name;
	}
}


void scwin::ProductEditAttributes::btnRemoveAttribute_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	using namespace Windows::UI::Popups;

	WeakReference wr(this);
	auto msgBox = ref new MessageDialog("Are you sure you want to disassociate this attribute from the product ?", "Disassociate Attribute");
	msgBox->CancelCommandIndex = 1;
	msgBox->DefaultCommandIndex = 1;
	msgBox->Commands->Append(ref new UICommand("Yes", nullptr, ref new Box<int>(0)));
	msgBox->Commands->Append(ref new UICommand("No", nullptr, ref new Box<int>(0)));
	msgBox->ShowAsync()->Completed = ref new AsyncOperationCompletedHandler<IUICommand^>([wr, sender](auto op, auto status)
	{
		auto cmd = op->GetResults();
		if (((IBox<int>^)cmd->Id)->Value == 1)
		{
			auto pav = dynamic_cast<ProductAttributeAndValue^>(((Control^)sender)->Tag);
			assert(pav != nullptr);
			create_task(scLocalDataService::Instance()->DeleteProductAttributeValueAsync(pav->Value->Id))
				.then([wr, pav](MessageIds ret)
			{
				auto ths = wr.Resolve<ProductEditAttributes>();
				if (SUCCESS(ret))
				{
					ths->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([wr, pav]()
					{
						// remove from local lsit
						auto ths = wr.Resolve<ProductEditAttributes>();
						UINT index;
						if (ths->ViewModel->Attributes->ProductAttributes->IndexOf(pav, &index))
						{
							ths->ViewModel->Attributes->ProductAttributes->RemoveAt(index);
						}
					}));
				}
				else
				{
					PubSubService::Instance->Publish(ths, PubSubMessageIds::ApplicationErrorStatusMessage, ref new String(GetMessage(ret)));
				}
			});
		}
	});
}


void scwin::ProductEditAttributes::btnSelectAddAttribute_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	AttributeSetupLookup^ dlg = ref new AttributeSetupLookup(ViewModel->Attributes);
	dlg->ShowAsync();
}


void scwin::ProductEditAttributes::btnFamilyLookup_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	FamilySetupLookup^ dlg = ref new FamilySetupLookup(ViewModel->Attributes);
	dlg->MinWidth = ActualWidth;
	dlg->ShowAsync();
}


void scwin::ProductEditAttributes::btnDefaultAttrib_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	auto pav = dynamic_cast<ProductAttributeAndValue^>(((Control^)sender)->Tag);
	if (pav != nullptr) // sure it is
	{
		WeakReference wr(this);
		create_task(scLocalDataService::Instance()->UpdateDefaultProductAttributeAsync(ViewModel->CurrentProduct->Id, pav->Attribute->Id))
			.then([wr, pav](MessageIds ret)
		{
			auto ths = wr.Resolve<ProductEditAttributes>();
			if (SUCCESS(ret))
			{
				pav->IsProductDefaultAttribute = true;
				// unset all others
				for (auto otherpav : ths->ViewModel->Attributes->ProductAttributes)
				{
					if (otherpav != pav)
					{
						otherpav->IsProductDefaultAttribute = false;
					}
				}
				PubSubService::Instance->Publish(ths, PubSubMessageIds::ApplicationInfoStatusMessage, "Successfully updated this attribute as product's default.");
			}
			else
			{
				PubSubService::Instance->Publish(ths, PubSubMessageIds::ApplicationErrorStatusMessage, "Failed to set the attribute as product's default.");
			}
		});
	}
}


