#pragma once

using namespace Platform;
using namespace Windows::UI::Xaml::Data;

namespace sgCore
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class BrushConverter sealed : public IValueConverter
	{
	public:
		BrushConverter();

		// Inherited via IValueConverter
		virtual Object ^ Convert(Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Object ^parameter, String ^language);
		virtual Object ^ ConvertBack(Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Object ^parameter, String ^language);
	};
}

