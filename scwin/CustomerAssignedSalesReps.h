#pragma once

#include "dto.h"

namespace scwin
{
	[Bindable]
	public ref class CustomerAssignedSalesReps sealed : IDTOBase
	{
	public:
		CustomerAssignedSalesReps();
		virtual ~CustomerAssignedSalesReps();

		// Inherited via IDTOBase
		virtual property DataState ObjectState;
		virtual property bool IsEmpty
		{
			bool get()
			{
				return false;
			}
		}

		virtual property bool IsValid 
		{
			bool get() 
			{
				return true;
			}
		}



		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		// Inherited via IDTOBase
		virtual property bool CanRaiseEvents;

	private:
		void RaisePropertyChanged(String ^ propName);
	};

}