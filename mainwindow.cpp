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
    , model(new QStandardItemModel())
    , boardState(8, QVector<ChessPiece*>(8, nullptr))
{
    ui->setupUi(this);
    ui->gridLayout->setSpacing(0);
    ui->gridLayout->setContentsMargins(0, 0, 0, 0);
    //todo: make GUI scrollable + dynamically scale

    //generate pieces and store them in a container
    //each piece will have x,y coordinates correlated to a piece
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

    //initialize board
    updateBoardDisplay(boardState);
}

//paints the board state on update, can be further optimized
//note: initialize board (64), then update only two tiles (2) instead of all tiles (64)
void MainWindow::updateBoardDisplay(QVector<QVector<ChessPiece*>> &boardState) {
    QLayoutItem *item;
    while ((item = ui->gridLayout->takeAt(0)) != nullptr) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater();  // Schedules widget for deletion
        }
        delete item;
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            QLabel *tile = new QLabel();
            DraggableLabel *label = new DraggableLabel(this, this);  // Pass 'this' as MainWindow
            label->setProperty("posX", i);
            label->setProperty("posY", j);
            label->setFixedSize(40, 40);
            label->setAlignment(Qt::AlignCenter);
            label->setAcceptDrops(true);

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

    bool isValidMove = false;
    //verify the move is valid(based on piece type, current position, and destination)
    ChessPiece *movingPiece = boardState[startX][startY];
    if (movingPiece && movingPiece->isValid(startX, startY, endX, endY)) {
        //prevent pieces from being able to cross existing pieces on that path
        if(!verifyPath(startX, startY, endX, endY)) {
            qDebug() << "invalid move: current piece is blocked";
            return;
        }

        //update the boardState and movingPiece (note, with smart pointers, don't have to manually delete).
        //delete the piece at end point
        if(boardState[endX][endY] != nullptr) {
            delete boardState[endX][endY];
        }
        isValidMove = true;

        //when moving the piece, need to verify that boardState[endX][endY] is empty, contains an opposing color,
        //and is not blocked by the same color in the path
        /*special cases:
         * 3. king: on capture, game is over
         * 4. Castle:
         *      1. verify king & castle has not moved (bool hasMoved in King & Rook class)
         *      2. no pieces are between (verifyPath, create condition for when end point is rook
         *      3. and king and rook is not in check (examine state of the board, seeing if any opposing piece can check)
         * 5. if king is in check, can only move king (player controller state: bool isCheck)
         *
         * IMPLEMENTED:
         * 1. pawn: en passant & queen
         * 2. knight: can go over pieces, ignoring having to check if the path is blocked
         * for all pieces: on next move, check if there is an obstacle between (startX, startY) -> (endX, endY)
        */
    }
    else if(movingPiece && movingPiece->getLabel() == "Pawn" && model->rowCount() > 0) {
        QString previousMove = model->item(model->rowCount() - 1)->text();
        QStringList moveList = previousMove.split(" ");
        QString previousColor = moveList[0];
        QString previousPiece = moveList[1];
        int col = moveList[2][1].unicode() - 'a';
        int row = abs(moveList[2][2].unicode() - '1' - 7);

        //check that the previous piece is a different color and is a pawn
        if(previousColor != movingPiece->getColor() && previousPiece == "Pawn") {
            ChessPiece *previousPawn = boardState[row][col];
            //check if the previous piece exists and it has moved 2 tiles (opening up for enpassant)
            if(previousPawn && !boardState[endX][endY] && abs(previousPawn->currX - previousPawn->prevX) == 2) {
                //check if the move point is valid
                if(previousPawn->currX == startX && previousPawn->currY == endY) {
                    if((movingPiece->getColor() == "white" && startX == endX + 1) ||
                        (movingPiece->getColor() == "black" && startX == endX - 1)) {
                        //capture pawn
                        delete boardState[row][col];
                        boardState[row][col] = nullptr;
                        isValidMove = true;
                    }
                }
            }
        }
    }

    if(isValidMove) {
        //update the current piece's location
        movingPiece->prevX = startX;
        movingPiece->prevY = startY;
        movingPiece->currX = endX;
        movingPiece->currY = endY;

        //pawn becomes queen if it reaches the end of the board
        if(movingPiece->getLabel() == "Pawn" && endX == boardState.size() - 1 || endX == 0) {
            QString color = movingPiece->getColor();
            delete movingPiece;
            boardState[endX][endY] = nullptr;
            movingPiece = new Queen(color);
        }
        boardState[endX][endY] = movingPiece;
        boardState[startX][startY] = nullptr;

        //display the current move taken
        char charVal = endY + 'a';
        char numVal = abs(endX - 7) + '1';
        QString coords = QString("(" + QString(charVal) + QString(numVal) + ")");
        QString pieceID = QString(movingPiece->getColor() + " " + movingPiece->getLabel());

        //update board & move display
        updateBoardDisplay(boardState);
        updateMovesDisplay(pieceID, coords);
    }
}

// update what was currently played in the "Played Moves" window
void MainWindow::updateMovesDisplay(QString pieceID, QString coords) {
    // testing function to display current steps
    QStandardItem *currentMove = new QStandardItem(pieceID + " " + coords);
    currentMove->setFlags(currentMove->flags() & ~Qt::ItemIsEditable);
    model->appendRow(currentMove);
    ui->listView->setModel(model);
}

// verify that the path from (startX, startY) -> (endX, endY) is not blocked (Bresenham's line algorithm)
bool MainWindow::verifyPath(int startX, int startY, int endX, int endY) {

    //check for every piece besides knight
    if(boardState[startX][startY]->getLabel() != "Knight") {
        int dX = abs(endX - startX);
        int dY = abs(endY - startY);
        //sign to increment currX, currY by
        int signX = (endX > startX) - (endX < startX);
        int signY = (endY > startY) - (endY < startY);
        int currX = startX;
        int currY = startY;
        int error = dX - dY; //margin of error for each shift

        while(currX != endX || currY != endY) {
            int currErr = 2*error;
            //if currErr > -dY, decrease error and move currX
            if(currErr > -dY) {
                error -= dY;
                currX += signX;
            }
            //if currErr < dX, increase error and move currY
            if(currErr < dX) {
                error += dX;
                currY += signY;
            }

            //no further verifications once curr coordinates reach end coordinates
            if(currX == endX && currY == endY) {
                break;
            }
            if(currX < 0 || currX >= boardState.length() || currY < 0 || currY >= boardState[0].length()) {
                return false;
            }
            if(boardState[currX][currY] != nullptr) {
                return false;
            }
        }
    }

    //at the destination, if there is a piece then verify alternating colors
    if(boardState[endX][endY] != nullptr && boardState[endX][endY]->getColor() == boardState[startX][startY]->getColor()) {
        return false;
    }
    return true;
}

MainWindow::~MainWindow()
{
    delete ui;
}

