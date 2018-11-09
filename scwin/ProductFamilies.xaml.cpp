//
// ProductFamilies.xaml.cpp
// Implementation of the ProductFamilies class
//

#include "pch.h"
#include "ProductFamilies.xaml.h"
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

DependencyProperty^ ProductFamilies::extendedProperty = nullptr;

void ProductFamilies::Register()
{
	if (extendedProperty == nullptr)
		extendedProperty = DependencyProperty::Register("Extended", bool::typeid, ProductFamilies::typeid, ref new PropertyMetadata(false));
}

ProductFamilies::ProductFamilies()
{
	InitializeComponent();

	Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::ProductFamilies::OnLoaded);
	Unloaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::ProductFamilies::OnUnloaded);

}

void scwin::ProductFamilies::OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	if (SelectedProducts != nullptr)
	{
		// observe changes to collection changes so that we know when to trigger edit mode
		auto olist = dynamic_cast<IObservableVector<Product^>^>(SelectedProducts);
		if (olist != nullptr)
		{
			selectedProductsChanged = olist->VectorChanged += ref new VectorChangedEventHandler<Product^>(this, &ProductFamilies::SelectedProductsListChanged);
		}
	}

	WeakReference wr(this);
	create_task(scLocalDataService::Instance()->GetProductFamiliesAsync(""))
		.then([wr](IVector<ProductFamily^>^ families)
	{
		auto ths = wr.Resolve<ProductFamilies>();
		ths->AllFamilies = families;
		if (families->First()->HasCurrent)
			ths->SelectedFamily = families->First()->Current;
	});

}


void scwin::ProductFamilies::txtFindFamily_TextChanged(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs^ args)
{
	AllFamilies = nullptr;
	string findStr = STRSAFECONV(txtFindFamily->Text);
	ostringstream ostr;
	ostr << "name like '%" << findStr << "%' "
		<< "or shortdesc like '%" << findStr << "%' "
		<< "or notes like '%" << findStr << "%' ";
	WeakReference wr(this);
	create_task(scLocalDataService::Instance()->GetProductFamiliesAsync(ostr.str()))
		.then([wr](IVector<ProductFamily^>^ families)
	{
		auto ths = wr.Resolve<ProductFamilies>();
		ths->AllFamilies = families;
		if (families->First()->HasCurrent)
			ths->SelectedFamily = families->First()->Current;
		else
			ths->SelectedFamily = ref new ProductFamily();
	});
}


void scwin::ProductFamilies::listOfFamilies_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	if (Extended && e->AddedItems->Size > 0)
	{
		auto selectedItem = dynamic_cast<ProductFamily^>(e->AddedItems->First()->Current);
		if (selectedItem != nullptr)
		{
			WeakReference wr(this);
			create_task(scLocalDataService::Instance()->GetProductFamilyDefaultAttributeAsync(selectedItem->Id))
				.then([wr](ProductAttributeAndValue^ result)
			{
				auto ths = wr.Resolve<ProductFamilies>();
				if (result != nullptr)
				{
					ths->DefaultAttribute = result->Attribute;
					ths->DefaultAttributeValue = result->Value;

					if (ths->DefaultAttributeValue != nullptr)
					{
						ths->txtAttrValueName->Text = dynamic_cast<String^>(ths->DefaultAttributeValue->Value);
						ths->txtAttrValueUom->Text = ths->DefaultAttributeValue->Uom;
					}
				}
				else
				{
					ths->txtAttrValueName->Text = "";
					ths->txtAttrValueUom->Text = "";
				}
			});
		}
	}
}

void scwin::ProductFamilies::UpdateSelectedItem(int selectedIndex)
{
	if (listOfFamilies->Items->Size > 0)
		listOfFamilies->SelectedIndex = selectedIndex;
}

DEFINERAISEPROPCHANGEDBASIC(ProductFamilies)

void scwin::ProductFamilies::ListProductsInFamily(String ^ familyId)
{
	if (StringHelpers::IsNullOrEmpty(familyId))
	{
		SelectedProducts = nullptr;
		return;
	}

	WeakReference wr(this);
	create_task(scLocalDataService::Instance()->GetProductsInFamilyAsync(familyId))
		.then([wr](IVector<Product^>^  products)
	{
		auto ths = wr.Resolve<ProductFamilies>();
		ths->SelectedProducts = products;
		ths->RaisePropertyChanged("SelectedProducts");
	});
}

