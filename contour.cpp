#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;

#define B 0
#define G 1
#define R 2
/*
  use data[i+B] for the blue canal, data[i+G] for green, and data[i+R] for red
  pour acceder a l'element (l,c) 0<=l<lines; 0<=c<rows
  data[l*3*cols + 3*c + R] = 255;
  data[l*3*cols + 3*c + G] = 255;
  data[l*3*cols + 3*c + B] = 255;
*/
uchar *dataGlobal = NULL;
uint rowsGlobal=0,colsGlobal = 0;
std::vector<std::vector<Point>> contoursGlobal;

int isRed(uchar *data,uint ind)
{
  static int first =1;
  if (first)
    {
      first = 0;
      
    }
  return 1;
}     // FIN int isRed(uchar *data,uint ind)
// ***************************************************************************************

int isRed0(uchar *data,uint ind)
{
  // return (data[ind+R]>127) && (data[ind+B]<100) && (data[ind+G]<100); trop severe
  // return (data[ind+R]>127) ; on voit du gris 
  if (data[ind+R]<150)
    return 0; 
  if (data[ind+G]>140)
    return 0;
  if (data[ind+B]>140)
    return 0;
  return 1;
}     // FIN int isRed0(uchar *data,uint ind)
// ***************************************************************************************

int isBlack(uchar *data,uint ind)
{
  return (data[ind+R]<100) && (data[ind+B]<100) && (data[ind+G]<100);
}     // FIN int isBlack(uchar *data,uint ind)
// ***************************************************************************************

void createAlphaImage(const Mat& mat, Mat_<Vec4b>& dst)
{
  std::vector<Mat> matChannels;
  split(mat, matChannels);
  
  // create alpha channel
  Mat alpha = matChannels.at(0) + matChannels.at(1) + matChannels.at(2);
  matChannels.push_back(alpha);

  merge(matChannels, dst);
}     // FIN int void createAlphaImage(const Mat& mat, cv::Mat_<Vec4b>& dst)
// ***************************************************************************************

void trackBarCallBack(int event, void* userdata)
{
  printf("param= %d\n",event);
}     // FIN void trackBarCallBack(int event void* userdata)
// ***************************************************************************************

int analyseContour(std::vector<Point> &contour)
// retourne 1 si mertote d'etre analyser
{
  static int cpt = 0;
  printf("%d longeur= %lu (row=%d,cols=%d) ",cpt++,contour.size(),rowsGlobal,colsGlobal);
  double xa=0,ya=0,xi=rowsGlobal,yi=rowsGlobal;
  for(uint i=0;i<contour.size();i++)
    {
      uint x = contour[i].x;
      uint y = contour[i].y;
      if (x<xi)
	xi=x;
      if (x>xa)
	xa=x;
      if (y<yi)
	yi=y;
      if (y>ya)
	ya=y;
    }
  double lx = xa-xi;
  double ly = ya-yi;
  double xc = (xi+xa)/2.;
  double yc = (yi+ya)/2.;
  printf("%lf<=x<=%lf  %lf<=x<=%lf  lx=%lf ly=%lf xc=%lf yc=%lf\n",xi,xa,yi,ya,lx,ly,xc,yc);
  return cpt==39;
  return (xi>colsGlobal/2.) && (xa<colsGlobal/2.);
  return (xi>rowsGlobal/2.) && (xa<rowsGlobal/2.);

  return 1;
}     // FIN void analyseContour(std::vector<Point> &contour)
// ***************************************************************************************

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
  //printf("%u %u ",x,y);
  uint l = y;
  uint c = x;
  if  ( event == EVENT_LBUTTONDOWN )
    {
      //printf("Left button of the mouse is clicked \n");
      printf("callBack: x=%u y=%u values= ",x,y);
      uint k = 3*l*colsGlobal + 3*c;
      printf("R=%u G=%u B=%u\n",dataGlobal[k + R],dataGlobal[k + G],dataGlobal[k + B]);
    }
  else if  ( event == EVENT_RBUTTONDOWN )
    {
      //printf("Right button of the mouse is clicked \n");
    }
  else if  ( event == EVENT_MBUTTONDOWN )
    {
      //printf("Middle button of the mouse is clicked \n");
    }
  else if ( event == EVENT_MOUSEMOVE )
    {
      //printf("Mouse move over the window - position \n");
    }
}     // FINvoid CallBackFunc(int event, int x, int y, int flags, void* userdata)
// ****************************************************************************************

int main(int argc, char** argv) 
{ 
  // Read the image file
  std::string fileName = "a.jpg";
  if (argc==2)
    fileName = std::string(argv[1]);
  printf("%s\n",fileName.c_str());
  Mat image = imread(fileName); 
  uint rows = image.rows;
  uint cols = image.cols;
  uchar *data = image.data;
  dataGlobal = data;
  rowsGlobal = rows;
  colsGlobal = cols;

  String windowName = "My Window"; //Name of the window
  namedWindow(windowName, WINDOW_AUTOSIZE); // Create a window
  imshow(windowName, image); // Show our image inside the created window.

  Mat img_gray;
  cvtColor(image, img_gray, COLOR_BGR2GRAY);
  String windowName2 = "Binary Image";
  namedWindow(windowName2, WINDOW_AUTOSIZE); // Create another window
  imshow(windowName2, img_gray); // Show our image inside the created window.
	
  // apply binary thresholding
  Mat thresh;
  threshold(img_gray, thresh, 150, 255, ADAPTIVE_THRESH_GAUSSIAN_C);//THRESH_BINARY);
  //imwrite("image_thres1.jpg", thresh);

  std::vector<Vec4i> hierarchy;
  findContours(thresh, contoursGlobal, hierarchy, RETR_TREE, CHAIN_APPROX_NONE);
  std::vector<std::vector<Point>> contours(1);
  Mat image_copy = image.clone();
  for(uint i= 0;i<contoursGlobal.size();i++)
    {
      if (contoursGlobal[i].size()<100)
      	continue;
      contours[0] = contoursGlobal[i];
      if (analyseContour(contours[0]))
	{
	  drawContours(image_copy, contours, -1, Scalar((i * 4) % 255,( 255 - 12 * i) % 255, (112 * i) % 255), 2);
	}
    }
  // std::vector<std::vector<Point>> contours1 = {contoursGlobal[1]};
  // std::vector<std::vector<Point>> contours2 = {contoursGlobal[2]};
  // std::vector<std::vector<Point>> contours3 = {contoursGlobal[3]};

  printf("il y a %lu contours\n",contoursGlobal.size());
  
  // draw contours on the original image
  // drawContours(image_copy, contours, -1, Scalar(255, 255, 0), 2);
  String windowName3 = "Contour";
  namedWindow(windowName3, WINDOW_AUTOSIZE); // Create another window
  imshow(windowName3, image_copy);

  waitKey(0); // Wait for any keystroke in the window
  destroyWindow(windowName); //destroy the created window
  return 0; 
}
