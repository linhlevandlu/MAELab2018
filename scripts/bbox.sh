#!/bin/bash
# Fill hole in binary images
#path:  /home/linh/Datasets/resize_256x192/tete/
FOLDER="/home/linh/Datasets/Morphometrics/pronotum/Images_without_grid_2/*"
#FOLDER="/home/linh/Datasets/Morphometrics/tete/tete/*"
#LM_FOLDER="/home/linh/Datasets/Morphometrics/pronotum/landmarks/*"
SAVEFOLDER="/home/linh/Desktop/results/2017/pronotum/bbox"
JPGARRAY=(${FOLDER})
#LMARRAY=(${LM_FOLDER})
TOTAL=${#JPGARRAY[@]}
EXECUTE="./MAELab_CI"
for((i=0; i<$TOTAL; i++))
do
	IMAGE="${JPGARRAY[$i]}"
	echo $IMAGE
	#LM="${LMARRAY[$i]}"
	#echo $LM
	#SAVEIMAGE="$SAVEFOLDER$(basename "${JPGARRAY[$i]}")"
	#echo $SAVEIMAGE
	$EXECUTE "$IMAGE" "$SAVEFOLDER"
done
