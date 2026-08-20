// created 22-04-2019
// author: Ryan Schmitz (UCSB)

//==============================================================================
#include "grDetectorConstruction.hh"
#include "grScintSD.hh"
#include "grPMTSD.hh"
#include "grBarParameterisation.hh"
#include "grPMTParameterisation.hh"
#include "grUserEventInformation.hh"

#include "grMonopoleFieldSetup.hh"

#include <sstream>
#include <math.h>
#include "globals.hh"
// Includes Physical Constants and System of Units
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4ios.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4ThreeVector.hh"
#include "G4TwoVector.hh"
#include "G4GenericTrap.hh"
#include "G4RotationMatrix.hh"
#include "G4Trd.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4RunManager.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "grVolumeID.hh"
#include "G4Colour.hh"
#include "G4EmCalculator.hh"
#include "G4ParticleDefinition.hh"
#include "G4MuonMinus.hh"
#include "G4MuonPlus.hh"

#include "G4ExtrudedSolid.hh"
#include "G4VSolid.hh"
#include "G4Box.hh"
#include "G4Para.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4EllipticalTube.hh"
#include "G4Trap.hh"
#include "G4Polycone.hh"
#include "G4Polyhedra.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4DisplacedSolid.hh"
#include "G4TessellatedSolid.hh"
#include "G4TriangularFacet.hh"
#include "G4QuadrangularFacet.hh"
#include "G4GeomTools.hh"
#include "G4Transform3D.hh"
#include "G4Exception.hh"

#include <vector>
#include <utility>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include "G4RunManager.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpticalSurface.hh"
#include "G4MaterialTable.hh"
#include "G4NistManager.hh"
#include "G4PhysicsVector.hh"

#include "G4Sphere.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolume.hh"

#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4VPVParameterisation.hh"
#include "G4PVParameterised.hh"
#include "globals.hh"
#include "G4SolidStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4GeometryManager.hh"

#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4AssemblyVolume.hh"


#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4SDManager.hh"
#include "G4UImanager.hh"

const G4int nLayers = 4; //number of layers in detector. Up here since it defines an array

//const G4String grOpticalFilePath = "/home/users/ryan/GRENDELDemoSim/OpticalData/";
//const G4String grOpticalFilePath = (std::string)std::getenv("PROJECTDIR")+"/OpticalData/";
const G4String grOpticalFilePath = "../OpticalData/";
//==============================================================================
grDetectorConstruction::grDetectorConstruction() :
    verbose(1)//,solidWorld(0), logicWorld(0), physicWorld(0)
{
  SetDefaults();
}

//==============================================================================
grDetectorConstruction::~grDetectorConstruction() {

}
//==============================================================================

//==============================================================================

G4VPhysicalVolume* grDetectorConstruction::Construct() {

  return SetupGeometry();
}

//==============================================================================

