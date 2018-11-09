#include "pch.h"
#include "DTOWrappers.h"


DEFINERAISEPROPCHANGEDBASIC(ProductSelectable)

scwin::ProductSelectable::ProductSelectable(Product ^ product, bool isselected) : product(product), isSelected(isselected)
{
}