void scwin::ProductFamilies::btnUndoFamily_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (SelectedFamily->ObjectState == DataState::Added)
	{
		allFamilies->RemoveAtEnd();
		listOfFamilies->SelectedIndex = lastSelectedIndex;
	}
	else if (SelectedFamily->ObjectState == DataState::Edited)
	{
		// replace from database
		auto pfam = scLocalDataService::Instance()->GetProductFamily(SelectedFamily->Id);
		_ASSERT(pfam != nullptr);
		auto idx = listOfFamilies->SelectedIndex;
		//allFamilies->RemoveAt(idx);
		//allFamilies->InsertAt(idx, pfam);
		allFamilies->SetAt(idx, pfam);
		//listOfFamilies->ItemsSource = allFamilies;
		listOfFamilies->SelectedIndex = idx;
	}

	// restore remove list
	for (auto p : removeUndoList)
	{
		SelectedProducts->Append(p);
	}
}


void scwin::ProductFamilies::btnDeleteFamily_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	//TODO: Remove the attribute (default) used by on this family and remove family attribute from products that uses this attribute.



	lastSelectedIndex = listOfFamilies->SelectedIndex;
	if (lastSelectedIndex >= 0)
	{
		auto familyId = allFamilies->GetAt(lastSelectedIndex)->Id;
		// allert user
		using namespace Windows::UI::Popups;
		String^ msg;
		if (SelectedProducts->Size > 0)
			msg = "Deleting this product family will result in disassociating all member products from this family. Do you want to continue ?";
		else
			msg = "Do you want to delete this family ?";
		auto dlg = ref new MessageDialog(msg, "Delete Family");
		WeakReference wr(this);
		dlg->Commands->Append(ref new UICommand("Yes", ref new UICommandInvokedHandler([wr, familyId](IUICommand^ cmd)
		{
			auto ths = wr.Resolve<ProductFamilies>();
			auto status = scLocalDataService::Instance()->DeleteProductFamily(familyId);
			if (HASFAILED(status))
			{
				PubSubService::Instance->Publish(ths, PubSubMessageIds::ApplicationErrorStatusMessage, "Failed to delete product family. Please retry.");
				return;
			}
			ths->allFamilies->RemoveAt(ths->lastSelectedIndex);
		})));
		dlg->Commands->Append(ref new UICommand("No"));
		dlg->DefaultCommandIndex = 1;
		dlg->CancelCommandIndex = 1;
		dlg->ShowAsync();
	}

}

void scwin::ProductFamilies::btnAddFamily_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	lastSelectedIndex = listOfFamilies->SelectedIndex;
	auto f = ref new ProductFamily();
	allFamilies->Append(f);
	auto lbi = dynamic_cast<ListBoxItem^>(listOfFamilies->ContainerFromItem(f));
	lbi->IsSelected = true;
	DefaultAttribute = nullptr;
	DefaultAttributeValue = nullptr;
}


