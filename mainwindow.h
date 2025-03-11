#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include "ChessPiece.h"
#include "ChessBoard.h"

using std::shared_ptr;
using std::unique_ptr;
using std::make_shared;

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

public slots:
    void updateBoardDisplay(QVector<QVector<std::shared_ptr<ChessPiece>>> boardState);
    void updateMovesDisplay(QString pieceID, QString coords);

protected:
    QVector<QVector<std::shared_ptr<ChessPiece>>> boardState;

private:
    Ui::MainWindow *ui;
    QStandardItemModel *model;
    std::shared_ptr<ChessBoard> chessBoard;
};
#endif // MAINWINDOW_H
