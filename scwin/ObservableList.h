#pragma once

#include <collection.h>
#include <unordered_map>

using namespace ::Platform::Collections;
using namespace Windows::Foundation::Collections;

namespace scwin
{

	ref class VectorChangedEventArgs sealed : IVectorChangedEventArgs
	{
	public:
		VectorChangedEventArgs()
		{

		}

		VectorChangedEventArgs(CollectionChange change, int index = -1)
		{
			this->CollectionChange = change;
			this->Index = index;
		}

		// Inherited via IVectorChangedEventArgs
		virtual property Windows::Foundation::Collections::CollectionChange CollectionChange;

		virtual property unsigned int Index;
	};

	typedef std::unordered_map<std::string, int> ObjectIdMap;

	template<typename T>
	ref class ObservableList sealed : IObservableVector<T>
	{
	private:
		Vector<T>^ list = ref new Vector<T>();

	public:
		ObservableList() {}

		// Inherited via IObservableVector
		virtual property unsigned int Size
		{
			UINT get()
			{
				return this->list->Size;
			}
		}

		virtual IIterator<T>^ First()
		{
			return list->First();
		}

		virtual T GetAt(unsigned int index)
		{
			return list->GetAt(index);
		}

		virtual Windows::Foundation::Collections::IVectorView<T> ^ GetView()
		{
			return list->GetView();
		}
		virtual bool IndexOf(T value, unsigned int *index)
		{
			return list->IndexOf(value, index);
		}
		virtual void SetAt(unsigned int index, T value)
		{
			list->SetAt(index, value);
		}
		virtual void InsertAt(unsigned int index, T value)
		{
			list->InsertAt(index, value);
		}
		virtual void RemoveAt(unsigned int index)
		{
			list->RemoveAt(index);
		}
		virtual void Append(T value)
		{
			list->Append(value);
		}
		virtual void RemoveAtEnd()
		{
			list->RemoveAtEnd();
		}
		virtual void Clear()
		{
			list->Clear();
		}
		virtual unsigned int GetMany(unsigned int startIndex, ::Platform::WriteOnlyArray<T, 1U> ^items)
		{
			return list->GetMany(startIndex, items);
		}
		virtual void ReplaceAll(const ::Platform::Array<T, 1U> ^items)
		{
			list->ReplaceAll(items);
		}

		virtual event Windows::Foundation::Collections::VectorChangedEventHandler<T> ^ VectorChanged;

	private:
		void InvokeChanged(CollectionChange change, int index = -1)
		{
			VectorChangedEventArgs^ e = ref new VectorChangedEventArgs(change, index);
			VectorChanged(this, e);
		}
	};

	template<typename T>
	ref class ObservableMappedList sealed : IObservableVector<T>
	{
	private:
		ObjectIdMap idMap;
		Vector<T>^ list = ref new Vector<T>();

	public:
		ObservableMappedList() {}

		// Inherited via IObservableVector
		virtual property unsigned int Size
		{
			UINT get()
			{
				return this->list->Size;
			}
		}

		virtual IIterator<T>^ First()
		{
			return list->First();
		}

		virtual T GetAt(unsigned int index)
		{
			return list->GetAt(index);
		}

		virtual Windows::Foundation::Collections::IVectorView<T> ^ GetView()
		{
			return list->GetView();
		}
		virtual bool IndexOf(T value, unsigned int *index)
		{
			return list->IndexOf(value, index);
		}
		virtual void SetAt(unsigned int index, T value)
		{
			list->SetAt(index, value);
			idMap.insert(value->Id, index);
		}
		virtual void InsertAt(unsigned int index, T value)
		{
			list->InsertAt(index, value);
			idMap.insert(value->Id, index);
		}

		virtual void RemoveAt(unsigned int index)
		{
			if (index >= 0 && list->Size > index)
			{
				auto id = list[index]->Id;
				idMap.erase(id);
				list->RemoveAt(index);
			}
		}

		virtual void Append(T value)
		{
			if (idMap.find(value->Id) != idMap.end())
			{
				list->Append(value);
				idMap.insert(value->Id, list->Size - 1);
			}
		}
		virtual void RemoveAtEnd()
		{
			if (list->Size)
			{
				auto lastItem = list[list->Size - 1];
				idMap.erase(lastItem->Id);
				list->RemoveAtEnd();
			}
		}
		virtual void Clear()
		{
			list->Clear();
			idMap.clear();
		}

		virtual unsigned int GetMany(unsigned int startIndex, ::Platform::WriteOnlyArray<T, 1U> ^items)
		{
			return list->GetMany(startIndex, items);
		}

		virtual void ReplaceAll(const ::Platform::Array<T, 1U> ^items)
		{
			list->ReplaceAll(items);
			//TODO idMap also need to be updated.
		}

		virtual event Windows::Foundation::Collections::VectorChangedEventHandler<T> ^ VectorChanged;

	private:
		void InvokeChanged(CollectionChange change, int index = -1)
		{
			VectorChangedEventArgs^ e = ref new VectorChangedEventArgs(change, index);
			VectorChanged(this, e);
		}
	};

}

