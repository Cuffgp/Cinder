#pragma once

#include "Cinder/Core/Base.h"

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)


namespace Cinder {

	class Log
	{
	public:
		static void Init();

		static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static Ref<spdlog::logger> s_CoreLogger;
		static Ref<spdlog::logger> s_ClientLogger;
	};

}

// Core log macros
#define CN_CORE_TRACE(...)    ::Cinder::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define CN_CORE_INFO(...)     ::Cinder::Log::GetCoreLogger()->info(__VA_ARGS__)
#define CN_CORE_WARN(...)     ::Cinder::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define CN_CORE_ERROR(...)    ::Cinder::Log::GetCoreLogger()->error(__VA_ARGS__)
#define CN_CORE_CRITICAL(...) ::Cinder::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define CN_TRACE(...)         ::Cinder::Log::GetClientLogger()->trace(__VA_ARGS__)
#define CN_INFO(...)          ::Cinder::Log::GetClientLogger()->info(__VA_ARGS__)
#define CN_WARN(...)          ::Cinder::Log::GetClientLogger()->warn(__VA_ARGS__)
#define CN_ERROR(...)         ::Cinder::Log::GetClientLogger()->error(__VA_ARGS__)
#define CN_CRITICAL(...)      ::Cinder::Log::GetClientLogger()->critical(__VA_ARGS__)
