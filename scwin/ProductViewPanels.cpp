#include "pch.h"
#include "ProductViewPanels.h"


void scwin::ProductViewPanel::InvokePropertyChanged(String ^ propName)
{
	WeakReference wr(this);
	Window::Current->Dispatcher->RunAsync(CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([wr, propName]()
	{
		auto ths = wr.Resolve<ProductViewPanel>();
		ths->PropertyChanged(ths, ref new PropertyChangedEventArgs(propName));
	}));
}

ProductViewPanel::ProductViewPanel()
{
}
