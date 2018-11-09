#pragma once

#include "SalesOrderWrapper.h"

namespace scwin
{

	public ref class OrderGroupedInfoList sealed
	{
	public:
		OrderGroupedInfoList();

		property String^ Key;

		property IObservableVector<SalesOrderWrapper^>^ Orders;
	};

}