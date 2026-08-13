#####################################################################################################################################

########### A script to convert MilliQan simulation data to a flat, lightweight tree in the format of experimental data #############

###########                                         9/8/23, R.Schmitz                                                   #############

#####################################################################################################################################

###### Designed to be run over EDep-only data, minimal outputs ######

import array
import ROOT
import sys
import math
import re
import random


# Function to convert sim copy number to data copy number for PMTs
def simToDataPMT(simChannel):

    #first, if the hit is in a panel or slab, we manually map it
    if(simChannel == 77):
        return 68
    elif(simChannel == 78):
        return 70
    elif(simChannel == 79):
        return 69
    elif(simChannel == 81):
        return 72
    elif(simChannel == 82):
         return 74
    elif(simChannel == 83): 
        return 73                                                                                                                                                                                                                                                                 
    elif(simChannel == 97):
        return 71
    elif(simChannel == 96): 
        return 75
    
    #we save the layer number, then map the sim channel to the correct data number, then add the layer number back in
    #layerNumber = math.floor(simChannel / 216)
    layerNumber = simChannel // 216
    simChannel = simChannel % 216
    
    if simChannel <= 4:
        return (simChannel + 11)+layerNumber*16
    elif simChannel <= 12:
        return simChannel - 1 + layerNumber*16
    elif simChannel <= 16:
        return simChannel - 13 + layerNumber*16
    else:
        print("Error: simChannel out of range")
        return -1

# Function to convert sim copy number to data copy number for Scint
def simToDataScint(simChannel):

    #first, if the hit is in a panel or slab, we manually map it
    if(simChannel == 73):
        return 68
    elif(simChannel == 74):
        return 70
    elif(simChannel == 75):
        return 69
    elif(simChannel == 81):
        return 72
    elif(simChannel == 82):
         return 74
    elif(simChannel == 83): 
        return 73                                                                                                                                                                                                                                                                 
    elif(simChannel == 67):
        return 71
    elif(simChannel == 68): 
        return 75

    #we save the layer number, then map the sim channel to the correct data number, then add the layer number back in
    #layerNumber = math.floor(simChannel / 216)
    layerNumber = simChannel // 216
    simChannel = simChannel % 216

    if simChannel <= 4:
        return (simChannel + 11)+layerNumber*16
    elif simChannel <= 12:
        return simChannel - 1 + layerNumber*16
    elif simChannel <= 16:
        return simChannel - 13 + layerNumber*16
    else:
        print("Error: simChannel out of range")
        print(simChannel)
        print(layerNumber)
        return -1


cali =[9.7,8.4,5.0,3.6,8.2,8.2,6.8,4.6,8.7,7.1,6.4,7.5,8.7,11.8,9.7,8.1,8.6,9.1,6.4,7.5,9.8,8.8,11,6.6,8.6,11,7.3,8.3,2.6,6.2,7.0,7.2,8.7,7.7,6.8,7.8,8.8,8.5,3.5,8.2,7.6,6.8,9.5,8.5,6.3,6.7,7.6,7.9,9.0,8.3,8.3,6.9,6.0,8.0,5.5,5.5,6.6,8.1,8.4,8.7,9.2,7.6,5.9,8.2]


