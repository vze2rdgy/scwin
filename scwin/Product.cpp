#include "pch.h"
#include "ProductAttributeAndValue.h"
#include "Product.h"

namespace scwin
{

	Product::Product()
	{
		CanRaiseEvents = true;
		this->id = SysInfoHelpers::NewCouchDbId();
		ObjectState = DataState::Added;
	}

	Product::Product(String^ id)
	{
		CanRaiseEvents = true;
		this->id = id;
		ObjectState = DataState::Unmodified;
	}

	IAsyncOperation<IVector<ProductAttributeAndValue^>^>^ Product::GetAttributes()
	{
		return scLocalDataService::Instance()->GetProductAttributesAsync(this->id);
	}

	DEFINERAISEPROPCHANGED(Product)

	Product^ Product::Clone()
	{
		Product^ p = ref new Product(this->id);
		if (barCode)
		{
			p->barCode = ref new Array<UCHAR>(p->barCode->Length);
			for (int i=0; i<barCode->Length;i++)
			{
				p->barCode[i] = barCode[i];
			}
		}
		p->desc = desc;
		p->isStockLess = isStockLess;
		p->name = name;
		p->notes = notes;
		p->pricePlanId = pricePlanId;
		if (pfamily)
		{
			p->pfamily = pfamily->Clone();
		}
		p->shortDesc = shortDesc;
		p->skun = skun;
		p->subName = subName;

		return p;
	}

	bool Product::IsEmpty::get()
	{
		return StringHelpers::IsNullOrEmpty(this->name);
	}

	ProductImage^ Product::TitleImage::get()
	{
		return img;
	}

	void Product::TitleImage::set(ProductImage^ val)
	{
		if (val != img)
		{
			img = val;
			RaisePropertyChanged("ImageSource");
		}
	}
}