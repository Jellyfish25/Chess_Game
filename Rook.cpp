#include "ChessPiece.h"

class Rook : public ChessPiece {
public:
    Rook(QString color, int x, int y) : ChessPiece(color, "Rook", QImage(), x, y) {
        QString imagePath = color == "white" ? "../../Assets/wR.svg" : "../../Assets/bR.svg";
        if (!this->pieceImage.load(imagePath)) {
            qWarning() << "Error: Failed to load image resource for Rook:" << color;
        }
    }

    //Rook can only move horizontal and vertical
    bool isValid(int startX, int startY, int endX, int endY) const override {
        return (startX == endX) || (startY == endY);
    }
};
