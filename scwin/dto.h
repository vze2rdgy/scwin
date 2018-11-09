#pragma once


namespace scwin
{

#ifndef DECLARERAISEPROPCHANGED 
	#define DECLARERAISEPROPCHANGED void RaisePropertyChanged(String^ propName)
#endif


#define PROPERTYNAME(prop) #prop

	// Datastate flag used in sqlite tables. See the state field in all tables.
	//IMPORTANT: DO NOT change the order of the flag values in this enum.
	public enum class DataState
	{
		Unmodified = 0,
		Added = 1,
		Edited = 2,
		Deleted = 3,	// ENSURE THAT Deleted's value is always above the values of unmodified, added and edited since all sqls are written assuming that condition
		Inactive = 4,
		Error = 5,
		Viewing = 6 // used for gui purpose.
	};


	using namespace Windows::UI::Xaml::Data;

	public interface struct IDTOBase : INotifyPropertyChanged
	{

		property bool CanRaiseEvents
		{
			bool get();
			void set(bool val);
		}

		property DataState ObjectState
		{
			DataState get();
			void set(DataState val);
		}

		property bool IsEmpty
		{
			bool get();
		}

		property bool IsValid
		{
			bool get();
		}
	};
}

