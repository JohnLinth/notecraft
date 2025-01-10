#include "homepage.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HomePage nl;

    // Set the background color
    QPalette palette = a.palette();
    palette.setColor(QPalette::Window, QColor(224, 255, 255)); // Light Cyan
    a.setPalette(palette);

    nl.show();
    return a.exec();
}
