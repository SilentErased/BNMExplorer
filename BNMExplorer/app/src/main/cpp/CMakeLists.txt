cmake_minimum_required(VERSION 3.8...3.26)

project("BNMExplorer")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

macro(SET_OPTION option value)
    set(${option} ${value} CACHE INTERNAL "" FORCE)
endmacro()
SET_OPTION(DOBBY_DEBUG OFF)
SET_OPTION(DOBBY_GENERATE_SHARED OFF)
add_subdirectory(extern/Dobby dobby)

set(PUBLIC_INCLUDE_DIRS
        extern/BNM-Android/include
        extern/BNM-Android/external/include
)

set(INCLUDE_DIRS
        extern/BNM-Android/include
        extern/BNM-Android/external/include
        extern/BNM-Android/external
        extern/BNM-Android/external/utf8
        extern/BNM-Android/src/private
        extern/Dobby/include
)

add_library(
        BNM
        STATIC
        extern/BNM-Android/src/Class.cpp
        extern/BNM-Android/src/ClassesManagement.cpp
        extern/BNM-Android/src/Coroutine.cpp
        extern/BNM-Android/src/Defaults.cpp
        extern/BNM-Android/src/Delegates.cpp
        extern/BNM-Android/src/EventBase.cpp
        extern/BNM-Android/src/Exceptions.cpp
        extern/BNM-Android/src/FieldBase.cpp
        extern/BNM-Android/src/Hooks.cpp
        extern/BNM-Android/src/Image.cpp
        extern/BNM-Android/src/Internals.cpp
        extern/BNM-Android/src/Loading.cpp
        extern/BNM-Android/src/MethodBase.cpp
        extern/BNM-Android/src/MonoStructures.cpp
        extern/BNM-Android/src/PropertyBase.cpp
        extern/BNM-Android/src/UnityStructures.cpp
        extern/BNM-Android/src/Utils.cpp
)

target_include_directories(
        BNM
        PUBLIC
        ${INCLUDE_DIRS}
)

set_property(TARGET BNM PROPERTY BNM_INCLUDE_DIRECTORIES ${PUBLIC_INCLUDE_DIRS})


add_library(${CMAKE_PROJECT_NAME}
        SHARED
        native-lib.cpp
)

target_include_directories(
        ${CMAKE_PROJECT_NAME}
        PUBLIC
        ${PUBLIC_INCLUDE_DIRS}
        Include
        explorer
        ${CMAKE_CURRENT_SOURCE_DIR}/extern/OpenSSL/${ANDROID_ABI}/include
)

target_link_libraries(
        ${CMAKE_PROJECT_NAME}
        PUBLIC
        android
        log
        BNM
        dobby
        ${CMAKE_CURRENT_SOURCE_DIR}/extern/OpenSSL/${ANDROID_ABI}/lib/libcrypto.a
        ${CMAKE_CURRENT_SOURCE_DIR}/extern/OpenSSL/${ANDROID_ABI}/lib/libssl.a
)