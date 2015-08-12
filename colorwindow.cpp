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
	scene->signal_size_allocate().connect(sigc::mem_fun(*this, &ColorWindow::onSceneSizeAllocate));
	preProcessButton->signal_clicked().connect(sigc::mem_fun(*this, &ColorWindow::onPreProcessButtonClick));
	pickMaterialButton->signal_toggled().connect(sigc::mem_fun(*this, &ColorWindow::onPickMaterialButtonToggle));
	referColor->signal_color_set().connect(sigc::mem_fun(*this, &ColorWindow::onColorSet));
	srcColor->signal_color_set().connect(sigc::mem_fun(*this, &ColorWindow::onColorSet));
	destColor->signal_color_set().connect(sigc::mem_fun(*this, &ColorWindow::onColorSet));

	return mainWindow;
}

/* Signal handler */
bool ColorWindow::onScenePress(GdkEventButton* event) {
	if (pick == PickStatus::awake) {
		pick = PickStatus::press;
	}
	return false;
}

bool ColorWindow::onSceneRelease(GdkEventButton* event) {
	pick = PickStatus::sleep;
	pickMaterialButton->set_active(false);
	sceneEventBox->get_window()->set_cursor();
	return false;
}

bool ColorWindow::onSceneMotion(GdkEventMotion* event) {
	if (pick == PickStatus::press) {
		Point2i point(event->x - border.x, event->y - border.y);

		int width = depthPixbuf->get_width();
		int height = depthPixbuf->get_height();

		if (point.x < width && point.y < height) {
			int channel = depthPixbuf->get_n_channels();
			int index = point.y * depthPixbuf->get_width() * channel + point.x * channel;
			guint8* depthRaw = depthPixbuf->get_pixels();
			Vec3b diffuseColor(depthRaw[index], depthRaw[index + 1], depthRaw[index + 2]);

			for (Material& m : library) {
				if (diffuseColor == m.diffuseColor) {
					material = &m;
					break;
				}
			}

			if (material != NULL) {
				Gdk::RGBA diffuseRGBA;
				diffuseRGBA.set_rgba_u(material->diffuseColor[0] << 8, material->diffuseColor[1] << 8, material->diffuseColor[2] << 8);
				srcColor->set_rgba(diffuseRGBA);

				Gdk::RGBA renderRGBA;
				renderRGBA.set_rgba_u(material->renderColor[0] << 8, material->renderColor[1] << 8, material->renderColor[2] << 8);
				referColor->set_rgba(renderRGBA);
			} else {
				MessageDialog dialog(*mainWindow, "Material pointer is NULL", false, MESSAGE_ERROR);
				dialog.set_secondary_text("Material pointer is NULL");
				dialog.run();
			}
		}
	}
	return false;
}

void ColorWindow::onSceneSizeAllocate(Allocation& allocation) {
	if (srcPixbuf) {
		border.x = (allocation.get_width()  - srcPixbuf->get_width())  / 2;
		border.y = (allocation.get_height() - srcPixbuf->get_height()) / 2;
	}
}

void ColorWindow::onPreProcessButtonClick() {
	try {
		srcPixbuf = Gdk::Pixbuf::create_from_file(sceneFile->get_filename());
		destPixbuf = srcPixbuf->copy();
		depthPixbuf = Gdk::Pixbuf::create_from_file(depthFile->get_filename());

		if (srcPixbuf->get_width() != depthPixbuf->get_width() || srcPixbuf->get_height() != depthPixbuf->get_height()) {
			MessageDialog dialog(*mainWindow, "Image size dismatch", false, MESSAGE_ERROR);
			dialog.set_secondary_text("Image size dismatch");
			dialog.run();
			return;
		}

		scene->set(destPixbuf);
		readMaterial();
	} catch (const FileError& e) {
		MessageDialog dialog(*mainWindow, g_quark_to_string(e.domain()), false, MESSAGE_ERROR);
		dialog.set_secondary_text(e.what());
		dialog.run();
	} catch (const Gdk::PixbufError& e) {
		MessageDialog dialog(*mainWindow, g_quark_to_string(e.domain()), false, MESSAGE_ERROR);
		dialog.set_secondary_text(e.what());
		dialog.run();
	}
}

void ColorWindow::onPickMaterialButtonToggle() {
	if (pickMaterialButton->get_active()) {
		if (srcPixbuf) {
			pick = PickStatus::awake;
			sceneEventBox->get_window()->set_cursor(Gdk::Cursor::create(Gdk::CursorType::CROSS));
		} else {
			MessageDialog dialog(*mainWindow, "PreProcess image first", false, MESSAGE_ERROR);
			dialog.set_secondary_text("PreProcess image first");
			dialog.run();
			pickMaterialButton->set_active(false);
		}

	} else {
		pick = PickStatus::sleep;
		sceneEventBox->get_window()->set_cursor();
	}
}

void ColorWindow::onColorSet() {
	if (!srcPixbuf) {
		MessageDialog dialog(*mainWindow, "PreProcess image first", false, MESSAGE_ERROR);
		dialog.set_secondary_text("PreProcess image first");
		dialog.run();
		return;
	}

	if (material == NULL) {
		MessageDialog dialog(*mainWindow, "Pick material first", false, MESSAGE_ERROR);
		dialog.set_secondary_text("Pick material first");
		dialog.run();
		return;
	}

	Gdk::RGBA referRGBA = referColor->get_rgba();
	Gdk::RGBA srcRGBA = srcColor->get_rgba();
	Gdk::RGBA destRGBA = destColor->get_rgba();

	Vec3b referRGB(referRGBA.get_red() * COLOR_DEPTH, referRGBA.get_green() * COLOR_DEPTH, referRGBA.get_blue() * COLOR_DEPTH);
	Vec3b srcRGB(srcRGBA.get_red() * COLOR_DEPTH, srcRGBA.get_green() * COLOR_DEPTH, srcRGBA.get_blue() * COLOR_DEPTH);
	Vec3b destRGB(destRGBA.get_red() * COLOR_DEPTH, destRGBA.get_green() * COLOR_DEPTH, destRGBA.get_blue() * COLOR_DEPTH);

	Vec3i diffRGB = diffHSV2RGB(referRGB, srcRGB, destRGB);
	applyDiffRGB(srcPixbuf, destPixbuf, material->region, diffRGB);

	scene->set(destPixbuf);
}

void ColorWindow::readMaterial() {
	guint8* srcRaw = srcPixbuf->get_pixels();
	guint8* depthRaw = depthPixbuf->get_pixels();

	int width = depthPixbuf->get_width();
	int height = depthPixbuf->get_height();
	int channel = depthPixbuf->get_n_channels();

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int index = y * width * channel + x * channel;
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

	channel = srcPixbuf->get_n_channels();
	for (Material& m : library) {
		unordered_map<Vec3b, int> billboard;

		for (Point2i& p : m.region) {
			int index = p.y * width * channel + p.x * channel;
			Vec3b renderColor(srcRaw[index], srcRaw[index + 1], srcRaw[index + 2]);

			// assume zero init
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

		Vec3b diffuseColorHSV = RGB2HSV(m.diffuseColor);
		Vec3b renderColorHSV = RGB2HSV(m.renderColor);

		// low saturation hue sanitize
		if (diffuseColorHSV[1] == 0) {
			renderColorHSV[0] = 0;
			m.renderColor = HSV2RGB(renderColorHSV);
		}
	}
}