# Function to populate the vectors in the flattened tree for ScintHits
def populate_vectors_scint(input_tree, scint_copyNo, scint_layer, scint_nPE, scint_time,scint_row,scint_column,scint_type,scint_muonHit):
    # Clear the vectors
    scint_copyNo.clear()
    scint_layer.clear()
    scint_nPE.clear()
    scint_time.clear()
    scint_row.clear()
    scint_column.clear()
    scint_type.clear()
    scint_muonHit.clear()
    # make an array of length 1000 to store temporary nPE values
    temp_nPE = [0]*1000
    temp_time = [0]*1000
    #check if the current channel got hit by muons. 1 for get muon hit and 0 for not geting muon hit.
    Muon_Chan = [0]*1000

    # Populate the vectors with flattened data
    for j in range(input_tree.ScintRHits.size()):
        edep = 0.0
        hit = input_tree.ScintRHits.at(j)
        #make temp variable to hold copy number, then convert it to data copy number
        tempCopyNo = hit.GetCopyNo()
        #tempCopyNo = simToDataScint(tempCopyNo)
        # add the hit to the temporary array, using the energy deposition as the nPE and converting it using a measure scale factor
        # also multiply by 7.5/11, the mean ratio of the measured nPE to the simulated nPE for the same geometry
        # this way we have three different ways to scale the nPE according to the geometry and correcting for the difference between simulation and data
        # simToDataScale = 0.682*2671/2398 #using average calibration measured in data, plus a correction for EDep-to-NPE using simulated NPE amounts. Basically a pre-correction to the nPEPerMeV below, might need additional tuning
        
        simToDataScale = 0.682 #using average calibration measured in data, plus a correction for EDep-to-NPE using simulated NPE amounts. Basically a pre-correction to the nPEPerMeV below, might need additional tuning
        transferChan = simToDataScint(tempCopyNo)
        if transferChan <=63 :    
            simToDataScale = cali[transferChan]/11
        if(tempCopyNo == 67 or tempCopyNo == 68):
            nPEPerMeV = 110.2*simToDataScale #measured using bar cosmic dataset
        elif(tempCopyNo == 73 or tempCopyNo == 74 or tempCopyNo == 75 or tempCopyNo == 81 or tempCopyNo == 82 or tempCopyNo == 83):
            nPEPerMeV = 28.7*simToDataScale #measured using bar cosmic dataset
        else:
            nPEPerMeV = 331.2*simToDataScale #measured using bar cosmic dataset
        edep = hit.GetEDep()
        name = hit.GetParticleName()
        if(hit.GetEDep()>0 and name == 11):
            edep = edep-0.511
        if(hit.GetEDep()<0 and name == 11):
            edep = edep+0.511
        if(hit.GetEDep()<0 and name == -11):
            edep = edep-0.511
        if(hit.GetEDep()>0 and name == -11):
            edep = edep+0.511
        if(hit.GetEDep()>0 and name == 2112):
            edep = edep+4.946323
        if(hit.GetEDep()<0 and name == 2112):
            edep = edep-4.946323

        temp_nPE[tempCopyNo] = temp_nPE[tempCopyNo] + edep*nPEPerMeV


        #Get the particle(PDG number) that hit the specific channel
        PNum=hit.GetParticleName()
        if abs(PNum) == 13:
            Muon_Chan[tempCopyNo] = 1



        # if the hit time in the channel is lower than the current time, replace the current time with the new time
        if(temp_time[tempCopyNo] == 0 or  ((hit.GetHitTime() >0)  and  (hit.GetHitTime() < temp_time[tempCopyNo])) ):
            temp_time[tempCopyNo] = hit.GetHitTime()

    # loop over the temporary arrays and add the values to the vectors
    for j in range(1000):
        # only save values where the nPE is greater than 0t
        #if(temp_nPE[j] > 0):
        if(temp_nPE[j] != 0):
            scint_muonHit.push_back(Muon_Chan[j])
            scint_nPE.push_back(temp_nPE[j])
            scint_time.push_back(temp_time[j])
            dataChan = simToDataScint(j)
            scint_copyNo.push_back(int(dataChan))
            if(j == 67):
                scint_layer.push_back(-1)
                scint_type.push_back(int(1))
                scint_row.push_back(int(0))
                scint_column.push_back(int(0))
            elif(j == 68):
                scint_layer.push_back(4)
                scint_type.push_back(int(1))
                scint_row.push_back(int(0))
                scint_column.push_back(int(0))
            elif(j == 73 or j == 74 or j == 75):
                scint_layer.push_back(0)
                scint_type.push_back(int(2))
                if j == 73:
                    scint_row.push_back(int(4))
                    scint_column.push_back(int(0))
                elif j == 74:
                    scint_row.push_back(int(0))
                    scint_column.push_back(int(-1))
                else:
                    scint_row.push_back(int(0))
                    scint_column.push_back(int(4))
                    
                    
            elif(j == 81 or j == 82 or j ==  83):
                scint_layer.push_back(2)
                scint_type.push_back(int(2))
                if j == 81: 
                    scint_row.push_back(int(4))
                    scint_column.push_back(int(0))
                elif j == 82:
                    scint_row.push_back(int(0))
                    scint_column.push_back(int(-1))
                else:
                    scint_column.push_back(int(4)) 
                    scint_row.push_back(int(0))
            else:
                scint_layer.push_back(int(j/216))
                scint_type.push_back(int(0))
                layerNum = int(dataChan/16)
                #find the super module number(SMnum)
                SMnum=int((dataChan-16*(layerNum))/4)
                barNumber = (dataChan-16*(layerNum))-4*SMnum
                if SMnum > 1:
                    if int(barNumber/2) == 1:
                        scint_row.push_back(int(0))
                    else:
                        scint_row.push_back(int(1)) 
                if SMnum <=1 :
                    if int(barNumber/2) == 1:
                        scint_row.push_back(int(2))
                    else:
                        scint_row.push_back(int(3))
                
                
                if (SMnum % 2 == 0):
                    if barNumber % 2 == 0:
                        scint_column.push_back(int(0))
                    else:
                        scint_column.push_back(int(1))
                
                if (SMnum % 2 == 1):                   
                    if barNumber % 2 == 0:
                        scint_column.push_back(int(2))
                    else:
                        scint_column.push_back(int(3))

