#pragma once

using namespace Windows::Storage;

namespace scwin
{

	class ImageHelpers
	{
	public:
		static task<BitmapImage^> ConvertToImage(const Array<byte>^ imageBytes);
		static task<String^> EncodeImageFileToBase64String(StorageFile^ file);
	};

}