#pragma once

namespace scwin
{

	class Enumerable
	{
	public:
		template<typename T>
		static int FindIndex(IVector<T>^ vector, const T& item)
		{
			for (int i = 0; i < vector->Size; i++)
			{
				if (item == vector->GetAt(i))
				{
					return i;
				}
			}
			return -1;
		}

		template<typename T>
		static int FindIndex(IVector<T>^ vector, std::function<bool(const T&)> predicate)
		{
			for (int i = 0; i < vector->Size; i++)
			{
				if (predicate(vector->GetAt(i)))
				{
					return i;
				}
			}
			return -1;
		}
	};

}
