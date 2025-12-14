cmake_minimum_required(VERSION 3.22.1)

add_definitions(-DPLATFORM_ANDROID -D__ANDROID__)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)

project("${APP_LIB_NAME}")

# Сначала определим корректный путь к raylib
set(RAYLIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/raylib)

# ★ 1. RAYLIB - ПРОВЕРЯЕМ СУЩЕСТВОВАНИЕ ФАЙЛОВ ★
# Основные файлы raylib для Android
set(RAYLIB_CORE_SOURCES
        ${RAYLIB_DIR}/src/rcore.c
        ${RAYLIB_DIR}/src/rshapes.c
        ${RAYLIB_DIR}/src/rtextures.c
        ${RAYLIB_DIR}/src/rtext.c
        ${RAYLIB_DIR}/src/rmodels.c
        ${RAYLIB_DIR}/src/raudio.c
        ${RAYLIB_DIR}/src/rlgl.c
)

# Проверяем существование файлов
foreach(source_file ${RAYLIB_CORE_SOURCES})
    if(NOT EXISTS ${source_file})
        message(WARNING "File not found: ${source_file}")
        # Пробуем альтернативные пути
        string(REPLACE "/rcore.c" "/core.c" alt_file ${source_file})
        if(EXISTS ${alt_file})
            list(APPEND RAYLIB_SOURCES_ALT ${alt_file})
            message(STATUS "Using alternative: ${alt_file}")
        endif()
    else()
        list(APPEND RAYLIB_SOURCES_VALID ${source_file})
    endif()
endforeach()

if(RAYLIB_SOURCES_VALID)
    set(RAYLIB_SOURCES ${RAYLIB_SOURCES_VALID})
elseif(RAYLIB_SOURCES_ALT)
    set(RAYLIB_SOURCES ${RAYLIB_SOURCES_ALT})
else()
    message(FATAL_ERROR "No raylib source files found! Check path: ${RAYLIB_DIR}")
endif()

message(STATUS "Found raylib sources: ${RAYLIB_SOURCES}")

# Android-specific файлы
set(RAYLIB_ANDROID_SOURCES
        ${RAYLIB_DIR}/src/randroid.c
)

foreach(android_file ${RAYLIB_ANDROID_SOURCES})
    if(EXISTS ${android_file})
        list(APPEND RAYLIB_SOURCES ${android_file})
        message(STATUS "Adding Android file: ${android_file}")
    endif()
endforeach()

add_library(raylib STATIC ${RAYLIB_SOURCES})

target_compile_definitions(raylib PRIVATE
        PLATFORM_ANDROID
        __ANDROID__
        _LINUX  # Да, Android считается вариантом Linux
        _POSIX
        GRAPHICS_API_OPENGL_ES2
)

target_include_directories(raylib PUBLIC
        ${RAYLIB_DIR}/src
        ${RAYLIB_DIR}/src/external
        ${RAYLIB_DIR}/src/external/glfw/include
        ${ANDROID_NDK}/sources/android/native_app_glue
)

# ★ 2. CHIPMUNK2D ★
set(CHIPMUNK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/chipmunk2d)
if(NOT EXISTS ${CHIPMUNK_DIR})
    message(FATAL_ERROR "Chipmunk2D not found at: ${CHIPMUNK_DIR}")
endif()

file(GLOB CHIPMUNK_SOURCES
        "${CHIPMUNK_DIR}/src/*.c"
)
add_library(chipmunk STATIC ${CHIPMUNK_SOURCES})
target_include_directories(chipmunk PUBLIC ${CHIPMUNK_DIR}/include)

# ★ 3. RAYGUI ★
set(RAYGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/raygui)
if(NOT EXISTS ${RAYGUI_DIR})
    message(FATAL_ERROR "RayGUI not found at: ${RAYGUI_DIR}")
endif()

add_library(raygui INTERFACE)
target_include_directories(raygui INTERFACE ${RAYGUI_DIR}/src)

# ★ 4. JSON ★
set(JSON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/json)
if(NOT EXISTS ${JSON_DIR})
    message(FATAL_ERROR "JSON library not found at: ${JSON_DIR}")
endif()

add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json INTERFACE ${JSON_DIR}/single_include)

# ★ 5. ВАШИ ФАЙЛЫ ★
set(SOURCES
        main.cpp containers.cpp effects.cpp game.cpp menu_creator.cpp
        menu_templates.cpp objects.cpp spaceBackground.cpp units.cpp world.cpp
)

# Проверяем существование ваших исходных файлов
foreach(src_file ${SOURCES})
    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${src_file})
        message(WARNING "Source file not found: ${src_file}")
    endif()
endforeach()

list(APPEND SOURCES
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
)

add_library(${APP_LIB_NAME} SHARED ${SOURCES})

target_compile_definitions(${APP_LIB_NAME} PRIVATE
        PLATFORM_ANDROID
        GRAPHICS_API_OPENGL_ES2
        SUPPORT_GESTURES_SYSTEM
        MA_NO_RESOURCE_MANAGER
        MA_NO_NODE_GRAPH
        MA_NO_GENERATION
        MA_SUPPORT_AAUDIO
        MA_ENABLE_AAUDIO
)

# ★ 6. INCLUDE ПУТИ ★
target_include_directories(${APP_LIB_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${ANDROID_NDK}/sources/android/native_app_glue
        ${RAYLIB_DIR}/src
        ${RAYLIB_DIR}/src/external
        ${RAYGUI_DIR}/src
        ${CHIPMUNK_DIR}/include
        ${JSON_DIR}/single_include
)

# ★ 7. ЛИНКОВКА ★
target_link_libraries(${APP_LIB_NAME}
        raylib
        chipmunk
        raygui
        nlohmann_json
        log android EGL GLESv2 OpenSLES atomic m
)

# ★ 8. ДОПОЛНИТЕЛЬНЫЕ НАСТРОЙКИ ДЛЯ ANDROID ★
set_target_properties(${APP_LIB_NAME} PROPERTIES
        CXX_EXTENSIONS OFF
        POSITION_INDEPENDENT_CODE TRUE
)

# Debug
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    target_compile_definitions(${APP_LIB_NAME} PRIVATE _DEBUG DEBUG)
    target_compile_definitions(raylib PRIVATE _DEBUG DEBUG)
endif()