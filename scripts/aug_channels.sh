#!/bin/bash
<<<<<<< HEAD
SCENEJPGFOLDER="/home/linhpc/data_CNN/linhlv/tdata/i102x102_pronotum/original/*"
SAVEFOLDER="/home/linhpc/data_CNN/linhlv/tdata/i102x102_pronotum/add_green/"
=======
SCENEJPGFOLDER="/home/vanlinh/data_CNN/i96x96/original/*"
SAVEFOLDER="/home/vanlinh/data_CNN/i96x96/add_green/"
>>>>>>> 2b427de4420c8be2d8c47b73edbe445e7f0b9f5e
EXECUTE="./MAELab_CI"
jpgarray=(${SCENEJPGFOLDER})
total=${#jpgarray[@]}
rpby="_g."
for (( i=0; i< $total; i++))
do
	SCENEJPG="${jpgarray[$i]}"
	saveImg="$SAVEFOLDER$(basename "${jpgarray[$i]}"/)"
	saveImg2=${saveImg/./$rpby}
	echo $saveImg2
	$EXECUTE "$SCENEJPG" "$saveImg2"
done
