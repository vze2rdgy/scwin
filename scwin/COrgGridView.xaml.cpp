//
// COrgGridView.xaml.cpp
// Implementation of the COrgGridView class
//

#include "pch.h"
#include "COrgGridView.xaml.h"
#include "CustomerAdminView.xaml.h"

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

COrgGridView::COrgGridView()
{
	InitializeComponent();

	TimeSpan span;
	span.Duration = 3000000; // .3 seconds
	compositor = ElementCompositionPreview::GetElementVisual(this)->Compositor;
	// fade in animation
	auto fadeInAnimation = compositor->CreateScalarKeyFrameAnimation();
	fadeInAnimation->Target = "Opacity";
	fadeInAnimation->Duration = span;
	fadeInAnimation->InsertKeyFrame(0.0, 0.0);
	fadeInAnimation->InsertKeyFrame(1.0, 1.0);

	// call GetElementVisual to work around a bug in 15025
	//ElementCompositionPreview::GetElementVisual(this);
	ElementCompositionPreview::SetImplicitShowAnimation(this, fadeInAnimation);

	// set a fade out animation when this page exits the scene
	auto fadeOutAnimation = compositor->CreateScalarKeyFrameAnimation();
	fadeOutAnimation->Target = "Opacity";
	fadeOutAnimation->Duration = span;
	fadeOutAnimation->InsertKeyFrame(0.0, 1.0);
	fadeOutAnimation->InsertKeyFrame(1.0, 0.0);

	ElementCompositionPreview::SetImplicitHideAnimation(this, fadeOutAnimation);

}


void scwin::COrgGridView::OnNavigatedTo(NavigationEventArgs ^ e)
{
	Page::OnNavigatedTo(e);
	this->ViewModel = ref new CustomerViewModel();
	if (e->NavigationMode == NavigationMode::Back)
	{
		gvOrgs->Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::COrgGridView::OnGridViewLoaded);
	}
	ViewModel->CurrentAction = DataState::Viewing;
}

void scwin::COrgGridView::OnNavigatingFrom(NavigatingCancelEventArgs ^ e)
{
	Page::OnNavigatingFrom(e);
	ViewModel->SaveState();
	ViewModel = nullptr;
}


void scwin::COrgGridView::gvOrgs_ItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	//TimeSpan span;
	//span.Duration = 5000000;
	//ConnectedAnimationService::GetForCurrentView()->DefaultDuration = span;
	//gvOrgs->PrepareConnectedAnimation("BorderSource", e->ClickedItem, "BorderSource");

	ViewModel->Current = dynamic_cast<CustomerDtosGroupedBindable^>(e->ClickedItem);
	ViewModel->CurrentAction = DataState::Viewing;
	Frame->Navigate(TypeName(CustomerAdminView::typeid), ViewModel);
}

void scwin::COrgGridView::OnGridViewLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	//auto connectedAnim = ConnectedAnimationService::GetForCurrentView()
	//	->GetAnimation("BorderDest");
	//if (connectedAnim != nullptr)
	//{
	//	if (gvOrgs->Items->Size)
	//	{
	//		if (ViewModel->Current == nullptr)
	//		{
	//			ViewModel->Current = dynamic_cast<CustomerDtosGroupedBindable^>(gvOrgs->Items->GetAt(0));
	//		}
	//		UINT index;
	//		if (gvOrgs->Items->IndexOf(ViewModel->Current, &index))
	//		{
	//			gvOrgs->ScrollIntoView(ViewModel->Current);
	//			gvOrgs->TryStartConnectedAnimationAsync(connectedAnim, ViewModel->Current, "BorderSource");
	//		}
	//	}
	//	else
	//	{
	//		connectedAnim->TryStart(lnkAddCustomer);
	//	}
	//}
}


void scwin::COrgGridView::gvOrgs_SearchKeyChanged(Windows::UI::Xaml::Controls::ListView^ sender, Platform::String^ args)
{
	// hide and show based on search criteria
	for (auto item : to_vector(ViewModel->Organizations))
	{
		auto lvi = (ListViewItem^)sender->ContainerFromItem(item);
		if (!StringHelpers::Match(item->Organization->Name, args)) // TODO: Match should be extended to cost center name and other fields as well.
		{
			lvi->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		}
		else
		{
			lvi->Visibility = Windows::UI::Xaml::Visibility::Visible;
		}
	}
}



void scwin::COrgGridView::gvOrgs_AddButtonClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	ViewModel->Current = ref new CustomerDtosGroupedBindable();
	ViewModel->CurrentAction = DataState::Viewing;
	Frame->Navigate(TypeName(CustomerAdminView::typeid), ViewModel);
}
