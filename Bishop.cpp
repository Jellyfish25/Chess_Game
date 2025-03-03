#include "chesspiece.h"

class Bishop : public ChessPiece {
public:
    Bishop(QString color) : ChessPiece(color, "Bishop", QImage()) {
        QString imagePath = color == "white" ? "../../Assets/wB.svg" : "../../Assets/bB.svg";
        if(!this->pieceImage.load(imagePath)) {
            qWarning() << "Error: Failed to load image resource for Bishop:" << color;
        }
    }

    //diagonal movement, slope should be 1
    bool isValid(int xStart, int xEnd, int yStart, int yEnd) const override {
        return abs(xStart - yEnd) == abs(yStart - xEnd) || abs(xStart - xEnd) == abs(yStart - yEnd);
    }
};
