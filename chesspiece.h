#ifndef CHESSPIECE_H
#define CHESSPIECE_H
#include <QString>
#include <QImage>

class ChessPiece
{

public:
    ChessPiece(QString color, QString label, QImage pieceImage);
    QString getColor() const;
    QString getLabel() const;
    QImage getImage() const;
    virtual bool isValid(int startX, int startY, int endX, int endY) const = 0;

protected:
    QString color;
    QString label;
    QImage pieceImage;
};

#endif // CHESSPIECE_H
