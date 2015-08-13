#ifndef _COLORWINDOW_HPP_
#define _COLORWINDOW_HPP_

#include <gtkmm/window.h>
#include <gtkmm/builder.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/colorbutton.h>
#include <gtkmm/togglebutton.h>
#include <gtkmm/filechooserbutton.h>
#include <gtkmm/messagedialog.h>
#include <glibmm/fileutils.h>
#include <opencv2/imgproc/imgproc.hpp>
#include "material.hpp"

using namespace std;
using namespace Gtk;
using namespace Glib;
using namespace cv;

static string ui_file = "colorwindow.glade";

enum class PickStatus {
	sleep,
	awake,
	press
};

class ColorWindow {
public:
	Window* create();

private:
	//Signal handlers
	bool onScenePress(GdkEventButton* event);
	bool onSceneRelease(GdkEventButton* event);
	bool onSceneMotion(GdkEventMotion* event);
	void onSceneSizeAllocate(Allocation& allocation);
	void onPreProcessButtonClick();
	void onPickMaterialButtonToggle();
	void onColorSet();
	void onOriginButtonToggle();
	void onSaveButtonClick();

	// PreProcess
	void readMaterial();

	//Widget pointer in the UI file
	Window* mainWindow = NULL;
	EventBox* sceneEventBox = NULL;
	Image* scene = NULL;
	FileChooserButton* sceneFile = NULL;
	FileChooserButton* depthFile = NULL;
	Button* preProcessButton = NULL;
	ToggleButton* pickMaterialButton = NULL;
	ColorButton* referColor = NULL;
	ColorButton* srcColor = NULL;
	ColorButton* destColor = NULL;
	ToggleButton* originButton = NULL;
	Button* saveButton = NULL;

	//Data section
	RefPtr<Gdk::Pixbuf> srcPixbuf;
	RefPtr<Gdk::Pixbuf> destPixbuf;
	RefPtr<Gdk::Pixbuf> depthPixbuf;

	// material related
	vector<Material> library;
	Material* material = NULL;

	// pick status
	PickStatus pick;
	Point2i border;
};

#endif // _COLORWINDOW_HPP_
