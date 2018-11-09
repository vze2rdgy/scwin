#pragma once

#include <memory>

namespace scwin
{

	template<typename T>
	ref class NativeTypeWrapper sealed
	{
	private:
		std::shared_ptr<T> pNative;

	internal:
		NativeTypeWrapper(T* nativePtr)
		{
			pNative = std::shared_ptr<T>(nativePtr);
		}

		T* get()
		{
			return this->pNative.get();
		}
	};

}