#!/bin/bash

if [ "$#" -ne 1 ]; then 
	echo "Eroare argumente"
	exit 1
fi

input_char="$1"
valid_phrases=0

while IFS= read -r phrase; do
	if [[ $phrase =~ ^[A-Z] ]]; then
		if [[ $phrase =~ [a-zA-Z0-9\ \!\?\.\,]* ]]; then
			if [[ $phrase =~ [\!\?\.]$ ]]; then
				if ! [[ $phrase =~ (, [sS][iI] )|(,[sS][iI] )|(, [sS][iI])|(,[sS][iI])$ ]]; then
					if [[ $phrase = *"$input_char"* ]]; then
						((valid_phrases++))
					fi
				fi
			fi
		fi
	fi
done

echo "Numarul de propozitii validate este $valid_phrases"
