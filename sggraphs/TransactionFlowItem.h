#pragma once

using namespace Platform;

namespace sggraphs
{
	public enum class TransactionFlowItemRenderType
	{
		ForwardConnector,
		StartNode,
		EndNode,
		Process,
	};

	public ref class TransactionFlowItem sealed
	{
	public:
		TransactionFlowItem();

		property TransactionFlowItemRenderType RenderType;

		property String^ Header;
		property String^ SubHeader;
		property String^ Text;

	};

}