#!/bin/bash
echo "Script starts now!"

# Get actual directory
actual_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

#Read folder structure from Folders.txt
echo "reading Folders_Linux.txt"

let i=0
declare -a array
while read line_data 
do
	array[i++]="${actual_dir}/${line_data}/" 
done < Folders_Linux.txt

#Create folder structure if not already existant
echo "creating folder structure"
for ((i=0; i < ${#array[*]}; i++ ))
do
	if [[ ! -e "${array[i]}" ]]; then
		mkdir "${array[i]}"
		echo "creating folder ${array[i]}"
	fi
done

#TODO Add command call for genesys_2.exe

echo "Done"
echo "Please execute genesys2 now!"


