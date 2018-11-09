#pragma once

using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace std;

namespace scwin
{

	delegate void ViewReleasedHandler(Object^ sender, Windows::UI::Core::ICoreWindowEventArgs^ e);

	ref class ViewLifeTimeController sealed : public INotifyPropertyChanged
	{
		event ViewReleasedHandler^ InternalReleased;
		CoreDispatcher^ dispatcher;
		Agile<CoreWindow^> window;
		String^ title;
		int refCount = 0;
		int viewId = 0;
		bool released = false;
		bool consolidated = true;
		EventRegistrationToken consolToken;
		std::mutex mtx;

	private:
		ViewLifeTimeController(CoreWindow^ newWindow);
		void RegisterForEvents();
		void UnregisterEvents();
		void ViewConsolidated(ApplicationView^ sender, ApplicationViewConsolidatedEventArgs^ e);
		void FinalRelease();

	public:
		static ViewLifeTimeController^ CreateForCurrentView();

		property String^ Title
		{
			String^ get() { return title; }
			void set(String^ val)
			{
				if (title != val)
				{
					title = val;
					PropertyChanged(this, ref new PropertyChangedEventArgs("Title"));
				}
			}
		}

		property CoreDispatcher^ Dispatcher
		{
			CoreDispatcher^ get() { return dispatcher; }
		}

		property int Id
		{
			int get() { return this->viewId; }
		}

		event ViewReleasedHandler^ Released
		{
			EventRegistrationToken add(ViewReleasedHandler^ handler)
			{
				EventRegistrationToken tok;
				bool releasedcopy = false;
				lock_guard<mutex> g(mtx);
				releasedcopy = released;
				if (!released)
					tok = InternalReleased += handler;
				if (releasedcopy)
					throw Exception::CreateException(E_FAIL, "This view is being disposed.");
				return tok;
			}

			void remove(EventRegistrationToken token)
			{
				InternalReleased -= token;
			}

			void raise(Object^ sender, ICoreWindowEventArgs^ e)
			{
				return InternalReleased(sender, e);
			}
		}

		int StartViewInUse();
		int StopViewInUse();

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;
	};


}
