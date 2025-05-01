#include "mainwindow.h"
#include "sockethandler.h"
#include <QApplication>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // mainwindow constructs the board and displays the moves played
    MainWindow w;
    // initialize the socket handler to connect & set the board state
    auto handler = SocketHandler(&w, w.getBoard());
    if(handler.displayMenuOptions() != QDialog::Accepted) {
        return 0;
    }

    // display the main window
    w.show();

    // on exit, close the server
    QObject::connect(&a, &QApplication::aboutToQuit, &handler, &SocketHandler::closeServer);
    return a.exec();
}
