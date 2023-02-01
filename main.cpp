#include<iostream>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>	// 报错：未定义标识符CV_WINDOW_AUTOSIZE，所以需要加上该行
#include<opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

extern void myBilateralFilter(const Mat &src, Mat &dst, int ksize, double space_sigma, double color_sigma);

int main()
{
	Mat src=imread("1.jpeg", IMREAD_COLOR);
	Mat dst1=src.clone();
	Mat dst2 = src.clone();
	int ksize = 20;
	double space_sigma = 80;
	double color_sigma = 80;
	myBilateralFilter(src, dst1,ksize,space_sigma,color_sigma);
	bilateralFilter(src, dst2, ksize, space_sigma, color_sigma);
	namedWindow("src", CV_WINDOW_AUTOSIZE);
	namedWindow("dst1-myBilateralFilter", CV_WINDOW_AUTOSIZE);
	namedWindow("dst2-bilateralFilter", CV_WINDOW_AUTOSIZE);
	imshow("src", src);
	imshow("dst1-myBilateralFilter", dst1);
	imshow("dst2-bilateralFilter", dst1);
	waitKey();
}

