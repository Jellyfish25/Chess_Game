#include "ChessPiece.h"

ChessPiece::ChessPiece(QString color, QString label, QImage pieceImage, int x, int y) {
    this->color = color;
    this->label = label;
    this->pieceImage = pieceImage;
    this->currPos[0] = x;
    this->currPos[1] = y;
}

QString ChessPiece::getColor() const {
    return color;
}

QString ChessPiece::getLabel() const {
    return label;
}

QImage ChessPiece::getImage() const {
    return pieceImage;
}
