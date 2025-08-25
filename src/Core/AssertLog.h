#pragma once
#include <iostream>
#include "Core.h"

#define ANSI_RED_COLOUR "\033[1;31m"	   //4
#define ANSI_GREEN_COLOUR "\033[1;32m"	   //2
#define ANSI_YELLOW_COLOUR "\033[1;33m"	   //3
#define ANSI_BLUE_COLOUR "\033[1;34m"      //1
#define ANSI_RESET_COLOUR "\033[0m"

constexpr const char* GetSeverityLvlANSIColour(int lvl = 0)
{
	switch (lvl)
	{
	case 0U:
		return ANSI_RESET_COLOUR;
	case 1U:
		return ANSI_BLUE_COLOUR;		//1
	case 2U:
		return ANSI_GREEN_COLOUR;	    //2
	case 3U:
		return ANSI_YELLOW_COLOUR;	    //3
	case 4U:
		return ANSI_RED_COLOUR; 	    //4
	}

	return ANSI_RESET_COLOUR;
}

constexpr const char* GetAssertName(int type)
{
	return (type == 4) ? "CRITICAL" : "WARNING";
}

template<typename... Args>
[[maybe_unused]] inline static void AssertOutput(const char* expr, int lvl, const char* file, unsigned line, Args&&... msg_args)
{
	auto& out = std::cout;
	out << GetSeverityLvlANSIColour(lvl) << "[" << GetAssertName(lvl) << "]: " << ANSI_RESET_COLOUR;

	out << "Assertion failed (" << expr << ").";

	((out << std::forward<Args>(msg_args)), ...);

	out << ". FILE: " << file
		<< ", LINE: " << line << std::endl;

}

