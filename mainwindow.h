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
    QPushButton *promoButton = nullptr;
    std::shared_ptr<ChessBoard> getBoard() {return chessBoard;};

public slots:
    void updateBoardDisplay(QVector<QVector<std::shared_ptr<ChessPiece>>> boardState);
    void updateMovesDisplay(QString pieceID, QString startCoords, QString endCoords);

protected:
    QVector<QVector<std::shared_ptr<ChessPiece>>> boardState;

private:
    Ui::MainWindow *ui;
    QStandardItemModel *model;
    std::shared_ptr<ChessBoard> chessBoard;
    void pawnPromoDisplay(QString color);
    void menuDisplay();
    const QString buttonStyleSheet =
        "QPushButton {"
        "   background-color: white;"
        "   border: none;"
        "   padding: 10px;"
        "   border-radius: 5px;"
        "}"

        "QPushButton:hover {"
        "   background-color: orange;"
        "}";
};
#endif // MAINWINDOW_H
