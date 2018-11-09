#include "pch.h"
#include "scUserStatus.h"

using namespace scwin;


scUserStatus::scUserStatus() : DtoBase()
{
}

scUserStatus::scUserStatus(JsonObject^ data) :
	DtoBase(data)
{
}

scUserStatus::~scUserStatus()
{
}

String^ scUserStatus::Code::get()
{
	if (_data->HasKey("code"))
	{
		return _data->GetNamedString("code");
	}
	return nullptr;
}

void scUserStatus::Code::set(String^ value)
{
	_data->SetNamedValue("code", JsonValue::CreateStringValue(value));
}

String^ scUserStatus::Desc::get()
{
	if (_data->HasKey("desc"))
	{
		return _data->GetNamedString("desc");
	}
	return nullptr;
}

void scUserStatus::Desc::set(String^ value)
{
	_data->SetNamedValue("desc", JsonValue::CreateStringValue(value));
}

