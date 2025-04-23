#ifndef SOCKETHANDLER_H
#define SOCKETHANDLER_H
#include <QTcpSocket>
#include <QTcpServer>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include "chessboard.h"

using std::unique_ptr;
class SocketHandler : public QObject
{
Q_OBJECT

public:
    explicit SocketHandler(QObject *parent = nullptr);
    void startServer(quint16 port);
    //void joinGame(quint16 port);
    int displayMenuOptions();
    bool isMyTurn = false;
    void setChessBoard(std::shared_ptr<ChessBoard> board);
    void closeServer();
    void joinServer(QString hostIp, quint16 port);

private:
    std::shared_ptr<ChessBoard> chessBoard;
    QTcpSocket *socket;
    QTcpServer *server;
    void receiveMove();
    bool isHost = false;

public slots:
    void newConnection();
    void sendMove(std::array<int, 2> startCoords, std::array<int, 2> endCoords);
};

#endif // SOCKETHANDLER_H
