#include "ChessPiece.h"

class Pawn : public ChessPiece {
public:
    //override the pieceImage to a pawn image
    Pawn(QString color, int x, int y) : ChessPiece(color, "Pawn", QImage(), x, y) {
        //QString imagePath = QCoreApplication::applicationDirPath() + "/../../Assets/bP.svg";
        //qDebug() << imagePath;
        QString imagePath = color == "white" ? "../../Assets/wP.svg" : "../../Assets/bP.svg";
        if(!this->pieceImage.load(imagePath)) {
            qWarning() << "Error: Failed to load image resource for Pawn:" << color;
        }
    }

    // Pawn can only move vertical, based on color
    bool isValidMove(int startX, int startY, int endX, int endY) const override {
        if(startY != endY) {
            return false;
        }
        if(this->getColor() == "black") {
            return (startX + 2) == endX || (startX + 1) == endX;
        }

        return (startX - 2) == endX || (startX - 1) == endX;
    }
};
