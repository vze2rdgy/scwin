#pragma once

namespace scwin
{

	public enum class SearchContexts
	{
		Global,
		Contextual,
	};

	ref class SearchData sealed
	{
	public:
		SearchData(SearchContexts ctxt, String^ searchText);
		property SearchContexts Context;
		property String^ SearchText;
	};

}
