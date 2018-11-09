#pragma once

namespace scwin
{
	[Bindable]
	public ref class StringList sealed : public IVector<String^>
	{
	private:
		Vector<String^>^ v;

	public:
		StringList();

		// Inherited via IVector
		virtual Windows::Foundation::Collections::IIterator<Platform::String ^> ^ First();
		virtual property unsigned int Size
		{
			UINT get() { return v->Size; }
		}
		virtual Platform::String ^ GetAt(unsigned int index);
		virtual Windows::Foundation::Collections::IVectorView<Platform::String ^> ^ GetView();
		virtual bool IndexOf(Platform::String ^value, unsigned int *index);
		virtual void SetAt(unsigned int index, Platform::String ^value);
		virtual void InsertAt(unsigned int index, Platform::String ^value);
		virtual void RemoveAt(unsigned int index);
		virtual void Append(Platform::String ^value);
		virtual void RemoveAtEnd();
		virtual void Clear();
		virtual unsigned int GetMany(unsigned int startIndex, Platform::WriteOnlyArray<Platform::String ^, 1U> ^items);
		virtual void ReplaceAll(const Platform::Array<Platform::String ^, 1U> ^items);
	};

}