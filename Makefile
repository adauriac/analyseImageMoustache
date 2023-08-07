LIBS=-lopencv_core -lopencv_highgui -lopencv_imgcodecs

OPENCV_LIBS = $(shell pkg-config --libs opencv4)

contour:contour.cpp Makefile
	g++ contour.cpp -I /usr/include/opencv4/ $(OPENCV_LIBS) -o contour -g -Wall

seuillage:seuillage.cpp Makefile
	g++ seuillage.cpp -I /usr/include/opencv4/ $(LIBS) -o seuillage -g -Wall
