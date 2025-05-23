#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QVector>
#include <QApplication>
#include <memory>
#include "Pawn.cpp"
#include "Rook.cpp"
#include "Knight.cpp"
#include "Bishop.cpp"
#include "Queen.cpp"
#include "King.cpp"
#include "./ui_mainwindow.h"

using std::make_shared;
using std::shared_ptr;
using std::unique_ptr;

//contains the board state and movement logic
class ChessBoard : public QObject
{
Q_OBJECT

public:
ChessBoard(QMainWindow* parentWindow, Ui::MainWindow* parentUi, QPushButton* &promoRef):
        boardState(8, QVector<std::shared_ptr<ChessPiece>>(8, nullptr)),
        previouslyMoved(nullptr),
        turnCounter(0),
        mainWindow(parentWindow),
        ui(parentUi),
        promoRef(promoRef){}

    ~ChessBoard(){};
    void initializeBoard();
    QMainWindow *mainWindow;
    Ui::MainWindow *ui;
    QPushButton* &promoRef;
    QEventLoop loop;

    // player state (online connectivity)
    QString playerColor;
    bool isLocal = false;

signals:
    // notify UI to update board & move displays
    void boardUpdated(QVector<QVector<std::shared_ptr<ChessPiece>>> boardState);
    void moveUpdated(QString pieceID, QString startCoords, QString endCoords);
    void gameWinner(QString playerColor);
    // sends the current move's coordinates to the socket handler
    void sendLocalData(std::array<int, 2> startCoords, std::array<int, 2> endCoords, QString promoType);

public slots:
    void handleMove(int startX, int startY, int endX, int endY, bool fromNetwork = false, QString promoType = "");
    bool isPlayerTurn(int posX, int posY);

private:
    QVector<QVector<std::shared_ptr<ChessPiece>>> boardState;
    std::shared_ptr<ChessPiece> whiteKing;
    std::shared_ptr<ChessPiece> blackKing;
    shared_ptr<ChessPiece> previouslyMoved;
    shared_ptr<ChessPiece> createPromoPiece(int endX, int endY, QString color, QString pieceType);

    bool isValidPath(int xStart, int yStart, int xEnd, int yEnd);
    bool isSafeMove(std::shared_ptr<ChessPiece> movingPiece, int endX, int endY);
    bool isCheckMate(QString color);
    bool isStaleMate(QString color);
    bool isPieceInCheck(std::shared_ptr<ChessPiece> piece);

    int possibleMoves(QVector<shared_ptr<ChessPiece>> pieceList);
    void updateDisplay(std::shared_ptr<ChessPiece> movingPiece);
    void promoNotification(int row, int col);
    int turnCounter;
};
#endif // CHESSBOARD_H
