#include "chesspiece.h"

class Bishop : public ChessPiece {
public:
    Bishop(QString color, int x, int y) : ChessPiece(color, "Bishop", QImage(), x, y) {
        QString imagePath = color == "white" ? "../../Assets/wB.svg" : "../../Assets/bB.svg";
        if(!this->pieceImage.load(imagePath)) {
            qWarning() << "Error: Failed to load image resource for Bishop:" << color;
        }
    }

    //diagonal movement, slope should be 1
    //fix this logic
    bool isValidMove(int xStart, int xEnd, int yStart, int yEnd) const override {
        bool val = abs(xStart - yEnd) == abs(yStart - xEnd) || abs(xStart - xEnd) == abs(yStart - yEnd);
        qDebug() << "is valid bishop move: " << val;
        return abs(xStart - yEnd) == abs(yStart - xEnd) || abs(xStart - xEnd) == abs(yStart - yEnd);
    }
};
