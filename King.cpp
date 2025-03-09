#include "ChessPiece.h"

class King : public ChessPiece {
public:
    King(QString color, int x, int y) : ChessPiece(color, "King", QImage(), x, y) {
        QString imagePath = color == "white" ? "../../Assets/wK.svg" : "../../Assets/bK.svg";
        if(!this->pieceImage.load(imagePath)) {
            qWarning() << "Error: Failed to load image resource for King:" << color;
        }
        this->isChecked = false;
    }
    bool isChecked;

    //can move only one tile
    bool isValidMove(int xStart, int yStart, int xEnd, int yEnd) const override {
        int dX = abs(xStart - xEnd);
        int dY = abs(yStart - yEnd);
        return dX <= 1 && dY <= 1;
    }
};
