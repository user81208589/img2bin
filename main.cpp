#include <QApplication>
#include "ImageCanvas.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    ImageCanvas window;
    window.show();

    return app.exec();
}
