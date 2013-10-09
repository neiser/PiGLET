
CC=g++

CFLAGS=-c -Wall -O3 -I/usr/local/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include -I/opt/vc/include/interface/vmcs_host/linux/

LDFLAGS=-L/opt/vc/lib -lEGL -lGLESv2 -lbcm_host -lm

SOURCES=BlockBuffer.cpp Interval.cpp main.cpp EGLWindow.cpp MyGLWindow.cpp EGLConfig.cpp

OBJECTS=$(SOURCES:%.cpp=%.o)

EXECUTABLE=PiGLPlot

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean :
	rm -f *.o $(EXECUTABLE)
