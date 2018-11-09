#pragma once

namespace scwin
{
	public ref class AppProperty sealed 
	{
	public:

		AppProperty()
		{
		}

		AppProperty(Platform::String^ name, Platform::String^ value)
		{
			Name = name;
			Value = value;
			IsRoaming = false;
		}

		AppProperty(Platform::String^ name, Platform::String^ value, bool isRoaming)
		{
			Name = name;
			Value = value;
			IsRoaming = isRoaming;
		}

		property Platform::String^ Name;
		property Platform::String^ Value;
		property bool IsRoaming;

		bool HasValue();
	};

}