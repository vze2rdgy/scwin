#include "pch.h"
#include "DtoBase.h"

using namespace scwin;


DtoBase::DtoBase() : _data(ref new JsonObject())
{
}

DtoBase::DtoBase(JsonObject^ data) : _data(data)
{
}



DtoBase::~DtoBase()
{
}

JsonObject ^ DtoBase::serialize()
{
	return _data;
}

String^ DtoBase::Id::get()
{
	return _data->GetNamedObject("_Id")->GetString();
}

uint64 DtoBase::CreatedOn::get()
{
	return 0L;
}

String^ DtoBase::RevisionNum::get()
{
	if (_data->HasKey("_rev"))
	{
		return _data->GetNamedString("_rev");
	}
	return nullptr;
}

