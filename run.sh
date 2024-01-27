#!/bin/bash
echo "Compiling and running chapter_categorizer..."

# Run Makefile
make

# Check if the compilation was successful
if [ $? -ne 0 ]; then
    echo "Compilation failed."
    exit $?
fi

# Run the compiled program
./out/chapter_categorizer

echo "Test run completed."
