#pragma once

using namespace ::Platform;

namespace scwin
{

	ref class Notifier : public Windows::UI::Xaml::Data::INotifyPropertyChanged
	{
	public:
		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;
	
	protected private:

		virtual void RaisePropertyChanged(String^ propertyName)
		{
			Windows::UI::Xaml::Data::PropertyChangedEventArgs^ e =
				ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs(propertyName);
			PropertyChanged(this, e);
		}
	};
}