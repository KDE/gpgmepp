add_definitions(-DTRANSLATION_DOMAIN=\"libqgpgme5\")


set( _any_gpgme_found false )

set( _qgpgme_version 1.0.1 )
set( _qgpgme_soversion 1 )

if ( WIN32 AND GPGME_VANILLA_FOUND )

   # on Windows, we require gpgme(-vanilla), which is thread-safe,
   # and we don't provide gpgme-qt support

   set( _any_gpgme_found true )
   set( _qgpgme_libgpgme gpgmepp )
   #set( _qgpgme_extra_src eventloopinteractor_win.cpp )

endif()

if ( NOT WIN32 AND GPGME_PTHREAD_FOUND )

   # on Unix, we requrie gpgme-pthread (for convenience, since vanilla
   # gpgme isn't thread-aware, and we don't want to restrict clients
   # of qgpgme to non-threaded useage of gpgme)

   set( _any_gpgme_found true )
   set( _qgpgme_libgpgme KF5::Gpgmepp-pthread )
   set( _qgpgme_extra_src qgpgme_eventloopinteractor.cpp eventloopinteractor_unix.cpp )
   set( _qgpgme_extra_headers qgpgme_eventloopinteractor.h )

endif()

# build / install only if any useable gpgme was found:

if ( _any_gpgme_found )

   set( qgpgme_LIB_SRCS
        dataprovider.cpp
        ${_qgpgme_extra_src}
   )

   add_library(KF5QGpgme ${qgpgme_LIB_SRCS})

   generate_export_header(KF5QGpgme BASE_NAME qgpgme)

   add_library(KF5::QGpgme ALIAS KF5QGpgme)

   target_link_libraries(KF5QGpgme
     PUBLIC
      Qt5::Core
     PRIVATE
      KF5::Gpgmepp
      ${_qgpgme_libgpgme}
   )

   target_include_directories(KF5QGpgme INTERFACE "$<INSTALL_INTERFACE:${KF5_INCLUDE_INSTALL_DIR}/qgpgme;${Boost_INCLUDE_DIR}>")
   target_include_directories(KF5QGpgme PUBLIC "$<BUILD_INTERFACE:${Gpgmepp_SOURCE_DIR}/src;${Gpgmepp_BINARY_DIR}/src;${Boost_INCLUDE_DIR}>")
   if ( GPGME_FOUND )
      if ( GPGME_INCLUDES )
         #REACTIVATE
         #target_include_directories(KF5QGpgme INTERFACE "$<INSTALL_INTERFACE:${GPGME_INCLUDES}>")
         #target_include_directories(KF5QGpgme PUBLIC "$<BUILD_INTERFACE:${GPGME_INCLUDES}>")
      endif()
   endif()

   set_target_properties(KF5QGpgme PROPERTIES
      VERSION ${GPGMEPP_VERSION_STRING}
      SOVERSION ${GPGMEPP_SOVERSION}
      EXPORT_NAME QGpgme
   )

   install(TARGETS KF5QGpgme EXPORT KF5GpgmeppTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

   install( FILES
     ${CMAKE_CURRENT_BINARY_DIR}/qgpgme_export.h
     ${_qgpgme_extra_headers}
     dataprovider.h 
     DESTINATION ${KF5_INCLUDE_INSTALL_DIR}/qgpgme COMPONENT Devel )

endif()

ecm_generate_pri_file(BASE_NAME QGpgme LIB_NAME KF5QGpgme DEPS "" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KF5_INCLUDE_INSTALL_DIR}/qgpgme)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

