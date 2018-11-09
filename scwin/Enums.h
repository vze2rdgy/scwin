#pragma once

namespace scwin
{

	/*********************Enum that defines exceptions***************/
	enum struct MessageIds : int
	{
		Success = 0,
		SuccessWithCaveat = 1,
		InternetNotAvailable = 2,
		SessionVerificationError = 3,
		SigningUpFailed = 4,
		SigningInFailed = 5,
		DBClearingFailed = 6,
		NoUserProfileSelected = 7,
		EmailAddressNotValid = 8,
		PasswordNotValid = 9,
		UserNameNotProvided = 10,
		SaveFailed = 11,
		PasswordsDoNotMatch = 12,
		ReplicationIncomingDataError = 13,
		ReplicationOutgoingDataError = 14,
		DBQueryFatalError = 15,
		IncorrectProductAttributeType = 16,
		MissingValuesError = 17,
		InvalidArgumentError = 18,
		OutOfRangeArgumentError = 19,
		DBTransactionBeginFailed = 20,
		DBTransactionCommitFailed = 21,
		DBUpdateFailed = 22,
		NotImplemented = 23,
		DuplicateId = 24,
		IsUsed = 25,
		UniqueConstraintFailed = 26,
		MissingPropertyError = 27,
		WebApiMethodFailed = 28,
	};

#define SUCCESS(msgid) msgid == MessageIds::Success
#define HASFAILED(msgid) msgid !=  MessageIds::Success

	public enum class LandingContentStage
	{
		SignIn,
		SignUp,
		Wait,
		Dashboard
	};

	public enum class ListViewTypes
	{
		Tiles = 0,
		Details = 1
	};

	public enum class UIEditMode
	{
		Unmodified,
		Add,
		Edit,
		Delete,
		Save,
		Cancel
	};

	public enum class VisualStateTriggers
	{
		NoDataState,
		ReadState,
		WriteState
	};

	/// <summary>
	/// Orientations
	/// </summary>
	public enum class Orientations
	{
		/// <summary>
		/// none
		/// </summary>
		None,
		/// <summary>
		/// landscape
		/// </summary>
		Landscape,
		/// <summary>
		/// portrait
		/// </summary>
		Portrait
	};

	/// <summary>
	/// Application status message types.
	/// </summary>
	public enum class StatusMessageTypes
	{
		Information,
		Warning,
		Error
	};

	public enum class InventoryMoveDirection
	{
		In = 1,
		Out = 2
	};

	public enum class DiscountTypes : int
	{
		Unknown = (int)'0',
		Percent = (int)'%',
		Amount = (int)'#'
	};


	/// <summary>
	/// This enum type is used in ChooseAProduct control.
	/// </summary>
	public enum class ProductChooseOptions
	{
		/// <summary> Display Image and Product Name </summary>
		Basic,
		/// <summary> Display Image, Product Name and product price </summary>
		ProductInfoWithPrice,
		/// <summary> Display Image, Product Name, Product Price and Inventory </summary>
		ProductInfoWithPriceAndInventory
	};

	constexpr size_t FunctionalRightsLen = 1;

	enum FunctionalRights : uint64
	{
		NoRights = 0,

		ViewProducts = 1,
		EditProducts = ViewProducts << 1,
		AddProducts = EditProducts << 1,
		DeleteProducts = AddProducts << 1,


		ViewCustomers = DeleteProducts << 1,
		EditCustomers = ViewCustomers << 1,
		AddCustomers = EditCustomers << 1,
		DeleteCustomers = AddCustomers << 1,

		ViewOrders = DeleteCustomers << 1,
		EditOrders = ViewOrders << 1,

		Customer = ViewProducts | ViewOrders,

		//FieldRep = 

		AllRights = 
			ViewProducts | EditProducts | AddProducts | DeleteProducts |
			ViewCustomers | EditCustomers | AddCustomers | DeleteCustomers


	};

	struct FunctionalRightDesc
	{
		std::string Name;
		std::string Description;
	};

	typedef std::map<uint64, FunctionalRightDesc> FunctionalRightsMap;

	extern FunctionalRightsMap FunctionalRightsDescriptions;

	/// <summary>
	/// Device families recogonized by this product.
	/// </summary>
	public enum class DeviceFamily
	{
		Unknown = 0,
		Desktop = 1,
		//Tablet = 2,
		//Kiosk = 2,
		Phone = 3,
		//Notebook = 1,
		//Convertible = 1,
		//Detachable = 4,
		//AllInOne = 1,
		//StickPC = 2, // for now xbox and stockpc behaves like tablet
		XBox = 2,
		//Puck = 1,
	};

	public enum class GridOrListViewViewType
	{
		List,
		Grid
	};

}

