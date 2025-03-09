#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QStandardItemModel>
#include <QVector>
#include "Pawn.cpp"
#include "Rook.cpp"
#include "Knight.cpp"
#include "Bishop.cpp"
#include "Queen.cpp"
#include "DraggableLabel.h"
#include <QApplication>
#include <memory>

using std::shared_ptr;
using std::make_shared;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , model(new QStandardItemModel())
    , boardState(8, QVector<std::shared_ptr<ChessPiece>>(8, nullptr))
{
    ui->setupUi(this);
    ui->gridLayout->setSpacing(0);
    ui->gridLayout->setContentsMargins(0, 0, 0, 0);
    //todo: make GUI scrollable + dynamically scale

    //generate pieces and store them in a container
    //each piece will have x,y coordinates correlated to a piece
    //insert pawns
    for(int col = 0; col < 8; col++) {
        boardState[1][col] = make_shared<Pawn>("black", 1, col);//new Pawn("black", 1, col);
        boardState[6][col] = make_shared<Pawn>("white", 6, col);//new Pawn("white", 6, col);
    }

    //insert rooks
    boardState[0][0] = make_shared<Rook>("black", 0, 0);//new Rook("black", 0, 0);
    boardState[0][7] = make_shared<Rook>("black", 0, 7); //new Rook("black", 0, 7);
    boardState[7][0] = make_shared<Rook>("white", 7, 0);//new Rook("white", 7, 0);
    boardState[7][7] = make_shared<Rook>("white", 7, 7);//new Rook("white", 7, 7);

    //insert knights
    boardState[0][1] = make_shared<Knight>("black", 0, 1); //new Knight("black", 0, 1);
    boardState[0][6] = make_shared<Knight>("black", 0, 6); //new Knight("black", 0, 6);
    boardState[7][1] = make_shared<Knight>("white", 7, 1); //new Knight("white", 7, 1);
    boardState[7][6] = make_shared<Knight>("white", 7, 6); //new Knight("white", 7, 6);

    //insert bishops
    boardState[0][2] = make_shared<Bishop>("black", 0, 2); //new Bishop("black", 0, 2);
    boardState[0][5] = make_shared<Bishop>("black", 0, 5); //new Bishop("black", 0, 5);
    boardState[7][2] = make_shared<Bishop>("white", 7, 2); //new Bishop("white", 7, 2);
    boardState[7][5] = make_shared<Bishop>("white", 7, 5); //new Bishop("white", 7, 5);

    //insert queens
    boardState[0][3] = make_shared<Queen>("black", 0, 3); //new Queen("black", 0, 3);
    boardState[7][3] = make_shared<Queen>("white", 7, 3); //new Queen("white", 7, 3);

    //insert kings (check if this is safe to do)
    blackKing = std::make_shared<King>("black", 0, 4);//new King("black", 0, 4);
    whiteKing = std::make_shared<King>("white", 7, 4);
    //whiteKing = new King("white", 7, 4);
    boardState[0][4] = blackKing;
    boardState[7][4] = whiteKing;

    //initialize board
    updateBoardDisplay(boardState);
}

