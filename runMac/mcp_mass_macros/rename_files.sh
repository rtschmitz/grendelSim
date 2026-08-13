#!/bin/bash

# Base directory containing the nested structure
base_dir=$1

# Traverse the nested directory structure
for mass_dir in "$base_dir"/m_*; do
  if [ -d "$mass_dir" ]; then
    # Extract the mass from the directory name
    mass=$(basename "$mass_dir")

    for charge_dir in "$mass_dir"/c_*; do
      if [ -d "$charge_dir" ]; then
        # Extract the charge from the directory name
        charge=$(basename "$charge_dir")

        # Full path to the file that needs renaming
        file_path="$charge_dir/mcp_novis.mac"

        # Check if the file exists
        if [ -f "$file_path" ]; then
          # Construct the new file name
          new_file_name="mcp_${mass}_${charge}.mac"
          new_file_path="$charge_dir/$new_file_name"

          # Rename the file
          echo "Renaming $file_path to $new_file_path"
          mv "$file_path" "$new_file_path"
        fi
      fi
    done
  fi
done

