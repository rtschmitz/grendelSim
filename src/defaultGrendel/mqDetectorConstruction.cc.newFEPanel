// created 22-04-2019
// author: Ryan Schmitz (UCSB)

//==============================================================================
#include "mqDetectorConstruction.hh"
#include "mqScintSD.hh"
#include "mqPMTSD.hh"
#include "mqBarParameterisation.hh"
#include "mqPMTParameterisation.hh"
#include "mqUserEventInformation.hh"

#include "MilliQMonopoleFieldSetup.hh"

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

#include "G4MultiUnion.hh"
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

//const G4String mqOpticalFilePath = "/home/users/ryan/milliQanDemoSim/OpticalData/";
//const G4String mqOpticalFilePath = (std::string)std::getenv("PROJECTDIR")+"/OpticalData/";
const G4String mqOpticalFilePath = "../OpticalData/";
//==============================================================================
mqDetectorConstruction::mqDetectorConstruction() :
    verbose(1)//,solidWorld(0), logicWorld(0), physicWorld(0)
{
	SetDefaults();
}

//==============================================================================
mqDetectorConstruction::~mqDetectorConstruction() {

}
//==============================================================================

//==============================================================================

G4VPhysicalVolume* mqDetectorConstruction::Construct() {

	return SetupGeometry();
}

//==============================================================================

G4VPhysicalVolume* mqDetectorConstruction::SetupGeometry() {

	if (verbose >= 0) {
		G4cout << "MilliQan> Construct geometry." << G4endl;
	}
	G4double fWorld_x = 30.0 * m;
	G4double fWorld_y = 30.0 * m;
	G4double fWorld_z = 30.0 * m;

	G4Material* worldMaterial = G4NistManager::Instance()->FindOrBuildMaterial(
	//			"Galactic");
				"G4_AIR");

	const G4int nEntriesAir = 2;
	G4double photonEnergyAir[nEntriesAir]={1.5 * eV,6.3 * eV};  //{ 6.3 * eV,1.5 * eV};
	G4double Air_RIND[nEntriesAir];

	for (int i = 0; i < nEntriesAir; i++) {
		Air_RIND[i]= 1.0;
	}// max value at 440 nm

	G4MaterialPropertiesTable* mptAir = new G4MaterialPropertiesTable();
	mptAir->AddProperty("RINDEX",photonEnergyAir, Air_RIND, nEntriesAir);//->SetSpline(true);
	worldMaterial->SetMaterialPropertiesTable(mptAir);

	G4Box* solidWorld = new G4Box("world", fWorld_x / 2, fWorld_y / 2, fWorld_z / 2);
	G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, worldMaterial, "World", 0, 0,
			0);

	G4VisAttributes* visAttWorld = new G4VisAttributes();
	visAttWorld->SetVisibility(false);
	logicWorld->SetVisAttributes(visAttWorld);

	//Must place the World Physical volume unrotated at (0,0,0).
	G4PVPlacement* physicWorld = new G4PVPlacement(0, // no rotation
			G4ThreeVector(), // at (0,0,0)
			logicWorld, // its logical volume
		"World", // its name
			0, // its mother  volume
			false, // no boolean operations
			0); // copy number

//// initialize magnetic field ////
/* //disabling for now since it made no difference and it breaks some builds
      	fMonFieldSetup = MilliQMonopoleFieldSetup::GetMonopoleFieldSetup();
	G4double fieldValueX = -1.61*gauss;		//-14.00*gauss;
	G4double fieldValueY = -1.73*gauss;		//12.63*gauss;
	G4double fieldValueZ = 21.57*gauss;		//10.74*gauss;
	//total magnitude: 21.7 gauss
	this->SetMagField(fieldValueX, fieldValueY, fieldValueZ);
	//0 is standard field, 1 is field which works for monopoles
	fMonFieldSetup->SetStepperAndChordFinder(1);
*/
///////////////////////////////    End world //////////////////////////////////////////////////
///////////////////////////////   Start Scintillator/Wrapping  ///////////////////////////////////////////
	//==============================================================================
	// size of Scintillator and wrapping
	//==============================================================================
 

	G4double wrapRefl = 0.97;//0.97;
	G4double scintX = 50/2*mm;
	G4double scintY = 50/2*mm;
	G4double scintXY = 50/2*mm;
	G4double scintZ  = 600/2*mm;//(each of these dimensions represents the half-width; e.g. this is 600mm long)
	G4double subBarSpacing = 1*mm;

	G4double airGapThickness = 0.05*cm; //1*mm
	G4double wrapThickness = 0.05*cm; //3*mm
	G4double airgapX = scintX+airGapThickness;	
	G4double airgapY = scintY+airGapThickness;	
	G4double airgapXY = scintXY+airGapThickness;	
	G4double airgapZ = scintZ+airGapThickness;
	G4double wrapX=airgapX+wrapThickness;
	G4double wrapY=airgapY+wrapThickness;
	G4double wrapXY=airgapXY+wrapThickness;
	G4double wrapZ=airgapZ+wrapThickness;

	G4double barSpacingInter = 0.5*cm;

        G4double frontLayerMid = -182.0*cm;
        G4double midLayerMid = -60.0*cm;
        G4double backLayerMid = 65.0*cm;
	G4double fourthLayerMid = 190.0*cm; //called so because the original sim had 3 layers

        //G4double frontLayerMid = -132*cm;
        //G4double midLayerMid = -10*cm;
        //G4double backLayerMid = 115*cm;
	//G4double fourthLayerMid = 240*cm; //called so because the original sim had 3 layers
	
	//measured from the centers; the gap between the bars
	G4double barSpacingXY = scintXY*2*2+1*cm+1*mm;
	G4double worldRotation = -43.1*degree;//-43.1*deg;// (demonstrator rotation)
	//G4double worldRotation = -43.1*deg;// (demonstrator rotation)

	//this is here because the entire geometry is shifted 6cm upwards
        G4double centerOffsetX = 18*cm;
	G4double centerOffsetZ = 9/2*barSpacingXY*std::cos(worldRotation)/std::tan(worldRotation);

	//G4double layerSpacing = 1000*mm; 
	//in visualization this gets swapped, so this is actually 3x2 rather than 2x3
        G4int nBarXCount = 2;//2 //number of bars in grid, so this is NxN
        G4int nBarYCount = 2;//3 //number of bars in grid, so this is NxN
	G4int nBarsX = nBarXCount;
	G4int nBarsY = nBarYCount;

	this->SetNBarPerLayer(nBarXCount*nBarYCount);
	this->SetNLayer(nLayers);
 	
	 G4double outerRadius_pmt   = 51* mm /2.; //26 //52
	 G4double outerRadius_cath  = 46* mm /2.; //26 //52
	 G4double height_pmt        = 150* mm; //147.*mm; //150 measured from model
	 G4double height_cath       = 0.5 * mm; //2mm
	 G4double pmtYoffset        = 0*mm;
	 G4double muMetalThickness = 0.5*mm;

	 //Using new Front End panel, combination of two panels!
	 G4double ScintSlabZFront1 = 2.94/2*cm; //5/2*cm;//scintXY*2+0.5*mm;
	 G4double ScintSlabZFront2 = 2.94/2*cm; //5/2*cm;//scintXY*2+0.5*mm;
	 G4double ScintSlabYFront1 = 20.32/2*cm; //25/2*cm;///barSpacingXY*(nBarsY+1)/2-4*cm;
	 G4double ScintSlabYFront2 = 20.32/2*cm; //25/2*cm;///barSpacingXY*(nBarsY+1)/2-4*cm;
	 G4double ScintSlabXFront1 = 30.5/2*cm; //25/2*cm;//barSpacingXY*(nBarsY+1)+5*cm;//0;//2.54*1/2*cm; //inch thickness
	 G4double ScintSlabXFront2 = 30.5/2*cm; //25/2*cm;//barSpacingXY*(nBarsY+1)+5*cm;//0;//2.54*1/2*cm; //inch thickness
	 G4double ScintSlabZEnd = 5/2*cm;//scintXY*2+0.5*mm;
	 G4double ScintSlabYEnd = 25/2*cm;///barSpacingXY*(nBarsY+1)/2-4*cm;
	 G4double ScintSlabXEnd = 25/2*cm;//barSpacingXY*(nBarsY+1)+5*cm;//0;//2.54*1/2*cm; //inch thickness
	 G4double frontPanelOverlapShift = 6.3*cm; //The relative shift of the two slabs to form a longer front end panel (verify!) 
	 //G4double ScintSlabY = barSpacingXY*(nBarsY+1)/2-4*cm;
	 //G4double ScintSlabX = barSpacingXY*(nBarsY+1)+5*cm;//0;//2.54*1/2*cm; //inch thickness
	 //G4double ScintSlabX = ((nBarXCount-1)*6+5)/2*cm+10*cm;
	 //G4double ScintSlabY = ((nBarYCount-1)*6+5)/2*cm+10*cm;
	 G4double ScintSlabWrapXFront1 = ScintSlabXFront1+airGapThickness+wrapThickness;
	 G4double ScintSlabWrapXFront2 = ScintSlabXFront2+airGapThickness+wrapThickness;
	 G4double ScintSlabWrapYFront1 = ScintSlabYFront1+airGapThickness+wrapThickness;
	 G4double ScintSlabWrapYFront2 = ScintSlabYFront2+airGapThickness+wrapThickness;
	 G4double ScintSlabWrapZFront1 = ScintSlabZFront1+airGapThickness+wrapThickness;
	 G4double ScintSlabWrapZFront2 = ScintSlabZFront2+airGapThickness+wrapThickness;
	 G4double ScintSlabWrapXEnd = ScintSlabXEnd+airGapThickness+wrapThickness;
	 G4double ScintSlabWrapYEnd = ScintSlabYEnd+airGapThickness+wrapThickness;
	 G4double ScintSlabWrapZEnd = ScintSlabZEnd+airGapThickness+wrapThickness;
	 G4double ScintSlabAirGapXFront1 = ScintSlabXFront1+airGapThickness;
	 G4double ScintSlabAirGapXFront2 = ScintSlabXFront2+airGapThickness;
	 G4double ScintSlabAirGapYFront1 = ScintSlabYFront1+airGapThickness;
	 G4double ScintSlabAirGapYFront2 = ScintSlabYFront2+airGapThickness;
	 G4double ScintSlabAirGapZFront1 = ScintSlabZFront1+airGapThickness;
	 G4double ScintSlabAirGapZFront2 = ScintSlabZFront2+airGapThickness;
	 G4double ScintSlabAirGapXEnd = ScintSlabXEnd+airGapThickness;
	 G4double ScintSlabAirGapYEnd = ScintSlabYEnd+airGapThickness;
	 G4double ScintSlabAirGapZEnd = ScintSlabZEnd+airGapThickness;

	 G4double ScintSlabOffsetX = centerOffsetX;//7.5*cm;
	 G4double ScintSlabOffsetZ0 = -scintZ-ScintSlabZEnd-0.73*cm; //scintZ-2*cm //-174*cm
	 G4double ScintSlabOffsetZ1 = -scintZ-ScintSlabZEnd-1.73*cm; //scintZ-3*cm //-53*cm
	 G4double ScintSlabOffsetZ2 = -scintZ-ScintSlabZEnd-1.23*cm; //scintZ-2.5*cm //72.5*cm;
	 G4double ScintSlabOffsetZ3 = -scintZ-ScintSlabZEnd-1.23*cm; //scintZ-2.5*cm //72.5*cm;
	 G4double ScintSlabOffsetZEnd = scintZ+height_pmt+ScintSlabZEnd+5.23*cm; //scintZ-2.5*cm //181.5
	 
	 //G4double ScintPanelX = 0.274*2.54/2*cm;
	 G4double ScintPanelX = 5/2*cm;//6/2*cm;//1.0/2*cm;
	 //G4double ScintPanelY = 7*2.54/2*cm;
	 //G4double ScintPanelZ = 40*2.54/2*cm;
	 G4double ScintPanelY = 25/2*cm;//(nBarsX)*scintXY*2+0.5*mm;
	 G4double ScintPanelZ = 170/2*cm;
	 G4double ScintPanelWrapX = ScintPanelX+airGapThickness+wrapThickness;
	 G4double ScintPanelWrapY = ScintPanelY+airGapThickness+wrapThickness;
	 G4double ScintPanelWrapZ = ScintPanelZ+airGapThickness+wrapThickness;
	 G4double ScintPanelAirGapX = ScintPanelX+airGapThickness;
	 G4double ScintPanelAirGapY = ScintPanelY+airGapThickness;
	 G4double ScintPanelAirGapZ = ScintPanelZ+airGapThickness;
	 
	G4double ScintPanelOffsetXTop = 4*wrapY+2*barSpacingInter+5.0*cm/2+2.5*cm+ScintPanelWrapX; //5.0cm is spacing between supermodules in X, 2.5cm comes from Sam's model
	 G4double ScintPanelOffsetXLeft = centerOffsetX-18*cm;//6.65*cm;
	 G4double ScintPanelOffsetXRight = centerOffsetX-18*cm;//6.65*cm;
	 G4double ScintPanelOffsetYRight = 4*wrapY+2*barSpacingInter+2.3*cm/2+7*cm+ScintPanelWrapX; //2.3cm is spacing between supermodules in Y, 7cm comes from Sam's model	
	 					//4*5.1*cm/2 + 2*0.5cm+2.3*cm/2+7*cm+5.1/2*cm, 21.9cm. add another 5.1/2*cm to get to the edge of the panel, so 24.45cm to edge from middle. 48.9cm between far side edges of panels
	 G4double ScintPanelOffsetYLeft = -ScintPanelOffsetYRight;

	G4double HodoX = 8.6*cm/2; //width
	G4double HodoY = 45*cm/2; //length //horizontal bars by default
	G4double HodoZ = 4.2*cm/2; //thickness

	G4double HodoWrapX = HodoX+wrapThickness;
	G4double HodoWrapY = HodoY+wrapThickness;
	G4double HodoWrapZ = HodoZ+wrapThickness;

	G4double HodoOffsetVertYRight = 4.8*cm;
	G4double HodoOffsetVertYLeft = -HodoOffsetVertYRight;
	G4double HodoOffsetVertX = -2.5*cm;

	G4double HodoOffsetHorXUpper = 10.4*cm;
	G4double HodoOffsetHorXLower = 0.8*cm;
	G4double HodoOffsetHorY = 0*cm;

	G4double HodoOffsetVertZFront1 = ScintSlabOffsetZ0+frontLayerMid-8.6*cm;
	G4double HodoOffsetVertZFront2 = ScintSlabOffsetZ0+frontLayerMid-8.6*cm;
	G4double HodoOffsetHorZFront1 = ScintSlabOffsetZ0+frontLayerMid-3.9*cm;
	G4double HodoOffsetHorZFront2 = ScintSlabOffsetZ0+frontLayerMid-3.9*cm;
	G4double HodoOffsetHorZBack = ScintSlabOffsetZEnd+backLayerMid+5.6*cm;
	G4double HodoOffsetVertZBack = ScintSlabOffsetZEnd+backLayerMid+11.1*cm;

	//controls rotation of detector w.r.t. ground 
