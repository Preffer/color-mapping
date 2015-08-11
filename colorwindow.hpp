#ifndef _COLORWINDOW_HPP_
#define _COLORWINDOW_HPP_

#include <gtkmm/window.h>
#include <gtkmm/builder.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/colorbutton.h>
#include <gtkmm/filechooserbutton.h>
#include <glibmm/fileutils.h>

using namespace std;
using namespace Gtk;
using namespace Glib;

static string ui_file = "colorwindow.glade";

class ColorWindow {
	public:
		Window* create();

	protected:
		//Signal handlers
		void onPreProcessButtonClick();
		void onReferColorSet();
		void onSrcColorSet();
		void onDestColorSet();

		//Widget pointer in the UI file
		Window* mainWindow = NULL;
		Image* scene = NULL;
		FileChooserButton* sceneFile = NULL;
		FileChooserButton* depthFile = NULL;
		Button* preProcessButton = NULL;
		ColorButton* referColor = NULL;
		ColorButton* srcColor = NULL;
		ColorButton* destColor = NULL;

		//Data section
		RefPtr<Gdk::Pixbuf> srcPixbuf;
		RefPtr<Gdk::Pixbuf> destPixbuf;
};

#endif // _COLORWINDOW_H_
