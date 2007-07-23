include_directories(  ${GPGME_INCLUDES} )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config-gpgme++.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-gpgme++.h )

add_subdirectory( interfaces )


########### next target ###############

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



kde4_add_library(gpgme++ SHARED ${gpgme_LIB_SRCS})

target_link_libraries(gpgme++ ${GPGME_LIBRARIES})

set_target_properties(gpgme++ PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION} )
install(TARGETS gpgme++  DESTINATION ${LIB_INSTALL_DIR})


########### install files ###############

install( FILES context.h key.h trustitem.h 
   eventloopinteractor.h data.h gpgmefw.h result.h keylistresult.h keygenerationresult.h 
   importresult.h decryptionresult.h verificationresult.h signingresult.h encryptionresult.h engineinfo.h gpgme++_export.h 
   DESTINATION ${INCLUDE_INSTALL_DIR}/gpgme++ )
