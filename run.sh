#!/bin/bash
echo "Compiling and running chapter_categorizer..."

# Run Makefile
make

# Check if the compilation was successful
if [ $? -ne 0 ]; then
    echo "Compilation failed."
    exit $?
fi

# Run the compiled program and save the output to a file
./out/chapter_categorizer > our_output.txt

echo "Test run completed. Output saved to our_output.txt."
echo "Comparing the output files...."

# Compare two files and calculate the percentage of similarity
similarity_percentage=$(diff -y --suppress-common-lines our_output.txt output.txt | wc -l)
total_lines=$(cat output.txt | wc -l)
similarity_percentage=$(( (total_lines - similarity_percentage) * 100 / total_lines ))

echo "Similarity percentage: $similarity_percentage%"

