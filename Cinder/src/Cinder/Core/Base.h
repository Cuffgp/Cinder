#pragma once

#include <memory>
#include <string>
#include <sstream>

#define CN_EXPAND_VARGS(x) x

#define CN_DEBUG

#ifdef CN_DEBUG
#define CN_ENABLE_ASSERTS
#endif

#ifdef CN_ENABLE_ASSERTS
#define CN_ASSERT_NO_MESSAGE(condition) { if(!(condition)) { CN_ERROR("Assertion Failed"); __debugbreak(); } }
#define CN_ASSERT_MESSAGE(condition, ...) { if(!(condition)) { CN_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }

#define CN_ASSERT_RESOLVE(arg1, arg2, macro, ...) macro
#define CN_GET_ASSERT_MACRO(...) CN_EXPAND_VARGS(CN_ASSERT_RESOLVE(__VA_ARGS__, CN_ASSERT_MESSAGE, CN_ASSERT_NO_MESSAGE))

#define CN_ASSERT(...) CN_EXPAND_VARGS( CN_GET_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__) )
#define CN_CORE_ASSERT(...) CN_EXPAND_VARGS( CN_GET_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__) )
#else
#define CN_ASSERT(...)
#define CN_CORE_ASSERT(...)
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

	using byte = uint8_t;
}

#include "Cinder/Core/Log.h"

#define BIT(x) (1 << x)
#define CN_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
