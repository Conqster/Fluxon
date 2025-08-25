cmake_minimum_required(VERSION 3.15)
project(Fluxon)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Path to Vulkan SDK
set (VULKAN_SDK "C:/VulkanSDK/1.4.313.2")
set (GLSLC "${VULKAN_SDK}/Bin/glslc.exe")

include_directories(
	${VULKAN_SDK}/Include
	Libraries/assimp/include
	Libraries/glfw/include
	Libraries/GLM
	Libraries/imgui
	Libraries/stb_image
	Libraries
	
	${PROJECT_SOURCE_DIR}
	${PROJECT_SOURCE_DIR}/src
)


link_directories(
	${VULKAN_SDK}/Lib
	Libraries/assimp/lib
	Libraries/GLM
	Libraries/glfw/lib-vc2022
)


#create list of cmake helper 
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/vendor_cmake)
#Libraries folder char name
set(PROJECT_LIB_FLODER_NAME "Libraries")
#add YAML 
option(USE_YAML "Use YAML in project for serialisatin" ON)
if(USE_YAML)
	find_package(YAML REQUIRED)
	
	#file(GLOB_RECURSE YAML_SRC "Libraries/yaml-cpp-installed/include/yaml-cpp/*.h" "Libraries/yaml-cpp-installed/include/yaml-cpp/*.cpp")

endif()


#Source files 
file(GLOB_RECURSE SRC_FILES src/*.cpp src/*.h)
file(GLOB LIB_FILES "Libraries/imgui/*.h" "Libraries/imgui/*.c" "Libraries/imgui/*.cpp")
	
#if(USE_YAML)
#	add_executable(${PROJECT_NAME} ${SRC_FILES} ${LIB_FILES} ${YAML_SRC})
#else()
	add_executable(${PROJECT_NAME} ${SRC_FILES} ${LIB_FILES})
#endif()

#Vulkan & core libraries
target_link_libraries(${PROJECT_NAME}
	glfw3.lib
	assimp-vc143-mtd.lib
	vulkan-1.lib
)



if(USE_YAML)
	target_include_directories(${PROJECT_NAME} PUBLIC ${YAML_ALL_INCLUDES_DIR})
	#YAML_LIBRARIES => includes both debug & Release libraries
	#target_link_directories(${PROJECT_NAME} PUBLIC ${YAML_LIBRARIES})
	#wrong directory use below
	target_link_directories(${PROJECT_NAME} PUBLIC 
												  optimized ${YAML_RELEASE_DIR}/lib
												  debug ${YAML_DEBUG_DIR}/lib)
	message(STATUS "Successfully found libraries: ${YAML_LIBRARIES_BY_NAME}")
	
	#Preprocessor definations
	target_compile_definitions(${PROJECT_NAME} PRIVATE YAML_CPP_STATIC_DEFINE)
	
	
	target_link_libraries(${PROJECT_NAME} ${YAML_LIBRARIES_BY_NAME})
endif()

#working directory in VS 
set_target_properties(${PROJECT_NAME} PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"#/bin"
)


##Builds configurations 
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
	COMMAND start "" cmd /c call "${CMAKE_SOURCE_DIR}/assets/shaders/recompile_shader_to_spirv.bat"
)

#copy needed DLLs after build 
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	${PROJECT_SOURCE_DIR}/Libraries/assimp/bin/assimp-vc143-mtd.dll
	$<TARGET_FILE_DIR:${PROJECT_NAME}>
)



set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

#add_custom_target(RecompileShaders
#	COMMAND ${CMAKE_COMMAND} -E start "" cmd /c call "${CMAKE_SOURCE_DIR}/assets/shaders/recompile_shader_to_spirv.bat"
#	COMMENT "Recompiling shaders to SPIR-V"
#	)
#

#set (GLSLC C:/VulkanSDK/1.4.313.2/Bin/glslc.exe)


# ============================SHADER COMPILATION =================================
#could have a required shader generted in c++ to only update/generate shader spirv
#for shader needed 


#message(STATUS " ")
#message(STATUS " ")
#message(STATUS "configuring shader files for compilation to SPIR-V")
#file(GLOB SHADERS "assets/shaders/*.vert" "assets/shaders/*.frag")
#foreach(SHADER ${SHADERS})
#	get_filename_component(FILE_NAME ${SHADER} NAME)
#	message(STATUS "including ${FILE_NAME} shader for compilation to SPIR-V")
#endforeach()
#
#set (SPIRV_OUTPUT_DIR ${CMAKE_BINARY_DIR}/assets/shaders/spirvs)
#file(MAKE_DIRECTORY ${SPIRV_OUTPUT_DIR})
#
#foreach(SHADER ${SHADERS})
#	get_filename_component(FILE_NAME ${SHADER} NAME)
#	set(SPIRV "${SPIRV_OUTPUT_DIR}/${FILE_NAME}.spv")
#	
#	set(ERR_LOG "${SPIRV_OUTPUT_DIR}/${FILE_NAME}.log")
#	
#	add_custom_command(
#		OUTPUT ${SPIRV}
#		COMMAND ${GLSLC} ${SHADER} -o ${SPIRV}
#		DEPENDS ${SHADER}
#		COMMENT "Compiling ${FILE_NAME} to SPIR-V"
#		
#		#OUTPUT ${SPIRV}
#		#COMMAND ${CMAKE_COMMAND} -E echo "Compiling ${FILE_NAME}..."
#		#COMMAND ${GLSLC} ${SHADER} -o ${SPIRV} 2> ${ERR_LOG}
#		#COMMAND start cmd /k "type ${ERR_LOG}"
#		#COMMAND ${CMAKE_COMMAND} -E cat ${ERR_LOG}
#		#COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red --bold "Check ${ERR_LOG} for compilation errors"
#		#DEPENDS ${SHADER}
#		#COMMENT "Compiling ${FILE_NAME} to SPIR-V"
#		#VERBATIM
#		
#	)
#	
#	list(APPEND SPIRV_OUTPUTS ${SPIRV})
#endforeach()
#
##Optional target just for building shaders
#add_custom_target(Shaders ALL DEPENDS ${SPIRV_OUTPUTS})
#add_dependencies(${PROJECT_NAME} Shaders)
#
#set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})




