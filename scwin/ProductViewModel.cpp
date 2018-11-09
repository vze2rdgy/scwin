#include "pch.h"
#include "ProductViewModel.h"
#include "cpplinq.hpp"
#include "ProductImageEditView.xaml.h"
#include "ImageHelpers.h"

using namespace Windows::Storage::Pickers;

ProductViewModel::ProductViewModel()
{

	// TODO: Roles is still very rudimentary. We need a much better approach on user roles.

	IsProductManager = true;

	Array<PubSubMessageIds>^ messageIds = { PubSubMessageIds::PricePlanSelected };
	PubSubService::Instance->Subscribe(
		this,
		ref new ReceivePubSubMessageCallback(this, &ProductViewModel::OnPubsubMessage),
		messageIds);

	prodPanels = ref new Vector<ProductViewPanel^>();
	prodPanels->Append(ref new ProductViewPanel());	// General
	prodPanels->Append(ref new ProductViewPanel());	// Images
	prodPanels->Append(ref new ProductViewPanel());	// Price Plan
	prodPanels->Append(ref new ProductViewPanel());	// Family and Attributes

	AddImageCommand = ref new Command(
		ref new ExecuteAction(this, &ProductViewModel::ExecuteAddImageCommand),
		ref new CanExecuteFunc(this, &ProductViewModel::CanExecuteAddImageCommand));

	ShowImageEditorCommand = ref new Command(
		ref new ExecuteAction(this, &ProductViewModel::ExecuteShowImageEditorCommand),
		ref new CanExecuteFunc(this, &ProductViewModel::CanExecuteShowImageEditorCommand));

	DeleteImageCommand = ref new Command(
		ref new ExecuteAction(this, &ProductViewModel::ExecuteDeleteImageCommand),
		ref new CanExecuteFunc(this, &ProductViewModel::CanExecuteDeleteImageCommand));

}

DEFINERAISEPROPCHANGEDBASIC(ProductViewModel)

void scwin::ProductViewModel::InvokePropertyChanged(String ^ propName)
{
	RaisePropertyChanged(propName);
}

task<IVector<IVector<ProductCategory^>^>^> scwin::ProductViewModel::ListProductCategories()
{
	return task<IVector<IVector<ProductCategory^>^>^>();
}


scwin::ProductViewModel::~ProductViewModel()
{
	PubSubService::Instance->Unsubscribe(this);
}

void scwin::ProductViewModel::Initialize(Product^ product)
{
	if (product == nullptr && this->CurrentProduct == nullptr)
	{
		// initialize a new product
		this->CurrentProduct = ref new Product();
		this->currProd->Name = "New Product";
		EditMode == UIEditMode::Add;
	}
	else if (product != nullptr && (CurrentProduct == nullptr || product->Id != CurrentProduct->Id))
	{
		WeakReference wr(this);
		CurrentProduct = product;
		// populate categories assigned to the product
		create_task(scLocalDataService::Instance()->GetProductCategoriesAsync(product->Id))
			.then([wr](IVector<ProductCategory^>^ cats) 
		{
			auto ths = wr.Resolve<ProductViewModel>();
			ths->CurrentProductCategories->Clear();
			for (auto c : cats)
			{
				// obtain path
				create_task(LOCSVC->GetCategoryRootToCurrentPath(c->Id))
					.then([wr](IVectorView<ProductCategory^>^ path)
				{
					auto ths = wr.Resolve<ProductViewModel>();
					ths->CurrentProductCategories->Append(path);
				});
			}
		});
		EditMode == UIEditMode::Edit;
	}
	else
	{
		return;
	}

	ExpandPanel = 0;
	EditPanel = -1;
}

void scwin::ProductViewModel::SetupProductSpecifics(int currentlyEditedPanel)
{
	Calendar^ cal;
	// For each step change edit/save/cancel buttons states when data population is completed. 
	switch (currentlyEditedPanel)
	{
	case 0:
		//0.  if product is not new, populate the product categories into CurrentProductCategories
		break;
	case 1:
		//1. Populate Images
		break;
	case 2: // Pricing plans. 
		// facade has null price plans, then load default values.
		if (PricePlan->PricePlan == nullptr)
		{
			PricePlan->PricePlan = ref new ProductPricePlan();
		}
		if (PricePlan->PriceDetails == nullptr || PricePlan->PriceDetails->Size == 0)
		{
			PricePlan->PriceDetails = ref new Vector<IPricePlanDetails^>();
			if (PricePlan->IsTieredPricePlan)
			{
				PricePlan->PriceDetails->Append(ref new PricePlanTierDetails());
			}
			else
			{
				PricePlan->PriceDetails->Append(ref new PricePlanStandardDetails());
			}
		}
		cal = ref new Calendar();
		PricePlan->PricePlan->StartDate = cal->GetDateTime();
		break;
	case 3: // Family and attributes
		break;
	}
}

