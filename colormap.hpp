#ifndef _COLORMAP_HPP_
#define _COLORMAP_HPP_

#include <iostream>
#include <boost/format.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace boost;
using namespace cv;

Vec3b RGB2HSV(Vec3b rgb) {
	Mat m(Size(1, 1), CV_8UC3, Scalar(rgb));
	cvtColor(m, m, CV_RGB2HSV);
	return m.at<Vec3b>(0, 0);
}

Vec3b HSV2RGB(Vec3b hsv) {
	Mat m(Size(1, 1), CV_8UC3, Scalar(hsv));
	cvtColor(m, m, CV_HSV2RGB);
	return m.at<Vec3b>(0, 0);
}

Vec3i diffHSV2RGB( Vec3b referRGB, Vec3b srcRGB, Vec3b destRGB) {
	Vec3b referHSV = RGB2HSV(referRGB);
	Vec3b srcHSV = RGB2HSV(srcRGB);
	Vec3b destHSV = RGB2HSV(destRGB);

	Vec3i diffHSV = Vec3i(destHSV) - Vec3i(srcHSV);
	Vec3i newHSV = Vec3i(referHSV) + diffHSV;
	if (newHSV[0] > 180) newHSV[0] -= 180;
	if (newHSV[0] <   0) newHSV[0] += 180;

	Vec3b newRGB = HSV2RGB(Vec3b(newHSV));
	Vec3i diffRGB = Vec3i(newRGB) - Vec3i(referRGB);

	cout << boost::format("referRGB: %1%, referHSV: %2%, srcRGB: %3%, srcHSV %4%, destRGB: %5%, destHSV: %6%, newRGB: %7%, newHSV: %8%, diffRGB: %9%, diffHSV: %10%") % referRGB % referHSV % srcRGB % srcHSV % destRGB % destHSV % newRGB % newHSV % diffRGB % diffHSV << endl;
	return diffRGB;
}

#endif // _COLORMAP_HPP_
