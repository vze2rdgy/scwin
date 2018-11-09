#include "pch.h"
#include "CategoricalAttributeValue.h"

namespace scwin
{

	CategoricalAttributeValue::CategoricalAttributeValue()
	{
		this->id = SysInfoHelpers::NewCouchDbId();
	}

	CategoricalAttributeValue::CategoricalAttributeValue(String ^ id)
	{
		this->id = id;
	}

	IObservableVector<String^>^ CategoricalAttributeValue::Values::get()
	{
		if (values == nullptr)
		{
			GUARDEDGLOCK;
			if (values == nullptr)
			{
				values = ref new Vector<String^>();
			}
		}
		return values;
	}

	Object^ CategoricalAttributeValue::Value::get()
	{
		return value; 
	}

	void CategoricalAttributeValue::Value::set(Object^ val)
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

	void CategoricalAttributeValue::RaisePropertyChanged(String ^ propName)
	{
		if (!RaiseEvents) return;
		WeakReference wr(this);
		if (Window::Current)
		{
			auto disp = Window::Current->Dispatcher;
			disp->RunAsync(CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([wr, propName]()
			{
				auto ths = wr.Resolve<CategoricalAttributeValue>();
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