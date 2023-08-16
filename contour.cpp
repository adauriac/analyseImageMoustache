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
#include <vector>
#include <set>
#include <unordered_set>
#include <algorithm>


#define _COMMENTS "\
cvtColor(image, img_gray, COLOR_BGR2GRAY); NO PARAM\n			\
threshold(img_gray, thresh, 150,255, ADAPTIVE_THRESH_GAUSSIAN_C); 3 PARAMS\n\
findContours(thresh, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE 2 PARAMS\n\
\n"

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

// **********************************************************************
//                            PROTOTYPE DES FUNCTIONS
// **********************************************************************
// **********************************************************************
//                            DEFINITION DES FUNCTIONS
// **********************************************************************
using namespace cv;
using namespace std;

uchar *dataGlobal = NULL;
uint rowsGlobal=0,colsGlobal = 0;
vector<vector<Point>> contoursGlobal;
int intGlobal=-1;
vector<Point> multiplon(vector<Point> &vecteurOriginal);

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
  vector<Mat> matChannels;
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

int analyseContour(vector<Point> &contour) // 36
// retourne 1 si mertote d'etre analyser
{
  static int cpt = -1;
  cpt++;
  /*
  if (!(((int)contour.size()>390) && ((int)contour.size()<582)))
    return 0;
  */
  int contSize = contour.size();
  if (contSize<300)
    return 0;
  if (contSize>600)
    return 0;
  printf("%d longeur= %lu (row=%d,cols=%d) ",cpt,contour.size(),rowsGlobal,colsGlobal);
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
  if ((xi>colsGlobal/2) || (xa<colsGlobal/2))
    return 0;
  // return (contour.size()>200) && (contour.size()<700);
  // return (xi>colsGlobal/2.) && (xa<colsGlobal/2.);
  // return (xi>rowsGlobal/2.) && (xa<rowsGlobal/2.);

  return 1;
}     // FIN void analyseContour(vector<Point> &contour)
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

void analyseContours(vector<vector<Point>> &contours)
{
  printf("Entering analyseContours\n");
  for (uint i=0;i<contours.size();i++)
    {
      vector<Point> contour = contours[i];
      uint s = contour.size();
      // creation fichier gnuplot
      FILE *ft = NULL;
      char name[255];
      sprintf(name,"contour%u_",(uint)s);
      char nameComplet[256+30];
      for(uint k=0;k<1000;k++)
	{
	  if (k==1000-1)
	    _STOP("%s : trop de fichiers de contour de cette taille\n",nameComplet);
	  sprintf(nameComplet,"%s%d.gnu",name,k);
	  ft = fopen(nameComplet,"r");
	  if (ft==NULL)
	    {
	      // n'existe pas, je peux le creer
	      ft = fopen(nameComplet,"w");
	      if (ft==NULL)
		exit(123);
	      //printf("%s ouvert\n",nameComplet);
	      break;
	    }
	  else
	    fclose(ft);
	}
      if (ft==NULL)
	exit(124);
      fprintf(ft,"# contour %d\n",i);
      for(uint j=0;j<s;j++)
	fprintf(ft,"%d %d\n",contour[j].x,contour[j].y);
      fclose(ft);
      vector<Point> mp = multiplon(contour);
      if (mp.size()!=0)
	{
	  printf("%s il a %d point double\n",nameComplet,(int)mp.size());
	}
      // y a t il des points doubles
    }
}     // FIN void analyseContours(vector<vector<Point>> &contours)
// ****************************************************************************************

vector<Point> multiplon(vector<Point> &vecteurOriginal)
// retourne le vecteur des multiplons
{
  vector<Point> ans;
  set<int> setOrigCode;
  for (Point element : vecteurOriginal)
    {
      int k = element.x*10000 + element.y;
      Point pt(k/10000,k%10000);
      if (setOrigCode.count(k)>=1)
	ans.push_back(Point(k/10000,k%10000));
      else
	setOrigCode.insert(k);
    }
  return ans;
}     // FIN vector<Point> multiplon(vector<Point> &vecteurOriginal)
// ****************************************************************************************

int main(int na,char *para[]) 
{
  int i;
  
  int verbose = 0;
  int n = 12;
  char fileName[256] = "b.jpg";
  int typeTh = 0; 
  int p1Th = 127;
  int p2Th = 255;
  int createContourFiles = 0;
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
	  _QU(p1Th,"%d"," premier parametre de threshold");
	  _QU(p2Th,"%d"," deuxieme parametre de threshold");
	  _QU(typeTh,"%d"," type de threshold");
	  _QU(createContourFiles,"%d"," si l'on cree un fichier gnuplot du contour");
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
  printf("%s\n",fileName);
  String windowName = "My Window"; //Name of the window
  namedWindow(windowName, WINDOW_AUTOSIZE); // Create a window
  Mat imageVraie = imread(fileName),image;
  uint rows = imageVraie.rows;
  uint cols = imageVraie.cols;
  if ((rows>960) || (cols>1280))
    {
      resize(imageVraie,image, Size(960,1280), INTER_LINEAR);
      printf("image resized from (%d,%d) to (%d,%d)\n",rows,cols,image.rows,image.cols);
     }
  else
    image = imageVraie;
  moveWindow(windowName,0,0);
  rows = image.rows;
  cols = image.cols;
  uchar *data = image.data;
  dataGlobal = data;
  rowsGlobal = rows;
  colsGlobal = cols;
  imshow(windowName, image); // Show our image inside the created window.

  Mat img_gray;
  cvtColor(image, img_gray, COLOR_BGR2GRAY);
  String windowName2 = "Binary Image";
  namedWindow(windowName2, WINDOW_AUTOSIZE); // Create another window
  imshow(windowName2, img_gray); // Show our image inside the created window.
	
  // apply binary thresholding
  Mat thresh;
  if (typeTh==0)
    threshold(img_gray, thresh, p1Th,p2Th, ADAPTIVE_THRESH_GAUSSIAN_C);
  else if (typeTh==1)
    threshold(img_gray, thresh, p1Th,p2Th, ADAPTIVE_THRESH_MEAN_C);
  else if (typeTh==2)
    threshold(img_gray, thresh, p1Th,p2Th, THRESH_BINARY);
  else if (typeTh==3)
    threshold(img_gray, thresh, p1Th,p2Th, THRESH_BINARY_INV);
  else if (typeTh==4)
    threshold(img_gray, thresh, p1Th,p2Th, THRESH_MASK);
  else if (typeTh==5)
    threshold(img_gray, thresh, p1Th,p2Th, THRESH_OTSU);
   else if (typeTh==6)
    threshold(img_gray, thresh, p1Th,p2Th, THRESH_TOZERO);
  else if (typeTh==7)
    threshold(img_gray, thresh, p1Th,p2Th, THRESH_TOZERO_INV);
  else if (typeTh==8)
    threshold(img_gray, thresh, p1Th,p2Th, THRESH_TRIANGLE);
 else if (typeTh==9)
    threshold(img_gray, thresh, p1Th,p2Th, THRESH_TRUNC);
  else
    _STOP("parametre de threshold %d invalide",typeTh);
  /*
    threshold(Mat, Mat, double, double, int)
    
	ADAPTIVE_THRESH_GAUSSIAN_C
	ADAPTIVE_THRESH_MEAN_C
	THRESH_BINARY
 	THRESH_BINARY_INV
 	THRESH_MASK
	THRESH_OTSU
	THRESH_TOZERO
	THRESH_TOZERO_INV
	THRESH_TRIANGLE
	THRESH_TRUNC
	    
	https://docs.opencv.org/4.x/d7/d1b/group__imgproc__misc.html#gae8a4a146d1ca78c626a53577199e9c57
  */
  imshow("b & w",thresh);
  //imwrite("image_thres1.jpg", thresh);

  vector<Vec4i> hierarchy;
  findContours(thresh, contoursGlobal, hierarchy, RETR_TREE, CHAIN_APPROX_NONE);
  if (createContourFiles)
    analyseContours(contoursGlobal);
  vector<vector<Point>> contours(1);
  Mat image_copy = image.clone();
  uint garde=0;
  for(uint i= 0;i<contoursGlobal.size();i++)
    {
      if (contoursGlobal[i].size()<100)
      	continue;
      contours[0] = contoursGlobal[i];
      if (analyseContour(contours[0]))
	{
	  garde++;
	  drawContours(image_copy, contours, -1, Scalar((i * 4) % 255,( 255 - 12 * i) % 255, (112 * i) % 255), 2);
	}
    }
  // vector<vector<Point>> contours1 = {contoursGlobal[1]};
  // vector<vector<Point>> contours2 = {contoursGlobal[2]};
  // vector<vector<Point>> contours3 = {contoursGlobal[3]};

  printf("il y a %lu contours dont %u gardes\n",contoursGlobal.size(),garde);
  
  // draw contours on the original image
  // drawContours(image_copy, contours, -1, Scalar(255, 255, 0), 2);
  String windowName3 = "Contour";
  namedWindow(windowName3, WINDOW_AUTOSIZE); // Create another window
  imshow(windowName3, image_copy);

  waitKey(0); // Wait for any keystroke in the window
  destroyWindow(windowName); //destroy the created window

  if (verbose)
    _PRINT_TIMENOW(stdout);  
  return 0;
}		/* FIN main() */
// *******************************************************************
