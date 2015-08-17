#ifndef _COLORMAP_HPP_
#define _COLORMAP_HPP_

#include <iostream>
#include <boost/format.hpp>
#include <gdkmm/pixbuf.h>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace boost;
using namespace Glib;
using namespace cv;

Vec3b RGB2HSV(Vec3b rgb) {
	Mat m(Size(1, 1), CV_8UC3, Scalar(rgb));
	cvtColor(m, m, CV_RGB2HSV);
	return m.at<Vec3b>(0, 0);
}

Vec3b RGB2Lab(Vec3b rgb) {
	Mat m(Size(1, 1), CV_8UC3, Scalar(rgb));
	cvtColor(m, m, CV_RGB2Lab);
	return m.at<Vec3b>(0, 0);
}

Vec3b HSV2RGB(Vec3b hsv) {
	Mat m(Size(1, 1), CV_8UC3, Scalar(hsv));
	cvtColor(m, m, CV_HSV2RGB);
	return m.at<Vec3b>(0, 0);
}

Vec3b Lab2RGB(Vec3b lab) {
	Mat m(Size(1, 1), CV_8UC3, Scalar(lab));
	cvtColor(m, m, CV_Lab2RGB);
	return m.at<Vec3b>(0, 0);
}

Vec3i diffHSV2RGB(Vec3b referRGB, Vec3b srcRGB, Vec3b destRGB) {
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

Vec3i diffLab2RGB(Vec3b referRGB, Vec3b srcRGB, Vec3b destRGB) {
	Vec3b referLab = RGB2Lab(referRGB);
	Vec3b srcLab = RGB2Lab(srcRGB);
	Vec3b destLab = RGB2Lab(destRGB);

	Vec3i diffLab = Vec3i(destLab) - Vec3i(srcLab);
	Vec3i newLab = Vec3i(referLab) + diffLab;

	Vec3b newRGB = Lab2RGB(Vec3b(newLab));
	Vec3i diffRGB = Vec3i(newRGB) - Vec3i(referRGB);

	cout << boost::format("referRGB: %1%, referLab: %2%, srcRGB: %3%, srcLab %4%, destRGB: %5%, destLab: %6%, newRGB: %7%, newLab: %8%, diffRGB: %9%, diffLab: %10%") % referRGB % referLab % srcRGB % srcLab % destRGB % destLab % newRGB % newLab % diffRGB % diffLab << endl;
	return diffRGB;
}

void applyDiffRGB(RefPtr<Gdk::Pixbuf> srcPixbuf, RefPtr<Gdk::Pixbuf> destPixbuf, vector<Point2i>& region, Vec3i diffRGB) {
	int width = srcPixbuf->get_width();
	int channel = srcPixbuf->get_n_channels();
	guint8* srcRaw = srcPixbuf->get_pixels();
	guint8* destRaw = destPixbuf->get_pixels();

	for (Point2i& p : region) {
		int i = p.y * width * channel + p.x * channel;

		int newR = int(srcRaw[i    ]) + diffRGB[0];
		int newG = int(srcRaw[i + 1]) + diffRGB[1];
		int newB = int(srcRaw[i + 2]) + diffRGB[2];

		destRaw[i    ] = saturate_cast<uchar>(newR);
		destRaw[i + 1] = saturate_cast<uchar>(newG);
		destRaw[i + 2] = saturate_cast<uchar>(newB);
	}
}

#endif // _COLORMAP_HPP_
