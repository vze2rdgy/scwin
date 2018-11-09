//
// SalesOrderView.xaml.cpp
// Implementation of the SalesOrderView class
//

#include "pch.h"
#include "SalesOrderView.xaml.h"
#include "cpplinq.hpp"
#include "XamlHelper.h"
#include <iomanip>

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

SalesOrderView::SalesOrderView()
{
	InitializeComponent();
	OrderFlow = ref new Vector<TransactionFlowItem^>();
}

void scwin::SalesOrderView::OnNavigatedTo(NavigationEventArgs ^ e)
{
	Page::OnNavigatedTo(e);

	using namespace cpplinq;

	ViewModel = dynamic_cast<SaleOrderViewModel^>(e->Parameter);

	if (ViewModel->CurrentOrder == nullptr)
		return;

	// customer viewmodel
	if (ViewModel->CustomerViewmodel == nullptr)
		ViewModel->CustomerViewmodel = ref new CustomerViewModel();

	auto wrapper = ViewModel->CurrentOrder;
	dtOrderExpectedDate->Date = wrapper->Order->CustomerExpectedDate->GetUWPDateTime();
	ViewModel->CurrentOrder = wrapper;


	if (wrapper->Order->ObjectState == DataState::Unmodified)
	{
		auto orderStatus = wrapper->OrderStatus;
		wrapper->OrderStatus =
			from(to_vector(ViewModel->OrderStatusList)) >>
		where([orderStatus](GeneralStatus^ gs)
		{
			return gs->Id == orderStatus->Id;
		}) >> first_or_default();
		auto payStatus = wrapper->PaymentStatus;
		if (payStatus != nullptr)
		{
			wrapper->PaymentStatus =
				from(to_vector(ViewModel->PaymentStatusList)) >>
			where([payStatus](GeneralStatus^ gs)
			{
				return gs->Id == payStatus->Id;
			}) >> first_or_default();
		}
	}

	DisplayOrderStatusHistory();


	// if sales order has already costcenter and contact selections, update them in custviewmodel
	create_task([=]() -> void
	{
		if (this == nullptr) return;


		auto c = wrapper->Contact;

		if (c == nullptr)
		{
			LogException("Contact information is invalid.");
			return;
		}

		auto org = wrapper->Customer;
		auto cc = wrapper->CostCenter;

		if (org != nullptr)
		{
			auto orgs = CustViewModel->Organizations;
			if (orgs != nullptr)
			{
				auto ov = to_vector(orgs);
				auto forg = from(ov) >> where([org](CustomerDtosGroupedBindable^ o)
				{
					return org->Id == o->Organization->Id;
				}) >> first_or_default();
				if (forg != nullptr)
				{
					// check default cost center
					if (cc != nullptr)
					{
						if (cc->Id == forg->DefaultCostCenter->CostCenter->Id)
						{
							forg->DefaultCostCenter->IsCostCenterSelectedForSalesOrder = true;
							if (c != nullptr)
							{
								for (auto cts : forg->DefaultCostCenter->Contacts)
								{
									if (cts->Value->Id == c->Id)
									{
										cts->IsContactSelectedForSalesOrder = true;
										return;
									}
								}
							}
						}
						else
						{
							for (auto fcc : forg->CostCenters)
							{
								if (fcc->CostCenter->Id == c->Id)
								{
									fcc->IsCostCenterSelectedForSalesOrder = true;
									if (c != nullptr)
									{
										for (auto cts : fcc->Contacts)
										{
											if (cts->Value->Id == c->Id)
											{
												cts->IsContactSelectedForSalesOrder = true;
												return;
											}
										}
									}
									return;
								}
							}
						}
					}
				}
			}
		}

	});

}

void scwin::SalesOrderView::OnNavigatingFrom(NavigatingCancelEventArgs ^ e)
{
	Page::OnNavigatingFrom(e);
}

void scwin::SalesOrderView::MainCommandBar_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (Windows::Foundation::Metadata::ApiInformation::IsPropertyPresent("Windows.UI.Xaml.Controls.CommandBar", "DefaultLabelPosition"))
	{
		if (Windows::System::Profile::AnalyticsInfo::VersionInfo->DeviceFamily == "Windows.Mobile")
		{
			((CommandBar^)sender)->DefaultLabelPosition = CommandBarDefaultLabelPosition::Bottom;
		}
		else
		{
			((CommandBar^)sender)->DefaultLabelPosition = CommandBarDefaultLabelPosition::Right;
		}
	}

}

