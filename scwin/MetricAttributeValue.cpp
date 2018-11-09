#include "pch.h"
#include "MetricAttributeValue.h"

namespace scwin
{

	MetricAttributeValue::MetricAttributeValue()
	{
		this->id = SysInfoHelpers::NewCouchDbId();
	}

	MetricAttributeValue::MetricAttributeValue(String ^ id)
	{
		this->id = id;
	}

	Object^ MetricAttributeValue::Value::get()
	{
		return value;
	}

	void MetricAttributeValue::Value::set(Object^ val)
	{
		if (value != val)
		{
			if (val != nullptr && val->GetType() != double::typeid)
			{
				ThrowException(MessageIds::IncorrectProductAttributeType);
			}
			value = safe_cast<double>(val);
			RaisePropertyChanged("Value");
		}
	}

	void MetricAttributeValue::RaisePropertyChanged(String ^ propName)
	{
		if (!RaiseEvents) return;
		WeakReference wr(this);
		if (Window::Current)
		{
			auto disp = Window::Current->Dispatcher;
			disp->RunAsync(CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([wr, propName]()
			{
				auto ths = wr.Resolve<MetricAttributeValue>();
				ths->PropertyChanged(ths, ref new PropertyChangedEventArgs(propName));
			}
			));
		}
		else
		{
			PropertyChanged(this, ref new PropertyChangedEventArgs(propName));
		}
	}

}