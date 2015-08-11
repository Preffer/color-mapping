#ifndef _UTIL_HPP_
#define _UTIL_HPP_

#include <iostream>
#include <gdkmm/rgba.h>
#include <boost/format.hpp>

using namespace std;
using namespace boost;

const int COLOR_DEPTH = 255;

ostream& operator<<(ostream& out, const Gdk::RGBA& rgba) {
	return out << format("[%1%, %2%, %3%, %4%]") % (rgba.get_red() * COLOR_DEPTH) % (rgba.get_green() * COLOR_DEPTH) % (rgba.get_blue() * COLOR_DEPTH) % (rgba.get_alpha() * COLOR_DEPTH);
}

#endif // _UTIL_HPP_
