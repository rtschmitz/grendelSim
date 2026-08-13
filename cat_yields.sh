#!/bin/bash

# Base directories
G4output_dir="/net/cms26/cms26r0/dgimani/milliq_mcgen/G4_output"

masses=("m_0p1" "m_0p3" "m_0p35" "m_0p4" "m_0p5" "m_0p7" "m_0p9" "m_1p0" "m_1p3" "m_1p5" "m_1p7" "m_2p0" "m_2p3" "m_2p7" "m_3p0" "m_3p5" "m_3p7" "m_4p0" "m_4p5" "m_4p7" "m_5p0" "m_9p0" "m_30p0")
#masses=("m_1p0")

# Date filter
date_cutoff="2024-12-09"



# Loop over each subdirectory in the propagated directory
for mass_dir in "$G4output_dir"/*; do
  if [ -d "$mass_dir" ]; then
    # Extract the mass (subdirectory name)
    mass=$(basename "$mass_dir")
    #if [[ " ${masses[@]} " =~ " ${mass} " ]]; then
      # Loop over each charge subdirectory inside the mass directory
      for charge_dir in "$mass_dir"/*; do
        if [ -d "$charge_dir" ]; then
          charge=$(basename "$charge_dir")

	  #yield_file="${G4output_dir}/${mass}/${charge}/yield.txt"
	  #yield_file="${G4output_dir}/${mass}/${charge}/yield_50NPE_cap.txt"
	  yield_file="${G4output_dir}/${mass}/${charge}/yield_SR1.txt"
	  #eff_file="${G4output_dir}/${mass}/${charge}/efficiency.txt"
	
          #echo "Catting yields and efficiency for mass: $mass, charge: $charge"
	  #if [ -f "$yield_file" ] && [ "$(find "$yield_file" -newermt "$date_cutoff" 2>/dev/null)" ]; then
	  if [ ! -f "$yield_file" ]; then
	    continue
          else
          # Use find to compare the file modification date
          file_date=$(stat -c %y "$yield_file" | cut -d' ' -f1)
	  #echo "$file_date"
            if [[ "$file_date" > "$date_cutoff" ]]; then
		  echo "$file_date"
		  echo "$yield_file"
		  awk '{print $1, $2, $3}' "$yield_file" >> simYields_SR1.txt
            fi
          fi
	  #head -n 1 "$yield_file" >> simYieldsThreeLayers.txt
	  #head -n 1 "$eff_file" >> simEfficiencies.txt 
        fi
    done
    #fi
  fi
done

