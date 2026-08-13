#!/bin/bash

# Base directories
G4output_dir="/net/cms26/cms26r0/dgimani/milliq_mcgen/G4_output"
output_script="resubmit_analysis_jobs.sh"
#output_script="test.sh"

# Set the date cutoff
date_cutoff="2025-02-17"

# Initialize the output script
echo "#!/bin/bash" > "$output_script"
chmod +x "$output_script"

# Debugging output
echo "Using date cutoff: $date_cutoff"

# Loop through the nested directories
for mass_dir in "$G4output_dir"/*; do
  if [ -d "$mass_dir" ]; then
    mass=$(basename "$mass_dir")

    for charge_dir in "$mass_dir"/*; do
      if [ -d "$charge_dir" ]; then
        charge=$(basename "$charge_dir")

        analysis_file="${charge_dir}/yield_50NPE_cap.txt"

        # Check if flattened_output.root is missing or outdated
        if [ ! -f "$analysis_file" ]; then
          echo "yield_50NPE_cap.txt is missing for mass: $mass, charge: $charge"
	  echo "bsub /net/cms17/cms17r0/schmitz/python310/bin/python3.10 simAnalysis.py "$charge_dir"/" >> "$output_script"
        else
          # Use find to compare the file modification date
          file_date=$(stat -c %y "$analysis_file" | cut -d' ' -f1)
          if [[ "$file_date" < "$date_cutoff" ]]; then
            echo "yield_50NPE_cap.txt is outdated for mass: $mass, charge: $charge (file date: $file_date)"
	    echo "bsub /net/cms17/cms17r0/schmitz/python310/bin/python3.10 simAnalysis.py "$charge_dir"/" >> "$output_script"
          fi
        fi
      fi
    done
  fi
done

echo "Job submission script written to $output_script"

