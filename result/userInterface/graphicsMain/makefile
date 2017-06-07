#//Last updated 17/06/07 by Christian Jonsson
commondir = ../common
incdir = ../include

Name = mainGraphics

all : $(Name)

$(Name) : graphics.cpp $(commondir)/*.c $(commondir)/*.cpp $(commondir)/Linux/*c 
	g++ graphics.cpp -std=c++14 -Wall -c -o $(Name).o -I$(incdir) -I$(commondir) -I$(commondir)/Linux -DGL_GLEXT_PROTOTYPES -fpermissive 
	g++ $(Name).o -std=c++14 $(commondir)/*.c $(commondir)/Linux/*.c $(incdir)/*.hpp -lGL -o $(Name) -I$(incdir) -I$(commondir) -I$(commondir)/Linux -DGL_GLEXT_PROTOTYPES  -lzmq -lpthread -lboost_system -lXt -lX11 -lm -fpermissive 


clean :

	rm $(Name).o
	rm $(Name)
	
