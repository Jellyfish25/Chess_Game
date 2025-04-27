#ifndef SOCKETHANDLER_H
#define SOCKETHANDLER_H
#include <QTcpSocket>
#include <QTcpServer>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include "chessboard.h"

using std::unique_ptr;
constexpr int kDelayTime = 1000; // kDelayTime is in ms
class SocketHandler : public QObject
{
Q_OBJECT

public:
    explicit SocketHandler(QObject *parent = nullptr, shared_ptr<ChessBoard> chessBoard = nullptr);
    void startServer(quint16 port);
    //void joinGame(quint16 port);
    int displayMenuOptions();
    void setChessBoard(std::shared_ptr<ChessBoard> board);
    void closeServer();
    void joinServer(QString hostIp, quint16 port);
    void initializeClientColor();

private:
    std::shared_ptr<ChessBoard> chessBoard;
    QString playerColor;
    QTcpSocket *socket;
    QTcpServer *server;
    void receiveNetData();
    bool isValidInput(bool isClient, QLineEdit &ipInput, QLineEdit &portInput);
    bool isHost = false;

public slots:
    void newConnection();
    void sendNetData(std::array<int, 2> startCoords, std::array<int, 2> endCoords, QString promoType);
};

#endif // SOCKETHANDLER_H
