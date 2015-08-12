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
	return out << boost::format("[%1%, %2%, %3%, %4%]") % (rgba.get_red_u() / COLOR_DEPTH) % (rgba.get_green_u() / COLOR_DEPTH) % (rgba.get_blue_u() / COLOR_DEPTH) % (rgba.get_alpha_u() / COLOR_DEPTH);
}

ostream& operator<<(ostream& out, const vector<Point2i>& r) {
	for (const Point2i& p : r) {
		out << p;
	}
	return out;
}

ostream& operator<<(ostream& out, const Material& m) {
	return out << boost::format("{diffuseColor: %1%, renderColor: %2%}") % m.diffuseColor % m.renderColor;
}

ostream& operator<<(ostream& out, const vector<Material> l) {
	out << "[";
	for (const Material& m : l) {
		out << m << ", ";
	}
	out << "]";
	return out;
}

namespace std {
	template <>
	struct hash<Vec3b> {
		size_t operator()(const Vec3b& k) const {
			return hash<int>()(k[0] + (k[1] << 8) + (k[2] << 16));
		}
	};
}

#endif // _UTIL_HPP_
