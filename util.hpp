#ifndef _UTIL_HPP_
#define _UTIL_HPP_

#include <iostream>
#include <gdkmm/rgba.h>
#include <boost/format.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "material.hpp"

using namespace std;
using namespace boost;
using namespace cv;

const int COLOR_DEPTH = 255;

ostream& operator<<(ostream& out, const Gdk::RGBA& rgba) {
	return out << boost::format("[%1%, %2%, %3%, %4%]") % (rgba.get_red() * COLOR_DEPTH) % (rgba.get_green() * COLOR_DEPTH) % (rgba.get_blue() * COLOR_DEPTH) % (rgba.get_alpha() * COLOR_DEPTH);
}

ostream& operator<<(ostream& out, const vector<Point2i>& r) {
	for (const Point2i& p : r) {
		out << p;
	}
	return out;
}

ostream& operator<<(ostream& out, const Material& m) {
	return out << boost::format("{diffuseColor: %1%, renderColor: %2%, region: %3%}") % m.diffuseColor % m.renderColor % m.region;
}

ostream& operator<<(ostream& out, const vector<Material> l) {
	out << "[";
	for (const Material& m : l) {
		out << m << ", ";
	}
	out << "]" << endl;
	return out;
}

namespace std {
	template <>
	struct hash<Vec3b> {
		size_t operator()(const Vec3b& k) const {
			return ((hash<uchar>()(k[0]) ^ (hash<uchar>()(k[1]) << 1)) >> 1) ^ (hash<uchar>()(k[2]) << 1);
		}
	};
}

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

#endif // _UTIL_HPP_
