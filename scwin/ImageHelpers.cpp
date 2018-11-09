#include "pch.h"
#include "ImageHelpers.h"

using namespace Windows::Security::Cryptography;
using namespace Windows::Storage::Streams;

namespace scwin
{

	task<BitmapImage^> ImageHelpers::ConvertToImage(const Array<byte>^ imageBytes)
	{
		return create_task([imageBytes]()
		{
			auto image = ref new BitmapImage();
			auto strm = ref new InMemoryRandomAccessStream();
			auto dw = ref new DataWriter(strm->GetOutputStreamAt(0));
			dw->WriteBytes(imageBytes);
			dw->StoreAsync()->GetResults();
			image->SetSourceAsync(strm);
			delete dw;
			delete strm;
			return image;
		});
	}
	task<String^> ImageHelpers::EncodeImageFileToBase64String(StorageFile ^ file)
	{
		return create_task([file]() -> String^
		{
			if (!file)
			{
				return nullptr;
			}
			auto stream = create_task(file->OpenAsync(FileAccessMode::Read)).get();
			auto buff = ref new Buffer(stream->Size);
			auto loadedBuffer = create_task(stream->ReadAsync(buff, stream->Size, InputStreamOptions::None)).get();
			auto data = CryptographicBuffer::EncodeToBase64String(buff);
			return data;
		});
	}
}