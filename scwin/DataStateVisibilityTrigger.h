#pragma once

#include "dto.h"

namespace scwin
{
	public ref class DataStateVisibilityTrigger sealed : public StateTriggerBase
	{
		static DependencyProperty^ objectStateProperty;
		static DependencyProperty^ objectStateTriggerProperty;

	public:

		static void Register();

		DataStateVisibilityTrigger();

		static property DependencyProperty^ ObjectStateProperty
		{
			DependencyProperty^ get() 
			{ 
				return objectStateProperty; 
			}
		}

		static property DependencyProperty^ ObjectStateTriggerProperty
		{
			DependencyProperty^ get() 
			{ 
				return objectStateProperty; 
			}
		}

		property DataState ObjectState
		{
			DataState get() 
			{
				auto val = dynamic_cast<IBox<DataState>^>(GetValue(objectStateProperty));
				if (val == nullptr)
					return DataState::Unmodified;
				return val->Value;
			}

			void set(DataState val)
			{
				SetValue(objectStateProperty, val);
			}
		}

		property DataState ObjectStateTrigger
		{
			DataState get()
			{
				auto val = dynamic_cast<IBox<DataState>^>(GetValue(objectStateTriggerProperty));
				if (val == nullptr)
					return DataState::Unmodified;
				return val->Value;
			}

			void set(DataState val)
			{
				SetValue(objectStateTriggerProperty, ref new Box<DataState>(val));
			}
		}

	private:
		static void OnPropertyChanged(DependencyObject^ dp, DependencyPropertyChangedEventArgs^ eargs);

	};

}