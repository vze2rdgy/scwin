#pragma once

using namespace Windows::Data::Xml::Dom;
using namespace Windows::UI::Notifications;

namespace scwin
{

	class ToastNotifyHelper
	{
	public:
		static void ShowBasic(XmlDocument^ toastData);
	};

}