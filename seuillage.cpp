#include <opencv2/opencv.hpp> 
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

int isRed(uchar *data,uint ind)
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
}     // FIN int isRed(uchar *data,uint ind)
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
 Mat image = imread("a.jpg"); 
 uint rows = image.rows;
 uint cols = image.cols;
 uchar *data = image.data;
 dataGlobal = data;
 rowsGlobal = rows;
 colsGlobal = cols;

 String windowName = "My Window"; //Name of the window
 namedWindow(windowName); // Create a window
 imshow(windowName, image); // Show our image inside the created window.
 setMouseCallback("My Window", CallBackFunc, NULL);
 waitKey(0); // Wait for any keystroke in the window
 printf("modifivation de l'image");
 
 for (uint i=0;i<3*rows*cols*3;i+=3)
   {
     if (isRed(data,i))
       continue;
     // if (isBlack(data,i))
     //  continue;
     data[i+R] = 255;
     data[i+G] = 255;
     data[i+B] = 255;
   }
 if (0) for (uint l = 100;l<200;l++)
   {
     for(uint c=cols/2-100;c<cols/2+100;c++)
       {

       }
   }
 imshow(windowName, image); // Show our image inside the created window.
 waitKey(0); // Wait for any keystroke in the window
 destroyWindow(windowName); //destroy the created window
 return 0; 
}