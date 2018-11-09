#include "pch.h"
#include "OpenGLES.h"
#include <concrt.h>

using namespace Platform;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace sggraphs;


/* IMPORTANT Issues discussed here with regards to having side-by-side swapchainpanels on the same window.
https://github.com/Microsoft/angle/issues/65
*/

concurrency::critical_section OpenGLES::cs;
OpenGLES* OpenGLES::gls = nullptr;

OpenGLES::OpenGLES() :
	eglConfig(nullptr),
	eglDisplay(EGL_NO_DISPLAY),
	eglContext(EGL_NO_CONTEXT)
{
	Initialize();
}


OpenGLES::~OpenGLES()
{
	CleanUp();
}

void sggraphs::OpenGLES::Initialize()
{
	const EGLint configAttributes[] = 
	{
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_DEPTH_SIZE, 8,
		EGL_STENCIL_SIZE, 8,
		EGL_NONE
	};

	const EGLint contextAttributes [] =
	{
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	const EGLint defaultDisplayAttributes[] =
	{
		// default display attributes asking for internal directx 11 renderer
		// initializes only when D3D11 feature level 10_0+ is available.
		EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,

		// enable a back buffer optimization for mobile devices
		EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER, EGL_TRUE,

		// enable angle to invoke IDXGIDevice3::Trim method when app gets suspended
		// IDXGIDevice3::Trim is required for appstore certification.
		EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE, EGL_TRUE,

		EGL_NONE
	};

	const EGLint fl9_3DisplayAttributes[] =
	{
		// used to request angle's D3D11 renderer with D3D11 feature level 9_3.
		// these attributes are used if the call to eglInitialize fails with default 
		// display attributes.
		EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,
		EGL_PLATFORM_ANGLE_MAX_VERSION_MAJOR_ANGLE, 9,
		EGL_PLATFORM_ANGLE_MAX_VERSION_MINOR_ANGLE, 3,
		EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER, EGL_TRUE,
		EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE, EGL_TRUE,
		EGL_NONE
	};

	const EGLint warpDisplayAttributes[] =
	{
		//attributes can be used to request D3D11 warp
		// used if egInitialize fails with both default and 9-3 display attribs (above)
		EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,
		EGL_PLATFORM_ANGLE_DEVICE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_DEVICE_TYPE_WARP_ANGLE,
		EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER, EGL_TRUE,
		EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE, EGL_TRUE,
		EGL_NONE,
	};

	EGLConfig config = NULL;
	PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT =
		reinterpret_cast<PFNEGLGETPLATFORMDISPLAYEXTPROC>(eglGetProcAddress("eglGetPlatformDisplayEXT"));
	if (!eglGetPlatformDisplayEXT)
		throw Exception::CreateException(E_FAIL, "Failed to get function 'eglGetPlatformDisplayEXT'");

	// for initializing display, make three calls to eglGetPlatformDisplayEXT and eglInitialize methods 
	// with different paramtesr until proper display is found
	eglDisplay = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE,
		EGL_DEFAULT_DISPLAY, defaultDisplayAttributes);
	if (eglDisplay == EGL_NO_DISPLAY)
	{
		throw Exception::CreateException(E_FAIL, "Failed to get EGL Display");
	}
	if (EGL_FALSE == eglInitialize(eglDisplay, NULL, NULL))
	{
		eglDisplay = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY, fl9_3DisplayAttributes);
		if (EGL_NO_DISPLAY == eglDisplay)
		{
			throw Exception::CreateException(E_FAIL, "Failed to get EGL display");
		}
		if (EGL_FALSE == eglInitialize(eglDisplay, NULL, NULL))
		{
			eglDisplay = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY, warpDisplayAttributes);
			if (EGL_NO_DISPLAY == eglDisplay)
			{
				throw Exception::CreateException(E_FAIL, "Failed to get EGL Display");
			}
			if (EGL_FALSE == eglInitialize(eglDisplay, NULL, NULL))
			{
				throw Exception::CreateException(E_FAIL, "Failed to initialize EGL");
			}
		}
	}

	EGLint numConfigs = 0;
	if ((eglChooseConfig(eglDisplay, configAttributes, &eglConfig, 1, &numConfigs) == EGL_FALSE) ||
		(numConfigs == 0))
	{
		throw Exception::CreateException(E_FAIL, "Failed to choose first EGLConfig");
	}
	if (EGL_NO_CONTEXT == (eglContext = eglCreateContext(eglDisplay, eglConfig, EGL_NO_CONTEXT, contextAttributes)))
	{
		throw Exception::CreateException(E_FAIL, "Failed to create EGL context");
	}
}

