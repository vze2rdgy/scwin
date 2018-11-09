#pragma once

namespace scwin 
{

	public enum class SharedViewModelIds
	{
		ProductViewModel
	};

	ref class SharedViewModels sealed
	{
		static Map<SharedViewModelIds, Object^>^ sharedVms;
	public:
		static Object^ SetViewModel(SharedViewModelIds vmid, Object^);
		static Object^ GetViewModel(SharedViewModelIds vmid);
		static IAsyncAction^ RemoveViewModel(SharedViewModelIds vmid);

	internal:
		template<typename T>
		static T^ SetGetViewModel(SharedViewModelIds vmid)
		{
			T^ vm;
			if (sharedVms->HasKey(vmid))
				vm = safe_cast<T^>(sharedVms->Lookup(vmid));
			else 
			{
				GUARDEDGLOCK;
				if (!sharedVms->HasKey(vmid))
				{
					vm = ref new T();
					sharedVms->Insert(vmid, vm);
				}
				else
				{
					vm = safe_cast<T^>(sharedVms->Lookup(vmid));
				}
			}
			return vm;
		}
	};

}