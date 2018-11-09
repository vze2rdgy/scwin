#pragma once

using namespace Windows::UI::Composition;
using namespace Windows::UI::Xaml::Hosting;
using namespace Windows::UI::Xaml::Media::Animation;

namespace scwin
{

	public ref class AnimationHelper sealed
	{
		Compositor^ compositor;

	public:
		AnimationHelper(UIElement^ targetElement);

		property Compositor^ CompositorInstance
		{
			Compositor^ get() { return compositor; }
		}
	};

}