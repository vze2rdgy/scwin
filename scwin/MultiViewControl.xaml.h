//
// MultiViewControl.xaml.h
// Declaration of the MultiViewControl class
//

#pragma once

namespace System = Platform;

#include "ProductView.xaml.h"
#include "MultiViewControl.g.h"

namespace scwin
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class MultiViewControl sealed
	{
		//static DependencyProperty^ contentTypeProperty;

		//static void ContentPropertyChanged(DependencyObject^ obj, DependencyPropertyChangedEventArgs^ e);

		Type^ contentType;

	public:
		//static void RegisterDependencyProperties();

		//static property DependencyProperty^ ContentTypeProperty
		//{
		//	DependencyProperty^ get()
		//	{
		//		return contentTypeProperty;
		//	}
		//}

	public:
		MultiViewControl();

		property TypeName ContentType
		{
			TypeName get()
			{
				//return dynamic_cast<Type^>(GetValue(contentTypeProperty));
				return contentType;
			}
			void set(TypeName val)
			{
				//SetValue(contentTypeProperty, val);
				contentType = val;
			}
		}

		void OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
	};
}
