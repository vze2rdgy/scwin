//
// TransactionFlowControl.xaml.cpp
// Implementation of the TransactionFlowControl class
//

#include "pch.h"
#include "TransactionFlowControl.xaml.h"
#include "TransactionFlowNodeSelector.h"

using namespace sggraphs;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

DependencyProperty^ TransactionFlowControl::transactionFlowListProperty = nullptr;

TransactionFlowControl::TransactionFlowControl()
{
	InitializeComponent();
	//ItemTemplateSelector="{StaticResource TransactionFlowNodeSelector}"
	ListViewOfNodes->ItemTemplateSelector = ref new TransactionFlowNodeSelector(Resources);
}

void sggraphs::TransactionFlowControl::Register()
{
	if (transactionFlowListProperty == nullptr)
	{
		transactionFlowListProperty = DependencyProperty::Register(
			"TransactionFlowList",
			IVector<TransactionFlowItem^>::typeid,
			TransactionFlowControl::typeid,
			ref new Windows::UI::Xaml::PropertyMetadata(nullptr)
		);
	}
}
