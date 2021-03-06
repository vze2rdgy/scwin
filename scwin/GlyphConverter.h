#pragma once

namespace scwin
{
	[Windows::UI::Xaml::Data::Bindable]
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class GlyphConverter sealed : Windows::UI::Xaml::Data::IValueConverter
	{
	public:
		GlyphConverter();

		// Inherited via IValueConverter
		virtual Platform::Object ^ Convert(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language);
		virtual Platform::Object ^ ConvertBack(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language);

		property Platform::String^ ExpandedGlyph
		{
			Platform::String^ get() { return expandedGlyph; }
			void set(Platform::String^ value) { expandedGlyph = value; }
		}

		property Platform::String^ CollapsedGlyph
		{
			Platform::String^ get() { return collapsedGlyph; }
			void set(Platform::String^ value) { collapsedGlyph = value; }
		}
	private:
		Platform::String^ expandedGlyph = nullptr;
		Platform::String^ collapsedGlyph = nullptr;
	};

}