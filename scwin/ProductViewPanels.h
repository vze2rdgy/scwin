#pragma once

namespace scwin
{
	[Bindable]
	public ref class ProductViewPanel sealed :
		public INotifyPropertyChanged
	{
		bool isEdited = false;
		bool isExpanded = false;

	private:
		void InvokePropertyChanged(String^ propName);

	public:
		ProductViewPanel();

		property bool IsEdited
		{
			bool get() { return isEdited; }
			void set(bool val)
			{
				if (isEdited != val)
				{
					isEdited = val;
					InvokePropertyChanged("IsEdited");
				}
			}
		}
		property bool IsExpanded
		{
			bool get() { return isExpanded; }
			void set(bool val)
			{
				if (isExpanded != val)
				{
					isExpanded = val;
					InvokePropertyChanged("IsExpanded");
				}
			}
		}

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;
	};


}
