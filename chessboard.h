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

using std::make_shared;
using std::shared_ptr;

//contains the board state and movement logic
class ChessBoard : public QObject
{
Q_OBJECT

public:
    ChessBoard():
        boardState(8, QVector<std::shared_ptr<ChessPiece>>(8, nullptr)),
        previouslyMoved(nullptr),
        turnCounter(0){}
    ~ChessBoard(){};
    void initializeBoard();

signals: //notify UI to update board & move displays
    void boardUpdated(QVector<QVector<std::shared_ptr<ChessPiece>>> boardState);
    void moveUpdated(QString pieceID, QString coords);

public slots:
    void handleMove(int startX, int startY, int endX, int endY);
    bool isPlayerTurn(int posX, int posY);

private:
    QVector<QVector<std::shared_ptr<ChessPiece>>> boardState;
    std::shared_ptr<ChessPiece> whiteKing;
    std::shared_ptr<ChessPiece> blackKing;
    shared_ptr<ChessPiece> previouslyMoved;

    bool isValidPath(int xStart, int yStart, int xEnd, int yEnd);
    bool isSafeMove(std::shared_ptr<ChessPiece> movingPiece, int endX, int endY);
    bool isCheckMate(QString color);
    bool isStaleMate(QString color);
    bool isPieceInCheck(std::shared_ptr<ChessPiece> piece);

    int possibleMoves(QVector<shared_ptr<ChessPiece>> pieceList);
    void updateDisplay(std::shared_ptr<ChessPiece> movingPiece);

    int turnCounter;
};
#endif // CHESSBOARD_H
