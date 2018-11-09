//
// COrgEditorView.xaml.cpp
// Implementation of the COrgEditorView class
//

#include "pch.h"
#include "COrgEditorView_older.xaml.h"
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
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Xaml::Hosting;
using namespace Windows::UI::Xaml::Media::Animation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

COrgEditorView::COrgEditorView()
{
	//auto dispInfo = DisplayInformation::GetForCurrentView();
	//if (dispInfo != nullptr)
	//{
	//	islandscape = (dispInfo->CurrentOrientation == DisplayOrientations::Landscape ||
	//		dispInfo->CurrentOrientation == DisplayOrientations::LandscapeFlipped);
	//	dispOrientationTok = dispInfo->OrientationChanged += ref new Windows::Foundation::TypedEventHandler<Windows::Graphics::Display::DisplayInformation ^, Platform::Object ^>(this, &scwin::COrgEditorView::OnOrientationChanged);
	//}
	//else
	//{
	//	islandscape = true;
	//}

	InitializeComponent();

	//Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::COrgEditorView::OnLoaded);
	//Unloaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::COrgEditorView::OnUnloaded);
	//SizeChanged += ref new Windows::UI::Xaml::SizeChangedEventHandler(this, &scwin::COrgEditorView::OnSizeChanged);

	compositor = ElementCompositionPreview::GetElementVisual(this)->Compositor;
	TimeSpan span;
	span.Duration = 4500000;
	auto animation = compositor->CreateScalarKeyFrameAnimation();
	animation->Target = "Translation.Y";
	animation->Duration = span;
	animation->InsertKeyFrame(0.0, -450.0);
	animation->InsertKeyFrame(1.0, 0.0);
	ElementCompositionPreview::SetIsTranslationEnabled(gridTitle, true);
	ElementCompositionPreview::SetImplicitShowAnimation(gridTitle, animation);

	// opacity animation for main content
	animation = compositor->CreateScalarKeyFrameAnimation();
	//span.Duration = 4000000;
	animation->Duration = span;
	animation->Target = "Opacity";
	animation->InsertKeyFrame(0, 0);
	animation->InsertKeyFrame(0.25, .25);
	animation->InsertKeyFrame(0.50, 0.50);
	animation->InsertKeyFrame(0.75, 0.75);
	animation->InsertKeyFrame(1, 1);
	ElementCompositionPreview::SetImplicitShowAnimation(OrgEditorPanel, animation);


	animation = compositor->CreateScalarKeyFrameAnimation();
	animation->Target = "Translation.Y";
	animation->Duration = span;
	animation->InsertKeyFrame(1.0, -30.0);
	ElementCompositionPreview::SetIsTranslationEnabled(gridTitle, true);
	ElementCompositionPreview::SetImplicitHideAnimation(gridTitle, animation);

	animation = compositor->CreateScalarKeyFrameAnimation();
	animation->Duration = span;
	animation->Target = "Opacity";
	animation->InsertKeyFrame(1, 1);
	animation->InsertKeyFrame(0.75, 0.75);
	animation->InsertKeyFrame(0.50, 0.50);
	animation->InsertKeyFrame(0.25, 0.25);
	animation->InsertKeyFrame(0, 0);
	ElementCompositionPreview::SetImplicitHideAnimation(OrgEditorPanel, animation);


}


void scwin::COrgEditorView::OnNavigatedTo(NavigationEventArgs ^ e)
{
	Page::OnNavigatedTo(e);
	ViewModel = dynamic_cast<CustomerViewModel^>(e->Parameter);
	
	//ConnectedAnimationService^ animService = nullptr;
	//animService = ConnectedAnimationService::GetForCurrentView();
	//ConnectedAnimation^ anim;
	//if (ViewModel->CurrentAction == DataState::Added)
	//{
	//	anim = animService->GetAnimation("AddCustomerLink");
	//}
	//else
	//{
	//	anim = animService->GetAnimation("BorderDest");
	//}
	//if (anim != nullptr)
	//{
	//	auto v = ref new Vector<UIElement^>({ OrgTitlePanel });
	//	anim->TryStart(OrgEditorPanel, v);
	//}
}

