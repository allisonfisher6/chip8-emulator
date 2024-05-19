#include "chip8display.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Chip8Display w;
    w.show();
    return a.exec();
}
