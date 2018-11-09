#pragma once

namespace scwin
{
	public ref class ProductInventoryTransaction sealed : public IDTOBase
	{
	public:
		ProductInventoryTransaction();

		// Inherited via IDTOBase
		virtual property bool CanRaiseEvents;
		virtual property DataState ObjectState;
		virtual property bool IsEmpty {bool get() { return false; }}
		virtual property bool IsValid {bool get() { return true; }}
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

	private:
		DECLARERAISEPROPCHANGED;
	};

}