void sggraphs::OpenGLES::CleanUp()
{
	if (eglDisplay != EGL_NO_DISPLAY && eglContext != EGL_NO_CONTEXT)
	{
		eglDestroyContext(eglDisplay, eglContext);
		eglContext = EGL_NO_CONTEXT;
	}
	if (eglDisplay != EGL_NO_DISPLAY)
	{
		eglTerminate(eglDisplay);
		eglDisplay = EGL_NO_DISPLAY;
	}
}

void sggraphs::OpenGLES::Reset()
{
	CleanUp();
	Initialize();
}

OpenGLES & sggraphs::OpenGLES::GetInstance()
{
	if (gls == nullptr)
	{
		concurrency::critical_section::scoped_lock lock(cs);
		if (gls == nullptr)
		{
			gls = new OpenGLES();
		}
	}
	return *gls;
}

void sggraphs::OpenGLES::Detach()
{
	if (gls != nullptr)
	{
		auto pgls = (OpenGLES*)InterlockedExchangePointer((PVOID volatile*)gls, nullptr);
		if (pgls != nullptr)
		{
			pgls->CleanUp();
			pgls = nullptr;
		}
	}
}


EGLSurface sggraphs::OpenGLES::CreateSurface(SwapChainPanel ^ panel, const Size * renderSurfaceSize, const float * renderResolutionScale)
{
	if (!panel)
	{
		throw Exception::CreateException(E_INVALIDARG, "panel parameter is invalid");
	}
	if (renderSurfaceSize != nullptr && renderResolutionScale != nullptr)
		throw Exception::CreateException(E_INVALIDARG, "Surface size and resolution scale both arguments cannot be provided");

	EGLSurface surface = EGL_NO_SURFACE;
	const EGLint surfaceAttributes[] =
	{
		// EGL_ANGLE_SURFACE_RENDER_TO_BACK_BUFFER is part of the same optimization as EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER (see above).
		EGL_ANGLE_SURFACE_RENDER_TO_BACK_BUFFER, EGL_TRUE,
		EGL_NONE
	};

	// Create a propertyset and initialize with EGLNativeWindowType
	PropertySet^ surfaceCreationProperties = ref new PropertySet();
	surfaceCreationProperties->Insert(ref new String(EGLNativeWindowTypeProperty), panel);

	// if render size is specified add it to the surface creation properties
	if (renderSurfaceSize != nullptr)
		surfaceCreationProperties->Insert(ref new String(EGLRenderSurfaceSizeProperty), PropertyValue::CreateSize(*renderSurfaceSize));

	surface = eglCreateWindowSurface(eglDisplay, eglConfig, reinterpret_cast<IInspectable*>(surfaceCreationProperties), surfaceAttributes);

	if (surface == EGL_NO_SURFACE)
		throw Exception::CreateException(E_FAIL, "Failed to create EGL Surface");

	return surface;
}

void sggraphs::OpenGLES::GetSurfaceDimensions(const EGLSurface surface, EGLint * width, EGLint * height)
{
	eglQuerySurface(eglDisplay, surface, EGL_WIDTH, width);
	eglQuerySurface(eglDisplay, surface, EGL_HEIGHT, height);
}

void sggraphs::OpenGLES::DestroySurface(const EGLSurface surface)
{
	if (eglDisplay != EGL_NO_DISPLAY && surface != EGL_NO_SURFACE)
		eglDestroySurface(eglDisplay, surface);
}

void sggraphs::OpenGLES::MakeCurrent(const EGLSurface surface)
{
	if (eglMakeCurrent(eglDisplay, surface, surface, eglContext) == EGL_FALSE)
		throw Exception::CreateException(E_FAIL, "Failed to make EGLSurface current");
}

EGLBoolean sggraphs::OpenGLES::SwapBuffers(const EGLSurface surface)
{
	return eglSwapBuffers(eglDisplay, surface);
}