void scwin::SalesOrderView::FindProduct(FrameworkElement^ sender, String^ text)
{
	ostringstream osql;
	auto txt = STRCONV(text->Data());
	if (!txt.empty())
		osql << "skun like '%" << txt << "%' or name like '%" << txt << "%'";
	auto productList = scLocalDataService::Instance()->GetProducts(osql.str());
	auto fly = (Flyout^)FlyoutBase::GetAttachedFlyout(sender);
	ChooseAProduct^ content = dynamic_cast<ChooseAProduct^>(fly->Content);
	if (content != nullptr)
	{
		content->ProductList = productList;
		content->Filter = text;
	}
	fly->ShowAt(sender);
}


void scwin::SalesOrderView::txtProductName_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	auto contentctl = (ContentControl^)sender;
	// find the textbox

	DependencyObject^ tb;
	if (XamlHelper::GetChildFromTree(contentctl, "txtProd", &tb))
	{
		FindProduct(contentctl, ((TextBox^)tb)->Text);
	}

	e->Handled = true;
}


void scwin::SalesOrderView::Button_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	e->Handled = false;
}


void scwin::SalesOrderView::txtProductName_KeyDown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
	auto origSource = dynamic_cast<TextBox^>(e->OriginalSource);
	if (origSource != nullptr)
	{
		auto contentctl = (ContentControl^)sender;
		if (e->Key == Windows::System::VirtualKey::Enter)
		{
			FindProduct(contentctl, origSource->Text);
		}
	}
}

void scwin::SalesOrderView::TextBox_KeyDown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
	e->Handled = false;
}


void scwin::SalesOrderView::ChooseAProduct_ProductSelected(scwin::ChooseAProduct^ sender, scwin::ProductWithInventoryWrapper^ productWrapper)
{
	using namespace cpplinq;

	auto dc = dynamic_cast<SalesOrderDetailsWrapper^>(sender->DataContext);
	dc->Message = "";
	dc->InnerItem->ProductId = productWrapper->ProductItem->Id;
	dc->ProductName = productWrapper->ProductItem->Name;
	dc->TitleImage = productWrapper->ProductItem->TitleImage;
	dc->ProductItem = productWrapper;

	if (productWrapper->PricePlan->IsTieredPricePlan)
	{
		auto vpdets = to_vector(productWrapper->PricePlan->PriceDetails);
		auto orderedpricelist = from(vpdets) >> orderby([](IPricePlanDetails^ pdets)
		{
			return ((PricePlanTierDetails^)pdets)->BeginQuantity;
		}) >> to_vector();
		if (orderedpricelist.size() > 1)
		{
			wstringstream ostr;
			ostr << L"This product has a tiered price plan ranging between " << std::fixed << setprecision(2) << orderedpricelist.at(0)->Price
				<< " and " << orderedpricelist.at(orderedpricelist.size() - 1)->Price << ". When you enter the "
				<< "quantity, appropriate price will be displayed on the price column. Depending on your functional rights, the price "
				<< "may be manually adjusted in the price column, if your sales requires so.";
			dc->Message = ref new String(ostr.str().c_str());
		}
	}
	else
	{
		// update unit price on sales order detail item
		dc->UnitPrice = productWrapper->PricePlan->DefaultPrice->Price;
	}
}


void scwin::SalesOrderView::flyExpeditedView_Opening(Platform::Object^ sender, Platform::Object^ e)
{
	auto flyo = dynamic_cast<Flyout^>(sender);
	if (flyo == nullptr) return;
	DependencyObject^ dp;
	if (XamlHelper::GetChildFromTree(flyo->Content, "dpOrderItemExpectedDate", &dp))
	{
		auto dpicker = (DatePicker^)dp;
		auto salesOrderDetails = dynamic_cast<SalesOrderDetailsWrapper^>(dpicker->DataContext);
		if (salesOrderDetails != nullptr)
		{
			auto cal = ref new Calendar();
			auto saveddate = salesOrderDetails->InnerItem->CustomerExpectedDate;
			if (saveddate == nullptr)
			{
				saveddate = ViewModel->CurrentOrder->Order->CustomerExpectedDate;
			}
			dpicker->Date = saveddate->GetUWPDateTime();
		}
	}
}


