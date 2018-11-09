#include "pch.h"
#include "AttributeViewModelFacade.h"
#include "cpplinq.hpp"


AttributeViewModelFacade::AttributeViewModelFacade(Product^ p) : product(p)
{
	Initialize();
}

scwin::AttributeViewModelFacade::~AttributeViewModelFacade()
{
}

void scwin::AttributeViewModelFacade::Initialize()
{
	UpdateFamily(product != nullptr && product->Family != nullptr ? product->Family : ref new ProductFamily());
	create_task(LoadAttribsAsync())
		.then([this]
	{
		UpdateState();
	});
}


void scwin::AttributeViewModelFacade::UpdateFamily(ProductFamily ^ fam)
{
	family = fam;
	FamilyName = family->Name;
	InvokePropertyChangedAsync("FamilyName");
}

void scwin::AttributeViewModelFacade::UpdateFamilyName(String ^ val)
{
	// check if family already available.
	auto result = scLocalDataService::Instance()->GetProductFamilies("name='" + STRSAFECONV(val) + "'");
	if (result->Size > 0)
	{
		// get the first
		Family = result->First()->Current;
	}
	else
	{
		FamilyName = val;
		Family = nullptr;
	}
}

IAsyncOperation<IVector<ProductFamily^>^>^ scwin::AttributeViewModelFacade::GetMatchingFamiliesAsync(String ^ familyNameOrDesc)
{
	return create_async([familyNameOrDesc]() -> IVector<ProductFamily^>^
	{
		ostringstream ocrit;
		ocrit << "name like '%" << STRCONV(familyNameOrDesc) << "%' or desc like '%" << STRCONV(familyNameOrDesc) << "%'";
		auto list = create_task(scLocalDataService::Instance()->GetProductFamiliesAsync(ocrit.str())).get();
		return list;
	});
}

IAsyncAction ^ scwin::AttributeViewModelFacade::LoadAttribsAsync()
{
	WeakReference wr(this);
	return create_async([wr]()
	{
		auto ths = wr.Resolve<AttributeViewModelFacade>();
		if (ths->product != nullptr)
		{
			create_task(scLocalDataService::Instance()->GetProductAttributesAsync(ths->product->Id))
				.then([wr](IVector<ProductAttributeAndValue^>^ vect)
			{
				auto ths = wr.Resolve<AttributeViewModelFacade>();
					ths->ProductAttributes = vect;
			});
		}
	});
}

void scwin::AttributeViewModelFacade::UpdateState()
{
	if (product->Family == nullptr && (prodAttributes == nullptr || !prodAttributes->Size))
	{
		this->VisualStateTrigger = VisualStateTriggers::NoDataState;
	}
	else
	{
		this->VisualStateTrigger = VisualStateTriggers::ReadState;
	}
}

void scwin::AttributeViewModelFacade::Reset()
{
	Initialize();
}

void scwin::AttributeViewModelFacade::InvokePropertyChangedAsync(String ^ propName)
{
	Dispatcher->RunAsync(CoreDispatcherPriority::Normal, 
		ref new DispatchedHandler([this, propName]()
	{
		InvokePropertyChanged(propName);
	}));
}

void scwin::AttributeViewModelFacade::InvokePropertyChanged(String ^ propName)
{
	PropertyChanged(this, ref new PropertyChangedEventArgs(propName));
}
