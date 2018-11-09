#include "pch.h"
#include "DataMapper.h"

using namespace scshared::datatransfer;

void scshared::datatransfer::DataMapper::Cleanup()
{
	if (!_bIsCleanedUp)
	{

		_bIsCleanedUp = true;
	}
}

DataMapper::DataMapper()
{
	PopulateSupportedColumnsMustOverride();
}


DataMapper::~DataMapper()
{
	Cleanup();
}

std::vector<ColumnDefinition> scshared::datatransfer::DataMapper::list_SupportedColumns()
{
	return _supportedColumns;
}
