#pragma once

namespace scwin
{
	public ref class BreadcrumbData sealed
	{
	public:
		property String^ Text;
		property Object^ PageParameter;
		property TypeName PageTypeName;
	};
}

