cmake_minimum_required(VERSION 2.8.12)

project(Gpgmepp)

# ECM setup
find_package(ECM 1.3.0 CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${Gpgmepp_SOURCE_DIR}/cmake ${ECM_MODULE_PATH})

include(GenerateExportHeader)
include(ECMGenerateHeaders)
include(ECMGeneratePriFile)
include(ECMPackageConfigHelpers)
include(ECMSetupVersion)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings)


# FIXME: Do we want the same versions in kdepimlibs and kf5?
set(KF5_VERSION "5.4.0")
set(KDEPIMLIBS_VERSION "4.73.0")
set(QT_MIN_VERSION "5.2.0")

ecm_setup_version(${KDEPIMLIBS_VERSION} VARIABLE_PREFIX GPGMEPP
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/gpgmepp_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5GpgmeppConfigVersion.cmake"
                        SOVERSION 5
)

########### Find packages ###########
find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED Core)

find_package(Gpgme)
set_package_properties(Gpgme PROPERTIES
                       URL "http://www.gnupg.org/related_software/gpgme/"
                       DESCRIPTION "The GnuPG Made Easy (GPGME) library)"
                       TYPE REQUIRED
)


########### CMake Config Files ###########
set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/KF5Gpgmepp")



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
   DESTINATION ${KF5_INCLUDE_INSTALL_DIR}
   COMPONENT Devel
)

########### Targets ###########
add_definitions("-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII")
add_subdirectory(src)

if ("${CMAKE_BINARY_DIR}" STREQUAL "${Gpgmepp_BINARY_DIR}")
  feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
endif()
