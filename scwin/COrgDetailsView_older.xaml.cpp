//
// COrgDetailsView.xaml.cpp
// Implementation of the COrgDetailsView class
//

#include "pch.h"
#include "COrgDetailsView_older.xaml.h"
#include "COrgEditorView_older.xaml.h"

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


COrgDetailsView::COrgDetailsView()
{
	InitializeComponent();

	compositor = ElementCompositionPreview::GetElementVisual(this)->Compositor;
	// translation animation
	TimeSpan span;
	span.Duration = 4500000;
	auto topBorderOffsetAnimation = compositor->CreateScalarKeyFrameAnimation();
	topBorderOffsetAnimation->Duration = span;
	topBorderOffsetAnimation->Target = "Translation.Y";
	topBorderOffsetAnimation->InsertKeyFrame(0.0, -450.0);
	topBorderOffsetAnimation->InsertKeyFrame(1.0, 0.0);

	ElementCompositionPreview::SetIsTranslationEnabled(TopBorder, true);
	//ElementCompositionPreview::GetElementVisual(TopBorder);
	ElementCompositionPreview::SetImplicitShowAnimation(TopBorder, topBorderOffsetAnimation);

	// add opacity and translation animation that will paly when the element is shown
	auto mainContentTranslationAnimation = compositor->CreateScalarKeyFrameAnimation();
	mainContentTranslationAnimation->DelayBehavior = AnimationDelayBehavior::SetInitialValueBeforeDelay;
	mainContentTranslationAnimation->Duration = span;
	span.Duration = 2000000;
	mainContentTranslationAnimation->DelayTime = span;
	mainContentTranslationAnimation->Target = "Translation.Y";
	mainContentTranslationAnimation->InsertKeyFrame(0.0, 50.0);
	mainContentTranslationAnimation->InsertKeyFrame(1.0, 0.0);

	auto mainContentOpacityAnimation = compositor->CreateScalarKeyFrameAnimation();
	span.Duration = 4000000;
	mainContentOpacityAnimation->Duration = span;
	mainContentOpacityAnimation->Target = "Opacity";
	mainContentOpacityAnimation->InsertKeyFrame(0, 0);
	mainContentOpacityAnimation->InsertKeyFrame(0.25, 0);
	mainContentOpacityAnimation->InsertKeyFrame(1, 1);

	auto mainContentShowAnimations = compositor->CreateAnimationGroup();
	mainContentShowAnimations->Add(mainContentTranslationAnimation);
	mainContentShowAnimations->Add(mainContentOpacityAnimation);

	ElementCompositionPreview::SetIsTranslationEnabled(MainContent, true);
	//ElementCompositionPreview::GetElementChildVisual(MainContent);
	ElementCompositionPreview::SetImplicitShowAnimation(MainContent, mainContentShowAnimations);

	// translation and opacity animations when element exits scene
	auto topBorderExitAnimation = compositor->CreateScalarKeyFrameAnimation();
	topBorderExitAnimation->Target = "Translation.Y";
	topBorderExitAnimation->InsertKeyFrame(1, -30);
	topBorderExitAnimation->Duration = span;
	ElementCompositionPreview::SetIsTranslationEnabled(TopBorder, true);
	//ElementCompositionPreview::GetElementVisual(TopBorder);
	ElementCompositionPreview::SetImplicitHideAnimation(TopBorder, topBorderExitAnimation);

	auto fadeOut = compositor->CreateScalarKeyFrameAnimation();
	fadeOut->Target = "Opacity";
	fadeOut->InsertKeyFrame(1, 0);
	fadeOut->Duration = span;

	// set z-index to force this page to the top during the hide animation.
	Canvas::SetZIndex(this, 1);
	//ElementCompositionPreview::GetElementVisual(this);
	ElementCompositionPreview::SetImplicitHideAnimation(this, fadeOut);
}

