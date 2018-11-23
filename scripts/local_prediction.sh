# Local prediction
# path in laptop
#SCENEJPGFOLDER="/home/linhpc/Data/images/*"
#SCENETPSFOLDER="/home/linhpc/Data/manual_landmarks/*"
# path in labo
SCENEJPGFOLDER="/home/linhpc/Biogical_Images/mandibule-gauche/Images_without_grid_2/*"
SCENETPSFOLDER="/home/linhpc/Biogical_Images/mandibule-gauche/predicted_landmarks_fine_tuning/*"

EXECUTE="./MAELab_CI"
jpgarray=(${SCENEJPGFOLDER})
tpsarray=(${SCENETPSFOLDER})
total=${#jpgarray[@]}
rpby="_"
for (( i=0; i< $total; i++))
do
	SCENEJPG="${jpgarray[$i]}"
	SCENETPS="${tpsarray[$i]}"
    #echo $SCENEJPG
    #echo $SCENETPS	
	$EXECUTE "$SCENEJPG" "$SCENETPS" 
done

#EXECUTE="./MAELab_CI"
#$EXECUTE