//	G4double worldRotation = -13*deg;
//	G4double worldRotation = 0*deg;
	
	 G4double AlSupportX     = 10*cm/2;
         G4double AlSupportY     = ScintSlabYEnd;
         G4double AlSupportZ     = (backLayerMid-frontLayerMid+height_pmt+scintZ*2+13*cm)/2; //360*cm
	 G4double AlSupportXOffset = ScintSlabOffsetX-ScintSlabXEnd-AlSupportX-10*cm;

	 G4double wallThickness = 1*m; //using 2m before
	 G4double wallCylRadius = 1.45*m;//1.45*m; //there were some conflicting measurements from the bar and slab I think related to the fact the cavern is turning there. The easier-to-make slab measurement was 2.89m,
	 						//the bar measurement was 2.72m but not sure where on the wall this hit etc.; I think the 2.9m is a better modeling of the cavern since I know the slab detector is not on a bend
							//like the bar detector is here. It's a bit hard to say for sure, I'm going to go with 2.9m since it seems to visually represent pictures the best. Overall minor difference of like 10cm
							//in radius won't do much either way, splitting hairs here a bit
	 G4double wallCylRadiusOut = wallCylRadius+wallThickness;
	 G4double wallZ = 15*m/2;

	 //G4double floorCutoutDepthX = (10*cm+wallCylRadius)/2;
	 G4double floorCutoutDepthX = (14*cm+(7*2.54*cm-5*cm)+wallCylRadius)/2;
	 G4double floorCutoutDepthY = (-20*cm+wallCylRadius);
	 G4double floorZ = wallZ;
	 G4double overallDetX = ScintPanelOffsetXTop+ScintSlabXEnd+60*cm;//30*cm; //adding larger buffer to get volume above and below
	 G4double overallDetY = ScintPanelOffsetYRight+ScintPanelY+50*cm; //adding larger buffer since the volumes are angled, PMT is extending, etc.
	 //G4double overallDetZ = fourthLayerMid+ScintSlabOffsetZ+ScintSlabWrapZ+22*cm+100*cm; //adding 5*mm buffer

	 //todo: modify this bool to be able to adjust from single bar mode to demonstrator mode
	//(together with modifying number of layers and bars, etc.)
	 bool SupportStructure = false;
	 G4double AlThickness = 1*cm;

	 G4double SteelThickness = 1*cm;
	 G4double SteelSideLength = 1*m/2;

	 G4double SteelSupportOffsetX = -0.7*m;
	 G4double SteelSupportOffsetZ = 0.8*m;

	G4double detLength = (wrapZ*2+height_pmt+15*cm)*4;
	G4double interactionDist=5000.0*m;
	G4double barAngleDelta = 0; //this parameter isn't used here anymore, defining supermod gaps in barParam directly

	G4NistManager* nistMan = G4NistManager::Instance();

///////////////////////////////////////////////////////////////////////////////
	//==========================================================================
	//Scintillator Materials
	//==========================================================================

	//==========================================================================
	// Elements
	//
	//*******************************************************************************************************************************
	G4Element* elH = nistMan->FindOrBuildElement("H");
	G4Element* elC = nistMan->FindOrBuildElement("C");
	G4Element* elO = nistMan->FindOrBuildElement("O");
	G4Element* elK = nistMan->FindOrBuildElement("K");
	G4Element* elNa = nistMan->FindOrBuildElement("Na");
	G4Element* elSi = nistMan->FindOrBuildElement("Si");
	G4Element* elAl = nistMan->FindOrBuildElement("Al");
        G4Element* elCs = nistMan->FindOrBuildElement("Cs"); // Cesium;
        G4Element* elSb = nistMan->FindOrBuildElement("Sb"); // Antimony;
        G4Element* elB = nistMan->FindOrBuildElement("B"); // Boron;
		
	//mu metal materials	
	G4Element* elNi = nistMan->FindOrBuildElement("Ni"); //Nickel;
	G4Element* elFe = nistMan->FindOrBuildElement("Fe"); //Iron;
	 
          //---------------------------------------------------
          //Borosilicate glass (Schott BK7) (8" PMT window);
          //---------------------------------------------------
          G4Material* matBorGlass = new G4Material("BorGlass", 2.51 * g / cm3, 6);
          matBorGlass->AddElement(elB, 0.040064);
          matBorGlass->AddElement(elO, 0.539562);
          matBorGlass->AddElement(elNa, 0.028191);
          matBorGlass->AddElement(elAl, 0.011644);
          matBorGlass->AddElement(elSi, 0.377220);
          matBorGlass->AddElement(elK, 0.003321);

          //BiAlkali photocathode (NIM A567, p.222) K2CsSb;
          //For the references for the optical properties see ../ref/ComplexRefractionSpectrum_KCSSb.pdf
          //---------------------------------------------------
          // exact composition of the bialkali is unknown, density is the 'realistic' guess!;
          G4Material* matBiAlkali = new G4Material("matBiAlkali", 1.3 * g / cm3, 3);
          matBiAlkali->AddElement(elK, 2);
          matBiAlkali->AddElement(elCs, 1);
          matBiAlkali->AddElement(elSb, 1);

	
	//there are other parts, but to first order this is 80% Ni, 15% Fe, 5% other stuff, so I'm just going to lump that into Fe
	G4Material* muMetal = new G4Material("muMetal",8.7*g/cm3, 2);
	muMetal->AddElement(elNi,0.8); //80% mass fraction
	muMetal->AddElement(elFe,0.2); //20% mass fraction

	G4Element* elPlastic = nistMan->FindOrBuildElement("G4_POLYSTYRENE");
	//==========================================================================
	// BC-400 plastic scintillator; according to datasheet, elH = 1.103 *elC, elH + elC = 1
	//For the references for the optical properties see ../ref/EmissionSpectrum_BC-400.pdf

	G4Material* matPlScin = new G4Material("plScintillator", 1.032 * g / cm3, 2);
	//matPlScin->AddElement(elC, .4755);
	//matPlScin->AddElement(elH, .5245);
	matPlScin->AddElement(elC, 10);
	matPlScin->AddElement(elH, 11);
	matPlScin->SetMaterialPropertiesTable(SetOpticalPropertiesOfPS());
	matPlScin->GetIonisation()->SetBirksConstant(0.126*mm/MeV); // according to L. Reichhart et al., Phys. Rev, use (0.149*mm/MeV) for neutrons, but otherwise use 0.126
	
	G4VisAttributes* visAttribplScin = new G4VisAttributes(G4Colour::Cyan());
	visAttribplScin->SetVisibility(true);

	
	//==========================================================================
	//G4Material* wrapMat = worldMaterial;
	G4Material* wrapMat = nistMan->FindOrBuildMaterial("G4_POLYETHYLENE");
	//G4Material* airgapMat = worldMaterial; //The air gap mat should be air, change this if you change worldmat to be vacuum/galactic		
	G4Material* AlMat = nistMan->FindOrBuildMaterial("G4_Al");
	G4Material* PbMat = nistMan->FindOrBuildMaterial("G4_Pb");

	G4Material* steelMat = nistMan->FindOrBuildMaterial("G4_STAINLESS-STEEL");
	//going to use concrete as a first order approximation to rock, since it's close and we just want something dense
	G4Material* concreteMat = nistMan->FindOrBuildMaterial("G4_CONCRETE");
	
	G4Material* silicaMat = new G4Material("SiO2", 2.65 * g / cm3, 2);
	silicaMat->AddElement(elSi, 1);
	silicaMat->AddElement(elO, 2);


//Container volume so we can rotate the entire detector at once

	G4RotationMatrix* detRot = new G4RotationMatrix();
	G4RotationMatrix* detRotsub = new G4RotationMatrix();
	detRotsub->rotateX(-90*deg);
	//detRot->rotateX(90*deg);
	detRot->rotateY(worldRotation); //worldRotation-45*degree
	//first 4 vertices are +z, next four are -z
	std::vector<G4TwoVector> vertices;
	vertices.push_back(G4TwoVector(nBarsX*barSpacingXY/2+28*cm,detLength/2+centerOffsetZ*2.4+scintZ+height_pmt/2-4*cm+30*cm));
	vertices.push_back(G4TwoVector(nBarsX*barSpacingXY/2+28*cm,-detLength/2-scintZ-height_pmt/2+4*cm-60*cm));
	vertices.push_back(G4TwoVector(-nBarsX*barSpacingXY/2-28*cm,-detLength/2-centerOffsetZ*2.4-scintZ-height_pmt/2+4*cm-60*cm));
	vertices.push_back(G4TwoVector(-nBarsX*barSpacingXY/2-28*cm,detLength/2+scintZ+height_pmt/2-4*cm+30*cm));
	vertices.push_back(G4TwoVector(nBarsX*barSpacingXY/2+28*cm,detLength/2+centerOffsetZ*2.4+scintZ+height_pmt/2-4*cm+30*cm));
	vertices.push_back(G4TwoVector(nBarsX*barSpacingXY/2+28*cm,-detLength/2-scintZ-height_pmt/2+4*cm-50*cm));
	vertices.push_back(G4TwoVector(-nBarsX*barSpacingXY/2-28*cm,-detLength/2-centerOffsetZ*2.4-scintZ-height_pmt/2+4*cm-60*cm));
	vertices.push_back(G4TwoVector(-nBarsX*barSpacingXY/2-28*cm,detLength/2+scintZ+height_pmt/2-4*cm+30*cm));

/*	
	vertices.push_back(G4TwoVector(nBarsX*barSpacingXY/2+28*cm,detLength/2+centerOffsetZ*2.4+scintZ+height_pmt/2-4*cm+60*cm));
	vertices.push_back(G4TwoVector(nBarsX*barSpacingXY/2+28*cm,-detLength/2-scintZ-height_pmt/2+4*cm-30*cm));
	vertices.push_back(G4TwoVector(-nBarsX*barSpacingXY/2-28*cm,-detLength/2-centerOffsetZ*2.4-scintZ-height_pmt/2+4*cm-30*cm));
	vertices.push_back(G4TwoVector(-nBarsX*barSpacingXY/2-28*cm,detLength/2+scintZ+height_pmt/2-4*cm+60*cm));
	vertices.push_back(G4TwoVector(nBarsX*barSpacingXY/2+28*cm,detLength/2+centerOffsetZ*2.4+scintZ+height_pmt/2-4*cm+60*cm));
	vertices.push_back(G4TwoVector(nBarsX*barSpacingXY/2+28*cm,-detLength/2-scintZ-height_pmt/2+4*cm-30*cm));
	vertices.push_back(G4TwoVector(-nBarsX*barSpacingXY/2-28*cm,-detLength/2-centerOffsetZ*2.4-scintZ-height_pmt/2+4*cm-30*cm));
	vertices.push_back(G4TwoVector(-nBarsX*barSpacingXY/2-28*cm,detLength/2+scintZ+height_pmt/2-4*cm+60*cm));
*/
	G4GenericTrap* detectorWorldSolidovr = new G4GenericTrap("detectorWorldSolidovr",
				overallDetY/2-5*cm,
				vertices);	
	G4Box* detectorWorldSolidsub = new G4Box("detectorWorldSolidsub",
				overallDetX/100,
				overallDetY/100+20*cm/100,
				detLength/2/100+110*cm/100);
	G4UnionSolid* detectorWorldSolid = new G4UnionSolid("detectorWorldSolid",
				detectorWorldSolidsub,
				detectorWorldSolidovr,
				detRotsub,
				G4ThreeVector(0,0,-scintZ-height_pmt/2-5*cm));

/*
	G4Box* detectorWorldSolid = new G4Box("detectorWorldSolid",
				overallDetX,
				overallDetY+20*cm,
				detLength/2+110*cm);
*/
/*
	G4Para* detectorWorldSolid = new G4Para("detectorWorldSolid",
				overallDetX,
				overallDetY,
				detLength/2,
				0,worldRotation,
				0);
*/
	G4LogicalVolume* detectorWorldLogic = new G4LogicalVolume(
				detectorWorldSolid,
				worldMaterial,
				"detectorWorldLogic",
				0,0,0);

	G4PVPlacement* detectorWorldPhysic = new G4PVPlacement(
				detRot,
				G4ThreeVector(centerOffsetX+27*cm*sin(-worldRotation), 0, centerOffsetZ+27*cm*cos(-worldRotation)),
				detectorWorldLogic,
				"detectorWorldPhysic",
				logicWorld,
				false,
				0,
				true);
