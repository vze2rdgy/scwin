#include "pch.h"
#include "ColorHelpers.h"
#include "StringHelpers.h"
#include <sstream>

using namespace scwin::utils;


void ColorHelpers::SetFromHex(Color& c, String^ hex)
{
	Color c1 = ToColorFromHex(hex);
	c.A = c1.A;
	c.R = c1.R;
	c.G = c1.G;
	c.B = c1.B;
}

Color ColorHelpers::ToColorFromHex(String^ hex)
{
	if (StringHelpers::IsNullOrEmpty(hex))
	{
		throw ref new InvalidArgumentException("hex argument cannot be empty.");
	}

	// remove any "#" characters
	while (*hex->Begin() == '#')
	{
		hex = StringHelpers::Substring(hex, 1, hex->Length()-1);
	}

	int num = 0;
	std::stringstream sstr;
	sstr << std::hex << hex->Data();
	sstr >> num;

	int pieces[4];
	if (hex->Length() > 7)
	{
		pieces[0] = ((num >> 24) & 0x000000ff);
		pieces[1] = ((num >> 16) & 0x000000ff);
		pieces[2] = ((num >> 8) & 0x000000ff);
		pieces[3] = (num & 0x000000ff);
	}
	else if (hex->Length() > 5)
	{
		pieces[0] = 255;
		pieces[1] = ((num >> 16) & 0x000000ff);
		pieces[2] = ((num >> 8) & 0x000000ff);
		pieces[3] = (num & 0x000000ff);
	}
	else if (hex->Length() == 3)
	{
		pieces[0] = 255;
		pieces[1] = ((num >> 8) & 0x0000000f);
		pieces[1] += pieces[1] * 16;
		pieces[2] = ((num >> 4) & 0x000000f);
		pieces[2] += pieces[2] * 16;
		pieces[3] = (num & 0x000000f);
		pieces[3] += pieces[3] * 16;
	}
	return Windows::UI::ColorHelper::FromArgb(pieces[0], pieces[1], pieces[2], pieces[3]);
}
