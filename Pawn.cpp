#include "ChessPiece.h"
//#include <QCoreApplication>
class Pawn : public ChessPiece {

public:
    //override the pieceImage to a pawn image
    Pawn(QString color) : ChessPiece(QString(), "Pawn", QImage()) {
        this->color = color;
        this->label = "pawn";
        //QString imagePath = QCoreApplication::applicationDirPath() + "/../../Assets/bP.svg";
        //qDebug() << imagePath;
        QImage bPImage("../../Assets/bP.svg");
        QImage wPImage("../../Assets/wP.svg");
        color == "white" ? this->pieceImage = wPImage : pieceImage = bPImage;

    }

    // Pawn can only move vertical, based on color
    bool isValid(int startX, int startY, int endX, int endY) const override {
        if(startX != endX) {
            return false;
        }
        if(this->getColor() == "white") {
            return (startY + 1) == endY;
        }

        return (startY - 1) == endY;
    }
};
