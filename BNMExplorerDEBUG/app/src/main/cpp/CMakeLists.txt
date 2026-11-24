cmake_minimum_required(VERSION 3.12...3.26)

project("BNMExplorer")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BNM_ENABLE_HTTPS "Enable HTTPS support in the embedded HTTP server" OFF)
set(BNM_OPENSSL_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/extern/OpenSSL/${ANDROID_ABI}" CACHE PATH "OpenSSL package for the selected Android ABI")

if(BNM_ENABLE_HTTPS)
    if(NOT EXISTS "${BNM_OPENSSL_ROOT}/include/openssl/ssl.h" OR NOT EXISTS "${BNM_OPENSSL_ROOT}/lib/libssl.a" OR NOT EXISTS "${BNM_OPENSSL_ROOT}/lib/libcrypto.a")
        message(FATAL_ERROR "BNM_ENABLE_HTTPS is ON, but OpenSSL headers or static libraries were not found under ${BNM_OPENSSL_ROOT}")
    endif()
endif()

macro(SET_OPTION option value)
    set(${option} ${value} CACHE INTERNAL "" FORCE)
endmacro()

SET_OPTION(DOBBY_DEBUG OFF)
SET_OPTION(DOBBY_GENERATE_SHARED OFF)
add_subdirectory(extern/Dobby dobby)

set(BNM_SOURCES
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

set(BNM_INCLUDE_DIRS
        extern/BNM-Android/include
        extern/BNM-Android/external/include
        extern/BNM-Android/external
        extern/BNM-Android/external/utf8
        extern/BNM-Android/src/private
        extern/Dobby/include
)

set(PUBLIC_INCLUDE_DIRS
        extern/BNM-Android/include
        extern/BNM-Android/external/include
)

set(LUA_SOURCES
        lua/lapi.c lua/lauxlib.c lua/lbaselib.c lua/lcode.c lua/ldebug.c
        lua/ldo.c lua/ldump.c lua/lfunc.c lua/lgc.c lua/llex.c lua/lmathlib.c
        lua/lmem.c lua/lobject.c lua/lopcodes.c lua/lparser.c lua/lstate.c
        lua/lstring.c lua/lstrlib.c lua/ltable.c lua/ltablib.c lua/ltm.c
        lua/lundump.c lua/lvm.c lua/lzio.c lua/linit.c
)

set_source_files_properties(${LUA_SOURCES} PROPERTIES LANGUAGE C)
set_source_files_properties(${LUA_SOURCES} PROPERTIES COMPILE_FLAGS "-U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0")

add_library(${CMAKE_PROJECT_NAME} SHARED
        unity_dispatch.cpp
        Include/autodetect/unity_version.cpp
        Include/autodetect/il2cpp_runtime.cpp
        ${LUA_SOURCES}
)

target_include_directories(${CMAKE_PROJECT_NAME}
        PUBLIC
        ${PUBLIC_INCLUDE_DIRS}
        Include
        explorer
        lua
)

function(add_bnm_variant variant unity_version unity_patch)
    set(bnm_target "BNM_${variant}")
    set(native_target "BNMNative_${variant}")
    set(namespace_name "BNM_${variant}")
    set(app_namespace "BNMExplorer_${variant}")
    set(entry_name "bnm_init_${variant}")

    add_library(${bnm_target} STATIC ${BNM_SOURCES})
    target_include_directories(${bnm_target} PUBLIC ${BNM_INCLUDE_DIRS})
    target_compile_definitions(${bnm_target} PRIVATE
            BNM=${namespace_name}
            UNITY_VER=${unity_version}
            UNITY_PATCH_VER=${unity_patch}
            BNM_TAG="BNMExplorer"
    )
    set_target_properties(${bnm_target} PROPERTIES POSITION_INDEPENDENT_CODE ON)

    add_library(${native_target} OBJECT native-lib.cpp)
    target_include_directories(${native_target} PRIVATE
            ${PUBLIC_INCLUDE_DIRS}
            ${BNM_INCLUDE_DIRS}
            Include
            explorer
            lua
    )
    if(BNM_ENABLE_HTTPS)
        target_include_directories(${native_target} PRIVATE ${BNM_OPENSSL_ROOT}/include)
        target_compile_definitions(${native_target} PRIVATE BNM_ENABLE_HTTPS)
    endif()
    target_compile_definitions(${native_target} PRIVATE
            BNM=${namespace_name}
            UNITY_VER=${unity_version}
            UNITY_PATCH_VER=${unity_patch}
            BNM_INIT_ENTRY=${entry_name}
            BNM_APP_NAMESPACE=${app_namespace}
            BNM_TAG="BNMExplorer"
    )
    set_target_properties(${native_target} PROPERTIES POSITION_INDEPENDENT_CODE ON)

    target_sources(${CMAKE_PROJECT_NAME} PRIVATE $<TARGET_OBJECTS:${native_target}>)
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${bnm_target})
endfunction()

add_bnm_variant(v56 56 0)
add_bnm_variant(v171 171 0)
add_bnm_variant(v172 172 0)
add_bnm_variant(v181 181 0)
add_bnm_variant(v182 182 0)
add_bnm_variant(v183 183 0)
add_bnm_variant(v191 191 0)
add_bnm_variant(v193 193 0)
add_bnm_variant(v194 194 0)
add_bnm_variant(v201 201 0)
add_bnm_variant(v202 202 0)
add_bnm_variant(v203 203 0)
add_bnm_variant(v211_pre24 211 0)
add_bnm_variant(v211_24 211 24)
add_bnm_variant(v212 212 0)
add_bnm_variant(v213 213 0)
add_bnm_variant(v221 221 0)
add_bnm_variant(v222 222 0)
add_bnm_variant(v231 231 0)
add_bnm_variant(v232 232 0)
add_bnm_variant(v600 600 0)
add_bnm_variant(v605 605 0)
add_bnm_variant(v606 606 0)

target_link_libraries(${CMAKE_PROJECT_NAME}
        PUBLIC
        android
        log
        dl
        z
        dobby
)

if(BNM_ENABLE_HTTPS)
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
            ${BNM_OPENSSL_ROOT}/lib/libssl.a
            ${BNM_OPENSSL_ROOT}/lib/libcrypto.a
    )
endif()