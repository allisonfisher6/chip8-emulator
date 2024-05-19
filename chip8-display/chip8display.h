#ifndef CHIP8DISPLAY_H
#define CHIP8DISPLAY_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class Chip8Display;
}
QT_END_NAMESPACE

class Chip8Display : public QMainWindow
{
    Q_OBJECT

public:
    Chip8Display(QWidget *parent = nullptr);
    ~Chip8Display();

private:
    Ui::Chip8Display *ui;
};
#endif // CHIP8DISPLAY_H
