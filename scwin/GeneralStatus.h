#pragma once

namespace scwin
{

	public ref class GeneralStatus sealed
	{
	public:
		GeneralStatus();
		GeneralStatus(int id, String^ publicName, String^ privateName);

		property int Id;
		property String^ PrivateName;
		property String^ PublicName;
		property String^ Description;

	};

}