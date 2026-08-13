cmake_minimum_required(VERSION 3.16)
project(grendelSim VERSION 1.0 LANGUAGES CXX)

option(GRENDEL_WITH_UIVIS "Build with Geant4 UI and visualization drivers" ON)
if(GRENDEL_WITH_UIVIS)
  find_package(Geant4 10 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 10 REQUIRED)
endif()
find_package(ROOT 6 REQUIRED COMPONENTS Cling Physics)
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
  src/grPMTRHit.cc
  src/grPMTSD.cc
  src/grPhotonTrack.cc
  src/grPrimaryGeneratorAction.cc
  src/grPrimaryGeneratorActionMessenger.cc
  src/grPrimaryGeneratorMessenger.cc
  src/grROOTEvent.cc
  src/grRunAction.cc
  src/grRunActionMessenger.cc
  src/grScintHit.cc
  src/grScintRHit.cc
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
set(GRENDEL_DICTIONARY_HEADERS
  include/grGammaTrack.hh
  include/grNeutronTrack.hh
  include/grMuonTrack.hh
  include/grElectronTrack.hh
  include/grMCPTrack.hh
  include/grPhotonTrack.hh
  include/grPMTRHit.hh
  include/grScintRHit.hh
  include/grROOTEvent.hh
)
set(GRENDEL_DICTIONARY_SOURCES
  src/grGammaTrack.cc
  src/grNeutronTrack.cc
  src/grMuonTrack.cc
  src/grElectronTrack.cc
  src/grMCPTrack.cc
  src/grPhotonTrack.cc
  src/grPMTRHit.cc
  src/grScintRHit.cc
  src/grROOTEvent.cc
)
list(REMOVE_ITEM GRENDEL_SOURCES ${GRENDEL_DICTIONARY_SOURCES})
ROOT_GENERATE_DICTIONARY(grendelSimDict ${GRENDEL_DICTIONARY_HEADERS}
  LINKDEF include/EventLinkDef.hh)
add_library(grendelSimCore SHARED ${GRENDEL_DICTIONARY_SOURCES}
  ${CMAKE_CURRENT_BINARY_DIR}/grendelSimDict.cxx)
target_include_directories(grendelSimCore PUBLIC include ${CMAKE_CURRENT_SOURCE_DIR} ${Boost_INCLUDE_DIRS})
target_link_libraries(grendelSimCore PUBLIC ${ROOT_LIBRARIES} ${ROOT_COMPONENT_LIBRARIES})

add_executable(grendelSim grendelSim.cc ${GRENDEL_SOURCES})
target_compile_features(grendelSim PRIVATE cxx_std_11)
target_compile_definitions(grendelSim PRIVATE ANALYSIS_HH_)
if(GRENDEL_WITH_UIVIS)
  target_compile_definitions(grendelSim PRIVATE G4UI_USE G4VIS_USE)
endif()
target_include_directories(grendelSim PRIVATE include ${Boost_INCLUDE_DIRS})
target_link_libraries(grendelSim PRIVATE grendelSimCore ${Geant4_LIBRARIES}
  ${ROOT_LIBRARIES} ${ROOT_COMPONENT_LIBRARIES})
set_target_properties(grendelSim PROPERTIES BUILD_RPATH "$ORIGIN")
file(COPY inputData/config DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
