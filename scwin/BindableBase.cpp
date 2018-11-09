#include "pch.h"
#include "BindableBase.h"

using namespace Sg;


BindableBase::BindableBase()
{

}

void Sg::BindableBase::OnPropertyChanged(String ^ propertyName)
{
	PropertyChanged(this, ref new PropertyChangedEventArgs(propertyName));
}