G4VPhysicalVolume* grDetectorConstruction::SetupGeometry() {

    // -------------------------------------------------------------------------
    // Curved GARGOYLE tunnel only
    //
    // Global axes:
    //   X-Z = horizontal tunnel-centerline plane
    //   Y   = vertical, positive up
    //
    // The tunnel, finite rock envelope, and detector layers are closed swept
    // tessellated solids. This avoids Boolean-solid visualization overhead while
    // retaining the bends that matter for detector design.
    // -------------------------------------------------------------------------

    if (verbose >= 0) {
        G4cout << "grendelSim> Construct curved GARGOYLE tunnel geometry."
               << G4endl;
    }

    G4NistManager* tunnelNist = G4NistManager::Instance();
    G4Material* tunnelAir = tunnelNist->FindOrBuildMaterial("G4_AIR");
    G4Material* tunnelRock = tunnelNist->FindOrBuildMaterial("G4_CONCRETE");

    // ---------------------------------------------------------------------
    // Tunnel cross-section (the existing 2.90 m x 3.15 m horseshoe).
    // Twenty-four arch chords give a maximum arch faceting error of only a
    // few millimetres, without carrying the previous 400-sided profile into
    // every centerline chord.
    // ---------------------------------------------------------------------
    const G4double tunnelFloorWidth = 2.90 * m;
    const G4double tunnelHeight = 3.15 * m;
    const G4double tunnelArchWidth = 2.90 * m;
    const G4double tunnelArchHeight = 1.90 * m;
    const G4double tunnelWallHeight = tunnelHeight - tunnelArchHeight;
    const G4double tunnelHalfFloor = 0.5 * tunnelFloorWidth;
    const G4double tunnelHalfArch = 0.5 * tunnelArchWidth;

    // Keep the old area-centroid convention, so later detector placement can
    // reuse the same transverse coordinates.
    const G4double tunnelRectArea = tunnelFloorWidth * tunnelWallHeight;
    const G4double tunnelRectCY = 0.5 * tunnelWallHeight;
    const G4double tunnelEllipseArea =
            0.5 * pi * tunnelHalfArch * tunnelArchHeight;
    const G4double tunnelEllipseCY =
            tunnelWallHeight + 4.0 * tunnelArchHeight / (3.0 * pi);
    const G4double tunnelCentroidY =
            (tunnelRectArea * tunnelRectCY
             + tunnelEllipseArea * tunnelEllipseCY)
            / (tunnelRectArea + tunnelEllipseArea);
    const G4double tunnelFloorY = -tunnelCentroidY;
    const G4double tunnelSpringY = tunnelFloorY + tunnelWallHeight;

    const G4int tunnelArchSegments = 24;
    std::vector<G4TwoVector> curvedTunnelProfile;
    curvedTunnelProfile.reserve(tunnelArchSegments + 3);
    curvedTunnelProfile.push_back(
            G4TwoVector(-tunnelHalfFloor, tunnelFloorY));
    curvedTunnelProfile.push_back(
            G4TwoVector( tunnelHalfFloor, tunnelFloorY));
    curvedTunnelProfile.push_back(
            G4TwoVector( tunnelHalfArch, tunnelSpringY));
    for (G4int i = 1; i < tunnelArchSegments; ++i) {
        const G4double angle = pi * static_cast<G4double>(i)
                             / static_cast<G4double>(tunnelArchSegments);
        curvedTunnelProfile.push_back(
                G4TwoVector(tunnelHalfArch * std::cos(angle),
                            tunnelSpringY
                            + tunnelArchHeight * std::sin(angle)));
    }
    curvedTunnelProfile.push_back(
            G4TwoVector(-tunnelHalfArch, tunnelSpringY));

    // ---------------------------------------------------------------------
    // Centerline parameterization from the fitted plan view.
    // Coordinates here are dimensionless source-image pixels.  The complete
    // visible centerline is scaled to 100 m below.
    // ---------------------------------------------------------------------
    const G4double requestedTunnelPathLength = 110.0 * m;
    std::vector<G4TwoVector> denseCenterlinePixels;
    denseCenterlinePixels.reserve(600);

    auto appendCenterlinePixel = [&](G4double x, G4double z) {
        if (!denseCenterlinePixels.empty()) {
            const G4double dx = x - denseCenterlinePixels.back().x();
            const G4double dz = z - denseCenterlinePixels.back().y();
            if (dx * dx + dz * dz < 1.0e-20) return;
        }
        denseCenterlinePixels.push_back(G4TwoVector(x, z));
    };

    auto tunnelSmoothstep5 = [](G4double u) -> G4double {
        return 10.0 * u * u * u
             - 15.0 * u * u * u * u
             +  6.0 * u * u * u * u * u;
    };

    // P0 -> P1: lower straight.
    for (G4int i = 0; i <= 80; ++i) {
        const G4double u = static_cast<G4double>(i) / 80.0;
        appendCenterlinePixel(0.0, 348.0 * u);
    }

    // P1 -> P2: refined smooth transition.  The correction and its first two
    // derivatives vanish at both endpoints, preserving the accepted joins.
    for (G4int i = 0; i <= 180; ++i) {
        const G4double u = static_cast<G4double>(i) / 180.0;
        const G4double q = tunnelSmoothstep5(u);
        const G4double correction =
                1500.0 * u * u * u * (1.0 - u) * (1.0 - u) * (1.0 - u);
        appendCenterlinePixel(-177.0 * q - correction,
                              348.0 + 430.0 * u);
    }

    // P2 -> P3: upper-left straight.
    for (G4int i = 0; i <= 60; ++i) {
        const G4double u = static_cast<G4double>(i) / 60.0;
        appendCenterlinePixel(-177.0, 778.0 + 253.0 * u);
    }

    // P3 -> P4: upper-left circular bend.
    for (G4int i = 0; i <= 80; ++i) {
        const G4double u = static_cast<G4double>(i) / 80.0;
        const G4double theta = pi - 0.5 * pi * u;
        appendCenterlinePixel(-60.0 + 117.0 * std::cos(theta),
                              1031.0 + 117.0 * std::sin(theta));
    }

    // P4 -> P5: nearly straight top section with the measured 3-pixel drift.
    for (G4int i = 0; i <= 40; ++i) {
        const G4double u = static_cast<G4double>(i) / 40.0;
        appendCenterlinePixel(-60.0 + 122.0 * u,
                              1148.0 - 3.0 * tunnelSmoothstep5(u));
    }

    // P5 -> P6: upper-right elliptical bend.
    for (G4int i = 0; i <= 80; ++i) {
        const G4double u = static_cast<G4double>(i) / 80.0;
        const G4double theta = 0.5 * pi * (1.0 - u);
        appendCenterlinePixel(62.0 + 114.0 * std::cos(theta),
                              1030.0 + 115.0 * std::sin(theta));
    }

    // P6 -> P7: right-hand straight down to the visible junction.
    for (G4int i = 0; i <= 30; ++i) {
        const G4double u = static_cast<G4double>(i) / 30.0;
        appendCenterlinePixel(176.0, 1030.0 - 94.0 * u);
    }

    // Centre the plan-view bounding box on the world origin.
    G4double centerlineMinX = denseCenterlinePixels.front().x();
    G4double centerlineMaxX = denseCenterlinePixels.front().x();
    G4double centerlineMinZ = denseCenterlinePixels.front().y();
    G4double centerlineMaxZ = denseCenterlinePixels.front().y();
    for (std::size_t i = 1; i < denseCenterlinePixels.size(); ++i) {
        centerlineMinX = std::min(centerlineMinX,
                                  denseCenterlinePixels[i].x());
        centerlineMaxX = std::max(centerlineMaxX,
                                  denseCenterlinePixels[i].x());
        centerlineMinZ = std::min(centerlineMinZ,
                                  denseCenterlinePixels[i].y());
        centerlineMaxZ = std::max(centerlineMaxZ,
                                  denseCenterlinePixels[i].y());
    }
    const G4double centerlineOffsetX =
            0.5 * (centerlineMinX + centerlineMaxX);
    const G4double centerlineOffsetZ =
            0.5 * (centerlineMinZ + centerlineMaxZ);

    // Preserve the analytic joins, but allocate geometry only where the
    // centerline actually bends. Straight pieces need one chord; the long
    // transition uses five and each quarter-turn uses three. This produces
    // 15 chords for the complete 110 m tunnel instead of 100 Boolean nodes.
    std::vector<G4TwoVector> representativePixels;
    auto appendRepresentativePixel = [&](G4double x, G4double z) {
        if (!representativePixels.empty()) {
            const G4double dx = x - representativePixels.back().x();
            const G4double dz = z - representativePixels.back().y();
            if (dx * dx + dz * dz < 1.0e-20) return;
        }
        representativePixels.push_back(G4TwoVector(x, z));
    };
    for (G4int i = 0; i <= 1; ++i) {
        const G4double u = static_cast<G4double>(i);
        appendRepresentativePixel(0.0, 348.0 * u);
    }
    for (G4int i = 0; i <= 5; ++i) {
        const G4double u = static_cast<G4double>(i) / 5.0;
        const G4double q = tunnelSmoothstep5(u);
        const G4double c = 1500.0 * u * u * u * (1.0-u) * (1.0-u) * (1.0-u);
        appendRepresentativePixel(-177.0 * q - c, 348.0 + 430.0 * u);
    }
    appendRepresentativePixel(-177.0, 1031.0);
    for (G4int i = 1; i <= 3; ++i) {
        const G4double u = static_cast<G4double>(i) / 3.0;
        const G4double theta = pi - 0.5 * pi * u;
        appendRepresentativePixel(-60.0 + 117.0 * std::cos(theta),
                                  1031.0 + 117.0 * std::sin(theta));
    }
    appendRepresentativePixel(62.0, 1145.0);
    for (G4int i = 1; i <= 3; ++i) {
        const G4double u = static_cast<G4double>(i) / 3.0;
        const G4double theta = 0.5 * pi * (1.0 - u);
        appendRepresentativePixel(62.0 + 114.0 * std::cos(theta),
                                  1030.0 + 115.0 * std::sin(theta));
    }
    appendRepresentativePixel(176.0, 936.0);

    G4double representativeLengthPixels = 0.0;
    for (std::size_t i = 1; i < representativePixels.size(); ++i) {
        const G4double dx = representativePixels[i].x() - representativePixels[i-1].x();
        const G4double dz = representativePixels[i].y() - representativePixels[i-1].y();
        representativeLengthPixels += std::sqrt(dx * dx + dz * dz);
    }
    const G4double representativeScale =
            requestedTunnelPathLength / representativeLengthPixels;
    G4int tunnelChordCount =
            static_cast<G4int>(representativePixels.size()) - 1;
    std::vector<G4TwoVector> tunnelStations;
    tunnelStations.reserve(representativePixels.size());
    for (std::size_t i = 0; i < representativePixels.size(); ++i) {
        tunnelStations.push_back(G4TwoVector(
                (representativePixels[i].x() - centerlineOffsetX) * representativeScale,
                (representativePixels[i].y() - centerlineOffsetZ) * representativeScale));
    }

    const char* tunnelModeEnv = std::getenv("GRENDEL_TUNNEL_MODE");
    G4String tunnelMode = tunnelModeEnv ? tunnelModeEnv : "full";
    if (!tunnelModeEnv) {
        try {
            boost::property_tree::ptree geometryConfig;
            boost::property_tree::ini_parser::read_ini("config/Geometry/onepc.ini", geometryConfig);
            tunnelMode = geometryConfig.get<std::string>("TunnelGeometry.EnabledGroups", tunnelMode);
        } catch (const boost::property_tree::ptree_error&) {
            /* Keep full-tunnel default when config is unavailable. */
        }
    }

    G4int selectedFirstChord = 0;
    G4int selectedLastChord = tunnelChordCount - 1;
    if (tunnelMode != "full" && tunnelMode != "all") {
        selectedFirstChord = tunnelChordCount;
        selectedLastChord = -1;
        std::stringstream groups(tunnelMode);
        std::string group;
        while (std::getline(groups, group, ',')) {
            const std::size_t first = group.find_first_not_of(" \t");
            const std::size_t last = group.find_last_not_of(" \t");
            group = first == std::string::npos ? "" : group.substr(first, last - first + 1);
            G4int groupFirst = -1;
            G4int groupLast = -1;
            if (group == "0" || group == "first") { groupFirst = 0; groupLast = 0; }
            else if (group == "1") { groupFirst = 1; groupLast = 1; }
            else if (group == "straightOnly") { groupFirst = 0; groupLast = 0; }
            else if (group == "straight") { groupFirst = 0; groupLast = 1; }
            else if (group == "shallow") { groupFirst = 1; groupLast = 5; }
            else if (group == "turn1") { groupFirst = 5; groupLast = 10; }
            else if (group == "turn2") { groupFirst = 10; groupLast = 14; }
            else if (group == "full" || group == "all") { groupFirst = 0; groupLast = tunnelChordCount - 1; }
            else if (!group.empty()) { G4cerr << "Unknown tunnel segment group '" << group << "'; ignoring it." << G4endl; }
            if (groupFirst >= 0) {
                selectedFirstChord = std::min(selectedFirstChord, groupFirst);
                selectedLastChord = std::max(selectedLastChord, groupLast);
            }
        }
        if (selectedLastChord < selectedFirstChord) {
            G4cerr << "No valid tunnel segment groups selected; using full tunnel." << G4endl;
            selectedFirstChord = 0;
            selectedLastChord = tunnelChordCount - 1;
        }
    }
    if (selectedFirstChord != 0 || selectedLastChord != tunnelChordCount - 1) {
        std::vector<G4TwoVector> selectedStations(
                tunnelStations.begin() + selectedFirstChord,
                tunnelStations.begin() + selectedLastChord + 2);
        tunnelStations.swap(selectedStations);
        tunnelChordCount = selectedLastChord - selectedFirstChord + 1;
    }

    G4double constructedTunnelLength = 0.0;
    for (std::size_t i = 1; i < tunnelStations.size(); ++i) {
        const G4double dx = tunnelStations[i].x() - tunnelStations[i - 1].x();
        const G4double dz = tunnelStations[i].y() - tunnelStations[i - 1].y();
        constructedTunnelLength += std::sqrt(dx * dx + dz * dz);
    }

    // ---------------------------------------------------------------------
    // Per-chord analytic construction.
    //
    // Earlier revisions built every selected chord as one continuous swept
    // tessellated mesh, with a single-chord special case ("fastStraight")
    // that fell back to cheap G4ExtrudedSolid pieces. Navigating a
    // multi-facet tessellated solid is far more expensive per step than
    // navigating a handful of analytic solids, so any selection spanning
    // more than one chord (e.g. "straight" = chords 0-1) never hit the
    // fast path and ran many times slower than a single chord despite
    // covering a similar length.
    //
    // Every selected chord -- however many -- is now built as its own
    // straight extruded segment, mitre-cut at both ends against the
    // bisector plane shared with its neighbour so consecutive segments
    // meet exactly, with no gap or overlap in the rock or tunnel-air
    // navigation volumes. This keeps the same polygonal centerline
    // approximation as before while using only cheap analytic solids.
    // ---------------------------------------------------------------------
    std::vector<G4double> chordAngles(tunnelChordCount, 0.0);
    for (G4int i = 0; i < tunnelChordCount; ++i) {
        const G4double dx = tunnelStations[i + 1].x()
                          - tunnelStations[i].x();
        const G4double dz = tunnelStations[i + 1].y()
                          - tunnelStations[i].y();
        chordAngles[i] = std::atan2(dx, dz);
    }
    std::vector<G4double> stationAngles(tunnelStations.size(), 0.0);
    stationAngles.front() = chordAngles.front();
    stationAngles.back() = chordAngles.back();
    for (std::size_t i = 1; i + 1 < tunnelStations.size(); ++i) {
        stationAngles[i] = std::atan2(
                std::sin(chordAngles[i-1]) + std::sin(chordAngles[i]),
                std::cos(chordAngles[i-1]) + std::cos(chordAngles[i]));
    }

    // Detector veto strips deliberately straddle the nominal tunnel
    // boundary, so the tunnel-air navigation solid and the rock cutout
    // both use this slightly expanded profile. That guarantees the two
    // independently-built solids share exactly the same inner surface
    // with no razor-thin navigation gap between them.
    std::vector<G4TwoVector> fastTunnelProfile = curvedTunnelProfile;
    {
        const G4double navigationAllowance = 4.0 * cm;
        for (std::size_t i = 0; i < fastTunnelProfile.size(); ++i) {
            const G4double x = fastTunnelProfile[i].x();
            fastTunnelProfile[i].setX(x + (x >= 0.0 ? navigationAllowance : -navigationAllowance));
            fastTunnelProfile[i].setY(fastTunnelProfile[i].y() +
                    (fastTunnelProfile[i].y() <= tunnelFloorY ? -navigationAllowance : navigationAllowance));
        }
    }

    auto makeFastExtrudedSolid = [](const G4String& name,
                                    const std::vector<G4TwoVector>& profile,
                                    G4double halfLength) -> G4VSolid* {
        return new G4ExtrudedSolid(name, profile, halfLength,
                                   G4TwoVector(0.0, 0.0), 1.0,
                                   G4TwoVector(0.0, 0.0), 1.0);
    };

    // One-metre finite rock envelope around the tunnel, offset outward
    // from the same cross-section used for the air.
    const G4double concreteThickness = 1.0 * m;
    auto cross2D = [](const G4TwoVector& a, const G4TwoVector& b) {
        return a.x() * b.y() - a.y() * b.x();
    };
    std::vector<G4TwoVector> outerRockProfile;
    outerRockProfile.reserve(curvedTunnelProfile.size());
    for (std::size_t i = 0; i < curvedTunnelProfile.size(); ++i) {
        const std::size_t previous =
                (i + curvedTunnelProfile.size() - 1) % curvedTunnelProfile.size();
        const std::size_t next = (i + 1) % curvedTunnelProfile.size();
        G4TwoVector previousDirection(
                curvedTunnelProfile[i].x() - curvedTunnelProfile[previous].x(),
                curvedTunnelProfile[i].y() - curvedTunnelProfile[previous].y());
        G4TwoVector nextDirection(
                curvedTunnelProfile[next].x() - curvedTunnelProfile[i].x(),
                curvedTunnelProfile[next].y() - curvedTunnelProfile[i].y());
        previousDirection = G4TwoVector(previousDirection.x() / previousDirection.mag(),
                                             previousDirection.y() / previousDirection.mag());
        nextDirection = G4TwoVector(nextDirection.x() / nextDirection.mag(),
                                    nextDirection.y() / nextDirection.mag());
        const G4TwoVector previousOutward(previousDirection.y(),
                                          -previousDirection.x());
        const G4TwoVector nextOutward(nextDirection.y(), -nextDirection.x());
        const G4TwoVector previousPoint = curvedTunnelProfile[i]
                                        + concreteThickness * previousOutward;
        const G4TwoVector nextPoint = curvedTunnelProfile[i]
                                    + concreteThickness * nextOutward;
        const G4double denominator = cross2D(previousDirection, nextDirection);
        if (std::fabs(denominator) > 1.0e-12) {
            const G4double distance =
                    cross2D(nextPoint - previousPoint, nextDirection)
                    / denominator;
            outerRockProfile.push_back(previousPoint
                                     + distance * previousDirection);
        } else {
            outerRockProfile.push_back(previousPoint);
        }
    }

    // Air world containing a finite rock shell and a distinct air tunnel.
    // Only the envelope around the selected chords is concrete; the rest
    // of the world is air, avoiding needless particle transport through a
    // huge rock box.
    G4double stationMinX = tunnelStations.front().x();
    G4double stationMaxX = tunnelStations.front().x();
    G4double stationMinZ = tunnelStations.front().y();
    G4double stationMaxZ = tunnelStations.front().y();
    for (std::size_t i = 1; i < tunnelStations.size(); ++i) {
        stationMinX = std::min(stationMinX, tunnelStations[i].x());
        stationMaxX = std::max(stationMaxX, tunnelStations[i].x());
        stationMinZ = std::min(stationMinZ, tunnelStations[i].y());
        stationMaxZ = std::max(stationMaxZ, tunnelStations[i].y());
    }
    G4double outerMaxAbsX = 0.0;
    G4double outerMaxAbsY = 0.0;
    for (std::size_t i = 0; i < outerRockProfile.size(); ++i) {
        outerMaxAbsX = std::max(outerMaxAbsX, std::fabs(outerRockProfile[i].x()));
        outerMaxAbsY = std::max(outerMaxAbsY, std::fabs(outerRockProfile[i].y()));
    }
    const G4double worldPadding = 1.0 * m;
    const G4double curvedWorldHalfX =
            std::max(std::fabs(stationMinX), std::fabs(stationMaxX))
            + outerMaxAbsX + worldPadding;
    const G4double curvedWorldHalfY = outerMaxAbsY + worldPadding;
    const G4double curvedWorldHalfZ =
            std::max(std::fabs(stationMinZ), std::fabs(stationMaxZ))
            + outerMaxAbsX + worldPadding;

    G4Box* curvedWorldSolid = new G4Box(
            "world", curvedWorldHalfX, curvedWorldHalfY, curvedWorldHalfZ);
    G4LogicalVolume* curvedWorldLogic = new G4LogicalVolume(
            curvedWorldSolid, tunnelAir, "World", 0, 0, 0);
    G4VisAttributes* curvedWorldVis = new G4VisAttributes();
    curvedWorldVis->SetVisibility(false);
    curvedWorldLogic->SetVisAttributes(curvedWorldVis);
    G4VPhysicalVolume* curvedWorldPhysical = new G4PVPlacement(
            0, G4ThreeVector(), curvedWorldLogic, "World", 0, false,
            grVolumeID::TunnelAir, true);

    // ---------------------------------------------------------------------
    // Per-chord placement geometry, plus a pair of half-space "mitre"
    // cutters at every station. The forward cutter keeps everything on
    // the far side of the station along the local bisector normal (used
    // to trim a chord's near end); the backward cutter keeps everything
    // on the near side (used to trim the previous chord's far end). At
    // the two open ends of the selection the bisector angle equals the
    // chord's own angle, so the cut lands exactly on the flat end cap.
    // ---------------------------------------------------------------------
    std::vector<G4double> chordLengths(tunnelChordCount, 0.0);
    std::vector<G4ThreeVector> chordMidpoints(tunnelChordCount);
    std::vector<G4RotationMatrix*> chordRotations(tunnelChordCount);
    for (G4int i = 0; i < tunnelChordCount; ++i) {
        const G4double dx = tunnelStations[i + 1].x() - tunnelStations[i].x();
        const G4double dz = tunnelStations[i + 1].y() - tunnelStations[i].y();
        chordLengths[i] = std::sqrt(dx * dx + dz * dz);
        chordMidpoints[i] = G4ThreeVector(
                0.5 * (tunnelStations[i].x() + tunnelStations[i + 1].x()), 0.0,
                0.5 * (tunnelStations[i].y() + tunnelStations[i + 1].y()));
        chordRotations[i] = new G4RotationMatrix();
        chordRotations[i]->rotateY(-chordAngles[i]);
    }

    // ---------------------------------------------------------------------
    // Mitre-plane cutters, one pair per station. The forward cutter keeps
    // everything on the far side of the station along the local bisector
    // normal (used to trim a chord's near end); the backward cutter keeps
    // everything on the near side (used to trim the previous chord's far
    // end). At the two open ends of the whole selection, stationAngles
    // already equals the chord's own angle, so the cut lands exactly on
    // the flat end cap with no effect -- the same construction handles
    // interior joints and open ends uniformly.
    //
    // An earlier attempt at this used 50 m cutter boxes and found that
    // Geant4's polyhedron visualizer couldn't render the result. That
    // turned out to be neither about it being a Boolean solid (a single
    // intersection renders fine) nor about chaining two intersections in
    // a row (also fine in isolation) -- it was specifically the cutter
    // box's LATERAL size relative to the ~3 m tunnel cross-section that
    // pushed HepPolyhedronProcessor's CSG clipping into a numerically
    // degenerate regime. A cutter sized to comfortably cover the real
    // cross-section (a few metres, not fifty) renders correctly at every
    // scale tested.
    //
    // The cutter's REACH along the mitre normal is a separate axis and
    // must be sized independently: each cutter is a *finite* box standing
    // in for an infinite half-space, so if its far face doesn't clear the
    // entire chord it's trimming, that face silently amputates whatever
    // chord material lies beyond it -- not at the joint, but wherever the
    // box happens to end. With a cube-shaped cutter this was invisible for
    // short chords (their whole length fit inside the box) but chewed a
    // real, metres-wide bite out of every chord longer than twice the
    // cutter half-size, which is exactly what produced the visible gaps
    // in long straight runs. The fix keeps the lateral half-size small
    // (still just a few metres, so the polyhedron export stays healthy --
    // verified with a null-polyhedron sweep over every active volume in
    // the full 15-chord tunnel).
    //
    // The reach is sized PER STATION, from only the one or two chords that
    // actually meet there -- not from the longest chord anywhere in the
    // tunnel. A single tunnel-wide reach first seemed simpler, but it means
    // every station's cutter (and every Boolean solid built from it, right
    // down to the shortest chord at the far end of the tunnel) inherits the
    // extent of whichever chord happens to be longest -- for this tunnel,
    // the ~23 m first chord bloats every other station's cutter to match,
    // even ones joining two ~4 m chords deep in a bend. That extra extent
    // doesn't move the actual mitred surface, but it does blow up the
    // bounding box Geant4's navigator voxelises the ~5000-volume tunnel
    // world on, which turned a ~2 minute run into an 8+ minute one for the
    // full tunnel (a single-chord selection has nothing else for a bloated
    // box to collide with in the voxelisation, which is why that case
    // stayed fast). Matching each cutter to its own local chords keeps
    // every station's extent close to what it actually needs.
    // ---------------------------------------------------------------------
    const G4double miterBoxLateral = 8.0 * m;
    const G4double miterCutterMargin = 2.0 * m;
    std::vector<G4double> miterBoxReach(tunnelStations.size(), 0.0);
    for (std::size_t st = 0; st < tunnelStations.size(); ++st) {
        G4double localHalfChord = 0.0;
        if (st > 0) localHalfChord = std::max(localHalfChord, 0.5 * chordLengths[st - 1]);
        if (static_cast<G4int>(st) < tunnelChordCount) localHalfChord = std::max(localHalfChord, 0.5 * chordLengths[st]);
        miterBoxReach[st] = localHalfChord + miterCutterMargin;
    }
    std::vector<G4VSolid*> forwardCutters(tunnelStations.size(), 0);
    std::vector<G4VSolid*> backwardCutters(tunnelStations.size(), 0);
    for (std::size_t st = 0; st < tunnelStations.size(); ++st) {
        std::ostringstream boxName;
        boxName << "gargoyle_miter_box_" << st;
        G4Box* rawBox = new G4Box(boxName.str(), miterBoxLateral, miterBoxLateral, miterBoxReach[st]);
        G4RotationMatrix* boxRot = new G4RotationMatrix();
        boxRot->rotateY(-stationAngles[st]);
        const G4ThreeVector normal(std::sin(stationAngles[st]), 0.0,
                                   std::cos(stationAngles[st]));
        const G4ThreeVector stationPos(tunnelStations[st].x(), 0.0,
                                       tunnelStations[st].y());
        forwardCutters[st] = new G4DisplacedSolid(boxName.str() + "_fwd", rawBox,
                boxRot, stationPos + miterBoxReach[st] * normal);
        backwardCutters[st] = new G4DisplacedSolid(boxName.str() + "_bwd", rawBox,
                boxRot, stationPos - miterBoxReach[st] * normal);
    }

    // Builds the portion of chord `chordIndex`'s straight extrusion that
    // lies between the mitre planes at both of its ends, expressed
    // directly in world coordinates (so the result is placed with an
    // identity transform). `overhang` only has to comfortably clear how
    // far the mitre plane can reach into the chord at the sharpest bend
    // actually present in this tunnel (well under 1 m); unlike the cutter
    // box it doesn't need to be generous.
    const G4double chordOverhang = 2.0 * m;
    auto buildChordSegment = [&](const G4String& name,
                                 const std::vector<G4TwoVector>& profile,
                                 G4int chordIndex) -> G4VSolid* {
        G4VSolid* raw = makeFastExtrudedSolid(name + "_raw", profile,
                0.5 * chordLengths[chordIndex] + chordOverhang);
        G4VSolid* world = new G4DisplacedSolid(name + "_world", raw,
                chordRotations[chordIndex], chordMidpoints[chordIndex]);
        G4VSolid* trimmedNear = new G4IntersectionSolid(name + "_near", world,
                forwardCutters[chordIndex]);
        return new G4IntersectionSolid(name, trimmedNear,
                backwardCutters[chordIndex + 1]);
    };

    // ---------------------------------------------------------------------
    // Per-chord envelope: one mitred air volume per chord, holding that
    // chord's own rock shell and every active layer as its daughters,
    // instead of placing all of it flat under curvedWorldLogic.
    //
    // With everything flat, "full" mode puts on the order of 5000 active
    // volumes (mostly the ~10 cm phi strips) as direct siblings under one
    // mother, and every navigator step has to search across all of them.
    // That's a large part of why a heavy e+/gamma run went from ~2 minutes
    // to over 8: each of the many secondary tracks in a shower takes many
    // steps through the densely-divided tracker, and every step pays for
    // a search across the full flat sibling list plus a nested-Boolean-
    // solid Inside()/DistanceToIn() call on each candidate. Grouping each
    // chord's ~330 daughters under their own envelope turns that into a
    // ~15-way choice of envelope followed by a ~330-way search inside just
    // one of them -- an order of magnitude fewer candidates per step,
    // without changing what any daughter's solid or placement actually is.
    //
    // The envelope reuses outerRockProfile (built with the same
    // buildChordSegment mitre cut as the rock shell it contains), so
    // neighbouring envelopes meet exactly the same way neighbouring rock
    // segments already do, and its 1 m margin beyond the tunnel wall
    // comfortably contains every active layer, which reaches at most
    // ~40 cm beyond the wall.
    // ---------------------------------------------------------------------
    std::vector<G4LogicalVolume*> chordEnvelopeLogics(tunnelChordCount, 0);
    for (G4int i = 0; i < tunnelChordCount; ++i) {
        std::ostringstream envName;
        envName << "gargoyle_chord_envelope_" << i;
        G4VSolid* envSolid = buildChordSegment(envName.str() + "_solid", outerRockProfile, i);
        G4LogicalVolume* envLogic = new G4LogicalVolume(
                envSolid, tunnelAir, envName.str() + "_logic", 0, 0, 0);
        G4VisAttributes* envVis = new G4VisAttributes();
        envVis->SetVisibility(false);
        envLogic->SetVisAttributes(envVis);
        new G4PVPlacement(0, G4ThreeVector(), envLogic, envName.str() + "_phys",
                curvedWorldLogic, false, i, false);
        chordEnvelopeLogics[i] = envLogic;
    }

    // ---------------------------------------------------------------------
    // Rock shell, one exactly-mitred segment per selected chord, now a
    // daughter of that chord's own envelope rather than curvedWorldLogic.
    // ---------------------------------------------------------------------
    for (G4int i = 0; i < tunnelChordCount; ++i) {
        std::ostringstream rockName;
        rockName << "gargoyle_curved_rock_chord" << i;

        G4VSolid* rockOuterSeg = buildChordSegment(rockName.str() + "_outer",
                outerRockProfile, i);
        G4VSolid* rockCutoutSeg = buildChordSegment(rockName.str() + "_cutout",
                fastTunnelProfile, i);
        G4VSolid* rockSeg = new G4SubtractionSolid(rockName.str() + "_solid",
                rockOuterSeg, rockCutoutSeg);
        G4LogicalVolume* rockLogic = new G4LogicalVolume(
                rockSeg, tunnelRock, rockName.str() + "_logic", 0, 0, 0);
        G4VisAttributes* rockVis = new G4VisAttributes(
                G4Colour(0.55, 0.50, 0.45, 0.35));
        rockVis->SetVisibility(true);
        rockVis->SetForceSolid(true);
        rockLogic->SetVisAttributes(rockVis);
        new G4PVPlacement(0, G4ThreeVector(), rockLogic, "rockPhysic",
                chordEnvelopeLogics[i], false, grVolumeID::Rock, false);
    }

    // ---------------------------------------------------------------------
    // Cosmic-ray source shell: a thin, physically inert (air) volume that
    // hugs the outside of the rock shell using the exact same per-chord
    // mitred contour, offset above the rock's true ceiling apex. This lets
    // the cosmic macros use /gps/pos/type Volume + /gps/pos/confine to
    // sample muon vertices over the whole curved tunnel's overburden
    // footprint instead of one flat rectangle sized for a single chord
    // group. Every chord's placement shares the same physical-volume name
    // ("gargoyle_cosmicSource"), so GPS's confine finds all of them and
    // combines them weighted by volume -- since the shell's cross-section
    // is identical at every chord, that's exactly proportional to arc
    // length, so longer chords naturally get proportionally more source
    // area with no per-chord weighting needed in the macro.
    //
    // Kept as a thin slab (not a full wrap of the rock cross-section) on
    // purpose: cosmic muons only ever arrive from the sky, so only the
    // rock's upward-facing side needs a source region above it. A full
    // wrap would also generate primaries under the floor and outside the
    // walls, most of which would be wasted (or wrong) given the
    // downward-biased zenith-angle distribution.
    // ---------------------------------------------------------------------
    G4double outerRockCeilingY = outerRockProfile.front().y();
    for (const auto& point : outerRockProfile) {
        outerRockCeilingY = std::max(outerRockCeilingY, point.y());
    }
    const G4double cosmicSourceMargin = 20.0 * cm;
    const G4double cosmicSourceThickness = 10.0 * cm;
    // 1 m margin beyond the rock's own outer half-width (2.45 m: 1.45 m
    // tunnel cavity + 1.0 m rock shell) on each side, everywhere along the
    // curve -- 6.9 m full width, rather than hugging the rock's own outer
    // width with no slack.
    const G4double cosmicSourceHalfWidth = 345.0 * cm;
    const G4double cosmicSourceInnerY = outerRockCeilingY + cosmicSourceMargin;
    const G4double cosmicSourceOuterY = cosmicSourceInnerY + cosmicSourceThickness;
    std::vector<G4TwoVector> cosmicSourceProfile = {
        G4TwoVector(-cosmicSourceHalfWidth, cosmicSourceInnerY),
        G4TwoVector( cosmicSourceHalfWidth, cosmicSourceInnerY),
        G4TwoVector( cosmicSourceHalfWidth, cosmicSourceOuterY),
        G4TwoVector(-cosmicSourceHalfWidth, cosmicSourceOuterY),
    };
    for (G4int i = 0; i < tunnelChordCount; ++i) {
        std::ostringstream sourceName;
        sourceName << "gargoyle_cosmic_source_chord" << i;
        G4VSolid* sourceSeg = buildChordSegment(sourceName.str() + "_solid",
                cosmicSourceProfile, i);
        G4LogicalVolume* sourceLogic = new G4LogicalVolume(
                sourceSeg, tunnelAir, sourceName.str() + "_logic", 0, 0, 0);
        G4VisAttributes* sourceVis = new G4VisAttributes(
                G4Colour(0.35, 0.75, 1.0, 0.25));
        sourceVis->SetVisibility(true);
        sourceVis->SetForceSolid(true);
        sourceLogic->SetVisAttributes(sourceVis);
        new G4PVPlacement(0, G4ThreeVector(), sourceLogic, "gargoyle_cosmicSource",
                curvedWorldLogic, false, i, false);
    }

    // ---------------------------------------------------------------------
    // Hermetic detector layers, one straight segment per selected chord.
    // The floor, walls, wall extensions, phi-segmented tracker strips, and
    // the z-longitudinal tracker are all built with buildChordSegment, the
    // same exact mitre cut used for the rock: neighbouring chords meet
    // with no gap and no overlap at every joint. The z-longitudinal
    // tracker used to be split into ~10 cm replica cells for extra
    // longitudinal resolution; see the comment near trackerSegmentWidth
    // below for why that was dropped in favour of one mitred volume per
    // chord.
    // ---------------------------------------------------------------------
    G4Material* matPlScin = G4Material::GetMaterial("plScintillator", false);
    if (!matPlScin) {
        G4Element* elH = tunnelNist->FindOrBuildElement("H");
        G4Element* elC = tunnelNist->FindOrBuildElement("C");
        matPlScin = new G4Material("plScintillator", 1.032 * g / cm3, 2);
        matPlScin->AddElement(elC, 10);
        matPlScin->AddElement(elH, 11);
    }
    matPlScin->GetIonisation()->SetBirksConstant(0.126 * mm / MeV);

    typedef std::vector<G4TwoVector> CrossPath;
    auto segmentNormal = [](const G4TwoVector& a, const G4TwoVector& b) {
        const G4double dx = b.x() - a.x(), dy = b.y() - a.y();
        const G4double length = std::sqrt(dx * dx + dy * dy);
        return G4TwoVector(-dy / length, dx / length);
    };
    auto offsetPath = [&](const CrossPath& path, G4double offset) {
        CrossPath result;
        result.reserve(path.size());
        for (std::size_t i = 0; i < path.size(); ++i) {
            G4TwoVector n;
            if (i == 0) n = segmentNormal(path[0], path[1]);
            else if (i + 1 == path.size()) n = segmentNormal(path[i - 1], path[i]);
            else {
                const G4TwoVector n0 = segmentNormal(path[i - 1], path[i]);
                const G4TwoVector n1 = segmentNormal(path[i], path[i + 1]);
                G4TwoVector sum(n0.x() + n1.x(), n0.y() + n1.y());
                const G4double sumLength = std::sqrt(sum.x() * sum.x() + sum.y() * sum.y());
                sum = G4TwoVector(sum.x() / sumLength, sum.y() / sumLength);
                const G4double projection = sum.x() * n0.x() + sum.y() * n0.y();
                n = G4TwoVector(sum.x() / projection, sum.y() / projection);
            }
            result.push_back(G4TwoVector(path[i].x() + offset * n.x(),
                                         path[i].y() + offset * n.y()));
        }
        return result;
    };
    auto bandProfile = [&](const CrossPath& path, G4double inner, G4double thickness) {
        const CrossPath inside = offsetPath(path, inner);
        const CrossPath outside = offsetPath(path, inner + thickness);
        CrossPath profile = inside;
        for (std::size_t i = outside.size(); i > 0; --i) profile.push_back(outside[i - 1]);
        return profile;
    };
    auto alternateColour = [](const G4Colour& colour) {
        const G4double blend = 0.25;
        return G4Colour(colour.GetRed() * (1.0 - blend) + blend,
                        colour.GetGreen() * (1.0 - blend) + blend,
                        colour.GetBlue() * (1.0 - blend) + blend, colour.GetAlpha());
    };

    std::vector<G4LogicalVolume*> activeLogics;
    auto placeActive = [&](const G4String& name, const std::vector<G4TwoVector>& profile,
                           G4int copyNo, const G4Colour& colour) {
        for (G4int i = 0; i < tunnelChordCount; ++i) {
            std::ostringstream chordName;
            chordName << name << "_c" << i;
            G4VSolid* solid = buildChordSegment(chordName.str() + "_extruded_solid",
                    profile, i);
            G4LogicalVolume* logic = new G4LogicalVolume(solid,
                matPlScin, chordName.str() + "_logic", 0, 0, 0);
            G4VisAttributes* vis = new G4VisAttributes(colour);
            vis->SetVisibility(true); vis->SetForceSolid(true);
            logic->SetVisAttributes(vis);
            new G4PVPlacement(0, G4ThreeVector(), logic,
                chordName.str() + "_phys", chordEnvelopeLogics[i], false, copyNo, false);
            activeLogics.push_back(logic);
        }
    };

    const G4double trackerSegmentWidth = 10.0 * cm;

    // The z-longitudinal tracker used to be divided into ~10 cm replica
    // cells along each chord for longitudinal (z) tracking resolution.
    // That resolution isn't needed: G4PVReplica's kZAxis slicing requires
    // a mother solid with a constant cross-section along the replication
    // axis, which an exactly-mitred (angle-cut) envelope cannot have, so
    // the replica version was the one active layer that couldn't be
    // closed up with the rest. Each chord's z-tracker band is now a
    // single mitred volume, built with the same buildChordSegment/
    // placeActive path as the floor, walls, and phi strips -- exact,
    // gap-free, overlap-free joints, and one volume per chord instead of
    // O(chordLength / 10 cm) of them. Position within a chord still comes
    // from the hit's stored 3D coordinates, same as before.

    const G4double wallGap = 0.1 * cm, floorThickness = 2.0 * cm;
    const G4double sublayerThickness = 1.5 * cm, sublayerGap = 1.0 * mm;
    const G4double trackerThickness = 2.0 * sublayerThickness + sublayerGap;
    const G4double midWallY = tunnelFloorY + 0.5 * tunnelWallHeight;
    const G4double wallBottomY = tunnelFloorY + wallGap + floorThickness;
    CrossPath floorPath; floorPath.push_back(G4TwoVector(-tunnelHalfFloor, tunnelFloorY)); floorPath.push_back(G4TwoVector(tunnelHalfFloor, tunnelFloorY));
    CrossPath rightWallPath; rightWallPath.push_back(G4TwoVector(tunnelHalfFloor, wallBottomY)); rightWallPath.push_back(G4TwoVector(tunnelHalfArch, midWallY));
    CrossPath leftWallPath; leftWallPath.push_back(G4TwoVector(-tunnelHalfArch, midWallY)); leftWallPath.push_back(G4TwoVector(-tunnelHalfFloor, wallBottomY));
    CrossPath upperPath; upperPath.push_back(G4TwoVector(tunnelHalfArch, midWallY)); upperPath.push_back(G4TwoVector(tunnelHalfArch, tunnelSpringY));
    for (G4int i = 1; i < tunnelArchSegments; ++i) {
        const G4double angle = pi * i / tunnelArchSegments;
        upperPath.push_back(G4TwoVector(tunnelHalfArch * std::cos(angle), tunnelSpringY + tunnelArchHeight * std::sin(angle)));
    }
    upperPath.push_back(G4TwoVector(-tunnelHalfArch, tunnelSpringY)); upperPath.push_back(G4TwoVector(-tunnelHalfArch, midWallY));

    CrossPath segmentedUpperPath; segmentedUpperPath.push_back(upperPath.front());
    for (std::size_t edge = 0; edge + 1 < upperPath.size(); ++edge) {
        const G4TwoVector delta(upperPath[edge + 1].x() - upperPath[edge].x(), upperPath[edge + 1].y() - upperPath[edge].y());
        const G4double length = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());
        const G4int pieces = std::max(1, static_cast<G4int>(std::ceil(length / trackerSegmentWidth)));
        for (G4int piece = 1; piece <= pieces; ++piece) {
            const G4double fraction = static_cast<G4double>(piece) / pieces;
            segmentedUpperPath.push_back(G4TwoVector(upperPath[edge].x() + fraction * delta.x(), upperPath[edge].y() + fraction * delta.y()));
        }
    }

    placeActive("gargoyle_scint_phys_floor", bandProfile(floorPath, wallGap, floorThickness), 100, G4Colour::Cyan());
    placeActive("gargoyle_scint_phys_walls_right", bandProfile(rightWallPath, wallGap, trackerThickness), 110, G4Colour::Cyan());
    placeActive("gargoyle_scint_phys_walls_left", bandProfile(leftWallPath, wallGap, trackerThickness), 111, G4Colour::Cyan());
    const G4double stationGaps[4] = {0.0*cm, 12.0*cm, 24.0*cm, 36.0*cm};
    const G4int layerNames[4] = {1, 3, 2, 5};
    const G4int phiIDs[4] = {1000, 2000, 3000, 4000};
    const G4int zIDs[4] = {10000, 20000, 30000, 40000};
    const G4Colour phiColours[4] = {G4Colour(0.1,0.8,0.1,.75), G4Colour(0.2,.85,.5,.75), G4Colour(.9,.9,.1,.75), G4Colour(.95,.55,.2,.75)};
    const G4Colour zColours[4] = {G4Colour(.1,.55,.95,.75), G4Colour(.2,.65,.95,.75), G4Colour(.95,.45,.1,.75), G4Colour(.95,.25,.55,.75)};
    for (G4int station = 0; station < 4; ++station) {
        const G4double base = station == 0 ? wallGap : wallGap + trackerThickness + stationGaps[station];
        std::ostringstream phiName, zName; phiName << "gargoyle_si_layer" << layerNames[station]; zName << phiName.str() << "_z";
        for (std::size_t strip = 0; strip + 1 < segmentedUpperPath.size(); ++strip) {
            CrossPath stripPath; stripPath.push_back(segmentedUpperPath[strip]); stripPath.push_back(segmentedUpperPath[strip + 1]);
            std::ostringstream stripName; stripName << phiName.str() << "_" << strip;
            placeActive(stripName.str(), bandProfile(stripPath, base, sublayerThickness),
                        phiIDs[station] + static_cast<G4int>(strip),
                        (strip % 2) ? alternateColour(phiColours[station]) : phiColours[station]);
        }
        placeActive(zName.str(), bandProfile(upperPath, base + sublayerThickness + sublayerGap, sublayerThickness),
                    zIDs[station], zColours[station]);
        if (station > 0) {
            const G4int wallBase = 100 + 10 * (station + 1);
            std::ostringstream wr, wl; wr << "gargoyle_scint_phys_wall_extension_" << station << "_right"; wl << "gargoyle_scint_phys_wall_extension_" << station << "_left";
            placeActive(wr.str(), bandProfile(rightWallPath, base, trackerThickness), wallBase, phiColours[station]);
            placeActive(wl.str(), bandProfile(leftWallPath, base, trackerThickness), wallBase + 1, phiColours[station]);
        }
    }
    G4SDManager* sdManager = G4SDManager::GetSDMpointer();
    grScintSD* scintSD = dynamic_cast<grScintSD*>(sdManager->FindSensitiveDetector("Scint_SD", false));
    if (!scintSD) { scintSD = new grScintSD("Scint_SD"); sdManager->AddNewDetector(scintSD); }
    for (std::size_t i = 0; i < activeLogics.size(); ++i) activeLogics[i]->SetSensitiveDetector(scintSD);
    this->SetNLayer(12); this->SetNBarPerLayer(1);
    G4cout << "  curved detector active physical volumes: " << activeLogics.size() << G4endl;
    G4cout << "  tracker segmentation: 10 cm transverse and longitudinal" << G4endl;
    G4cout << "  tunnel segment groups: " << tunnelMode << G4endl;

    if (verbose >= 0) {
        G4cout << "Curved GARGOYLE tunnel summary:" << G4endl;
        G4cout << "  centerline length: "
               << G4BestUnit(constructedTunnelLength, "Length") << G4endl;
        G4cout << "  representative centerline chords: "
               << tunnelChordCount << G4endl;
        G4cout << "  cross-section: "
               << G4BestUnit(tunnelFloorWidth, "Length") << " wide x "
               << G4BestUnit(tunnelHeight, "Length") << " high" << G4endl;
        G4cout << "  arch facets: " << tunnelArchSegments << G4endl;
        G4cout << "  world half-size: ("
               << G4BestUnit(curvedWorldHalfX, "Length") << ", "
               << G4BestUnit(curvedWorldHalfY, "Length") << ", "
               << G4BestUnit(curvedWorldHalfZ, "Length") << ")" << G4endl;
        G4cout << "  cosmic source shell transverse width: "
               << G4BestUnit(2.0 * cosmicSourceHalfWidth, "Length")
               << " (rock outer half-width "
               << G4BestUnit(outerMaxAbsX, "Length") << " + "
               << G4BestUnit(cosmicSourceHalfWidth - outerMaxAbsX, "Length")
               << " margin, each side)" << G4endl;
        G4cout << "  cosmic source shell: X in ["
               << G4BestUnit(stationMinX - cosmicSourceHalfWidth, "Length") << ", "
               << G4BestUnit(stationMaxX + cosmicSourceHalfWidth, "Length")
               << "], Z in ["
               << G4BestUnit(stationMinZ - cosmicSourceHalfWidth, "Length") << ", "
               << G4BestUnit(stationMaxZ + cosmicSourceHalfWidth, "Length")
               << "], Y in ["
               << G4BestUnit(cosmicSourceInnerY, "Length") << ", "
               << G4BestUnit(cosmicSourceOuterY, "Length") << "]" << G4endl;
    }

    return curvedWorldPhysical;
}
void grDetectorConstruction::SetMagField(G4double fieldValueX, G4double fieldValueY, G4double fieldValueZ) {

  fMonFieldSetup->SetMagField(fieldValueX, fieldValueY, fieldValueZ);

}

