#!/bin/bash
# SCRIPT EXTRACT THE PATCH AROUND THE LANDMARK TO CALCULATE THE PROCRUSTES
# THE OUTPUT IS A MATRIX 7X7 FOR EACH LANDMARK
SCENEJPGFOLDER="/home/linhpc/data_CNN/linhlv/pronotum/v1/original/*"
SCENETPSFOLDER="/home/linhpc/data_CNN/linhlv/pronotum/v1/landmarks/*"
SAVEFOLDER="/home/linhpc/Results/2018/pronotum/procrustes/landmark_8/"
LMINDEX=7
BSIZE=7
EXECUTE="./MAELab_CI"
jpgarray=(${SCENEJPGFOLDER})
tpsarray=(${SCENETPSFOLDER})
total=${#jpgarray[@]}
rpby="_"
for (( i=0; i< $total; i++))
do
	SCENEJPG="${jpgarray[$i]}"
	SCENETPS="${tpsarray[$i]}"	
	echo $SCENETPS
	echo $SCENEJPG
	#saveTPS="$SAVEFOLDER$(basename "${tpsarray[$i]}"/)"
	#saveTPS2=${saveTPS/ /$rpby}
	$EXECUTE "$SCENEJPG" "$SCENETPS" "$LMINDEX" "$BSIZE" "$SAVEFOLDER"
done

#EXECUTE="./MAELab_CI"
#$EXECUTE

