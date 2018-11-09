#pragma once
#include "dto.h"
#include "ProductViewPanels.h"
#include "typedefinitions.h"
#include "PricePlanViewModelFacade.h"
#include "ProductInventoryFacade.h"

namespace scwin
{

	[Bindable]
	public ref class ProductViewModel sealed : public INotifyPropertyChanged
	{
	private:
		int lastContentid = 0;
		int panelBeingEdited = -1;
		int panelExpanded = 0;
		IVector<ProductViewPanel^>^ prodPanels;
		Product^ currProd;
		Product^ prodMemento;
		UIEditMode editMode = UIEditMode::Unmodified;
		IVector<IVectorView<ProductCategory^>^>^ prodCategories;
		IVector<ProductImage^>^ prodImages;
		bool bAreOneOreMoreImagesSelected = false;
		bool displayImageEditor = false;
		PricePlanViewModelFacade^ pplan;
		AttributeViewModelFacade^ pattrs;

		int selectedImageCount = 0;

	private:

		void InvokePropertyChanged(String^ propName);

		void RaisePropertyChanged(String^ propName);

		task < IVector<IVector<ProductCategory^>^>^> ListProductCategories();

		void ExecuteAddImageCommand(Object^ param);
		bool CanExecuteAddImageCommand(Object^ param);

		void ExecuteShowImageEditorCommand(Object^ param);
		bool CanExecuteShowImageEditorCommand(Object^ param);
		
		void ExecuteDeleteImageCommand(Object^ param);
		bool CanExecuteDeleteImageCommand(Object^ param);

		void OnPubsubMessage(Platform::Object^ sender, PubSubMessageIds messageId, Platform::Object^ data);

	public:
		ProductViewModel();
		virtual ~ProductViewModel();

		void Initialize(Product^ product);


		void SetupProductSpecifics(int panel);
		void LoadProductDataForPanel(int panel);
		void UnloadProductDataForPanel(int panel);


		void UpdateImageProperty(IVector<ProductImage^>^ newdata) 
		{
			this->Images = newdata;
		}

	internal:
		MessageIds CancelProductDataForPanel(int panel);
		IAsyncOperation<int>^ SaveProductDataForPanel(int panel);
		void SaveState();

	public:

		property bool IsProductManager;


		property UIEditMode EditMode
		{
			UIEditMode get()
			{
				return editMode;
			}
			void set(UIEditMode mode)
			{
				editMode = mode;
				InvokePropertyChanged("EditMode");
			}
		}

		property Product^ CurrentProduct
		{
			Product^ get()
			{
				return currProd;
			}
			void set(Product^ val)
			{
				if (currProd = val)
				{
					currProd = val;
					InvokePropertyChanged("CurrentProduct");
				}
			}
		}

		property ProductCategoryPath^ CurrentProductCategories
		{
			ProductCategoryPath^ get()
			{
				if (!prodCategories)
				{
					prodCategories = ref new Vector<IVectorView<ProductCategory^>^>();
				}
				return prodCategories;
			}
		}

		property IVector<ProductViewPanel^>^ ProductPanels
		{
			IVector<ProductViewPanel^>^ get() { return this->prodPanels; }
		}

		property int EditPanel
		{
			int get() { return panelBeingEdited; }
			void set(int val);
		}

		property int ExpandPanel
		{
			int get() { return panelExpanded; }
			void set(int val);
		}


		/* Properties below are in use with ProductManageImages xaml view */
		property bool AreOneOreMoreImagesSelected
		{
			bool get() { return bAreOneOreMoreImagesSelected; }
			void set(bool val) 
			{
				selectedImageCount = val ? selectedImageCount + 1 : selectedImageCount - 1;
				bAreOneOreMoreImagesSelected = selectedImageCount;
				InvokePropertyChanged("AreOneOreMoreImagesSelected");
			}
		}

		property bool DisplayImageEditor
		{
			bool get() { return displayImageEditor; }
			void set(bool val)
			{
				displayImageEditor = val;
				InvokePropertyChanged("DisplayImageEditor");
			}
		}

		property IVector<ProductImage^>^ Images
		{
			IVector<ProductImage^>^ get();
			void set(IVector<ProductImage^>^);
		}

		property Command^ ShowImageEditorCommand;

		property Command^ AddImageCommand;

		property Command^ DeleteImageCommand;

		/* Properties below are in use with ProductPricePlan xaml view */

		property PricePlanViewModelFacade^ PricePlan
		{
			PricePlanViewModelFacade^ get() { return pplan; }
			void set(PricePlanViewModelFacade^ plan)
			{
				if (pplan != plan)
				{
					pplan = plan;
					InvokePropertyChanged("PricePlan");
				}
			}
		}

		/* Properties below are for product attributes and families */
		property AttributeViewModelFacade^ Attributes
		{
			AttributeViewModelFacade^ get()
			{
				return this->pattrs;
			}

			void set(AttributeViewModelFacade^ val)
			{
				if (pattrs != val)
				{
					pattrs = val;
					InvokePropertyChanged("Attributes");
				}
			}
		}

		/* Other properties and methods are below.*/

		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

	internal:

		// Start editing on a clone of the product instance.
		void BeginEdit();

		IAsyncOperation<int>^ Save();

		// replace the original product with the the cloned product if result from
		// save operation is success, else retry.
		void CommitEdit();
		
		void RollbackEdit();

	};




}
