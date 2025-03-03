#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QStandardItemModel>
#include <QVector>
#include "Pawn.cpp"
#include "Rook.cpp"
#include "Knight.cpp"
#include "Bishop.cpp"
#include "King.cpp"
#include "Queen.cpp"
#include "DraggableLabel.h"
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , boardState(8, QVector<ChessPiece*>(8, nullptr))
{
    ui->setupUi(this);
    ui->gridLayout->setSpacing(0);
    ui->gridLayout->setContentsMargins(0, 0, 0, 0);
    this->model = new QStandardItemModel();

    //generate pieces and store them in a container
    //each piece will have x,y coordinates correlated to a piece
    //note: initialize 2d vector boardstate of size 8x8 with null values
    //QVector<QVector<ChessPiece*>> boardState(8, QVector<ChessPiece*>(8, nullptr));
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

    //insert queens
    boardState[0][3] = new Queen("black");
    boardState[7][3] = new Queen("white");

    //insert kings
    boardState[0][4] = new King("black");
    boardState[7][4] = new King("white");
    updateBoardDisplay(boardState);
}

void MainWindow::updateBoardDisplay(QVector<QVector<ChessPiece*>> &boardState) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            QLabel *tile = new QLabel();
            DraggableLabel *label = new DraggableLabel(this, this, ui->gridLayout);  // Pass 'this' as MainWindow
            label->setProperty("posX", i);
            label->setProperty("posY", j);
            label->setFixedSize(40, 40);
            label->setAlignment(Qt::AlignCenter);

            label->setAcceptDrops(true);
            label->setBoardState(&boardState);

            if ((i + j) % 2 != 0) {
                tile->setStyleSheet("background-color: rgb(242, 111, 246);");
            } else {
                tile->setStyleSheet("background-color: white;");
            }

            if(boardState[i][j] != nullptr) {
                QPixmap pieceImage = QPixmap::fromImage(boardState[i][j]->getImage());
                label->setPixmap(pieceImage.scaled(40,40, Qt::KeepAspectRatio));
            }
            ui->gridLayout->addWidget(tile, i, j);
            ui->gridLayout->addWidget(label, i, j);
        }
    }
}

void MainWindow::handleMove(int startX, int startY, int endX, int endY) {
    //verify that the new coordinates are within the boardState
    if(endX >= boardState.length() || endY >= boardState[0].length()) {
        return;
    }
    // 1. Validate the move (based on piece type, current position, and destination)

    ChessPiece *movingPiece = boardState[startX][startY];
    if (movingPiece && movingPiece->isValid(startX, startY, endX, endY)) {
        //prevent from being able to capture same colored pieces
        if(boardState[endX][endY] != nullptr) {
            if(boardState[endX][endY]->getColor() == movingPiece->getColor()) {
                return;
            }
        }
        boardState[endX][endY] = movingPiece;
        boardState[startX][startY] = nullptr;
        updateBoardDisplay(boardState);
        movingPiece->prevX = startX;
        movingPiece->prevY = startY;
        movingPiece->currX = endX;
        movingPiece->currY = endY;

        //when moving the piece, need to verify that boardState[endX][endY] is empty, contains an opposing color,
        //and is not blocked by the same color in the path
        /*special cases:
         * 1. pawn: if moving two tiles, gives the opponent the opportunity for en passant
         *      - opposite piece needs to be next to current piece (check left/right of pawns)
         *      - reaching the opposite side of the board converts to queen
         * 2. knight: can go over pieces, ignoring having to check if the path is blocked
         * 3. king: on capture, game is over
         * for all pieces: on next move, check if there is an obstacle between (startX, startY) -> (endX, endY)
        */
        if(movingPiece->getLabel() == "Pawn" && abs(movingPiece->prevX - endX) == 2) {
            qDebug() << "En passant opportunity";
        }
        char charVal = endY + 'a';
        char numVal = endX + '0';
        QString coords = QString("(" + QString(charVal) + QString(numVal) + ")");
        QString pieceID = QString(movingPiece->getColor() + " " + movingPiece->getLabel());
        updateMovesDisplay(pieceID, coords);
    }
}

void MainWindow::updateMovesDisplay(QString pieceID, QString coords) {
    // testing function to display current steps
    QStandardItem *currentMove = new QStandardItem(pieceID + " " + coords);
    currentMove->setFlags(currentMove->flags() & ~Qt::ItemIsEditable);
    model->appendRow(currentMove);
    ui->listView->setModel(model);
}

MainWindow::~MainWindow()
{
    delete ui;
}