void scwin::COrgEditorView::OnNavigatingFrom(NavigatingCancelEventArgs ^ e)
{
	Page::OnNavigatingFrom(e);
	//ConnectedAnimationService::GetForCurrentView()->PrepareToAnimate("BorderDest", OrgEditorPanel);
	ViewModel->CurrentAction = DataState::Viewing;
}

void scwin::COrgEditorView::LogoImage_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	btnLogoEdit->Visibility = Windows::UI::Xaml::Visibility::Visible;
}


void scwin::COrgEditorView::LogoImage_PointerExited(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	btnLogoEdit->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
}


void scwin::COrgEditorView::btnAddAddress_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	btnAddAddress_Click(sender, e);
	e->Handled = true;
}

void scwin::COrgEditorView::btnAddAddress_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto btn = (Button^)sender;
	auto flyout = dynamic_cast<MenuFlyout^>(FlyoutBase::GetAttachedFlyout(btn));
	/*
	<MenuFlyoutItem Text="Mail"/>
	<MenuFlyoutItem Text="Shipping"/>
	<MenuFlyoutItem Text="Billing"/>
	*/
	auto dc = dynamic_cast<CostCenterDtosGroupedBindable^>(btn->DataContext);
	if (dc != nullptr)
	{
		std::vector<string> mnuTexts({ MAILING, BILLING, SHIPPING });
		flyout->Items->Clear();
		if (dc->Addresses != nullptr)
		{
			for (auto addr : dc->Addresses)
			{
				auto str = STRCONV(addr->Key->Data());
				auto iter = std::find_if(mnuTexts.cbegin(), mnuTexts.cend(), [&str](const string& s)
				{
					return s == str;
				});
				if (iter != mnuTexts.cend())
				{
					mnuTexts.erase(iter);
				}
			}
		}
		for (auto& str : mnuTexts)
		{
			auto m = ref new MenuFlyoutItem();
			m->Text = StringHelpers::ConvertToString(str.c_str());
			m->Tapped += ref new Windows::UI::Xaml::Input::TappedEventHandler(this, &scwin::COrgEditorView::MenuItemOnTapped);
			m->Click += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::COrgEditorView::MenuItemOnClick);
			flyout->Items->Append(m);
		}
	}
	flyout->ShowAt(btn);


}



void scwin::COrgEditorView::MenuItemOnTapped(Platform::Object ^sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs ^e)
{
	MenuItemOnClick(sender, e);
	e->Handled = true;
}

void scwin::COrgEditorView::MenuItemOnClick(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	using namespace cpplinq;
	auto menuItem = (MenuFlyoutItem^)sender;
	auto dc = dynamic_cast<CostCenterDtosGroupedBindable^>(menuItem->DataContext);

	if (dc == nullptr) return;

	auto hasAny = false;
	auto addedaddroptions =
		from(to_vector(dc->Addresses)) >>
		select([](AddressKeyValuePair^ pair) {return pair->Key; })
		>> distinct() >> to_vector();
	auto txt = menuItem->Text;
	using namespace cpplinq;
	hasAny = from(to_vector(dc->Addresses)) >> any([txt](AddressKeyValuePair^ pair)
	{
		return txt == pair->Key;
	});

	if (!hasAny)
	{
		auto newaddr = ref new AddressKeyValuePair(txt, ref new Address());
		newaddr->AddressSameAsOptions = ref new Vector<String^>(addedaddroptions);
		newaddr->AddressSameAsOptions->InsertAt(0, "None");
		dc->Addresses->Append(newaddr);
		newaddr->ShowAddressOptions = dc->Addresses->Size > 1;
		newaddr->AddressOptionSelected = "None";
	}



	// update btn visibility
	for (int i = 0; i < dc->Addresses->Size; i++)
	{
		bool yes = i == dc->Addresses->Size - 1;
		dc->Addresses->GetAt(i)->CanDisplayDeleteButton = yes;
	}
}


