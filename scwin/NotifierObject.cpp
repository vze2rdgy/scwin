#include "pch.h"
#include "NotifierObject.h"


NotifierObject::NotifierObject()
{
}


NotifierObject::~NotifierObject()
{

}

void NotifierObject::NotifyPropertyChanged(PropertyChangedEventArgs ^ args)
{
	PropertyChanged(this, args);
}

