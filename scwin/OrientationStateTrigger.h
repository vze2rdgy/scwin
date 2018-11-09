#pragma once

#include "ITriggerValue.h"

using namespace Windows::Graphics::Display;

namespace scwin
{

	/// <summary>
	/// Trigger for switching when the screen orientation changes
	/// </summary>
	public ref class OrientationStateTrigger sealed : public StateTriggerBase, public ITriggerValue
	{
		static DependencyProperty^ orientationProperty;
		EventRegistrationToken tok;
		bool isActive;

	public:

		static void RegisterDps();

		/// <summary>
		/// Initializes a new instance of the OrientationStateTrigger class.
		/// </summary>
		OrientationStateTrigger();

		virtual ~OrientationStateTrigger();

		// Inherited via ITriggerValue
		virtual property bool IsActive 
		{
			bool get() 
			{ 
				return isActive; 
			}
		private:
			void set(bool val)
			{
				if (isActive != val)
				{
					isActive = val;
					StateTriggerBase::SetActive(val);
					IsActiveChanged(this, isActive);
				}
			}
		}

		virtual event TypedEventHandler<ITriggerValue^, bool>^ IsActiveChanged;

		property Orientations Orientation
		{
			Orientations get()
			{
				auto ret = GetValue(orientationProperty);
				if (ret == nullptr)
					return Orientations::Landscape;
				return (Orientations)GetValue(orientationProperty);
			}
			void set(Orientations val)
			{
				SetValue(orientationProperty, val);
			}
		}

	private:

		static void OnOrientationPropertyChanged(DependencyObject^ d, DependencyPropertyChangedEventArgs^ e);

		void UpdateTrigger(DisplayOrientations orientation);

		void OrientationStateTrigger_OrientationChanged(DisplayInformation^ sender, Object^ args);

	};


}