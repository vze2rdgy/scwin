#pragma once

namespace scwin
{
	public ref class NavMenuItem sealed :
		public INotifyPropertyChanged
	{
	private:
		bool isSelected;
		Visibility selectedVisibility;

	public:
		NavMenuItem(String^ label);

		NavMenuItem(String^, int, String^, TypeName);
		NavMenuItem(String^, int, String^, TypeName, Object^ args);

		property String^ Label;
		property Symbol Symbol;
		property String^ ImageSource;
		property String^ ParentLabel;
		property String^ FontName;
		property String^ Glyph;

		property TypeName DestPage;

		property Object^ Arguments;
		
		property bool IsSelected
		{
			bool get();
			void set(bool);
		}

		property Visibility SelectedVisibility
		{
			Visibility get();
			void set(Visibility value);
		}

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

	protected:
		void NotifyPropertyChanged(String^ propName);

	private:
		String ^ ConvertGlyphToString(int glyph);
	};
}
