#!/bin/bash

# Count lines in all files in the current directory and subdirectories
total_lines=$(find . -type f -exec wc -l {} + | awk '{total += $1} END {print total}')

# Print the result
echo "Total lines in all files: $total_lines"

