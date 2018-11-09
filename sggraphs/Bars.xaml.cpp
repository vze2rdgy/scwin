//
// Bars.xaml.cpp
// Implementation of the Bars class
//

#include "pch.h"
#include "Bars.xaml.h"
#include "SimpleRenderer.h"

using namespace sggraphs;

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


Bars::Bars() : openGles(OpenGLES::GetInstance()), renderSurface(EGL_NO_SURFACE)
{
	InitializeComponent();

	auto window = Window::Current->CoreWindow;
	window->VisibilityChanged += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow^, Windows::UI::Core::VisibilityChangedEventArgs^>(this, &Bars::OnVisibilityChanged);
	Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &Bars::OnPageLoaded);
	Unloaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &Bars::OnPageUnloaded);

}

void sggraphs::Bars::OnPageLoaded(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
	CreateRenderSurface();
	StartRenderLoop();
}

void sggraphs::Bars::OnVisibilityChanged(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::VisibilityChangedEventArgs ^ args)
{
	if (args->Visible && renderSurface != EGL_NO_SURFACE)
	{
		StartRenderLoop();
	}
	else
	{
		StopRenderLoop();
	}
}

void sggraphs::Bars::CreateRenderSurface()
{
	if (renderSurface == EGL_NO_SURFACE)
	{
		// The app can configure the the SwapChainPanel which may boost performance.
		// By default, this template uses the default configuration.
		renderSurface = openGles.CreateSurface(swapChainPanel, nullptr, nullptr);

		// You can configure the SwapChainPanel to render at a lower resolution and be scaled up to
		// the swapchain panel size. This scaling is often free on mobile hardware.
		//

		// One way to configure the SwapChainPanel is to specify precisely which resolution it should render at.
		// Size customRenderSurfaceSize = Size(800, 600);
		// renderSurface = openGles.CreateSurface(swapChainPanel, &customRenderSurfaceSize, nullptr);
		//

		// Another way is to tell the SwapChainPanel to render at a certain scale factor compared to its size.
		// e.g. if the SwapChainPanel is 1920x1280 then setting a factor of 0.5f will make the app render at 960x640
		// float customResolutionScale = 0.5f;
		// renderSurface = openGles.CreateSurface(swapChainPanel, nullptr, &customResolutionScale);
		// 

	}
}

void sggraphs::Bars::DestroyRenderSurface()
{
	openGles.DestroySurface(renderSurface);
	renderSurface = EGL_NO_SURFACE;
}

void sggraphs::Bars::RecoverFromLostDevice()
{
	// Stop the render loop, reset OpenGLES, recreate the render surface
	// and start the render loop again to recover from a lost device.
	StopRenderLoop();
	{
		concurrency::critical_section::scoped_lock lock(csForRenderSurface);
		DestroyRenderSurface();
		openGles.Reset();
		CreateRenderSurface();
	}
	StartRenderLoop();
}

void sggraphs::Bars::StartRenderLoop()
{
	if (renderLoopWorker != nullptr && renderLoopWorker->Status == AsyncStatus::Started)
		return;
	auto workItemHandler = ref new Windows::System::Threading::WorkItemHandler([this](IAsyncAction^ action)
	{
		concurrency::critical_section::scoped_lock lock(csForRenderSurface);
		openGles.MakeCurrent(renderSurface);
		SimpleRenderer renderer;
		while (action->Status == AsyncStatus::Started)
		{
			EGLint panelWidth = 0;
			EGLint panelHeight = 0;
			openGles.GetSurfaceDimensions(renderSurface, &panelWidth, &panelHeight);
			renderer.UpdateWindowSize(panelWidth, panelHeight);
			renderer.Draw();

			if (openGles.SwapBuffers(renderSurface) != GL_TRUE)
			{
				swapChainPanel->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([=]()
				{
					RecoverFromLostDevice();
				}, CallbackContext::Any));
				
				return;
			}
		}
	});

	// Run task on a dedicated high priority background thread.
	renderLoopWorker = Windows::System::Threading::ThreadPool::RunAsync(workItemHandler, Windows::System::Threading::WorkItemPriority::High, Windows::System::Threading::WorkItemOptions::TimeSliced);


}

void sggraphs::Bars::StopRenderLoop()
{
	if (renderLoopWorker != nullptr)
	{
		renderLoopWorker->Cancel();
		renderLoopWorker = nullptr;
	}
}



void sggraphs::Bars::OnPageUnloaded(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
	StopRenderLoop();
	DestroyRenderSurface();
}

sggraphs::Bars::~Bars()
{

}

void sggraphs::Bars::MakeCurrent(bool yes)
{
	// ignore if render surface is not created
	if (renderSurface == EGL_NO_SURFACE) return;

	if (yes)
	{
		StartRenderLoop();
	}
	else
	{
		StopRenderLoop();
	}
}