void scwin::ProductFamilies::btnSaveFamily_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	WeakReference wr(this);

	// validate
	if (SelectedFamily->IsValid)
	{
		DataState ds = SelectedFamily->ObjectState;

		auto ths = this;
		LOCSVC->StartTransaction();
		MessageIds status;

		if (ths->Extended && ths->chkApplyAttributeToAllMemberProducts->IsChecked)
		{
			// update member products
			using namespace cpplinq;
			auto vprods = to_vector(ths->SelectedProducts);
			auto vproductIds = from(vprods) >> select([](Product^ p)
			{
				return p->Id;
			}) >> to_vector();

			Vector<String^>^ prodids = ref new  Vector<String^>(vproductIds);

			status = LOCSVC->UpdateProductFamilyAndAssociateWithProducts(ths->SelectedFamily, prodids);

			if (HASFAILED(status))
			{
				PubSubService::Instance->Publish(ths, PubSubMessageIds::ApplicationErrorStatusMessage, "Failed to save family and associate member products with the family.");
				return;
			}

			// delink any removed products
			for (auto p : removeUndoList)
			{
				status = LOCSVC->DisassociateProductFromFamily(p->Id);
				if (HASFAILED(status))
				{
					PubSubService::Instance->Publish(ths, PubSubMessageIds::ApplicationErrorStatusMessage, "Failed to disassociate products which are removed from this family.");
					return;
				}
			}

		}
		else
		{
			status = LOCSVC->UpdateProductFamily(SelectedFamily, nullptr);
		}

		if (HASFAILED(status))
		{
			if (ds == DataState::Added)
			{
				ths->allFamilies->RemoveAtEnd();
			}

			ths->listOfFamilies->SelectedIndex = ths->lastSelectedIndex;
			PubSubService::Instance->Publish(ths, PubSubMessageIds::ApplicationErrorStatusMessage, "Failed to save product family. Please retry.");

			LOCSVC->CancelTransaction();

			return;

		}


		if (ths->Extended)
		{
			bool hasError = false;
			// Resolve attribute part first
			if (!StringHelpers::IsNullOrEmpty(ths->txtAttribuetName->Text))
			{
				if (ths->cmbAttrTypes->SelectedIndex < 0)
				{
					// attribute type is not selected alert he user.
					PubSubService::Instance->Publish(ths,
						PubSubMessageIds::ApplicationErrorStatusMessage, "Failed to save product family, because Attribute's Type is not selected (see Default Attribute section).");
					hasError = true;
				}
				if (StringHelpers::IsNullOrEmpty(ths->txtAttrValueName->Text))
				{
					// value is not provided, alert he user.
					PubSubService::Instance->Publish(ths,
						PubSubMessageIds::ApplicationErrorStatusMessage, "Failed to save product family, because Attribute's Value is required if Attribute's Name is provided (see Default Attribute section).");
					hasError = true;
				}
			}

			if (hasError)
			{
				status = MessageIds::SaveFailed;
				LOCSVC->CancelTransaction();
				return;
			}

			if (SUCCESS(status))
			{
				if (!StringHelpers::IsNullOrEmpty(ths->txtAttribuetName->Text))
				{
					// create a new default attribute
					if (ths->DefaultAttribute == nullptr)
					{
						ths->DefaultAttribute = ref new ProductAttribute();
						ths->DefaultAttribute->Name = ths->txtAttribuetName->Text;
						ths->DefaultAttribute->Type = (String^)ths->cmbAttrTypes->SelectedItem;
						ths->DefaultAttribute->Description = ths->txtAttrDesc->Text;
					}

					if (ths->DefaultAttributeValue == nullptr)
					{
						ths->DefaultAttributeValue = ref new GeneralAttributeValue();
						ths->DefaultAttributeValue->Value = ths->txtAttrValueName->Text;
						ths->DefaultAttributeValue->Uom = ths->txtAttrValueUom->Text;
					}

					status = LOCSVC->UpdateProductFamilyDefaultAttribute(ths->SelectedFamily->Id, ths->DefaultAttribute, ths->DefaultAttributeValue);
					if (HASFAILED(status))
					{
						PubSubService::Instance->Publish(ths,
							PubSubMessageIds::ApplicationErrorStatusMessage, "Failed to save family default attribute. Reported error - " + ref new String(GetMessage(status)));
						LOCSVC->CancelTransaction();
						return;
					}

					if (chkApplyAttributeToAllMemberProducts->IsChecked)
					{
						for (auto p : SelectedProducts)
						{
							auto attrValue = ref new GeneralAttributeValue();
							attrValue->Value = ths->DefaultAttributeValue->Value;
							attrValue->Uom = ths->DefaultAttributeValue->Uom;
							status = LOCSVC->UpdateProductAttribute(p->Id, ths->DefaultAttribute, attrValue);
							if (HASFAILED(status))
							{
								PubSubService::Instance->Publish(ths,
									PubSubMessageIds::ApplicationErrorStatusMessage,
									"Failed to apply save family attribute to a member product. Reported error - " + ref new String(GetMessage(status)));
								LOCSVC->CancelTransaction();
							}
							status = LOCSVC->UpdateDefaultProductAttribute(p->Id, ths->DefaultAttribute->Id);
							if (HASFAILED(status))
							{
								PubSubService::Instance->Publish(ths,
									PubSubMessageIds::ApplicationErrorStatusMessage, 
									"Failed to apply default attribute to a member product. Reported error - " + ref new String(GetMessage(status)));
								LOCSVC->CancelTransaction();
							}
							return;
						}
					}

				}

			}
		}


		// Finally
		PubSubService::Instance->Publish(ths,
			PubSubMessageIds::ApplicationInfoStatusMessage,
			"Product family is saved.");

		using namespace cpplinq;
		auto v = to_vector(ths->allFamilies);
		String^ id = ths->SelectedFamily->Id;
		auto itr = find_if(v.begin(), v.end(), [id](ProductFamily^ f)
		{
			return f->Id == id;
		});
		if (itr != v.end())
		{
			auto item = (*itr);
			item->CanRaiseEvents = false;
			item->ObjectState = DataState::Unmodified;
			item->CanRaiseEvents = true;
			item->NotifyObjectStateChange();
			ListBoxItem^ lstItem;
			lstItem = dynamic_cast<ListBoxItem^>(ths->listOfFamilies->ContainerFromItem(item));
			if (lstItem != nullptr)
			{
				lstItem->IsSelected = true;
			}
		}
	}
}


