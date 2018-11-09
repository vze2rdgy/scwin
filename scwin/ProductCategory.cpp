#include "pch.h"
#include "ProductCategory.h"


ProductCategory::ProductCategory()
{
	id = SysInfoHelpers::NewCouchDbId();
	CanRaiseEvents = true;
	ObjectState = DataState::Added;
}

scwin::ProductCategory::ProductCategory(String^ id)
{
	this->id = id;
	CanRaiseEvents = true;
	ObjectState = DataState::Unmodified;
}

//void scwin::ProductCategory::RaisePropertyChanged(String ^ propName)
//{
//	if (!CanRaiseEvents) return;
//	WeakReference wr(this); 
//	if (Window::Current)
//	{
//		auto disp = Window::Current->Dispatcher;
//		disp->RunAsync(CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([wr, propName]()
//		{
//			auto ths = wr.Resolve<ProductCategory>();
//			ths->PropertyChanged(ths, ref new PropertyChangedEventArgs(propName));
//		}
//		));
//	}
//	else
//	{
//		PropertyChanged(this, ref new PropertyChangedEventArgs(propName));
//	}
//}

DEFINERAISEPROPCHANGED(ProductCategory)

int ProductCategory::ChildrenCount::get()
{
	// query cache database
	// not ideal to have a block call. will revisit.
	return scLocalDataService::Instance()->GetCache().GetCategoryChildrenCountAsync(this).get();
}

IAsyncOperationWithProgress<IObservableVector<ProductCategory^>^, int>^ scwin::ProductCategory::GetCategoryChildren()
{
	if (KeepChildren)
	{
		WeakReference wr(this);
		if (!children)
		{
			GUARDEDGLOCK;
			if (!children)
			{
				create_task([wr]()
				{
					auto ths = wr.Resolve<ProductCategory>();
					ths->children = scLocalDataService::Instance()->GetCategoryChildren(ths)->GetResults();
				});
			}
		}
		return create_async([wr](progress_reporter<int> reporter, cancellation_token token)
		{
			auto ths = wr.Resolve<ProductCategory>();
			return ths->children;
		});
	}
	else
	{
		return scLocalDataService::Instance()->GetCategoryChildren(this);
	}
}

bool ProductCategory::IsEmpty::get()
{
	return StringHelpers::IsNullOrEmpty(name);
}