//void scwin::COrgEditorView::OnOrientationChanged(Windows::Graphics::Display::DisplayInformation ^sender, Platform::Object ^args)
//{
//	islandscape = (sender->CurrentOrientation == DisplayOrientations::Landscape ||
//		sender->CurrentOrientation == DisplayOrientations::LandscapeFlipped);
//	if (Dispatcher->HasThreadAccess)
//	{
//		PropertyChanged(this, ref new PropertyChangedEventArgs("IsLandscape"));
//	}
//	else
//	{
//		WeakReference wr(this);
//		Dispatcher->RunAsync(CoreDispatcherPriority::Normal, 
//			ref new DispatchedHandler([wr, sender]()
//		{
//			auto ths = wr.Resolve<COrgEditorView>();
//			ths->PropertyChanged(ths, ref new PropertyChangedEventArgs("IsLandscape"));
//		}));
//	}
//
//	if (ViewModel->Current != nullptr)
//	{
//		for (auto cc : ViewModel->Current->CostCenters)
//		{
//			cc->IsLandscape = islandscape;
//		}
//	}
//}
//
//void scwin::COrgEditorView::OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
//{
//	auto parent = safe_cast<FrameworkElement^>(lstCostCenters->Parent);
//	lstCostCenters->Width = parent->ActualWidth;
//}
//
//void scwin::COrgEditorView::OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
//{
//	//auto dispInfo = DisplayInformation::GetForCurrentView();
//	//if (dispInfo != nullptr)
//	//{
//	//	dispInfo->OrientationChanged -= dispOrientationTok;
//	//}
//
//}
//
//void scwin::COrgEditorView::OnSizeChanged(Platform::Object ^ sender, Windows::UI::Xaml::SizeChangedEventArgs ^ e)
//{
//	auto parent = safe_cast<FrameworkElement^>(lstCostCenters->Parent);
//	lstCostCenters->Width = parent->ActualWidth;
//}


void scwin::COrgEditorView::cmbAddressSameAsOptions_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{

}


void scwin::COrgEditorView::cmbAddressSameAsOptions_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
}


void scwin::COrgEditorView::cmbAddressSameAsOptions_DataContextChanged(Windows::UI::Xaml::FrameworkElement^ sender, Windows::UI::Xaml::DataContextChangedEventArgs^ args)
{
	//auto cmb = dynamic_cast<ComboBox^>(sender);
	//if (cmb != nullptr)
	//{
	//	if (cmb->ItemsSource != nullptr)
	//	{
	//		if (dynamic_cast<IVector<String^>^>(cmb->ItemsSource)->Size > 0)
	//		{
	//			cmb->SelectedIndex = 0;
	//		}
	//	}
	//}
}


