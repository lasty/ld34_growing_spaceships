
set(LD34_TARGET_NAME ld34)

set(SOURCE_FILES
        main.cpp
        globals.h
        game.cpp game.h
        render.cpp render.h
        sprite.cpp sprite.h
        surface.cpp surface.h
        camera.cpp camera.h
        assets.cpp assets.h
        ship.cpp ship.h
        part.cpp part.h
        transform.cpp transform.h
        text.cpp text.h
        font.cpp font.h
        hud.cpp hud.h
        tractorbeam.cpp tractorbeam.h
        projectile.cpp projectile.h)

add_executable(${LD34_TARGET_NAME} ${SOURCE_FILES})

target_link_libraries(${LD34_TARGET_NAME} SDL2Main SDL2)

