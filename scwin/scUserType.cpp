#include "pch.h"
#include "scUserType.h"

using namespace scwin;

scUserType::scUserType() : DtoBase()
{
}

scUserType::scUserType(JsonObject ^ data) : DtoBase(data)
{
}

String ^ scUserType::Name()
{
	if (_data->HasKey("name"))
	{
		return _data->GetNamedString("name");
	}
	return nullptr;
}

void scUserType::setName(String ^ val)
{
	if (val == nullptr)
		_data->SetNamedValue("name", JsonValue::CreateNullValue());
	else
		_data->SetNamedValue("name", JsonValue::CreateStringValue(val));
}

