#!/bin/bash
MODELJPG="/home/linh/Desktop/editedImages/mg_images/Mg 045.JPG"
MODELTPS="/home/linh/Desktop/editedImages/mg_landmarks/Mg 045.TPS"
SCENEJPGFOLDER="/home/linh/Desktop/editedImages/mg_images/*"
SCENETPSFOLDER="/home/linh/Desktop/editedImages/mg_landmarks/*"
#METHOD: 1 for template maching, 2 for SIFT
METHOD="2"
SAVEFOLDER="/home/linh/Desktop/results/2017/mg/31mars/"
FIRSTSIZE="9"
SECONDSIZE="36"
EXECUTE="./MAELab_CI"
#model : md 54.jpg, mg 45.jpg
#/home/linh/Desktop/rsImages/*
#/home/linh/Desktop/editedImages/md_images/size1/*
#TOTAL= ls -lR $SCENEJPGFOLDER | wc -l
#echo $TOTAL
#for i in $SCENEJPGFOLDER; do
#	echo item: $img
#done
#files=$( ls $SCENEJPGFOLDER )
#for ((i=227;i<${TOTAL};i++));
#do
#	echo ${SCENEJPGFOLDER[$i]}
#done
jpgarray=(${SCENEJPGFOLDER})
tpsarray=(${SCENETPSFOLDER})
total=${#jpgarray[@]}
for (( i=0; i< $total; i++))
do
	SCENEJPG="${jpgarray[$i]}"
	img=${SCENEJPG:${#imagepath}-10}
	SAVEJPG="$SAVEFOLDER$img"
	echo $SCENEJPG
	#echo "${jpgarray[$i]}"
	SCENETPS="${tpsarray[$i]}"	
	tps=${SCENETPS:${#tpspath}-10}
	SAVETPS="$SAVEFOLDER$tps"
	#echo ${tpsarray[$i]}
	$EXECUTE "$MODELJPG" "$MODELTPS" "$SCENEJPG" "$SCENETPS" "$METHOD" "$FIRSTSIZE" "$SECONDSIZE" "$SAVEJPG" "$SAVETPS"
done

#EXECUTE="./MAELab_CI"
#$EXECUTE

