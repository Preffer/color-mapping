#ifndef _UTIL_HPP_
#define _UTIL_HPP_

#include <iostream>
#include <gdkmm/rgba.h>
#include <boost/format.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace boost;
using namespace cv;

const int COLOR_DEPTH = 255;

ostream& operator<<(ostream& out, const Gdk::RGBA& rgba) {
	return out << boost::format("[%1%, %2%, %3%, %4%]") % (rgba.get_red() * COLOR_DEPTH) % (rgba.get_green() * COLOR_DEPTH) % (rgba.get_blue() * COLOR_DEPTH) % (rgba.get_alpha() * COLOR_DEPTH);
}

Vec3i diffHSVToRGB(Vec3i src, Vec3i dest, Vec3i base) {
	Mat srcRGB = Mat(Size(1, 1), CV_8UC3, Scalar(src));
	Mat srcHSV;
	cvtColor(srcRGB, srcHSV, CV_RGB2HSV);

	Mat destRGB = Mat(Size(1, 1), CV_8UC3, Scalar(dest));
	Mat destHSV;
	cvtColor(destRGB, destHSV, CV_RGB2HSV);

	int diffH = int(destHSV.at<Vec3b>(0, 0)[0]) - int(srcHSV.at<Vec3b>(0, 0)[0]);
	int diffS = int(destHSV.at<Vec3b>(0, 0)[1]) - int(srcHSV.at<Vec3b>(0, 0)[1]);
	int diffV = int(destHSV.at<Vec3b>(0, 0)[2]) - int(srcHSV.at<Vec3b>(0, 0)[2]);

	Mat baseRGB = Mat(Size(1, 1), CV_8UC3, Scalar(base));
	Mat baseHSV;
	cvtColor(baseRGB, baseHSV, CV_RGB2HSV);

	int newH = int(baseHSV.at<Vec3b>(0, 0)[0]) + diffH;
	int newS = int(baseHSV.at<Vec3b>(0, 0)[1]) + diffS;
	int newV = int(baseHSV.at<Vec3b>(0, 0)[2]) + diffV;
	if (newH > 180) newH -= 180;
	if (newH <   0) newH += 180;

	Mat afterHSV = Mat(Size(1, 1), CV_8UC3, Scalar(newH, saturate_cast<uchar>(newS), saturate_cast<uchar>(newV)));
	Mat afterRGB;
	cvtColor(afterHSV, afterRGB, CV_HSV2RGB);

	Vec3i after = afterRGB.at<Vec3b>(0, 0);
	Vec3i diff = after - base;

	cout << boost::format("srcRGB: %1%, srcHSV %2%, destRGB: %3%, destHSV: %4%, baseRGB: %5%, baseHSV: %6%, afterHSV: %7%, afterRGB: %8%, diffHSV: %9%, diffRGB: %10%") % srcRGB.at<Vec3b>(0, 0) % srcHSV.at<Vec3b>(0, 0) % destRGB.at<Vec3b>(0, 0) % destHSV.at<Vec3b>(0, 0) % baseRGB.at<Vec3b>(0, 0) % baseHSV.at<Vec3b>(0, 0) % Vec3i(newH, newS, newV) % after % Vec3i(diffH, diffS, diffV) % diff << endl;
	return diff;
}

#endif // _UTIL_HPP_