void scwin::COrgDetailsView::OnNavigatedTo(NavigationEventArgs ^ e)
{
	Page::OnNavigatedTo(e);
	this->ViewModel = dynamic_cast<CustomerViewModel^>(e->Parameter);
	ConnectedAnimation^ anim;
	if (ViewModel->Current->Organization->ObjectState == DataState::Added)
	{
		anim = ConnectedAnimationService::GetForCurrentView()
			->GetAnimation("AddCustomerLink");
	}
	else // edited.
	{
		anim = ConnectedAnimationService::GetForCurrentView()
			->GetAnimation("BorderSource");
	}

	if (anim != nullptr)
	{
		IVector<UIElement^>^ v = ref new Vector<UIElement^>();
		v->Append(DescriptionRoot);
		anim->TryStart(BorderDest, v);
	}
	ViewModel->CurrentAction = DataState::Viewing;

	costCenters.emplace_back(ViewModel->Current->DefaultCostCenter);
	auto v = to_vector(ViewModel->Current->CostCenters);
	if (v.begin() != v.end())
		std::copy(v.begin(), v.end(), back_inserter(costCenters));
	if (ViewModel->Current->CurrentCostCenter == nullptr)
	{
		ViewModel->Current->CurrentCostCenter = *costCenters.begin();
	}
}

void scwin::COrgDetailsView::OnNavigatingFrom(NavigatingCancelEventArgs ^ e)
{
	Page::OnNavigatingFrom(e);
	ConnectedAnimationService::GetForCurrentView()
		->PrepareToAnimate("BorderDest", BorderDest);
}


void scwin::COrgDetailsView::btnEditOrg_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	WeakReference wr(this);
	create_task([wr]()
	{
		auto ths = wr.Resolve<COrgDetailsView>();
		auto f = [wr]()
		{
			auto ths = wr.Resolve<COrgDetailsView>();
			TimeSpan span;
			span.Duration = 5000000;
			ConnectedAnimationService::GetForCurrentView()->DefaultDuration = span;
			auto connectedAnimation = ConnectedAnimationService::GetForCurrentView()
				->PrepareToAnimate("AddCustomerLink", ths->btnEditOrg);
			ths->ViewModel->CurrentAction = DataState::Edited;
			ths->Frame->Navigate(COrgEditorView::typeid, ths->ViewModel);
		};
		if (ths->Dispatcher->HasThreadAccess)
		{
			f();
		}
		else
		{
			ths->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler(f));
		}
	});

}


void scwin::COrgDetailsView::lnkOtherLocations_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto link = safe_cast<HyperlinkButton^>(sender);
	auto flyout = dynamic_cast<MenuFlyout^>(FlyoutBase::GetAttachedFlyout(link));
	flyout->ShowAt(link);
}


void scwin::COrgDetailsView::btnPreviousLocation_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	WeakReference wr(this);
	auto pos = find_if(costCenters.cbegin(), costCenters.cend(), [wr](CostCenterDtosGroupedBindable^ item)
	{
		auto ths = wr.Resolve<COrgDetailsView>();
		return item == ths->ViewModel->Current->CurrentCostCenter;
	});
	std::vector<CostCenterDtosGroupedBindable^>::const_iterator next;
	if (pos != costCenters.cbegin())
	{
		next = --pos;
		if (next == costCenters.cend())
			next = costCenters.cend() - 1;
	}
	else
	{
		next = costCenters.end() - 1;
	}
	ViewModel->Current->CurrentCostCenter = *next;
}


void scwin::COrgDetailsView::btnPreviousLocation_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	btnPreviousLocation_Click(sender, e);
	e->Handled = true;
}


void scwin::COrgDetailsView::btnNextLocation_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	WeakReference wr(this);
	auto pos = find_if(costCenters.cbegin(), costCenters.cend(), [wr](CostCenterDtosGroupedBindable^ item)
	{
		auto ths = wr.Resolve<COrgDetailsView>();
		return item == ths->ViewModel->Current->CurrentCostCenter;
	});
	std::vector<CostCenterDtosGroupedBindable^>::const_iterator next;
	if (pos != costCenters.cend())
	{
		next = ++pos;
		if (next == costCenters.cend())
			next = costCenters.begin();
	}
	else
	{
		next = costCenters.begin();
	}
	ViewModel->Current->CurrentCostCenter = *next;
}


void scwin::COrgDetailsView::btnNextLocation_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	btnNextLocation_Click(sender, e);
	e->Handled = true;
}