G4MaterialPropertiesTable* grDetectorConstruction::SetOpticalPropertiesOfPS(){


G4MaterialPropertiesTable* mptPlScin = new G4MaterialPropertiesTable();

 const G4int nEntries= 43;//301;//100;

  G4double EJ200_SCINT[nEntries];
  G4double EJ200_RIND[nEntries];
  G4double EJ200_ABSL[nEntries];
  G4double photonEnergy[nEntries];

  std::ifstream ReadEJ200;
  G4int ScintEntry=0;
  G4String filler;
  G4double pEnergy;
  G4double pWavelength;
  G4double pSEff;
  G4cout << "grOpticalFilePath is: " << grOpticalFilePath << G4endl;
  ReadEJ200.open(grOpticalFilePath+"EJ200ScintSpectrum.txt");
  if(ReadEJ200.is_open()){
  while(!ReadEJ200.eof()){
  ReadEJ200 >> pWavelength >> pSEff;
  pEnergy = (1240/pWavelength)*eV;
  photonEnergy[ScintEntry] = pEnergy;
  EJ200_SCINT[ScintEntry] = pSEff;
  G4cout << "read-in energy scint: " << photonEnergy[ScintEntry] << " eff: " << EJ200_SCINT[ScintEntry] << G4endl;
  ScintEntry++;
  }
  }
  else
  G4cout << "Error opening file: " << "EJ200ScintSpectrum.txt" << G4endl;
  ReadEJ200.close();

  for (int i = 0; i < nEntries; i++) {
    EJ200_RIND[i] = 1.58;//58; // refractive index at 425 nm
    //EJ200_ABSL[i] *= myPSAttenuationLength;
    EJ200_ABSL[i] = 3.8*m;//2.5 * m; // bulk attenuation at 425 nm
  }


  mptPlScin->AddProperty("FASTCOMPONENT", photonEnergy, EJ200_SCINT, nEntries);
        mptPlScin->AddConstProperty("FASTTIMECONSTANT", 2.1 * ns); //decay time, according to EJ200

  mptPlScin->AddProperty("ABSLENGTH", photonEnergy, EJ200_ABSL,
        nEntries);//->SetSpline(true);

  mptPlScin->AddConstProperty("SCINTILLATIONYIELD", 10000. / MeV); //--- according to EJ200
  mptPlScin->AddConstProperty("RESOLUTIONSCALE", 1.0);
  mptPlScin->AddProperty("RINDEX", photonEnergy, EJ200_RIND, nEntries);//->SetSpline(true);

return mptPlScin;
}