void scwin::COrgEditorView::cmbAddressSameAsOptions_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	using namespace cpplinq;
	auto cmb = safe_cast<ComboBox^>(sender);
	auto spanel = safe_cast<StackPanel^>(cmb->Parent);
	if (spanel == nullptr)
		return;

	String^ selectedText = dynamic_cast<String^>(cmb->SelectedItem);

	auto cc = dynamic_cast<CostCenterDtosGroupedBindable^>(spanel->Tag);
	auto dc = dynamic_cast<AddressKeyValuePair^>(cmb->DataContext);

	if (dc->Key == selectedText)
	{
		return;
	}

	if (e->AddedItems != nullptr && e->AddedItems->Size)
	{
		auto key= (String^)e->AddedItems->GetAt(0);
		auto addrType = dc->Key;
		auto v = to_vector(cc->Addresses);
		auto itr = find_if(v.cbegin(), v.cend(), [addrType](AddressKeyValuePair^ pair)
		{
			return pair->Key == addrType;
		});
		size_t pos;
		if (itr != v.cend())
		{
			pos = itr - v.cbegin();
		}

		AddressKeyValuePair^ origAddrObj = cc->Addresses->GetAt(pos);
		AddressKeyValuePair^ newAddress = nullptr;

		if (key == "None")
		{
			// assign a new address object
			cc->Addresses->SetAt(pos, newAddress = ref new AddressKeyValuePair(addrType, ref new Address()));
		}
		else
		{
			itr = find_if(v.cbegin(), v.cend(), [key](AddressKeyValuePair^ pair)
			{
				return pair->Key == key;
			});
			auto addrObj = (*itr)->Value;
			newAddress = ref new AddressKeyValuePair(addrType, addrObj);
			cc->Addresses->SetAt(pos, newAddress);
		}
		
		newAddress->AddressSameAsOptions = origAddrObj->AddressSameAsOptions;
		newAddress->ShowAddressOptions = origAddrObj->ShowAddressOptions;
		// if costcenter is in edit mode, then btnremove can be visible always
		if (ViewModel->CurrentAction == DataState::Edited)
		{
			newAddress->CanDisplayDeleteButton = true;
		}
		newAddress->AddressOptionSelected = key;
	}
}


void scwin::COrgEditorView::btnDeleteAddress_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	using namespace cpplinq;
	using namespace Windows::UI::Popups;
	auto btn = (Button^)sender;
	auto cc = dynamic_cast<CostCenterDtosGroupedBindable^>(btn->Tag);
	auto dc = dynamic_cast<AddressKeyValuePair^>(btn->DataContext);
	if (cc != nullptr && dc != nullptr)
	{
		auto strKey = dc->Key;
		auto yesAction = ref new UICommandInvokedHandler([strKey, cc](IUICommand^ cmd)
		{
			auto v = to_vector(cc->Addresses);
			auto itr = find_if(v.cbegin(), v.cend(), [strKey](AddressKeyValuePair^ pair)
			{
				return pair->Key == strKey;
			});
			size_t pos;
			if (itr != v.cend())
			{
				pos = itr - v.cbegin();
			}
			cc->Addresses->RemoveAt(pos);
			// update btn visibility
			for (int i=0; i<cc->Addresses->Size; i++)
			{
				bool yes = i == cc->Addresses->Size - 1;
				cc->Addresses->GetAt(i)->CanDisplayDeleteButton = yes;
			}
		});

		MessageDialog^ dlg = ref new MessageDialog("Are you sure you want to remove " + strKey + " address ?", "Remove Address");
		dlg->Commands->Append(ref new UICommand("Yes", yesAction));
		dlg->Commands->Append(ref new UICommand("No"));
		dlg->DefaultCommandIndex = 1;
		dlg->CancelCommandIndex = 1;
		dlg->ShowAsync();

	}

}



void scwin::COrgEditorView::btnAddContact_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	auto btn = safe_cast<Button^>(sender);
	auto cc = dynamic_cast<CostCenterDtosGroupedBindable^>(btn->DataContext);
	if (cc != nullptr)
	{
		auto sz = cc->Contacts->Size;
		cc->Contacts->Append(ref new CustomerContactKeyValuePair(sz + 1, ref new CustomerContact()));
	}
}


void scwin::COrgEditorView::btnCancelOrg_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	if (Frame != nullptr && Frame->CanGoBack)
	{
		Frame->GoBack();
	}
}


