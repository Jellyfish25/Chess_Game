#include "ChessPiece.h"

class Knight : public ChessPiece {
public:
    Knight(QString color, int x, int y) : ChessPiece(color, "Knight", QImage(), x, y) {
        QString imagePath = color == "white" ? "../../Assets/wN.svg" : "../../Assets/bN.svg";
        if(!this->pieceImage.load(imagePath)) {
            qWarning() << "Error: Failed to load image resource for Knight:" << color;
        }
    }

    //L shaped movement
    bool isValidMove(int xStart, int yStart, int xEnd, int yEnd) const override {
        int dx = abs(xStart - xEnd);
        int dy = abs(yStart - yEnd);
        return (dx == 2 && dy == 1) || (dx == 1 && dy == 2);
    }
};
