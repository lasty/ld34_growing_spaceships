
set(LD34_TARGET_NAME ld34)

set(SOURCE_FILES
        main.cpp
)

add_executable(${LD34_TARGET_NAME} ${SOURCE_FILES})

target_link_libraries(${LD34_TARGET_NAME} SDL2Main SDL2)

