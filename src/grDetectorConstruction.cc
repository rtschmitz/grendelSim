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
#include "G4TessellatedSolid.hh"
#include "G4TriangularFacet.hh"
#include "G4QuadrangularFacet.hh"
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

    if (verbose >= 0) {
        G4cout << "grendelSim> Construct GARGOYLE tunnel geometry." << G4endl;
    }

    // -------------------------------------------------------------------------
    // Geometry convention
    //   X = horizontal transverse
    //   Y = vertical, positive up
    //   Z = tunnel axis
    //
    // The cross-section constants are copied from gargoyle_geometry.py.
    // This implementation makes the requested straight 10 m tunnel segment
    // along z, rather than the full survey-path extrusion used for ray-casting.
    // -------------------------------------------------------------------------

    G4NistManager* nistMan = G4NistManager::Instance();

    G4Material* worldMaterial = nistMan->FindOrBuildMaterial("G4_AIR");
    G4Material* concreteMat   = nistMan->FindOrBuildMaterial("G4_CONCRETE");
    G4Material* matPlScin = G4Material::GetMaterial("plScintillator", false);
    if (!matPlScin) {
        G4Element* elH = nistMan->FindOrBuildElement("H");
        G4Element* elC = nistMan->FindOrBuildElement("C");
        matPlScin = new G4Material("plScintillator", 1.032 * g / cm3, 2);
        matPlScin->AddElement(elC, 10);
        matPlScin->AddElement(elH, 11);
    }
    matPlScin->GetIonisation()->SetBirksConstant(0.126 * mm / MeV);

    // World: slightly larger than the 2 m-thick concrete envelope.
    const G4double worldHalfX = 6.0 * m;
    const G4double worldHalfY = 6.0 * m;
    const G4double worldHalfZ = 10.0 * m;

    G4Box* solidWorld = new G4Box("world", worldHalfX, worldHalfY, worldHalfZ);
    G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, worldMaterial, "World", 0, 0, 0);

    G4VisAttributes* visAttWorld = new G4VisAttributes();
    visAttWorld->SetVisibility(false);
    logicWorld->SetVisAttributes(visAttWorld);

    G4PVPlacement* physicWorld = new G4PVPlacement(0,
            G4ThreeVector(),
            logicWorld,
            "World",
            0,
            false,
            0,
            true);

    // GARGOYLE cross-section constants from gargoyle_geometry.py.
    const G4double tunnelAlpha = 2.90 * m;  // floor width
    const G4double tunnelBeta  = 3.15 * m;  // total height
    const G4double tunnelGamma = 2.90 * m;  // arch width at springline
    const G4double tunnelDelta = 1.90 * m;  // arch height
    const G4double wallHeight  = tunnelBeta - tunnelDelta;

    const G4double halfFloor = tunnelAlpha / 2.0;
    const G4double halfArch  = tunnelGamma / 2.0;

    // Same centroid convention as gargoyle_geometry.py:tunnel_profile_points().
    const G4double rectArea    = tunnelAlpha * wallHeight;
    const G4double rectCY      = wallHeight / 2.0;
    const G4double ellipseArea = pi * halfArch * tunnelDelta / 2.0;
    const G4double ellipseCY   = wallHeight + 4.0 * tunnelDelta / (3.0 * pi);
    const G4double centroidY   = (rectArea * rectCY + ellipseArea * ellipseCY)
                               / (rectArea + ellipseArea);

    const G4double floorY      = -centroidY;
    const G4double midWallY    = floorY + wallHeight / 2.0;
    const G4double springY     = floorY + wallHeight;
    const G4double tunnelTopY  = floorY + tunnelBeta;
    const G4double rockCenterY = 0.5 * (floorY + tunnelTopY);

    const G4double tunnelLength = 18.0 * m;
    const G4double tunnelHalfZ  = tunnelLength / 2.0;

    auto makeExtrudedSolid = [](const G4String& name,
                                const std::vector<G4TwoVector>& profile,
                                G4double halfZ) -> G4VSolid* {
        if (profile.size() < 3) {
            G4Exception("grDetectorConstruction::SetupGeometry",
                        "GARGOYLE_BAD_PROFILE", FatalException,
                        "Attempted to build an extruded solid with fewer than three profile points.");
        }

        // Native Geant4 extrusion is much faster than manually tessellating
        // every side/cap, especially for boolean subtraction and navigation.
        return new G4ExtrudedSolid(name,
                                   profile,
                                   halfZ,
                                   G4TwoVector(0.0, 0.0), 1.0,
                                   G4TwoVector(0.0, 0.0), 1.0);
    };

    // Rotate the whole tunnel cross-section about the global z axis.
    // +90 deg maps (x,y) -> (-y,x).  Use -90 deg if you want the opposite handedness.
    const G4double crossSectionRotZ = -90.0 * deg;
    const G4double rotC = std::cos(crossSectionRotZ);
    const G4double rotS = std::sin(crossSectionRotZ);

    auto rotateXY = [&](const G4TwoVector& p) -> G4TwoVector {
        return G4TwoVector(rotC * p.x() - rotS * p.y(),
                           rotS * p.x() + rotC * p.y());
    };

    auto rotateProfile = [&](const std::vector<G4TwoVector>& in) -> std::vector<G4TwoVector> {
        std::vector<G4TwoVector> out;
        out.reserve(in.size());
        for (std::size_t i = 0; i < in.size(); ++i) {
            out.push_back(rotateXY(in[i]));
        }
        return out;
    };

    // Build the tunnel boundary.  Points follow the same counter-clockwise
    // convention as gargoyle_geometry.py, with an explicit midpoint on each wall
    // so the scintillator/silicon transition is clean.
    const G4int nArchSegments = 400;
    std::vector<G4TwoVector> baseTunnelProfile;
    baseTunnelProfile.push_back(G4TwoVector(-halfFloor, floorY));
    baseTunnelProfile.push_back(G4TwoVector( halfFloor, floorY));
    baseTunnelProfile.push_back(G4TwoVector( halfArch,  midWallY));
    baseTunnelProfile.push_back(G4TwoVector( halfArch,  springY));
    for (G4int i = 1; i < nArchSegments; ++i) {
        const G4double angle = pi * static_cast<G4double>(i)
                             / static_cast<G4double>(nArchSegments);
        baseTunnelProfile.push_back(G4TwoVector(halfArch * std::cos(angle),
                                                springY + tunnelDelta * std::sin(angle)));
    }
    const std::size_t leftSpringIndex = baseTunnelProfile.size();
    baseTunnelProfile.push_back(G4TwoVector(-halfArch, springY));
    const std::size_t leftMidIndex = baseTunnelProfile.size();
    baseTunnelProfile.push_back(G4TwoVector(-halfArch, midWallY));

    std::vector<G4TwoVector> tunnelProfile = rotateProfile(baseTunnelProfile);

    G4VSolid* tunnelCutoutSolid = makeExtrudedSolid(
            "gargoyle_tunnel_cutout_solid", tunnelProfile, tunnelHalfZ + 1.0 * cm);

    // Concrete/rock envelope: 2 m beyond the rotated tunnel cutout in x/y.
    // Computing the bounds from tunnelProfile makes the envelope work for any
    // crossSectionRotZ value without hand-swapping dimensions.
    const G4double concreteThickness = 1 * m;
    G4double tunnelMinX = tunnelProfile[0].x();
    G4double tunnelMaxX = tunnelProfile[0].x();
    G4double tunnelMinY = tunnelProfile[0].y();
    G4double tunnelMaxY = tunnelProfile[0].y();
    for (std::size_t i = 1; i < tunnelProfile.size(); ++i) {
        tunnelMinX = std::min(tunnelMinX, tunnelProfile[i].x());
        tunnelMaxX = std::max(tunnelMaxX, tunnelProfile[i].x());
        tunnelMinY = std::min(tunnelMinY, tunnelProfile[i].y());
        tunnelMaxY = std::max(tunnelMaxY, tunnelProfile[i].y());
    }

    const G4double rockMinX = tunnelMinX - concreteThickness;
    const G4double rockMaxX = tunnelMaxX + concreteThickness;
    const G4double rockMinY = tunnelMinY - concreteThickness;
    const G4double rockMaxY = tunnelMaxY + concreteThickness;

    const G4double rockHalfX = 0.5 * (rockMaxX - rockMinX);
    const G4double rockHalfY = 0.5 * (rockMaxY - rockMinY);
    const G4double rockHalfZ = tunnelHalfZ;
    const G4double rockCenterX = 0.5 * (rockMinX + rockMaxX);
    const G4double rockCenterYRot = 0.5 * (rockMinY + rockMaxY);

    G4Box* rockBoxSolid = new G4Box("gargoyle_rock_box_solid", rockHalfX, rockHalfY, rockHalfZ);
    G4SubtractionSolid* rockSolid = new G4SubtractionSolid(
            "gargoyle_rock_with_tunnel_solid",
            rockBoxSolid,
            tunnelCutoutSolid,
            0,
            G4ThreeVector(-rockCenterX, -rockCenterYRot, 0.0));

    G4LogicalVolume* rockLogic = new G4LogicalVolume(rockSolid, concreteMat,
                                                     "gargoyle_rock_logic", 0, 0, 0);
    new G4PVPlacement(0,
            G4ThreeVector(rockCenterX, rockCenterYRot, 0.0),
            rockLogic,
            "rockPhysic",
            logicWorld,
            false,
            0,
            true);

    G4VisAttributes* rockVis = new G4VisAttributes(G4Colour(0.55, 0.50, 0.45, 0.35));
    rockVis->SetVisibility(true);
    rockVis->SetForceSolid(true);
    rockLogic->SetVisAttributes(rockVis);

    typedef std::pair<G4TwoVector, G4TwoVector> Segment;

    std::vector<Segment> scintSegments;
    // Floor, right lower wall, left lower wall.
    scintSegments.push_back(Segment(tunnelProfile[0], tunnelProfile[1]));
    scintSegments.push_back(Segment(tunnelProfile[1], tunnelProfile[2]));
    scintSegments.push_back(Segment(tunnelProfile[leftMidIndex], tunnelProfile[0]));

    std::vector<Segment> upperSegments;
    // Right upper wall.
    upperSegments.push_back(Segment(tunnelProfile[2], tunnelProfile[3]));
    // Arch.
    for (std::size_t i = 3; i < leftSpringIndex; ++i) {
        upperSegments.push_back(Segment(tunnelProfile[i], tunnelProfile[i + 1]));
    }
    // Left upper wall.
    upperSegments.push_back(Segment(tunnelProfile[leftSpringIndex], tunnelProfile[leftMidIndex]));

    auto shifted = [](const G4TwoVector& p,
                      const G4TwoVector& n,
                      G4double d) -> G4TwoVector {
        return G4TwoVector(p.x() + n.x() * d, p.y() + n.y() * d);
    };

    auto nearlySamePoint = [](const G4TwoVector& a,
                              const G4TwoVector& b) -> G4bool {
        const G4double tol = 1.0e-9 * mm;
        const G4double dx = a.x() - b.x();
        const G4double dy = a.y() - b.y();
        return std::sqrt(dx * dx + dy * dy) < tol;
    };

    auto segmentDirection = [](const Segment& s) -> G4TwoVector {
        const G4double dx = s.second.x() - s.first.x();
        const G4double dy = s.second.y() - s.first.y();
        const G4double len = std::sqrt(dx * dx + dy * dy);
        if (len <= 0.0) return G4TwoVector(0.0, 0.0);
        return G4TwoVector(dx / len, dy / len);
    };

    auto segmentInwardNormal = [&](const Segment& s) -> G4TwoVector {
        const G4TwoVector dir = segmentDirection(s);

        // Boundary is CCW, so the inward normal is the left normal.
        return G4TwoVector(-dir.y(), dir.x());
    };

    auto lineIntersection = [](const G4TwoVector& p,
                               const G4TwoVector& r,
                               const G4TwoVector& q,
                               const G4TwoVector& s,
                               G4TwoVector& out) -> G4bool {
        const G4double denom = r.x() * s.y() - r.y() * s.x();

        if (std::fabs(denom) < 1.0e-12) {
            return false;
        }

        const G4TwoVector qp(q.x() - p.x(), q.y() - p.y());
        const G4double t = (qp.x() * s.y() - qp.y() * s.x()) / denom;

        out = G4TwoVector(p.x() + t * r.x(),
                          p.y() + t * r.y());
        return true;
    };

    auto miterPoint = [&](const Segment& prev,
                          const Segment& curr,
                          const G4TwoVector& joint,
                          G4double offset) -> G4TwoVector {
        const G4TwoVector prevDir  = segmentDirection(prev);
        const G4TwoVector currDir  = segmentDirection(curr);
        const G4TwoVector prevNorm = segmentInwardNormal(prev);
        const G4TwoVector currNorm = segmentInwardNormal(curr);

        const G4TwoVector p = shifted(joint, prevNorm, offset);
        const G4TwoVector q = shifted(joint, currNorm, offset);

        G4TwoVector inter;
        if (lineIntersection(p, prevDir, q, currDir, inter)) {
            return inter;
        }

        // Fallback for nearly collinear neighboring segments.
        G4TwoVector avg(prevNorm.x() + currNorm.x(),
                        prevNorm.y() + currNorm.y());
        const G4double avgLen = std::sqrt(avg.x() * avg.x() + avg.y() * avg.y());

        if (avgLen > 0.0) {
            avg = G4TwoVector(avg.x() / avgLen, avg.y() / avgLen);
            return shifted(joint, avg, offset);
        }

        return shifted(joint, currNorm, offset);
    };

    auto offsetStartPoint = [&](const std::vector<Segment>& segments,
                                std::size_t i,
                                G4double offset) -> G4TwoVector {
        const G4TwoVector& p0 = segments[i].first;
        const G4TwoVector currNorm = segmentInwardNormal(segments[i]);

        if (i > 0 && nearlySamePoint(segments[i - 1].second, p0)) {
            return miterPoint(segments[i - 1], segments[i], p0, offset);
        }

        return shifted(p0, currNorm, offset);
    };

    auto offsetEndPoint = [&](const std::vector<Segment>& segments,
                              std::size_t i,
                              G4double offset) -> G4TwoVector {
        const G4TwoVector& p1 = segments[i].second;
        const G4TwoVector currNorm = segmentInwardNormal(segments[i]);

        if (i + 1 < segments.size() && nearlySamePoint(segments[i + 1].first, p1)) {
            return miterPoint(segments[i], segments[i + 1], p1, offset);
        }

        return shifted(p1, currNorm, offset);
    };

    std::vector<G4LogicalVolume*> activeLayerLogics;

    auto makeAlternateColour = [](const G4Colour& c) -> G4Colour {
        // Blend odd-numbered mitered strip volumes slightly toward white while
        // preserving the original transparency.  This makes the shared miter
        // faces visible in OpenGL without changing the geometry.
        const G4double blend = 0.25;
        return G4Colour(c.GetRed()   * (1.0 - blend) + blend,
                        c.GetGreen() * (1.0 - blend) + blend,
                        c.GetBlue()  * (1.0 - blend) + blend,
                        c.GetAlpha());
    };

    auto makeAndPlaceLayerSegments = [&](const G4String& baseName,
                                         const G4String& physBaseName,
                                         const std::vector<Segment>& segments,
                                         G4double wallOffset,
                                         G4double thickness,
                                         G4Material* material,
                                         const G4Colour& colour,
                                         G4int copyBase,
                                         G4bool miterStrips,
                                         G4double stripEndOverlap) {
        for (std::size_t i = 0; i < segments.size(); ++i) {
            const G4TwoVector& p0 = segments[i].first;
            const G4TwoVector& p1 = segments[i].second;
            const G4double dx = p1.x() - p0.x();
            const G4double dy = p1.y() - p0.y();
            const G4double len = std::sqrt(dx * dx + dy * dy);
            if (len <= 0.0) continue;

            const G4TwoVector along(dx / len, dy / len);

            // Boundary is CCW, so the inward normal is the left normal.
            const G4TwoVector inward(-dy / len, dx / len);

            const G4double innerOffset = wallOffset;
            const G4double outerOffset = wallOffset + thickness;

            std::vector<G4TwoVector> stripProfile;

            if (miterStrips) {
                // Mitered tracker/scintillator strips.  Neighboring strips share
                // the same offset-line intersection at each joint, so the layer
                // is continuous without the fabricated-strip shingling/overlap
                // scheme.  This matches the proposed tracker concept in which
                // charge sharing provides the positional information.
                stripProfile.push_back(offsetStartPoint(segments, i, innerOffset));
                stripProfile.push_back(offsetEndPoint  (segments, i, innerOffset));
                stripProfile.push_back(offsetEndPoint  (segments, i, outerOffset));
                stripProfile.push_back(offsetStartPoint(segments, i, outerOffset));
            } else {
                // Rectangular strip pieces.  This path is still used for the
                // lower floor/wall scintillator pieces, where the input segments
                // are already shortened to avoid corner overlaps.
                const G4double startEndOverlap = (i > 0) ? stripEndOverlap : 0.0;
                const G4double finishEndOverlap =
                        (i + 1 < segments.size()) ? stripEndOverlap : 0.0;

                const G4TwoVector q0(p0.x() - along.x() * startEndOverlap,
                                     p0.y() - along.y() * startEndOverlap);
                const G4TwoVector q1(p1.x() + along.x() * finishEndOverlap,
                                     p1.y() + along.y() * finishEndOverlap);

                stripProfile.push_back(shifted(q0, inward, innerOffset));
                stripProfile.push_back(shifted(q1, inward, innerOffset));
                stripProfile.push_back(shifted(q1, inward, outerOffset));
                stripProfile.push_back(shifted(q0, inward, outerOffset));
            }

            std::ostringstream solidName;
            std::ostringstream logicName;
            std::ostringstream physName;
            solidName << baseName << "_solid_" << i;
            logicName << baseName << "_logic_" << i;
            physName  << physBaseName << "_" << i;

            G4VSolid* stripSolid = makeExtrudedSolid(solidName.str(), stripProfile, tunnelHalfZ);
            G4LogicalVolume* stripLogic = new G4LogicalVolume(stripSolid,
                                                              material,
                                                              logicName.str(),
                                                              0, 0, 0);

            const G4bool useAlternateVis = miterStrips && ((i % 2) == 1);
            G4VisAttributes* vis = new G4VisAttributes(useAlternateVis
                    ? makeAlternateColour(colour)
                    : colour);
            vis->SetVisibility(true);
            vis->SetForceSolid(true);
            if (miterStrips) {
                vis->SetForceAuxEdgeVisible(true);
            }
            stripLogic->SetVisAttributes(vis);

            // Do not enable overlap checking for each thin strip here.  In
            // Geant4 10.5, many thin tessellated/boolean-adjacent volumes can
            // make startup look like it has hung.  The strips are constructed
            // from the same profile and are intentionally just inside the tunnel.
            new G4PVPlacement(0,
                    G4ThreeVector(),
                    stripLogic,
                    physName.str(),
                    logicWorld,
                    false,
                    copyBase + static_cast<G4int>(i),
                    false);

            activeLayerLogics.push_back(stripLogic);
        }
    };


    const G4double wallGap          = 0.1 * cm;
    const G4double scintThickness   = 2.0 * cm;
    const G4double trackerThickness = 2.0 * cm;

    // There are two total tracker layers.  The tracker layers are now mitered
    // scintillator, not staggered/offset silicon.  Preserve the current 24 cm
    // air gap between the two tracker layers.
    const G4double trackerLayerGap  = 24.0 * cm;

    // The scintillator is made from three rectangular fabricated pieces:
    //   copy 0 = floor
    //   copy 1 = right lower wall
    //   copy 2 = left lower wall
    //
    // If all three pieces start exactly at the same tunnel corner, their
    // finite-thickness rectangles overlap in an L/cross shape.  Keep the pieces
    // rectangular, but shorten the input line segments before making the strips.
    // The floor and wall pieces are both cut back at the lower corners by the
    // full scintillator envelope plus a small requested air gap.  This gives a
    // real gap between active volumes rather than merely making one piece flush
    // with the side face of the other.
    const G4double scintJointGap = 0.1 * mm;
    const G4double scintFloorEndCutback = wallGap + scintJointGap;
    const G4double scintWallFloorCutback = wallGap + scintThickness + 2*scintJointGap;
    const G4double scintWallTrackerGap = scintJointGap;

    std::vector<Segment> scintSegmentsNoOverlap = scintSegments;

    auto shortenSegmentEnds = [](const Segment& seg,
                                 G4double startCut,
                                 G4double endCut) -> Segment {
        const G4TwoVector& p0 = seg.first;
        const G4TwoVector& p1 = seg.second;

        const G4double dx = p1.x() - p0.x();
        const G4double dy = p1.y() - p0.y();
        const G4double len = std::sqrt(dx * dx + dy * dy);

        if (len <= 0.0) return seg;

        const G4TwoVector along(dx / len, dy / len);

        // Avoid inverting a segment if someone later exaggerates the detector
        // thicknesses beyond what the corresponding tunnel section can support.
        const G4double maxTotalCut = 0.90 * len;
        const G4double requestedTotalCut = startCut + endCut;
        G4double safeStartCut = startCut;
        G4double safeEndCut   = endCut;

        if (requestedTotalCut > maxTotalCut && requestedTotalCut > 0.0) {
            const G4double scale = maxTotalCut / requestedTotalCut;
            safeStartCut *= scale;
            safeEndCut   *= scale;
        }

        const G4TwoVector newP0(p0.x() + along.x() * safeStartCut,
                                p0.y() + along.y() * safeStartCut);
        const G4TwoVector newP1(p1.x() - along.x() * safeEndCut,
                                p1.y() - along.y() * safeEndCut);

        return Segment(newP0, newP1);
    };

    // Floor: cut back both ends so the finite-thickness floor slab does not
    // touch or overlap the finite-thickness wall slabs at the lower corners.
    scintSegmentsNoOverlap[0] =
        shortenSegmentEnds(scintSegmentsNoOverlap[0],
                            scintFloorEndCutback,
                            scintFloorEndCutback);

    // Right lower wall: segment direction is floor -> mid-wall.  Cut the bottom
    // by the full floor-scintillator envelope plus the requested gap, and cut
    // the top by the scint/tracker gap.
    scintSegmentsNoOverlap[1] =
        shortenSegmentEnds(scintSegmentsNoOverlap[1],
                            scintWallFloorCutback,
                            scintWallTrackerGap);

    // Left lower wall: segment direction is mid-wall -> floor.  So the start cut
    // is the tracker gap, and the end cut is the full floor-scintillator envelope
    // plus the requested gap.
    scintSegmentsNoOverlap[2] =
        shortenSegmentEnds(scintSegmentsNoOverlap[2],
                            scintWallTrackerGap,
                            scintWallFloorCutback);

    makeAndPlaceLayerSegments("gargoyle_scint",
                              "gargoyle_scint_phys",
                              scintSegmentsNoOverlap,
                              wallGap,
                              scintThickness,
                              matPlScin,
                              G4Colour::Cyan(),
                              0,
                              false,
                              0.0);

    makeAndPlaceLayerSegments("gargoyle_si_layer1",
                              "gargoyle_si_layer1_phys",
                              upperSegments,
                              wallGap,
                              trackerThickness,
                              matPlScin,
                              G4Colour(0.1, 0.8, 0.1, 0.75),
                              1000,
                              true,
                              0.0);

    makeAndPlaceLayerSegments("gargoyle_si_layer2",
                              "gargoyle_si_layer2_phys",
                              upperSegments,
                              wallGap + trackerThickness + trackerLayerGap,
                              trackerThickness,
                              matPlScin,
                              G4Colour(0.9, 0.9, 0.1, 0.75),
                              2000,
                              true,
                              0.0);

    // Reuse the existing Scint_SD/grScintSD infrastructure for all active layers.
    G4SDManager* SDman = G4SDManager::GetSDMpointer();
    G4String SDnameOfScintDetector = "Scint_SD";
    grScintSD* myScintSD = dynamic_cast<grScintSD*>(
            SDman->FindSensitiveDetector(SDnameOfScintDetector, false));
    if (!myScintSD) {
        myScintSD = new grScintSD(SDnameOfScintDetector);
        SDman->AddNewDetector(myScintSD);
    }

    for (std::size_t i = 0; i < activeLayerLogics.size(); ++i) {
        activeLayerLogics[i]->SetSensitiveDetector(myScintSD);
    }

    // Treat this as three active layers with one large channel per layer.
    this->SetNLayer(3);
    this->SetNBarPerLayer(1);

    if (verbose >= 0) {
        G4cout << "GARGOYLE tunnel summary:" << G4endl;
        G4cout << "  tunnel length: " << G4BestUnit(tunnelLength, "Length") << G4endl;
        G4cout << "  tunnel floor width: " << G4BestUnit(tunnelAlpha, "Length") << G4endl;
        G4cout << "  tunnel height: " << G4BestUnit(tunnelBeta, "Length") << G4endl;
        G4cout << "  wall midpoint transition y: " << G4BestUnit(midWallY, "Length") << G4endl;
        G4cout << "  scintillator thickness: " << G4BestUnit(scintThickness, "Length") << G4endl;
        G4cout << "  tracker scintillator thickness: " << G4BestUnit(trackerThickness, "Length") << G4endl;
        G4cout << "  tracker inter-layer gap: " << G4BestUnit(trackerLayerGap, "Length") << G4endl;
        G4cout << "  tracker geometry: mitered scintillator strips" << G4endl;
        G4cout << "  tracker visualization: alternating odd/even strip colours" << G4endl;
        G4cout << "  active physical layer segments: " << activeLayerLogics.size() << G4endl;
    }

    return physicWorld;
}


