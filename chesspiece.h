#ifndef CHESSPIECE_H
#define CHESSPIECE_H
#include <QString>
#include <QImage>
#include <QDebug>

class ChessPiece
{

public:
    ChessPiece(QString color, QString label, QImage pieceImage);
    virtual ~ChessPiece() {}; //virtual deconstructor
    QString getColor() const;
    QString getLabel() const;
    QImage getImage() const;
    virtual bool isValid(int startX, int startY, int endX, int endY) const = 0;
    int prevX;
    int prevY;
    int currX;
    int currY;

protected:
    QString color;
    QString label;
    QImage pieceImage;
};

#endif // CHESSPIECE_H
