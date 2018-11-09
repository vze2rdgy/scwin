//
// TransactionFlowControl.xaml.h
// Declaration of the TransactionFlowControl class
//

#pragma once

#include "TransactionFlowControl.g.h"
#include "TransactionFlowItem.h"


using namespace Windows::UI::Xaml;
using namespace Windows::Foundation::Collections;

namespace sggraphs
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class TransactionFlowControl sealed
	{
	private:
		static DependencyProperty^ transactionFlowListProperty;

	public:
		TransactionFlowControl();

		static void Register();

		static property DependencyProperty^ TransactionFlowListProperty
		{
			DependencyProperty^ get() 
			{ 
				return transactionFlowListProperty;
			}
		}

		property IVector<TransactionFlowItem^>^ TransactionFlowItems
		{
			IVector<TransactionFlowItem^>^ get()
			{
				return (IVector<TransactionFlowItem^>^)GetValue(transactionFlowListProperty);
			}

			void set(IVector<TransactionFlowItem^>^ val)
			{
				SetValue(transactionFlowListProperty, val);
			}
		}
	};
}
