#!/bin/sh

echo "Starting"
sleep 10
python3 ~/ARC/ARC/lidar/segmentAlgorithm.py&
sleep 5
python ~/ARC/ARC/camera_prog/camera.py&
sleep 5
python ~/ARC/ARC/arc_ssh.py
#sleep 5
#echo "Good Morning"