#!/bin/bash
# Start all of the programs in the correct order

echo "Starting"
sleep 20
python3 lidar/segmentAlgoritm.py&
sleep 2
python camera_prog/camera.py&
sleep 2
python arc_ssh.py&
#sleep 3
echo "Good Morning"

# Make executable:
# chmod +x startup.sh
# Add to startup via crontab:
# (sudo) crontab -e

