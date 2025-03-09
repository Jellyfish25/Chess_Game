#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include "ChessPiece.h"
#include "King.cpp"
using std::unordered_map;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void handleMove(int startX, int startY, int endX, int endY);
    void updateMovesDisplay(QString pieceID, QString coords);

protected:
    //QVector<QVector<ChessPiece*>> boardState;
    QVector<QVector<std::shared_ptr<ChessPiece>>> boardState;
    //boardState(boardState(8, QVector<std::shared_ptr<ChessPiece>>(8, nullptr))

private:
    Ui::MainWindow *ui;
    QStandardItemModel *model;
    std::shared_ptr<King> whiteKing;
    std::shared_ptr<King> blackKing;
    //King *blackKing;
    //King *whiteKing;
    void updateBoardDisplay(QVector<QVector<std::shared_ptr<ChessPiece>>> boardState);
    bool isValidPath(int xStart, int yStart, int xEnd, int yEnd);
    bool isChecked(ChessPiece *movingPiece, int futureX, int futureY);
};
#endif // MAINWINDOW_H
