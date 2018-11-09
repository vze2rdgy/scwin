#pragma once

namespace scwin
{
	template<typename T>
	class ScopeComplete
	{
	public:
		typedef std::function<void(T& t)> UnscopeHandler;

	private:
		T& scoped;
		std::function<void(T& t)> h;

	public:
		ScopeComplete(T& t, UnscopeHandler handler) : scoped(t), h(handler)
		{
		}
		~ScopeComplete()
		{
			h(scoped);
		}
	};

}