void scwin::ProductViewModel::LoadProductDataForPanel(int panel)
{
	switch (panel)
	{
	case 0:
		//0.  if product is not new, populate the product categories into CurrentProductCategories

		break;
	case 1:
		//1. Populate Images
		selectedImageCount = 0;
		create_task(scLocalDataService::Instance()->GetProductImagesAsync(CurrentProduct->Id))
			.then([=](IVector<ProductImage^>^ images)
		{
			UpdateImageProperty(images);
		});
		break;
	case 2: // Pricing plans. 
		PricePlan = ref new PricePlanViewModelFacade(currProd);
		break;
	case 3: // Family and attributes
		Attributes = ref new AttributeViewModelFacade(currProd);
		break;
	}
}

void scwin::ProductViewModel::UnloadProductDataForPanel(int panel)
{
	switch (panel)
	{
	case 0: // details.
		break;
	case 1: // images
		break;
	case 2: // price plans
		break;
	case 3: // attributes
		break;
	}
}

MessageIds scwin::ProductViewModel::CancelProductDataForPanel(int panel)
{
	LoadProductDataForPanel(panel); // restore to original state.

	return MessageIds::Success;
}

IAsyncOperation<int>^ scwin::ProductViewModel::SaveProductDataForPanel(int panel)
{
	WeakReference wr(this);
	return create_async([wr, panel]()
	{
		auto ths = wr.Resolve<ProductViewModel>();
		ProductPricePlan^ plan;
		ProductFamily^ family;
		MessageIds code;
		switch (panel)
		{
		case 1:
			MessageIds code;
			// look for images which are modified
			for (auto pimg : ths->prodImages)
			{
				if (pimg->ObjectState != DataState::Unmodified)
				{
					if (!pimg->IsValid)
					{
						PubSubService::Instance->Publish(ths, PubSubMessageIds::ApplicationErrorStatusMessage, "Product Images : Required information are not provided.");
						return (int)MessageIds::MissingValuesError;
					}

					pimg->Pid = ths->CurrentProduct->Id;
					code = scLocalDataService::Instance()->SaveProductImageAsync(pimg).get();
					if ((int)code > 2) // some kind of error
					{
						PubSubService::Instance->Publish(
							ths,
							PubSubMessageIds::ApplicationErrorStatusMessage,
							"Some images failed to save. Please retry."
						);
					}
				}
			}
			break;
		case 2:
			// validate
			plan = ths->PricePlan->PricePlan;

			if (!plan->IsValid || (ths->PricePlan->PriceDetails == nullptr || ths->PricePlan->PriceDetails->Size == 0))
			{
				PubSubService::Instance->Publish(ths, PubSubMessageIds::ApplicationErrorStatusMessage, "Not all required information is provided for price plan ");
				return (int)MessageIds::MissingValuesError;
			}

			for (auto pd : ths->PricePlan->PriceDetails->GetView())
			{
				if (!StringHelpers::IsNullOrEmpty(pd->Id))
				{
					pd->PricePlanId = plan->Id;
				}
				if (!pd->IsValid)
				{
					PubSubService::Instance->Publish(ths, PubSubMessageIds::ApplicationErrorStatusMessage, "Not all required information is provided for price plan ");
					return (int)MessageIds::MissingValuesError;
				}
			}
			if (!ths->PricePlan->IsTieredPricePlan)
			{
				code = create_task(scLocalDataService::Instance()
					->SavePricePlan(plan, dynamic_cast<PricePlanStandardDetails^>(ths->PricePlan->PriceDetails->GetAt(0)))).get();
				if (HASFAILED(code))
				{
					PubSubService::Instance->Publish(ths, PubSubMessageIds::ApplicationErrorStatusMessage, ref new String(GetMessage(MessageIds::DBUpdateFailed)));
					return (int)MessageIds::DBUpdateFailed;
				}
			}
			else
			{
				Vector<PricePlanTierDetails^>^ tieredPrices = ref new Vector<PricePlanTierDetails^>();
				for (auto tierdet : ths->PricePlan->PriceDetails->GetView())
				{
					tieredPrices->Append(dynamic_cast<PricePlanTierDetails^>(tierdet));
				}
				code = create_task(scLocalDataService::Instance()->SavePricePlan(plan, tieredPrices)).get();
				if (HASFAILED(code))
				{
					PubSubService::Instance->Publish(ths, PubSubMessageIds::ApplicationErrorStatusMessage, ref new String(GetMessage(MessageIds::DBUpdateFailed)));
					return (int)MessageIds::DBUpdateFailed;
				}
			}

			code = create_task(scLocalDataService::Instance()->AssociatePricePlanWithProduct(ths->CurrentProduct->Id, plan->Id)).get();
			if (HASFAILED(code))
			{
				PubSubService::Instance->Publish(ths, PubSubMessageIds::ApplicationErrorStatusMessage, ref new String(GetMessage(MessageIds::DBUpdateFailed)));
				return (int)MessageIds::DBUpdateFailed;
			}

		case 3: // Family/attribute panel
			family = ths->Attributes->Family;
			if (family->IsValid)
			{
				code = create_task(scLocalDataService::Instance()->UpdateProductFamilyAsync(ths->Attributes->Family, ths->CurrentProduct->Id)).get();
				if (HASFAILED(code))
				{
					PubSubService::Instance->Publish(ths, PubSubMessageIds::ApplicationErrorStatusMessage, ref new String(GetMessage(MessageIds::DBUpdateFailed)));
					return (int)MessageIds::DBUpdateFailed;
				}
				else
				{
					ths->CurrentProduct->Family = family;
				}
			}
			break;
		}
		return (int)MessageIds::Success;
	});
}

