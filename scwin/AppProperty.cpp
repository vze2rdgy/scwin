#include "pch.h"

bool scwin::AppProperty::HasValue()
{
	return !StringHelpers::IsNullOrEmpty(Value);
}
