
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
   set( _qgpgme_libgpgme gpgmepp-pthread )
   set( _qgpgme_extra_src eventloopinteractor.cpp eventloopinteractor_unix.cpp )
   set( _qgpgme_extra_headers eventloopinteractor.h )

endif()

# build / install only if any useable gpgme was found:

if ( _any_gpgme_found )

   include_directories( ${CMAKE_CURRENT_BINARY_DIR} ${GPGME_INCLUDES} ${CMAKE_SOURCE_DIR}/gpgme++ ${Boost_INCLUDE_DIR} )

   add_subdirectory( tests )

   set( qgpgme_LIB_SRCS
        dataprovider.cpp
        ${_qgpgme_extra_src}
   )

   add_library( qgpgme ${LIBRARY_TYPE} ${qgpgme_LIB_SRCS} )
   generate_export_header(qgpgme)

   target_link_libraries( qgpgme ${QT_QTCORE_LIBRARY} ${_qgpgme_libgpgme} )

   set_target_properties( qgpgme PROPERTIES
     VERSION ${_qgpgme_version}
     SOVERSION ${_qgpgme_soversion}
   )

   install( TARGETS qgpgme EXPORT kdepimlibsLibraryTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

   install( FILES
     ${CMAKE_CURRENT_BINARY_DIR}/qgpgme_export.h
     ${_qgpgme_extra_headers}
     dataprovider.h 
     DESTINATION ${INCLUDE_INSTALL_DIR}/qgpgme COMPONENT Devel )

endif()
