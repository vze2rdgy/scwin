#pragma once

namespace scwin
{
	generic<typename T> delegate void Action(T obj);
	generic<typename T, typename R> delegate R Function(T obj);
}