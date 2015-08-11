#include "colorwindow.hpp"

using namespace std;
using namespace Gtk;
using namespace Glib;

int main(int argc, char *argv[]) {
	RefPtr<Application> app = Application::create(argc, argv, "com.qunhe.colormap");
	ColorWindow mainWindow;

	return app->run(*(mainWindow.create()));
}
