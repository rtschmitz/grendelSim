Make flat, high-level ntuples from the geant output to facilitate analysis.

To compile, just run `make -j4`. There were some difficulties in getting `make` to play
nice with the ROOT dictionary generation, so it's possible that this fails the first
time. Just try running `make` again, and it should work. If you still get errors,
contact Bennett.

Once compiled, run it over a geant output file with
```
./make_ntuple <outdir> <fileID> <geant output file>
```
where `outdir` is the dirctory to store the output, and
`fileID` is an integer to ID the file (for bookkeeping, just use 1 if you don't care).

**Output tree branches**
* `fileID` - copy of the fileID argument above
* `orig_evt` - the entry number of the event in the original geant output file
* `scale1fb` - the per-event weight for 1 ifb. Only meaningful if this was set properly in the geant output.
* `procID` - integer specifying the production mode (for muons, 1=QCD(bc), 2=W, 3=DY, 4=QCD(nonbc). 
For mCPs, see the list [here](https://github.com/bjmarsh/milliq_mcgen/tree/master/decayMCP)). 
Again only meaningful if set properly in the geant output.
* `mu_{E,x,y}0` - the initial energy (in GeV) and transverse position (in m) of the muon/mCP
* `chan_nPE[32]` - integer number of true PE in each channel
* `chan_fracMuon[32]` - fraction of PE coming directly from muon/mCP
* `chan_fracEl{Scint,Rock,Al,Pb}[32]` - fraction of PE coming from electrons produced in scintillator, rock, aluminum, lead
* `chan_fracOther[32]` - fraction of PE coming from things not listed above
* `chan_firstTime[32]` - the time of the first PE in each channel, in ns
* `chan_meanTime[32]` - the mean PE time in each channel, in ns
* `chan_timeCalib[32]` - the time calibration used for each channel. The time branches listed above give calibrated time, so that hits consisistent with a relativistic particle from the beam have the same time in all channels. To get the raw, uncalibrated time, add this number to each channel's time.
* `chan_muDist[32]` - the distance the muon/mCP traverses in the scintillator for each channel, in cm. Set to -1 if the muon/mCP does not hit
* `chan_muTime[32]` - the time the muon/mCP enters the scintillator. Set to -1 if the muon/mCP does not hit
* `chan_firstPEPathLength[32]` - the total path length to get to the first PE in the channel, in cm (e.g. if there is a gamma produced in the aluminum, which then bounces from the cavern wall and then hits a scintillator, the path length will be long)
* `chan_type[32]` - 0 for slabs, 1 for bars, 2 for panels
* `mcTruth_nMuSlabHits` - number of slabs hit by the muon/mCP, at truth level
* `mcTruth_nMuBarHits` - number of bars hit by the muon/mCP, at truth level
* `mcTruth_nMuPanelHits` - number of panels hit by the muon/mCP, at truth level
* `mcTruth_fourSlab` - boolean for whether all four slabs are hit by muon/mCP
* `mcTruth_threeBarLine` - boolean for whether 3 bars in a straight line are hit by the muon/mCP
* `mcTruth_noBar` - true if no bars are hit by muon/mCP
* `mcTruth_noChan` - true if no scintillators of any type are hit by muon/mCP
* `mcTruth_verticalCosmic` - nonzero if a panel and three bars in a vertical line in a single layer are hit by muon/mCP. The value represents which path it hits (1=layer 1 left, 2=layer 1 right, 3=layer 2 left, etc)
* `chanX_PEtimes[160]` - one branch for each of the 32 channels. 
Gives the number of PE in each of 160 0.625 ns time bins. 
You can use this to plot the time profile of PE in a give channel with something like 
`Events->Draw("Iteration$>>(160,0,160)","(Entry$==12)*(chan8_PEtimes)","HIST")`
