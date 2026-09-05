#include <QApplication>
#include "gamelogic.h"
#include "gamewidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Gamelogic logic;
    Gamewidget w(logic);

    return app.exec();
}