void scwin::SalesOrderView::flyExpeditedView_Closed(Platform::Object^ sender, Platform::Object^ e)
{
	auto flyo = dynamic_cast<Flyout^>(sender);
	if (flyo == nullptr) return;
	DependencyObject^ dp;
	if (XamlHelper::GetChildFromTree(flyo->Content, "dpOrderItemExpectedDate", &dp))
	{
		auto dpicker = (DatePicker^)dp;
		auto salesOrderDetails = dynamic_cast<SalesOrderDetailsWrapper^>(dpicker->DataContext);
		if (salesOrderDetails != nullptr)
		{
			auto selecteddate = dpicker->Date;
			// convert
			Date^ appDate = ref new Date(selecteddate);
			salesOrderDetails->InnerItem->CustomerExpectedDate = appDate;
		}
	}
}


void scwin::SalesOrderView::btnAddOrderItem_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto orderId = ViewModel->CurrentOrder->Order->Id;
	ViewModel->CurrentOrder->OrderDetails->Append(ref new SalesOrderDetailsWrapper(orderId));
}


void scwin::SalesOrderView::dtOrderExpectedDate_DateChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::DatePickerValueChangedEventArgs^ e)
{

}


void scwin::SalesOrderView::txtOrderHeaderDiscountType_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

}


void scwin::SalesOrderView::btnRemoveOrderLine_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	using namespace Windows::UI::Popups;

	auto dc = dynamic_cast<SalesOrderDetailsWrapper^>(((Button^)sender)->DataContext);
	if (dc != nullptr)
	{
		auto cont = lvOrderDetails->ContainerFromItem(dc);
		auto index = lvOrderDetails->IndexFromContainer(cont);

		if (dc->ProductItem == nullptr)
		{
			ViewModel->CurrentOrder->OrderDetails->RemoveAt(index);
			return;
		}

		auto msg = ref new MessageDialog("Do you want to removed this order item ?", "Remove Order Item");
		msg->Commands->Append(ref new UICommand("Yes", ref new UICommandInvokedHandler([=](IUICommand^ cmd)
		{
			ViewModel->CurrentOrder->OrderDetails->RemoveAt(index);
		})));
		msg->Commands->Append(ref new UICommand("No"));
		msg->ShowAsync();
	}
}


void scwin::SalesOrderView::ButtonRevertOrder_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	using namespace Windows::UI::Popups;
	WeakReference wr(this);
	// ask user
	auto msgDialog = ref new MessageDialog("Do you want to revert the order to original state? Your changes will be lost.", "Revert Order");
	msgDialog->Commands->Append(ref new UICommand("Yes", ref new UICommandInvokedHandler([wr](IUICommand^ cmd)
	{
		auto ths = wr.Resolve<SalesOrderView>();
		// if the order is new, simply reload a new order.
		ths->ViewModel->RevertCurrentOrder();
		// update date control since they are not databoundable
		ths->dtOrderExpectedDate->Date = ths->ViewModel->CurrentOrder->Order->CustomerExpectedDate->GetUWPDateTime();

		ths->DisplayOrderStatusHistory();

	})));
	msgDialog->Commands->Append(ref new UICommand("No"));
	msgDialog->ShowAsync();
}


void scwin::SalesOrderView::ButtonSaveOrder_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	try
	{
		ViewModel->SaveCurrentOrder();
	}
	catch (Exception^ ex)
	{
		PubSubService::Instance->Publish(this, PubSubMessageIds::ApplicationErrorStatusMessage, "Saving Order Failed with a message '" + ex->Message + "'");
	}
}

void scwin::SalesOrderView::DisplayOrderStatusHistory()
{

	OrderFlow->Clear();

	if (ViewModel->CurrentOrder == nullptr) return;

	using namespace sggraphs;
	// update orderflow diagram
	auto item = ref new TransactionFlowItem();
	item->RenderType = TransactionFlowItemRenderType::StartNode;
	OrderFlow->Append(item);

	for (auto osh : ViewModel->CurrentOrder->OrderStatusHistory)
	{
		item = ref new TransactionFlowItem();
		item->RenderType = TransactionFlowItemRenderType::ForwardConnector;
		OrderFlow->Append(item);

		item = ref new TransactionFlowItem();
		item->RenderType = TransactionFlowItemRenderType::Process;
		item->Header = osh->OrderStatus->PrivateName;
		item->SubHeader = String::Concat("Changed On ", osh->OrderStatusDate->ToString());
		item->Text = String::Concat("By ", osh->CreatedBy);
		OrderFlow->Append(item);

	}

	item = ref new TransactionFlowItem();
	item->RenderType = TransactionFlowItemRenderType::ForwardConnector;
	OrderFlow->Append(item);

	item = ref new TransactionFlowItem();
	item->RenderType = TransactionFlowItemRenderType::EndNode;
	OrderFlow->Append(item);

}
