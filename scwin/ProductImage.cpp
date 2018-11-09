#include "pch.h"
#include "ProductImage.h"

using namespace Windows::Graphics::Imaging;

namespace scwin
{
	ProductImage::ProductImage()
	{
		id = SysInfoHelpers::NewCouchDbId();
		ObjectState = DataState::Added;
		CanRaiseEvents = true;
	}

	ProductImage::ProductImage(String ^ id)
	{
		this->id = id;
		ObjectState = DataState::Unmodified;
		CanRaiseEvents = true;
	}



	void ProductImage::BeginDataToImageConversion()
	{
		WeakReference wr(this);
		create_task(BeginConvertFromDataToImage(this->Data))
			.then([wr](SoftwareBitmap^ bmp)
		{
			auto ths = wr.Resolve<ProductImage>();
			if (ths != nullptr && bmp != nullptr)
			{
				AppDispatcher->RunAsync(
					CoreDispatcherPriority::High,
					ref new DispatchedHandler([wr, bmp]()
				{
					auto ths = wr.Resolve<ProductImage>();
					if (ths != nullptr)
					{
						auto img = ref new SoftwareBitmapSource();
						create_task(img->SetBitmapAsync(bmp)).then([wr, img]()
						{
							auto ths = wr.Resolve<ProductImage>();
							if (ths != nullptr)
								ths->Image = img;
						});
					}
				}));
			}
		});
	}

	IAsyncOperationWithProgress<SoftwareBitmap^, int>^ ProductImage::BeginConvertFromDataToImage(String^ data)
	{
		using namespace Windows::Security::Cryptography;
		using namespace Windows::Storage::Streams;
		using namespace Windows::Graphics::Imaging;

		return create_async([data](progress_reporter<int> reporter) -> SoftwareBitmap^
		{
			if (StringHelpers::IsNullOrEmpty(data))
			{
				return nullptr;
			}
			reporter.report(1);
			auto buffer = CryptographicBuffer::DecodeFromBase64String(data);
			if (!buffer)
			{
				reporter.report(100);
				return nullptr;
			}

			InMemoryRandomAccessStream^ outputStream = ref new InMemoryRandomAccessStream();
			auto readcnt = create_task(outputStream->WriteAsync(buffer)).get();
			reporter.report(20);

			BitmapDecoder^ bmpDec = create_task(BitmapDecoder::CreateAsync(outputStream)).get();
			reporter.report(40);

			SoftwareBitmap^ bmp = create_task(bmpDec->GetSoftwareBitmapAsync()).get();
			reporter.report(80);

			bmp = SoftwareBitmap::Convert(bmp, BitmapPixelFormat::Bgra8, BitmapAlphaMode::Premultiplied);
			reporter.report(100);

			outputStream = nullptr;
			bmpDec = nullptr;

			return bmp;
		});
	}

	IAsyncOperationWithProgress<String^, int>^ ProductImage::BeginConvertFromFileToData(StorageFile^ file)
	{
		using namespace Windows::Security::Cryptography;
		using namespace Windows::Storage::Streams;

		return create_async([file](progress_reporter<int> reporter) -> String^
		{
			if (!file)
			{
				reporter.report(100);
				return nullptr;
			}
			auto stream = create_task(file->OpenAsync(FileAccessMode::Read)).get();
			reporter.report(45);
			auto buff = ref new Buffer(stream->Size);
			auto loadedBuffer = create_task(stream->ReadAsync(buff, stream->Size, InputStreamOptions::None)).get();
			reporter.report(45);
			auto data = CryptographicBuffer::EncodeToBase64String(buff);
			stream = nullptr;
			buff = nullptr;
			reporter.report(10);
			return data;
		});
	}

	IAsyncOperationWithProgress<SoftwareBitmap^, int>^ ProductImage::BeginConvertStorageFileToImage(StorageFile ^ file)
	{
		return create_async([file](progress_reporter<int> reporter) -> SoftwareBitmap^
		{
			if (!file)
			{
				reporter.report(100);
				return nullptr;
			}
			auto strm = create_task(file->OpenReadAsync()).get();
			auto bmpDecoder = create_task(BitmapDecoder::CreateAsync(strm)).get();
			auto sbmp = create_task(bmpDecoder->GetSoftwareBitmapAsync()).get();
			sbmp = SoftwareBitmap::Convert(sbmp, BitmapPixelFormat::Bgra8, BitmapAlphaMode::Premultiplied);
			strm = nullptr;
			bmpDecoder = nullptr;
			return sbmp;
		});
	}

	bool ProductImage::IsEmpty::get()
	{
		return StringHelpers::IsNullOrEmpty(this->data);
	}


	DEFINERAISEPROPCHANGED(ProductImage)
}