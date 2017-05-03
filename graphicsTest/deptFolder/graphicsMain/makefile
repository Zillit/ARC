commondir = ../../common
srcdir = ../../src
incdir = ../../include

Name = mainGraphics

all : $(Name)

$(Name) : graphics.cpp $(commondir)/*.c $(commondir)/*.cpp $(commondir)/Linux/*c 
	g++ graphics.cpp -std=c++17 -Wall -c -o $(Name).o -I$(incdir) -I$(commondir) -I$(commondir)/Linux -DGL_GLEXT_PROTOTYPES -fpermissive 
	g++ $(Name).o -std=c++17 $(commondir)/*.c $(commondir)/Linux/*.c $(srcdir)/*.cpp $(incdir)/*.hpp -lGL -o $(Name) -I$(incdir) -I$(commondir) -I$(commondir)/Linux -DGL_GLEXT_PROTOTYPES  -lzmq -lpthread -lboost_system -lXt -lX11 -lm -fpermissive 


clean :

	rm $(Name).o
	rm $(Name)
	
