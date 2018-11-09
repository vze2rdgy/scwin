#pragma once

namespace scwin
{
	/// <summary>
	/// Implement this interface to be able to observe triggers 
	/// </summary>
	public interface class ITriggerValue
	{
		/// <summary>
		/// Gets a value indicating whether this trigger is active.
		/// </summary>
		/// <value><c>true</c> if this trigger is active; otherwise, <c>false</c>.</value>
		property bool IsActive{ bool get(); }

		/// <summary>
		/// Occurs when the <see cref="IsActive"/> property has changed.
		/// </summary>
		event TypedEventHandler<ITriggerValue^, bool>^ IsActiveChanged;
	};

}