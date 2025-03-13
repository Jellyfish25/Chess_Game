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
    // note: can only move two tiles forward on it's first move
    bool isValidMove(int startX, int startY, int endX, int endY) const override {
        if(startY != endY) {
            return false;
        }

        //can move one or two tiles on first move
        if(this->getPrevPos()[0] == -1) {
            if(this->getColor() == "black") {
                return (startX + 2) == endX || (startX + 1) == endX;
            }
            return (startX - 2) == endX || (startX - 1) == endX;
        }

        //after first move, can only move one tile
        if(this->getColor() == "black") {
            return (startX + 1) == endX;
        }

        return (startX - 1) == endX;
    }
};
