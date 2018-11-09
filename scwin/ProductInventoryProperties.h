#pragma once
namespace scwin
{

	public ref class ProductInventoryProperties sealed : public IDTOBase
	{
		String^ id;
		GeneralStatus^ invfilltype;

	public:
		ProductInventoryProperties();
		ProductInventoryProperties(String^ id);

		property String^ Id
		{
			String^ get() { return id; }
		}

		property GeneralStatus^ FillType
		{
			GeneralStatus^ get() 
			{ 
				return invfilltype; 
			}
			void set(GeneralStatus^ val)
			{
				if (invfilltype != val)
				{
					invfilltype = val;
					RaisePropertyChanged(PROPERTYNAME(FillType));
				}
			}
		}

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