#define FLUX_INTERNAL_ASSERT_IMPL(expr, lvl, ...) \
			do { \
				if(!(expr)) { \
					AssertOutput(#expr, lvl, __FILE__, __LINE__, __VA_ARGS__); \
					if(lvl > 1) __debugbreak(); \
				} \
			}while(0)

#define FLUX_DEBUG_ASSERT(expr, ...) FLUX_INTERNAL_ASSERT_IMPL(expr, 1, __VA_ARGS__)
#define FLUX_DEBUG_ASSERT_CRITICAL(expr, ...) FLUX_INTERNAL_ASSERT_IMPL(expr, 4, __VA_ARGS__)



//Logs 
//implementations
// 
// 

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

namespace Fluxon {
	namespace Debug {

		enum class SEVERITY : u8
		{
			UNKNOWN = 0,
			INFO,        //<----- Low as for basic logging infomation like "Player pos: {0.0, 0.0, 0.0}, Debug::Log("Position: ", pos);
			STATUS,		 //<----- Low/Medium for logging program status like "Loading Texture ID: 0", Debug::LogStatus("Loading Texture ID: ", 42);
			WARNING,	 //<----- Medium/High (experince performance) for logging infomation "Player health: -100, Debug::LogWarning("player health: ", -100), Debug::LogWarning("High memory usage: ", 96.7f, "%"); 
			ERROR		 //<----- High (system performance) for logging error that affects sys, memeory, format, out of range, Debug::LogError("Shader failed to compile").
		};


		constexpr const char* SeverityToString(const SEVERITY& value)
		{
			switch (value)
			{
			case SEVERITY::UNKNOWN:
				return "Unknown";
			case SEVERITY::INFO:
				return "INFO";
			case SEVERITY::STATUS:
				return "STATUS";
			case SEVERITY::WARNING:
				return "WARNING";
			case SEVERITY::ERROR:
				return "ERROR";
			}

			return "[INVALID SEVERITY LOG]";
		}

		//detail are {Debug} internal used information
		namespace detail {

			template<glm::length_t L, typename T, glm::qualifier Q>
			inline std::ostream& operator<<(std::ostream& os, const glm::vec<L, T, Q>& vector)
			{
				return os << glm::to_string(vector);
			}

			inline void ConsoleLog(std::string& msg, const SEVERITY& severity = SEVERITY::UNKNOWN)
			{
#if FLUX_ENABLE_CONSOLE_LOGGING
				if constexpr (kFluxShowDebugSeverityLvl)
				{
					std::cout << GetSeverityLvlANSIColour((int)severity) << "[" << SeverityToString(severity) << "]: " << ANSI_RESET_COLOUR << msg << "\n";
				}
				else
					std::cout << msg << "\n";
				printf("[USE OLD CONSOLE LOG SYSTEM] size: %d, & .size(): %d.\n", int(sizeof(msg)), int(msg.size()));
#endif// FLUX_ENABLE_CONSOLE_LOGGING
			}


			template <typename A, typename... N>
			inline void ConsoleLog(const SEVERITY& severity, A&& a, N&&... n)
			{
				auto& out = std::cout;
				if constexpr (kFluxShowDebugSeverityLvl)
				{
					out << GetSeverityLvlANSIColour((int)severity)
						<< "[" << SeverityToString(severity) << "]: "
						<< ANSI_RESET_COLOUR;
				}
				out << std::forward<A>(a);
				((out << std::forward<N>(n)), ...);
				out << "\n";
			}


			constexpr bool ShouldLogSeverity(const Debug::SEVERITY& s)
			{
#if FLUX_ONLY_DEBUG_HIGH_SEVERITY_LOG
				return s >= Debug::SEVERITY::STATUS;
#else
				return true;
#endif // FLUX_ONLY_DEBUG_HIGH_SEVERITY_LOG
			}

		} // namespace detail



#if FLUX_ENABLE_CONSOLE_LOGGING

//implementation entry
#define FLUX_DEBUG_LOG_IMPL(type, ...) Debug::detail::ConsoleLog(type, __VA_ARGS__)
		constexpr bool ShouldLogSeverity(const Debug::SEVERITY& s);  //<--- forward function declare, see below for decleration
		//#define INTERNAL_CONSOLE_DEBUG_LOG_CHECK_IMPL(type, ...) \
		//		if constexpr (ShouldLogSeverity(type)) \
		//			Debug::detail::ConsoleLog(type, __VA_ARGS__)



#if FLUX_ONLY_DEBUG_HIGH_SEVERITY_LOG
#define FLUX_DEBUG_LOG(...)
#define FLUX_DEBUG_LOG_INFO(...)
#define FLUX_DEBUG_LOG_STATUS(...) FLUX_DEBUG_LOG_IMPL(Debug::SEVERITY::STATUS, __VA_ARGS__)
#define FLUX_DEBUG_LOG_WARNING(...) FLUX_DEBUG_LOG_IMPL(Debug::SEVERITY::WARNING, __VA_ARGS__)
#define FLUX_DEBUG_LOG_ERROR(...) FLUX_DEBUG_LOG_IMPL(Debug::SEVERITY::ERROR, __VA_ARGS__)

#else //FLUX_ONLY_DEBUG_HIGH_SEVERITY_LOG

#define FLUX_DEBUG_LOG(...) FLUX_DEBUG_LOG_IMPL(Debug::SEVERITY::INFO, __VA_ARGS__)
#define FLUX_DEBUG_LOG_INFO(...) FLUX_DEBUG_LOG_IMPL(Debug::SEVERITY::INFO, __VA_ARGS__)
#define FLUX_DEBUG_LOG_STATUS(...) FLUX_DEBUG_LOG_IMPL(Debug::SEVERITY::STATUS, __VA_ARGS__)
#define FLUX_DEBUG_LOG_WARNING(...) FLUX_DEBUG_LOG_IMPL(Debug::SEVERITY::WARNING, __VA_ARGS__)
#define FLUX_DEBUG_LOG_ERROR(...) FLUX_DEBUG_LOG_IMPL(Debug::SEVERITY::ERROR, __VA_ARGS__)

#endif // FLUX_ONLY_DEBUG_HIGH_SEVERITY_LOG

#else // FLUX_ENABLE_CONSOLE_LOGGING

//#define INTERNAL_CONSOLE_DEBUG_LOG_CHECK_IMPL(type, msg) (0) 
#define FLUX_DEBUG_LOG_IMPL(type, ...) (0)
#define FLUX_DEBUG_LOG(...) (0)
#define FLUX_DEBUG_LOG_INFO(...) (0) 
#define FLUX_DEBUG_LOG_STATUS(...) (0)
#define FLUX_DEBUG_LOG_WARNING(...) (0)
#define FLUX_DEBUG_LOG_ERROR(...) (0)

#endif // FLUX_ENABLE_CONSOLE_LOGGING


	} // namespace Debug
}//namespace Fluxon

