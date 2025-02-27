#include "ChessPiece.h"

ChessPiece::ChessPiece(QString color, QString label, QImage pieceImage) {
    this->color = color;
    this->label = label;
    this->pieceImage = pieceImage;
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
