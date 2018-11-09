//
// AttributeSetupLookup.xaml.h
// Declaration of the AttributeSetupLookup class
//

#pragma once

#include "AttributeSetupLookup.g.h"

namespace scwin
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class AttributeSetupLookup sealed : INotifyPropertyChanged
	{
	private:
		AttributeViewModelFacade^ facade;
		IVector<ProductAttribute^>^ attribs;
		IVector<String^>^ attrTypes;
		ProductAttribute^ currentAttrib;
		GeneralAttributeValue^ currentAttribVal;
	public:
		AttributeSetupLookup(AttributeViewModelFacade^ facade);

		property IVector<String^>^ AttributeTypes
		{
			IVector<String^>^ get()
			{
				return attrTypes;
			}
		}

		property IVector<ProductAttribute^>^ Attributes
		{
			IVector<ProductAttribute^>^ get() { return attribs; }
		private:
			void set(IVector<ProductAttribute^>^ val)
			{
				attribs = val;
				WeakReference wr(this);
				this->Dispatcher->RunAsync(CoreDispatcherPriority::Normal,
					ref new DispatchedHandler([wr]()
				{
					auto ths = wr.Resolve<AttributeSetupLookup>();
					ths->PropertyChanged(ths, ref new PropertyChangedEventArgs("Attributes"));
				}));
			}
		}

		property ProductAttribute^ CurrentAttribute
		{
			ProductAttribute^ get() { return currentAttrib; }
			void set(ProductAttribute^ val)
			{
				currentAttrib = val;
				WeakReference wr(this);
				this->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, 
					ref new DispatchedHandler([wr]()
				{
					auto ths = wr.Resolve<AttributeSetupLookup>();
					ths->PropertyChanged(ths, ref new PropertyChangedEventArgs("CurrentAttribute"));
					ths->PropertyChanged(ths, ref new PropertyChangedEventArgs("IsCurrentAttributeSet"));
				}));
			}
		}

		property GeneralAttributeValue ^ CurrentAttributeValue
		{
			GeneralAttributeValue^ get() { return currentAttribVal; }
			void set(GeneralAttributeValue^ val)
			{
				currentAttribVal = val;
				WeakReference wr(this);
				this->Dispatcher->RunAsync(CoreDispatcherPriority::Normal,
					ref new DispatchedHandler([wr]()
				{
					auto ths = wr.Resolve<AttributeSetupLookup>();
					ths->PropertyChanged(ths, ref new PropertyChangedEventArgs("CurrentAttributeValue"));
				}));
			}
		}

		property bool IsCurrentAttributeSet
		{
			bool get() { return this->currentAttrib != nullptr; }
		}

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

	private:

		void btnClose_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void btnAccept_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void btnAdd_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
		void AutoSuggestBox_QuerySubmitted(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxQuerySubmittedEventArgs^ args);
		void AutoSuggestBox_SuggestionChosen(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxSuggestionChosenEventArgs^ args);
		void txtFind_TextChanged(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs^ args);
		void lstAttributes_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void AutoSuggestBox_TextChanged(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs^ args);
		void txtAttributeValue_GotFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void txtUom_GotFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void lstAttributes_ItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
	};
}