////////////////////////////////////////////////////////////////////////////////////
//Overall volume for 2x2 stack

	G4RotationMatrix* rot = new G4RotationMatrix(); //just an unrotated matrix used where this can't be a 0 arg
	
	G4Box* subStackSolidAlTotal = new G4Box("subStackSolidTotal",
				(2*scintXY+barSpacingInter+1.2*cm), //1.2cm is from a picture measurement
				(2*scintXY+barSpacingInter+1.2*cm),
				86*cm*2+10*cm);
				//detLength/2-7.5*cm);
	G4Box* subStackSolidAlGap = new G4Box("subStackSolidAlGap",
				0.4*cm/2, //actually 0.2cm but overlap for vis purposes and to make sure there's a gap
				6*cm/2, //1.2cm is from a picture measurement
				86*cm*2+10*cm);
				//detLength/2-7.5*cm);

	G4SubtractionSolid* subStackSolidAl1 = new G4SubtractionSolid("subStackSolidAl1",
					subStackSolidAlTotal,
					subStackSolidAlGap,
                                        rot,
                                        G4ThreeVector((2*scintXY+barSpacingInter+1.2*cm)-(0.2*cm)/2,
						      0,
                                                      0));
	
	G4SubtractionSolid* subStackSolidAl = new G4SubtractionSolid("subStackSolidAl",
					subStackSolidAl1,
					subStackSolidAlGap,
                                        rot,
                                        G4ThreeVector(-(2*scintXY+barSpacingInter+1.2*cm)+(0.2*cm)/2,
						      0,
                                                      0));

	G4LogicalVolume* subStackLogicAl = new G4LogicalVolume(
				subStackSolidAl,
				AlMat,
				"subStackLogicAl",
				0,0,0);
	
	G4Box* subStackSolid = new G4Box("subStackSolid",
				(2*scintXY+barSpacingInter+1.2*cm)-0.2*cm, //1.2cm is from a picture measurement
				(2*scintXY+barSpacingInter+1.2*cm)-0.2*cm,
				86*cm*2+10*cm);
	
	G4LogicalVolume* subStackLogic = new G4LogicalVolume(
				subStackSolid,
				worldMaterial,
				"subStackLogic",
				0,0,0);

	G4PVPlacement* subStackPhysic = new G4PVPlacement(
				0,
				G4ThreeVector(),
				subStackLogic,
				"subStackPhysic",
				subStackLogicAl,
				false,
				0,
				true);

/*
	G4PVPlacement* subStackPhysic = new G4PVPlacement(
				0,
				G4ThreeVector(),
				subStackLogic,
				"subStackPhysic",
				logicWorld,
				false,
				0,
				true);
*/
////////////////////////////////////////////////////////////////////////////////////////////////////
//Rock to be penetrated by muons as a test. Turn this off by commenting out the placement line below
////////////////////////////////////////////////////////////////////////////////////////////////////
//	G4RotationMatrix* wallRot = new G4RotationMatrix();
//	wallRot->rotateY(43.1*deg);

	G4Tubs* wallsCyl = new G4Tubs("wallsCyl",
				wallCylRadius, //rMin
				wallCylRadiusOut, //rMax
				wallZ, //half-Z
				0*deg,
				360*deg);

	G4Box* floorCutout = new G4Box("floorCutout",
				floorCutoutDepthX,
				floorCutoutDepthY,
				floorZ-10*cm);

	G4SubtractionSolid* rockSolid = new G4SubtractionSolid("rockWalls",
				wallsCyl,
				floorCutout,
				rot,
				G4ThreeVector(-floorCutoutDepthX,0,0));
				//G4ThreeVector(0,0,0));
///*
	G4LogicalVolume* rockLogic = new G4LogicalVolume(
				rockSolid,
				//
				//silicaMat,
				concreteMat,
				//PbMat, //testing muon-induced neutrons
				"rockLogic",
				0,0,0);
////////////////////////////////////////////// TURN CAVERN ON/OFF BY COMMENTING OR UNCOMMENTING THIS CODE BLOCK /////////////////////
///*
	G4PVPlacement* rockPhysic = new G4PVPlacement(
				rot,
				//wallRot,
				//G4ThreeVector(50*cm,0,0),//muonLoc: 20*cm+ScintSlabOffsetZEnd+ScintSlabZ+scintZ), //20cm from end of slab, plus add space for slabZ and bar thickness
				//G4ThreeVector(6*cm,0,0),//6cm is flush. from measurements from Neha and Teresa, shift is 4in
				//G4ThreeVector(10*cm,0,0),//6cm is flush. from measurements from Neha and Teresa, shift is 4in
				G4ThreeVector(10*cm-4*2.54*cm-5*cm+7*2.54*cm,-5*2.54*cm,0),//6cm is flush. from measurements from Neha and Teresa, shift is 4in from below, 7in from above. 5in shift to the left/negative Y
				rockLogic,
				"rockPhysic",
				logicWorld,
				false,
				0,
				true);
//*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///*	
	G4VisAttributes* rockVis = new G4VisAttributes(
			G4Colour::White());
	rockVis->SetColour(0.64,0.16,0.16,0.2);
	rockVis->SetVisibility(true);
	rockLogic->SetVisAttributes(rockVis);
//*/
/////////////////////////////////////////////////////////////////////////////////
//                            Wrapping Geometry                                //
/////////////////////////////////////////////////////////////////////////////////	

	G4Tubs* wrap_PMT_hole = new G4Tubs("wrap_PMT_hole",
			0,
			outerRadius_cath,
			(wrapThickness+airGapThickness)/2,
			0*deg,
			360*deg);	

	
///* //geometry to be used when photocathode is attached to PMT
	G4Box* wrap_solid_total = new G4Box("wrap_solid_total",
			wrapXY,
			wrapXY,
			wrapZ);

//	G4ThreeVector* subVec = new G4ThreeVector(0,0,wrapX-wrapThickness/2);
	G4SubtractionSolid* wrap_solid = new G4SubtractionSolid("wrap_solid",
			wrap_solid_total,
			wrap_PMT_hole,
			rot,
			//11 mm is y-axis PMT offset
			G4ThreeVector(0,0,-wrapZ+(airGapThickness+wrapThickness)/2));
//*/
/*
//geometry to be used when photocathode is attached to end of Bar
	G4Box* wrap_solid = new G4Box("wrap_solid",
			wrapXY,
			wrapXY,
			wrapZ);
*/

	G4LogicalVolume* wrap_logic = new G4LogicalVolume(
			wrap_solid,
			wrapMat,
			"wrap_logic",
			0, 0, 0);

	
	G4double barShift = wrapXY+barSpacingInter;
				
/*	
	G4PVPlacement* wrap_physic = new G4PVPlacement(
			0,
			G4ThreeVector(),
			wrap_logic,
			"wrap_physic",
			logicWorld,
			false,
			0,
			true);
*/
	
/////////////////////////////////////////////////////////////////////////////////
//                            Air Gap Geometry                                //
/////////////////////////////////////////////////////////////////////////////////	

	G4Box* airgap_solid_total = new G4Box("airgap_solid_total",
			airgapX,
			airgapY,
			airgapZ);

	G4Tubs* airgap_PMT_hole = new G4Tubs("airgap_PMT_hole",
			0,
			outerRadius_cath,
			airGapThickness/2,
			0*deg,
			360*deg);	

	G4RotationMatrix* rot2 = new G4RotationMatrix();
//	G4ThreeVector* subVec = new G4ThreeVector(0,0,wrapX-wrapThickness/2);
	G4SubtractionSolid* airgap_solid = new G4SubtractionSolid("airgap_solid",
			airgap_solid_total,
			airgap_PMT_hole,
			rot2,
			//11 mm is y-axis PMT offset
			G4ThreeVector(0,pmtYoffset,-airgapZ+airGapThickness/2));

	G4LogicalVolume* airgap_logic = new G4LogicalVolume(
			airgap_solid,
			worldMaterial,
			"airgap_logic",
			0, 0, 0);

	G4PVPlacement* airgap_physic = new G4PVPlacement(
			0,
			G4ThreeVector(),
			airgap_logic,
			"airgap_physic",
			wrap_logic,
			false,
			0,
			true);
	
/////////////////////////////////////////////////////////////////////////////////
//                            Scintillator Geometry                                //
/////////////////////////////////////////////////////////////////////////////////	

	G4Box* plScin_solid = new G4Box("plScin_solid",
				scintX, 
				scintY,
				scintZ);

	G4LogicalVolume* plScin_logic = new G4LogicalVolume(
				plScin_solid,
				matPlScin,
				"plScin_logic",
				0, 0, 0);

	G4PVPlacement* plScin_physic = new G4PVPlacement(
			0,
			G4ThreeVector(),
			plScin_logic,
			"plScin_physic",
			airgap_logic,
			false,
			0,
			true);
//attaching photocathode to end of bar so it detects right
/*
	 G4Tubs* phCathSolidBar = new G4Tubs(
			 "photocath_tube_Bar",
			 0,
			 outerRadius_pmt,
	//		 (airGapThickness+wrapThickness)/2,
			 height_cath/2,
			 0*deg,
			 360*deg);

	 G4LogicalVolume* phCathLogBar = new G4LogicalVolume(phCathSolidBar,
					matBiAlkali,
					"phCathLogBar");
*/
//	 G4ThreeVector phCathPosition(0,0,(-height_pmt/2 + height_cath/2));
//	 G4ThreeVector phCathPosition(0,0,scintZ+(airGapThickness+wrapThickness)/2);
//	 G4ThreeVector phCathPositionBar(0,0,scintZ-height_cath/2);
/*
	 G4PVPlacement* phCathPhysBar = new G4PVPlacement(
			 0,
			 phCathPositionBar,
			 phCathLogBar,
			 "phCathPhysBar",
			 plScin_logic,
			 //wrap_logic,
			 //pmtLog,
			 false,
			 0,
			 true);
*/

////////////////////////////////////Parameterization placement//////////////////////////

///*

	G4double sourceDist[nLayers];

	sourceDist[0]=interactionDist;
	sourceDist[1]=sourceDist[0]+(midLayerMid-frontLayerMid);
	sourceDist[2]=sourceDist[1]+(backLayerMid-midLayerMid);
	sourceDist[3]=sourceDist[2]+(fourthLayerMid-backLayerMid);

	G4double barStack_offset = (scintZ-wrapZ)/2;
	G4double layerPos = 0;
	G4double ScintSlabPlaceZ = 0;
	G4RotationMatrix rotPanL = G4RotationMatrix();
	G4RotationMatrix rotPanR = G4RotationMatrix();
	G4RotationMatrix rotPanT = G4RotationMatrix();
	rotPanT.rotateX(180*degree);
	rotPanR.rotateX(180*degree);
	rotPanR.rotateZ(90*degree);
	rotPanL.rotateY(180*degree);
	rotPanL.rotateZ(-90*degree);
	G4ThreeVector panelPlace(0,0,0);
//	G4VPVParameterisation* barParam[nLayers];
	//G4Sphere* barStack_solid[nLayers];
/*
	G4Trap* barStack_solid = new G4Trap("barStack_solid",
				nBarsY*barSpacingXY*4,
				std::abs(centerOffsetZ*2*3),
			        nBarsX*barSpacingXY*4,
			        nBarsX*barSpacingXY*4
				);
*/				

	//detRot->rotateX(90*deg);
	//detRot->rotateZ(-worldRotation); //worldRotation-45*degree
	//first 4 vertices are +z, next four are -z
	std::vector<G4TwoVector> verticesBarStack;
/*
	verticesBarStack.push_back(G4TwoVector(nBarsX*barSpacingXY/2+5*cm,detLength/2+5*cm+scintZ+height_pmt/2-52*cm));
	verticesBarStack.push_back(G4TwoVector(nBarsX*barSpacingXY/2+5*cm,-detLength/2-5*cm-scintZ-height_pmt/2+65*cm));
	verticesBarStack.push_back(G4TwoVector(-nBarsX*barSpacingXY/2-5*cm,-detLength/2-5*cm-scintZ-height_pmt/2+65*cm));
	verticesBarStack.push_back(G4TwoVector(-nBarsX*barSpacingXY/2-5*cm,detLength/2+5*cm+scintZ+height_pmt/2-52*cm));
	verticesBarStack.push_back(G4TwoVector(nBarsX*barSpacingXY/2+5*cm,detLength/2+5*cm+scintZ+height_pmt/2-52*cm));
	verticesBarStack.push_back(G4TwoVector(nBarsX*barSpacingXY/2+5*cm,-detLength/2-5*cm-scintZ-height_pmt/2+65*cm));
	verticesBarStack.push_back(G4TwoVector(-nBarsX*barSpacingXY/2-5*cm,-detLength/2-5*cm-scintZ-height_pmt/2+65*cm));
	verticesBarStack.push_back(G4TwoVector(-nBarsX*barSpacingXY/2-5*cm,detLength/2+5*cm+scintZ+height_pmt/2-52*cm));
*/
	verticesBarStack.push_back(G4TwoVector(nBarsX*barSpacingXY/2+4*cm,detLength/2+5*cm+scintZ+height_pmt/2-47*cm));
	verticesBarStack.push_back(G4TwoVector(nBarsX*barSpacingXY/2+4*cm,-detLength/2-5*cm-scintZ-height_pmt/2+30*cm));
	verticesBarStack.push_back(G4TwoVector(-nBarsX*barSpacingXY/2-4*cm,-detLength/2-5*cm-scintZ-height_pmt/2+30*cm));
	verticesBarStack.push_back(G4TwoVector(-nBarsX*barSpacingXY/2-4*cm,detLength/2+5*cm+scintZ+height_pmt/2-47*cm));
	verticesBarStack.push_back(G4TwoVector(nBarsX*barSpacingXY/2+4*cm,detLength/2+5*cm+scintZ+height_pmt/2-47*cm));
	verticesBarStack.push_back(G4TwoVector(nBarsX*barSpacingXY/2+4*cm,-detLength/2-5*cm-scintZ-height_pmt/2+30*cm));
	verticesBarStack.push_back(G4TwoVector(-nBarsX*barSpacingXY/2-4*cm,-detLength/2-5*cm-scintZ-height_pmt/2+30*cm));
	verticesBarStack.push_back(G4TwoVector(-nBarsX*barSpacingXY/2-4*cm,detLength/2+5*cm+scintZ+height_pmt/2-47*cm));

	G4GenericTrap* barStack_solidovr = new G4GenericTrap("barStack_solidovr",
				nBarsY*barSpacingXY/2+6*cm,
				verticesBarStack);
/*	
	G4Box* detectorWorldSolidsub = new G4Box("detectorWorldSolidsub",
				overallDetX/100,
				overallDetY/100+20*cm/100,
				detLength/2/100+110*cm/100);
*/
	G4UnionSolid* barStack_solid = new G4UnionSolid("barStack_solid",
				detectorWorldSolidsub,
				barStack_solidovr,
				detRotsub,
				G4ThreeVector(0,0,-scintZ-height_pmt/2-5*cm));
	