void scwin::ProductFamilies::OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	if (SelectedFamily == nullptr) return;

	if (SelectedFamily->ObjectState == DataState::Added)
	{
		AllFamilies->RemoveAtEnd();
	}
	auto olist = dynamic_cast<IObservableVector<Product^>^>(SelectedProducts);
	if (olist != nullptr)
	{
		olist->VectorChanged -= selectedProductsChanged;
	}
}




void scwin::ProductFamilies::txtAttribuetName_TextChanged(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs^ args)
{
	WeakReference wr(this);
	switch (args->Reason)
	{
	case AutoSuggestionBoxTextChangeReason::UserInput:
		sender->ItemsSource = nullptr;
		create_task(scLocalDataService::Instance()->GetAttributesAsync("name like '%" + sender->Text + "%'", " name "))
			.then([wr, sender](IVector<ProductAttribute^>^ attributes)
		{
			auto ths = wr.Resolve<ProductFamilies>();
			sender->ItemsSource = attributes;
			if (!attributes->Size)
			{
				if (ths->DefaultAttribute == nullptr ||
					ths->DefaultAttribute->ObjectState != DataState::Added)
				{
					ths->DefaultAttribute = ref new ProductAttribute();
				}
				ths->DefaultAttribute->Name = sender->Text;
			}
		});
		break;
	case AutoSuggestionBoxTextChangeReason::SuggestionChosen:
		break;
	case AutoSuggestionBoxTextChangeReason::ProgrammaticChange:
		break;
	}
}


void scwin::ProductFamilies::txtAttribuetName_SuggestionChosen(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxSuggestionChosenEventArgs^ args)
{
	DefaultAttribute = safe_cast<ProductAttribute^>(args->SelectedItem);
}


void scwin::ProductFamilies::txtAttribuetName_QuerySubmitted(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxQuerySubmittedEventArgs^ args)
{
	if (args->ChosenSuggestion != nullptr)
	{
		DefaultAttribute = safe_cast<ProductAttribute^>(args->ChosenSuggestion);
	}
	else
	{
		WeakReference wr(this);
		create_task(scLocalDataService::Instance()->GetAttributesAsync("name like '%" + args->QueryText + "%'", " name "))
			.then([sender, wr](IVector<ProductAttribute^>^ attributes)
		{
			sender->ItemsSource = attributes;
			auto ths = wr.Resolve<ProductFamilies>();
			if (attributes->Size > 0)
			{
				ths->DefaultAttribute = attributes->GetAt(0);
			}
		});
	}
}


void scwin::ProductFamilies::txtAttrValueName_TextChanged(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs^ args)
{
	WeakReference wr(this);
	if (DefaultAttribute == nullptr) return;
	switch (args->Reason)
	{
	case AutoSuggestionBoxTextChangeReason::SuggestionChosen:
	case AutoSuggestionBoxTextChangeReason::UserInput:
		create_task(scLocalDataService::Instance()->GetProductAttributeValuesDistinctAsync(DefaultAttribute->Id, sender->Text ))
			.then([wr, sender](IVector<String^>^ values)
		{
			sender->ItemsSource = values;
		});
		break;
	}

	if (SelectedFamily->ObjectState == DataState::Unmodified)
	{
		SelectedFamily->ObjectState = DataState::Edited;
	}

}


void scwin::ProductFamilies::txtAttrValueName_SuggestionChosen(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxSuggestionChosenEventArgs^ args)
{
	if (DefaultAttribute == nullptr) return;
	DefaultAttributeValue = nullptr;
	DefaultAttributeValue = ref new GeneralAttributeValue();
	DefaultAttributeValue->Value = safe_cast<String^>(args->SelectedItem);
}


void scwin::ProductFamilies::txtAttrValueName_QuerySubmitted(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxQuerySubmittedEventArgs^ args)
{
	if (DefaultAttribute == nullptr) return;
	if (args->ChosenSuggestion != nullptr)
	{
		DefaultAttributeValue = nullptr;
		DefaultAttributeValue = ref new GeneralAttributeValue();
		DefaultAttributeValue->Value = safe_cast<String^>(args->ChosenSuggestion);
	}
	else
	{
		WeakReference wr(this);
		sender->ItemsSource = nullptr;
		create_task(scLocalDataService::Instance()->GetProductAttributeValuesDistinctAsync(DefaultAttribute->Id, args->QueryText))
			.then([sender, wr](IVector<String^>^ values)
		{
			auto ths = wr.Resolve<ProductFamilies>();
			sender->ItemsSource = values;
			if (values->Size)
			{
				ths->DefaultAttributeValue = nullptr;
				ths->DefaultAttributeValue= ref new GeneralAttributeValue();
				ths->DefaultAttributeValue->Value = values->GetAt(0);
			}
		});
	}

}


