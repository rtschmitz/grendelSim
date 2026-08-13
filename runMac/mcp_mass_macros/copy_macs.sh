#!/bin/bash

# Define the file to copy
file_to_copy="../mcp_novis.mac"  

# Define the target base directory
target_base_dir=$1  # Replace with the full path to the nested directory structure

# Traverse the nested directory structure and copy the file to each subdirectory
for target_dir in $(find "$target_base_dir" -type d); do
  # Skip directories where you don't want to copy the file (optional)
  # if [[ "$target_dir" == *"skip_this_directory"* ]]; then
  #   continue
  # fi

  # Copy the file to the current directory
  echo "Copying $file_to_copy to $target_dir"
  cp "$file_to_copy" "$target_dir/"
done
