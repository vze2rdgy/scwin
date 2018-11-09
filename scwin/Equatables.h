#pragma once

#include <functional>

using namespace Windows::UI;


namespace Sg
{
	struct EquateColor : public std::equal_to<Color>
	{
		constexpr bool operator()(const Color& _Left, const Color& _Right) const;
	};
}

constexpr bool Sg::EquateColor::operator()(const Color& _Left, const Color& _Right) const
{	// apply operator== to operands
	return _Left.A == _Right.A &&
		_Left.R == _Right.R &&
		_Left.G == _Right.G &&
		_Left.B == _Right.B;
}
