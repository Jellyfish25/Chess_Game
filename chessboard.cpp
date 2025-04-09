#include "chessboard.h"
#include <QDialog>
#include <QPushButton>
#include <Qlabel>
/*
 * To do:
 *  1. verify gameplay correctness + edge cases
 *      * create test cases
 *      * note: can begin prior to step 1
 *  2. update GUI to display Winner/Loser message popup on checkmate (using notifications)
 *  3. Implement peer-to-peer connectivity, locking boardstate based on current turn
 *  4. update GUI to be dynamic (adjustable/scrollable)
 */

void ChessBoard::initializeBoard() {
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
    //updateBoardDisplay(boardState);
    emit boardUpdated(boardState);
    // connects the Promote button (notifies that the user wants to promote pawn)
    connect(ui->pushButton, &QPushButton::clicked, &loop, &QEventLoop::quit);
}

// note: this can be readjusted to create a pop up for winning/losing (future)
void ChessBoard::promoNotification(int row, int col) {
    QApplication::processEvents();
    QDialog *popup = new QDialog(mainWindow);
    popup->setWindowTitle("Pawn Promotion");

    popup->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    popup->setModal(true); // Makes it block interaction with the main window
    popup->setFixedSize(160, 40);

    // Layout to display promotion options
    QHBoxLayout *layout = new QHBoxLayout(popup);
    layout->setSpacing(0);  // No spacing between buttons
    layout->setContentsMargins(0, 0, 0, 0);  // No margins to ensure tight packing

    // Create buttons for each promotion option (Queen, Rook, Bishop, Knight)
    QPushButton *queenBtn = new QPushButton(popup);
    QPushButton *rookBtn = new QPushButton(popup);
    QPushButton *bishopBtn = new QPushButton(popup);
    QPushButton *knightBtn = new QPushButton(popup);

    // Set images (pieces) for each button
    Queen* q = new Queen("white", 0, 0);
    Rook* r = new Rook("white", 0, 0);
    Bishop* b = new Bishop("white", 0, 0);
    Knight* k = new Knight("white", 0, 0);

    queenBtn->setIcon(QPixmap::fromImage(q->getImage()));
    rookBtn->setIcon(QPixmap::fromImage(r->getImage()));
    bishopBtn->setIcon(QPixmap::fromImage(b->getImage()));
    knightBtn->setIcon(QPixmap::fromImage(k->getImage()));

    // Set the icon size to fit the buttons (optional)
    queenBtn->setIconSize(QSize(40, 40)); // Adjust size as needed
    rookBtn->setIconSize(QSize(40, 40));
    bishopBtn->setIconSize(QSize(40, 40));
    knightBtn->setIconSize(QSize(40, 40));

    // Add buttons to layout
    layout->addWidget(queenBtn);
    layout->addWidget(rookBtn);
    layout->addWidget(bishopBtn);
    layout->addWidget(knightBtn);

    // Move the popup to the calculated global position
    QPoint p = mainWindow->pos();
    p.setX(2.5*p.x() + 40);
    p.setY(p.y() + 40);
    // Move the popup to the calculated position
    popup->move(p);
    popup->show();
}

