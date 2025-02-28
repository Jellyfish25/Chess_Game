#import "ChessPiece.h"

class Queen : public ChessPiece {
public:
    Queen(QString color) : ChessPiece(color, "Queen", QImage()) {
        QString imagePath = color == "white" ? "../../Assets/wQ.svg" : "../../Assets/bQ.svg";
        if (!this->pieceImage.load(imagePath)) {
            qWarning() << "Error: Failed to load image resource for Rook:" << color;
        }
    }

    // can traverse diagonal + horizontal
    bool isValid(int xStart, int yStart, int xEnd, int yEnd) const override {
        int dX = abs(xStart - xEnd);
        int dY = abs(yStart - yEnd);
        return (dX == dY) || xStart == xEnd || yStart == yEnd;
    }
};
