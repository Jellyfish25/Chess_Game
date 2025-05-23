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
    bool isValidMove(int xStart, int xEnd, int yStart, int yEnd) const override {
        return abs(xStart - yStart) == abs(xEnd - yEnd);
    }
};