void ChessBoard::handleMove(int startX, int startY, int endX, int endY) {
    if(promoRef != nullptr) {
        qDebug() << "promo button selected: " << promoRef->objectName();
    }
    //verify that the new coordinates are within the boardState
    if(endX >= boardState.size() || endY >= boardState[0].size()) {
        return;
    }

    //check if the current movingPiece is not null
    shared_ptr<ChessPiece> movingPiece = boardState[startX][startY];
    if(!movingPiece) {
        return;
    }

    //Check if the player is attempting to castle
    if(!movingPiece->isValidMove(startX, startY, endX, endY)) {
        //1. king moves two tiles (on either side of the rook)
        if(movingPiece->getLabel() == "King" && abs(startY - endY) == 2) {
            //2. both king & rook have not moved
            int dirY = (endY > startY) - (endY < startY);
            int rookPosY = dirY == 1 ? endY + dirY : endY + dirY + dirY;
            if(movingPiece->getPrevPos()[0] == -1
                && boardState[endX][rookPosY]
                && boardState[endX][rookPosY]->getLabel() == "Rook"
                && boardState[endX][rookPosY]->getPrevPos()[0] == -1) {
                //3. king + moving tiles should not be under attack
                if(isSafeMove(movingPiece, startX, startY)
                    && isSafeMove(movingPiece, startX, startY + dirY)
                    && isSafeMove(movingPiece, startX, startY + dirY + dirY)) {
                    qDebug() << "VALID CASTLE";

                    shared_ptr<ChessPiece> rookPiece = boardState[endX][rookPosY];
                    int endRookY = endY + (dirY * -1);

                    //move king piece
                    boardState[endX][endY] = movingPiece;
                    boardState[startX][startY] = nullptr;
                    movingPiece->setCoordinates(endX, endY);

                    //move rook piece
                    boardState[endX][endRookY] = boardState[endX][rookPosY];
                    boardState[endX][rookPosY] = nullptr;
                    rookPiece->setCoordinates(endX, endRookY);

                    previouslyMoved = movingPiece;
                    updateDisplay(movingPiece);
                }
            }
        }
        return;
    }

    //***************process the move if it is valid *********************
    //en passant scenario (adjacent movement)
    bool capturePawn = false;
    if(movingPiece->getLabel() == "Pawn") {
        //check that the previous piece is a different color and is a pawn
        if(previouslyMoved && previouslyMoved->getColor() != movingPiece->getColor() && previouslyMoved->getLabel() == "Pawn") {
            std::array<int, 2> prevCoords = previouslyMoved->getPrevPos();
            std::array<int, 2> currCoords = previouslyMoved->getCurrPos();

            //check if the previous pawn has moved 2 tiles (opening up for enpassant)
            if(!boardState[endX][endY] && abs(currCoords[0] - prevCoords[0]) == 2) {
                //check if the move point is on the same row & same column as opponent pawn
                if(currCoords[0] == startX && currCoords[1] == endY && abs(startY - endY) == 1) {
                    //check if the move point is going in the correct direction
                    if((movingPiece->getColor() == "white" && endX == startX - 1) ||
                        (movingPiece->getColor() == "black" && endX == startX + 1)) {
                        //capture pawn
                        capturePawn = true;
                    }
                }
            }
        }
    }

    //prevent pieces from being able to cross existing pieces on that path
    if(!isValidPath(startX, startY, endX, endY) && !capturePawn) {
        qDebug() << "invalid move: current path is blocked";
        return;
    }

    //verify if the current move is safe (not placing king in check)
    if(!isSafeMove(movingPiece, endX, endY)) {
        qDebug() << "error: Current move will place king in check";
        return;
    }

    //************* play the current move (move is valid) *************
    //capture pawn on en passant & verify if move does not place king in check
    if(capturePawn) {
        std::array<int, 2> currCoords = previouslyMoved->getCurrPos();
        shared_ptr<ChessPiece> tempPawn = boardState[currCoords[0]][currCoords[1]];
        boardState[currCoords[0]][currCoords[1]] = nullptr;
        if(!isSafeMove(movingPiece, endX, endY)) {
            qDebug() << "error: En passant capture places king in check";
            boardState[currCoords[0]][currCoords[1]] = tempPawn;
            return;
        }
    }

    //pawn promotion if the pawn reaches the end of the board
    //todo: give the player the option to change pawn into queen, rook, bishop, or knight
    if (movingPiece->getLabel() == "Pawn" && (endX == boardState.size() - 1 || endX == 0)) {
        QString color = movingPiece->getColor();

        // if the radio button (auto promote) is not active, wait until user clicks "Promote"
        if(!ui->radioButton->isChecked()) {
            loop.exec();
        }

        QString promoPieceName = this->promoRef->objectName();
        if(promoPieceName == "knight") {
            movingPiece = make_shared<Knight>(color, endX, endY);
        }
        else if(promoPieceName == "rook") {
            movingPiece = make_shared<Rook>(color, endX, endY);
        }
        else if(promoPieceName == "bishop") {
            movingPiece = make_shared<Bishop>(color, endX, endY);
        }
        else {
            movingPiece = make_shared<Queen>(color, endX, endY);
        }
    }

    //update boardState & display
    boardState[endX][endY] = movingPiece;
    boardState[startX][startY] = nullptr;

    //update the moving piece coordinates
    movingPiece->setCoordinates(endX, endY);
    previouslyMoved = movingPiece;

    //update the current display (move played + potential checkmate/stalemate)
    updateDisplay(movingPiece);
}

