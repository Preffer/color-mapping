#include <iostream>
#include <boost/format.hpp>
#include <unordered_map>
#include "util.hpp"
#include "colormap.hpp"
#include "colorwindow.hpp"

using namespace std;
using namespace boost;
using namespace Gtk;
using namespace Glib;
using namespace cv;

Window* ColorWindow::create() {
	RefPtr<Builder> builder;

	try {
		builder = Builder::create_from_file(ui_file);
	} catch (const FileError& e) {
		cerr << e.what() << endl;
		throw;
	}

	builder->get_widget("mainWindow", mainWindow);
	builder->get_widget("sceneEventBox", sceneEventBox);
	builder->get_widget("scene", scene);
	builder->get_widget("sceneFile", sceneFile);
	builder->get_widget("depthFile", depthFile);
	builder->get_widget("preProcessButton", preProcessButton);
	builder->get_widget("pickMaterialButton", pickMaterialButton);
	builder->get_widget("referColor", referColor);
	builder->get_widget("srcColor",   srcColor);
	builder->get_widget("destColor",  destColor);
	builder->get_widget("originButton",  originButton);
	builder->get_widget("saveButton",  saveButton);

	sceneEventBox->signal_button_press_event().connect(sigc::mem_fun(*this, &ColorWindow::onScenePress));
	sceneEventBox->signal_button_release_event().connect(sigc::mem_fun(*this, &ColorWindow::onSceneRelease));
	sceneEventBox->signal_motion_notify_event().connect(sigc::mem_fun(*this, &ColorWindow::onSceneMotion));
	preProcessButton->signal_clicked().connect(sigc::mem_fun(*this, &ColorWindow::onPreProcessButtonClick));
	referColor->signal_color_set().connect(sigc::mem_fun(*this, &ColorWindow::onReferColorSet));
	srcColor->signal_color_set().connect(sigc::mem_fun(*this, &ColorWindow::onSrcColorSet));
	destColor->signal_color_set().connect(sigc::mem_fun(*this, &ColorWindow::onDestColorSet));

	return mainWindow;
}

/* Signal handler */
bool ColorWindow::onScenePress(GdkEventButton* event) {
	cout << boost::format("Press: (%1%, %2%)") % event->x % event->y << endl;
	return false;
}

bool ColorWindow::onSceneRelease(GdkEventButton* event) {
	cout << boost::format("Release: (%1%, %2%)") % event->x % event->y << endl;
	return false;
}

bool ColorWindow::onSceneMotion(GdkEventMotion* event) {
	cout << boost::format("Motion: (%1%, %2%)") % event->x % event->y << endl;
	return false;
}

void ColorWindow::onPreProcessButtonClick() {
	cout << boost::format("onPreProcessButtonClick: sceneFile => %1%, depthFile => %2%") % sceneFile->get_filename() % depthFile->get_filename() << endl;

	// ensure alpha and always rgba
	srcPixbuf = Gdk::Pixbuf::create_from_file(sceneFile->get_filename())->add_alpha(false, 255, 255, 255);
	destPixbuf = srcPixbuf->copy();
	depthPixbuf = Gdk::Pixbuf::create_from_file(depthFile->get_filename())->add_alpha(false, 255, 255, 255);
	scene->set(destPixbuf);

	readMaterial();
}

void ColorWindow::onReferColorSet() {
	cout << "onReferColorSet: " << referColor->get_rgba() << endl;
}

void ColorWindow::onSrcColorSet() {
	cout << "onSrcColorSet: " << srcColor->get_rgba() << endl;
}

void ColorWindow::onDestColorSet() {
	cout << "onDestColorSet: " << destColor->get_rgba() << endl;

	/*Gdk::RGBA rgba = destColor->get_rgba();

	Vec3b src(255, 255, 255);
	Vec3b dest(rgba.get_red() * COLOR_DEPTH, rgba.get_green() * COLOR_DEPTH, rgba.get_blue() * COLOR_DEPTH);
	Vec3b refer(174, 174, 174);

	Vec3i diffRGB = diffHSV2RGB(src, dest, refer);
	int width = srcPixbuf->get_width();
	guint8* srcRaw = srcPixbuf->get_pixels();
	guint8* destRaw = destPixbuf->get_pixels();

	for (Point2i& point : material.second) {
		int i = point.y * width * 4 + point.x * 4;

		int newR = int(srcRaw[i    ]) + diff[0];
		int newG = int(srcRaw[i + 1]) + diff[1];
		int newB = int(srcRaw[i + 2]) + diff[2];

		destRaw[i    ] = saturate_cast<uchar>(newR);
		destRaw[i + 1] = saturate_cast<uchar>(newG);
		destRaw[i + 2] = saturate_cast<uchar>(newB);
	}

	scene->set(destPixbuf);*/
}

void ColorWindow::readMaterial() {
	guint8* depthRaw = depthPixbuf->get_pixels();
	guint8* srcRaw = srcPixbuf->get_pixels();

	int width = destPixbuf->get_width();
	int height = destPixbuf->get_height();

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int index = y * width * 4 + x * 4;
			Vec3b diffuseColor(depthRaw[index], depthRaw[index + 1], depthRaw[index + 2]);

			bool found = false;
			for (Material& m : library) {
				if (m.diffuseColor == diffuseColor) {
					m.region.push_back(Point2i(x, y));
					found = true;
					break;
				}
			}

			if (!found) {
				Material m(diffuseColor);
				m.region.push_back(Point2i(x, y));
				library.push_back(m);
			}
		}
	}

	for (Material& m : library) {
		unordered_map<Vec3b, int> billboard;

		for (Point2i& p : m.region) {
			int index = p.y * width * 4 + p.x * 4;
			Vec3b renderColor(srcRaw[index], srcRaw[index + 1], srcRaw[index + 2]);

			// implicit zero init
			billboard[renderColor]++;
		}

		Vec3b renderColor = billboard.begin()->first;
		int max = billboard.begin()->second;
		for (auto& s : billboard) {
			if (s.second > max) {
				renderColor = s.first;
				max = s.second;
			}
		}
		m.renderColor = renderColor;
	}
}
