#pragma once
#include "dto.h"

using namespace Windows::Storage;
using namespace Windows::Graphics::Imaging;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Imaging;

namespace scwin
{
	[Bindable]
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class ProductImage sealed : public IDTOBase
	{
		String^ id;
		String^ pid;
		bool isTitleImage;
		String^ data;
		String^ contentType;
		String^ title;
		String^ desc;
		ImageSource^ image;

	public:
		ProductImage();

		ProductImage(String^ id);

		property String^ Id
		{
			String^ get()
			{
				return id;
			}
		}

		property String^ Pid
		{
			String^ get() { return this->pid; }
			void set(String^ val)
			{
				if (pid != val)
				{
					this->pid = val;
					RaisePropertyChanged("Pid");
				}
			}
		}
		property bool IsTitleImage
		{
			bool get() { return this->isTitleImage; }
			void set(bool val)
			{
				if (isTitleImage != val)
				{
					this->isTitleImage = val;
					RaisePropertyChanged("IsTitleImage");
				}
			}
		}

		/// <summary>Returns image data as a hex string </summary>
		/// <remarks>Use GetImage method to obtain a bitmap image from this data </remarks>
		property String^ Data
		{
			String^ get() 
			{ 
				return this->data; 
			}
			void set(String^ val)
			{
				if (data != val)
				{
					this->data = val;
					RaisePropertyChanged("Data");
				}
			}
		}

		property String^ ContentType
		{
			String^ get() { return this->contentType; }
			void set(String^ val)
			{
				if (contentType != val)
				{
					this->contentType = val;
					RaisePropertyChanged("ContentType");
				}
			}
		}

		property String^ Title
		{
			String^ get() { return this->title; }
			void set(String^ val)
			{
				if (title != val)
				{
					this->title = val;
					RaisePropertyChanged("Title");
				}
			}
		}

		property String^ Description
		{
			String^ get() { return this->desc; }
			void set(String^ val)
			{
				if (desc != val)
				{
					this->desc = val;
					RaisePropertyChanged("Description");
				}
			}
		}

		property ImageSource^ Image
		{
			ImageSource^ get()
			{
				return image;
			}
			void set(ImageSource^ val)
			{
				if (val != image)
				{
					image = val;
					RaisePropertyChanged("Image");
				}
			}
		}

		void BeginDataToImageConversion();

		static IAsyncOperationWithProgress<SoftwareBitmap^, int>^ BeginConvertFromDataToImage(String^ data);

		static IAsyncOperationWithProgress<String^, int>^ BeginConvertFromFileToData(StorageFile^ file);

		static IAsyncOperationWithProgress<SoftwareBitmap^, int>^ BeginConvertStorageFileToImage(StorageFile^ file);

		virtual property bool CanRaiseEvents;

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		void RaisePropertyChanged(String^ propName);

		// Inherited via IDTOBase
		virtual property DataState ObjectState;

		virtual property bool IsEmpty
		{
			bool get();
		}
		virtual property bool IsValid
		{
			bool get()
			{
				if (StringHelpers::IsNullOrEmpty(title) ||
					StringHelpers::IsNullOrEmpty(data))
					return false;
				return true;
			}
		}

	};

}