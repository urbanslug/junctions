"""
Concatenate all the benchmark results files into a single file
"""

import os
import re

def extract_samples(filename):
    # Define the pattern to capture sample names
    pattern = re.compile(r'gb_(.+?)\.msa_vs_(.+?)\.msa\.txt')

    # Use the pattern to find matches in the filename
    match = pattern.search(filename)

    if match:
        # Extract sample names from the matched groups
        sample1 = match.group(1)
        sample2 = match.group(2) if match.group(2) else sample1  # If no second sample, use the first one

        return sample1, sample2

    return None

# Directory containing files
dir_path = "./out"
output_file = os.path.join(dir_path, "./sim/accumulated_results.txt") 
header = ""
x = True

# Process all files starting with g_
for filename in os.listdir(dir_path):
    if filename.startswith("gb_"):
        # Full path to the file
        file_path = os.path.join(dir_path, filename)

        # Extract samples
        samples = extract_samples(filename)
        # uncomment the following line to see the samples
        #print(f"Filename: {filename}, Samples: {samples}")

        # Read the last line of the file
        with open(file_path, 'r') as file:
            lines = file.readlines()
            last_line = lines[-1].strip()
            if not header:
                header = "Sample_1\tSample_2\t" + lines[0].strip()

        # Accumulate the last line into the output file
        with open(output_file, 'a') as output:
            if x:
                output.write(f"{header}\n")
                x = False
            output.write(f"{samples[0]}\t{samples[1]}\t{last_line}\n")
