#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <iostream>
using namespace std;
#include <jc.h>

#include <opencv2/opencv.hpp>

#define _COMMENTS "\n"
using namespace cv;

void helper(){}
// **********************************************************************
//                        VARIABLES GLOBALES
// **********************************************************************

// **********************************************************************
//                            MACROS
// **********************************************************************
#include "/home/dauriac/include/macros.h"
#define B 0
#define G 1
#define R 2
/*
  use data[i+B] for the blue canal, data[i+G] for green, and data[i+R] for red
  pour acceder a l'element (l,c) 0<=l<lines; 0<=c<rows
  data[l*3*cols + 3*c + R] = 255
  data[l*3*cols + 3*c + G] = 255;
  data[l*3*cols + 3*c + B] = 255;
*/
uchar *dataGlobal = NULL;
uint rowsGlobal=0,colsGlobal = 0;
uchar *dataGlobalInitial = NULL;

int isRed1(uchar *data,uint ind)
{
  return ((data[ind+R] > data[ind+B]) && (data[ind+R] > data[ind+G]));
}     // FIN int isRed(uchar *data,uint ind)
// ***************************************************************************************
// **********************************************************************
//                            PROTOTYPE DES FUNCTIONS
// **********************************************************************
// **********************************************************************
//                            DEFINITION DES FUNCTIONS
// **********************************************************************

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
}     // FIN int isRed1(uchar *data,uint ind)
// ***************************************************************************************

int isBlack(uchar *data,uint ind)
{
  uint seuil = 60;
  return (data[ind+R]<seuil) && (data[ind+B]<seuil) && (data[ind+G]<seuil);
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
  static int first = 1;
  static FILE *ft;
  uint l = y;
  uint c = x;
  if  ( event == EVENT_LBUTTONDOWN )
    {
      //printf("Left button of the mouse is clicked \n");
      if (first)
	{
	  first = 0;
	  ft = fopen("traceRed","a");
	  if (ft==NULL)
	    exit(0);
	}
      printf("callBack: x=%u y=%u values= ",x,y);
      uint k = 3*l*colsGlobal + 3*c;
      printf("R=%u G=%u B=%u\n",dataGlobalInitial[k + R],dataGlobalInitial[k + G],dataGlobalInitial[k + B]);
      fprintf(ft,"%u %u %u   %u %u\n",dataGlobalInitial[k + R],dataGlobalInitial[k + G],dataGlobalInitial[k + B],x,y);
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


int main(int na,char *para[]) 
{
  int i;
  
  int verbose = 0;
  int n = 12;
  char fileName[256] = "a.jpg";
  _INITQ;
  for (i=1;i<=na;i++)
    {
      char *st,noml[8192],ok,j;
     
      st = para[i];
      ok = i==na;
      if (!ok)
        cmdline+=string(st)+" ";
      
      for (j=0;j<2;j++)
	{
	  if (j && !ok) 
	    {
	      printf(_COMMENTS);
	      printf("usage (%s):\n",st);
	    }
	  // METTRE ICI LES DIFFERNTES VALEURS DE LA LIGNE DE COMMANDE
	  _QU(verbose,"%d"," (niveau de blabla)");
	  _QU(n,"%d"," (valeur de n)");
	  _QS(fileName,"%s","fichier a traiter");
	}
      if (!ok) 
        {
          helper();
          exit(0);
        }
    }
  // _SET_SEED; // decommenter si seed existe : resoud seed=0 et set redoline
  if (verbose)
    _IMPRIM_PARAM(stdout);
  Mat image = imread(fileName); 
  uint rows = image.rows;
  uint cols = image.cols;
  uchar *data = image.data;
  dataGlobal = data;
  rowsGlobal = rows;
  colsGlobal = cols;

  _ALLOUE(dataGlobalInitial,rows*cols*3,uchar);
  for(uint i=0;i<rows*cols*3;i++)
    dataGlobalInitial[i] = data[i];
 
  String windowName = "My Window"; //Name of the window
  namedWindow(windowName); // Create a window
  imshow(windowName, image); // Show our image inside the created window.
  setMouseCallback("My Window", CallBackFunc, NULL);
  waitKey(0); // Wait for any keystroke in the window
  printf("modification de l'image");
 
  for (uint i=0;i<3*rows*cols*3;i+=3)
    {
      if (isRed(data,i))
	{
	  data[i+R] = 255;
	  data[i+G] = 0;
	  data[i+B] = 0;
	  continue;
	}
      if (isBlack(data,i))
	{
	  data[i+R] = 0;
	  data[i+G] = 0;
	  data[i+B] = 0;
	  continue;
	}
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

  if (verbose)
    _PRINT_TIMENOW(stdout);  
  return 0;
}		/* FIN main() */
// *******************************************************************
