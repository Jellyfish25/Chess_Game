#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "DraggableLabel.h"
#include <QDialog>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , model(new QStandardItemModel())
    , boardState(8, QVector<std::shared_ptr<ChessPiece>>(8, nullptr))
    , chessBoard(std::make_shared<ChessBoard>(this, this->ui, this->promoButton))
{
    ui->setupUi(this);
    ui->gridLayout->setSpacing(0);
    ui->gridLayout->setContentsMargins(0, 0, 0, 0);

    connect(chessBoard.get(), &ChessBoard::boardUpdated, this, &MainWindow::updateBoardDisplay);
    connect(chessBoard.get(), &ChessBoard::moveUpdated, this, &MainWindow::updateMovesDisplay);
    chessBoard->initializeBoard();

    //queenBtn->setStyleSheet("background-color: green;");
    //todo: make GUI scrollable + dynamically scale

    // set the pawn promotion UI
    menuDisplay("white");
}

// creates the pawn promotion menu display
void MainWindow::menuDisplay(QString color) {
    // adjust radio button
    QRadioButton *radioBtn = ui->radioButton;
    const QString radioStyleSheet =
        "QRadioButton::indicator:checked {"
        "border: 1px solid gray;"
        "border-radius: 7px;"
        "background-color: green;"
        "}";
    radioBtn->setStyleSheet(radioStyleSheet);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *queenBtn = new QPushButton();
    QPushButton *rookBtn = new QPushButton();
    QPushButton *bishopBtn = new QPushButton();
    QPushButton *knightBtn = new QPushButton();

    // Set piece image & size for each button
    unique_ptr<Queen> q = std::make_unique<Queen>(color, 0, 0);
    unique_ptr<Rook> r = std::make_unique<Rook>(color, 0, 0);
    unique_ptr<Bishop> b = std::make_unique<Bishop>(color, 0, 0);
    unique_ptr<Knight> k = std::make_unique<Knight>(color, 0, 0);

    queenBtn->setIcon(QPixmap::fromImage(q->getImage()));
    rookBtn->setIcon(QPixmap::fromImage(r->getImage()));
    bishopBtn->setIcon(QPixmap::fromImage(b->getImage()));
    knightBtn->setIcon(QPixmap::fromImage(k->getImage()));

    queenBtn->setIconSize(QSize(40, 40));
    rookBtn->setIconSize(QSize(40, 40));
    bishopBtn->setIconSize(QSize(40, 40));
    knightBtn->setIconSize(QSize(40, 40));

    // Update the currently selected promo button (default to queen)
    if(promoButton == NULL) {
        promoButton = queenBtn;
    }

    // adjust stylesheet for each button
    queenBtn->setStyleSheet(this->buttonStyleSheet);
    queenBtn->setObjectName("queen");
    rookBtn->setStyleSheet(this->buttonStyleSheet);
    rookBtn->setObjectName("rook");
    bishopBtn->setStyleSheet(this->buttonStyleSheet);
    bishopBtn->setObjectName("bishop");
    knightBtn->setStyleSheet(this->buttonStyleSheet);
    knightBtn->setObjectName("knight");

    buttonLayout->addWidget(queenBtn);
    buttonLayout->addWidget(rookBtn);
    buttonLayout->addWidget(bishopBtn);
    buttonLayout->addWidget(knightBtn);

    // add select functionality
    auto selectButton = [this](QPushButton* selected) {
        if(promoButton != NULL) {
            promoButton->setStyleSheet(this->buttonStyleSheet);
        }
        selected->setStyleSheet("QPushButton {"
                                "   background-color: Green;"
                                "   border: none;"
                                "   padding: 10px;"
                                "   border-radius: 5px;"
                                "}"

                                "QPushButton:hover {"
                                "   background-color: orange;"
                                "}");
        promoButton = selected;
    };

    connect(queenBtn, &QPushButton::clicked, this, [this, queenBtn, selectButton]() {
        selectButton(queenBtn);
    });
    connect(rookBtn, &QPushButton::clicked, this, [this, rookBtn, selectButton]() {
        selectButton(rookBtn);
    });
    connect(bishopBtn, &QPushButton::clicked, this, [this, bishopBtn, selectButton]() {
        selectButton(bishopBtn);
    });
    connect(knightBtn, &QPushButton::clicked, this, [this, knightBtn, selectButton]() {
        selectButton(knightBtn);
    });

    ui->horizontalLayout->addLayout(buttonLayout);
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

