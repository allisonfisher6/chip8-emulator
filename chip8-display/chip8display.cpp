#include "chip8display.h"
#include "./ui_chip8display.h"

Chip8Display::Chip8Display(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Chip8Display)
{
    ui->setupUi(this);
}

Chip8Display::~Chip8Display()
{
    delete ui;
}
