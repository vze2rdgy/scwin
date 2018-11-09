#pragma once

namespace scwin
{

	template<class _Ty = void>
	struct dtoless
	{	// functor for operator<
		typedef _Ty first_argument_type;
		typedef _Ty second_argument_type;
		typedef bool result_type;

		constexpr bool operator()(const _Ty& _Left, const _Ty& _Right) const
		{	// apply operator< to operands
			return (_Left->Id < _Right->Id);
		}
	};

}