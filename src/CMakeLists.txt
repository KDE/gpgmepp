# Turn exceptions on
kde_enable_exceptions()

include (ConfigureChecks.cmake)

include_directories(${GPGME_INCLUDES})

add_definitions(-DMAKE_GPGMEPP_LIB)

if (MINGW)
  set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -mms-bitfields")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mms-bitfields")
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config-gpgme++.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-gpgme++.h )

## gpgme comes in three flavours on each of the platforms:
##  Windows: gpgme, gpgme-glib, gpgme-qt
##     Unix: gpgme, gpgme-pthread, gpgme-pth
## We're building corresponding gpgme++ flavours

set(gpgme_LIB_SRCS
        exception.cpp
        context.cpp key.cpp
        trustitem.cpp
        data.cpp
        callbacks.cpp
        eventloopinteractor.cpp
        editinteractor.cpp
        assuanresult.cpp
        keylistresult.cpp
        keygenerationresult.cpp
        importresult.cpp
        decryptionresult.cpp
        verificationresult.cpp
        signingresult.cpp
        encryptionresult.cpp
        engineinfo.cpp
        gpgsetexpirytimeeditinteractor.cpp
        gpgsetownertrusteditinteractor.cpp
        gpgsignkeyeditinteractor.cpp
        gpgadduserideditinteractor.cpp
        defaultassuantransaction.cpp
        scdgetinfoassuantransaction.cpp
        gpgagentgetinfoassuantransaction.cpp
        vfsmountresult.cpp
        configuration.cpp
)

set( _gpgmepp_version 2.8.0 )
set( _gpgmepp_soversion 2 )

if ( GPGME_VANILLA_FOUND )

   add_library(KF5Gpgmepp ${gpgme_LIB_SRCS} context_vanilla.cpp)

   generate_export_header(KF5Gpgmepp BASE_NAME gpgmepp)

   add_library(KF5::Gpgmepp ALIAS KF5Gpgmepp)

   target_link_libraries(KF5Gpgmepp
      ${GPGME_VANILLA_LIBRARIES}
      Qt5::Core
   )

   target_include_directories(KF5Gpgmepp INTERFACE "$<INSTALL_INTERFACE:${KF5_INCLUDE_INSTALL_DIR}/gpgme++>")
   target_include_directories(KF5Gpgmepp PUBLIC "$<BUILD_INTERFACE:${Gpgmepp_SOURCE_DIR}/src;${Gpgmepp_BINARY_DIR}/src>")

   set_target_properties(KF5Gpgmepp PROPERTIES
      VERSION ${GPGMEPP_VERSION_STRING}
      SOVERSION ${GPGMEPP_SOVERSION}
      EXPORT_NAME Gpgmepp
   )
   install(TARGETS KF5Gpgmepp EXPORT KF5GpgmeppTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

endif()

if ( GPGME_GLIB_FOUND )
   add_library(KF5Gpgmepp-glib ${gpgme_LIB_SRCS} context_glib.cpp)

   generate_export_header(KF5Gpgmepp-glib BASE_NAME gpgmepp-glib)

   add_library(KF5::Gpgmepp-glib ALIAS KF5Gpgmepp-glib)

   target_link_libraries(KF5Gpgmepp-glib
      ${GPGME_GLIB_LIBRARIES}
      Qt5::Core
   )

   target_include_directories(KF5Gpgmepp-glib INTERFACE "$<INSTALL_INTERFACE:${KF5_INCLUDE_INSTALL_DIR}/gpgme++>")
   target_include_directories(KF5Gpgmepp-glib PUBLIC "$<BUILD_INTERFACE:${Gpgmepp_SOURCE_DIR}/src;${Gpgmepp_BINARY_DIR}/src>")

   set_target_properties(KF5Gpgmepp-glib PROPERTIES
      VERSION ${GPGMEPP_VERSION_STRING}
      SOVERSION ${GPGMEPP_SOVERSION}
      EXPORT_NAME Gpgmepp-glib
   )
   install(TARGETS KF5Gpgmepp-glib EXPORT KF5GpgmeppTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})
endif()


if ( GPGME_QT_FOUND )
   add_library(KF5Gpgmepp-qt ${gpgme_LIB_SRCS} context_qt.cpp)

   generate_export_header(KF5Gpgmepp-qt BASE_NAME gpgmepp-qt)

   add_library(KF5::Gpgmepp-qt ALIAS KF5Gpgmepp-qt)

   target_link_libraries(KF5Gpgmepp-qt
      ${GPGME_QT_LIBRARIES}
      Qt5::Core
   )

   target_include_directories(KF5Gpgmepp-qt INTERFACE "$<INSTALL_INTERFACE:${KF5_INCLUDE_INSTALL_DIR}/gpgme++>")
   target_include_directories(KF5Gpgmepp-qt PUBLIC "$<BUILD_INTERFACE:${Gpgmepp_SOURCE_DIR}/src;${Gpgmepp_BINARY_DIR}/src>")

   if(WIN32)
      target_link_libraries( gpgmepp-qt ${GPGME_VANILLA_LIBRARIES} )
   endif()

   set_target_properties(KF5Gpgmepp-qt PROPERTIES
      VERSION ${GPGMEPP_VERSION_STRING}
      SOVERSION ${GPGMEPP_SOVERSION}
      EXPORT_NAME Gpgmepp-qt
   )
   install(TARGETS KF5Gpgmepp-qt EXPORT KF5GpgmeppTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})