void scwin::ProductViewModel::ExecuteAddImageCommand(Object ^ param)
{
	if (prodImages == nullptr)
	{
		prodImages = ref new Vector<ProductImage^>();
	}

	WeakReference wr(this);

	FileOpenPicker^ picker = ref new FileOpenPicker();
	picker->ViewMode = PickerViewMode::Thumbnail;
	picker->SuggestedStartLocation = PickerLocationId::PicturesLibrary;
	picker->FileTypeFilter->Append(".jpg");
	picker->FileTypeFilter->Append(".png");
	picker->FileTypeFilter->Append(".jpeg");
	picker->FileTypeFilter->Append(".gif");
	picker->FileTypeFilter->Append(".tif");

	create_task(picker->PickMultipleFilesAsync())
		.then([wr](IVectorView<StorageFile^>^ pickedfiles)
	{

		// add the files
		// convert to imagesource
		// convert to data
		for (auto file : pickedfiles)
		{
			ProductImage^ imgFile =
				ref new ProductImage();
			imgFile->Title = "";
			imgFile->Description = "";
			imgFile->IsTitleImage = false;
			create_task(ProductImage::BeginConvertStorageFileToImage(file))
				.then([wr, file, imgFile](SoftwareBitmap^ bmp)
			{
				auto ths = wr.Resolve<ProductViewModel>();
				Window::Current->Dispatcher->RunAsync(
					CoreDispatcherPriority::Normal,
					ref new DispatchedHandler([wr, bmp, imgFile]()
				{
					auto ths = wr.Resolve<ProductViewModel>();
					auto img = ref new SoftwareBitmapSource();
					ths->Images->Append(imgFile);
					create_task(img->SetBitmapAsync(bmp)).then([imgFile, img]()
					{
						imgFile->Image = img;
					});
				}));
			}).then([file, imgFile]()
			{
				ImageHelpers::EncodeImageFileToBase64String(file)
					.then([imgFile](String^ str)
				{
					imgFile->Data = str;
					OutputDebugString(L"\nImage data is updated.\n");
				});
			});
		}
	});
}

bool scwin::ProductViewModel::CanExecuteAddImageCommand(Object ^ param)
{
	if (!(
		scLocalDataService::Instance()->GetCurrentUser()->HasRight(FunctionalRights::EditProducts)
		|| 
		scLocalDataService::Instance()->GetCurrentUser()->HasRight(FunctionalRights::AddProducts)
		))
		return false;
	return true;
}

void scwin::ProductViewModel::ExecuteShowImageEditorCommand(Object ^ param)
{
	this->DisplayImageEditor = true;
}

bool scwin::ProductViewModel::CanExecuteShowImageEditorCommand(Object ^ param)
{
	return true;
}

