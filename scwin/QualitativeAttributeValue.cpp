#include "pch.h"
#include "QualitativeAttributeValue.h"

namespace scwin
{

	QualitativeAttributeValue::QualitativeAttributeValue()
	{
		id = SysInfoHelpers::NewCouchDbId();
	}

	QualitativeAttributeValue::QualitativeAttributeValue(String ^ id)
	{
		this->id = id;
	}

	Object^ QualitativeAttributeValue::Value::get()
	{
		return value;
	}

	void QualitativeAttributeValue::Value::set(Object^ val)
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

	void QualitativeAttributeValue::RaisePropertyChanged(String ^ propName)
	{
		if (!RaiseEvents) return;
		WeakReference wr(this);
		if (Window::Current)
		{
			auto disp = Window::Current->Dispatcher;
			disp->RunAsync(CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([wr, propName]()
			{
				auto ths = wr.Resolve<QualitativeAttributeValue>();
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