#import "ChessPiece.h"

class King : public ChessPiece {
public:
    King(QString color) : ChessPiece(color, "King", QImage()) {
        QString imagePath = color == "white" ? "../../Assets/wK.svg" : "../../Assets/bK.svg";
        if(!this->pieceImage.load(imagePath)) {
            qWarning() << "Error: Failed to load image resource for King:" << color;
        }
    }

    //can move only one tile
    bool isValid(int xStart, int yStart, int xEnd, int yEnd) const override {
        int dX = abs(xStart - xEnd);
        int dY = abs(yStart - yEnd);
        return dX <= 1 && dY <= 1;
    }
};