/*
	G4Box* barStack_solid = new G4Box("barStack_solid",
			        nBarsX*barSpacingXY/2,
				nBarsY*barSpacingXY/2,
				detLength/2);
*/	
	G4LogicalVolume* barStack_logic = new G4LogicalVolume(
				barStack_solid,
				worldMaterial,
				"barStack_logic",
				0,0,0);
	
	//G4PVParameterised* barParamPhys;
        //G4ThreeVector barStackPlacement(centerOffsetX,0,0);
        G4ThreeVector barStackPlacement(0,0,-2*cm);
	G4RotationMatrix* barStack_rot = new G4RotationMatrix();
	//barStack_rot->rotateX(-1*(barAngleDelta+(nBarYCount-1)*barAngleDelta)/2);
//	barStack_rot->rotateY(-1*worldRotation);


        G4double PMTStack_offset = scintZ+height_pmt/2;

	G4double PMTSourceDist[nLayers];
	for(int k=0;k<nLayers;k++) PMTSourceDist[k]=sourceDist[k]+PMTStack_offset;

	//G4Sphere* PMTStack_solid[nLayers];
	G4Box* PMTStack_solid[nLayers];
	G4VPVParameterisation* PMTParam[nLayers];
	
	G4LogicalVolume* PMTStack_logic[nLayers];
	G4PVPlacement* PMTStack_physic[nLayers];
	//G4PVPlacement* muMetalShield_physic[nLayers];
	G4PVParameterised* PMTParamPhys[nLayers];

	G4double PanelToPMT = ScintPanelY+height_pmt/2;
	G4double LPanelToPMTX= -PanelToPMT*cos(10*degree);
	G4double RPanelToPMTX= -PanelToPMT*cos(10*degree);
	G4double LPanelToPMTY= -PanelToPMT*sin(10*degree);
	G4double RPanelToPMTY= PanelToPMT*sin(10*degree);

	G4ThreeVector muMetalPlacement(centerOffsetX, 0, muMetalThickness/2); //adjusting Z in loop
        G4ThreeVector PMTStackPlacement(centerOffsetX, 0, 0); //adjusting Z in loop
	G4ThreeVector PMTSlabPlacement(ScintSlabXEnd-outerRadius_pmt+ScintSlabOffsetX, ScintSlabYEnd+height_pmt/2, 0); //adjusting Z in loop
/*
	G4ThreeVector PMTPanelPlacementTop1(ScintPanelOffsetXTop, -ScintPanelY-height_pmt/2, 0); //adjusting Z in loop
	G4ThreeVector PMTPanelPlacementRight1(ScintPanelOffsetXRight+RPanelToPMTX, ScintPanelOffsetYRight+RPanelToPMTY, -ScintPanelZ+outerRadius_pmt); //adjusting Z in loop
	G4ThreeVector PMTPanelPlacementLeft1(ScintPanelOffsetXLeft+LPanelToPMTX, ScintPanelOffsetYLeft+LPanelToPMTY, -ScintPanelZ+outerRadius_pmt); //adjusting Z in loop
	G4ThreeVector PMTPanelPlacementTop2(ScintPanelOffsetXTop, ScintPanelY+height_pmt/2, 0); //adjusting Z in loop
	G4ThreeVector PMTPanelPlacementRight2(ScintPanelOffsetXRight-RPanelToPMTX, ScintPanelOffsetYRight-RPanelToPMTY, -ScintPanelZ+outerRadius_pmt); //adjusting Z in loop
	G4ThreeVector PMTPanelPlacementLeft2(ScintPanelOffsetXLeft-LPanelToPMTX, ScintPanelOffsetYLeft-LPanelToPMTY, -ScintPanelZ+outerRadius_pmt); //adjusting Z in loop
*/
	G4ThreeVector PMTPanelPlacementTop1(ScintPanelOffsetXTop, 0, 0); //adjusting Z in loop
	G4ThreeVector PMTPanelPlacementRight1(ScintPanelOffsetXRight, ScintPanelOffsetYRight, -ScintPanelZ+outerRadius_pmt); //adjusting Z in loop
	G4ThreeVector PMTPanelPlacementLeft1(ScintPanelOffsetXLeft, ScintPanelOffsetYLeft, -ScintPanelZ+outerRadius_pmt); //adjusting Z in loop
	G4ThreeVector PMTPanelPlacementTop2(ScintPanelOffsetXTop, 0, 0); //adjusting Z in loop
	G4ThreeVector PMTPanelPlacementRight2(ScintPanelOffsetXRight, ScintPanelOffsetYRight, -ScintPanelZ+outerRadius_pmt); //adjusting Z in loop
	G4ThreeVector PMTPanelPlacementLeft2(ScintPanelOffsetXLeft, ScintPanelOffsetYLeft, -ScintPanelZ+outerRadius_pmt); //adjusting Z in loop

	G4RotationMatrix* pmt1RotSlab = new G4RotationMatrix();
	G4RotationMatrix* pmt2RotSlab = new G4RotationMatrix();
	G4RotationMatrix* pmtRotPanelTop1 = new G4RotationMatrix();
	G4RotationMatrix* pmtRotPanelBottom1 = new G4RotationMatrix();
	G4RotationMatrix* pmtRotPanelLeft1 = new G4RotationMatrix();
	G4RotationMatrix* pmtRotPanelRight1 = new G4RotationMatrix();
	G4RotationMatrix* pmtRotPanelTop2 = new G4RotationMatrix();
	G4RotationMatrix* pmtRotPanelBottom2 = new G4RotationMatrix();
	G4RotationMatrix* pmtRotPanelLeft2 = new G4RotationMatrix();
	G4RotationMatrix* pmtRotPanelRight2 = new G4RotationMatrix();

	G4RotationMatrix* rotDetPart = new G4RotationMatrix();
//	rotDetPart->rotateY(45*degree);

	pmt1RotSlab->rotateY(-90*degree);
	//pmt2RotSlab->rotateY(90*degree);
	pmt2RotSlab->rotateY(-90*degree);
	pmt2RotSlab->rotateX(-90*degree);
	//pmtRotPanelTop1->rotateX(-90*degree);
	//pmtRotPanelTop2->rotateX(90*degree);
//	pmtRotPanelTop->rotateY(-worldRotation);
//	pmtRotPanelBottom1->rotateX(-90*degree);
//	pmtRotPanelBottom2->rotateX(90*degree);
//	pmtRotPanelBottom->rotateY(-worldRotation);
	
//	pmtRotPanelRight1->rotateX(90*degree);
//	pmtRotPanelRight1->rotateY(80*degree);
//	pmtRotPanelRight2->rotateX(-90*degree);
//	pmtRotPanelRight2->rotateY(80*degree);
	
//	pmtRotPanelLeft1->rotateX(-90*degree);
//	pmtRotPanelLeft1->rotateY(80*degree);
//	pmtRotPanelLeft2->rotateX(90*degree);
//	pmtRotPanelLeft2->rotateY(80*degree);

	G4double pmtTotalOffsetZ = 0;

	G4RotationMatrix* PMTStack_rot = new G4RotationMatrix();
//	PMTStack_rot->rotateX(-1*(barAngleDelta+(nBarYCount-1)*barAngleDelta)/2);
//	PMTStack_rot->rotateY((barAngleDelta+(nBarXCount-1)*barAngleDelta)/2);


/////////////////////////////////////////////////////////////////
G4RotationMatrix* rotSlab1 = new G4RotationMatrix();
G4RotationMatrix* rotSlab2 = new G4RotationMatrix();
	//rotSlab1->rotateX(90*degree);
	rotSlab2->rotateY(90*degree);

G4Box* ScintSlabWrapSolidTotalFront1 = new G4Box("ScintSlabWrapSolidTotalFront1",
				ScintSlabWrapXFront1,
				ScintSlabWrapYFront1,
				ScintSlabWrapZFront1);

G4Box* ScintSlabWrapSolidTotalFront2 = new G4Box("ScintSlabWrapSolidTotalFront2",
				ScintSlabWrapXFront2,
				ScintSlabWrapYFront2,
				ScintSlabWrapZFront2);

G4Box* ScintSlabWrapSolidTotalEnd = new G4Box("ScintSlabWrapSolidTotalEnd",
				ScintSlabWrapXEnd,
				ScintSlabWrapYEnd,
				ScintSlabWrapZEnd);
	
G4SubtractionSolid* ScintSlabWrapSolidFront1 = new G4SubtractionSolid("ScintSlabWrapSolid1Front1",
					ScintSlabWrapSolidTotalFront1,
					wrap_PMT_hole,
					rotSlab2,
					G4ThreeVector(
						      ScintSlabWrapXFront1-(airGapThickness+wrapThickness)/2,
						      -frontPanelOverlapShift,
						      ScintSlabWrapZFront1));

G4SubtractionSolid* ScintSlabWrapSolidFront2 = new G4SubtractionSolid("ScintSlabWrapSolid1Front2",
					ScintSlabWrapSolidTotalFront2,
					wrap_PMT_hole,
					rotSlab2,
					G4ThreeVector(
						      ScintSlabWrapXFront2-(airGapThickness+wrapThickness)/2,
						      frontPanelOverlapShift,
						      -ScintSlabWrapZFront1));

G4SubtractionSolid* ScintSlabWrapSolidEnd = new G4SubtractionSolid("ScintSlabWrapSolid1End",
					ScintSlabWrapSolidTotalEnd,
					wrap_PMT_hole,
					rotSlab2,
					G4ThreeVector(
						      ScintSlabWrapXEnd-(airGapThickness+wrapThickness)/2,
						      0,
						      0));

/*G4SubtractionSolid* ScintSlabWrapSolid = new G4SubtractionSolid("ScintSlabWrapSolid",
					ScintSlabWrapSolid1,
					wrap_PMT_hole,
					rotSlab2,
					G4ThreeVector(0,
						      -1*(-ScintSlabWrapY+(airGapThickness+wrapThickness)/2),
						      -1*(ScintSlabZ-outerRadius_cath)));	
*/ 
G4LogicalVolume* ScintSlabWrapLogicFront1 = new G4LogicalVolume(
					ScintSlabWrapSolidFront1,
					wrapMat,
					"ScintSlabWrapLogicFront1",
					0,0,0);

G4LogicalVolume* ScintSlabWrapLogicFront2 = new G4LogicalVolume(
					ScintSlabWrapSolidFront2,
					wrapMat,
					"ScintSlabWrapLogicFront2",
					0,0,0);

G4LogicalVolume* ScintSlabWrapLogicEnd = new G4LogicalVolume(
					ScintSlabWrapSolidEnd,
					wrapMat,
					"ScintSlabWrapLogicEnd",
					0,0,0);

G4Box* ScintSlabAirGapSolidTotalFront1 = new G4Box("ScintSlabAirGapSolidTotalFront1",
				ScintSlabAirGapXFront1,
				ScintSlabAirGapYFront1,
				ScintSlabAirGapZFront1);

G4Box* ScintSlabAirGapSolidTotalFront2 = new G4Box("ScintSlabAirGapSolidTotalFront2",
				ScintSlabAirGapXFront2,
				ScintSlabAirGapYFront2,
				ScintSlabAirGapZFront2);

G4Box* ScintSlabAirGapSolidTotalEnd = new G4Box("ScintSlabAirGapSolidTotalEnd",
				ScintSlabAirGapXEnd,
				ScintSlabAirGapYEnd,
				ScintSlabAirGapZEnd);

G4SubtractionSolid* ScintSlabAirGapSolidFront1 = new G4SubtractionSolid("ScintSlabAirGapSolid1Front1",
					ScintSlabAirGapSolidTotalFront1,
					airgap_PMT_hole,
					rotSlab1,
					G4ThreeVector(ScintSlabAirGapYFront1-airGapThickness/2,
						      -frontPanelOverlapShift,
						      ScintSlabAirGapZFront1));

G4SubtractionSolid* ScintSlabAirGapSolidFront2 = new G4SubtractionSolid("ScintSlabAirGapSolid1Front2",
					ScintSlabAirGapSolidTotalFront2,
					airgap_PMT_hole,
					rotSlab1,
					G4ThreeVector(ScintSlabAirGapYFront2-airGapThickness/2,
						      frontPanelOverlapShift,
						      -ScintSlabAirGapZFront1));

G4SubtractionSolid* ScintSlabAirGapSolidEnd = new G4SubtractionSolid("ScintSlabAirGapSolid1End",
					ScintSlabAirGapSolidTotalEnd,
					airgap_PMT_hole,
					rotSlab1,
					G4ThreeVector(ScintSlabAirGapYEnd-airGapThickness/2,
						      0,
						      0));
/*
G4SubtractionSolid* ScintSlabAirGapSolid = new G4SubtractionSolid("ScintSlabAirGapSolid",
					ScintSlabAirGapSolid1,
					airgap_PMT_hole,
					rotSlab2,
					G4ThreeVector(0,
						      -1*(-ScintSlabAirGapY+airGapThickness/2),
						      -1*(ScintSlabZ-outerRadius_cath)));
*/
G4LogicalVolume* ScintSlabAirGapLogicFront1 = new G4LogicalVolume(
					ScintSlabAirGapSolidFront1,
					worldMaterial,
					"ScintSlabAirGapLogicFront1",
					0,0,0);

G4LogicalVolume* ScintSlabAirGapLogicFront2 = new G4LogicalVolume(
					ScintSlabAirGapSolidFront2,
					worldMaterial,
					"ScintSlabAirGapLogicFront2",
					0,0,0);

G4LogicalVolume* ScintSlabAirGapLogicEnd = new G4LogicalVolume(
					ScintSlabAirGapSolidEnd,
					worldMaterial,
					"ScintSlabAirGapLogicEnd",
					0,0,0);
	
	G4PVPlacement* airGapSlab_physicFront1 = new G4PVPlacement(
			0,
			G4ThreeVector(),
			ScintSlabAirGapLogicFront1,
			"airGapSlab_physicFront1",
			ScintSlabWrapLogicFront1,
			false,
			0,
			true);

	G4PVPlacement* airGapSlab_physicFront2 = new G4PVPlacement(
			0,
			G4ThreeVector(),
			ScintSlabAirGapLogicFront2,
			"airGapSlab_physicFront2",
			ScintSlabWrapLogicFront2,
			false,
			0,
			true);

	G4PVPlacement* airGapSlab_physicEnd = new G4PVPlacement(
			0,
			G4ThreeVector(),
			ScintSlabAirGapLogicEnd,
			"airGapSlab_physicEnd",
			ScintSlabWrapLogicEnd,
			false,
			0,
			true);


