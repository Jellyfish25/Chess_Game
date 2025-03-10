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

using std::make_shared;
/*
 * To do:
 *  1.implement castle logic
 *      * King cannot be in check before castling
 *      * King cannot move through or end in check
 *      * King + Rook should not have moved
 *      * No pieces exist between the path
 *  2. create a gameOver function
 *      * called when there is no possible way for the King to get out of a checkmate
 *      * simulate all possible moves
 *          * 1. verify if the king can move to a safe tile
 *          * 2. find all attacker pieces (isValidMove = true going to)
 *          * 3. for each attacker, verify if a defending piece can block it
 *  3.create a ChessBoard class to maintain the board logic
 *      * keep the main window for display logic only
 *      * boardState, handleMove, isSafeMove, isValidPath in ChessBoard
 *      * instanced by MainWindow
 *      * ChessBoard can contain player state too, or create a Player class (alternating)
 *  4.verify gameplay correctness + edge cases
 *      * create test cases
 *      * note: can begin prior to step 1
 *  5.Implement peer-to-peer connectivity, locking boardstate based on current turn
 */
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
    qDebug() << movingPiece->getColor();
    //check if the current movingPiece is not null
    if(!movingPiece) {
        return;
    }

    if(movingPiece->isValidMove(startX, startY, endX, endY)) { //main movement logic
        //prevent pieces from being able to cross existing pieces on that path
        if(!isValidPath(startX, startY, endX, endY)) {
            qDebug() << "invalid move: current piece is blocked";
            return;
        }
        isValidMove = true;
    }
    else if(movingPiece->getLabel() == "Pawn" && model->rowCount() > 0) { //en passant scenario
        QString previousMove = model->item(model->rowCount() - 1)->text();
        QStringList moveList = previousMove.split(" ");
        if (moveList.size() < 3) {
            return;
        }

        //previous coordinates played
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
                        //note: will never expose opposing king, can automatically remove
                        boardState[row][col] = nullptr;
                    }
                }
            }
        }
    }

    if(isValidMove) {
        //todo: if the king is checkmated, display winner + reset board
        //call gameOver() function
        if (boardState[endX][endY] == whiteKing) {
            //whiteKing = nullptr;
        } else if (boardState[endX][endY] == blackKing) {
            //blackKing = nullptr;
        }

        if(!isSafeMove(movingPiece, endX, endY)) {
            qDebug() << "error: king is in check";
            return;
        }
        //pawn becomes queen if it reaches the end of the board
        if (movingPiece->getLabel() == "Pawn" && (endX == boardState.size() - 1 || endX == 0)) {
            QString color = movingPiece->getColor();
            movingPiece = make_shared<Queen>(color, endX, endY);//new Queen(color, endX, endY);
        }

        //play the current move (update boardState & display)
        boardState[endX][endY] = movingPiece;
        boardState[startX][startY] = nullptr;
        movingPiece->setCoordinates(endX, endY);
        //qDebug() << "prev coords after move: " << movingPiece->getPrevPos()[0] << movingPiece->getPrevPos()[1];
        //qDebug() << "curr coords after move: " << movingPiece->getCurrPos()[0] << movingPiece->getCurrPos()[1];

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

// verify that the move will not result in a check (looking forward to the proposed move)
bool MainWindow::isSafeMove(shared_ptr<ChessPiece> movingPiece, int endX, int endY) {
    if (!whiteKing || !blackKing) {  // Ensure whiteKing is not null
        qDebug() << "White king is captured!";
        return false;
    }

    //Set the board state
    shared_ptr<ChessPiece> tempPiece = boardState[endX][endY];
    std::array<int, 2> currCoords = movingPiece->getCurrPos();

    //simulate the move
    boardState[endX][endY] = movingPiece;
    boardState[currCoords[0]][currCoords[1]] = nullptr;  // Clear old position

    //get the coordinates of the current king
    shared_ptr<King> kingPiece = this->blackKing;
    QString oppColor = "white";
    if(movingPiece->getColor() == "white") {
        kingPiece = this->whiteKing;
        oppColor = "black";
    }
    std::array<int, 2> kingCoords = kingPiece->getCurrPos();

    //set king coordinates to end coords if moving piece is king piece
    if(movingPiece->getLabel() == "King") {
        kingCoords[0] = endX;
        kingCoords[1] = endY;
    }
    qDebug() << "king coordinates: " << kingCoords[0] << kingCoords[1];

    bool isSafe = true;
    for(int i = 0; i < boardState.size(); i++) {
        for(int j = 0; j < boardState[0].size(); j++) {
            if(boardState[i][j] &&
                boardState[i][j]->getColor() == oppColor &&
                boardState[i][j]->isValidMove(i, j, kingCoords[0], kingCoords[1]) &&
                isValidPath(i, j, kingCoords[0], kingCoords[1])) {
                qDebug() << "checked by:" << boardState[i][j]->getColor()
                         << boardState[i][j]->getLabel()
                         <<"coords: " << i << j;
                isSafe = false;
            }
            if(!isSafe) {
                break;
            }
        }
    }

    //set the board state back
    boardState[currCoords[0]][currCoords[1]] = movingPiece;
    boardState[endX][endY] = tempPiece;
    return isSafe;
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

