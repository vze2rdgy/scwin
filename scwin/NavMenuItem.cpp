#include "pch.h"
#include "NavMenuItem.h"

using namespace scwin;

scwin::NavMenuItem::NavMenuItem(String ^ label)
{
	Label = label;
}

scwin::NavMenuItem::NavMenuItem(String ^ label, int glyph, String ^fontName, TypeName tp)
{
	Label = label;
	Glyph = ConvertGlyphToString(glyph);
	FontName = fontName;
	DestPage = tp;
	Arguments = nullptr;
}

scwin::NavMenuItem::NavMenuItem(String ^ label, int glyph, String ^ fontName, TypeName tn, Object ^ args)
{
	Label = label;
	Glyph = ConvertGlyphToString(glyph);
	FontName = fontName;
	DestPage = tn;
	Arguments = args;
}

bool NavMenuItem::IsSelected::get()
{
	return isSelected;
}

void NavMenuItem::IsSelected::set(bool val)
{
	isSelected = val;
	SelectedVisibility = val ? Visibility::Visible : Visibility::Collapsed;
	NotifyPropertyChanged("IsSelected");
}

Visibility NavMenuItem::SelectedVisibility::get()
{
	selectedVisibility = IsSelected ? Visibility::Visible : Visibility::Collapsed;
	return selectedVisibility;
}

void NavMenuItem::SelectedVisibility::set(Visibility val)
{
	selectedVisibility = val;
	NotifyPropertyChanged("SelectedVisibility");
}

void NavMenuItem::NotifyPropertyChanged(String ^ propName)
{
	PropertyChanged(this, ref new PropertyChangedEventArgs(propName));
}

String ^ scwin::NavMenuItem::ConvertGlyphToString(int glyph)
{
	WCHAR c[] = { (WCHAR)glyph, '\0' };
	return ref new String(c);
}
