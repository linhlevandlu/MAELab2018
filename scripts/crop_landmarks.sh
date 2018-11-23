#!/bin/bash
# SCRIPT CALCULATE THE SIFT DESCRIPTOR OF A PATCH AROUND THE LANDMARK
# THE OUTPUT IS A MATRIX 9X8 FOR EACH LANDMARK ( DEFAULT SIZE 9X9 -> 9 X(3X3))
SCENEJPGFOLDER="/home/linhpc/data_CNN/linhlv/pronotum/v1/original/*"
SCENETPSFOLDER="/home/linhpc/data_CNN/linhlv/pronotum/v1/landmarks/*"
SAVEFOLDER="/home/linhpc/data_CNN/linhlv/tdata/i192x192/landmarks/"
EXECUTE="./MAELab_CI"
jpgarray=(${SCENEJPGFOLDER})
tpsarray=(${SCENETPSFOLDER})
total=${#jpgarray[@]}
rpby="_"
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
	echo $SCENETPS
	saveTPS="$SAVEFOLDER$(basename "${tpsarray[$i]}"/)"
	saveTPS2=${saveTPS/ /$rpby}
	$EXECUTE "$SCENEJPG" "$SCENETPS" "$PATCHSIZE" "0" "$saveTPS2"
done

#EXECUTE="./MAELab_CI"
#$EXECUTE

