#include <iostream>
#include <boost/format.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace boost;
using namespace cv;

Mat srcBGR;
Mat srcLab;
Mat destBGR;
Mat destLab;

void onMouse(int event, int x, int y, int flag, void* param) {
	if (event == EVENT_LBUTTONDOWN) {
		cout << boost::format("srcBGR: %1%, srcLab: %2%, destBGR: %3%, destLab: %4%") % srcBGR.at<Vec3b>(y, x) % srcLab.at<Vec3b>(y, x) % destBGR.at<Vec3b>(y, x) % destLab.at<Vec3b>(y, x) << endl;
	}
}

int main(int argc, char* argv[]) {
	if (argc <= 8) {
		cout << boost::format("Usage: %1% <src image> <dest image> <src r> <src g> <src b> <dest r> <dest g> <dest b>") % argv[0] << endl;
		return EXIT_FAILURE;
	}

	srcBGR = imread(argv[1]);
	srcLab = srcBGR.clone();
	destBGR = srcBGR.clone();
	destLab = srcBGR.clone();
	imshow("srcBGR", srcBGR);

	Mat srcColorBGR(Size(1, 1), CV_8UC3, CV_RGB(atoi(argv[3]), atoi(argv[4]), atoi(argv[5])));
	Mat srcColorLab = srcColorBGR.clone();
	cvtColor(srcColorBGR, srcColorLab, CV_BGR2Lab);

	cout << boost::format("SRC  color: BGR=> %1%, Lab=> %2%") % srcColorBGR.at<Vec3b>(0, 0) % srcColorLab.at<Vec3b>(0, 0) << endl;

	Mat destColorBGR(Size(1, 1), CV_8UC3, CV_RGB(atoi(argv[6]), atoi(argv[7]), atoi(argv[8])));
	Mat destColorLab = destColorBGR.clone();
	cvtColor(destColorBGR, destColorLab, CV_BGR2Lab);

	cout << boost::format("DEST color: BGR=> %1%, Lab=> %2%") % destColorBGR.at<Vec3b>(0, 0) % destColorLab.at<Vec3b>(0, 0) << endl;

	int diffL = int(destColorLab.at<Vec3b>(0, 0)[0]) - int(srcColorLab.at<Vec3b>(0, 0)[0]);
	int diffa = int(destColorLab.at<Vec3b>(0, 0)[1]) - int(srcColorLab.at<Vec3b>(0, 0)[1]);
	int diffb = int(destColorLab.at<Vec3b>(0, 0)[2]) - int(srcColorLab.at<Vec3b>(0, 0)[2]);

	cout << boost::format("Lab color vector: [%1%, %2%, %3%]") % diffL % diffa % diffb << endl;

	cvtColor(srcBGR, srcLab, CV_BGR2Lab);

	for(int i = 0; i < srcLab.rows; i++) {
		for(int j = 0; j < srcLab.cols; j++) {
			if (int(srcLab.at<Vec3b>(i, j)[1]) > 0 && int(srcLab.at<Vec3b>(i, j)[1]) < 12) {
				srcLab.at<Vec3b>(i, j)[0] = srcColorLab.at<Vec3b>(0, 0)[0];
			}
			int newL = saturate_cast<uchar>(int(srcLab.at<Vec3b>(i, j)[0]) + diffL);
			int newa = saturate_cast<uchar>(int(srcLab.at<Vec3b>(i, j)[1]) + diffa);
			int newb = saturate_cast<uchar>(int(srcLab.at<Vec3b>(i, j)[2]) + diffb);

			destLab.at<Vec3b>(i, j) = Vec3b(newL, newa, newb);
		}
	}

	cvtColor(destLab, destBGR, CV_Lab2BGR);

	imshow("destBGR", destBGR);
	setMouseCallback("srcBGR", onMouse);
	setMouseCallback("destBGR", onMouse);
	imwrite(argv[2], destBGR);
	waitKey();
	
	return EXIT_SUCCESS;
}