void grDetectorConstruction::SetMagField(G4double fieldValueX, G4double fieldValueY, G4double fieldValueZ) {

  fMonFieldSetup->SetMagField(fieldValueX, fieldValueY, fieldValueZ);

}

void SortProperty(G4double* energies, G4double* values, int n) {
    std::vector<std::pair<G4double,G4double>> pairs;
    for(int i=0; i<n; i++) pairs.emplace_back(energies[i], values[i]);
    std::sort(pairs.begin(), pairs.end(),
              [](auto &a, auto &b){ return a.first < b.first; });
    for(int i=0; i<n; i++) {
        energies[i] = pairs[i].first;
        values[i]   = pairs[i].second;
    }
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
  SortProperty(photonEnergy, EJ200_SCINT, ScintEntry);

  for (int i = 0; i < nEntries; i++) {
    EJ200_RIND[i] = 1.58;//58; // refractive index at 425 nm
    //EJ200_ABSL[i] *= myPSAttenuationLength;
    EJ200_ABSL[i] = 3.8*m;//2.5 * m; // bulk attenuation at 425 nm
  }


  mptPlScin->AddProperty("SCINTILLATIONCOMPONENT1", photonEnergy, EJ200_SCINT, ScintEntry);
        mptPlScin->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 2.1 * ns); //decay time, according to EJ200

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
        SortProperty(photonEnergyPMT, photocath_EFF, nEntriesPMT);


  mptPMT->AddProperty("REFLECTIVITY", photonEnergyPMT,PhCath_REFL, nEntriesPMT);//->SetSpline(true);
  mptPMT->AddProperty("EFFICIENCY",photonEnergyPMT,photocath_EFF,nEntriesPMT);//->SetSpline(true);

  G4cout << "successfully at end of PMT optical table" << G4endl;

  return mptPMT;

}

