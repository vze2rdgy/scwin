#include "pch.h"
#include "BreadcrumbEventArgs.h"

BreadcrumbEventArgs::BreadcrumbEventArgs(Object^ item, int index)
{
	Item = item;
	ItemIndex = index;
}

