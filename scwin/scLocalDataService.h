#pragma once

#include "scRemoteDataService.h"
#include "NotifierT.h"
#include "ReplicationService.h"
#include <string>
#include "Enums.h"
#include "typedefinitions.h"
#include "ProductAttributeAndValue.h"

namespace f = Windows::Foundation;

namespace scwin 
{
	class scLocalDataService sealed 
	{
	private:

		scRemoteDataService* rmData;
		static scLocalDataService* singleton;
		// reference to the replicator
		scwin::services::ReplicatorService* replicator;
		DependencyObject^ mainVm;

	private:
		scLocalDataService();

		void ReplicationCallback(const std::wstring& typeName, const std::wstring& jsonData);

		void LogEx(Exception^ ex);

		MessageIds InsertPricePlan(ProductPricePlan^ priceplan);
		MessageIds UpdatePricePlan(ProductPricePlan^ priceplan);
		MessageIds InsertStdPriceDetails(PricePlanStandardDetails^ dets);
		MessageIds UpdateStdPriceDetails(PricePlanStandardDetails^ dets);
		MessageIds InsertTieredPriceDetails(PricePlanTierDetails^ dets);
		MessageIds UpdateTieredPriceDetails(PricePlanTierDetails^ dets);
		MessageIds MarkPriceDetailsasDeleted(String^ pricePlanId, String^ planType = L"std");

		GeneralStatus^ BuildStatus(char** rowData);

		Product^ BuildProduct(char** rowData);
		ProductFamily^ BuildProductFamilyDto(char** rowData);

		CustomerOrg^ BuildCustomerOrg(char** rowData);
		COrgCostCenter^ BuildCostCenter(char** rowData);
		CustomerContact^ BuildCustomerContact(char** rowData);
		Address^ BuildAddress(char** rowData);
		ProductAttribute^ BuildProductAttribute(char** argv);
		SalesOrder^ BuildOrder(char** rowData);
		SaleOrderDetails^ BuildOrderDetails(char** rowData);
		SalesOrderStatusHistory^ BuildOrderStatusHistory(char** rowData);
		SalesOrderPaymentHistory^ BuildOrderPaymentStatusHistory(char** rowData);

		ProductInventoryLocation^ BuildInventoryLocation(char** rowData);

		/// <summary> Removes all attributes and values which
		// which are lying orphaned, ie., not linked to a product or a family
		/// </summary>
		MessageIds DeleteUnusedAttributeAndValues();

	public:

		virtual ~scLocalDataService();

		static scLocalDataService* Instance();

		static void Cleanup();

		bool Connect(DependencyObject^ mainVm);

		bool Disconnect();

		SqliteDataManager& GetCache();

		bool StartTransaction();
		bool CommitTransaction();
		void CancelTransaction();

		AppProperty^ GetProperty(String^ name);
		IAsyncOperation<AppProperty^>^ GetPropertyAsync(String^ name);
		void SetProperty(AppProperty^ prop);
		task<void> SetPropertyAsync(AppProperty^ prop);

		String^ AutoGenerateCode(String^ tableName);

		IAsyncOperation<UserContext^>^ VerifySession();
		IAsyncOperation<UserContext^>^ Signup(String^ email, String^ pwd, String^ company, String^ fullName);
		IAsyncOperation<UserContext^>^ Signin(String^ email, String^ pwd);
		IAsyncOperation<bool>^ SignOut();
		UserContext^ GetCurrentUser();

		// Methods for roles
		IAsyncOperation<IVector<UserRole^>^>^ ListRoles();
		IAsyncOperation<bool>^ UpsertRole(UserRole^ role);
		IAsyncOperation<bool>^ RemRole(String^ roleName);

		task<MessageIds> ResetUserPassword(String^ userId);

		// Methods for users
		IAsyncOperation<IUserProfileList^>^ ListUsers();
		IAsyncOperation<bool>^ UpsertUser(UserProfile^ role, String^ password);
		IAsyncOperation<bool>^ RemUser(String^ emailAddress);
		IAsyncOperation<UserProfile^>^ GetUser(String^ userid);

		IAsyncOperation<IUserProfileList^>^ ListUsersByRoleAsync(String^ roleName);

		Address^ GetAddress(String^ addressId);
		IAsyncOperation<Address^>^ GetAddressAsync(String^ addressId);
		MessageIds UpdateAddress(Address^ address);
		task<MessageIds> UpdateAddressAsync(Address^ address);



