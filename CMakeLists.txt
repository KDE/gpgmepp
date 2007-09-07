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

if ( GPGME_VANILLA_FOUND )
   kde4_add_library( gpgmepp SHARED ${gpgme_LIB_SRCS} context_vanilla.cpp )
   target_link_libraries( gpgmepp ${GPGME_VANILLA_LIBRARIES} )
   set_target_properties( gpgmepp PROPERTIES
    VERSION     ${_gpgmepp_version}
    SOVERSION   ${_gpgmepp_version}
    OUTPUT_NAME gpgme++
   )
   install(TARGETS gpgmepp  DESTINATION ${LIB_INSTALL_DIR})
endif ( GPGME_VANILLA_FOUND )

if ( GPGME_GLIB_FOUND )
   kde4_add_library( gpgmepp-glib SHARED ${gpgme_LIB_SRCS} context_glib.cpp )
   target_link_libraries( gpgmepp-glib ${GPGME_GLIB_LIBRARIES} )
   set_target_properties( gpgmepp-glib PROPERTIES
    VERSION     ${_gpgmepp_version}
    SOVERSION   ${_gpgmepp_version}
    OUTPUT_NAME gpgme++-glib DEFINE_SYMBOL MAKE_GPGME___LIB
   )
   install(TARGETS gpgmepp-glib  DESTINATION ${LIB_INSTALL_DIR})
endif ( GPGME_GLIB_FOUND )

if ( GPGME_QT_FOUND )
   kde4_add_library( gpgmepp-qt SHARED ${gpgme_LIB_SRCS} context_qt.cpp )
   target_link_libraries( gpgmepp-qt ${GPGME_QT_LIBRARIES} )
   set_target_properties( gpgmepp-qt PROPERTIES
    VERSION     ${_gpgmepp_version}
    SOVERSION   ${_gpgmepp_version}
    OUTPUT_NAME gpgme++-qt DEFINE_SYMBOL MAKE_GPGME___LIB
   )
   install(TARGETS gpgmepp-qt  DESTINATION ${LIB_INSTALL_DIR})
endif ( GPGME_QT_FOUND )

if ( GPGME_PTHREAD_FOUND )
   kde4_add_library( gpgmepp-pthread SHARED ${gpgme_LIB_SRCS} context_vanilla.cpp )
   target_link_libraries( gpgmepp-pthread ${GPGME_PTHREAD_LIBRARIES} )
   set_target_properties( gpgmepp-pthread PROPERTIES
    VERSION     ${_gpgmepp_version}
    SOVERSION   ${_gpgmepp_version}
    OUTPUT_NAME gpgme++-pthread DEFINE_SYMBOL MAKE_GPGME___LIB
   )
   install(TARGETS gpgmepp-pthread  DESTINATION ${LIB_INSTALL_DIR})
endif ( GPGME_PTHREAD_FOUND )

if ( GPGME_PTH_FOUND )
   kde4_add_library( gpgmepp-pth SHARED ${gpgme_LIB_SRCS} context_vanilla.cpp )
   target_link_libraries( gpgmepp-pth ${GPGME_PTH_LIBRARIES} )
   set_target_properties( gpgmepp-pth PROPERTIES
    VERSION     ${_gpgmepp_version}
    SOVERSION   ${_gpgmepp_version}
    OUTPUT_NAME gpgme++-pth DEFINE_SYMBOL MAKE_GPGME___LIB
   )
   install(TARGETS gpgmepp-pth  DESTINATION ${LIB_INSTALL_DIR})
endif ( GPGME_PTH_FOUND )


########### install files ###############

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

endif ( GPGME_FOUND )
