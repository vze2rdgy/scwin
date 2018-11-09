#include "pch.h"
#include "ProductAttributeAndValue.h"


ProductAttributeAndValue::ProductAttributeAndValue()
{
}

scwin::ProductAttributeAndValue::ProductAttributeAndValue(UINT ordinal, ProductAttribute ^ attribute, GeneralAttributeValue ^ attrvalue)
{
	if (attribute == nullptr || attrvalue == nullptr)
		throw Exception::CreateException(E_INVALIDARG, "attribute or attrvalue is invalid");

	Ordinal = ordinal;
	Attribute = attribute;
	Value = attrvalue;
}

DEFINERAISEPROPCHANGED(ProductAttributeAndValue)