		// parent if null, considered root category.
		IAsyncOperationWithProgress<IObservableVector<ProductCategory^>^, int>^ GetCategoryChildren(ProductCategory^ parent);

		ProductCategory^ GetCategory(CONDITIONS condition);
		IAsyncOperation<ProductCategory^>^ GetCategoryAsync(CONDITIONS condition);
		
		ProductCategory^ GetCategory(String^ id);

		/// <summary> Return a collection starting from the root category to the child identified by catId </summary>
		IAsyncOperation<IVectorView<ProductCategory^>^>^ GetCategoryRootToCurrentPath(String^ catId);

		//// Populate current category + plus its children products
		//IAsyncOperationWithProgress<ProductFamilyList^, int>^ GetProductFamiliesAsync();
		//IAsyncOperationWithProgress<ProductList^, int>^ GetProductsOfFamily(String^ productfamilyid);

		ProductList^ GetProducts();
		IAsyncOperationWithProgress<ProductList^, int>^ GetProductsAsync();
		/// <summary>
		/// Get products. 
		/// conditions included: 
		///		operator = [begins, like]
		///		productid = ?, productname = ?
		///		categoryid = ?, categoryname = ?, familyid = ?, familyname = ?, 
		///		attributeid = ?, attributename = ?, skun = ?
		///		pricelow = ?, pricehigh = ? (if both are given, range is applied, other greater or lesser is applied)
		/// </summary>
		ProductList^ GetProducts(const CONDITIONS condition);
		IAsyncOperationWithProgress<ProductList^, int>^ GetProductsAsync(const CONDITIONS condition);

		ProductList^ GetProducts(const string& sqlcondition);
		IAsyncOperation<ProductList^>^ GetProductsAsync(const string& sqlcondition);

		Product^ GetProduct(String^ productId);
		IAsyncOperation<Product^>^ GetProductAsync(String^ productId);

		task<MessageIds> SaveProductWithCategoriesAsync(Product^ obj, IVector<ProductCategory^>^ categories);
		task<MessageIds> SaveExistingProductAsync(Product^ obj);

		// Product Categtory methods.
		IAsyncOperation<IVector<ProductCategory^>^>^ GetProductCategoriesAsync(String^ productId);
		task<MessageIds> UpdateProductCategoriesAsync(String^ productId, IVector<ProductCategory^>^ categories);
		task<MessageIds> DeleteProductCategoriesAsync(String^ productId, IVector<ProductCategory^>^ categories);

		// Price plan methods.
		/// <summary>
		/// Get a list of all active price plans. If productid is provided, only the price plans associated
		/// with that product is listed.
		/// </summary>
		IVectorView<ProductPricePlan^>^ GetProductPricingPlans(String^ productId = nullptr);
		IAsyncOperation<IVectorView<ProductPricePlan^>^>^ GetProductPricingPlansAsync(String^ productId = nullptr);
		
		ProductPricePlan^ GetProductPricingPlan(String^ pricePlanId);
		IAsyncOperation<ProductPricePlan^>^ GetProductPricingPlanAsync(String^ pricePlanId);
		
		ProductTierPricePlans^ GetProductTierPricePlans(String^ pricePlanId);
		IAsyncOperation<ProductTierPricePlans^>^ GetProductTierPricePlansAsync(String^ pricePlanId);
		
		PricePlanStandardDetails^ GetProductStandardPricePlan(String^ pricePlanId);
		IAsyncOperation<PricePlanStandardDetails^>^ GetProductStandardPricePlanAsync(String^ pricePlanId);
		
		GenericPriceDetailsList^ GetPricePlanDetails(String^ pricePlanId);
		IAsyncOperation<GenericPriceDetailsList^>^ GetPricePlanDetailsAsync(String^ pricePlanId);

		task<MessageIds> SavePricePlan(ProductPricePlan^ pricePlan, PricePlanStandardDetails^ stdDetails);
		task<MessageIds> SavePricePlan(ProductPricePlan^ pricePlan, ProductTierPricePlans^ tierDetails);
		task<MessageIds> AssociatePricePlanWithProduct(String^ productId, String^ pricePlanId);

