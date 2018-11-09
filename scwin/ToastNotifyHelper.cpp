#include "pch.h"
#include "ToastNotifyHelper.h"

void scwin::ToastNotifyHelper::ShowBasic(XmlDocument ^ toastData)
{
	//auto toastData = ref new Uri("Toasts/ProductTierPriceChosenToast.xml");
	//StreamResourceInfo info = Application.GetResourceStream(uri);

	auto toastXml = ref new XmlDocument();
	toastXml->LoadXml("<toast>"
		"<visual>"
		"<binding template = 'ToastGeneric'>"
		"<text>Ghost Toast</text>"
		"<text>Time's up</text>"
		"</binding>"
		"</visual>"
		"<actions>"
		"<action content = 'dismiss' activationType='foreground' arguments = 'dismiss'/>"
		"</actions>"
		"</toast>");
	auto toast = ref new ToastNotification(toastXml);
	ToastNotificationManager::CreateToastNotifier()->Show(toast);
}
