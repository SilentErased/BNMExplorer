cmake_minimum_required(VERSION 3.8...3.26)

project(BNM)

# Add dependents to liblog.so
set(BNM_LINK_LOG OFF)

option(BNM_TEST_HOOKING "Build BNM with test/stub hooking" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PUBLIC_INCLUDE_DIRS
        include
        external/include
)

set(INCLUDE_DIRS
        include
        external/include
        external
        external/utf8
        src/private
)

add_library(
        BNM
        STATIC
        src/Class.cpp
        src/ClassesManagement.cpp
        src/Coroutine.cpp
        src/Defaults.cpp
        src/Delegates.cpp
        src/EventBase.cpp
        src/Exceptions.cpp
        src/FieldBase.cpp
        src/Hooks.cpp
        src/Image.cpp
        src/Internals.cpp
        src/Loading.cpp
        src/MethodBase.cpp
        src/MonoStructures.cpp
        src/PropertyBase.cpp
        src/UnityStructures.cpp
        src/Utils.cpp
)

add_library(BNM::BNM ALIAS BNM)

target_compile_features(BNM PUBLIC cxx_std_20)

target_include_directories(
        BNM
        PUBLIC
        ${INCLUDE_DIRS}
)

if (DEFINED BNM_CONFIG_INCLUDE_DIRS)
    target_include_directories(
            BNM
            BEFORE
            PUBLIC
            ${BNM_CONFIG_INCLUDE_DIRS}
    )
    list(INSERT PUBLIC_INCLUDE_DIRS 0 ${BNM_CONFIG_INCLUDE_DIRS})
endif ()

if (${BNM_LINK_LOG})
    target_link_libraries(
            BNM
            PUBLIC
            log
    )
endif ()

if (BNM_TEST_HOOKING)
    target_compile_definitions(BNM PUBLIC BNM_TEST_HOOKING)
endif ()

set_property(TARGET BNM PROPERTY BNM_INCLUDE_DIRECTORIES ${PUBLIC_INCLUDE_DIRS})
