cmake_minimum_required(VERSION 3.8)
project(crawler)

set(CMAKE_CXX_STANDARD 17)

#set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG -Wall -fsanitize=undefined -fsanitize=address -fsanitize=leak -fsanitize=integer-divide-by-zero -fsanitize=null -fsanitize=signed-integer-overflow -fsanitize=bounds -fstack-protector")
#set(CMAKE_C_FLAGS_DEBUG "-g -Wall -fsanitize=leak -fsanitize=integer-divide-by-zero -fsanitize=null -fsanitize=signed-integer-overflow -fsanitize=bounds -fstack-protector")
#set(CMAKE_C_FLAGS_DEBUG "-g -Wall -fsanitize=leak")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")
#set(CMAKE_CXX_FLAGS_DEBUG "-O3 -Wall -funroll-loops -fomit-frame-pointer -ftree-loop-distribution -ftree-vectorize")
# uncomment this to compile as gui app (doesnt open the console)
#set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")

if(APPLE)
set(CMAKE_EXE_LINKER_FLAGS "-L/opt/homebrew/Cellar/sdl2/2.26.2/lib -lSDL2")
#    set(CMAKE_CXX_FLAGS "-framework OpenGL")
# -framework OpenGL -framework CoreVideo
#    add_compile_definitions(GL_SILENCE_DEPRECATION)
endif()

# includes cmake/FindSDL2.cmake
#set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if(UNIX AND NOT APPLE)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
    set(CMAKE_EXE_LINKER_FLAGS "-Wl,--copy-dt-needed-entries")
endif()
#if(WIN32)
#endif()


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "../")
find_package(ZLIB REQUIRED)
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(Freetype REQUIRED)
include_directories(${SDL2_INCLUDE_DIR} ${SDL2_MIXER_INCLUDE_DIRS} ${GLM_INCLUDE_DIR} ${FREETYPE_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/src/include ${ZLIB_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/src/imgui)
#message(0, ${SDL2_MIXER_LIBRARIES})

set(IM_GUI_SOURCE_FILES
        src/imgui/imgui_impl_opengl3.cpp
        src/imgui/imgui_impl_sdl.cpp
        src/imgui/imgui.cpp
        src/imgui/imgui_demo.cpp
        src/imgui/imgui_draw.cpp
        src/imgui/imgui_widgets.cpp
        src/imgui/imgui_tables.cpp
        src/imgui/imgui_stdlib.cpp)

set(UTIL_SOURCE_FILES
        src/util/string_util.cpp
        src/util/string_util.h
        src/include/stb_rect_pack.h
        src/include/tiny_obj_loader.h
        src/include/earcut.h
        src/include/json.hpp
        src/include/Vector2.h
        src/util/lodepng.h
        src/util/lodepng.c
        src/util/math_util.c
        src/util/math_util.h
        src/util/ImGuiFileDialog.h
        src/util/ImGuiFileDialog.cpp
        src/include/SparseVector.h)

set(ENGINE_SOURCE_FILES
        src/include/defs.h
        src/main.cpp
        src/glad/glad.c
        src/renderer/PixelBuffer.cpp
        src/renderer/PixelBuffer.h
        src/input/mapped_input.h
        src/input/Input.cpp
        src/input/Input.h
        src/input/SDLInput.cpp
        src/input/SDLInput.h
        src/renderer/RenderBuffer.cpp
        src/renderer/RenderBuffer.h
        src/renderer/ShaderProgram.cpp
        src/renderer/ShaderProgram.h
        src/renderer/VertexBuffer.cpp
        src/renderer/VertexBuffer.h
        src/renderer/FrameBuffer.cpp
        src/renderer/FrameBuffer.h
        src/renderer/Texture.cpp
        src/renderer/Texture.h
        src/renderer/TextureAtlas.cpp
        src/renderer/TextureAtlas.h
        src/renderer/Viewport.cpp
        src/renderer/Viewport.h
        src/renderer/Font.cpp
        src/renderer/Font.h
        src/renderer/Renderer.cpp
        src/renderer/Renderer.h
        src/renderer/LevelRenderer.cpp
        src/renderer/LevelRenderer.h
        src/renderer/Camera.h
        src/renderer/Model.cpp
        src/renderer/Model.h
)

set(GAME_SOURCE_FILES
        src/game/Game.cpp
        src/game/Game.h
        src/game/Database.cpp
        src/game/Database.h
        src/game/PolyUtil.h
        src/game/PolyUtil.cpp
        src/game/animation/Animations.cpp
        src/game/animation/Animations.h
        src/game/level/Level.cpp
        src/game/level/Level.h
        src/game/level/Lighting.cpp
        src/game/level/Lighting.h
)

add_executable(game ${GAME_SOURCE_FILES} ${ENGINE_SOURCE_FILES} ${UTIL_SOURCE_FILES} ${IM_GUI_SOURCE_FILES})

target_link_libraries(game m ${CMAKE_DL_LIBS} ${OPENGL_LIBRARIES} ${SDL2_LIBRARY} ${SDL2_MIXER_LIBRARIES} ${FREETYPE_LIBRARIES} ${ZLIB_LIBRARIES})