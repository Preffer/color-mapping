#include <iostream>
#include "colorwindow.h"

using namespace std;
using namespace Gtk;
using namespace Glib;

Window* ColorWindow::create() {
	RefPtr<Builder> builder;

	try {
		builder = Builder::create_from_file(ui_file);
	} catch (const FileError& e) {
		cerr << e.what() << endl;
		throw;
	}

	builder->get_widget("mainWindow", mainWindow);
	builder->get_widget("referColor", referColor);
	builder->get_widget("srcColor",   srcColor);
	builder->get_widget("destColor",  destColor);

	referColor->signal_color_set().connect(sigc::mem_fun(*this, &ColorWindow::onReferColorSet));
	srcColor->signal_color_set().connect(sigc::mem_fun(*this, &ColorWindow::onSrcColorSet));
	destColor->signal_color_set().connect(sigc::mem_fun(*this, &ColorWindow::onDestColorSet));

	return mainWindow;
}

/* Signal handler */
void ColorWindow::onReferColorSet() {
	Gdk::RGBA rgba = referColor->get_rgba();
	//cout << rgba << endl;
	cout << "color set" << endl;
}

void ColorWindow::onSrcColorSet() {
	cout << "color set" << endl;
}

void ColorWindow::onDestColorSet() {
	cout << "color set" << endl;
}
