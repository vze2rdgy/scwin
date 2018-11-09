#pragma once

namespace scwin
{
	namespace utils
	{
		// Apply guidance from https://msdn.microsoft.com/windows/uwp/input-and-devices/designing-for-tv
		// streetcat\samples\UWPSamples\Samples\XamlNavigation\shared\TenFootStylesheet.xaml
		class SysInfoHelpers sealed
		{
		public:
			// Tenfoot experience is currently enabled in xbox
			static bool IsTenFootExperience();
			static bool IsXbox();
			static bool IsInternetAvailable();
			static Platform::Guid NewGUID();
			static String^ NewCouchDbId();
		
			static String^ WinRtTypeToJsonType(Type^ type);
			static Type^ JsonTypeToWinRtType(String^ jsonType);

			static bool ShowNetworkErrorMessageDialog();
			static bool ShowNotImplementedMessageDialog(String^ title = nullptr);

		};
	}
}

