cmake_minimum_required(VERSION 3.0)

#
# FindSDL2
# -------
#
# Locate SDL2 library
#
# Adapted from original FindSDL cmake script

# This defines an interface library "SDL2"
# which you can link to using
# target_link_library(program SDL2)

# Optional components known:  image, mixer
# Will define compiler definitions USE_SDL2_IMAGE and USE_SDL2_MIXER
# if used
# eg.  find_package(SDL2 2.0.0 REQUIRED COMPONENTS image)

# Install dll functions for windows:
#
# SDL2_INSTALL(DEST)
# SDL2_IMAGE_INSTALL(DEST)
# SDL2_MIXER_INSTALL(DEST)

#message(WARNING "===> CMAKE_PREFIX_PATH  ${CMAKE_PREFIX_PATH}")

# Find include path

find_path(SDL2_INCLUDE_DIR SDL.h PATH_SUFFIXES SDL2)

# SDL2 library

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(VC_LIB_PATH_SUFFIX lib/x64)
else()
	set(VC_LIB_PATH_SUFFIX lib/x86)
endif()

find_library(SDL2_LIBRARY
	NAMES SDL2
	PATH_SUFFIXES lib ${VC_LIB_PATH_SUFFIX}
)

find_library(SDL2_MAIN_LIBRARY
	NAMES SDL2main
	PATH_SUFFIXES lib
)


set(SDL_REQUIRED SDL2_LIBRARY SDL2_INCLUDE_DIR)

#Only find SDLMain if required
if(NOT APPLE)
	list(APPEND SDL_REQUIRED SDL2_MAIN_LIBRARY)
	set(USE_SDL_MAIN)
endif()


# Get version info

if(SDL2_INCLUDE_DIR AND EXISTS "${SDL2_INCLUDE_DIR}/SDL_version.h")
	file(STRINGS "${SDL2_INCLUDE_DIR}/SDL_version.h" SDL2_VERSION_MAJOR_LINE REGEX "^#define[ \t]+SDL_MAJOR_VERSION[ \t]+[0-9]+$")
	file(STRINGS "${SDL2_INCLUDE_DIR}/SDL_version.h" SDL2_VERSION_MINOR_LINE REGEX "^#define[ \t]+SDL_MINOR_VERSION[ \t]+[0-9]+$")
	file(STRINGS "${SDL2_INCLUDE_DIR}/SDL_version.h" SDL2_VERSION_PATCH_LINE REGEX "^#define[ \t]+SDL_PATCHLEVEL[ \t]+[0-9]+$")
	string(REGEX REPLACE "^#define[ \t]+SDL_MAJOR_VERSION[ \t]+([0-9]+)$" "\\1" SDL2_VERSION_MAJOR "${SDL2_VERSION_MAJOR_LINE}")
	string(REGEX REPLACE "^#define[ \t]+SDL_MINOR_VERSION[ \t]+([0-9]+)$" "\\1" SDL2_VERSION_MINOR "${SDL2_VERSION_MINOR_LINE}")
	string(REGEX REPLACE "^#define[ \t]+SDL_PATCHLEVEL[ \t]+([0-9]+)$" "\\1" SDL2_VERSION_PATCH "${SDL2_VERSION_PATCH_LINE}")
	set(SDL2_VERSION_STRING ${SDL2_VERSION_MAJOR}.${SDL2_VERSION_MINOR}.${SDL2_VERSION_PATCH})
	unset(SDL2_VERSION_MAJOR_LINE)
	unset(SDL2_VERSION_MINOR_LINE)
	unset(SDL2_VERSION_PATCH_LINE)
	unset(SDL2_VERSION_MAJOR)
	unset(SDL2_VERSION_MINOR)
	unset(SDL2_VERSION_PATCH)
endif()



#Create interface library for convenience

add_library(SDL2 INTERFACE)

target_include_directories(SDL2 SYSTEM INTERFACE ${SDL2_INCLUDE_DIR})


target_link_libraries(SDL2 INTERFACE ${SDL2_LIBRARY})


add_library(SDL2_MAIN INTERFACE)

if(MINGW)
	find_library(mingw32 NAMES mingw32)
	target_link_libraries(SDL2_MAIN INTERFACE mingw32)
endif()


#Link to main if required
if(USE_SDL_MAIN)
	target_link_libraries(SDL2_MAIN INTERFACE ${SDL2_MAIN_LIBRARY})
endif()


