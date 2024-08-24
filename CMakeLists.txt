cmake_minimum_required (VERSION 3.5)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set (PROJECT_NAME SiprixUA)
Project(${PROJECT_NAME} LANGUAGES CXX)

set(SiprixUA_OUT_DIR ${SiprixUA_BINARY_DIR}/out)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG   ${SiprixUA_OUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${SiprixUA_OUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${SiprixUA_OUT_DIR})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${SiprixUA_OUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${SiprixUA_OUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${SiprixUA_OUT_DIR})


set(BUILD_TYPE "Release")
if(DEFINED ENV{BUILD_TYPE})
    set(BUILD_TYPE $ENV{BUILD_TYPE})
endif()

set (SOURCES
    SiprixUA.cxx
)

if(APPLE)   
   add_executable(${PROJECT_NAME} MACOSX_BUNDLE  ${SOURCES})
else()
   add_executable(${PROJECT_NAME} ${SOURCES})
endif()


if(WIN32)
    set(FRAMEWORK_DIR "${CMAKE_SOURCE_DIR}/win/siprix.framework")

    target_include_directories(${PROJECT_NAME} PUBLIC ${FRAMEWORK_DIR}/include)
    target_link_libraries(${PROJECT_NAME}             ${FRAMEWORK_DIR}/lib/siprix.lib)

    set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${SiprixUA_OUT_DIR}")

    file(COPY ${FRAMEWORK_DIR}/lib/siprix.dll      DESTINATION ${SiprixUA_OUT_DIR}) 
    file(COPY ${FRAMEWORK_DIR}/lib/siprixMedia.dll DESTINATION ${SiprixUA_OUT_DIR}) 
endif()

if(UNIX)
    if(APPLE)
        set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST
           ${CMAKE_SOURCE_DIR}/macos/MacOSXBundleInfo.plist.in)

        set_target_properties(${PROJECT_NAME} PROPERTIES XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS 
           "@executable_path/../Frameworks")

        target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/macos/siprix.framework/Headers")

        find_library(COREAUDIO_FRAMEWORK CoreAudio)
        set(SIPRIX_FRAMEWORK       "${CMAKE_SOURCE_DIR}/macos/siprix.framework")
        set(SIPRIX_MEDIA_FRAMEWORK "${CMAKE_SOURCE_DIR}/macos/siprixMedia.framework")

        set(EMBED_FRAMEWORKS ${EMBED_FRAMEWORKS} ${SIPRIX_FRAMEWORK})
        set(EMBED_FRAMEWORKS ${EMBED_FRAMEWORKS} ${SIPRIX_MEDIA_FRAMEWORK})

        set(LINK_LIBS ${LINK_LIBS} ${SIPRIX_FRAMEWORK})
        set(LINK_LIBS ${LINK_LIBS} ${COREAUDIO_FRAMEWORK})
        
        set_target_properties(${PROJECT_NAME} PROPERTIES  XCODE_EMBED_FRAMEWORKS "${EMBED_FRAMEWORKS}")
        set_target_properties(${PROJECT_NAME} PROPERTIES  XCODE_EMBED_FRAMEWORKS_CODE_SIGN_ON_COPY TRUE)
        
        target_link_libraries(${PROJECT_NAME} ${LINK_LIBS})
    else()
        set(FRAMEWORK_DIR "${CMAKE_SOURCE_DIR}/linux/siprix.framework")

        target_include_directories(${PROJECT_NAME} PUBLIC ${FRAMEWORK_DIR}/include)
  
        file(COPY ${FRAMEWORK_DIR}/lib/libsiprix.so      DESTINATION ${SiprixUA_OUT_DIR}) 
        file(COPY ${FRAMEWORK_DIR}/lib/libsiprixMedia.so DESTINATION ${SiprixUA_OUT_DIR}) 

        target_link_libraries(${PROJECT_NAME}            ${SiprixUA_OUT_DIR}/libsiprix.so)
        target_link_libraries(${PROJECT_NAME}            ${SiprixUA_OUT_DIR}/libsiprixMedia.so)
    endif()
endif()
