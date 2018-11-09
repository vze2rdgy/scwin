#pragma once

namespace scwin
{

	public ref class ProductImageAvailabilityConverter sealed :
		public IValueConverter
	{
	public:
		ProductImageAvailabilityConverter();

		// Inherited via IValueConverter
		virtual Platform::Object ^ Convert(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language);
		virtual Platform::Object ^ ConvertBack(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language);

		property bool IsInverse
		{
			bool get() { return isInverse; }
			void set(bool value) { isInverse = value; }
		}
	private:
		bool isInverse = false;

	};


}
