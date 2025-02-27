#include "mainwindow.h"
#include <QApplication>
#include "Pawn.cpp"

int main(int argc, char *argv[])
{
    //can also just call Pawn pawnStars("white") to allocate it on stack instead of heap
    //dont have to manually delete the object
    // Pawn *pawnStars = new Pawn("white");
    // qDebug() << pawnStars->getColor() << Qt::endl;
    // qDebug() << pawnStars->isValid(0, 0, 0, 1) << Qt::endl;
    // qDebug() << pawnStars->getImage().isNull();
    // delete pawnStars;

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
