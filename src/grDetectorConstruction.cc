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
#include "G4MultiUnion.hh"
#include "G4SubtractionSolid.hh"
#include "G4TessellatedSolid.hh"
#include "G4TriangularFacet.hh"
#include "G4QuadrangularFacet.hh"
#include "G4GeomTools.hh"
#include "G4Transform3D.hh"
#include "G4Exception.hh"

#include <vector>
#include <utility>
#include <cmath>
#include <algorithm>

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
    // The air volume is one voxelized G4MultiUnion made from short native
    // G4ExtrudedSolid chords.  This keeps navigation substantially cheaper
    // than a long chain of Boolean unions/subtractions or a fine tessellated
    // sweep, while retaining the bends that matter for detector design.
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
    const G4double tunnelTopY = tunnelFloorY + tunnelHeight;

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
    const G4int tunnelChordCount =
            static_cast<G4int>(representativePixels.size()) - 1;
    std::vector<G4TwoVector> tunnelStations;
    tunnelStations.reserve(representativePixels.size());
    for (std::size_t i = 0; i < representativePixels.size(); ++i) {
        tunnelStations.push_back(G4TwoVector(
                (representativePixels[i].x() - centerlineOffsetX) * representativeScale,
                (representativePixels[i].y() - centerlineOffsetZ) * representativeScale));
    }

    // ---------------------------------------------------------------------
    // Build the one-piece air tunnel.  Neighboring chords overlap only enough
    // to close the small outside wedge at a bend; G4MultiUnion removes those
    // internal faces before navigation.  Voxelize() is essential for runtime.
    // ---------------------------------------------------------------------
    std::vector<G4double> chordAngles(tunnelChordCount, 0.0);
    for (G4int i = 0; i < tunnelChordCount; ++i) {
        const G4double dx = tunnelStations[i + 1].x()
                          - tunnelStations[i].x();
        const G4double dz = tunnelStations[i + 1].y()
                          - tunnelStations[i].y();
        chordAngles[i] = std::atan2(dx, dz);
    }

    // A single closed swept polyhedron avoids the expensive visualization-time
    // Boolean polygonization performed by G4MultiUnion::CreatePolyhedron().
    G4TessellatedSolid* curvedTunnelSolid =
            new G4TessellatedSolid("gargoyle_curved_tunnel_air_solid");
    const G4double maximumLateralHalfWidth =
            std::max(tunnelHalfFloor, tunnelHalfArch);

    std::vector<G4double> stationAngles(tunnelStations.size(), 0.0);
    stationAngles.front() = chordAngles.front();
    stationAngles.back() = chordAngles.back();
    for (std::size_t i = 1; i + 1 < tunnelStations.size(); ++i) {
        stationAngles[i] = std::atan2(
                std::sin(chordAngles[i-1]) + std::sin(chordAngles[i]),
                std::cos(chordAngles[i-1]) + std::cos(chordAngles[i]));
    }

    typedef std::vector<G4ThreeVector> TunnelRing;
    std::vector<TunnelRing> tunnelRings;
    tunnelRings.reserve(tunnelStations.size());
    for (std::size_t station = 0; station < tunnelStations.size(); ++station) {
        TunnelRing ring;
        ring.reserve(curvedTunnelProfile.size());
        const G4double c = std::cos(stationAngles[station]);
        const G4double sineAngle = std::sin(stationAngles[station]);
        for (std::size_t vertex = 0; vertex < curvedTunnelProfile.size(); ++vertex) {
            const G4double lateral = curvedTunnelProfile[vertex].x();
            ring.push_back(G4ThreeVector(
                    tunnelStations[station].x() + c * lateral,
                    curvedTunnelProfile[vertex].y(),
                    tunnelStations[station].y() - sineAngle * lateral));
        }
        tunnelRings.push_back(ring);
    }

    const std::size_t profileVertices = curvedTunnelProfile.size();
    for (std::size_t station = 0; station + 1 < tunnelRings.size(); ++station) {
        for (std::size_t vertex = 0; vertex < profileVertices; ++vertex) {
            const std::size_t next = (vertex + 1) % profileVertices;
            curvedTunnelSolid->AddFacet(new G4TriangularFacet(
                    tunnelRings[station][vertex], tunnelRings[station][next],
                    tunnelRings[station+1][next], ABSOLUTE));
            curvedTunnelSolid->AddFacet(new G4TriangularFacet(
                    tunnelRings[station][vertex], tunnelRings[station+1][next],
                    tunnelRings[station+1][vertex], ABSOLUTE));
        }
    }
    for (std::size_t vertex = 1; vertex + 1 < profileVertices; ++vertex) {
        curvedTunnelSolid->AddFacet(new G4TriangularFacet(
                tunnelRings.front()[0], tunnelRings.front()[vertex+1],
                tunnelRings.front()[vertex], ABSOLUTE));
        curvedTunnelSolid->AddFacet(new G4TriangularFacet(
                tunnelRings.back()[0], tunnelRings.back()[vertex],
                tunnelRings.back()[vertex+1], ABSOLUTE));
    }
    curvedTunnelSolid->SetSolidClosed(true);

    // Concrete world with a single air daughter is both physically natural
    // and faster than subtracting the tunnel from a large rock solid.
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

    const G4double rockPadding = 5.0 * m;
    const G4double curvedWorldHalfX =
            std::max(std::fabs(stationMinX), std::fabs(stationMaxX))
            + maximumLateralHalfWidth + rockPadding;
    const G4double curvedWorldHalfY =
            std::max(std::fabs(tunnelFloorY), std::fabs(tunnelTopY))
            + rockPadding;
    const G4double curvedWorldHalfZ =
            std::max(std::fabs(stationMinZ), std::fabs(stationMaxZ))
            + maximumLateralHalfWidth + rockPadding;

    G4Box* curvedWorldSolid = new G4Box(
            "world", curvedWorldHalfX, curvedWorldHalfY, curvedWorldHalfZ);
    G4LogicalVolume* curvedWorldLogic = new G4LogicalVolume(
            curvedWorldSolid, tunnelRock, "World", 0, 0, 0);
    G4VisAttributes* curvedWorldVis = new G4VisAttributes();
    curvedWorldVis->SetVisibility(false);
    curvedWorldLogic->SetVisAttributes(curvedWorldVis);

    G4VPhysicalVolume* curvedWorldPhysical = new G4PVPlacement(
            0, G4ThreeVector(), curvedWorldLogic, "rockPhysic", 0, false,
            grVolumeID::Rock, true);

    G4LogicalVolume* curvedTunnelLogic = new G4LogicalVolume(
            curvedTunnelSolid, tunnelAir, "gargoyle_curved_tunnel_air_logic",
            0, 0, 0);
    G4VisAttributes* curvedTunnelVis = new G4VisAttributes(
            G4Colour(0.20, 0.75, 1.00, 0.25));
    curvedTunnelVis->SetVisibility(true);
    curvedTunnelVis->SetForceSolid(true);
    curvedTunnelLogic->SetVisAttributes(curvedTunnelVis);
    new G4PVPlacement(0, G4ThreeVector(), curvedTunnelLogic,
            "gargoyle_curved_tunnel_air_phys", curvedWorldLogic,
            false, grVolumeID::TunnelAir, false);

    // ---------------------------------------------------------------------
    // Hermetic detector shells swept through the same centerline chords.
    // Eight tracker shells and the veto shells remain continuous through bends;
    // each is one voxelized solid, avoiding O(pathLength/pitch) placements.
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
    auto sweepProfile = [&](const G4String& name, const CrossPath& inputProfile) {
        CrossPath profile = inputProfile;
        G4double signedArea = 0.0;
        for (std::size_t i = 0; i < profile.size(); ++i) {
            const std::size_t next = (i + 1) % profile.size();
            signedArea += profile[i].x() * profile[next].y()
                        - profile[next].x() * profile[i].y();
        }
        if (signedArea < 0.0) std::reverse(profile.begin(), profile.end());

        G4TessellatedSolid* solid = new G4TessellatedSolid(name + "_solid");
        std::vector<TunnelRing> rings;
        rings.reserve(tunnelStations.size());
        for (std::size_t station = 0; station < tunnelStations.size(); ++station) {
            TunnelRing ring;
            ring.reserve(profile.size());
            const G4double cosineAngle = std::cos(stationAngles[station]);
            const G4double sineAngle = std::sin(stationAngles[station]);
            for (std::size_t vertex = 0; vertex < profile.size(); ++vertex) {
                const G4double lateral = profile[vertex].x();
                ring.push_back(G4ThreeVector(
                        tunnelStations[station].x() + cosineAngle * lateral,
                        profile[vertex].y(),
                        tunnelStations[station].y() - sineAngle * lateral));
            }
            rings.push_back(ring);
        }
        for (std::size_t station = 0; station + 1 < rings.size(); ++station) {
            for (std::size_t vertex = 0; vertex < profile.size(); ++vertex) {
                const std::size_t next = (vertex + 1) % profile.size();
                solid->AddFacet(new G4TriangularFacet(
                        rings[station][vertex], rings[station][next],
                        rings[station+1][next], ABSOLUTE));
                solid->AddFacet(new G4TriangularFacet(
                        rings[station][vertex], rings[station+1][next],
                        rings[station+1][vertex], ABSOLUTE));
            }
        }
        std::vector<G4int> capTriangles;
        if (!G4GeomTools::TriangulatePolygon(profile, capTriangles)) {
            G4Exception("grDetectorConstruction::SetupGeometry", "InvalidShellProfile",
                        FatalException, "Could not triangulate detector shell profile.");
        }
        for (std::size_t i = 0; i + 2 < capTriangles.size(); i += 3) {
            const G4int a = capTriangles[i];
            const G4int b = capTriangles[i+1];
            const G4int c = capTriangles[i+2];
            const G4double triangleArea =
                    (profile[b].x() - profile[a].x()) * (profile[c].y() - profile[a].y())
                  - (profile[b].y() - profile[a].y()) * (profile[c].x() - profile[a].x());
            const G4int forwardB = triangleArea > 0.0 ? b : c;
            const G4int forwardC = triangleArea > 0.0 ? c : b;
            solid->AddFacet(new G4TriangularFacet(
                    rings.front()[a], rings.front()[forwardC],
                    rings.front()[forwardB], ABSOLUTE));
            solid->AddFacet(new G4TriangularFacet(
                    rings.back()[a], rings.back()[forwardB],
                    rings.back()[forwardC], ABSOLUTE));
        }
        solid->SetSolidClosed(true);
        return solid;
    };
    std::vector<G4LogicalVolume*> activeLogics;
    auto placeActive = [&](const G4String& name, const CrossPath& profile,
                           G4int copyNo, const G4Colour& colour) {
        G4LogicalVolume* logic = new G4LogicalVolume(sweepProfile(name, profile),
            matPlScin, name + "_logic", 0, 0, 0);
        G4VisAttributes* vis = new G4VisAttributes(colour);
        vis->SetVisibility(true); vis->SetForceSolid(true);
        logic->SetVisAttributes(vis);
        new G4PVPlacement(0, G4ThreeVector(), logic, name + "_phys",
            curvedTunnelLogic, false, copyNo, false);
        activeLogics.push_back(logic);
    };

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
        placeActive(phiName.str(), bandProfile(upperPath, base, sublayerThickness), phiIDs[station], phiColours[station]);
        placeActive(zName.str(), bandProfile(upperPath, base + sublayerThickness + sublayerGap, sublayerThickness), zIDs[station], zColours[station]);
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
    G4cout << "  curved detector active shells: " << activeLogics.size() << G4endl;

    if (verbose >= 0) {
        G4cout << "Curved GARGOYLE tunnel summary:" << G4endl;
        G4cout << "  centerline length: "
               << G4BestUnit(requestedTunnelPathLength, "Length") << G4endl;
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