///*	
G4Box* ScintSlabSolidFront1 = new G4Box("ScintSlabSolidFront1",
				ScintSlabXFront1,
				ScintSlabYFront1,
				ScintSlabZFront1);

G4Box* ScintSlabSolidFront2 = new G4Box("ScintSlabSolidFront2",
				ScintSlabXFront2,
				ScintSlabYFront2,
				ScintSlabZFront2);

G4Box* ScintSlabSolidEnd = new G4Box("ScintSlabSolidEnd",
				ScintSlabXEnd,
				ScintSlabYEnd,
				ScintSlabZEnd);

G4LogicalVolume* ScintSlabLogicFront1 = new G4LogicalVolume(
					ScintSlabSolidFront1,
					matPlScin,
					"ScintSlabLogicFront1",
					0,0,0);

G4LogicalVolume* ScintSlabLogicFront2 = new G4LogicalVolume(
					ScintSlabSolidFront2,
					matPlScin,
					"ScintSlabLogicFront2",
					0,0,0);

G4LogicalVolume* ScintSlabLogicEnd = new G4LogicalVolume(
					ScintSlabSolidEnd,
					matPlScin,
					"ScintSlabLogicEnd",
					0,0,0);

	G4PVPlacement* slab_physicFront1 = new G4PVPlacement(
			0,
			G4ThreeVector(),
			ScintSlabLogicFront1,
			"slab_physicFront1",
			ScintSlabAirGapLogicFront1,
			false,
			0,
			true);

	G4PVPlacement* slab_physicFront2 = new G4PVPlacement(
			0,
			G4ThreeVector(),
			ScintSlabLogicFront2,
			"slab_physicFront2",
			ScintSlabAirGapLogicFront2,
			false,
			0,
			true);

	G4PVPlacement* slab_physicEnd = new G4PVPlacement(
			0,
			G4ThreeVector(),
			ScintSlabLogicEnd,
			"slab_physicEnd",
			ScintSlabAirGapLogicEnd,
			false,
			0,
			true);
//*/
//////////////////////////////////////////////////////////////////////////////
G4Box* ScintPanelWrapSolidTotal = new G4Box("ScintPanelWrapSolidTotal",
				ScintPanelWrapX,
				ScintPanelWrapY,
				ScintPanelWrapZ);

G4SubtractionSolid* ScintPanelWrapSolid1 = new G4SubtractionSolid("ScintPanelWrapSolid1",
					ScintPanelWrapSolidTotal,
					wrap_PMT_hole,
					rotSlab1,
					G4ThreeVector(0,
						      0,
						      //-ScintPanelWrapY+(airGapThickness+wrapThickness)/2,
						      //ScintPanelZ-outerRadius_pmt));	
						      ScintPanelWrapZ-(airGapThickness+wrapThickness)/2));	
/*
G4SubtractionSolid* ScintPanelWrapSolid = new G4SubtractionSolid("ScintPanelWrapSolid",
					ScintPanelWrapSolid1,
					wrap_PMT_hole,
					rotSlab2,
					G4ThreeVector(0,
						      -1*(-ScintPanelWrapY+(airGapThickness+wrapThickness)/2),
						      -1*(ScintPanelZ-outerRadius_pmt)));	
*/
G4LogicalVolume* ScintPanelWrapLogic = new G4LogicalVolume(
					ScintPanelWrapSolid1,
					wrapMat,
					"ScintPanelWrapLogic",
					0,0,0);


G4Box* ScintPanelAirGapSolidTotal = new G4Box("ScintPanelAirGapSolidTotal",
				ScintPanelAirGapX,
				ScintPanelAirGapY,
				ScintPanelAirGapZ);
G4SubtractionSolid* ScintPanelAirGapSolid1 = new G4SubtractionSolid("ScintPanelAirGapSolid1",
					ScintPanelAirGapSolidTotal,
					airgap_PMT_hole,
					rotSlab1,
					G4ThreeVector(0,
						      //-ScintPanelAirGapY+airGapThickness/2,
						      //ScintPanelZ-outerRadius_pmt));
						      0,
						      ScintPanelWrapZ-airGapThickness/2));
/*
G4SubtractionSolid* ScintPanelAirGapSolid = new G4SubtractionSolid("ScintPanelAirGapSolid",
					ScintPanelAirGapSolid1,
					airgap_PMT_hole,
					rotSlab2,
					G4ThreeVector(0,
						      -1*(-ScintPanelAirGapY+airGapThickness/2),
						      -1*(ScintPanelZ-outerRadius_pmt)));
*/
G4LogicalVolume* ScintPanelAirGapLogic = new G4LogicalVolume(
					ScintPanelAirGapSolid1,
					worldMaterial,
					"ScintPanelAirGapLogic",
					0,0,0);
	
	G4PVPlacement* airGapPanel_physic = new G4PVPlacement(
			0,
			G4ThreeVector(),
			ScintPanelAirGapLogic,
			"airGapPanel_physic",
			ScintPanelWrapLogic,
			false,
			0,
			true);


G4Box* ScintPanelSolid = new G4Box("ScintPanelSolid",
				ScintPanelX,
				ScintPanelY,
				ScintPanelZ);

G4LogicalVolume* ScintPanelLogic = new G4LogicalVolume(
					ScintPanelSolid,
					matPlScin,
					//worldMaterial,
					"ScintPanelLogic",
					0,0,0);

	G4PVPlacement* panel_physic = new G4PVPlacement(
			0,
			G4ThreeVector(),
			ScintPanelLogic,
			"panel_physic",
			ScintPanelAirGapLogic,
			false,
			0,
			true);

///////////////////////////////////////////////////////////////////////

//==========================================================================
    //---------------PMT Geometry---------------
    //==========================================================================
//////////////////// defining phCathSolid as PMT so we can parameterise it /////////////
///*
	 G4Tubs* phCathSolid = new G4Tubs(
			 "photocath_tube",
			 0,
			 outerRadius_cath,
	//		 (airGapThickness+wrapThickness)/2,
			 height_pmt/2,
			 0*deg,
			 360*deg);

	 G4LogicalVolume* phCathLog = new G4LogicalVolume(phCathSolid,
					matBiAlkali,
					"phCathLog");

//	 G4ThreeVector phCathPosition(0,0,(-height_pmt/2 + height_cath/2));
//	 G4ThreeVector phCathPosition(0,0,scintZ+(airGapThickness+wrapThickness)/2);
/*
	 G4PVPlacement* phCathPhys = new G4PVPlacement(
			 0,
			 phCathPosition,
			 phCathLog,
			 "phCathPhys",
			 wrap_logic,
//			 pmtLog,
			 false,
			 0,
			 true);
*/
////////// add pmt as daughter of photocathode (impacts visualization only)

	 G4Tubs* pmtSolid = new G4Tubs(
			 "pmt_tube",
			 outerRadius_cath,
			 outerRadius_pmt,
			 height_pmt/2,
		         //(height_pmt-height_cath)/2,
		         0*deg,
		         360*deg);

	 G4LogicalVolume* pmtLog = new G4LogicalVolume(
			 pmtSolid,
	 	 	 //matBorGlass, //actually made of matBorGlass, but the material isn't a significant source
			 worldMaterial, //so, going with air (world material) insteadhttps://www.chem.uci.edu/~unicorn/243/handouts/pmt.pdf
 	 		"pmtLog");
