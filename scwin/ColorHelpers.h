#pragma once

using namespace Platform;
using namespace Windows::UI;
using namespace Windows::UI::Core;

namespace scwin
{
	namespace utils
	{
		[Windows::Foundation::Metadata::WebHostHiddenAttribute]
		ref class ColorHelpers sealed
		{
		internal:
			static void SetFromHex(Color& c, String^ hex);
			static Color ToColorFromHex(String^ hex);

		};
	}
}