		/// <summary> Return all product families or the product family of the product.</summary>
		//[[deprecated("A product has only one family assignment")]]
		//IAsyncOperation<IVector<ProductFamily^>^>^ GetProductFamiliesAsync(String^ productId = nullptr);

		IVector<ProductFamily^>^ GetProductFamilies(const string& criteria);
		IAsyncOperation<IVector<ProductFamily^>^>^ GetProductFamiliesAsync(const string& criteria);

		/// <summary>Get a family instance </summary>
		/// <returns>Nullptr if not found</returns>
		ProductFamily^ GetProductFamily(String^ familyId);

		MessageIds UpdateProductFamilyAndAssociateWithProducts(ProductFamily^ productFamily, IVector<String^>^ associatedProducts = nullptr);
		task<MessageIds> UpdateProductFamilyAndAssociateWithProductsAsync(ProductFamily^ productFamily, IVector<String^>^ associatedProducts = nullptr);

		MessageIds DisassociateProductFromFamily(String^ productId);

		/// <summary> Update a product family and optionally associate  with a product </summary>
		MessageIds UpdateProductFamily(ProductFamily^ family, String^ productId = nullptr);
		task<MessageIds> UpdateProductFamilyAsync(ProductFamily^ family, String^ productId = nullptr);

		MessageIds LinkProductToFamily(String^ familyId, String^ productId);

		/// <summary> Delete Product family item. This call will cause the product family to be disassociated with the products </summary>.
		MessageIds DeleteProductFamily(String^ productFamilyId);
		task<MessageIds> DeleteProductFamilyAsync(String^ productFamilyid);

		IVector<Product^>^ GetProductsInFamily(String^ familyId);
		IAsyncOperation<IVector<Product^>^>^ GetProductsInFamilyAsync(String^ familyId);

		
		/// <summary>Get all attributes in the system </summary>
		IAsyncOperation<IVector<ProductAttribute^>^>^ GetAttributesAsync(String^ sqlwhere = nullptr, String^ sqlorder = nullptr);

		ProductAttribute^ GetProductAttribute(String^ sqlwhere);

		/// <summary>Get a list of product attributes. If conditions is empty, then all attributes are returned </summary>
		/// <param ref="conditions">
		/// Possible condition key-value pairs are :
		/// productid = <id>
		/// TODO
		/// </param>
		IAsyncOperation<IVector<ProductAttributeAndValue^>^>^ GetProductAttributesAsync(String^ productId);

		IAsyncOperation<ProductAttributeAndValue^>^ GetProductFamilyDefaultAttributeAsync(String^ familyId);

		ProductAttributeAndValue^ GetProductFamilyDefaultAttribute(String^ familyId);

		/// <summary>Returns distinct attribute values for an attribute.</summary>
		IAsyncOperation<IVector<String^>^>^ GetProductAttributeValuesDistinctAsync(String^ attributeId, String^ attrvaluePattern = nullptr);
		/// <summary>Returns a distinct list of UOMs entered with attribute values for a given attribute. </summary>
		IAsyncOperation<IVector<String^>^>^ GetAttributeValueDistinctUomsAsync(String^ attributeId, String^ uomPattern = nullptr);

		MessageIds UpdateProductAttribute(String^ productId, ProductAttribute^ obj, IProductAttributeValue^ value);
		task<MessageIds> UpdateProductAttributeAsync(String^ productId, ProductAttribute^ obj, IProductAttributeValue^ value);

		MessageIds UpdateDefaultProductAttribute(String^ productId, String^ attribId);
		task<MessageIds> UpdateDefaultProductAttributeAsync(String^ productId, String^ attribId);

		MessageIds UpdateProductFamilyDefaultAttribute(String^ familyId, ProductAttribute^ obj, IProductAttributeValue^ value);
		task<MessageIds> UpdateProductFamilyDefaultAttributeAsync(String^ familyId, ProductAttribute^ obj, IProductAttributeValue^ value);

		/// <summary>Returns MessageIds::IsUsed if attribute is assigned to a product </summary>
		task<MessageIds> DeleteProductAttributesAsync(String^ attributeId);
		/// <summary>Removes attribute value and its relation with a product.</summary>
		task<MessageIds> DeleteProductAttributeValueAsync(String^ attributeValueId);

		IAsyncOperationWithProgress<IVector<ProductImage^>^, int>^ GetProductImagesAsync(String^ productId);
		
		IAsyncOperation<ProductImage^>^ GetProductImageAsync(String^ imageId);

