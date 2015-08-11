#ifndef _COLORMAP_HPP_
#define _COLORMAP_HPP_

#include <iostream>
#include <boost/format.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace boost;
using namespace cv;

Vec3i diffHSV2RGB(Vec3i src, Vec3i dest, Vec3i base) {
	Mat srcRGB = Mat(Size(1, 1), CV_8UC3, Scalar(src));
	Mat srcHSV;
	cvtColor(srcRGB, srcHSV, CV_RGB2HSV);

	Mat destRGB = Mat(Size(1, 1), CV_8UC3, Scalar(dest));
	Mat destHSV;
	cvtColor(destRGB, destHSV, CV_RGB2HSV);

	Mat baseRGB = Mat(Size(1, 1), CV_8UC3, Scalar(base));
	Mat baseHSV;
	cvtColor(baseRGB, baseHSV, CV_RGB2HSV);

	Vec3i diffHSV = Vec3i(destHSV.at<Vec3b>(0, 0)) - Vec3i(srcHSV.at<Vec3b>(0, 0));
	Vec3i newHSV = Vec3i(baseHSV.at<Vec3b>(0, 0)) + diffHSV;
	if (newHSV[0] > 180) newHSV[0] -= 180;
	if (newHSV[0] <   0) newHSV[0] += 180;

	Mat newHSVMat = Mat(Size(1, 1), CV_8UC3, Scalar(newHSV));
	Mat newRGBMat;
	cvtColor(newHSVMat, newRGBMat, CV_HSV2RGB);

	Vec3i newRGB = newRGBMat.at<Vec3b>(0, 0);
	Vec3i diffRGB = newRGB - base;

	cout << boost::format("srcRGB: %1%, srcHSV %2%, destRGB: %3%, destHSV: %4%, baseRGB: %5%, baseHSV: %6%, newRGB: %7%, newHSV: %8%, diffRGB: %9%, diffHSV: %10%") % src % srcHSV.at<Vec3b>(0, 0) % dest % destHSV.at<Vec3b>(0, 0) % base % baseHSV.at<Vec3b>(0, 0) % newRGB % newHSV % diffRGB % diffHSV << endl;
	return diffRGB;
}

#endif // _COLORMAP_HPP_
