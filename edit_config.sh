#!/bin/bash

# Paths to the base directories for configs and macros
config_base_dir="/homes/dgimani/milliQanSim/build/config/mass_configs"  
#config_base_dir="/homes/dgimani/milliQanSim/build/config/prop_c_0p1"  
macro_base_dir="/homes/dgimani/milliQanSim/runMac/mcp_mass_macros"   
#macro_base_dir="/homes/dgimani/milliQanSim/runMac/mcp_mass_macros/c_0p1"   
input_base_dir="/homes/dgimani/milliQanSim/inputData/Run3_G4_mcp_inputs"
#input_base_dir="/homes/dgimani/milliQanSim/inputData/Run3_G4_mcp_inputs/c_0p1"

# Paths to the template files
particles_template="/homes/dgimani/milliQanSim/build/config/particles.ini" 
onepc_template="/homes/dgimani/milliQanSim/build/config/onepc.ini"  
macro_template="/homes/dgimani/milliQanSim/runMac/mcp_novis.mac"   

#masses=("m_2p0" "m_2p3" "m_2p5" "m_2p7" "m_3p0" "m_3p5" "m_3p7" "m_4p0" "m_4p5" "m_4p7" "m_5p0" "m_5p5" "m_6p0" "m_6p5" "m_7p0" "m_8p0" "m_9p0" "m_10p0" "m_12p0" "m_15p0" "m_17p0" "m_20p0" "m_25p0" "m_27p0" "m_33p0" "m_35p0" "m_40p0")
masses=("m_3p5" "m_3p7" "m_4p0" "m_4p5" "m_1p5" "m_1p7" "m_2p3" "m_2p7" "m_30p0")
#masses=("m_2p0")
#charges=("c_0p003" "c_0p004" "c_0p005" "c_0p006" "c_0p007" "c_0p008" "c_0p009" "c_0p01" "c_0p02" "c_0p023" "c_0p025" "c_0p027" "c_0p03" "c_0p033" "c_0p035" "c_0p037" "c_0p04" "c_0p05" "c_0p06" "c_0p07" "c_0p08" "c_0p09" "c_0p093"  "c_0p095"  "c_0p1" "c_0p2" "c_0p3")
#masses=("m_0p1" "m_0p3" "m_0p35" "m_0p4" "m_0p5" "m_0p7" "m_0p9" "m_1p0")
charges=("c_0p033" "c_0p035" "c_0p037")
#charges=("c_0p023")

# Loop through mass directories
#for mass_dir in "$config_base_dir"/m_30*; do
for mass in "${masses[@]}"; do
  mass_dir="${config_base_dir}/${mass}"
  #mkdir -p "$mass_dir"
  if [ -d "$mass_dir" ]; then
    # Extract the mass from the directory name
    mass=$(basename "$mass_dir" | sed 's/m_//')

    # Convert the mass format for naming
    mass_formatted=$(echo "$mass" | sed 's/\p/./')

    for charge_dir in "$mass_dir"/c_*; do
    #for charge in "${charges[@]}"; do
      #charge_dir="${mass_dir}/${charge}"
      #mkdir -p "$charge_dir"
      if [ -d "$charge_dir" ]; then
        # Extract the charge from the directory name
        charge=$(basename "$charge_dir" | sed 's/c_//')

        # Convert the charge format for naming
        charge_formatted=$(echo "$charge" | sed 's/\p/./')

        # Define paths for the new files
        particles_file="$charge_dir/particles_m_${mass}_c_${charge}.ini"
        onepc_file="$charge_dir/onepc_m_${mass}_c_${charge}.ini"
        macro_file="$macro_base_dir/m_${mass}/c_${charge}/mcp_novis_m_${mass}_c_${charge}.mac"

        # Ensure the macro directory exists
        mkdir -p "$(dirname "$macro_file")"

        # Copy template files to their respective directories
        cp "$particles_template" "$particles_file"
        cp "$onepc_template" "$onepc_file"
        cp "$macro_template" "$macro_file"

        # Edit the particles.ini file
        sed -i "s/^MonopoleMass = .*/MonopoleMass = ${mass_formatted}/" "$particles_file"
        sed -i "s/^ElectricCharge = .*/ElectricCharge = ${charge_formatted}/" "$particles_file"
        sed -i "s/^FileName = .*/FileName = mcp_m_${mass}_G4_input.txt/" "$particles_file"
        sed -i "s|^PathName = .*|PathName = ${input_base_dir}/m_${mass}/|" "$particles_file"
	
	# Edit the onepc.ini file
	sed -i "s|^ParticleConfigFile = .*|ParticleConfigFile = ${particles_file}|" "$onepc_file"

        # Edit the macro file
        output_path="/net/cms26/cms26r0/dgimani/milliq_mcgen/G4_output/m_${mass}/c_${charge}/"
        #output_path="/net/cms26/cms26r0/dgimani/milliq_mcgen/G4_output/c_0p1/m_${mass}/c_${charge}/"
        input_file="${input_base_dir}/m_${mass}/mcp_m_${mass}_G4_input.txt"

        # Create the output path directory if necessary
        mkdir -p "$output_path"

        sed -i "s|^/run/fname .*|/run/fname ${output_path}|" "$macro_file"

        # Count the number of lines in the input file
        if [[ -f "$input_file" ]]; then
          line_count=$(wc -l < "$input_file")
          sed -i "s|^/run/beamOn .*|/run/beamOn ${line_count}|" "$macro_file"
        else
          echo "Input file $input_file not found; skipping line count update."
        fi

        echo "Processed mass: ${mass}, charge: ${charge}"
      fi
    done
  fi
done

