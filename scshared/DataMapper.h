#pragma once
#include "Enums.h"

namespace scshared
{
	namespace datatransfer
	{
		/// Definition of a column. 
		/// The first item in the tuple is the name of the column in the table and is not displayed to the user.
		/// 2nd item is the user-displayed column name 
		/// 3nd item is a description of the column
		/// 4nd item is the type of the column.
		/// 5rd item is ordinal of the column when listed.
		typedef std::tuple<std::string, std::string, std::string, DataColumnTypes, int > ColumnDefinition;
		/*
		Abstract base class for any data mapping.
		*/
		class DataMapper
		{
		private:
			bool _bIsCleanedUp = false;
		protected:
			std::vector<ColumnDefinition> _supportedColumns;

		protected:
			virtual void PopulateSupportedColumnsMustOverride() = 0;
			virtual void Cleanup();

		public:
			DataMapper();
			virtual ~DataMapper();

			/// List of columns supported by streetcat. For a datamapping tool, 
			/// each selected column from an data import input file is mapped to 
			/// one of these columns.
			std::vector<ColumnDefinition> list_SupportedColumns();
		};
	}
}
