#!/bin/bash

function language_to_code {
	if [ "$1" == "catalan" ]; then
		echo "ca_ES"
	
	elif [ "$1" == "spanish" ]; then
		echo "es_ES"
	
	elif [ "$1" == "english" ]; then
		echo "en_GB"
	fi
}

language=$2
echo "Language '$language'"
code=$(language_to_code $language)
echo "Code: '$code'"

if [ "$1" == "update" ]; then
	
	lupdate -recursive . -ts src/qr-encoder_"$code".ts
	
elif [ "$1" == "translate" ]; then
	
	linguist src/qr-encoder_"$code".ts
	
elif [ "$1" == "compile" ]; then

	lrelease src/qr-encoder_"$code".ts
fi