//paints the board state on update, can be further optimized
//note: initialize board (64), then update only two tiles (2) instead of all tiles (64)
void MainWindow::updateBoardDisplay(QVector<QVector<std::shared_ptr<ChessPiece>>> boardState) {
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
                //qDebug() << "not null: " << i << j;
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
    shared_ptr<ChessPiece> movingPiece = boardState[startX][startY];
    //check if the current movingPiece is not null
    if(!movingPiece) {
        return;
    }

    if (movingPiece->isValid(startX, startY, endX, endY)) { //main movement logic
        //next move will need to get the king out of check
        if(movingPiece->getColor() == "white" && whiteKing->isChecked) {

        }
        else {

        }

        //prevent pieces from being able to cross existing pieces on that path
        if(!isValidPath(startX, startY, endX, endY)) {
            qDebug() << "invalid move: current piece is blocked";
            return;
        }
        isValidMove = true;
    }
    else if(movingPiece->getLabel() == "Pawn" && model->rowCount() > 0) { //en passant scenario (WORKING)
        QString previousMove = model->item(model->rowCount() - 1)->text();
        QStringList moveList = previousMove.split(" ");
        if (moveList.size() < 3) {
            return;
        }

        int col = moveList[2][1].unicode() - 'a';
        int row = abs(moveList[2][2].unicode() - '1' - 7);
        //check that the previous piece is a different color and is a pawn
        shared_ptr<ChessPiece> previousPawn = boardState[row][col];
        if(previousPawn && previousPawn->getColor() != movingPiece->getColor() && previousPawn->getLabel() == "Pawn") {
            std::array<int, 2> prevCoords = previousPawn->getPrevPos();
            std::array<int, 2> currCoords = previousPawn->getCurrPos();

            //check if it has moved 2 tiles (opening up for enpassant)
            if(!boardState[endX][endY] && abs(currCoords[0] - prevCoords[0]) == 2) {
                //check if the move point is valid for en passant
                if(currCoords[0] == startX && currCoords[1] == endY && abs(startY - endY) == 1) {
                    if((movingPiece->getColor() == "white" && startX == endX + 1) ||
                        (movingPiece->getColor() == "black" && startX == endX - 1)) {
                        //capture pawn
                        isValidMove = true;
                    }
                }
            }
        }
    }

    if(isValidMove) {
        //check if king is captured
        //todo: if the king is captured, display winner + reset board
        //call gameOver() function
        if (boardState[endX][endY] == whiteKing) {
            //whiteKing = nullptr;
        } else if (boardState[endX][endY] == blackKing) {
            //blackKing = nullptr;
        }

        //update the current piece's location
        movingPiece->setCoordinates(endX, endY);

        //pawn becomes queen if it reaches the end of the board
        if (movingPiece->getLabel() == "Pawn" && (endX == boardState.size() - 1 || endX == 0)) {
            QString color = movingPiece->getColor();
            movingPiece = make_shared<Queen>(color, endX, endY);//new Queen(color, endX, endY);
        }

        boardState[endX][endY] = movingPiece;
        boardState[startX][startY] = nullptr;

        //see if this current move places the opposite color in check
        std::array<int, 2> oppKingCoords = whiteKing->getCurrPos();
        if(movingPiece->getColor() == "white") { //black
            oppKingCoords = blackKing->getCurrPos();
        }
        //if the moving piece can reach opponent's king coordinates, mark as in check
        if(movingPiece->isValid(endX, endY, oppKingCoords[0], oppKingCoords[1])
            && isValidPath(endX, endY, oppKingCoords[0], oppKingCoords[1])) {
            if(movingPiece->getColor() == "white") {
                this->blackKing->isChecked = true;
                qDebug() << "black king is checked";
            }
            else {
                this->whiteKing->isChecked = true;
                qDebug() << "white king is checked";
            }
        }

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

// verify that the selected piece is not in check (looking forward to the proposed move)
// *********IMPLEMENT THIS && CALL ON LINE 115
bool MainWindow::isChecked(ChessPiece *movingPiece, int futureX, int futureY) {
    if (!whiteKing || !blackKing) {  // Ensure whiteKing is not null
        qDebug() << "White king is captured!";
        return false;
    }

    //verify if the next move by the king is not checked
    if(movingPiece->getLabel() == "King") {

        return false;
    }
    else { //verify if the move will protect the king
        //get the coordinates of the current king
        shared_ptr<King> kingPiece = this->blackKing;
        if(movingPiece->getColor() == "white") {
            kingPiece = this->whiteKing;
        }
        std::array<int, 2> currCoords = kingPiece->getCurrPos();
        for(int i = 0; i < boardState.size(); i++) {
            for(int j = 0; j < boardState[0].size(); j++) {
                if(boardState[i][j] && boardState[i][j]->getColor() == "black" && boardState[i][j]->isValid(i, j, currCoords[0], currCoords[1])) {
                    qDebug() << "checked by:" << boardState[i][j]->getLabel() << boardState[i][j]->getColor();
                    return true;
                }
            }
        }
        return false;
    }
}

// verify that the path from (startX, startY) -> (endX, endY) is not blocked (Bresenham's line algorithm)
bool MainWindow::isValidPath(int startX, int startY, int endX, int endY) {

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