/*
	G4PVPlacement* pmtPhys = new G4PVPlacement(
			 0,
			 G4ThreeVector(0,0,(height_cath+50*mm)/2),
			 //G4ThreeVector(0,0,(height_cath+0.1*mm)/2),
			 pmtLog,
			 "pmtPhys",
			 phCathLog,
			 false,
			 0,
			 true);
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////
G4double topZ,leftZ,rightZ,botZ,sign, shift;
G4RotationMatrix* rotPanel = new G4RotationMatrix();
rotPanel->rotateY(-1*worldRotation);
double barSep = 86*cm; //from Sam Alcott's 3D model, using measurements of physical detector
double zShift = 12*cm;
for(int l=0;l<nLayers;l++){

	new G4PVPlacement(
		0,
		G4ThreeVector(barShift,barShift,-7.5*cm-3*wrapZ-2*height_pmt+barSep*l+zShift),
		wrap_logic,
		"wrap_physic1_L"+std::to_string(l),
		subStackLogic,
		false,
		1,
		true);

	new G4PVPlacement(
		0,
		G4ThreeVector(barShift,barShift,-7.5*cm-3*wrapZ-2*height_pmt+barSep*l-scintZ-height_pmt/2+zShift),
		//G4ThreeVector(barShift,barShift,-(3-2*l)*(2.5*cm+wrapZ)-(2-l)*height_pmt-scintZ-height_pmt/2),
		phCathLog,
		"phCath_physic1_L"+std::to_string(l),
		subStackLogic,
		false,
		1,
		true);
	
	new G4PVPlacement(
		0,
		G4ThreeVector(barShift,barShift,-7.5*cm-3*wrapZ-2*height_pmt+barSep*l-scintZ-height_pmt/2+zShift),
		pmtLog,
		"pmt_physic1_L"+std::to_string(l),
		subStackLogic,
		false,
		1,
		true);
/////////////
	new G4PVPlacement(
		0,
		G4ThreeVector(barShift,-barShift,-7.5*cm-3*wrapZ-2*height_pmt+barSep*l+zShift),
		wrap_logic,
		"wrap_physic2_L"+std::to_string(l),
		subStackLogic,
		false,
		2,
		true);

	new G4PVPlacement(
		0,
		G4ThreeVector(barShift,-barShift,-7.5*cm-3*wrapZ-2*height_pmt+barSep*l-scintZ-height_pmt/2+zShift),
		phCathLog,
		"phCath_physic2_L"+std::to_string(l),
		subStackLogic,
		false,
		2,
		true);
	
	new G4PVPlacement(
		0,
		G4ThreeVector(barShift,-barShift,-7.5*cm-3*wrapZ-2*height_pmt+barSep*l-scintZ-height_pmt/2+zShift),
		pmtLog,
		"pmt_physic2_L"+std::to_string(l),
		subStackLogic,
		false,
		2,
		true);
/////////////
	new G4PVPlacement(
		0,
		G4ThreeVector(-barShift,barShift,-7.5*cm-3*wrapZ-2*height_pmt+barSep*l+zShift),
		wrap_logic,
		"wrap_physic3_L"+std::to_string(l),
		subStackLogic,
		false,
		3,
		true);

	new G4PVPlacement(
		0,
		G4ThreeVector(-barShift,barShift,-7.5*cm-3*wrapZ-2*height_pmt+barSep*l-scintZ-height_pmt/2+zShift),
		phCathLog,
		"phCath_physic3_L"+std::to_string(l),
		subStackLogic,
		false,
		3,
		true);

	new G4PVPlacement(
		0,
		G4ThreeVector(-barShift,barShift,-7.5*cm-3*wrapZ-2*height_pmt+barSep*l-scintZ-height_pmt/2+zShift),
		pmtLog,
		"pmt_physic3_L"+std::to_string(l),
		subStackLogic,
		false,
		3,
		true);
//////////////
	new G4PVPlacement(
		0,
		G4ThreeVector(-barShift,-barShift,-7.5*cm-3*wrapZ-2*height_pmt+barSep*l+zShift),
		wrap_logic,
		"wrap_physic4_L"+std::to_string(l),
		subStackLogic,
		false,
		4,
		true);

	new G4PVPlacement(
		0,
		G4ThreeVector(-barShift,-barShift,-7.5*cm-3*wrapZ-2*height_pmt+barSep*l-scintZ-height_pmt/2+zShift),
		phCathLog,
		"phCath_physic4_L"+std::to_string(l),
		subStackLogic,
		false,
		4,
		true);

	new G4PVPlacement(
		0,
		G4ThreeVector(-barShift,-barShift,-7.5*cm-3*wrapZ-2*height_pmt+barSep*l-scintZ-height_pmt/2+zShift),
		pmtLog,
		"pmt_physic4_L"+std::to_string(l),
		subStackLogic,
		false,
		4,
		true);
	
       //////// place three scintillator panels: one above, one on each side (rotated by 10 deg.)
	if(l%2==1){
	topZ = 86*cm*l+centerOffsetZ+frontLayerMid-5.9*cm-3.7*cm; //-5.9cm is flush. 3.7cm from Sam's model
	if(l==1) topZ+=0.5*cm; //since the panels are 1.5cm apart, rather than 2cm, we adjust for that here
	leftZ = topZ;
	rightZ = leftZ;
	if(l==1) sign = 1;
	else sign = -1;
	//shift = 4*cm*sign;
	shift = 0;//4*cm*sign;
if(l==3){
		rotPanT.rotateX(180*degree);
		rotPanR.rotateX(180*degree);
		rotPanL.rotateX(180*degree);
        }
//if(l==3){topZ+=3*cm;leftZ+=3*cm;rightZ+=3*cm;botZ+=5*cm;}

	//top
			panelPlace=G4ThreeVector(ScintPanelOffsetXTop+shift,0,topZ);
		new G4PVPlacement(
			G4Transform3D(rotPanT,panelPlace),
			//0,//rotPanel,
			//G4ThreeVector(ScintPanelOffsetXTop+shift,0,topZ),
			ScintPanelWrapLogic,
			"ScintPanelTop"+std::to_string(l),
			//logicWorld,
			detectorWorldLogic,
			false,
			nLayers*nBarXCount*nBarYCount*4+(nLayers+1)+4*l,
			true);
	
	//left
			panelPlace=G4ThreeVector(ScintPanelOffsetXLeft,ScintPanelOffsetYLeft,leftZ);
//			rotPanL.rotateY(-worldRotation);
	new G4PVPlacement(
			G4Transform3D(rotPanL,panelPlace),
			ScintPanelWrapLogic,
			"ScintPanelLeft"+std::to_string(l),
			//logicWorld,
			detectorWorldLogic,
			false,
			nLayers*nBarXCount*nBarYCount*4+(nLayers+2)+4*l,
			true);
	
	//right
			panelPlace=G4ThreeVector(ScintPanelOffsetXRight,ScintPanelOffsetYRight,rightZ);
//			rotPanR.rotateY(-worldRotation);
	new G4PVPlacement(
			G4Transform3D(rotPanR,panelPlace),
			ScintPanelWrapLogic,
			"ScintPanelRight"+std::to_string(l),
			//logicWorld,
			detectorWorldLogic,
			false,
			nLayers*nBarXCount*nBarYCount*4+(nLayers+3)+4*l,
			true);
	//PMTs attached to upper and side scintillator panels
       	PMTPanelPlacementTop1.setZ(topZ+ScintPanelZ+height_pmt/2);
	PMTPanelPlacementTop1.setX(ScintPanelOffsetXTop+shift);
       	PMTPanelPlacementLeft1.setZ(leftZ+ScintPanelZ+height_pmt/2);
       	PMTPanelPlacementRight1.setZ(rightZ+ScintPanelZ+height_pmt/2);
       	PMTPanelPlacementTop2.setZ(topZ-ScintPanelZ-height_pmt/2);
	PMTPanelPlacementTop2.setX(ScintPanelOffsetXTop+shift);
       	PMTPanelPlacementLeft2.setZ(leftZ-ScintPanelZ-height_pmt/2);
       	PMTPanelPlacementRight2.setZ(rightZ-ScintPanelZ-height_pmt/2);

	//top panel
if(l==3){
	new G4PVPlacement(
			pmtRotPanelTop1,
			PMTPanelPlacementTop1,
			phCathLog,
			"ScintPanelPMTTop1"+std::to_string(l),
			//logicWorld,
			detectorWorldLogic,
			false,
			nLayers*nBarXCount*nBarYCount*4+(nLayers+1)+4*l,
			true);
	new G4PVPlacement(
			pmtRotPanelTop1,
			PMTPanelPlacementTop1,
			pmtLog,
			"pmtScintPanelPMTTop1"+std::to_string(l),
			//logicWorld,
			detectorWorldLogic,
			false,
			nLayers*nBarXCount*nBarYCount*4+(nLayers+1)+4*l,
			true);
}
if(l==1){
	new G4PVPlacement(
			pmtRotPanelTop2,
			PMTPanelPlacementTop2,
			phCathLog,
			"ScintPanelPMTTop2"+std::to_string(l),
			//logicWorld,
			detectorWorldLogic,
			false,
			nLayers*nBarXCount*nBarYCount*4+(nLayers+5)+4*l,
			true);
	new G4PVPlacement(
			pmtRotPanelTop2,
			PMTPanelPlacementTop2,
			pmtLog,
			"pmtScintPanelPMTTop2"+std::to_string(l),
			//logicWorld,
			detectorWorldLogic,
			false,
			nLayers*nBarXCount*nBarYCount*4+(nLayers+5)+4*l,
			true);
}
	//left panel
if(l==3){
	new G4PVPlacement(
			pmtRotPanelLeft1,
			PMTPanelPlacementLeft1,
			phCathLog,
			"ScintPanelPMTLeft1"+std::to_string(l),
			detectorWorldLogic,
			//logicWorld,
			false,
			nLayers*nBarXCount*nBarYCount*4+(nLayers+2)+4*l,
			true);
	new G4PVPlacement(
			pmtRotPanelLeft1,
			PMTPanelPlacementLeft1,
			pmtLog,
			"pmtScintPanelPMTLeft1"+std::to_string(l),
			detectorWorldLogic,
			//logicWorld,
			false,
			nLayers*nBarXCount*nBarYCount*4+(nLayers+2)+4*l,
			true);
}
if(l==1){
	new G4PVPlacement(
			pmtRotPanelLeft2,
			PMTPanelPlacementLeft2,
			phCathLog,
			"ScintPanelPMTLeft2"+std::to_string(l),
			detectorWorldLogic,
			//logicWorld,
			false,
			nLayers*nBarXCount*nBarYCount*4+(nLayers+6)+4*l,
			true);
	new G4PVPlacement(
			pmtRotPanelLeft2,
			PMTPanelPlacementLeft2,
			pmtLog,
			"pmtPanelPMTLeft2"+std::to_string(l),
			detectorWorldLogic,
			//logicWorld,
			false,
			nLayers*nBarXCount*nBarYCount*4+(nLayers+6)+4*l,
			true);
}
	//right panel
if(l==3){
	new G4PVPlacement(
			pmtRotPanelRight1,
			PMTPanelPlacementRight1,
			phCathLog,
			"ScintPanelPMTRight1"+std::to_string(l),
			//logicWorld,
			detectorWorldLogic,
			false,
			nLayers*nBarXCount*nBarYCount*4+(nLayers+3)+4*l,
			true);
	new G4PVPlacement(
			pmtRotPanelRight1,
			PMTPanelPlacementRight1,
			pmtLog,
			"pmtScintPanelPMTRight1"+std::to_string(l),
			//logicWorld,
			detectorWorldLogic,
			false,
			nLayers*nBarXCount*nBarYCount*4+(nLayers+3)+4*l,
			true);
}
if(l==1){
	new G4PVPlacement(
			pmtRotPanelRight2,
			PMTPanelPlacementRight2,
			phCathLog,
			"ScintPanelPMTRight2"+std::to_string(l),
			//logicWorld,
			detectorWorldLogic,
			false,
			nLayers*nBarXCount*nBarYCount*4+(nLayers+7)+4*l,
			true);
	new G4PVPlacement(
			pmtRotPanelRight2,
			PMTPanelPlacementRight2,
			pmtLog,
			"pmtScintPanelPMTRight2"+std::to_string(l),
			//logicWorld,
			detectorWorldLogic,
			false,
			nLayers*nBarXCount*nBarYCount*4+(nLayers+7)+4*l,
			true);
}
/*
	//bottom panel
if(l==3){
	new G4PVPlacement(
			pmtRotPanelBottom1,
			PMTPanelPlacementBottom1,
			phCathLog,
			"ScintPanelPMTBottom1"+std::to_string(l),
			//logicWorld,
			detectorWorldLogic,
			false,
			nLayers*nBarXCount*nBarYCount*4+(nLayers+4)+4*l,
			true);

}
if(l==1){
	new G4PVPlacement(
			pmtRotPanelBottom2,
			PMTPanelPlacementBottom2,
			phCathLog,
			"ScintPanelPMTBottom2"+std::to_string(l),
			//logicWorld,
			detectorWorldLogic,
			false,
			nLayers*nBarXCount*nBarYCount*4+(nLayers+8)+4*l,
			true);
	}
*/
	}

}
	double slabshiftback=57.5*cm-13.5*cm; //57.5 is flush //add an extra 21.5cm from Sam's model
        double slabshiftfront=34.5*cm+42.1*cm; //34.5 is flush //19.5cm+pmt_height //47.3cm from Sam's model
        G4RotationMatrix* rotSlabBot = new G4RotationMatrix();
	G4RotationMatrix* rotSlabTop = new G4RotationMatrix();
	rotSlabTop->rotateZ(180*degree);
	rotSlabBot->rotateZ(90*degree);


	//place extra scintillator slabs at the front
        new G4PVPlacement(                        
                        rotSlabBot,
                        //G4ThreeVector(-wallCylRadius+6*cm,0,-std::cos(worldRotation)*detLength+80*cm),
                        G4ThreeVector(frontPanelOverlapShift,0,-detLength/2-slabshiftfront-ScintSlabWrapZFront1),
                        ScintSlabWrapLogicFront1,
                        "ScintSlabPhysFront1",
                        //logicWorld,
                        detectorWorldLogic,
                        false,
                        nLayers*nBarXCount*nBarYCount*4+nLayers-1,
                        true);
        new G4PVPlacement(                        
                        rotSlabBot,
                        //G4ThreeVector(-wallCylRadius+6*cm,0,-std::cos(worldRotation)*detLength+80*cm),
                        G4ThreeVector(-frontPanelOverlapShift,0,-detLength/2-slabshiftfront+ScintSlabWrapZFront1),
                        ScintSlabWrapLogicFront2,
                        "ScintSlabPhysFront2",
                        //logicWorld,
                        detectorWorldLogic,
                        false,
                        nLayers*nBarXCount*nBarYCount*4+nLayers-1,
                        true);
	//place extra scintillator slab at the end
        new G4PVPlacement(                        
                        rotSlabTop,
                        //G4ThreeVector(wallCylRadius-35*cm,0,std::cos(worldRotation)*detLength-212*cm),
                        G4ThreeVector(0,0,detLength/2-slabshiftback),
                        ScintSlabWrapLogicEnd,
                        "ScintSlabPhysEnd",
                        //logicWorld,
                        detectorWorldLogic,
                        false,
                        nLayers*nBarXCount*nBarYCount*4+nLayers,
                        true);
       	//G4ThreeVector PMTSlabPlacementBot1(-wallCylRadius+6*cm,-ScintSlabY-height_pmt/2,-std::cos(worldRotation)*detLength+80*cm+ScintSlabZ-outerRadius_pmt);
       	//G4ThreeVector PMTSlabPlacementTop1(wallCylRadius-35*cm,-ScintSlabY-height_pmt/2,std::cos(worldRotation)*detLength-212*cm+ScintSlabZ-outerRadius_pmt);
       	G4ThreeVector PMTSlabPlacementBot1(0,-ScintSlabXFront1-height_pmt/2,-detLength/2-slabshiftfront);
       	G4ThreeVector PMTSlabPlacementBot2(ScintSlabYFront2+height_pmt/2,0,-detLength/2-slabshiftfront);
       	G4ThreeVector PMTSlabPlacementTop1(-ScintSlabYEnd-height_pmt/2,0,detLength/2-slabshiftback);
       	//G4ThreeVector PMTSlabPlacementBot1(0,-ScintSlabYFront1-height_pmt/2,-detLength/2-slabshiftfront);
       	//G4ThreeVector PMTSlabPlacementTop1(0,-ScintSlabYEnd-height_pmt/2,detLength/2-slabshiftback);
///*
	//top slab
	new G4PVPlacement(
			pmt1RotSlab,
			PMTSlabPlacementTop1,
			phCathLog,
			"ScintSlabPMTTop1",
			//logicWorld,
			detectorWorldLogic,
			false,
			nLayers*nBarXCount*nBarYCount*4+(nLayers+1)+4*4+1+10,
			true);
	new G4PVPlacement(
			pmt1RotSlab,
			PMTSlabPlacementTop1,
			pmtLog,
			"pmtScintSlabPMTTop1",
			//logicWorld,
			detectorWorldLogic,
			false,
			nLayers*nBarXCount*nBarYCount*4+(nLayers+1)+4*4+1+10,
			true);
	//bot slab
	new G4PVPlacement(
			pmt2RotSlab,
			PMTSlabPlacementBot1,
			phCathLog,
			"ScintSlabPMTBot1",
			//logicWorld,
			detectorWorldLogic,
			false,
			nLayers*nBarXCount*nBarYCount*4+(nLayers+1)+4*4+2+10,
			true);
	new G4PVPlacement(
			pmt2RotSlab,
			PMTSlabPlacementBot1,
			pmtLog,
			"pmtScintSlabPMTBot1",
			//logicWorld,
			detectorWorldLogic,
			false,
			nLayers*nBarXCount*nBarYCount*4+(nLayers+1)+4*4+2+10,
			true);
//*/
       	G4ThreeVector PMTSlabPlacementBot21(0,ScintSlabYFront1+height_pmt/2,-detLength/2-slabshiftfront);
       	G4ThreeVector PMTSlabPlacementBot22(0,ScintSlabYFront2+height_pmt/2,-detLength/2-slabshiftfront);
       	G4ThreeVector PMTSlabPlacementTop2(0,ScintSlabYEnd+height_pmt/2,detLength/2-slabshiftback);
       	//G4ThreeVector PMTSlabPlacementBot2(-wallCylRadius+6*cm,ScintSlabY+height_pmt/2,-std::cos(worldRotation)*detLength+80*cm-ScintSlabZ+outerRadius_pmt);
       	//G4ThreeVector PMTSlabPlacementTop2(wallCylRadius-35*cm,ScintSlabY+height_pmt/2,std::cos(worldRotation)*detLength-212*cm-ScintSlabZ+outerRadius_pmt);
	//top slab
/*
	new G4PVPlacement(
			pmt2RotSlab,
			PMTSlabPlacementTop2,
			phCathLog,
			"ScintSlabPMTTop2",
			//logicWorld,
			detectorWorldLogic,
			false,
			nLayers*nBarXCount*nBarYCount*4+(nLayers+1)+4*4+3+10,
			true);
	new G4PVPlacement(
			pmt2RotSlab,
			PMTSlabPlacementTop2,
			pmtLog,
			"pmtScintSlabPMTTop2",
			//logicWorld,
			detectorWorldLogic,
			false,
			nLayers*nBarXCount*nBarYCount*4+(nLayers+1)+4*4+3+10,
			true);
	//bot slab
	new G4PVPlacement(
			pmt2RotSlab,
			PMTSlabPlacementBot2,
			phCathLog,
			"ScintSlabPMTBot2",
			//logicWorld,
			detectorWorldLogic,
			false,
			nLayers*nBarXCount*nBarYCount*4+(nLayers+1)+4*4+4+10,
			true);
	new G4PVPlacement(
			pmt2RotSlab,
			PMTSlabPlacementBot2,
			pmtLog,
			"pmtScintSlabPMTBot2",
			//logicWorld,
			detectorWorldLogic,
			false,
			nLayers*nBarXCount*nBarYCount*4+(nLayers+1)+4*4+4+10,
			true);
*/
//*/
	G4PVPlacement* barStack_physic = new G4PVPlacement(
			barStack_rot,
			barStackPlacement,
			barStack_logic,
			"barStack_physic",
			//logicWorld,
			detectorWorldLogic,
			false,
			0,
			true);

	mqBarParameterisation* barParam = new mqBarParameterisation(
			nBarXCount,
			nBarYCount, 
			-38*cm-zShift, //z offset
			//-38*cm, //z offset
			barAngleDelta,
			interactionDist,
			barSpacingXY, //actually not using either of these two bottom parameters now, but we could if bars are different sizes
			worldRotation);

	G4PVParameterised* barParamPhys= new G4PVParameterised("barParamPhys",
			subStackLogicAl,
			barStack_logic,
			kZAxis,
			nBarXCount*nBarYCount,
			barParam,
			true); //checking overlaps


