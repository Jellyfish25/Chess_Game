#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include "ChessPiece.h"
using std::unordered_map;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void handleMove(int startX, int startY, int endX, int endY);
    void updateMovesDisplay(QString pieceID, QString coords);

protected:
    QVector<QVector<ChessPiece*>> boardState;

private:
    Ui::MainWindow *ui;
    QStandardItemModel *model;
    void updateBoardDisplay(QVector<QVector<ChessPiece*>> &boardState);
    //unordered_map<QString, QString> pieceNameMap;
};
#endif // MAINWINDOW_H
