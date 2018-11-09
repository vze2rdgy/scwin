#pragma once


using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

namespace scwin
{
	public ref class BreadcrumbItem sealed : public Button
	{
	};

	public ref class BreadcrumbSeparator sealed : public ContentControl
	{
	};

	public ref class BreadcrumbHome sealed : public Button
	{
	};
}