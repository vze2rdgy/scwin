#include "pch.h"
#include "AnimationHelper.h"

scwin::AnimationHelper::AnimationHelper(UIElement ^ targetElement)
{
	TimeSpan span;
	span.Duration = 3000000; // .3 seconds
	compositor = ElementCompositionPreview::GetElementVisual(targetElement)->Compositor;
	// fade in animation
	auto fadeInAnimation = compositor->CreateScalarKeyFrameAnimation();
	fadeInAnimation->Target = "Opacity";
	fadeInAnimation->Duration = span;
	fadeInAnimation->InsertKeyFrame(0.0, 0.0);
	fadeInAnimation->InsertKeyFrame(1.0, 1.0);

	// call GetElementVisual to work around a bug in 15025
	ElementCompositionPreview::GetElementVisual(targetElement);
	ElementCompositionPreview::SetImplicitShowAnimation(targetElement, fadeInAnimation);

	// set a fade out animation when this page exits the scene
	auto fadeOutAnimation = compositor->CreateScalarKeyFrameAnimation();
	fadeOutAnimation->Target = "Opacity";
	fadeOutAnimation->Duration = span;
	fadeOutAnimation->InsertKeyFrame(0.0, 1.0);
	fadeOutAnimation->InsertKeyFrame(1.0, 0.0);

	ElementCompositionPreview::SetImplicitHideAnimation(targetElement, fadeOutAnimation);

}
