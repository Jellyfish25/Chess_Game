#include "ChessPiece.h"

class Pawn : public ChessPiece {
public:
    //override the pieceImage to a pawn image
    Pawn(QString color) : ChessPiece(color, "Pawn", QImage()) {
        //QString imagePath = QCoreApplication::applicationDirPath() + "/../../Assets/bP.svg";
        //qDebug() << imagePath;
        QString imagePath = color == "white" ? "../../Assets/wP.svg" : "../../Assets/bP.svg";
        if(!this->pieceImage.load(imagePath)) {
            qWarning() << "Error: Failed to load image resource for Pawn:" << color;
        }
    }

    // Pawn can only move vertical, based on color
    bool isValid(int startX, int startY, int endX, int endY) const override {
        if(startX != endX) {
            return false;
        }
        if(this->getColor() == "black") {
            return (startY + 2) == endY || (startY + 1) == endY;
        }

        return (startY - 2) == endY || (startY - 1) == endY;
    }
};