// hodoscope scintillator placement
/* defined above, reminding of the variables here
	G4double HodoX = 8.6*cm/2; //width
	G4double HodoY = 45*cm/2; //length //horizontal bars by default
	G4double HodoZ = 4.2*cm/2; //thickness

	G4double HodoOffsetVertYRight = 4.8*cm;
	G4double HodoOffsetVertYLeft = -HodoOffsetVertYRight;
	G4double HodoOffsetVertX = -2.5*cm;

	G4double HodoOffsetHorXUpper = 10.4*cm;
	G4double HodoOffsetHorXLower = 0.8*cm;
	G4double HodoOffsetHorY = 0*cm;

	G4double HodoOffsetVertZFront1 = ScintSlabOffsetZ0-8.6*cm;
	G4double HodoOffsetHorZFront1 = ScintSlabOffsetZ0-3.9*cm;
	G4double HodoOffsetHorZBack = ScintSlabOffsetZEnd+5.6*cm;
	G4double HodoOffsetVertZBack = ScintSlabOffsetZEnd+11.1*cm;
*/
/*
G4Box* HodoScintSolid = new G4Box("HodoScintSolid", //horizontal by default
				HodoX,
				HodoY, 
				HodoZ);

G4Box* HodoWrapSolid = new G4Box("HodoWrapSolid", //horizontal by default
				HodoWrapX,
				HodoWrapY, 
				HodoWrapZ);


G4LogicalVolume* HodoWrapLogic = new G4LogicalVolume(
					HodoWrapSolid,
					wrapMat,
					"HodoWrapLogic",
					0,0,0);

G4LogicalVolume* HodoScintLogic = new G4LogicalVolume(
					HodoScintSolid,
					matPlScin,
					"HodoScintLogic",
					0,0,0);

	G4PVPlacement* HodoScintPhysic = new G4PVPlacement(
			0,
			G4ThreeVector(),
			HodoScintLogic,
			"HodoScintPhysic",
			HodoWrapLogic,
			false,
			0,
			true);

	G4RotationMatrix rotHor = G4RotationMatrix();
	G4RotationMatrix rotVert = G4RotationMatrix();
	rotVert.rotateZ(90*degree);
	
//8 placements, 4 on each side

	G4ThreeVector hodoVFL(HodoOffsetVertX,HodoOffsetVertYLeft,HodoOffsetVertZFront1);
	G4ThreeVector hodoVFR(HodoOffsetVertX,HodoOffsetVertYRight,HodoOffsetVertZFront1);
	G4ThreeVector hodoVBL(HodoOffsetVertX,HodoOffsetVertYLeft,HodoOffsetVertZBack);
	G4ThreeVector hodoVBR(HodoOffsetVertX,HodoOffsetVertYRight,HodoOffsetVertZBack);
	G4ThreeVector hodoHFU(HodoOffsetHorXUpper,HodoOffsetHorY,HodoOffsetHorZFront1);
	G4ThreeVector hodoHFL(HodoOffsetHorXLower,HodoOffsetHorY,HodoOffsetHorZFront1);
	G4ThreeVector hodoHBU(HodoOffsetHorXUpper,HodoOffsetHorY,HodoOffsetHorZBack);
	G4ThreeVector hodoHBL(HodoOffsetHorXLower,HodoOffsetHorY,HodoOffsetHorZBack);
	
	//front left (negative in Y), looking down from the back of the detector
	new G4PVPlacement(
			G4Transform3D(rotVert,hodoVFL),
			HodoWrapLogic,
			"HodoVertFront1Left",
			detectorWorldLogic,
			false,
			0,
			true);
	
	//front right (positive in Y), looking down from the back of the detector
	new G4PVPlacement(
			G4Transform3D(rotVert,hodoVFR),
			HodoWrapLogic,
			"HodoVertFront1Right",
			detectorWorldLogic,
			false,
			1,
			true);
	

	//front upper, looking down from the back of the detector
	new G4PVPlacement(
			G4Transform3D(rotHor,hodoHFU),
			HodoWrapLogic,
			"HodoHorFront1Upper",
			detectorWorldLogic,
			false,
			2,
			true);
	
	//front lower, looking down from the back of the detector
	new G4PVPlacement(
			G4Transform3D(rotHor,hodoHFL),
			HodoWrapLogic,
			"HodoHorFront1Lower",
			detectorWorldLogic,
			false,
			3,
			true);

	//back upper, looking down from the back of the detector
	new G4PVPlacement(
			G4Transform3D(rotHor,hodoHBU),
			HodoWrapLogic,
			"HodoHorBackUpper",
			detectorWorldLogic,
			false,
			4,
			true);
	
	//back lower, looking down from the back of the detector
	new G4PVPlacement(
			G4Transform3D(rotHor,hodoHBL),
			HodoWrapLogic,
			"HodoHorBackLower",
			detectorWorldLogic,
			false,
			5,
			true);


	//back left, looking down from the back of the detector
	new G4PVPlacement(
			G4Transform3D(rotVert,hodoVBL),
			HodoWrapLogic,
			"HodoVertBackLeft",
			detectorWorldLogic,
			false,
			6,
			true);
	
	//back right, looking down from the back of the detector
	new G4PVPlacement(
			G4Transform3D(rotVert,hodoVBR),
			HodoWrapLogic,
			"HodoVertBackRight",
			detectorWorldLogic,
			false,
			7,
			true);

*/
//*/		
//**********       Trigger Paddle (upper)    **************
//
//**********************************************************

/*
	G4double trigLengthX_up = 2/2*cm;
	G4double trigLengthY_up = 35/2*cm;
	G4double trigLengthZ_up = 2/2*cm;
	
	G4double trigWrapThickness = 0.2*mm; //thickness of 2 layers of black electrical tape
	
	G4double trigPadOffsetX_up = wrapX+3*(trigLengthX_up+trigWrapThickness);

	G4Box* trigPad_solid_up = new G4Box("trigPad_solid_up",trigLengthX_up,trigLengthY_up,trigLengthZ_up);

	G4LogicalVolume* trigPad_logic_up = new G4LogicalVolume(trigPad_solid_up, worldMaterial,
						"trigPad_logic_up", 0,0,0);
	
	G4ThreeVector padPos_up(trigPadOffsetX_up, 0, measurementPos);

	G4PVPlacement* trigPad_physic_up = new G4PVPlacement(
			0,
			padPos_up,
			trigPad_logic_up,
			"trigPad_physic_up",
			logicWorld,
			false,
			0,
			true);
	
	trigPad_logic_up->SetVisAttributes(visAttribplScin);
*/



//**********       Trigger Paddle (lower)    **************
//
//**********************************************************
/*
	G4double trigLengthX_low = 2/2*cm;
	G4double trigLengthY_low = 2/2*cm;
	G4double trigLengthZ_low = 35/2*cm;
	
	G4double trigPadOffsetX_low = wrapX+trigLengthX_low+trigWrapThickness;

	G4Box* trigPad_solid_low = new G4Box("trigPad_solid_low",trigLengthX_low,trigLengthY_low,trigLengthZ_low);

	G4LogicalVolume* trigPad_logic_low = new G4LogicalVolume(trigPad_solid_low, worldMaterial,
						"trigPad_logic_low", 0,0,0);
	
	G4ThreeVector padPos(trigPadOffsetX_low, 0, measurementPos);

	G4PVPlacement* trigPad_physic_low = new G4PVPlacement(
			0,
			padPos,
			trigPad_logic_low,
			"trigPad_physic_low",
			logicWorld,
			false,
			0,
			true);
	
	trigPad_logic_low->SetVisAttributes(visAttribplScin);
*/
/*
//mu metal shielding
G4Box* muMetalOuterShield = new G4Box("muMetalOuterShield",
			(2*outerRadius_pmt+(nBarXCount-1)*barSpacingXY)/2+muMetalThickness,
			(2*outerRadius_pmt+(nBarXCount-1)*barSpacingXY)/2+muMetalThickness,
			//(2*wrapXY+(nBarXCount-1)*barSpacingXY)/2+muMetalThickness,
			//(2*wrapXY+(nBarXCount-1)*barSpacingXY)/2+muMetalThickness,
			height_pmt/2+muMetalThickness/2);
*/
////////////////////////////// PMT Parameterization placement ////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
///*
/*
G4SubtractionSolid* muMetalShield = new G4SubtractionSolid("muMetalShield",
					muMetalOuterShield,
					PMTStack_solid,
					rot,
					G4ThreeVector(0,0,-muMetalThickness/2));

G4LogicalVolume* muMetalShield_logic = new G4LogicalVolume(
					muMetalShield,
					worldMaterial,
					"muMetalShield_logic",
					0,0,0);			
*/
        
////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	The PMTs that are actually placed below are strictly the ones coupled to slabs and panels
//
//	The bar-coupled photocathodes actually can't be placed in a separate parameterization volume
//	(except either with expensive multi-union solids or perhaps adjusting the individual end of
//	every bar; it's possible but a headache). So to solve this problem cheaply, I'm just going to
//	move the photocathode volume to be a daughter of the scintillator bars. The rest of the readout
//	PMTs should otherwise be unaffected though.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////    G4 Vis attributes        /////////////////////////////
	
	G4VisAttributes* visAttribplScinExt = new G4VisAttributes(G4Colour::Cyan());
	visAttribplScinExt->SetVisibility(false);
	
	G4VisAttributes* visAttribWrap = new G4VisAttributes(
			G4Colour::White());
	visAttribWrap->SetVisibility(true);

	G4VisAttributes* visAttribWrapExt = new G4VisAttributes(
			G4Colour::White());
	visAttribWrapExt->SetColour(0.5,0.5,0.5,0.3);
	visAttribWrapExt->SetVisibility(true);

	G4VisAttributes* visAttribLead = new G4VisAttributes(
			G4Colour::Black());
	visAttribLead->SetVisibility(true);
	
	G4VisAttributes* visAttribMuMetal = new G4VisAttributes(
			G4Colour::Black());
	visAttribMuMetal->SetColour(0,0,0,0.1);
	visAttribMuMetal->SetVisibility(true);
	
	G4VisAttributes* visAttribAl = new G4VisAttributes(
			G4Colour::Gray());
	visAttribAl->SetColour(0,0.2,0.35,0.3);
	visAttribAl->SetVisibility(true);
	visAttribAl->SetForceWireframe(true);
	

	wrap_logic->SetVisAttributes(visAttribWrap);
	ScintPanelWrapLogic->SetVisAttributes(visAttribWrapExt);
	ScintSlabWrapLogicFront1->SetVisAttributes(visAttribWrapExt);
	ScintSlabWrapLogicFront2->SetVisAttributes(visAttribWrapExt);
	//ScintPanelAirGapLogic->SetVisAttributes(visAttribplScinExt);
	ScintSlabAirGapLogicFront1->SetVisAttributes(visAttWorld);
	ScintSlabAirGapLogicFront2->SetVisAttributes(visAttWorld);
	ScintSlabAirGapLogicEnd->SetVisAttributes(visAttWorld);
	ScintPanelAirGapLogic->SetVisAttributes(visAttWorld);
	plScin_logic->SetVisAttributes(visAttribplScin);
	ScintSlabLogicFront1->SetVisAttributes(visAttribplScinExt);
	ScintSlabLogicFront2->SetVisAttributes(visAttribplScinExt);
	ScintSlabLogicEnd->SetVisAttributes(visAttribplScinExt);
	ScintPanelLogic->SetVisAttributes(visAttribplScinExt);
	//HodoWrapLogic->SetVisAttributes(visAttribWrapExt);
	//HodoScintLogic->SetVisAttributes(visAttribplScinExt);

		
//	muMetalShield_logic->SetVisAttributes(visAttribMuMetal);
	//detectorWorldLogic->SetVisAttributes(visAttribWrapExt);
	detectorWorldLogic->SetVisAttributes(visAttWorld);
//	detectorWorldLogic->SetVisAttributes(visAttribWrapExt);
//	subStackLogic->SetVisAttributes(visAttribplScin);	
	subStackLogicAl->SetVisAttributes(visAttribAl);
	subStackLogic->SetVisAttributes(visAttWorld);	
//	barStack_logic->SetVisAttributes(visAttribplScin);	
//	barStack_logic->SetVisAttributes(visAttribWrapExt);	
	barStack_logic->SetVisAttributes(visAttWorld);
////////////////////////////////////////////////////////////////////////////
// Long Al Support Placement


/*//////////////////////////////////////////////////////////////////////////////////////////////////////
*/
	 //the "photocathode" is a metal slab at the front of the glass that
	 //is only an approximation of the real thing since it only
	 //absorbs or detects the photons based on the efficiency set

//////////////////// moving placement of phCathSolid from PMT to the      ////////////////////////
//////////////////// Bar volume s.t. there are no logical volume overlaps ///////////////////////

   //sensitive detector is not actually on the photocathode.
   //processHits gets done manually by the stepping action.
   //It is used to detect when photons hit and get absorbed&detected at the
   //boundary to the photocathode (which doesnt get done by attaching it to a
   //logical volume.
   //It does however need to be attached to something or else it doesnt get

    //==========================================================================
    //---------------Scintillator Sensitive Detector---------------
    //==========================================================================
/*
	G4SDManager* SDman = G4SDManager::GetSDMpointer();
	G4String SDnameC = "MilliQan/ScintiSensitiveDetector";
	mqScintSD* scintSD = new mqScintSD(SDnameC);
	SDman->AddNewDetector ( scintSD );

	plScin_logic->SetSensitiveDetector(scintSD);

	const int nbAllPmts = nBarXCount*nBarYCount;
  */  
   //reset at the begining of events
   //==========================================================================
   //---------------PMT Sensitive Detector---------------
   //==========================================================================
   G4String SDnameOfPMTDetector = "PMT_SD";
   mqPMTSD* myPMTSD = new mqPMTSD(SDnameOfPMTDetector);

   G4SDManager::GetSDMpointer()->AddNewDetector(myPMTSD);
