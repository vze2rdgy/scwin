#pragma once

namespace scwin
{
	public interface class IProductAttributeValue : IDTOBase
	{
		property String^ Id
		{
			String^ get();
		}

		property Platform::Type^ DataType
		{
			Platform::Type^ get();
		}

		property Object^ Value
		{
			Object^ get();
			void set(Object^ val);
		}
	};

}