void scwin::ProductFamilies::txtAttrValueUom_TextChanged(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs^ args)
{
	if (DefaultAttribute == nullptr) return;
	switch (args->Reason)
	{
	case AutoSuggestionBoxTextChangeReason::SuggestionChosen:
	case AutoSuggestionBoxTextChangeReason::UserInput:
		sender->ItemsSource = nullptr;
		create_task(scLocalDataService::Instance()->GetAttributeValueDistinctUomsAsync(DefaultAttribute->Id, sender->Text))
			.then([sender](IVector<String^>^ vect)
		{
			sender->ItemsSource = vect;
		});
		break;
	}

	if (SelectedFamily->ObjectState == DataState::Unmodified)
	{
		SelectedFamily->ObjectState = DataState::Edited;
	}

}


void scwin::ProductFamilies::txtAttrValueUom_SuggestionChosen(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxSuggestionChosenEventArgs^ args)
{
	if (DefaultAttribute== nullptr) return;
	if (DefaultAttributeValue == nullptr) return;
	DefaultAttributeValue->Uom = safe_cast<String^>(args->SelectedItem);
}


void scwin::ProductFamilies::txtAttrValueUom_QuerySubmitted(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxQuerySubmittedEventArgs^ args)
{
	if (DefaultAttributeValue == nullptr) return;
	if (args->ChosenSuggestion != nullptr)
	{
		DefaultAttributeValue->Value = safe_cast<String^>(args->ChosenSuggestion);
	}
	else
	{
		WeakReference wr(this);
		sender->ItemsSource = nullptr;
		create_task(scLocalDataService::Instance()->GetAttributeValueDistinctUomsAsync(DefaultAttribute->Id, args->QueryText))
			.then([sender, wr](IVector<String^>^ values)
		{
			auto ths = wr.Resolve<ProductFamilies>();
			sender->ItemsSource = values;
			if (values->Size)
			{
				ths->DefaultAttributeValue->Uom = values->GetAt(0);
			}
		});
	}
}

void scwin::ProductFamilies::ClearValues()
{
	cmbAttrTypes->SelectedIndex = -1;
	txtAttrDesc->Text = nullptr;
	txtAttrValueUom->Text = nullptr;
}



void scwin::ProductFamilies::btnAddProductsToFamily_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

	auto vselected = to_vector(SelectedProducts);

	using namespace cpplinq;
	WeakReference wr(this);
	create_task(LOCSVC->GetProductsAsync())
		.then([wr, vselected](ProductList^ products)
	{
		auto ths = wr.Resolve<ProductFamilies>();
		ths->AllProducts = ref new Vector<ProductSelectable^>(
			from(to_vector(products)) >>
			select([&vselected](Product^ p) 
		{ 
			auto hasit = find_if(vselected.cbegin(), vselected.cend(), [&p](Product^ psel)
			{
				return p->Id == psel->Id;
			}) != vselected.cend();
			return ref new ProductSelectable(p, hasit);
		}) >>
			to_vector());
		ths->RaisePropertyChanged("AllProducts");
	});

	if (SelectedFamily != nullptr && SelectedFamily->ObjectState == DataState::Unmodified)
	{
		listOfFamilies->IsEnabled = false;
	}
	ProductSelectList->IsOpen = true;
}


void scwin::ProductFamilies::ProductSelectList_Closed(Platform::Object^ sender, Platform::Object^ e)
{
	AllProducts = nullptr;
	if (SelectedFamily != nullptr && SelectedFamily->ObjectState == DataState::Unmodified)
	{
		listOfFamilies->IsEnabled = true;
	}
}


void scwin::ProductFamilies::btnRemoveProduct_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto dc = dynamic_cast<Product^>(safe_cast<Button^>(sender)->DataContext);
	auto v = to_vector(SelectedProducts);
	auto ftr = find_if(v.cbegin(), v.cend(), [dc](Product^ p)
	{
		return dc->Id == p->Id;
	});
	if (ftr != v.cend())
	{
		removeUndoList.emplace_back(*ftr);
		SelectedProducts->RemoveAt((UINT)(ftr - v.begin()));
	}
}

void scwin::ProductFamilies::SelectedProductsListChanged(IObservableVector<Product^>^ sender, IVectorChangedEventArgs ^ args)
{
	if (SelectedFamily != nullptr && SelectedFamily->ObjectState == DataState::Unmodified)
	{
		SelectedFamily->ObjectState = DataState::Edited;
	}
}
