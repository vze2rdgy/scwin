//
// Triangle.xaml.cpp
// Implementation of the Triangle class
//

#include "pch.h"
#include "Triangle.xaml.h"
#include "TriangleRenderer.h"

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

Triangle::Triangle() : openGles(OpenGLES::GetInstance())
{
	InitializeComponent();

	auto window = Window::Current->CoreWindow;
	window->VisibilityChanged += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^, Windows::UI::Core::VisibilityChangedEventArgs ^>(this, &sggraphs::Triangle::OnVisibilityChanged);
	Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &sggraphs::Triangle::OnLoaded);
	Unloaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &sggraphs::Triangle::OnUnloaded);
}


void sggraphs::Triangle::OnVisibilityChanged(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::VisibilityChangedEventArgs ^args)
{
}


void sggraphs::Triangle::OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	if (renderSurface == EGL_NO_SURFACE)
	{
		renderSurface = openGles.CreateSurface(swapChainPanel, nullptr, nullptr);
	}
}


void sggraphs::Triangle::OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	openGles.DestroySurface(renderSurface);
	renderSurface = EGL_NO_SURFACE;
}

void sggraphs::Triangle::RecoverFromLostDevice()
{
	concurrency::critical_section::scoped_lock lock(csForRenderSurface);
	openGles.DestroySurface(renderSurface);
	renderSurface = EGL_NO_SURFACE;
	openGles.Reset();
	openGles.CreateSurface(swapChainPanel, nullptr, nullptr);
}

void sggraphs::Triangle::MakeCurrent(bool yes)
{

	// ignore if render surface is not created
	if (renderSurface == EGL_NO_SURFACE) return;

	if (yes)
	{
		if (renderSurface != EGL_NO_SURFACE)
		{
			openGles.MakeCurrent(renderSurface);
			TriangleRenderer renderer;
			EGLint panelWidth = 0;
			EGLint panelHeight = 0;
			openGles.GetSurfaceDimensions(renderSurface, &panelWidth, &panelHeight);
			renderer.UpdateWindowSize(panelWidth, panelHeight);
			renderer.Draw();
			if (openGles.SwapBuffers(renderSurface) != GL_TRUE)
			{
				if (swapChainPanel->Dispatcher->HasThreadAccess)
				{
					RecoverFromLostDevice();
				}
				else
				{
					swapChainPanel->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([=]()
					{
						RecoverFromLostDevice();
					}, CallbackContext::Any));
				}
			}
		}
	}
}
