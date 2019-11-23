#!/bin/bash
echo "Cleaning Script starts now! Erasing previous results!"

# Get actual directory
actual_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"


rm -f ${actual_dir}/Output/General/*
rm -f ${actual_dir}/Output/Region/*

rm ${actual_dir}/result.dat
rm ${actual_dir}/output.csv


echo "Done Cleaning previous results!"
echo "Please execute genesys2 now!"


