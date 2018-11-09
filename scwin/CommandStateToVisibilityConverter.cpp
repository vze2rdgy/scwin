#include "pch.h"
#include "CommandStateToVisibilityConverter.h"

namespace scwin
{

	CommandStateToVisibilityConverter::CommandStateToVisibilityConverter()
	{
	}

	Platform::Object ^ scwin::CommandStateToVisibilityConverter::Convert(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
	{
		IBox<ICommand^>^ visibility = dynamic_cast<IBox<ICommand^>^>(value);
		if (visibility != nullptr && visibility->Value->CanExecute(parameter) != IsInverse)
		{
			return Visibility::Visible;
		}
		else
		{
			return Visibility::Collapsed;
		}
	}

	Platform::Object ^ scwin::CommandStateToVisibilityConverter::ConvertBack(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
	{
		throw ref new Platform::NotImplementedException();
		// TODO: insert return statement here
	}

}