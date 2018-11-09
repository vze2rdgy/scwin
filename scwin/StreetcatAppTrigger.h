#pragma once

#include "scMainViewModel.h"

namespace scwin
{

	public ref class StreetcatAppTrigger sealed :
		public StateTriggerBase
	{
	private:
		static DependencyProperty^ isAuthorizedProperty;
		scMainViewModel^ mvm;

	private:
		void UpdateTrigger();

	public:
		static void RegisterDependencyProperties();

		static property DependencyProperty^ IsAuthorizedProperty
		{
			DependencyProperty^ get();
		}

		static void OnIsAuthorizedPropertyChanged(DependencyObject^ d, DependencyPropertyChangedEventArgs^ e);
		
		StreetcatAppTrigger();


		property bool IsAuthorized
		{
			bool get();
			void set(bool val);
		}

	};


}

