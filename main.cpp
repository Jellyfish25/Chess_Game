#include "mainwindow.h"
#include "sockethandler.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // initialize the handler
    // note: don't initialize mainwindow/chessboard if the user exits the application
    SocketHandler handler;
    if(handler.displayMenuOptions() != QDialog::Accepted) {
        return 0;
    }

    // display the main window (chess logic)
    // mainwindow constructs the board and displays the moves played
    MainWindow w;

    // give the sockethandler a reference to the chessboard to relay moves
    handler.setChessBoard(w.getBoard());
    w.show();

    // on exit, close the server
    QObject::connect(&a, &QApplication::aboutToQuit, &handler, &SocketHandler::closeServer);
    return a.exec();
}
