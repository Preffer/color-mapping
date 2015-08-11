#include <iostream>
#include <boost/format.hpp>
#include "util.hpp"
#include "colorwindow.hpp"

using namespace std;
using namespace boost;
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
	builder->get_widget("scene", scene);
	builder->get_widget("sceneFile", sceneFile);
	builder->get_widget("depthFile", depthFile);
	builder->get_widget("preProcessButton", preProcessButton);
	builder->get_widget("referColor", referColor);
	builder->get_widget("srcColor",   srcColor);
	builder->get_widget("destColor",  destColor);

	preProcessButton->signal_clicked().connect(sigc::mem_fun(*this, &ColorWindow::onPreProcessButtonClick));
	referColor->signal_color_set().connect(sigc::mem_fun(*this, &ColorWindow::onReferColorSet));
	srcColor->signal_color_set().connect(sigc::mem_fun(*this, &ColorWindow::onSrcColorSet));
	destColor->signal_color_set().connect(sigc::mem_fun(*this, &ColorWindow::onDestColorSet));

	return mainWindow;
}

/* Signal handler */
void ColorWindow::onPreProcessButtonClick() {
	cout << format("onPreProcessButtonClick: sceneFile => %1%, depthFile => %2%") % sceneFile->get_filename() % depthFile->get_filename() << endl;

	scene->set(sceneFile->get_filename());
	destPixbuf = scene->get_pixbuf();
	srcPixbuf = destPixbuf->copy();
}

void ColorWindow::onReferColorSet() {
	cout << "onReferColorSet: " << referColor->get_rgba() << endl;
}

void ColorWindow::onSrcColorSet() {
	cout << "onSrcColorSet: " << srcColor->get_rgba() << endl;
}

void ColorWindow::onDestColorSet() {
	cout << "onDestColorSet: " << destColor->get_rgba() << endl;
}
