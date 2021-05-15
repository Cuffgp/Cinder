#pragma once

#include <memory>
#include <string>
#include <sstream>

#define CN_DEBUG

#ifdef CN_DEBUG
#define CN_ENABLE_ASSERTS
#endif

#ifdef CN_ENABLE_ASSERTS
#define CN_ASSERT(x, ...) {if(!(x)) {CN_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define CN_CORE_ASSERT(x, ...) {if(!(x)) {CN_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define CN_ASSERT(x, ...)
#define CN_CORE_ASSERT(x, ...)
#endif

namespace Cinder {

	// Unique and Shared pointsers to Scope<> and Ref<>
	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}

#include "Cinder/Core/Log.h"

#define BIT(x) (1 << x)
#define CN_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
