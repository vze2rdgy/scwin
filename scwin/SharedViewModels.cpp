#include "pch.h"
#include "SharedViewModels.h"


Map<SharedViewModelIds, Object^>^ SharedViewModels::sharedVms = ref new Map<SharedViewModelIds, Object^>();


Object^ scwin::SharedViewModels::SetViewModel(SharedViewModelIds vmid, Object^ vm)
{
	if (!sharedVms->HasKey(vmid))
	{
		GUARDEDGLOCK;
		if (!sharedVms->HasKey(vmid))
		{
			sharedVms->Insert(vmid, vm);
		}
	}
	return vm;
}

Object ^ scwin::SharedViewModels::GetViewModel(SharedViewModelIds vmid)
{
	return sharedVms->Lookup(vmid);
}

IAsyncAction ^ scwin::SharedViewModels::RemoveViewModel(SharedViewModelIds vmid)
{
	return create_async([=]
	{
		if (sharedVms->HasKey(vmid))
		{
			GUARDEDGLOCK;
			if (sharedVms->HasKey(vmid))
			{
				sharedVms->Remove(vmid);
			}
		}
	});
}