G4MaterialPropertiesTable* grDetectorConstruction::SetOpticalPropertiesOfPMT(){

  G4MaterialPropertiesTable* mptPMT = new G4MaterialPropertiesTable();

  const G4int nEntriesPMT = 41;//27;//36;//2;//37;//15; //just figure out how many entries are in the text file,
                //unless you're not lazy like me and want to dynamically allocate memory
//* PMT QEff read-in (updated Jan 2018)
  G4double photonEnergyPMT[nEntriesPMT];
  G4double photocath_EFF[nEntriesPMT];
  G4double PhCath_REFL[nEntriesPMT]; // to be determined

  std::ifstream ReadPMTQEff;
  G4int PMTEntry=0;
  G4String filler;
  G4double pEnergy;
  G4double pWavelength;
  G4double pQEff;
  ReadPMTQEff.open(grOpticalFilePath+"PMT_R878_QE_orig.txt");
  if(ReadPMTQEff.is_open()){
  while(!ReadPMTQEff.eof()){
  ReadPMTQEff >> pWavelength >> pQEff;
  pQEff=1;
  pEnergy = (1240/pWavelength)*eV;
  photonEnergyPMT[PMTEntry] = pEnergy;
  photocath_EFF[PMTEntry] = pQEff;
  PhCath_REFL[PMTEntry] = 0;
  G4cout << "read-in energy: " << photonEnergyPMT[PMTEntry] << " eff: " << photocath_EFF[PMTEntry] << G4endl;
  PMTEntry++;
  }
  }
  else
  G4cout << "Error opening file: " << "PMT_R878_QE.txt" << G4endl;
  ReadPMTQEff.close();


  mptPMT->AddProperty("REFLECTIVITY", photonEnergyPMT,PhCath_REFL, nEntriesPMT);//->SetSpline(true);
  mptPMT->AddProperty("EFFICIENCY",photonEnergyPMT,photocath_EFF,nEntriesPMT);//->SetSpline(true);

  G4cout << "successfully at end of PMT optical table" << G4endl;

  return mptPMT;

}

