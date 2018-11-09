#pragma once

namespace scwin
{
	
	public ref class Picture sealed
	{
	public:
		Picture(String^ bytes);

		property String^ EncodedBytes
		{
			String^ get();
		}
	};

	/// Returns a cached list of images. The cache expires
	/// after certain period of time unless another request
	/// is made before expiration.
	public ref class PictureCache sealed
	{
		// EXPLORE Windows API File Mapping to conserve physical memory.
		typedef std::map<tuple<String^, String^>, Picture^> PICTURECACHE;
		static PictureCache^ _piccache;

	private:
		PictureCache();

	public:
		static property PictureCache^ Instance
		{
			PictureCache^ get();
		}

		Picture^ RequestPicture(String^ key, String^ subkey);

		IAsyncOperation<Picture^>^ RequestPictureAsync(String^ key, String^ subkey);


	};

}