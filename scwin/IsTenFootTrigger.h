#pragma once

namespace scwin
{
	public ref class IsTenFootTrigger sealed :
		public StateTriggerBase
	{
	private:
		bool isTenFootRequested;

	public:
		IsTenFootTrigger();
	
		property bool IsTenFoot
		{
			bool get();
			void set(bool val);
		}
	
	};


}

