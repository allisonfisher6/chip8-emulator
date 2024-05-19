#include "chip8display.h"
#include "./ui_chip8display.h"
#include <QPixmap>
Chip8Display::Chip8Display(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Chip8Display)
{
    ui->setupUi(this);
    QPixmap pixMap(640, 320);
    pixMap.fill(Qt::black);
    ui->labelDisplay->setPixmap(pixMap);
}

Chip8Display::~Chip8Display()
{
    delete ui;
}

