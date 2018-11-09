#pragma once

#include <regex>
#include "NativeTypeWrapper.h"

using namespace Windows::UI::Xaml::Input;

namespace scwin
{

	typedef NativeTypeWrapper<std::wregex> RGWrapper;
		
	[Windows::Foundation::Metadata::WebHostHiddenAttribute]
	public ref class ColorMasking sealed : DependencyObject
	{
	private:
		static DependencyProperty^ passColorProperty;
		static DependencyProperty^ failColorProperty;
		static DependencyProperty^ maskProperty;
		static DependencyProperty^ maskExprProperty;
		static DependencyProperty^ failedMessageProperty;
		static Windows::Foundation::EventRegistrationToken keyUpEventToken;

	public:

		ColorMasking();
	private:
		~ColorMasking();
	
	public:
		static void RegisterDependencyProperties();
		static void UnregisterDependencyProperties();

		static property DependencyProperty^ PassColorProperty
		{
			DependencyProperty^ get()
			{
				return passColorProperty;
			}
		}

		static property DependencyProperty^ FailColorProperty
		{
			DependencyProperty^ get()
			{
				return failColorProperty;
			}
		}

		static property DependencyProperty^ MaskProperty
		{
			DependencyProperty^ get()
			{
				return maskProperty;
			}
		}

		static property DependencyProperty^ MaskExpressionProperty
		{
			DependencyProperty^ get()
			{
				return maskProperty;
			}
		}

		static property DependencyProperty^ FailedMessageProperty
		{
			DependencyProperty^ get()
			{
				return failedMessageProperty;
			}
		}

	internal:
		static void SetMaskExpression(DependencyObject^ obj, RGWrapper^ maskExpression);
		
		static RGWrapper^ GetMaskExpression(DependencyObject^ obj);

	public:
		static void OnMaskChanged(DependencyObject^ dependencyObject, DependencyPropertyChangedEventArgs^ e);

		static void SetPassColor(DependencyObject^ obj, String^ passColor)
		{
			obj->SetValue(PassColorProperty, passColor);
		}

		static String^ GetPassColor(DependencyObject^ obj)
		{
			return (String^)obj->GetValue(PassColorProperty);
		}


		static void SetFailColor(DependencyObject^ obj, String^ failColor)
		{
			obj->SetValue(FailColorProperty, failColor);
		}

		static String^ GetFailColor(DependencyObject^ obj)
		{
			return (String^)obj->GetValue(FailColorProperty);
		}

		/// <summary>
		/// Gets the mask for a given <see cref="TextBox"/>.
		/// </summary>
		/// <param name="textBox">
		/// The <see cref="TextBox"/> whose mask is to be retrieved.
		/// </param>
		/// <returns>
		/// The mask, or <see langword="null"/> if no mask has been set.
		/// </returns>
		static String^ GetMask(TextBox^ textBox);

		/// <summary>
		/// Sets the mask for a given <see cref="TextBox"/>.
		/// </summary>
		/// <param name="textBox">
		/// The <see cref="TextBox"/> whose mask is to be set.
		/// </param>
		/// <param name="mask">
		/// The mask to set, or <see langword="null"/> to remove any existing mask from <paramref name="textBox"/>.
		/// </param>
		static void SetMask(TextBox^ textBox, String^ mask);

		static void SetFailedMessage(DependencyObject^ obj, String^ message)
		{
			obj->SetValue(FailedMessageProperty, message);
		}

		static String^ GetFailedMessage(DependencyObject^ obj)
		{
			return (String^)obj->GetValue(FailedMessageProperty);
		}



	private:
		static void textBox_KeyUp(Object^ obj, KeyRoutedEventArgs^ e);

		static void OnPassColorChanged(DependencyObject^ dependencyObject, DependencyPropertyChangedEventArgs^ e);

		//static void textBox_PreviewTextInput(object sender, TextCompositionEventArgs e)
		//{
		//	var textBox = sender as TextBox;
		//	var maskExpression = GetMaskExpression(textBox);

		//	string passHex = (string)textBox.GetValue(PassColorProperty);
		//	string failHex = (string)textBox.GetValue(FailColorProperty);
		//	Color passColor = Extensions.ToColorFromHex(passHex);
		//	Color failColor = Extensions.ToColorFromHex(failHex);

		//	if (maskExpression == null)
		//	{
		//		return;
		//	}

		//	var proposedText = GetProposedText(textBox, e.Text);

		//	if (!maskExpression.IsMatch(proposedText))
		//	{
		//		textBox.Background = new SolidColorBrush(failColor);
		//	}
		//	else
		//	{
		//		textBox.Background = new SolidColorBrush(passColor);
		//	}
		//}

		//static void textBox_PreviewKeyDown(Object^ sender, KeyEventArgs^ e)
		//{
		//	var textBox = sender as TextBox;
		//	var maskExpression = GetMaskExpression(textBox);

		//	string passHex = (string)textBox.GetValue(PassColorProperty);
		//	string failHex = (string)textBox.GetValue(FailColorProperty);
		//	Color passColor = Extensions.ToColorFromHex(passHex);
		//	Color failColor = Extensions.ToColorFromHex(failHex);

		//	if (maskExpression == null)
		//	{
		//		return;
		//	}

		//	string proposedText = null;

		//	//pressing space doesn't raise PreviewTextInput, reasons here http://social.msdn.microsoft.com/Forums/en-US/wpf/thread/446ec083-04c8-43f2-89dc-1e2521a31f6b?prof=required
		//	if (e.Key == Key.Space)
		//	{
		//		proposedText = GetProposedText(textBox, " ");
		//	}
		//	// Same story with backspace
		//	else if (e.Key == Key.Back)
		//	{
		//		proposedText = GetProposedTextBackspace(textBox);
		//	}

		//	if (proposedText != null && !maskExpression.IsMatch(proposedText))
		//	{
		//		textBox.Background = new SolidColorBrush(failColor);
		//	}
		//	else
		//	{
		//		textBox.Background = new SolidColorBrush(passColor);
		//	}

		//}

		//static void Pasting(object sender, DataObjectPastingEventArgs e)
		//{
		//	TextBox textBox = sender as TextBox;
		//	var maskExpression = GetMaskExpression(textBox);

		//	string passHex = (string)textBox.GetValue(PassColorProperty);
		//	string failHex = (string)textBox.GetValue(FailColorProperty);
		//	Color passColor = Extensions.ToColorFromHex(passHex);
		//	Color failColor = Extensions.ToColorFromHex(failHex);

		//	if (maskExpression == null)
		//	{
		//		return;
		//	}

		//	if (e.DataObject.GetDataPresent(typeof(string)))
		//	{
		//		var pastedText = e.DataObject.GetData(typeof(string)) as string;
		//		var proposedText = GetProposedText(textBox, pastedText);

		//		if (!maskExpression.IsMatch(proposedText))
		//		{
		//			textBox.Background = new SolidColorBrush(failColor);
		//		}
		//		else
		//		{
		//			textBox.Background = new SolidColorBrush(passColor);
		//		}
		//	}
		//	else
		//	{
		//		textBox.Background = new SolidColorBrush(failColor);
		//	}
		//}

		//static String^ GetProposedTextBackspace(TextBox^ textBox);

		//static String^ GetProposedText(TextBox^ textBox, String^ newText);

		//static String^ GetTextWithSelectionRemoved(TextBox^ textBox);
	};
}

