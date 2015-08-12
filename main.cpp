#include <iostream>
#include <boost/format.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace boost;
using namespace cv;

Mat srcImgBGR;
Mat srcImgHSV;
Mat destImgBGR;
Mat destImgHSV;

Mat srcColorBGR;
Mat srcColorHSV;
Mat destColorBGR;
Mat destColorHSV;

Vec3i vectorHSV;
Vec3i vectorBGR;

void onMouse(int event, int x, int y, int flag, void* param) {
	if (event == EVENT_LBUTTONDOWN) {
		cout << boost::format("srcImgBGR: %1%, srcImgHSV: %2%, destImgBGR: %3%, destImgHSV: %4%") % srcImgBGR.at<Vec3b>(y, x) % srcImgHSV.at<Vec3b>(y, x) % destImgBGR.at<Vec3b>(y, x) % destImgHSV.at<Vec3b>(y, x) << endl;
	}

	if (event == EVENT_RBUTTONDOWN) {
		Vec3b referVecBGR = srcImgBGR.at<Vec3b>(y, x);
		Mat referColorBGR = Mat(Size(1, 1), CV_8UC3, Scalar(referVecBGR));
		Mat referColorHSV;
		cvtColor(referColorBGR, referColorHSV, CV_BGR2HSV);
		Vec3b referVecHSV = referColorHSV.at<Vec3b>(0, 0);
		
		int newH = int(referVecHSV[0]) + vectorHSV[0];
		int newS = int(referVecHSV[1]) + vectorHSV[1];
		int newV = int(referVecHSV[2]) + vectorHSV[2];
		if (newH > 180) newH -= 180;
		if (newH <   0) newH += 180;
		if (newS > 255) newS = 255;
		if (newS <   0) newS = 0;
		if (newV > 255) newV = 255;
		if (newV <   0) newV = 0;

		Vec3b newVecHSV(newH, newS, newV);
		Mat newColorHSV = Mat(Size(1, 1), CV_8UC3, Scalar(newVecHSV));
		Mat newColorBGR;
		cvtColor(newColorHSV, newColorBGR, CV_HSV2BGR);
		Vec3b newVecBGR = newColorBGR.at<Vec3b>(0, 0);

		vectorBGR = Vec3i(newVecBGR) - Vec3i(referVecBGR);
		cout << boost::format("referBGR: %1%, referHSV %2%, newBGR: %3%, newHSV %4%") % referVecBGR % referVecHSV % newVecBGR % newVecHSV << endl;
		cout << boost::format("BGR color vector: %1%") % vectorBGR << endl;

		for(int i = 0; i < srcImgBGR.rows; i++) {
			for(int j = 0; j < srcImgBGR.cols; j++) {
				int newB = int(srcImgBGR.at<Vec3b>(i, j)[0]) + vectorBGR[0];
				int newG = int(srcImgBGR.at<Vec3b>(i, j)[1]) + vectorBGR[1];
				int newR = int(srcImgBGR.at<Vec3b>(i, j)[2]) + vectorBGR[2];
				if (newB > 255) newB = 255;
				if (newB <   0) newB = 0;
				if (newG > 255) newG = 255;
				if (newG <   0) newG = 0;
				if (newR > 255) newR = 255;
				if (newR <   0) newR = 0;

				destImgBGR.at<Vec3b>(i, j) = Vec3i(newB, newG, newR);
			}
		}

		imshow("destImgBGR", destImgBGR);

		char** argv = (char**)param;
		imwrite(argv[2], destImgBGR);
	}
}

int main(int argc, char* argv[]) {
	if (argc <= 8) {
		cout << boost::format("Usage: %1% <src image> <dest image> <src r> <src g> <src b> <dest r> <dest g> <dest b>") % argv[0] << endl;
		return EXIT_FAILURE;
	}

	srcImgBGR = imread(argv[1]);
	destImgBGR = srcImgBGR.clone();

	cvtColor(srcImgBGR, srcImgHSV, CV_BGR2HSV);
	cvtColor(destImgBGR, destImgHSV, CV_BGR2HSV);

	imshow("srcImgBGR", srcImgBGR);

	srcColorBGR = Mat(Size(1, 1), CV_8UC3, CV_RGB(atoi(argv[3]), atoi(argv[4]), atoi(argv[5])));
	cvtColor(srcColorBGR, srcColorHSV, CV_BGR2HSV);

	cout << boost::format("SRC  color: BGR=> %1%, HSV=> %2%") % srcColorBGR.at<Vec3b>(0, 0) % srcColorHSV.at<Vec3b>(0, 0) << endl;

	destColorBGR = Mat(Size(1, 1), CV_8UC3, CV_RGB(atoi(argv[6]), atoi(argv[7]), atoi(argv[8])));
	cvtColor(destColorBGR, destColorHSV, CV_BGR2HSV);

	cout << boost::format("DEST color: BGR=> %1%, HSV=> %2%") % destColorBGR.at<Vec3b>(0, 0) % destColorHSV.at<Vec3b>(0, 0) << endl;

	int diffH = int(destColorHSV.at<Vec3b>(0, 0)[0]) - int(srcColorHSV.at<Vec3b>(0, 0)[0]);
	int diffS = int(destColorHSV.at<Vec3b>(0, 0)[1]) - int(srcColorHSV.at<Vec3b>(0, 0)[1]);
	int diffV = int(destColorHSV.at<Vec3b>(0, 0)[2]) - int(srcColorHSV.at<Vec3b>(0, 0)[2]);

	vectorHSV = Vec3i(diffH, diffS, diffV);
	cout << boost::format("HSV color vector: %1%") % vectorHSV << endl;
	cout << "Please select refer color in src image" << endl;

	imshow("destImgBGR", destImgBGR);
	setMouseCallback("srcImgBGR", onMouse, argv);
	setMouseCallback("destImgBGR", onMouse, argv);
	waitKey();
	
	return EXIT_SUCCESS;
}