# Function to create the branches in the new tree for PMTHits
def create_branches_pmt(output_tree, pmt_nPE, pmt_copyNo, pmt_time, pmt_layer,pmt_row,pmt_column,pmt_type):
    # Create the branch for the flattened data
    output_tree.Branch("pmt_nPE", pmt_nPE)
    output_tree.Branch("pmt_chan", pmt_copyNo)
    output_tree.Branch("pmt_time", pmt_time)
    output_tree.Branch("pmt_layer", pmt_layer)
    output_tree.Branch("pmt_row", pmt_row)
    output_tree.Branch("pmt_column", pmt_column)
    output_tree.Branch("pmt_type",pmt_type)
               
# Function to create the branches in the new tree for ScintHits
def create_branches_scint(output_tree, scint_copyNo, scint_layer, scint_nPE, scint_time,scint_row,scint_column,scint_type,scint_muonHit):
    # Create the branch for the flattened data
    output_tree.Branch("chan", scint_copyNo)
    output_tree.Branch("layer", scint_layer)
    output_tree.Branch("nPE", scint_nPE)
    output_tree.Branch("time", scint_time)
    output_tree.Branch("row", scint_row)
    output_tree.Branch("column", scint_column)
    output_tree.Branch("type",scint_type)
    output_tree.Branch("muonHit",scint_muonHit)

def create_branches_event(output_tree, event, eventWeight, runNumber):
    output_tree.Branch("event", event,"event/I")
    output_tree.Branch("eventWeight", eventWeight,"eventWeight/D")
    output_tree.Branch("runNumber", runNumber,"runNumber/I")

def populate_vectors_event(input_tree, event, eventWeight, runNumber, fileNum):
    #eventID.clear()
    #runNumber.clear()

    evt = input_tree.ROOTEvent
    #eventID.push_back(event.GetEventID())
    event[0] = evt.GetEventID()
    eventWeight[0] = evt.GetEventWeight()
    #runNumber.push_back(1)
    runNumber[0] = fileNum

def populate_vectors_pmt(input_tree, pmt_nPE, pmt_copyNo, pmt_time, pmt_layer,pmt_row,pmt_column,pmt_type):

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
        
        simToDataScale = 0.682 #using average PMT scale factor to calibrate to data. this is a float, so can be recalibrated easily
        # we don't need to scale the NPE since PMTs are already scaled in the simulation
        transferChan = simToDataPMT(hit.GetPMTNumber())
        if transferChan <=63 :
            simToDataScale = cali[transferChan]/11

        if(random.random() < simToDataScale): #test against calibrated PMT quantum efficiency
            temp_nPE[hit.GetPMTNumber()] = temp_nPE[hit.GetPMTNumber()] + 1
            
            # if the hit time in the channel is lower than the current time, replace the current time with the new time
            if(temp_time[hit.GetPMTNumber()] == 0 or ((hit.GetFirstHitTime() >0) and (hit.GetFirstHitTime() < temp_time[hit.GetPMTNumber()])) ):
                temp_time[hit.GetPMTNumber()] = hit.GetFirstHitTime()

    # loop over the temporary arrays and add the values to the vectors
    for j in range(1000):
        # only save values where the nPE is greater than 0
        if(temp_nPE[j] > 0):
            pmt_nPE.push_back(temp_nPE[j])
            pmt_time.push_back(temp_time[j])
            dataChan = simToDataPMT(j)
            pmt_copyNo.push_back(int(dataChan))
            if(j == 97 or j == 99):
                pmt_layer.push_back(-1)
                pmt_type.push_back(int(1))
                pmt_row.push_back(int(0))
                pmt_column.push_back(int(0))
            elif(j == 96 or j == 98):
                pmt_layer.push_back(4)
                pmt_type.push_back(int(1))
                pmt_row.push_back(int(0))
                pmt_column.push_back(int(0))
            elif(j == 77 or j == 78 or j == 79):
                pmt_layer.push_back(0)
                pmt_type.push_back(int(2))
                if  j == 77: 
                    pmt_row.push_back(int(4))
                    pmt_column.push_back(int(0))
                elif j == 78:
                    pmt_row.push_back(int(0))
                    pmt_column.push_back(int(-1))
                else:
                    pmt_row.push_back(int(0))
                    pmt_column.push_back(int(4))
                
            elif(j == 81 or j == 82 or j == 83):
                pmt_layer.push_back(2)
                pmt_type.push_back(int(2))
                if j == 81: 
                    pmt_row.push_back(int(4))
                    pmt_column.push_back(int(0))
                elif j == 82:
                    pmt_row.push_back(int(0))
                    pmt_column.push_back(int(-1))
                else:
                    pmt_column.push_back(int(4)) 
                    pmt_row.push_back(int(0))


            else:
                pmt_layer.push_back(int(j/216))
                pmt_type.push_back(int(0))
                layerNum = int(dataChan/16)
                #find the super module number(SMnum)
                SMnum=int((dataChan-16*(layerNum))/4)
                barNumber = (dataChan-16*(layerNum))-4*SMnum
                if SMnum > 1:
                    if int(barNumber/2) == 1:
                        pmt_row.push_back(int(0))
                    else:
                        pmt_row.push_back(int(1)) 
                if SMnum <=1 :
                    if int(barNumber/2) == 1:
                        pmt_row.push_back(int(2))
                    else:
                        pmt_row.push_back(int(3))
                
                
                if (SMnum % 2 == 0):
                    if barNumber % 2 == 0:
                        pmt_column.push_back(int(0))
                    else:
                        pmt_column.push_back(int(1))
                
                if (SMnum % 2 == 1):                   
                    if barNumber % 2 == 0:
                        pmt_column.push_back(int(2))
                    else:
                        pmt_column.push_back(int(3))
            
