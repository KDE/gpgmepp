project( gpgmepp )

include_directories(  ${GPGME_INCLUDES} )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config-gpgme++.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-gpgme++.h )

## gpgme comes in three flavours on each of the platforms:
##  Windows: gpgme, gpgme-glib, gpgme-qt
##     Unix: gpgme, gpgme-pthread, gpgme-pth
## We're building corresponding gpgme++ flavours

set(gpgme_LIB_SRCS 
	context.cpp key.cpp 
	trustitem.cpp 
	data.cpp 
	callbacks.cpp 
	eventloopinteractor.cpp 
	keylistresult.cpp            
	keygenerationresult.cpp 
	importresult.cpp
	decryptionresult.cpp 
	verificationresult.cpp 
	signingresult.cpp 
	encryptionresult.cpp 
	engineinfo.cpp
)

set( _gpgmepp_version 1.0.0 )

set( GPGMEPP_INCLUDE ${INCLUDE_INSTALL_DIR} ${GPGME_INCLUDES} )

if ( GPGME_VANILLA_FOUND )
   kde4_add_library( gpgmepp SHARED ${gpgme_LIB_SRCS} context_vanilla.cpp )
   target_link_libraries( gpgmepp ${GPGME_VANILLA_LIBRARIES} )
   set_target_properties( gpgmepp PROPERTIES
    VERSION     ${_gpgmepp_version}
    SOVERSION   ${_gpgmepp_version}
    OUTPUT_NAME gpgme++
   )
   get_target_property( GPGMEPP_VANILLA_LIBRARY gpgmepp LOCATION )
   install(TARGETS gpgmepp DESTINATION ${LIB_INSTALL_DIR} )
endif ( GPGME_VANILLA_FOUND )

if ( GPGME_GLIB_FOUND )
   kde4_add_library( gpgmepp-glib SHARED ${gpgme_LIB_SRCS} context_glib.cpp )
   target_link_libraries( gpgmepp-glib ${GPGME_GLIB_LIBRARIES} )
   set_target_properties( gpgmepp-glib PROPERTIES
    VERSION     ${_gpgmepp_version}
    SOVERSION   ${_gpgmepp_version}
    OUTPUT_NAME gpgme++-glib DEFINE_SYMBOL MAKE_GPGME___LIB
   )
   get_target_property( GPGMEPP_GLIB_LIBRARY gpgmepp LOCATION )
   install(TARGETS gpgmepp-glib DESTINATION ${LIB_INSTALL_DIR} )
endif ( GPGME_GLIB_FOUND )

if ( GPGME_QT_FOUND )
   kde4_add_library( gpgmepp-qt SHARED ${gpgme_LIB_SRCS} context_qt.cpp )
   target_link_libraries( gpgmepp-qt ${GPGME_QT_LIBRARIES} )
   set_target_properties( gpgmepp-qt PROPERTIES
    VERSION     ${_gpgmepp_version}
    SOVERSION   ${_gpgmepp_version}
    OUTPUT_NAME gpgme++-qt DEFINE_SYMBOL MAKE_GPGME___LIB
   )
   get_target_property( GPGMEPP_QT_LIBRARY gpgmepp LOCATION )
   install(TARGETS gpgmepp-qt DESTINATION ${LIB_INSTALL_DIR})
endif ( GPGME_QT_FOUND )

if ( GPGME_PTHREAD_FOUND )
   kde4_add_library( gpgmepp-pthread SHARED ${gpgme_LIB_SRCS} context_vanilla.cpp )
   target_link_libraries( gpgmepp-pthread ${GPGME_PTHREAD_LIBRARIES} )
   set_target_properties( gpgmepp-pthread PROPERTIES
    VERSION     ${_gpgmepp_version}
    SOVERSION   ${_gpgmepp_version}
    OUTPUT_NAME gpgme++-pthread DEFINE_SYMBOL MAKE_GPGME___LIB
   )
   get_target_property( GPGMEPP_PTHREAD_LIBRARY gpgmepp LOCATION )
   install(TARGETS gpgmepp-pthread DESTINATION ${LIB_INSTALL_DIR} )
endif ( GPGME_PTHREAD_FOUND )

if ( GPGME_PTH_FOUND )
   kde4_add_library( gpgmepp-pth SHARED ${gpgme_LIB_SRCS} context_vanilla.cpp )
   target_link_libraries( gpgmepp-pth ${GPGME_PTH_LIBRARIES} )
   set_target_properties( gpgmepp-pth PROPERTIES
    VERSION     ${_gpgmepp_version}
    SOVERSION   ${_gpgmepp_version}
    OUTPUT_NAME gpgme++-pth DEFINE_SYMBOL MAKE_GPGME___LIB
   )
   get_target_property( GPGMEPP_PTH_LIBRARY gpgmepp LOCATION )
   install(TARGETS gpgmepp-pth DESTINATION ${LIB_INSTALL_DIR} )
endif ( GPGME_PTH_FOUND )


include( CMakeExportBuildSettings )
# this writes way too much, but do we care?
EXPORT_LIBRARY_DEPENDENCIES( ${CMAKE_CURRENT_BINARY_DIR}/GpgmeppLibraryDepends.cmake )

########### install files ###############

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/GpgmeppConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/GpgmeppConfig.cmake @ONLY )

if ( GPGME_FOUND )

   add_subdirectory( interfaces )

   install(
     FILES
        global.h error.h context.h key.h trustitem.h 
        eventloopinteractor.h data.h gpgmefw.h result.h
        keylistresult.h keygenerationresult.h 
        importresult.h decryptionresult.h verificationresult.h
        signingresult.h encryptionresult.h notation.h engineinfo.h
        gpgme++_export.h 
     DESTINATION ${INCLUDE_INSTALL_DIR}/gpgme++
   )

   install(
     FILES
     ${CMAKE_CURRENT_BINARY_DIR}/GpgmeppConfig.cmake
     ${CMAKE_CURRENT_BINARY_DIR}/GpgmeppLibraryDepends.cmake
     DESTINATION
     ${LIB_INSTALL_DIR}/gpgmepp )

endif ( GPGME_FOUND )
