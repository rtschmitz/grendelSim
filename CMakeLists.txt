cmake_minimum_required(VERSION 3.16)
project(grendelSim VERSION 1.0 LANGUAGES CXX)

option(GRENDEL_WITH_UIVIS "Build with Geant4 UI and visualization drivers" ON)
if(GRENDEL_WITH_UIVIS)
  find_package(Geant4 11 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 11 REQUIRED)
endif()
if(DEFINED ENV{ROOTSYS} AND EXISTS "$ENV{ROOTSYS}/cmake/ROOTConfig.cmake")
  list(PREPEND CMAKE_PREFIX_PATH "$ENV{ROOTSYS}/cmake")
  message(STATUS "Using ROOT from ROOTSYS: $ENV{ROOTSYS}/cmake")
endif()
find_package(ROOT 6 REQUIRED COMPONENTS Core RIO Tree)
find_package(Boost REQUIRED)
include(${Geant4_USE_FILE})
include(${ROOT_USE_FILE})

set(GRENDEL_SOURCES
  src/grBarParameterisation.cc
  src/grDetectorConstruction.cc
  src/grEMPhysics.cc
  src/grElectronTrack.cc
  src/grEventAction.cc
  src/grFilePrimaryGeneratorAction.cc
  src/grGammaTrack.cc
  src/grGenFileName.cc
  src/grHistoManager.cc
  src/grMCPTrack.cc
  src/grMonopole.cc
  src/grMonopoleEquation.cc
  src/grMonopoleFieldMessenger.cc
  src/grMonopoleFieldSetup.cc
  src/grMonopolePhysics.cc
  src/grMonopolePhysicsMessenger.cc
  src/grMuonPhysics.cc
  src/grMuonTrack.cc
  src/grNeutronTrack.cc
  src/grPMTHit.cc
  src/grPMTParameterisation.cc
  src/grPMTSD.cc
  src/grPrimaryGeneratorAction.cc
  src/grPrimaryGeneratorActionMessenger.cc
  src/grPrimaryGeneratorMessenger.cc
  src/grRunAction.cc
  src/grRunActionMessenger.cc
  src/grScintHit.cc
  src/grScintSD.cc
  src/grSession.cc
  src/grSessionMessenger.cc
  src/grShieldingPhysicsList.cc
  src/grStackingAction.cc
  src/grSteppingAction.cc
  src/grSteppingMessenger.cc
  src/grSteppingVerbose.cc
  src/grTrackingAction.cc
  src/grUserEventInformation.cc
  src/grUserTrackInformation.cc
)

add_executable(grendelSim grendelSim.cc ${GRENDEL_SOURCES})
target_compile_features(grendelSim PRIVATE cxx_std_17)
target_compile_definitions(grendelSim PRIVATE ANALYSIS_HH_)
if(GRENDEL_WITH_UIVIS)
  target_compile_definitions(grendelSim PRIVATE G4UI_USE G4VIS_USE)
endif()
target_include_directories(grendelSim PRIVATE include ${Boost_INCLUDE_DIRS})
target_link_libraries(grendelSim PRIVATE ${Geant4_LIBRARIES}
  ${ROOT_LIBRARIES} ${ROOT_COMPONENT_LIBRARIES})
set_target_properties(grendelSim PROPERTIES BUILD_RPATH "$ORIGIN")
file(COPY inputData/config DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
