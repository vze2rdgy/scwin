#include "pch.h"
#include "ProductsMapper.h"

using namespace scshared::datatransfer;

ProductsMapper::ProductsMapper()
{
}


ProductsMapper::~ProductsMapper()
{
}

void scshared::datatransfer::ProductsMapper::PopulateSupportedColumnsMustOverride()
{
	// Products Basic Features
	_supportedColumns.emplace_back(std::make_tuple("name", "Name", "Name of the product", DataColumnTypes::String, 0));
	_supportedColumns.emplace_back(std::make_tuple("skun", "SKUN", "Stock Keeping Unit Number", DataColumnTypes::String, 1));
	_supportedColumns.emplace_back(std::make_tuple("subname", "SubName", "Substitute name of the product", DataColumnTypes::String, 3));
	_supportedColumns.emplace_back(std::make_tuple("desc", "Description", "Description of the product", DataColumnTypes::String, 4));
	_supportedColumns.emplace_back(std::make_tuple("shortdesc", "Short Description", "Short description of the product", DataColumnTypes::String, 5));
	_supportedColumns.emplace_back(std::make_tuple("notes", "Notes", "Name of the product", DataColumnTypes::String, 6));
	_supportedColumns.emplace_back(std::make_tuple("barcode", "Barcode", "Barcode identifier of the product", DataColumnTypes::String, 7));
	_supportedColumns.emplace_back(std::make_tuple("isstockless", "'Is Stockless' Flag", "True when the product is generally stored in a warehouse, false when the product is delivered to the buyer directly from the manufacturer/supplier.", DataColumnTypes::Boolean, 7));
	_supportedColumns.emplace_back(std::make_tuple("uom", "Unit of Measure", "Unit of measure application to the product", DataColumnTypes::String, 9));

	// product price plan

	// product attributes/family
}
