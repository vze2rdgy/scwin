#pragma once

#include "DataMapper.h"

namespace scshared
{
	namespace datatransfer
	{

		class ProductsMapper : public DataMapper
		{
		public:
			ProductsMapper();
			virtual ~ProductsMapper();

		protected:
			// Inherited via DataMapper
			virtual void PopulateSupportedColumnsMustOverride() override;
		};

	}
}