//	myPMTSD->InitPMTs(nbAllPmts);
	myPMTSD->SetR878_QE(GetPMTEff_R878());
	myPMTSD->SetR7725_QE(GetPMTEff_R7725());
	myPMTSD->SetET9814B_QE(GetPMTEff_ET9814B());
	phCathLog->SetSensitiveDetector(myPMTSD); //change HERE for PMT Scenario
//	phCathLogBar->SetSensitiveDetector(myPMTSD); //change HERE for PMT Scenario

   G4String SDnameOfScintDetector = "Scint_SD";
   mqScintSD* myScintSD = new mqScintSD(SDnameOfScintDetector);

   G4SDManager::GetSDMpointer()->AddNewDetector(myScintSD);
//   	wrap_logic->SetSensitiveDetector(myScintSD);
//   	ScintSlabWrapLogic->SetSensitiveDetector(myScintSD);
//   	ScintPanelWrapLogic->SetSensitiveDetector(myScintSD);

//	myPMTSD->InitPMTs(nbAllPmts);
//	myPMTSD->SetR878_QE(GetPMTEff_R878());
//	myPMTSD->SetR7725_QE(GetPMTEff_R7725());
//	myPMTSD->SetET9814B_QE(GetPMTEff_ET9814B());
//	phCathLog->SetSensitiveDetector(myPMTSD); //change HERE for PMT Scenario


//  	G4ThreeVector PMTPosition1 = G4ThreeVector(0,pmtYoffset,scintZ+height_pmt/2+(nLayers-1)*layerSpacing/2);
  //  	G4RotationMatrix rotm1  = G4RotationMatrix();//(G4ThreeVector(0, -1., 0. ), pi / 2.0);
//	G4Transform3D transform = G4Transform3D(rotm1,PMTPosition1);
/*
	G4PVPlacement* pmtPhys = new G4PVPlacement(transform,   //rotation,position
			                     pmtLog,                   //its logical volume
			                     "pmt_physic",                //its name
			                     logicWorld,              //its mother  volume
			                     false,                    //no boolean operation
			                     nbAllPmts,             //copy number
			                     true);           //checking overlaps
*/
	// visual attributes for all PMTs;
	G4VisAttributes* visAttribPMT = new G4VisAttributes(G4Colour::Blue());
	visAttribPMT->SetForceWireframe(false);
	visAttribPMT->SetForceSolid(true);
	visAttribPMT->SetVisibility(true);

	//setting same color so it doesn't look bad in isometric view
	
	G4VisAttributes* visAttribPhCath = new G4VisAttributes(G4Colour::Blue());
	//G4VisAttributes* visAttribPhCath = new G4VisAttributes(G4Colour::Red());
	visAttribPhCath->SetForceWireframe(false);
	visAttribPhCath->SetForceSolid(false);
	visAttribPhCath->SetVisibility(true);

	pmtLog->SetVisAttributes(visAttribPMT);
	phCathLog->SetVisAttributes(visAttribPhCath);
	//phCathLogBar->SetVisAttributes(visAttribPhCath);

	//==============================================================================
	// Optical Surfaces
	//==============================================================================
	// Optical Properties
	// Note that, as mentioned in HyperNews posts and the G4AppDev Guide, the definition
	// of a logical bordfer surface involves a ordered pair of materials, such that
	// despite there being no clearly defined structure/class in the prototpye, the
	// order in which the surfaces are listed defines the orientation of the surface;
	// particles "see" this surface only when traveling from the first listed physical
	// volume into the second, which is why I have created a pair of complementary
	// surfaces for every interface (producing identical, bidirectional interactions).


    G4OpticalSurface* opSDielectricBiAlkali = new G4OpticalSurface("Detector", unified,
                                                    polished, dielectric_metal);
    //////////////////
    G4MaterialPropertiesTable* mtphcath =  matBiAlkali->GetMaterialPropertiesTable();
    opSDielectricBiAlkali->SetMaterialPropertiesTable(mtphcath);
    opSDielectricBiAlkali->SetMaterialPropertiesTable(SetOpticalPropertiesOfPMT());

    G4OpticalSurface* opSWrapScintillator = new G4OpticalSurface("Wrapping", unified,
                              /*ground*/      groundteflonair,
                                                dielectric_metal);
	const G4int nEntriesWrap = 2;
	G4double photonEnergyWrap[nEntriesWrap]={ 1.5 * eV,6.3 * eV};
	G4double wrap_REFL[nEntriesWrap] = {wrapRefl,wrapRefl};//{0.95,0.95}
//	G4double wrap_RIND[nEntriesWrap];

	G4MaterialPropertiesTable* mptWrap = new G4MaterialPropertiesTable();
	//mptWrap->AddProperty("TRANSMITTANCE",photonEnergyWrap, foil_REFL, nEntriesWrap);//->SetSpline(true);
	mptWrap->AddProperty("REFLECTIVITY",photonEnergyWrap, wrap_REFL, nEntriesWrap);
    
    opSWrapScintillator->SetMaterialPropertiesTable(mptWrap);
////////////////// PMT non-photocath refl ///////////////////////////
    G4OpticalSurface* opSPMT = new G4OpticalSurface("PMT Dead Zone", unified,
                              /*ground*/      polished,
                                                dielectric_metal);
        G4double photonEnergyPMT[nEntriesWrap]={ 1.5 * eV,6.3 * eV};
        G4double wrap_PMT[nEntriesWrap] = {0,0};//{0.95,0.95}
//      G4double wrap_RIND[nEntriesWrap];

        G4MaterialPropertiesTable* mptPMT = new G4MaterialPropertiesTable();
        //mptWrap->AddProperty("TRANSMITTANCE",photonEnergyWrap, foil_REFL, nEntriesWrap);//->SetSpline(true);
        mptPMT->AddProperty("REFLECTIVITY",photonEnergyPMT, wrap_PMT, nEntriesWrap);

    opSPMT->SetMaterialPropertiesTable(mptPMT);
/////////////////////////////////////////////////////////////////////// 

//define optical surfaces for each object in the sim, and match the optical properties of each surface to it

    new G4LogicalSkinSurface("Wrap", wrap_logic, opSWrapScintillator);
    new G4LogicalSkinSurface("Wrap", ScintPanelWrapLogic, opSWrapScintillator);
    new G4LogicalSkinSurface("Wrap", ScintSlabWrapLogicFront1, opSWrapScintillator);
    new G4LogicalSkinSurface("Wrap", ScintSlabWrapLogicEnd, opSWrapScintillator);
    new G4LogicalSkinSurface("PhCath", phCathLog, opSDielectricBiAlkali);
    new G4LogicalSkinSurface("PMTDeadZone", pmtLog, opSPMT);

//Connect detector volume (photocathode) to each surface which is in contact with it
// use this if you want to explicitly define the allowed optical surfaces rather than a wrapping
//        new G4LogicalBorderSurface("Glass->PhCath",
//			pmtPhys,phCathPhys,opSDielectricBiAlkali);
//			PMTParam_phys,phCathPhys,opSDielectricBiAlkali);

//	new G4LogicalBorderSurface("PhCath->Glass",
//			phCathPhys,pmtPhys,opSDielectricBiAlkali);
//			phCathPhys,PMTParam_phys,opSDielectricBiAlkali);
/*
    new G4LogicalBorderSurface("Scinti->PhCathBar",
//			barParamPhys[0], PMTParamPhys[0] ,opSDielectricBiAlkali);
			plScin_physic, phCathPhys ,opSDielectricBiAlkali);

    new G4LogicalBorderSurface("PhCathBar->Scinti",
//    			PMTParamPhys[0] ,barParamPhys[0] ,opSDielectricBiAlkali);
    			phCathPhys ,plScin_physic ,opSDielectricBiAlkali);
*/
/*
    new G4LogicalBorderSurface("AirGap->PhCath",
			airgap_physic, phCathPhysBar ,opSDielectricBiAlkali);

    new G4LogicalBorderSurface("PhCath->AirGap",
    			phCathPhysBar ,airgap_physic ,opSDielectricBiAlkali);
*/
//*/


    return physicWorld;

}

void mqDetectorConstruction::SetMagField(G4double fieldValueX, G4double fieldValueY, G4double fieldValueZ) {

  fMonFieldSetup->SetMagField(fieldValueX, fieldValueY, fieldValueZ);

}

//Needed for sorting spectra
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


G4MaterialPropertiesTable* mqDetectorConstruction::SetOpticalPropertiesOfPS(){


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
	G4cout << "mqOpticalFilePath is: " << mqOpticalFilePath << G4endl;
	ReadEJ200.open(mqOpticalFilePath+"EJ200ScintSpectrum.txt");
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
	//std::reverse(photonEnergy, photonEnergy+nEntries); //Must reverse to put in ascending order. G4p11
	//std::reverse(EJ200_SCINT, EJ200_SCINT + nEntries); //Must reverse to put in ascending order. G4p11
	SortProperty(photonEnergy, EJ200_SCINT, ScintEntry);


	for (int i = 0; i < nEntries; i++) {
		EJ200_RIND[i] = 1.58;//58; // refractive index at 425 nm
		//EJ200_ABSL[i] *= myPSAttenuationLength;
		EJ200_ABSL[i] = 3.8*m;//2.5 * m; // bulk attenuation at 425 nm
	}

	//mptPlScin->AddProperty("FASTCOMPONENT", photonEnergy, EJ200_SCINT,   //OLD, FASTCOMPONENT obsolete
	//		ScintEntry);//->SetSpline(true);
	mptPlScin->AddProperty("SCINTILLATIONCOMPONENT1", photonEnergy, EJ200_SCINT,
			ScintEntry);//->SetSpline(true);


	mptPlScin->AddProperty("ABSLENGTH", photonEnergy, EJ200_ABSL,
				nEntries);//->SetSpline(true);

	mptPlScin->AddConstProperty("SCINTILLATIONYIELD", 10000. / MeV); //--- according to EJ200
	mptPlScin->AddConstProperty("RESOLUTIONSCALE", 1.0);
	//mptPlScin->AddConstProperty("FASTTIMECONSTANT", 2.1 * ns); //decay time, according to EJ200  //FASTTIMECONSTANT obsolete
	mptPlScin->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 2.1 * ns); //decay time, according to EJ200
	mptPlScin->AddProperty("RINDEX", photonEnergy, EJ200_RIND, nEntries);//->SetSpline(true);

return mptPlScin;
}

G4MaterialPropertiesTable* mqDetectorConstruction::SetOpticalPropertiesOfPMT(){

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
	ReadPMTQEff.open(mqOpticalFilePath+"PMT_R878_QE_orig.txt");
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
	//std::reverse(photonEnergyPMT, photonEnergyPMT+nEntriesPMT); //Must reverse to put in ascending order. G4p11
	//std::reverse(photocath_EFF, photocath_EFF + nEntriesPMT); //Must reverse to put in ascending order. G4p11
	SortProperty(photonEnergyPMT, photocath_EFF, nEntriesPMT);
/*	
	G4double PhCath_REFL[nEntriesPMT] = { // to be determined
			0., 0.,0.,0.,0.,0.,0.,0.,0.,0.,
			0., 0.,0.,0.,0.//,0.,0.,0.,0.,0.,
			//0., 0.,0.,0.,0.,0.,0.//,0.,0.,0.,
			//0., 0.,0.,0.,0.,0.//,0.
			};
*/

	mptPMT->AddProperty("REFLECTIVITY", photonEnergyPMT,PhCath_REFL, nEntriesPMT);//->SetSpline(true);
	mptPMT->AddProperty("EFFICIENCY",photonEnergyPMT,photocath_EFF,nEntriesPMT);//->SetSpline(true);

	G4cout << "successfully at end of PMT optical table" << G4endl;
	
	return mptPMT;

}

G4PhysicsVector mqDetectorConstruction::GetPMTEff_R878(){
	
	std::ifstream ReadPMTQEff;
	ReadPMTQEff.open(mqOpticalFilePath+"PMT_R878_QE.txt");
	G4PhysicsVector effVec;
	effVec.Retrieve(ReadPMTQEff,true);
	if (effVec.GetVectorLength()!=0) G4cout << "Quantum Efficiency successfully retrieved for PMT_R878_QE" << G4endl;
	else G4cout << "ERROR: Vector length is zero!" << G4endl;
        effVec.ScaleVector(1,1); //increase QE here if you want to scale to match experiment
	ReadPMTQEff.close();
	return effVec;
}

G4PhysicsVector mqDetectorConstruction::GetPMTEff_R7725(){
	
	std::ifstream ReadPMTQEff;
	ReadPMTQEff.open(mqOpticalFilePath+"PMT_R7725_QE.txt");
	G4PhysicsVector effVec;
	effVec.Retrieve(ReadPMTQEff,true);
	if (effVec.GetVectorLength()!=0) G4cout << "Quantum Efficiency successfully retrieved for PMT R7725" << G4endl;
	else G4cout << "ERROR: Vector length is zero!" << G4endl;
        effVec.ScaleVector(1,1); //increase QE here if you want to scale to match experiment
	ReadPMTQEff.close();
	return effVec;
}

G4PhysicsVector mqDetectorConstruction::GetPMTEff_ET9814B(){
	std::ifstream ReadPMTQEff;
	ReadPMTQEff.open(mqOpticalFilePath+"PMT_ET9814B_QE.txt");
	G4PhysicsVector effVec;
	effVec.Retrieve(ReadPMTQEff,true);
	if (effVec.GetVectorLength()!=0) G4cout << "Quantum Efficiency successfully retrieved for PMT ET9814B" << G4endl;
	else G4cout << "ERROR: Vector length is zero!" << G4endl;
        effVec.ScaleVector(1,1); //increase QE here if you want to scale to match experiment
	ReadPMTQEff.close();
	return effVec;
}

void mqDetectorConstruction::UpdateGeometry() {
	if (verbose >= 0) {
		G4cout << "MilliQan> Update geometry." << G4endl;
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


void mqDetectorConstruction::SetDefaults(){
  //Resets to default values


  updated=true;
}



