#pragma once

namespace scwin
{

	public ref class StatusMessageTrigger sealed : StateTriggerBase, public ITriggerValue
	{
		static DependencyProperty^ messageTypeProperty;
		StatusMessageTypes msgtrigger;

		bool isActive = false;

	public:

		static void RegisterDps();

		StatusMessageTrigger();

		virtual ~StatusMessageTrigger();

		virtual property bool IsActive
		{
			bool get() { return isActive; };
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

		static property DependencyProperty^ MessageTypeProperty
		{
			DependencyProperty^ get() { return messageTypeProperty; }
		}

		property StatusMessageTypes MessageType
		{
			StatusMessageTypes get() 
			{ 
				auto ret = GetValue(MessageTypeProperty); 
				if (ret == nullptr)
				{
					return StatusMessageTypes::Information;
				}
				auto boxed = dynamic_cast<IBox<StatusMessageTypes>^>(ret);
				return boxed->Value;
			}

			void set(StatusMessageTypes val)
			{
				SetValue(MessageTypeProperty, val);
			}
		}

		property StatusMessageTypes MessageTypeTrigger
		{
			StatusMessageTypes get()
			{
				return msgtrigger;
			}

			void set(StatusMessageTypes val)
			{
				msgtrigger = val;
				IsActive = MessageType == msgtrigger;
			}
		}
	};

}