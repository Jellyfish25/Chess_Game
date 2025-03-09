#ifndef CHESSPIECE_H
#define CHESSPIECE_H
#include <QString>
#include <QImage>
#include <QDebug>

class ChessPiece {

public:
    ChessPiece(QString color, QString label, QImage pieceImage, int x, int y);
    virtual ~ChessPiece() {}; //virtual deconstructor
    virtual bool isValidMove(int startX, int startY, int endX, int endY) const = 0;

    QString getColor() const;
    QString getLabel() const;
    QImage getImage() const;    
    std::array<int, 2> getCurrPos() const { return currPos; }
    std::array<int, 2> getPrevPos() const { return prevPos; }
    void setCoordinates(int newX, int newY) {
        prevPos[0] = currPos[0];
        prevPos[1] = currPos[1];
        currPos[0] = newX;
        currPos[1] = newY;
    }

protected:
    QString color;
    QString label;
    QImage pieceImage;

private:
    std::array<int, 2> prevPos = {-1, -1};
    std::array<int, 2> currPos = {-1, -1};
};

#endif // CHESSPIECE_H