void grDetectorConstruction::GetPMTEff_R878(G4PhysicsVector& effVec)
{
    std::ifstream ReadPMTQEff(grOpticalFilePath + "PMT_R878_QE.txt");

    if (!ReadPMTQEff.is_open()) {
        G4cerr << "ERROR: Could not open PMT_R878_QE.txt at "
               << grOpticalFilePath << G4endl;
        return;
    }

    effVec.Retrieve(ReadPMTQEff, true);

    if (effVec.GetVectorLength() != 0)
        G4cout << "Quantum Efficiency successfully retrieved for PMT_R878_QE" << G4endl;
    else
        G4cout << "ERROR: Vector length is zero!" << G4endl;

    effVec.ScaleVector(1, 1);
}


void grDetectorConstruction::GetPMTEff_R7725(G4PhysicsVector& effVec)
{
    std::ifstream ReadPMTQEff(grOpticalFilePath + "PMT_R7725_QE.txt");

    if (!ReadPMTQEff.is_open()) {
        G4cerr << "ERROR: Could not open PMT_R7725_QE.txt at "
               << grOpticalFilePath << G4endl;
        return;
    }

    effVec.Retrieve(ReadPMTQEff, true);

    if (effVec.GetVectorLength() != 0)
        G4cout << "Quantum Efficiency successfully retrieved for PMT R7725" << G4endl;
    else
        G4cout << "ERROR: Vector length is zero!" << G4endl;

    effVec.ScaleVector(1, 1);
}


void grDetectorConstruction::GetPMTEff_ET9814B(G4PhysicsVector& effVec)
{
    std::ifstream ReadPMTQEff(grOpticalFilePath + "PMT_ET9814B_QE.txt");

    if (!ReadPMTQEff.is_open()) {
        G4cerr << "ERROR: Could not open PMT_ET9814B_QE.txt at "
               << grOpticalFilePath << G4endl;
        return;
    }

    effVec.Retrieve(ReadPMTQEff, true);

    if (effVec.GetVectorLength() != 0)
        G4cout << "Quantum Efficiency successfully retrieved for PMT ET9814B" << G4endl;
    else
        G4cout << "ERROR: Vector length is zero!" << G4endl;

    effVec.ScaleVector(1, 1);
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
