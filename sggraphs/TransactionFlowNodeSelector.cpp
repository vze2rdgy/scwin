#include "pch.h"
#include "TransactionFlowNodeSelector.h"
#include "TransactionFlowItem.h"

using namespace sggraphs;

TransactionFlowNodeSelector::TransactionFlowNodeSelector(ResourceDictionary^ controlResources)
	:controlResources(controlResources)
{

}

DataTemplate ^ sggraphs::TransactionFlowNodeSelector::SelectTemplateCore(Object ^ item)
{
	return SelectTemplateCore(item, nullptr);
}

DataTemplate ^ sggraphs::TransactionFlowNodeSelector::SelectTemplateCore(Object ^ item, DependencyObject ^ container)
{
	if (item == nullptr)
		return nullptr;
	auto transFlowItem = dynamic_cast<TransactionFlowItem^>(item);
	if (transFlowItem == nullptr)
		return nullptr;
	auto etype = transFlowItem->RenderType;
	DataTemplate^ tmpl = nullptr;
	switch (etype)
	{
	case TransactionFlowItemRenderType::StartNode:
		tmpl = (DataTemplate^)controlResources->Lookup("StartNodeTemplate");
		break;
	case TransactionFlowItemRenderType::ForwardConnector:
		tmpl = (DataTemplate^)controlResources->Lookup("ForwardConnectorTemplate");
		break;
	case TransactionFlowItemRenderType::Process:
		tmpl = (DataTemplate^)controlResources->Lookup("ProcessNodeTemplate");
		break;
	case TransactionFlowItemRenderType::EndNode:
		tmpl = (DataTemplate^)controlResources->Lookup("EndNodeTemplate");
		break;
	}
	return tmpl;
}
