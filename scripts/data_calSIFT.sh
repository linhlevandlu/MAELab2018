#!/bin/bash
# SCRIPT CALCULATE THE SIFT DESCRIPTOR OF A PATCH AROUND THE LANDMARK
# THE OUTPUT IS A MATRIX 9X8 FOR EACH LANDMARK ( DEFAULT SIZE 9X9 -> 9 X(3X3))
SCENEJPGFOLDER="/home/linh/Desktop/data/pronotum_data_5/data_aug/train_org/*"
SCENETPSFOLDER="/home/linh/Datasets/Morphometrics/pronotum/landmarks/*"
SAVEFOLDER="/home/linh/Desktop/data/pronotum_data_5/data_aug/train_blue/"
XRATIO=12.75
YRATIO=12.75
EXECUTE="./MAELab_CI"
jpgarray=(${SCENEJPGFOLDER})
tpsarray=(${SCENETPSFOLDER})
total=${#jpgarray[@]}
rpby="_b."
for (( i=0; i< $total; i++))
do
	SCENEJPG="${jpgarray[$i]}"
	#img=${SCENEJPG:${#imagepath}-10}
	#SAVEJPG="$SAVEFOLDER$img"
	#echo $SCENEJPG
	#echo "${jpgarray[$i]}"
	SCENETPS="${tpsarray[$i]}"	
	#tps=${SCENETPS:${#tpspath}-10}
	#SAVETPS="$SAVEFOLDER$tps"
	saveImg="$SAVEFOLDER$(basename "${jpgarray[$i]}"/)"
	saveImg2=${saveImg/./$rpby}
	echo $saveImg2
	$EXECUTE "$SCENEJPG" "$SCENETPS" "$PATCHSIZE" "0" "$saveImg2"
done

#EXECUTE="./MAELab_CI"
#$EXECUTE

