#!/bin/bash
# SCRIPT segmentation the image and overlap on original image
#
SCENEJPGFOLDER="/home/linhpc/data_CNN/linhlv/pronotum/v1/original/*"
SAVEFOLDER="/home/linhpc/Results/2018/pronotum/procrustes/segmentation/"

EXECUTE="./MAELab_CI"
jpgarray=(${SCENEJPGFOLDER})
total=${#jpgarray[@]}
rpby="_"
for (( i=0; i< $total; i++))
do
	SCENEJPG="${jpgarray[$i]}"
	echo $SCENEJPG
	saveIMG="$SAVEFOLDER$(basename "${jpgarray[$i]}")"
    echo $saveIMG
	#saveTPS2=${saveTPS/ /$rpby}
	$EXECUTE "$SCENEJPG" "$saveIMG"
done

#EXECUTE="./MAELab_CI"
#$EXECUTE

