#include "pch.h"
#include "ViewLifeTimeController.h"

scwin::ViewLifeTimeController::ViewLifeTimeController(CoreWindow^ newWindow) : window(newWindow)
{
	dispatcher = newWindow->Dispatcher;
	viewId = ApplicationView::GetApplicationViewIdForWindow(window.Get());
	RegisterForEvents();
}

void scwin::ViewLifeTimeController::RegisterForEvents()
{
	consolToken = ApplicationView::GetForCurrentView()->Consolidated += ref new Windows::Foundation::TypedEventHandler<Windows::UI::ViewManagement::ApplicationView ^, Windows::UI::ViewManagement::ApplicationViewConsolidatedEventArgs ^>(this, &scwin::ViewLifeTimeController::ViewConsolidated);
}

void scwin::ViewLifeTimeController::UnregisterEvents()
{
	ApplicationView::GetForCurrentView()->Consolidated -= consolToken;
}

void scwin::ViewLifeTimeController::ViewConsolidated(ApplicationView ^ sender, ApplicationViewConsolidatedEventArgs ^ e)
{
	StopViewInUse();
}

void scwin::ViewLifeTimeController::FinalRelease()
{
	bool justreleased = false;
	{
		lock_guard<mutex> g(mtx);
		if (refCount == 0)
		{
			justreleased = true;
			released = true;
		}
	}
	if (justreleased)
	{
		UnregisterEvents();
		InternalReleased(this, nullptr);
	}
}

ViewLifeTimeController^ scwin::ViewLifeTimeController::CreateForCurrentView()
{
	return ref new ViewLifeTimeController(CoreWindow::GetForCurrentThread());
}

int scwin::ViewLifeTimeController::StartViewInUse()
{
	bool releasedcopy = false;
	int refcountcopy = 0;
	{
		lock_guard<mutex> g(mtx);
		releasedcopy = this->released;
		if (!this->released)
			refcountcopy = ++this->refCount;
	}
	if (releasedcopy)
		throw Exception::CreateException(E_FAIL, "This view is being disposed.");
	return refcountcopy;
}

int scwin::ViewLifeTimeController::StopViewInUse()
{
	int refcountycopy = 0;
	bool releasedcopy = false;
	{
		lock_guard<mutex> g(mtx);
		refcountycopy = --refCount;
		if (refcountycopy == 0)
		{
			dispatcher->RunAsync(CoreDispatcherPriority::Low, ref new DispatchedHandler(this, &scwin::ViewLifeTimeController::FinalRelease));
		}
	}
	if (releasedcopy)
	{
		throw Exception::CreateException(E_FAIL, "This view is being disposed");
	}
	return refcountycopy;
}

