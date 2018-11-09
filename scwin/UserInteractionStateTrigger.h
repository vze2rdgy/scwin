#pragma once

namespace scwin
{

	public ref class UserInteractionStateTrigger sealed : public StateTriggerBase, public ITriggerValue
	{
		static DependencyProperty^ interactionMode;
		bool isActive;

	public:

		static void Register();

		UserInteractionStateTrigger();

		// Inherited via ITriggerValue
		virtual property bool IsActive
		{
		public:
			bool get();
		internal:
			void set(bool);
		}

		static property DependencyProperty^ InteractionModeProperty
		{
			DependencyProperty^ get();
		}

		property Windows::UI::ViewManagement::UserInteractionMode InteractionMode
		{
			Windows::UI::ViewManagement::UserInteractionMode get();
			void set(Windows::UI::ViewManagement::UserInteractionMode);
		}

		virtual event TypedEventHandler<ITriggerValue^, bool>^ IsActiveChanged;

	private:
		void UpdateTrigger(Windows::UI::ViewManagement::UserInteractionMode mode);
		void Window_SizeChanged(Object^ sender, WindowSizeChangedEventArgs^ e);
		static void OnInteractionModeChanged(DependencyObject^ d, DependencyPropertyChangedEventArgs^ e);

	};

}