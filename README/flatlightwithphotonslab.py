#####################################################################################################################################

########### A script to convert MilliQan simulation data to a flat, lightweight tree in the format of experimental data #############

###########                                         9/8/23, R.Schmitz                                                   #############

#####################################################################################################################################

###### Designed to be run over EDep-only data, minimal outputs ######

import ROOT
import sys
import math

# Function to convert sim copy number to data copy number for Scint
def slabSimToDataScint(simChannel):

    # for slabs, we have a different mapping
    # given by 18+(4*y+12*z+i). i is the layer number, y is the column number, z is the row number
    # 0 is in the bottom right of the lowest layer, 8 is in the lower left, 36 is in the top right, 44 is in the top left
    # 1 is in the bottom right of the second layer, 9 is in the lower left, 37 is in the top right, 45 is in the top left
    # and so on
    # we can use this to map the sim channel to the data channel

    simChannel=simChannel-18
    simlayer=simChannel % 4
    simrow=math.floor((simChannel-simlayer)/12) # 0 at the bottom, 3 at the top
    simcolumn=((simChannel-simlayer)%12)/4 # 0 at the right, 2 at the left

    datalayer=simlayer #do any manipulations between the mappings that is necessary
    datarow=simrow
    datacolumn=simcolumn

    return(12*datalayer+4*datacolumn+datarow)

# Function to convert sim copy number to data copy number for PMTs
def slabSimToDataPMT(simChannel):

    # The PMTs are numbered as (num)+4*(4*y+12*z+i), num=18,19,20, or 21
    # the slabs are numbered as 18+4*y+12*z+i
    # we want to convert the pmt number to the data slab number. So we subtract off the num first, then divide by 4

    simChannel=simChannel-18
    simChannel=simChannel-simChannel%4
    simChannel=simChannel/4
    return(slabSimToDataScint(simChannel+18))

def pmtPosition(simChannel, column, row, layer):
    """
    Determine whether PMTs are at the front or back of the slab
    Return: 
    0 -> PMT is at the front of the slab
    1 -> PMT is at the back of the slab
    """
    simChannel -= (4 * ((4 * column)+ (12 * row) + layer))
    # Ensure that we did the subtraction correctly
    assert simChannel in (18, 19, 20, 21), "Conversion done incorrectly."

    return 0 if simChannel in (18, 19) else 1
    
# Function to populate the vectors in the flattened tree for ScintHits
def populate_vectors_scint(input_tree, scint_copyNo, scint_layer, scint_row, scint_column, scint_nPE, scint_time):
    # Clear the vectors
    scint_copyNo.clear()
    scint_layer.clear()
    scint_row.clear()
    scint_column.clear()
    scint_nPE.clear()
    scint_time.clear()

    # make an array of length 1000 to store temporary nPE values
    temp_nPE = [0]*1000
    temp_time = [0]*1000

    # Populate the vectors with flattened data
    for j in range(input_tree.ScintRHits.size()):
        hit = input_tree.ScintRHits.at(j)
        #make temp variable to hold copy number, then convert it to data copy number
        tempCopyNo = hit.GetCopyNo()
        #tempCopyNo = simToDataScint(tempCopyNo)
        # add the hit to the temporary array, using the energy deposition as the nPE and converting it using a measure scale factor
        # also multiply by 7.5/11, the mean ratio of the measured nPE to the simulated nPE for the same geometry
        # this way we have three different ways to scale the nPE according to the geometry and correcting for the difference between simulation and data
        
        # for slabs, 19k NPE per 20 MeV. That's 950 NPE per MeV. But the slab sim was run at 2x light, so use 475 NPE per MeV
        # keep in mind this has not been calibrated experimentally, so some assumption
        simToDataScale = 0.682*0.5 #using average calibration measured in data, plus a correction for EDep-to-NPE using simulated NPE amounts. Basically a pre-correction to the nPEPerMeV below, might need additional tuning
        nPEPerMeV = 950.0*simToDataScale #measured using slab cosmic dataset
        temp_nPE[tempCopyNo] = temp_nPE[tempCopyNo] + hit.GetEDep()*nPEPerMeV

        # if the hit time in the channel is lower than the current time, replace the current time with the new time
        if(temp_time[tempCopyNo] == 0 or hit.GetHitTime() < temp_time[tempCopyNo]):
            temp_time[tempCopyNo] = hit.GetHitTime()

    # loop over the temporary arrays and add the values to the vectors
    for j in range(1000):
        # only save values where the nPE is greater than 0
        if(temp_nPE[j] > 0):
            scint_nPE.push_back(temp_nPE[j])
            scint_time.push_back(temp_time[j])
            dataChan = slabSimToDataScint(j)
            scint_copyNo.push_back(int(dataChan))
            scint_layer.push_back(int(dataChan/12))
            scint_row.push_back(int(dataChan%4)+1)
            scint_column.push_back(int((dataChan%12)/4)+1)

