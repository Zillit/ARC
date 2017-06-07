//Last updated 17/06/07 by Christian Jonsson

Setting up the development:


Required libraries OpenGL
cmake
freeglut3
freeglut3-dev
libglew1.5
libglew1.5-dev
libglu1-mesa
libglu1-mesa-dev
libgl1-mesa-glx
libgl1-mesa-dev
libxmu-dev 
libxi-dev
libboost-system
libboost-dev
libboost
linzmq

Connect over ssh
To get to the server:  ssh -t -p 4444 arc@nhkim91.ddns.net

At the car input the command: ssh -N -R 2222:localhost:22 -p 4444 pi@nhkim91.ddns.net

At the user input the command: ssh -t -p 4444 pi@nhkim91.ddns.net ssh -p 2222 pi@localhost

User name (user): arc, password: ""
User name (car): pi, password: ""

To compile the graphicTest file compile through command while in the result/userInterface/graphicsMain.
Write make in the terminal or simple write make and the makefile will compile it for you.
This will create a exicutable named mainGraphics that takes simple keyboard commands and display a world.

Communication should be established by shell script on devices. If not make sure to run the following
python proxy.py on the proxy server.
python user_ssh.py on the user side.
start2.sh on car

Make sure the pi is connected to the server:
Install autossh, crontab.
Make sure to give the pi.sh file permission by chmod 755 pi.sh. 
Move the script into the bin folder so that it will be seen in path.
Run #crontab @reboot /home/user/bin/pi.sh