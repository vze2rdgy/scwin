#pragma once

[Bindable]
ref class NotifierObject abstract : public INotifyPropertyChanged
{
public:
	// Inherited via INotifyPropertyChanged
	virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

protected private:
	NotifierObject();

public:
	virtual ~NotifierObject();

protected private:
	virtual void NotifyPropertyChanged(PropertyChangedEventArgs^ args);

	template<typename T>
	bool SetProperty(String^ propertyName, T& localVar, T newVal)
	{
		if (localVar != newVal)
		{
			localVar = newVal;
			NotifyPropertyChanged(ref new PropertyChangedEventArgs(propertyName));
			return true;
		}
		return false;
	}

};

