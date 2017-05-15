#!/bin/bash
# Start all of the programs in the correct order

#sleep 3
python3 lidar/segmentAlgoritm.py
#sleep 3
python arc_ssh.py
#sleep 3

# Make executable:
# chmod +x startup.sh
# Add to startup via crontab:
# (sudo) crontab -e
# @reboot /home/pi/ARC/ARC/startup.sh