		IAsyncOperation<bool>^ LoadTitleImageIntoProductInstanceAsync(Product^ product);
		
		task<MessageIds> SaveProductImageAsync(ProductImage^ image);

		task<MessageIds> DeleteProductImageAsync(String^ imageId);

		// product inventory methods
		IMap<String^, ProductInventory^>^ GetProductsInventories();
		ProductInventory^ GetProductInventory(String^ productId);
		/// <summary> Update product's inventory. </summary>
		/// <param ref="movementType">The numeric identifier representing a value from "inventorymovementtypes" list. </param>
		/// <param ref="productId">Product Identifier </param>
		/// <param ref="location">A value from inventory locations </param>
		/// <param ref="qty">New quantity adjustment</param>
		/// <remarks>
		MessageIds UpdateInventory(int movementType, String^ productId, String^ location, uint64 qty);


		IVector<CustomerOrg^>^ GetCustomerOrganizations(String^ sqlcondition = nullptr);
		IAsyncOperation<IVector<CustomerOrg^>^>^ GetCustomerOrganizationsAsync(String^ sqlcondition = nullptr);
		CustomerOrg^ GetCustomerOrganization(String^ ccid);
		IAsyncOperation<CustomerOrg^>^ GetCustomerOrganizationAsync(String^ ccid);
		MessageIds UpdateCustomerOrganization(CustomerOrg^ org);
		task<MessageIds> UpdateCustomerOrganizationAsync(CustomerOrg^ org);
		MessageIds DeleteCustomerOrganization(String^ orgId);
		task<MessageIds> DeleteCustomerOrganizationAsync(String^ orgId);

		CustomerOrg^ GetCustomerOrgOfCostCenter(String^ costcenterId);
		IVector<COrgCostCenter^>^ GetCustomerCostCenters(String^ sqlcondition = nullptr);
		IAsyncOperation<IVector<COrgCostCenter^>^>^ GetCustomerCostCentersAsync(String^ sqlcondition = nullptr);
		COrgCostCenter^ GetCustomerCostCenter(String^ ccid);
		IAsyncOperation<COrgCostCenter^>^ GetCustomerCostCenterAsync(String^ ccid);
		MessageIds UpdateCustomerCostCenter(COrgCostCenter^ org);
		task<MessageIds> UpdateCustomerCostCenterAsync(COrgCostCenter^ org);
		MessageIds DeleteCustomerCostCenter(String^ orgId);
		task<MessageIds> DeleteCustomerCostCenterAsync(String^ orgId);

		IVector<CustomerContact^>^ GetCustomerContacts(String^ sqlcondition = nullptr);
		IAsyncOperation<IVector<CustomerContact^>^>^ GetCustomerContactsAsync(String^ sqlcondition = nullptr);
		CustomerContact^ GetCustomerContact(String^ ccid);
		IAsyncOperation<CustomerContact^>^ GetCustomerContactAsync(String^ ccid);
		MessageIds UpdateCustomerContact(String^ costCenterId, CustomerContact^ contact);
		task<MessageIds> UpdateCustomerContactAsync(String^ costCenterId, CustomerContact^ contact);
		MessageIds DeleteCustomerContact(String^ costCenterId, String^ contactId);
		task<MessageIds> DeleteCustomerContactAsync(String^ costCenterId, String^ contactId);

		IMap<String^, Address^>^ GetCostCenterAddresses(String^ costCenterId);
		IAsyncOperation<IMap<String^, Address^>^>^ GetCostCenterAddressesAsync(String^ costCenterId);
		Address^ GetCostCenterAddress(String^ costCenterId, String^ addressType);
		IAsyncOperation<Address^>^ GetCostCenterAddressAsync(String^ costCenterId, String^ addressType);
		MessageIds UpdateCostCenterAddress(String^ costCenterId, String^ addressType, Address^ address);
		task<MessageIds> UpdateCostCenterAddressAsync(String^ costCenterId, String^ addressType, Address^ address);
		MessageIds DeleteCostCenterAddress(String^ costCenterId, String^ addressId);
		task<MessageIds> DeleteCostCenterAddressAsync(String^ costCenterId, String^ addressId);

