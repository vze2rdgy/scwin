#pragma once

namespace scwin
{

	ref class BreadcrumbControl;

	public ref class BreadcrumbEventArgs sealed : public RoutedEventArgs
	{
	public:
		BreadcrumbEventArgs(Object^ item, int index);

		property Object^ Item;
		property int ItemIndex;
	};

	public delegate void BreadcrumbEventHandler(BreadcrumbControl^ control, BreadcrumbEventArgs^ eargs);

}