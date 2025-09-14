#pragma once

#include "Core/AssertLog.h"


//TO-DO(Conqster): Need to have a return 
#define FLUX_VK_ASSERT_SUCCESS(func, info) \
	FLUX_DEBUG_ASSERT_CRITICAL(func == VK_SUCCESS, info)
