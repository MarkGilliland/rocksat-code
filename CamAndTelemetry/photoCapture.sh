sudo apt install fswebcam	# Need to install fswebcam package

sudo usermod -a -G video <username> 	# Add username here to allow file permission



mkdir camPhotos 		# Execute this outside of the Bash Script to make a directory

chmod 777 camPhotos.sh 	# Make camPhotos executable



bash /home/pi/camPhotos.sh			# Run Script every time RPi restarts


#********************************** Bash Script ****************************************

#!/bin/bash

sleep 30

# Run five pictures at lower resolution to be sent wirelessly

count=”0”

while [ $count -lt 05 ];do

sleep 02 

# Camera supports 3456x1728 resolution or 6912x3456

fswebcam -r 3456x1728 –no-banner /home/pi/camPhotos/minesRocksatImage$count.jpg	 

count=$[$count+1]

done


# Run an additional 54 pictures to be taken at the higher offered resolution

count=”6”

while [$count -lt 60]; do

sleep 02 

# Camera supports 3456x1728 resolution or 6912x3456

fswebcam -r 6912x3456 –no-banner /home/pi/camPhotos/minesRocksatImage$count.jpg 	 

count = $[$count+1]

done
