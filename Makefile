NAME = kinect
CC = g++
CFLAGS = -I/opt/local/include/ -I/usr/local/include/libfreenect/
LFLAGS = -L/usr/lib -L/opt/local/lib/

LIBS = -lopencv_core -lopencv_highgui -lopencv_objdetect -lopencv_imgproc
LIBS += -lfreenect -lfreenect_sync

all: kinect

kinect:
	$(CC) $(CFLAGS) $(LFLAGS) $(LIBS) $(NAME).cpp -o $(NAME)

clean:
	rm -rf *.o
	rm -rf $(NAME)