#Not sure if threads library is needed?
#find_package(Threads)
#target_link_libraries(sdl2 INTERFACE ${THREADS_LIBRARY})


#Optional SDL2 components
foreach(COMPONENT ${SDL2_FIND_COMPONENTS})
	#message("Component ${COMPONENT}")
	set(component_handled FALSE)

	# SDL_Image library
	if(${COMPONENT} STREQUAL "image")
		find_path(SDL2_IMAGE_INCLUDE_DIR SDL_image.h PATH_SUFFIXES SDL2)
		find_library(SDL2_IMAGE_LIBRARY NAMES SDL2_image PATH_SUFFIXES lib ${VC_LIB_PATH_SUFFIX})
		#message("SDL2_IMAGE_LIBRARY ${SDL2_IMAGE_LIBRARY}")

		target_link_libraries(SDL2 INTERFACE ${SDL2_IMAGE_LIBRARY})
		target_include_directories(SDL2 SYSTEM INTERFACE ${SDL2_IMAGE_INCLUDE_DIR})
		target_compile_definitions(SDL2 INTERFACE USE_SDL2_IMAGE)
		list(APPEND SDL_REQUIRED SDL2_IMAGE_LIBRARY)
		set(USE_SDL2_IMAGE YES)
		set(component_handled TRUE)
	endif()

	# SDL_Mixer library
	if(${COMPONENT} STREQUAL "mixer")
		find_path(SDL2_MIXER_INCLUDE_DIR SDL_mixer.h PATH_SUFFIXES SDL2)
		find_library(SDL2_MIXER_LIBRARY NAMES SDL2_mixer PATH_SUFFIXES lib ${VC_LIB_PATH_SUFFIX})
		#message("SDL2_MIXER_LIBRARY ${SDL2_MIXER_LIBRARY}")

		target_link_libraries(SDL2 INTERFACE ${SDL2_MIXER_LIBRARY})
		target_include_directories(SDL2 SYSTEM INTERFACE ${SDL2_MIXER_INCLUDE_DIR})
		target_compile_definitions(SDL2 INTERFACE USE_SDL2_MIXER)
		list(APPEND SDL_REQUIRED SDL2_MIXER_LIBRARY)
		set(USE_SDL2_MIXER YES)
		set(component_handled TRUE)
	endif()

	# SDL_TTF library
	if(${COMPONENT} STREQUAL "ttf")
		find_path(SDL2_TTF_INCLUDE_DIR SDL_ttf.h PATH_SUFFIXES SDL2)
		find_library(SDL2_TTF_LIBRARY NAMES SDL2_ttf PATH_SUFFIXES lib ${VC_LIB_PATH_SUFFIX})

		target_link_libraries(SDL2 INTERFACE ${SDL2_TTF_LIBRARY})
		target_include_directories(SDL2 SYSTEM INTERFACE ${SDL2_TTF_INCLUDE_DIR})
		target_compile_definitions(SDL2 INTERFACE USE_SDL2_TTF)
		list(APPEND SDL_REQUIRED SDL2_TTF_LIBRARY)
		set(USE_SDL2_TTF YES)
		set(component_handled TRUE)
	endif()


	#Sub components of SDL_Image and SDL_Mixer (Used in DLL finding below)
	foreach(item png jpeg tiff webp flac ogg mod mp3)
		if(${COMPONENT} STREQUAL "${item}")
			string(TOUPPER "${item}" ITEM)
			target_compile_definitions(SDL2 INTERFACE USE_${ITEM})
			set(USE_${ITEM} YES)
			set(component_handled TRUE)
		endif()
	endforeach()
	unset(item)
	unset(ITEM)


	if(NOT component_handled)
		message(FATAL_ERROR "SDL2: Unknown component '${COMPONENT}'.")
	endif()
	unset(component_handled)

endforeach()


find_package(PackageHandleStandardArgs)
find_package_handle_standard_args(SDL2 REQUIRED_VARS ${SDL_REQUIRED} VERSION_VAR SDL2_VERSION_STRING)



#Installs dll for windows in given DEST directory, under COMPONENT dll
function(SDL2_INSTALL DEST)
	if(WIN32)
		find_file(SDL2_DLL NAMES SDL2.dll PATH_SUFFIXES bin)
		message(STATUS "found: ${SDL2_DLL}")
		install(FILES ${SDL2_DLL} DESTINATION ${DEST} COMPONENT dll)

	endif(WIN32)

	if(USE_SDL2_IMAGE)
		SDL2_IMAGE_INSTALL(${DEST})
	endif()

	if(USE_SDL2_MIXER)
		SDL2_MIXER_INSTALL(${DEST})
	endif()

	if(USE_SDL2_TTF)
		SDL2_TTF_INSTALL(${DEST})
	endif()

