#include "pch.h"
#include "UserContext.h"

using namespace scwin;
using namespace Platform::Collections;


UserContext::UserContext() : userRights(FunctionalRights::NoRights)
{
}

bool scwin::UserContext::HasRight(FunctionalRights requestedRight)
{
	return requestedRight == ( userRights & (uint64)requestedRight );
}

void scwin::UserContext::SetRight(FunctionalRights newRight)
{
	userRights |= (uint64)newRight;
}

void scwin::UserContext::RemoveRight(FunctionalRights oldRight)
{
	userRights &= ~oldRight;
}

void scwin::UserContext::ClearRight()
{
	userRights = FunctionalRights::NoRights;
}

void UserContext::RaisePropertyChanged(String ^ propertyName)
{
	auto e = ref new PropertyChangedEventArgs(propertyName);
	this->PropertyChanged(this, e);
}

String^ UserContext::Name::get()
{
	return name;
}

void UserContext::Name::set(String^ val)
{
	name = val;
}

IVector<UserRole^>^ UserContext::Roles::get()
{
	if (roles == nullptr)
	{
		roles = ref new Vector<UserRole^>();
	}
	return roles;
}

void UserContext::Roles::set(IVector<UserRole^>^ val)
{
	roles = val;
}

String^ UserContext::FullName::get()
{
	return fullName;
}

void UserContext::FullName::set(String^ val)
{
	fullName = val;
	RaisePropertyChanged("FullName");
}

String^ UserContext::Email::get()
{
	return emailAddress;
}

void UserContext::Email::set(String^ val)
{
	emailAddress = val;
	RaisePropertyChanged("Email");
}

BitmapImage^ UserContext::ProfileImage::get()
{
	return profImg;
}

void UserContext::ProfileImage::set(BitmapImage^ val)
{
	profImg = val;
	RaisePropertyChanged("ProfileImage");
}



