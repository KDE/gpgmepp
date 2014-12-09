kde_enable_exceptions()

include(ECMMarkAsTest)

find_package(Qt5Test CONFIG REQUIRED)


if ( WIN32 AND GPGME_VANILLA_FOUND )
   set( _qgpgme_libgpgme gpgmepp )
endif()

if ( NOT WIN32 AND GPGME_PTHREAD_FOUND )
   set( _qgpgme_libgpgme gpgmepp-pthread )
endif()

set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${GPGME_INCLUDES} )
include_directories( ${Boost_INCLUDE_DIR} )
link_directories( ${_gpgme_library_dirs} )

########### next target ###############

set( qgpgme_TESTS
  dataprovidertest
  installtest
  # PORT ME AWAY FROM EVENT LOOPS
  #gpgsetexpirytimetest
  #gpgsetownertrusttest
  #gpgsignkeytest
  #gpgadduseridtest
  )

foreach( _qgpgme_test ${qgpgme_TESTS} )
  kde4_add_executable( "${_qgpgme_test}" TEST "${_qgpgme_test}.cpp" )
  if (WIN32) # what's up?
    target_link_libraries( "${_qgpgme_test}" qgpgme ${_qgpgme_libgpgme} ${_gpgme_vanilla_library} ${_gpg_error_library} ${QT_QTCORE_LIBRARY} )
  else()
    target_link_libraries( "${_qgpgme_test}" qgpgme ${_qgpgme_libgpgme} gpgme gpg-error ${QT_QTCORE_LIBRARY} )
  endif()
endforeach( _qgpgme_test )

########### next target ###############

# PORT ME AWAY FROM EVENT LOOPS
#kde4_add_executable(editinteractortest TEST editinteractortest.cpp)
#target_link_libraries(editinteractortest  qgpgme ${QT_QTGUI_LIBRARY})
