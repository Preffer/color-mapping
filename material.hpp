#ifndef _MATERIALIAL_HPP_
#define _MATERIALIAL_HPP_

#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

class Material {
public:
	Material(Vec3b _diffuseColor) : diffuseColor(_diffuseColor) {};

	Vec3b diffuseColor;
	Vec3b renderColor;
	vector<Point2i> region;
};

#endif // _MATERIALIAL_HPP_