endif()


if ( GPGME_PTHREAD_FOUND )
   add_library(KF5Gpgmepp-pthread ${gpgme_LIB_SRCS} context_vanilla.cpp)

   generate_export_header(KF5Gpgmepp-pthread BASE_NAME gpgmepp-pthread)

   add_library(KF5::Gpgmepp-pthread ALIAS KF5Gpgmepp-pthread)

   target_link_libraries(KF5Gpgmepp-pthread
      ${GPGME_PTHREAD_LIBRARIES}
      Qt5::Core
   )

   target_include_directories(KF5Gpgmepp-pthread INTERFACE "$<INSTALL_INTERFACE:${KF5_INCLUDE_INSTALL_DIR}/gpgme++>")
   target_include_directories(KF5Gpgmepp-pthread PUBLIC "$<BUILD_INTERFACE:${Gpgmepp_SOURCE_DIR}/src;${Gpgmepp_BINARY_DIR}/src>")

   set_target_properties(KF5Gpgmepp-pthread PROPERTIES
      VERSION ${GPGMEPP_VERSION_STRING}
      SOVERSION ${GPGMEPP_SOVERSION}
      EXPORT_NAME Gpgmepp-pthread
   )
   install(TARGETS KF5Gpgmepp-pthread EXPORT KF5GpgmeppTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

endif()

if ( GPGME_PTH_FOUND )
   add_library(KF5Gpgmepp-pth ${gpgme_LIB_SRCS} context_vanilla.cpp)

   generate_export_header(KF5Gpgmepp-pth BASE_NAME gpgmepp-pthread)

   add_library(KF5::Gpgmepp-pth ALIAS KF5Gpgmepp-pth)

   target_link_libraries(KF5Gpgmepp-pth
      ${GPGME_PTH_LIBRARIES}
      Qt5::Core
   )

   target_include_directories(KF5Gpgmepp-pth INTERFACE "$<INSTALL_INTERFACE:${KF5_INCLUDE_INSTALL_DIR}/gpgme++>")
   target_include_directories(KF5Gpgmepp-pth PUBLIC "$<BUILD_INTERFACE:${Gpgmepp_SOURCE_DIR}/src;${Gpgmepp_BINARY_DIR}/src>")

   set_target_properties(KF5Gpgmepp-pth PROPERTIES
      VERSION ${GPGMEPP_VERSION_STRING}
      SOVERSION ${GPGMEPP_SOVERSION}
      EXPORT_NAME Gpgmepp-pth
   )
   install(TARGETS KF5Gpgmepp-pth EXPORT KF5GpgmeppTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})
endif()

# this writes way too much, but do we care?
#EXPORT_LIBRARY_DEPENDENCIES( ${CMAKE_CURRENT_BINARY_DIR}/GpgmeppLibraryDepends.cmake )

############ build QGPGME ###############
add_subdirectory(qgpgme)

########### install files ###############

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/GpgmeppConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/GpgmeppConfig.cmake @ONLY )

if ( GPGME_FOUND )

   add_subdirectory( interfaces )

   install(FILES
        global.h error.h exception.h context.h key.h trustitem.h
        eventloopinteractor.h editinteractor.h data.h gpgmefw.h result.h
        assuanresult.h
        keylistresult.h keygenerationresult.h
        importresult.h decryptionresult.h verificationresult.h
        signingresult.h encryptionresult.h notation.h engineinfo.h
        gpgsetexpirytimeeditinteractor.h
        gpgsetownertrusteditinteractor.h
        gpgsignkeyeditinteractor.h
        gpgadduserideditinteractor.h
        defaultassuantransaction.h
        scdgetinfoassuantransaction.h
        gpgagentgetinfoassuantransaction.h
        configuration.h
        ${CMAKE_CURRENT_BINARY_DIR}/gpgmepp_export.h vfsmountresult.h
        DESTINATION ${KF5_INCLUDE_INSTALL_DIR}/gpgme++ COMPONENT Devel
   )

   install(
     FILES
     ${CMAKE_CURRENT_BINARY_DIR}/GpgmeppConfig.cmake
     #${CMAKE_CURRENT_BINARY_DIR}/GpgmeppLibraryDepends.cmake
     DESTINATION
     ${LIB_INSTALL_DIR}/gpgmepp )

endif()
