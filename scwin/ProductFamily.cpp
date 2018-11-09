#include "pch.h"
#include "ProductFamily.h"

namespace scwin
{

	ProductFamily::ProductFamily()
	{
		this->id = SysInfoHelpers::NewCouchDbId();
		objectState = DataState::Added;
		CanRaiseEvents = true;
	}

	ProductFamily::ProductFamily(String ^ id)
	{
		this->id = id;
		objectState = DataState::Unmodified;
		CanRaiseEvents = true;
	}

	ProductFamily ^ ProductFamily::Clone()
	{
		auto fam = ref new ProductFamily(id);
		fam->name = name;
		fam->desc = desc;
		fam->shortDesc = shortDesc;
		fam->notes = notes;
		return fam;
	}

	void ProductFamily::NotifyObjectStateChange()
	{
		auto pc = ref new PropertyChangedEventArgs("ObjectState");
		if (Window::Current->Dispatcher->HasThreadAccess)
		{
			PropertyChanged(this, pc);
		}
		else
		{
			WeakReference wr(this);
			Window::Current->Dispatcher->RunAsync(
				CoreDispatcherPriority::Normal,
				ref new DispatchedHandler([wr, pc]()
			{
				auto ths = wr.Resolve<ProductFamily>();
				ths->PropertyChanged(ths, pc);
			}));
		}
	}

	bool ProductFamily::IsEmpty::get()
	{
		return StringHelpers::IsNullOrEmpty(this->name);
	}

	DEFINERAISEPROPCHANGED(ProductFamily)
}