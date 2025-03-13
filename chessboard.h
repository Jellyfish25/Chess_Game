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
        previouslyMoved(nullptr) {}
    ~ChessBoard(){};
    void initializeBoard();

signals: //notify UI to update board & move displays
    void boardUpdated(QVector<QVector<std::shared_ptr<ChessPiece>>> boardState);
    void moveUpdated(QString pieceID, QString coords);

public slots:
    void handleMove(int startX, int startY, int endX, int endY);

private:
    QVector<QVector<std::shared_ptr<ChessPiece>>> boardState;
    std::shared_ptr<King> whiteKing;
    std::shared_ptr<King> blackKing;
    //void updateBoardDisplay(QVector<QVector<std::shared_ptr<ChessPiece>>> boardState);
    bool isValidPath(int xStart, int yStart, int xEnd, int yEnd);
    bool isSafeMove(std::shared_ptr<ChessPiece> movingPiece, int endX, int endY);
    bool isCheckMate(QString color);

    shared_ptr<ChessPiece> previouslyMoved;
};
#endif // CHESSBOARD_H
