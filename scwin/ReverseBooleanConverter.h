#pragma once

using namespace Platform;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Data;

namespace scwin
{

	public ref class ReverseBooleanConverter sealed : public IValueConverter
	{
	public:
		ReverseBooleanConverter();

		// Inherited via IValueConverter
		virtual Platform::Object ^ Convert(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language);
		virtual Platform::Object ^ ConvertBack(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language);
	};

}