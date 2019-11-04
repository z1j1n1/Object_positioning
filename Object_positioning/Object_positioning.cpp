#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>


using namespace cv;
using namespace std;

int res = 0;
//find the biggest contour
vector<Point> FindBigestContour(Mat src) {
	int imax = 0;
	int imaxcontour = -1;
	std::vector<std::vector<Point> >contours;
	findContours(src, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
	for (int i = 0; i < contours.size(); i++) {
		int itmp = contourArea(contours[i]);
		if (imaxcontour < itmp) {
			imax = i;
			imaxcontour = itmp;
		}
	}
	vector <Point> emp; emp.clear();
	if (contours.size()) return contours[imax]; else return(emp);
}

vector<vector<Point>> Findall(Mat src) {
	int imax = 0;
	int imaxcontour = -1;
	std::vector<std::vector<Point> >contours;
	findContours(src, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
	/*for (int i = 0; i < contours.size(); i++) {
		int itmp = contourArea(contours[i]);
		if (imaxcontour < itmp) {
			imax = i;
			imaxcontour = itmp;
		}
	}*/
	return contours;
}

//remove Light difference by using top hat
Mat moveLightDiff(Mat src, int radius) {
	Mat dst;
	Mat srcclone = src.clone();
	Mat mask = Mat::zeros(radius * 2, radius * 2, CV_8U);
	circle(mask, Point(radius, radius), radius, Scalar(255), -1);
	//top hat
	erode(srcclone, srcclone, mask);
	dilate(srcclone, srcclone, mask);
	dst = src - srcclone;
	return dst;
}

void capture_blue (Mat edges) {
	for (int i = 0; i < edges.rows; i++) {
		uchar* data = edges.ptr<uchar>(i);//get the address of row i;  
		for (int j = 0; j < edges.cols; j++) {
			//if (data[3 * j] > 100 && data[3 * j + 1] < 100 && data[3 * j + 2] < 100)
      if (data[3*j] > data[3*j+1]+ data[3*j+2])
				data[3 * j] = data[3 * j + 1] = data[3 * j + 2] = 255; else
				data[3 * j] = data[3 * j + 1] = data[3 * j + 2] = 0;
		}
	}
}

void capture_green(Mat edges) {
	for (int i = 0; i < edges.rows; i++) {
		uchar* data = edges.ptr<uchar>(i);//get the address of row i;  
		for (int j = 0; j < edges.cols; j++) {
			//if (data[3 * j] > 100 && data[3 * j + 1] < 100 && data[3 * j + 2] < 100)
			if (data[3 * j+1] > data[3 * j ] + data[3 * j + 2])
				data[3 * j] = data[3 * j + 1] = data[3 * j + 2] = 255; else
				data[3 * j] = data[3 * j + 1] = data[3 * j + 2] = 0;
		}
	}
}

void capture_red(Mat edges) {
	for (int i = 0; i < edges.rows; i++) {
		uchar* data = edges.ptr<uchar>(i);//get the address of row i;  
		for (int j = 0; j < edges.cols; j++) {
			//if (data[3 * j] > 100 && data[3 * j + 1] < 100 && data[3 * j + 2] < 100)
			if (data[3 * j+2] > data[3 * j + 1] + data[3 * j ])
				data[3 * j] = data[3 * j + 1] = data[3 * j + 2] = 255; else
				data[3 * j] = data[3 * j + 1] = data[3 * j + 2] = 0;
		}
	}
}

int CONTOUR(Mat inp,int typ)
{
	Mat src = inp;
	Mat src_hsv;
	Mat bin;
	Mat src_h=src.clone();

	//imshow("nan", src_h);
	if (typ == 0) capture_blue(src_h); else
		if (typ == 1) capture_green(src_h); else
			if (typ == 2) capture_red(src_h);
	//src_h = moveLightDiff(src_h, 40);
	vector<Mat> rgb_planes;
	split(src_h, rgb_planes);
	src_h = rgb_planes[1]; // h channel is useful
	threshold(src_h, bin, 100, 255, THRESH_OTSU);
	//bin = src_h.clone();

	//find and draw the biggest contour
	imwrite("thres.jpg",bin);
	vector<Point> bigestcontrour = FindBigestContour(bin);
	vector<vector<Point> > controus; 
  controus.push_back(bigestcontrour);
	//vector<vector<Point> > controus = Findall(bin);
  if (controus[0].size()) {
	  cv::drawContours(src, controus, 0, Scalar(0, 0, 0), 10);

	  imshow("cur", src);

	  Moments moment;
	  moment = moments(controus[0], false);
    if (moment.m00) 
		  printf("x=%d y=%d area=%d\n", cvRound(moment.m10 / moment.m00), cvRound(moment.m01 / moment.m00),cvRound(contourArea(controus[0])));
  }else {
	  printf("???\n");
  }
	
	return 0;
}

int main(int argc, char ** argv) {
  if (argc==1) {
	  printf("Input Color?");
	  return(-1);
  }
  int typ = 0;
  if (argv[1][0] == 'b') typ = 0; else
	  if (argv[1][0] == 'g') typ = 1; else
		  if (argv[1][0] == 'r') typ = 2;
  VideoCapture cap(0);
  if (!cap.isOpened())
  {
	  return -1;
  }
  while (1) {
	  Mat frame;
	  cap >> frame;
	  imshow("ori", frame);
	  CONTOUR(frame,typ);
	  waitKey(1);
  }
}