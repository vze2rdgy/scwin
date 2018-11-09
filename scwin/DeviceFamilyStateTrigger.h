#pragma once

#include "Enums.h"
#include "ITriggerValue.h"

namespace scwin
{
	public ref class DeviceFamilyStateTrigger sealed : 
		public StateTriggerBase, 
		public ITriggerValue
	{
		static DependencyProperty^ deviceFamilyProperty;
		static String^ deviceFamily;
		bool isActive;

	public:

		static void Register();

		static property DependencyProperty^ DevicefamilyProperty
		{
			DependencyProperty^ get();
		}

		DeviceFamilyStateTrigger();

		// Inherited via ITriggerValue
		virtual property bool IsActive
		{
			bool get();
		internal:
			void set(bool);
		}

		virtual event TypedEventHandler<ITriggerValue^, bool>^ IsActiveChanged;

		property DeviceFamily Devicefamily
		{
			DeviceFamily get();
			void set(DeviceFamily val);
		}


	private:
		static void OnDeviceTypePropertyChanged(
			DependencyObject^ d,
			DependencyPropertyChangedEventArgs^ e);
	};

}