void ChessBoard::updateDisplay(shared_ptr<ChessPiece> movingPiece) {
    turnCounter++; //increment turn counter

    int endX = movingPiece->getCurrPos()[0];
    int endY = movingPiece->getCurrPos()[1];
    //display the current move taken
    char charVal = endY + 'a';
    char numVal = abs(endX - 7) + '1';
    QString coords = QString("(" + QString(charVal) + QString(numVal) + ")");
    QString pieceID = QString(movingPiece->getColor() + " " + movingPiece->getLabel());

    //verify if the opponent has any safe moves (to prevent checkmate)
    //todo: create gameOver() function to display winner
    if(isCheckMate(movingPiece->getColor() == "white" ? "black" : "white")) {
        qDebug() << "Checkmated by: " << movingPiece->getColor();
    }

    //todo: check for stale mate (no legal moves left, but king not in check)
    if(isStaleMate(movingPiece->getColor() == "white" ? "black" : "white")) {
        qDebug() <<  "stale mate";
    }

    //update board & move display
    emit boardUpdated(boardState);
    emit moveUpdated(pieceID, coords);
}

// verify that the path from (startX, startY) -> (endX, endY) is not blocked (Bresenham's line algorithm)
bool ChessBoard::isValidPath(int startX, int startY, int endX, int endY) {

    //pawn pathing
    if (boardState[startX][startY]->getLabel() == "Pawn") {
        // moving two tiles forward if path isnt blocked
        if(abs(endX - startX) == 2 && boardState[startX][startY]->getPrevPos()[0] == -1) {
            int midX = (startX + endX) / 2;
            if (!boardState[midX][startY] && !boardState[endX][endY]) {
                return true;
            }
        }
        // moving one tile forward if path isnt blocked
        if (abs(startX - endX) == 1 && startY == endY && !boardState[endX][endY]) {
            return true;
        }

        // verify that there is an enemy piece to capture on diagonal move
        int dirY = (boardState[startX][startY]->getColor() == "black") ? 1 : -1;
        if(boardState[startX][startY]->getLabel() == "Pawn"
            && endX == startX + dirY
            && abs(startY - endY) == 1
            && boardState[endX][endY]
            && boardState[startX][startY]->getColor() != boardState[endX][endY]->getColor()) {
            return true;
        }
        return false;
    }

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
            if(currX < 0 || currX >= boardState.size() || currY < 0 || currY >= boardState[0].size()) {
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

// verify that the move will not result in a check (looking forward to the proposed move)
bool ChessBoard::isSafeMove(shared_ptr<ChessPiece> movingPiece, int endX, int endY) {
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
    shared_ptr<ChessPiece> kingPiece = this->blackKing;
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
    //qDebug() << "king coordinates: " << kingCoords[0] << kingCoords[1];

    bool isSafe = true;
    for(int i = 0; i < boardState.size(); i++) {
        for(int j = 0; j < boardState[0].size(); j++) {
            if(boardState[i][j] &&
                boardState[i][j]->getColor() == oppColor &&
                boardState[i][j]->isValidMove(i, j, kingCoords[0], kingCoords[1]) &&
                isValidPath(i, j, kingCoords[0], kingCoords[1])) {
                // qDebug() << "checked by:" << boardState[i][j]->getColor()
                // << boardState[i][j]->getLabel()
                // <<"coords: " << i << j;
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

//verifies that the current piece is not in check
bool ChessBoard::isPieceInCheck(shared_ptr<ChessPiece> piece) {
    if(!piece) {
        return true;
    }

    std::array<int, 2> pieceCoords = piece->getCurrPos();
    QString oppColor = piece->getColor() == "white" ? "black" : "white";

    //verify that the current piece can be captured
    for(int i = 0; i < boardState.size(); i++) {
        for(int j = 0; j < boardState[0].size(); j++) {
            if(boardState[i][j] &&
                boardState[i][j]->getColor() == oppColor &&
                boardState[i][j]->isValidMove(i, j, pieceCoords[0], pieceCoords[1]) &&
                isValidPath(i, j, pieceCoords[0], pieceCoords[1]) &&
                //verify that moving the current selected piece will not result in a check
                isSafeMove(boardState[i][j], pieceCoords[0], pieceCoords[1])) {
                return true;
            }
        }
    }

    return false;
}

//returns the count of total possible moves that can be played
int ChessBoard::possibleMoves(QVector<shared_ptr<ChessPiece>> pieceList) {

    int validMoveCount = 0;
    //try all possible moves for each piece, verifying if the king is not in check after
    for(auto &piece : pieceList) {
        std::array<int, 2> coords = piece->getCurrPos();
        int startX = coords[0];
        int startY = coords[1];

        for(int endX = 0; endX < boardState.size(); endX++) {
            for(int endY = 0; endY < boardState[0].size(); endY++) {
                if(piece->isValidMove(startX, startY, endX, endY)
                    && isValidPath(startX, startY, endX, endY)
                    && isSafeMove(piece, endX, endY)) {
                        qDebug() << "safe move: " << piece->getColor() << piece->getLabel() << startX << startY << "to: " << endX << endY;
                        validMoveCount++;
                }
            }
        }
    }

    return validMoveCount;
}

// Verifies that the current color (player) has a move to get out of check
// note: Checkmate Verification
bool ChessBoard::isCheckMate(QString color) {
    shared_ptr<ChessPiece> kingPiece = (color == "white") ? whiteKing : blackKing;

    //verify that the king is in check
    if(!isPieceInCheck(kingPiece)) {
        return false;
    }

    //get all pieces that can currently attack the king
    QVector<shared_ptr<ChessPiece>> pieceList;
    for(int i = 0; i < boardState.size(); i++) {
        for(int j = 0; j < boardState[0].size(); j++) {
            if(boardState[i][j] && boardState[i][j]->getColor() == color) {
                pieceList.append(boardState[i][j]);
            }
        }
    }

    //verify that there are no possible valid moves to be made (including king movement)
    int validMoves = possibleMoves(pieceList);
    qDebug() << "available moves: " << validMoves;

    if(validMoves > 0) {
        return false;
    }
    return true;
}

//verifies if there is a stale mate (draw)
//conditions: king is not in check but has no legal moves left
bool ChessBoard::isStaleMate(QString color) {
    shared_ptr<ChessPiece> kingPiece = color == "white" ? whiteKing : blackKing;

    //king should not be in check
    if(isPieceInCheck(kingPiece)) {
        return false;
    }

    //get all playable pieces
    QVector<shared_ptr<ChessPiece>> playerPieces;
    for(int i = 0; i < boardState.size(); i++) {
        for(int j = 0; j < boardState[0].size(); j++) {
            if(boardState[i][j] && boardState[i][j]->getColor() == color && boardState[i][j]->getLabel() != "King") {
                playerPieces.append(boardState[i][j]);
            }
        }
    }

    //verify that the player has no valid moves left (excluding king)
    int validMoves = possibleMoves(playerPieces);
    if(validMoves > 0) {
        return false;
    }

    return true;
}

bool ChessBoard::isPlayerTurn(int posX, int posY) {
    shared_ptr<ChessPiece> currPiece = boardState[posX][posY];
    if(!currPiece) {
        return false;
    }

    if(turnCounter % 2 == 0 && currPiece->getColor() != "white") {
        return false;
    }

    if(turnCounter % 2 == 1 && currPiece->getColor() != "black") {
        return false;
    }

    return true;
}
