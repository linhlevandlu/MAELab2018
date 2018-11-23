#!/bin/bash
# SCRIPT CALCULATE THE SIFT DESCRIPTOR OF A PATCH AROUND THE LANDMARK
# THE OUTPUT IS A MATRIX 9X8 FOR EACH LANDMARK ( DEFAULT SIZE 9X9 -> 9 X(3X3))
SCENEJPGFOLDER="/home/linh/Datasets/Morphometrics/pronotum/Images_without_grid_2/*"
SCENETPSFOLDER="/home/linh/Datasets/Morphometrics/pronotum/landmarks/*"
SAVEFOLDER="/home/linh/Desktop/results/2017/pronotum/SIFT/10_juilliet/org/"
PATCHSIZE="9"
EXECUTE="./MAELab_CI"
jpgarray=(${SCENEJPGFOLDER})
tpsarray=(${SCENETPSFOLDER})
total=${#jpgarray[@]}
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
	#echo ${tpsarray[$i]}
	$EXECUTE "$SCENEJPG" "$SCENETPS" "$PATCHSIZE" "0" "$SAVEFOLDER"
done

#EXECUTE="./MAELab_CI"
#$EXECUTE

