#include "pch.h"
#include "SearchContexts.h"


scwin::SearchData::SearchData(SearchContexts ctxt, String ^ searchText) 
{
	this->Context = ctxt;
	this->SearchText = searchText;
}
