#!/bin/bash
# SCRIPT CALCULATE THE SIFT DESCRIPTOR OF A PATCH AROUND THE LANDMARK
# THE OUTPUT IS A MATRIX 9X8 FOR EACH LANDMARK ( DEFAULT SIZE 9X9 -> 9 X(3X3))
<<<<<<< HEAD
SCENEJPGFOLDER="/home/linhpc/data_CNN/linhlv/tdata/i2448x2448/original/*"
SCENETPSFOLDER="/home/linhpc/data_CNN/linhlv/tdata/i2448x2448/landmarks_2448x2448/*"
SAVEFOLDER="/home/linhpc/data_CNN/linhlv/tdata/i102x102_pronotum/landmarks_i102x102/"
XRATIO=24
YRATIO=24
=======
SCENEJPGFOLDER="/home/vanlinh/data_CNN/i2448x2448/original/*"
SCENETPSFOLDER="/home/vanlinh/data_CNN/i2448x2448/landmarks_2448x2448/*"
SAVEFOLDER="/home/vanlinh/data_CNN/i96x96/landmarks/"
XRATIO=10
YRATIO=10
>>>>>>> 2b427de4420c8be2d8c47b73edbe445e7f0b9f5e
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

