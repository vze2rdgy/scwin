#include "pch.h"
#include "StringList.h"


StringList::StringList() : v(ref new Vector<String^>())
{

}

Windows::Foundation::Collections::IIterator<Platform::String ^> ^ scwin::StringList::First()
{
	return v->First();
}

Platform::String ^ scwin::StringList::GetAt(unsigned int index)
{
	return v->GetAt(index);
}

Windows::Foundation::Collections::IVectorView<Platform::String ^> ^ scwin::StringList::GetView()
{
	return v->GetView();
}

bool scwin::StringList::IndexOf(Platform::String ^value, unsigned int *index)
{
	return v->IndexOf(value, index);
}

void scwin::StringList::SetAt(unsigned int index, Platform::String ^value)
{
	v->SetAt(index, value);
}

void scwin::StringList::InsertAt(unsigned int index, Platform::String ^value)
{
	v->InsertAt(index, value);
}

void scwin::StringList::RemoveAt(unsigned int index)
{
	v->RemoveAt(index);
}

void scwin::StringList::Append(Platform::String ^value)
{
	v->Append(value);
}

void scwin::StringList::RemoveAtEnd()
{
	v->RemoveAtEnd();
}

void scwin::StringList::Clear()
{
	v->Clear();
}

unsigned int scwin::StringList::GetMany(unsigned int startIndex, Platform::WriteOnlyArray<Platform::String ^, 1U> ^items)
{
	return v->GetMany(startIndex, items);
}

void scwin::StringList::ReplaceAll(const Platform::Array<Platform::String ^, 1U> ^items)
{
	v->ReplaceAll(items);
}