void scwin::COrgEditorView::btnSaveOrg_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	if (ViewModel->Current->CanExecuteSaveCommand(sender))
	{
		WeakReference wr(this);
		ViewModel->Current->ExecuteSaveCommand(sender).then([wr](MessageIds status)
		{
			auto ths = wr.Resolve<COrgEditorView>();
			if (SUCCESS(status))
			{
				if (ths->ViewModel->Organizations == nullptr)
					ths->ViewModel->Organizations = ref new Vector<CustomerDtosGroupedBindable^>();

				ths->ViewModel->Organizations->Append(ths->ViewModel->Current);

				ths->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([wr]()
				{
					auto ths = wr.Resolve<COrgEditorView>();
					if (ths->Frame != nullptr && ths->Frame->CanGoBack)
					{
						ths->Frame->GoBack();
					}
				}));
			}
			else
			{
				String^ message;
				switch (status)
				{
				case MessageIds::DBTransactionBeginFailed:
					message = "Cache Db failed to initiate a transaction. Please retry.";
					break;
				case MessageIds::UniqueConstraintFailed:
					message = "Duplicated code provided. Business or cost center codes must be unique.";
					break;
				default:
					message = "Failed to save the default cost center. Please ensure that all required information is provided and retry.";
					break;
				}
				auto ps = PubSubService::Instance;
				ps->Publish(
					ths,
					PubSubMessageIds::ApplicationErrorStatusMessage,
					message);
			}
		});
	}
}


void scwin::COrgEditorView::btnRemoveContact_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	using namespace Windows::UI::Popups;
	Button^ btn = safe_cast<Button^>(sender);
	auto dc = dynamic_cast<CustomerContactKeyValuePair^>(btn->DataContext);
	auto cc = dynamic_cast<CostCenterDtosGroupedBindable^>(btn->Tag);
	if (cc != nullptr)
	{
		auto yesAction = ref new UICommandInvokedHandler([dc, cc](IUICommand^ cmd)
			{
				auto v = to_vector(cc->Contacts);
				auto iter = find_if(v.cbegin(), v.cend(), [dc](CustomerContactKeyValuePair^ contact)
				{
					return contact == dc;
				});
				if (iter != v.cend())
				{
					auto pos = iter - v.cbegin();
					cc->Contacts->RemoveAt(pos);
				}

				// update keys
				UINT pos = 1;
				for (auto c : cc->Contacts)
				{
					c->SetKey(pos++);
				}

			});
		auto dlg = ref new MessageDialog("Are you sure you want to remove this contact information ?", "Remove Contact");
		dlg->Commands->Append(ref new UICommand("Yes", yesAction));
		dlg->Commands->Append(ref new UICommand("No"));
		dlg->DefaultCommandIndex = 1;
		dlg->CancelCommandIndex = 1;
		dlg->ShowAsync();
	}
}


void scwin::COrgEditorView::btnDeleteCostCenter_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	using namespace Windows::UI::Popups;
	auto btn = safe_cast<Button^>(sender);
	auto cc = dynamic_cast<CostCenterDtosGroupedBindable^>(btn->DataContext);
	WeakReference wr(this);
	if (cc != nullptr && !cc->CostCenter->IsDefault)
	{
		auto v = to_vector(ViewModel->Current->CostCenters);
		auto iter = find_if(v.cbegin(), v.cend(), [cc](CostCenterDtosGroupedBindable^ item)
		{
			return cc == item;
		});
		if (iter != v.cend())
		{
			auto pos = iter - v.cbegin();
			auto yesAction = ref new UICommandInvokedHandler([wr, pos, cc](IUICommand^ cmd)
			{
				auto ths = wr.Resolve<COrgEditorView>();
				ths->ViewModel->Current->CostCenters->RemoveAt(pos);
				// update keys
				UINT pos = 1;
				for (auto c : cc->Contacts)
				{
					c->SetKey(pos++);
				}

				PubSubService::Instance->
					Publish(ths, PubSubMessageIds::ApplicationInfoStatusMessage,
						"Costcenter is not deleted from the database for alpha release.");

			});
			auto dlg = ref new MessageDialog("Are you sure you want to remove this Costcenter information ?", "Remove Costcenter");
			dlg->Commands->Append(ref new UICommand("Yes", yesAction));
			dlg->Commands->Append(ref new UICommand("No"));
			dlg->DefaultCommandIndex = 1;
			dlg->CancelCommandIndex = 1;
			dlg->ShowAsync();
		}
	}
}


void scwin::COrgEditorView::btnAddCostcenter_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	ViewModel->Current->AddCostCenter();
}




