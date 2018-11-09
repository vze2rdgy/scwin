#include "pch.h"
#include "Enums.h"

namespace scwin
{
	FunctionalRightsMap FunctionalRightsDescriptions =
	{
		{
			FunctionalRights::ViewProducts,
	{
		"View Products", "A user with 'View Products' rights can only view products in the application."
	}
		},
		{
			FunctionalRights::EditProducts,
	{
		"Edit Products", "A user with 'Edit Products' rights can make changes to existing products in the application."
	}
		},
		{
			FunctionalRights::AddProducts,
	{
		"Add Products", "A user with 'Add Products' rights can add products in the application."
	}
		},
		{
			FunctionalRights::DeleteProducts,
	{
		"Delete Products", "A user with 'Delete Products' rights can remove a product from the application."
	}
		},
		{
			FunctionalRights::ViewCustomers,
	{
		"View Customers", "A user with 'View Customers' rights can only view customers in the application."
	}
		},
		{
			FunctionalRights::EditCustomers,
	{
		"Edit Customers", "A user with 'Edit Customers' rights can make changes to existing customers in the application."
	}
		},
		{
			FunctionalRights::AddCustomers,
	{
		"Add Customers", "A user with 'Add Customers' rights can add custmers in the application."
	}
		},
		{
			FunctionalRights::DeleteCustomers,
	{
		"Delete Customers", "A user with 'Delete Customers' rights can remove a customer from the application."
	}
		},

	};
}