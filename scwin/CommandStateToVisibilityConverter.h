#pragma once

namespace scwin
{
	[Bindable]
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class CommandStateToVisibilityConverter sealed :
		public IValueConverter
	{
		bool isInverse = false;

	public:

		CommandStateToVisibilityConverter();

		property bool IsInverse
		{
			bool get() { return isInverse; }
			void set(bool value) { isInverse = value; }
		}

		// Inherited via IValueConverter
		virtual Platform::Object ^ Convert(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language);
		virtual Platform::Object ^ ConvertBack(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language);
	};

}