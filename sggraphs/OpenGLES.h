#pragma once

using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

namespace sggraphs
{

	class OpenGLES
	{
	private:
		static OpenGLES* gls;
		static concurrency::critical_section cs;

		EGLDisplay eglDisplay;
		EGLContext eglContext;
		EGLConfig eglConfig;

	private:
		OpenGLES();

	public:
		virtual ~OpenGLES();

	private:
		void Initialize();
		void CleanUp();

	public:
		EGLSurface CreateSurface(SwapChainPanel^ panel,
			const Size* renderSurfaceSize,
			const float* renderResolutionScale);
		void GetSurfaceDimensions(const EGLSurface surface, EGLint* width, EGLint* height);
		void DestroySurface(const EGLSurface surface);
		void MakeCurrent(const EGLSurface surface);
		EGLBoolean SwapBuffers(const EGLSurface surface);
		void Reset();

		static OpenGLES& GetInstance();
		static void Detach();
	};

}