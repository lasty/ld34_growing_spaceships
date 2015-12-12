cmake_minimum_required(VERSION 3.0)

#Make an interface library for GLM (OpenGl Mathematics Library)

find_path(GLM_INCLUDE_DIR NAMES glm/glm.hpp)

add_library(GLM INTERFACE)

target_include_directories(GLM SYSTEM INTERFACE ${GLM_INCLUDE_DIR})

target_compile_definitions(GLM INTERFACE GLM_FORCE_PURE) # Disables SIMD detection


# Find version string
unset(GLM_VERSION_STRING)
foreach(VPART MAJOR MINOR PATCH REVISION)
	file(STRINGS "${GLM_INCLUDE_DIR}/glm/detail/setup.hpp" VERSION_LINE REGEX "^#define[ \t]+GLM_VERSION_${VPART}[ \t]+[0-9]+$")
	string(REGEX REPLACE "^#define[ \t]+GLM_VERSION_${VPART}[ \t]+([0-9]+)$" "\\1" VERSION_LINE ${VERSION_LINE})
	set(GLM_VERSION_STRING "${GLM_VERSION_STRING}${SEP}${VERSION_LINE}")
	set(SEP .)
endforeach()
unset(VERSION_LINE)
unset(SEP)

#message(STATUS "GLM_VERSION_STRING ${GLM_VERSION_STRING}")


# Silence the warning about radians in versions 0.9.5.x
if(${GLM_VERSION_STRING} VERSION_LESS 0.9.6.0)
	message("GLM: Forcing Radians, please update to 0.9.6.x or higher")
	target_compile_definitions(GLM INTERFACE GLM_FORCE_RADIANS)
endif()



find_package(PackageHandleStandardArgs)
find_package_handle_standard_args(GLM REQUIRED_VARS GLM_INCLUDE_DIR VERSION_VAR GLM_VERSION_STRING)


function(USE_GLM)
	include_directories(SYSTEM ${GLM_INCLUDE_DIR})
	add_definitions(-DGLM_FORCE_PURE)
endfunction()
