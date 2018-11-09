#pragma once

using namespace Platform;
using namespace Windows::UI::Xaml::Data;

namespace Sg
{
	[Windows::Foundation::Metadata::WebHostHidden]
	ref class BindableBase abstract : public INotifyPropertyChanged
	{
	public:
		virtual event PropertyChangedEventHandler^ PropertyChanged;

	protected private:
		BindableBase();

	protected private:
		template<typename T>
		bool SetProperty(T& storage, T value, String^ propertyName);

		void OnPropertyChanged(String^ propertyName);
	};

	template<typename T>
	bool BindableBase::SetProperty(T& storage, T value, String^ propertyName)
	{
		if (::Platform::Object::ReferenceEquals(storage, value))
		{
			return false;
		}
		storage = value;
		OnPropertyChanged(propertyName);
		return true;
	}
}