		IVector<FieldRep^>^ GetFieldReps(String^ sqlcondition = nullptr);
		IAsyncOperation<IVector<FieldRep^>^>^ GetFieldRepsAsync(String^ sqlcondition = nullptr);
		FieldRep^ GetFieldRep(String^ ccid);
		IAsyncOperation<FieldRep^>^ GetFieldRepAsync(String^ ccid);
		MessageIds UpdateFieldRep(FieldRep^ org);
		task<MessageIds> UpdateFieldRepAsync(FieldRep^ org);
		MessageIds DeleteFieldRep(String^ orgId);
		task<MessageIds> DeleteFieldRepAsync(String^ orgId);

		/// <summary>
		/// Get a status type structured based on the type <see ref="GeneralStatus"/>
		/// The statustype parameter is the table name used for that type (ORDERSTATUS, PAYMENTSTATUS, PAYMENTINSTRUMENT, etc.).
		/// </summary>
		IVector<GeneralStatus^>^ GetGeneralStatus(String^ statusType);
		IAsyncOperation<IVector<GeneralStatus^>^>^ GetGeneralStatusAsync(String^ statusType);

		GeneralStatus^ GetGeneralStatus(String^ statusType, int statusId);

		IVector<SalesOrder^>^ GetSalesOrders(const char* sqlcondition = nullptr, const char* orderby = nullptr);
		IAsyncOperation<IVector<SalesOrder^>^>^ GetSalesOrdersAsync(const char* sqlcondition = nullptr, const char* orderby = nullptr);

		SalesOrder^ GetSalesOrder(String^ orderId);
		IAsyncOperation<SalesOrder^>^ GetSalesOrderAsync(String^ orderId);

		IVector<SaleOrderDetails^>^ GetSalesOrderDetailList(String^ orderId);
		IAsyncOperation<IVector<SaleOrderDetails^>^>^ GetSalesOrderDetailListAsync(String^ orderId);

		IVector<SalesOrderStatusHistory^>^ GetOrderStatusHistory(String^ orderId);
		IAsyncOperation<IVector<SalesOrderStatusHistory^>^>^ GetOrderStatusHistoryAsync(String^ orderId);

		IVector<SalesOrderPaymentHistory^>^ GetOrderPaymentHistory(String^ orderId);
		IAsyncOperation<IVector<SalesOrderPaymentHistory^>^>^ GetOrderPaymentHistoryAsync(String^ orderId);

		MessageIds SaveSalesOrder(
			SalesOrder^ order, 
			IVector<SaleOrderDetails^>^ orderDets, 
			SalesOrderStatusHistory^ currentStatus,
			SalesOrderPaymentHistory^ currentPaymentStatus);

		task<MessageIds> SaveSalesOrderAsync(
			SalesOrder^ order, 
			IVector<SaleOrderDetails^>^ orderDets, 
			SalesOrderStatusHistory^ currentStatus,
			SalesOrderPaymentHistory^ currentPaymentStatus);

		SalesOrderStatusHistory^ GetLatestOrderStatus(String^ orderId);

		SalesOrderPaymentHistory^ GetLatestOrderPaymentStatus(String^ orderId);

		IObservableVector<ProductInventoryLocation^>^ GetInventoryLocations();

		IAsyncOperation<IObservableVector<ProductInventoryLocation^>^>^ GetInventoryLocationsAsync();

		ProductInventoryLocation^ GetInventoryLocation(String^ locationId);

		IAsyncOperation<ProductInventoryLocation^>^ GetInventoryLocationAsync(String^ locationId);

		MessageIds SaveInventoryLocation(ProductInventoryLocation^ location);

		MessageIds UpdateProductInventory(String^ productId, uint64 quantity, String^ locationId, InventoryMoveDirection movementType);

		/// <summary>
		/// Returns the details of the business that owns the application.
		/// </summary>
		/// <returns>
		/// If not already saved before, returns an empty instance. Use that instance
		/// to populate data and save it by calling <see cref="SaveAppBusinessOrg"/> method.
		/// </returns>
		AppBusinessOrg^ GetAppBusinessOrg();
		
		IAsyncOperation<AppBusinessOrg^>^ GetAppBusinessOrgAsync();
		
		MessageIds SaveAppBusinessOrg(AppBusinessOrg^ org);

		task<MessageIds> SaveAppBusinessOrgAsync(AppBusinessOrg^ org);

#ifdef DEBUG
		void PrintDbState();
#endif


	};

#define LOCSVC scwin::scLocalDataService::Instance()

}