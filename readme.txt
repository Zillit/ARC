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

Connect over ssh
At the car input the command: ssh -N -R 2222:localhost:22 -p 4444 arc@nhkim91.ddns.net

At the user input the command: ssh -t -p 4444 arc@nhikim91.ddns.net ssh -p 2222 pi@localhost

User name (user): arc, password: stavarett
User name (car): pi, password: stavarett

To compile the graphicTest file compile through command while in the graphicTest/depthFolder/Perspective bunny 2.
gcc perspectivebunny.c ../../common/*.c ../../common/Linux/*.c -lGL -o test -I../../common -I../../common/Linux -DGL_GLEXT_PROTOTYPES  -lXt -lX11 -lm
This will create a exicutable named test that takes simple keyboard commands. In the perspectivebunny.c you can change said commands and play around.