#include <iostream>
#include <boost/format.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace boost;
using namespace cv;

Mat srcBGR;
Mat srcHSV;
Mat destBGR;
Mat destHSV;

void onMouse(int event, int x, int y, int flag, void* param) {
	//static bool down = false;
	//if (event == EVENT_LBUTTONDOWN) down = true;
	//if (event == EVENT_LBUTTONUP) down = false;

	if (event == EVENT_LBUTTONDOWN) {
		cout << boost::format("srcBGR: %1%, srcHSV: %2%, destBGR: %3%, destHSV: %4%") % srcBGR.at<Vec3b>(y, x) % srcHSV.at<Vec3b>(y, x) % destBGR.at<Vec3b>(y, x) % destHSV.at<Vec3b>(y, x) << endl;
	}
}

int main(int argc, char* argv[]) {
	if (argc <= 8) {
		cout << boost::format("Usage: %1% <src image> <dest image> <src r> <src g> <src b> <dest r> <dest g> <dest b>") % argv[0] << endl;
		return EXIT_FAILURE;
	}

	srcBGR = imread(argv[1]);
	srcHSV = srcBGR.clone();
	destBGR = srcBGR.clone();
	destHSV = srcBGR.clone();
	imshow("srcBGR", srcBGR);

	Mat srcColorBGR(Size(1, 1), CV_8UC3, CV_RGB(atoi(argv[3]), atoi(argv[4]), atoi(argv[5])));
	Mat srcColorHSV = srcColorBGR.clone();
	cvtColor(srcColorBGR, srcColorHSV, CV_BGR2HSV);

	cout << boost::format("SRC  color: BGR=> %1%, HSV=> %2%") % srcColorBGR.at<Vec3b>(0, 0) % srcColorHSV.at<Vec3b>(0, 0) << endl;

	Mat destColorBGR(Size(1, 1), CV_8UC3, CV_RGB(atoi(argv[6]), atoi(argv[7]), atoi(argv[8])));
	Mat destColorHSV = destColorBGR.clone();
	cvtColor(destColorBGR, destColorHSV, CV_BGR2HSV);

	cout << boost::format("DEST color: BGR=> %1%, HSV=> %2%") % destColorBGR.at<Vec3b>(0, 0) % destColorHSV.at<Vec3b>(0, 0) << endl;

	int diffH = int(destColorHSV.at<Vec3b>(0, 0)[0]) - int(srcColorHSV.at<Vec3b>(0, 0)[0]);
	int diffS = int(destColorHSV.at<Vec3b>(0, 0)[1]) - int(srcColorHSV.at<Vec3b>(0, 0)[1]);
	int diffV = int(destColorHSV.at<Vec3b>(0, 0)[2]) - int(srcColorHSV.at<Vec3b>(0, 0)[2]);

	cout << boost::format("HSV color vector: [%1%, %2%, %3%]") % diffH % diffS % diffV << endl;

	cvtColor(srcBGR, srcHSV, CV_BGR2HSV);

	for(int i = 0; i < srcHSV.rows; i++) {
		for(int j = 0; j < srcHSV.cols; j++) {
			if (int(srcHSV.at<Vec3b>(i, j)[1]) > 0 && int(srcHSV.at<Vec3b>(i, j)[1]) < 12) {
				srcHSV.at<Vec3b>(i, j)[0] = srcColorHSV.at<Vec3b>(0, 0)[0];
			}
			int newH = int(srcHSV.at<Vec3b>(i, j)[0]) + diffH;
			int newS = int(srcHSV.at<Vec3b>(i, j)[1]) + diffS;
			int newV = int(srcHSV.at<Vec3b>(i, j)[2]) + diffV;

			if (newH > 180) newH -= 180;
			if (newH < 0) newH += 180;

			if (newS > 255) newS = 255;
			if (newS < 0) newS = 0;

			if (newV > 255) newV = 255;
			if (newV < 0) newV = 0;

			destHSV.at<Vec3b>(i, j) = Vec3b(newH, newS, newV);
		}
	}

	cvtColor(destHSV, destBGR, CV_HSV2BGR);

	imshow("destBGR", destBGR);
	setMouseCallback("srcBGR", onMouse);
	setMouseCallback("destBGR", onMouse);
	imwrite(argv[2], destBGR);
	waitKey();
	
	return EXIT_SUCCESS;
}
