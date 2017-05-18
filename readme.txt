Setting up the development:
Clone Zillit/ARC repository from github
mkdir build #will generate the build folder
cd build/ #move to build folder
cmake .. #generate the make files for your system
make #generate the ARC program etc


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

Connect over ssh
To get to the server:  ssh -t -p 4444 arc@nhkim91.ddns.net

At the car input the command: ssh -N -R 2222:localhost:22 -p 4444 arc@nhkim91.ddns.net

At the user input the command: ssh -t -p 4444 arc@nhkim91.ddns.net ssh -p 2222 pi@localhost

User name (user): arc, password: stavarett
User name (car): pi, password: stavarett

To compile the graphicTest file compile through command while in the graphicTest/depthFolder/graphicsMain.
Write make in the terminal or simple write make and the makefile will compile it for you.
//gcc graphics.cpp -c -o mainGraphics.o -I../../common -I../../common/Linux -DGL_GLEXT_PROTOTYPES
//g++ mainGraphics.o ../../common/*.c ../../common/Linux/*.c ../../common/*.cpp -lGL -o mainGraphics -I../../common -I../../common/Linux -DGL_GLEXT_PROTOTYPES  -lXt -lX11 -lm -fpermissive
This will create a exicutable named mainGraphics that takes simple keyboard commands and display a world.

Communication should be established by watchdog or shell script on devices. If not make sure to run the following
python proxy.py on the proxy server.
python user_ssh.py on the user side.
python(3?) arc_ssh.py on the car. Pilot algoritm needs to run and communicate with arc_ssh.py.

Make sure the pi is connected to the server:
Install autossh, crontab.
Make sure to give the pi.sh file permission by chmod 755 pi.sh. 
Move the script into the bin folder so that it will be seen in path.
Run #crontab @reboot /home/user/bin/pi.sh