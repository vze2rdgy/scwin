//
// MobView.xaml.h
// Declaration of the MobView class
//

#pragma once

#include "MobView.g.h"

namespace scwin
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class MobView sealed
	{
	public:
		MobView(scMainViewModel^ vm);
	};
}
