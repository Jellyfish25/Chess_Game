#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "DraggableLabel.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , model(new QStandardItemModel())
    , boardState(8, QVector<std::shared_ptr<ChessPiece>>(8, nullptr))
    , chessBoard(std::make_shared<ChessBoard>())
{
    ui->setupUi(this);
    ui->gridLayout->setSpacing(0);
    ui->gridLayout->setContentsMargins(0, 0, 0, 0);

    connect(chessBoard.get(), &ChessBoard::boardUpdated, this, &MainWindow::updateBoardDisplay);
    connect(chessBoard.get(), &ChessBoard::moveUpdated, this, &MainWindow::updateMovesDisplay);

    chessBoard->initializeBoard();
    //todo: make GUI scrollable + dynamically scale
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
            DraggableLabel *label = new DraggableLabel(chessBoard, this);  // Pass 'this' as MainWindow
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

// update what was currently played in the "Played Moves" window
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