G4PhysicsVector grDetectorConstruction::GetPMTEff_R878(){
  std::ifstream ReadPMTQEff;
  ReadPMTQEff.open(grOpticalFilePath+"PMT_R878_QE.txt");
  G4PhysicsVector effVec;
  effVec.Retrieve(ReadPMTQEff,true);
  if (effVec.GetVectorLength()!=0) G4cout << "Quantum Efficiency successfully retrieved for PMT_R878_QE" << G4endl;
  else G4cout << "ERROR: Vector length is zero!" << G4endl;
  effVec.ScaleVector(1,1);
  ReadPMTQEff.close();
  return effVec;
}

G4PhysicsVector grDetectorConstruction::GetPMTEff_R7725(){
  std::ifstream ReadPMTQEff;
  ReadPMTQEff.open(grOpticalFilePath+"PMT_R7725_QE.txt");
  G4PhysicsVector effVec;
  effVec.Retrieve(ReadPMTQEff,true);
  if (effVec.GetVectorLength()!=0) G4cout << "Quantum Efficiency successfully retrieved for PMT R7725" << G4endl;
  else G4cout << "ERROR: Vector length is zero!" << G4endl;
  effVec.ScaleVector(1,1);
  ReadPMTQEff.close();
  return effVec;
}

G4PhysicsVector grDetectorConstruction::GetPMTEff_ET9814B(){
  std::ifstream ReadPMTQEff;
  ReadPMTQEff.open(grOpticalFilePath+"PMT_ET9814B_QE.txt");
  G4PhysicsVector effVec;
  effVec.Retrieve(ReadPMTQEff,true);
  if (effVec.GetVectorLength()!=0) G4cout << "Quantum Efficiency successfully retrieved for PMT ET9814B" << G4endl;
  else G4cout << "ERROR: Vector length is zero!" << G4endl;
  effVec.ScaleVector(1,1);
  ReadPMTQEff.close();
  return effVec;
}

void grDetectorConstruction::UpdateGeometry() {
  if (verbose >= 0) {
    G4cout << "grendelSim> Update geometry." << G4endl;
    G4cout
        << "               Don't use this command explicitly, it's obsolete and can crash the run."
        << G4endl;
  }
  // clean-up previous geometry
    G4GeometryManager::GetInstance()->OpenGeometry();
    G4PhysicalVolumeStore::GetInstance()->Clean();
    G4LogicalVolumeStore::GetInstance()->Clean();
    G4SolidStore::GetInstance()->Clean();
    G4LogicalSkinSurface::CleanSurfaceTable();
    G4LogicalBorderSurface::CleanSurfaceTable();
    G4SurfaceProperty::CleanSurfacePropertyTable();
    //define new one
  G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
  updated=false;
}


void grDetectorConstruction::SetDefaults(){
  //Resets to default values


  updated=true;
}
