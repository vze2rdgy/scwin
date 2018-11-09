#include "pch.h"
#include "Picture.h"


Picture::Picture(String^ bytes)
{
}

String^ Picture::EncodedBytes::get()
{
	throw ref new NotImplementedException(); // TODO;
}


//////////////////////// PictureCache ///////////////////////////

PictureCache^ PictureCache::_piccache = nullptr;

PictureCache::PictureCache()
{

}

PictureCache^ PictureCache::Instance::get()
{
	if (_piccache == nullptr)
	{
		std::lock_guard<std::recursive_mutex> g(GlobalMutex);
		if (_piccache == nullptr)
		{
			_piccache = ref new PictureCache();
		}
	}
	return _piccache;
}


Picture ^ scwin::PictureCache::RequestPicture(String ^ key, String ^ subkey)
{
	throw ref new Platform::NotImplementedException();
	// TODO: insert return statement here
}

IAsyncOperation<Picture^>^ scwin::PictureCache::RequestPictureAsync(String ^ key, String ^ subkey)
{
	throw ref new Platform::NotImplementedException();
	// TODO: insert return statement here
}

