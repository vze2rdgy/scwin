#pragma once

using namespace Windows::UI::Xaml::Data;

namespace fmeta = Windows::Foundation::Metadata;

namespace scwin
{
	[fmeta::WebHostHidden]
	public ref class DtoBase 
	{
	public:

	protected private:
		JsonObject ^ _data = nullptr;

	protected private:
		DtoBase();
		DtoBase(JsonObject^ data);


	internal:
		virtual JsonObject^ serialize();

	public:
		virtual ~DtoBase();


		property String^ Id
		{
			String^ get();
		}

		property uint64 CreatedOn
		{
			uint64 get();
		}

		property String^ RevisionNum
		{
			String^ get();
		}

	};

}