##################################################################################################################
# Main script
# Load the custom dictionary
if ROOT.gSystem.Load("/homes/dgimani/milliQanSim/build/libMilliQanCore.so") < 0:
    raise Exception("Failed to load custom dictionary.")

# Open the input ROOT file specified in the first argument
#filename = "/net/cms26/cms26r0/zheng/barSimulation/barWithPhotonUpdate/BARcosmic" + sys.argv[1] + "/MilliQan.root"
filename = sys.argv[1] + sys.argv[2] + ".root" # should be file location + file name
outname =  sys.argv[1] + "flattened_output.root"


#get the file number
#in beam muon sim, since the each file come with unique setting, there is no need to use filenumber to identify the file like I did in cosmic background sim
#so I set the filenumber to 0. 


try:
    fileNumber = int(sys.argv[2])  #E.g. cosmic file
except ValueError:
    fileNumber = int(0)  #handle the case of file name is string. E.g. beam file




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
scint_nPE = ROOT.std.vector('float')()
scint_time = ROOT.std.vector('float')()
scint_type = ROOT.std.vector('int')()
scint_row = ROOT.std.vector('int')()
scint_column = ROOT.std.vector('int')()
scint_muonHit = ROOT.std.vector('int')()

# PMT variables to hold flattened data
pmt_nPE = ROOT.std.vector('float')()
pmt_copyNo = ROOT.std.vector('int')()
pmt_time = ROOT.std.vector('float')()
pmt_layer = ROOT.std.vector('int')()
pmt_type = ROOT.std.vector('int')()
pmt_row = ROOT.std.vector('int')()
pmt_column = ROOT.std.vector('int')()

# Variables to hold event-level data

#eventID = ROOT.std.vector('int')()
#runNumber = ROOT.std.vector('int')()

event = array.array('i', [0])  # 'i' is for integer type
eventWeight = array.array('d', [0])  # 'd' is for double type
runNumber = array.array('i', [0])


# Create the branches in the new tree
# want only the following branches with the following names:
# event - Will be the same as the input tree eventID
# eventWeight - Will be the same as the input tree weight
# runNumber - Will use Run 1 for now

# nPE - Will be determined using a sum of scint_EDep and a scaling factor
# time - Will use a minimum time from scint_firstHitTime
# layer - will store layer number, 0-indexed, with end panels as -1 and 4
# chan - will store copyNo from Scint hit

# of these, event-level parameters are event and runNumber
# scintillator-level parameters are nPE, time, layer, chan

# Create the branches in the new tree
#create_branches_event(output_tree, eventID, runNumber)
create_branches_event(output_tree, event, eventWeight, runNumber)
create_branches_pmt(output_tree, pmt_nPE, pmt_copyNo, pmt_time, pmt_layer,pmt_row,pmt_column,pmt_type)
create_branches_scint(output_tree, scint_copyNo, scint_layer, scint_nPE, scint_time,scint_row,scint_column,scint_type,scint_muonHit)

# Loop over entries in the input tree
n_entries = input_tree.GetEntries()
for i in range(n_entries):
    input_tree.GetEntry(i)

    # Populate the vectors with flattened data
    populate_vectors_event(input_tree, event, eventWeight, runNumber,fileNumber)
    populate_vectors_pmt(input_tree, pmt_nPE, pmt_copyNo, pmt_time, pmt_layer,pmt_row,pmt_column,pmt_type)
    populate_vectors_scint(input_tree, scint_copyNo, scint_layer, scint_nPE, scint_time,scint_row,scint_column,scint_type,scint_muonHit)
    
    # Fill the new tree with the flattened data
    output_tree.Fill()

# Write and close the output file
output_file.Write()
output_file.Close()

print("Finished file " + sys.argv[2])