# Function to create the branches in the new tree for ScintHits
def create_branches_scint(output_tree, scint_copyNo, scint_layer, scint_row, scint_column, scint_nPE, scint_time):
    # Create the branch for the flattened data
    output_tree.Branch("chan", scint_copyNo)
    output_tree.Branch("layer", scint_layer)
    output_tree.Branch("row", scint_row)
    output_tree.Branch("column", scint_column)
    output_tree.Branch("nPE", scint_nPE)
    output_tree.Branch("time", scint_time)

# Function to create the branches in the new tree for PMTHits
def create_branches_pmt(output_tree, pmt_nPE, pmt_copyNo, pmt_time, pmt_layer, pmt_row, pmt_column, pmt_type):
    # Create the branch for the flattened data
    output_tree.Branch("pmt_nPE", pmt_nPE)
    output_tree.Branch("pmt_chan", pmt_copyNo)
    output_tree.Branch("pmt_time", pmt_time)
    output_tree.Branch("pmt_layer", pmt_layer)
    output_tree.Branch("pmt_row", pmt_row)
    output_tree.Branch("pmt_column", pmt_column)
    output_tree.Branch("pmt_type", pmt_type)

def create_branches_event(output_tree, eventID, runNumber):
    output_tree.Branch("eventID", eventID)
    output_tree.Branch("runNumber", runNumber)

def populate_vectors_event(input_tree, eventID, runNumber):
    eventID.clear()
    runNumber.clear()

    event = input_tree.ROOTEvent
    eventID.push_back(event.GetEventID())
    runNumber.push_back(1)

def populate_vectors_pmt(input_tree, pmt_nPE, pmt_copyNo, pmt_time, pmt_layer, pmt_row, pmt_column, pmt_type):
    pmt_nPE.clear()
    pmt_copyNo.clear()
    pmt_time.clear()
    pmt_layer.clear()
    pmt_row.clear()
    pmt_column.clear()
    pmt_type.clear()

    # make an array of length 1000 to store temporary nPE values
    temp_nPE = [0]*1000
    temp_time = [0]*1000


    # Populate the vectors with flattened data
    for j in range(input_tree.PMTHits.size()):
        hit = input_tree.PMTHits.at(j)

        simToDataScale = 0.682*0.5 #using average PMT scale factor to calibrate to data. this is a float, so can be recalibrated easily
        # we don't need to scale the NPE since PMTs are already scaled in the simulation
        temp_nPE[hit.GetPMTNumber()] = temp_nPE[hit.GetPMTNumber()] + simToDataScale
        # if the hit time in the channel is lower than the current time, replace the current time with the new time
        if(temp_time[hit.GetPMTNumber()] == 0 or hit.GetFirstHitTime() < temp_time[hit.GetPMTNumber()]):
            temp_time[hit.GetPMTNumber()] = hit.GetFirstHitTime()

    # loop over the temporary arrays and add the values to the vectors
    for j in range(500):
        # only save values where the nPE is greater than 0
        if(temp_nPE[j] > 0):
            pmt_nPE.push_back(temp_nPE[j])
            pmt_time.push_back(temp_time[j])
            dataChan = slabSimToDataPMT(j)
            pmt_copyNo.push_back(int(dataChan))
            pmt_layer.push_back(int(dataChan/12))
            pmt_row.push_back(int(dataChan%4)+1)
            pmt_column.push_back(int((dataChan%12)/4)+1)
            pmt_type.push_back(pmtPosition(j, int(dataChan/4) % 3, int(dataChan%4), int(dataChan/12)))
            
