#pragma once

#include <collection.h>

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;

namespace scwin
{
	template<typename K, typename V>
	ref class ObservableDictionary sealed : public IObservableMap<K, V>
	{
		ref class ObservableDictionaryChangedEventArgs sealed : public IMapChangedEventArgs<K>
		{
		private:
			Windows::Foundation::Collections::CollectionChange change;
			K key;

		public:
			ObservableDictionaryChangedEventArgs(CollectionChange change, K key)
			{
				this->key = key;
				this->change = change;
			}
			
			property Windows::Foundation::Collections::CollectionChange CollectionChange
			{
			public:
				Windows::Foundation::Collections::CollectionChange get()
				{
					return change;
				}
			}

			property K Key
			{
			public:
				K get() { return key; }
			}
		};

	private:
		Map<K, V>^ dict = ref new Map<K, V>();

	private:
		void InvokeMapChanged(Windows::Foundation::Collections::CollectionChange change, K key)
		{
			MapChanged(this, ref new ObservableDictionaryChangedEventArgs(change, key));
		}

	public:

		virtual event MapChangedEventHandler<K, V>^ MapChanged;

		ObservableDictionary()
		{

		}

		property unsigned int Size
		{
			virtual UINT get()
			{
				return this->Size;
			}
		}

		virtual IMapView<K, V>^ GetView()
		{
			return dict->GetView();
		}

		virtual bool HasKey(K key)
		{
			return dict->HasKey(key);
		}

		virtual bool Insert(K key, V val)
		{
			return dict->Insert(key, val);
		}

		void Add(K key, V value) 
		{
			dict->Insert(key, val);
		}

		virtual void Remove(K key) 
		{
			if (dict->HasKey(key))
			{
				dict->Remove(key);
			}
		}

		virtual V Lookup(K key)
		{
			if (dict->HasKey(key))
			{
				return dict->Lookup(key);
			}
			return nullptr;
		}

		virtual void Clear()
		{
			dict->Clear();

		}
		
		virtual IIterator<IKeyValuePair<K, V>^>^ First()
		{
			return dict->First();
		}
	};
}