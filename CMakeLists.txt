cmake_minimum_required(VERSION 2.8.12)

project(Gpgmepp)

# ECM setup
find_package(ECM 5.19.0 CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${Gpgmepp_SOURCE_DIR}/cmake ${ECM_MODULE_PATH})

include(GenerateExportHeader)
include(ECMGenerateHeaders)
include(ECMGeneratePriFile)
include(ECMPackageConfigHelpers)
include(ECMSetupVersion)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)


set(KF5_VERSION "5.19.0")
set(GPGMEPP_LIB_VERSION "5.2.0")
set(QT_MIN_VERSION "5.2.0")

ecm_setup_version(${GPGMEPP_LIB_VERSION} VARIABLE_PREFIX GPGMEPP
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/gpgmepp_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5GpgmeppConfigVersion.cmake"
                        SOVERSION 5
)

########### Find packages ###########
find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED Core)

find_package(Gpgme)
set_package_properties(Gpgme PROPERTIES
    PURPOSE "Required to build Gpgme++"
    TYPE REQUIRED
)

find_package(Boost REQUIRED)

########### CMake Config Files ###########
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5Gpgmepp")



ecm_configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/KF5GpgmeppConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/KF5GpgmeppConfig.cmake"
  INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/KF5GpgmeppConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/KF5GpgmeppConfigVersion.cmake"
  DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
  COMPONENT Devel
)

install(EXPORT
    KF5GpgmeppTargets
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    FILE KF5GpgmeppTargets.cmake
    NAMESPACE KF5::)


install(FILES
   ${CMAKE_CURRENT_BINARY_DIR}/gpgmepp_version.h
   DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}
   COMPONENT Devel
)

########### Targets ###########
add_definitions("-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII")
add_subdirectory(src)

if(BUILD_TESTING)
    #qt5 reactivate it
    #add_subdirectory(autotests)
endif()

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
