#pragma once
#include <iostream>

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