##################################################################################################################
# Main script
# Load the custom dictionary
if ROOT.gSystem.Load("/net/cms26/cms26r0/zheng/slabsim/withPhoton/milliQanSim/build/libMilliQanCore.so") < 0:
#if ROOT.gSystem.Load("libMilliQanCore.so") < 0:
    raise Exception("Failed to load custom dictionary.")

# Open the input ROOT file specified in the first argument
#filename = "/net/cms26/cms26r0/zheng/barSimulation/barWithPhotonUpdate/BARcosmic" + sys.argv[1] + "/MilliQan.root"
filename = sys.argv[1] + sys.argv[2] + "/MilliQan.root"
outname = "output_" + sys.argv[2] + ".root"
#filename = "MilliQan.root"
#outname = "MilliQan_flat.root"

input_file = ROOT.TFile(filename, "READ")

# Get the input ROOT tree
input_tree = input_file.Get("Events")

# Create an output ROOT file
# it should be FILENAME_flat.root
output_file = ROOT.TFile(outname, "RECREATE")
output_tree = ROOT.TTree("t", "A flat, lightweight tree for MilliQan sim data")

# Scint variables to hold flattened data
scint_copyNo = ROOT.std.vector('int')()
scint_layer = ROOT.std.vector('int')()
scint_row = ROOT.std.vector('int')()
scint_column = ROOT.std.vector('int')()
scint_nPE = ROOT.std.vector('float')()
scint_time = ROOT.std.vector('float')()

# PMT variables to hold flattened data
pmt_nPE = ROOT.std.vector('float')()
pmt_copyNo = ROOT.std.vector('int')()
pmt_time = ROOT.std.vector('float')()
pmt_layer = ROOT.std.vector('int')()
pmt_row = ROOT.std.vector('int')()
pmt_column = ROOT.std.vector('int')()
pmt_type = ROOT.std.vector('int')()

# Variables to hold event-level data
eventID = ROOT.std.vector('int')()
runNumber = ROOT.std.vector('int')()

# Create the branches in the new tree
# want only the following branches with the following names:
# event - Will be the same as the input tree eventID
# runNumber - Will use Run 1 for now

# nPE - Will be determined using a sum of scint_EDep and a scaling factor
# time - Will use a minimum time from scint_firstHitTime
# layer - will store layer number, 0-indexed, with end panels as -1 and 4
# chan - will store copyNo from Scint hit

# of these, event-level parameters are event and runNumber
# scintillator-level parameters are nPE, time, layer, chan

# Create the branches in the new tree
create_branches_event(output_tree, eventID, runNumber)
create_branches_scint(output_tree, scint_copyNo, scint_layer, scint_row, scint_column, scint_nPE, scint_time)
create_branches_pmt(output_tree, pmt_nPE, pmt_copyNo, pmt_time, pmt_layer, pmt_row, pmt_column, pmt_type)

# Loop over entries in the input tree
n_entries = input_tree.GetEntries()
for i in range(n_entries):
    input_tree.GetEntry(i)

    # Populate the vectors with flattened data
    populate_vectors_event(input_tree, eventID, runNumber)
    populate_vectors_scint(input_tree, scint_copyNo, scint_layer, scint_row, scint_column, scint_nPE, scint_time)
    populate_vectors_pmt(input_tree, pmt_nPE, pmt_copyNo, pmt_time, pmt_layer, pmt_row, pmt_column, pmt_type)
    
    # Fill the new tree with the flattened data
    output_tree.Fill()

# Write and close the output file
output_file.Write()
output_file.Close()

#print("Finished file " + sys.argv[2])
