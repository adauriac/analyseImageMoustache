LIBS=-lopencv_core -lopencv_highgui -lopencv_imgcodecs

OPENCV_LIBS = $(shell pkg-config --libs opencv4)

all: seuillage contour

contour:contour.cpp Makefile
	g++ contour.cpp -I /usr/include/opencv4/ $(OPENCV_LIBS) -o contour -g -Wall

seuillage:seuillage.cpp Makefile
	g++ seuillage.cpp -I /usr/include/opencv4/ $(LIBS) -o seuillage -g -Wall

voirCouleur:voirCouleur.cpp Makefile
	g++ voirCouleur.cpp -o voirCouleur -I /usr/include/opencv4/ $(OPENCV_LIBS) -g -Wall

test:test.cpp Makefile
	g++ test.cpp -o test -I /usr/include/opencv4/ $(OPENCV_LIBS) -g -Wall

findContoursTest:findContoursTest.cpp Makefile
	g++ findContoursTest.cpp -o findContoursTest -I /usr/include/opencv4/ $(OPENCV_LIBS) -g -Wall

trackBar:trackBar.cpp Makefile
	g++ trackBar.cpp -o trackBar -I /usr/include/opencv4/ $(OPENCV_LIBS) -g -Wall
