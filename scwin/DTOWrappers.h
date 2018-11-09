#pragma once

namespace scwin
{

	public ref class ProductSelectable sealed : INotifyPropertyChanged
	{
		Product^ product;
		bool isSelected = false;
	public:
		ProductSelectable(Product^ product, bool isSelected);

		property Product^ Item
		{
			Product^ get() { return product; }
		}

		property bool IsSelected
		{
			bool get() { return isSelected; }
			void set(bool val)
			{
				if (isSelected != val)
				{
					isSelected = val;
					RaisePropertyChanged("IsSelected");
				}
			}
		}

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

	private:
		void RaisePropertyChanged(String^ propName);
	};

}