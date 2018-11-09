#include "pch.h"
#include "LabelAttributeValue.h"

namespace scwin
{

	LabelAttributeValue::LabelAttributeValue()
	{
		id = SysInfoHelpers::NewCouchDbId();
	}

	LabelAttributeValue::LabelAttributeValue(String ^ id)
	{
		this->id = id;
	}

	Object^ LabelAttributeValue::Value::get()
	{
		return value;
	}

	void LabelAttributeValue::Value::set(Object^ val)
	{
		if (value != val)
		{
			if (val != nullptr && (value = dynamic_cast<String^>(val)) == nullptr)
			{
				ThrowException(MessageIds::IncorrectProductAttributeType);
			}
			value = safe_cast<String^>(val);
			RaisePropertyChanged("Value");
		}
	}

	void LabelAttributeValue::RaisePropertyChanged(String ^ propName)
	{
		if (!RaiseEvents) return;
		WeakReference wr(this);
		if (Window::Current)
		{
			auto disp = Window::Current->Dispatcher;
			disp->RunAsync(CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([wr, propName]()
			{
				auto ths = wr.Resolve<LabelAttributeValue>();
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