endfunction()


#Installs SDL2_image.dll and the PNG dlls.
function(SDL2_IMAGE_INSTALL DEST)
	if(WIN32)

		# add any variants of dlls to these lists (it will not break if they are not found)
		list(APPEND dllnames SDL2_image.dll)

		# Uncomment these if you need png, jpeg, tiff or webp support
		if(USE_PNG)
			list(APPEND dllnames libpng16-16.dll zlib1.dll)
		endif()

		if(USE_JPEG)
			list(APPEND dllnames libjpeg-9.dll)
		endif()

		if(USE_TIFF)
			list(APPEND dllnames libtiff-5.dll)
		endif()

		if(USE_WEBP)
			list(APPEND dllnames libwebp-4.dll)
		endif()

		foreach(dll ${dllnames})
			find_file(DLL_FILE NAMES ${dll} PATH_SUFFIXES bin)

			if(DLL_FILE)
				message(STATUS "found: ${DLL_FILE}")
				install(FILES ${DLL_FILE} DESTINATION ${DEST} COMPONENT dll)
			endif()
			unset(DLL_FILE CACHE) #so much for contained scope inside a function
		endforeach()

	endif(WIN32)
endfunction()



#Installs SDL2_mixer.dll and the PNG dlls.
function(SDL2_MIXER_INSTALL DEST)
	if(WIN32)

		# add any variants of dlls to these lists (it will not break if they are not found)
		list(APPEND dllnames SDL2_mixer.dll)

		if(USE_FLAC)
			list(APPEND dllnames libFLAC-8.dll)
		endif()

		if(USE_OGG)
			list(APPEND dllnames libogg-0.dll libvorbis-0.dll libvorbisfile-3.dll)
		endif()

		if(USE_MOD)
			list(APPEND dllnames libmikmod-2.dll libmodplug-1.dll)
		endif()

		if(USE_MP3)
			list(APPEND dllnames smpeg2.dll)
		endif()

		foreach(dll ${dllnames})
			find_file(DLL_FILE NAMES ${dll} PATH_SUFFIXES bin)

			if(DLL_FILE)
				message(STATUS "found: ${DLL_FILE}")
				install(FILES ${DLL_FILE} DESTINATION ${DEST} COMPONENT dll)
			endif()
			unset(DLL_FILE CACHE) #so much for contained scope inside a function
		endforeach()

	endif(WIN32)
endfunction()



#Installs SDL2_ttf.dll
function(SDL2_TTF_INSTALL DEST)
	if(WIN32)
		# add any variants of dlls to these lists (it will not break if they are not found)
		list(APPEND dllnames SDL2_ttf.dll)

		foreach(dll ${dllnames})
			find_file(DLL_FILE NAMES ${dll} PATH_SUFFIXES bin)

			if(DLL_FILE)
				message(STATUS "found: ${DLL_FILE}")
				install(FILES ${DLL_FILE} DESTINATION ${DEST} COMPONENT dll)
			endif()
			unset(DLL_FILE CACHE) #so much for contained scope inside a function
		endforeach()

	endif(WIN32)
endfunction()


#setup includes and definitions for use in current cmakelist folder
function(USE_SDL2)
	include_directories(SYSTEM ${SDL2_INCLUDE_DIR})

	if(APPLE)
		#Add framework path for #include <SDL2/SDL2xxx.h> style lookups inside SDL2Image etc.
		find_path(SDL2_FRAMEWORK_DIR NAMES SDL2.framework)
		add_definitions(-F ${SDL2_FRAMEWORK_DIR})
	endif()


	if(USE_SDL2_IMAGE)
		include_directories(SYSTEM ${SDL2_IMAGE_INCLUDE_DIR})
		add_definitions(-DUSE_SDL2_IMAGE)
	endif()

	if(USE_SDL2_MIXER)
		include_directories(SYSTEM ${SDL2_MIXER_INCLUDE_DIR})
		add_definitions(-DUSE_SDL2_MIXER)
	endif()

	if(USE_SDL2_TTF)
		include_directories(SYSTEM ${SDL2_TTF_INCLUDE_DIR})
		add_definitions(-DUSE_SDL2_TTF)
	endif()

endfunction()

