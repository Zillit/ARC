#!/bin/bash
#Make sure the pi is connected to the server. Make sure to give the file permission by chmod 755 pi.sh. Move the script into the bin folder so that it will be seen in path.
autossh -N -R 2222:localhost:22 -p 4444 arc@nhkim91.ddns.net && sleep 1 && stavarett && sleep 1 && stavarett