void scwin::ProductViewModel::ExecuteDeleteImageCommand(Object ^ param)
{
	throw ref new Platform::NotImplementedException();
}

bool scwin::ProductViewModel::CanExecuteDeleteImageCommand(Object ^ param)
{
	return false;
}

void scwin::ProductViewModel::OnPubsubMessage(Platform::Object ^ sender, PubSubMessageIds messageId, Platform::Object ^ data)
{
	auto disp = AppDispatcher;
	if (disp->HasThreadAccess)
	{
		switch (messageId)
		{
		case PubSubMessageIds::PricePlanSelected:
			if (this->PricePlan != nullptr)
			{
				this->PricePlan->PricePlanSelectionChanged((PricePlanType^)data);
			}
			break;
		}
	}
	else
	{
		disp->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([=]()
		{
			switch (messageId)
			{
			case PubSubMessageIds::PricePlanSelected:
				if (this->PricePlan != nullptr)
				{
					this->PricePlan->PricePlanSelectionChanged((PricePlanType^)data);
				}
				break;
			}
		}));

	}
}

void scwin::ProductViewModel::BeginEdit()
{
	prodMemento = currProd->Clone();
}

IAsyncOperation<int>^ scwin::ProductViewModel::Save()
{
	using namespace cpplinq;

	WeakReference wr(this);
	return create_async([wr]()
	{
		auto ths = wr.Resolve<ProductViewModel>();
		auto prod = ths->CurrentProduct;
		auto retCode = MessageIds::Success;
		switch (ths->EditPanel)
		{
		case 0:
			// obtain categories
			auto pcats = from(to_vector(ths->CurrentProductCategories)) >>
				select([](IVectorView<ProductCategory^>^ catpath)
			{
				return from(to_vector(catpath)) >> last_or_default();
			}) >> to_vector();
			retCode = scLocalDataService::Instance()
				->SaveProductWithCategoriesAsync(ths->CurrentProduct, ref new Vector<ProductCategory^>(pcats)).get();
			break;
		}
		return (int)retCode;
	});
}

void scwin::ProductViewModel::CommitEdit()
{
	using namespace cpplinq;

	// replace
	if (EditMode == UIEditMode::Add)
	{
		// update product in product list
		create_task(scLocalDataService::Instance()->GetProductsAsync())
			.then([=](ProductList^ products)
		{
			if (products)
			{
				//auto ps = static_cast<Vector<Product^>^>(products);
				//// find product and update.
				auto found = from(to_vector(products)) >> where([=](Product^ p)
				{
					return p->Id == CurrentProduct->Id;
				});
				auto prod = found.front();
				
			}
		});
		PubSubService::Instance->Publish(this, PubSubMessageIds::ProductNewCommitted, currProd);
	}
	else
	{
		PubSubService::Instance->Publish(this, PubSubMessageIds::ProductEditCommitted, currProd);
	}
	// clear memento
	prodMemento = nullptr;
}

void scwin::ProductViewModel::RollbackEdit()
{
	CurrentProduct = prodMemento;
}

void scwin::ProductViewModel::EditPanel::set(int val)
{
	if (val < 0 || val >= prodPanels->Size)
		return;
	panelBeingEdited = val;
	for (int i = 0; i < prodPanels->Size; i++)
	{
		if (panelBeingEdited == -1)
		{
			prodPanels->GetAt(i)->IsEdited = false;
		}
		else
		{
			prodPanels->GetAt(i)->IsEdited = (i == val);
		}
	}
	SetupProductSpecifics(val);
	InvokePropertyChanged("EditPanel");
}

void scwin::ProductViewModel::ExpandPanel::set(int val)
{
	if (val < 0 || val >= prodPanels->Size)
		return;
	panelExpanded = val;
	//for (int i = 0; i < prodPanels->Size; i++)
	//{
	prodPanels->GetAt(val)->IsExpanded = !prodPanels->GetAt(val)->IsExpanded;
	//}
	InvokePropertyChanged("ExpandPanel");
}

IVector<ProductImage^>^ ProductViewModel::Images::get()
{
	if (prodImages == nullptr)
		prodImages = ref new Vector<ProductImage^>();
	return prodImages;
}

void ProductViewModel::Images::set(IVector<ProductImage^>^ val)
{
	prodImages = val;
	InvokePropertyChanged("Images");
}


void ProductViewModel::SaveState()
{

}