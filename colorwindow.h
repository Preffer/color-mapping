#ifndef _COLORWINDOW_H_
#define _COLORWINDOW_H_

#include <gtkmm/window.h>
#include <gtkmm/builder.h>
#include <gtkmm/label.h>
#include <gtkmm/colorbutton.h>
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
		void onReferColorSet();
		void onSrcColorSet();
		void onDestColorSet();

		//Widget pointer in the UI file
		Window* mainWindow = NULL;		
		ColorButton* referColor = NULL;
		ColorButton* srcColor = NULL;
		ColorButton* destColor = NULL;
};

#endif // _COLORWINDOW_H_
