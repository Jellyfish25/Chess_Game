#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QLabel>
#include <QStandardItemModel>
#include "Pawn.cpp"
#include "Rook.cpp"
#include "Knight.cpp"
#include "Bishop.cpp"
#include <QVector>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->gridLayout->setSpacing(0);
    ui->gridLayout->setContentsMargins(0, 0, 0, 0);

    //generate pieces and store them in a container
    //each piece will have x,y coordinates correlated to a piece
    //note: initialize 2d vector boardstate of size 8x8 with null values
    QVector<QVector<ChessPiece*>> boardState(8, QVector<ChessPiece*>(8, nullptr));
    //insert pawns
    for(int col = 0; col < 8; col++) {
        boardState[1][col] = new Pawn("black");
        boardState[6][col] = new Pawn("white");
    }

    //insert rooks
    boardState[0][0] = new Rook("black");
    boardState[0][7] = new Rook("black");
    boardState[7][0] = new Rook("white");
    boardState[7][7] = new Rook("white");

    //todo: Make classes for each piece and insert it into the boardState
    //insert knights
    boardState[0][1] = new Knight("black");
    boardState[0][6] = new Knight("black");
    boardState[7][1] = new Knight("white");
    boardState[7][6] = new Knight("white");
    //insert bishops
    boardState[0][2] = new Bishop("black");
    boardState[0][5] = new Bishop("black");
    boardState[7][2] = new Bishop("white");
    boardState[7][5] = new Bishop("white");

    //insert rooks

    //insert queen

    //insert king

    initializeBoard(boardState);
    updateSteps();
}

void MainWindow::initializeBoard(QVector<QVector<ChessPiece*>> boardState) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            QLabel *label = new QLabel();
            label->setFixedSize(40, 40);
            if ((i + j) % 2 != 0) {
                label->setStyleSheet("background-color: rgb(242, 111, 246);");
            } else {
                label->setStyleSheet("background-color: white;");
            }

            if(boardState[i][j] != nullptr) {
                QPixmap pieceImage = QPixmap::fromImage(boardState[i][j]->getImage());
                label->setPixmap(pieceImage.scaled(40,40, Qt::KeepAspectRatio));
            }
            ui->gridLayout->addWidget(label, i, j);
        }
    }
}

void MainWindow::updateSteps() {
    // testing function to display current steps
    QStandardItemModel *model = new QStandardItemModel();
    for(int i = 0; i < 5; i++) {
        QStandardItem *currentMove = new QStandardItem(QString("test"));
        currentMove->setFlags(currentMove->flags() & ~Qt::ItemIsEditable);
        model->appendRow(currentMove);
    }
    ui->listView->setModel(model);
}

MainWindow::~MainWindow()
{
    delete ui;
}

