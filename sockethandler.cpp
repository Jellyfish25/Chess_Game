#include "sockethandler.h"
#include "mainwindow.h"
#include "QLineEdit"
#include "QFormLayout"
#include "QMessageBox"

/*
 * currently:
 *      1. allow for a client to connect to the server (locally).
 *      2. relay moves played between host & client
 *      3. pawn promotion + network logic working as intended
 * note: use valgrind or gdb to look for dangling pointers
 *
 * to do:
 *      1. instead of passing previous + current coordinates && label type, pass in the moved chess piece
 *          1.1 move a piece locally and call sendLocalData(ChessPiece piece)
 *          1.2 on sendNetData(ChessPiece piece), serialize the object into a byte stream
 *          1.3 on receieveNetData() deserialize the object and play the move locally: handleMove(prevX, prevY, currX, currY, true, label)
 *      2. test on private networks + public networks, and adjust connectivity code if necessary
 *      3. add winner/loser screen pop ups for both players on a local checkmate move (no network relays req)
 *      4. add the option for the host to restart the game
 *      5. if the host or client closes the game, generate a pop up that the session has been closed
*/
SocketHandler::SocketHandler(QObject *parent, std::shared_ptr<ChessBoard> chessBoard): QObject(parent){
    this->socket = nullptr;
    this->server = nullptr;
    setChessBoard(chessBoard);
}

// sets up the chessboard + host/server connections
void SocketHandler::setChessBoard(std::shared_ptr<ChessBoard> chessBoard) {
    this->chessBoard = chessBoard;
    connect(chessBoard.get(), &ChessBoard::sendLocalData, this, &SocketHandler::sendNetData);
}

// send a move over the connection
void SocketHandler::sendNetData(std::array<int, 2> startCoords, std::array<int, 2> endCoords, QString promoType) {
    qDebug() << (server ? "Host sending move" : "Client sending move");
    //qDebug() << "Sending move: " << startCoords[0] << "," << startCoords[1] << " -> " << endCoords[0] << "," << endCoords[1];
    if (socket && socket->isOpen()) {
        int startX = startCoords[0];
        int startY = startCoords[1];
        int endX = endCoords[0];
        int endY = endCoords[1];

        socket->write(QString::number(startX).toUtf8() + " " + QString::number(startY).toUtf8() + " " +
                      QString::number(endX).toUtf8() + " " + QString::number(endY).toUtf8() + " " +
                      promoType.toUtf8() + "\r\n");
        socket->flush();
    }
    else {
        qDebug() << "socket not open";
    }
}

// receive a move over the connection
void SocketHandler::receiveNetData() {
    qDebug() << " Received move";
    while (socket->canReadLine()) {
        QByteArray line = socket->readLine().trimmed();
        QString data = QString::fromUtf8(line);

        qDebug() << line;
        bool x1,y1,x2,y2;
        QStringList dataList = data.split(" ");

        if(dataList.size() != 4 && dataList.size() != 5) {
            qDebug() << "Received malformed move data" << dataList;
            qDebug() << dataList.size();
            return;
        }

        int startX = dataList[0].toInt(&x1);
        int startY = dataList[1].toInt(&y1);
        int endX = dataList[2].toInt(&x2);
        int endY = dataList[3].toInt(&y2);
        QString type = "";
        if(dataList.size() == 5) {
            type = dataList[4];
        }

        // verify the data was received correctly
        if(x1 && x2 && y1 && y2) {
            chessBoard->handleMove(startX, startY, endX, endY, true, type);
        }
        else {
            qDebug() << "Recieved invalid coordinate data";
        }
    }
}

// ************ client logic ********************
// Connects to the server and initializes the client's color
void SocketHandler::joinServer(QString hostIp, quint16 port) {
    socket = new QTcpSocket(this);
    socket->connectToHost(hostIp, port);
    if(socket->waitForConnected(3000)) {
        this->initializeClientColor();
        qDebug() << "Connected to host";
    }
    else {
        qDebug() << "Connection Failed";
    }
}

// Receives and initializes the client's color, and connects to the receive move handler
void SocketHandler::initializeClientColor() {
    // read the color on initial connection & set the color value
    connect(socket, &QTcpSocket::readyRead, this, [this](){
        QByteArray line = socket->readLine().trimmed();
        QString clientColor = QString::fromUtf8(line).trimmed();
        // set color (client)
        chessBoard->playerColor = clientColor;

        // disconnect this slot to prevent updating the color
        disconnect(socket, &QTcpSocket::readyRead, this, nullptr);

        // onnect to the receive move handler
        connect(socket, &QTcpSocket::readyRead, this, &SocketHandler::receiveNetData);
    });
}

// ************ server-side logic ***************
// accepts client connections
void SocketHandler::newConnection() {
    qDebug() << "New connection received!";
    QTcpSocket *clientSocket = server->nextPendingConnection();
    connect(clientSocket, &QTcpSocket::readyRead, this, &SocketHandler::receiveNetData);

    socket = clientSocket;
    qDebug() << "Connected to:" << clientSocket->peerAddress().toString();

    // set the host's color
    chessBoard->playerColor = playerColor;

    // send the client's color over (opposite of host color)
    QString clientColor = playerColor == "white" ? "black" : "white";
    QByteArray colorData = clientColor.toUtf8();
    socket->write(colorData);
    socket->flush();
    socket->waitForBytesWritten(kDelayTime);
    //clientSocket->write("New Connection Established\r\n");
    //clientSocket->flush();
    //clientSocket->waitForBytesWritten(3000);
    //clientSocket->close();
}

// initiates the server
void SocketHandler::startServer(quint16 port) {
    // if the server is open, close it before attempting to open again
    if(server) {
        closeServer();
    }

    server = new QTcpServer(this);
    connect(server, &QTcpServer::newConnection, this, &SocketHandler::newConnection);
    if(!server->listen(QHostAddress::Any, port)) {
        qDebug() << "Server could not start";
    }
    else {
        qDebug() << "Server started";
    }
}

void SocketHandler::closeServer() {
    if(server && socket && socket->isOpen()) {
        socket->write("Disconnected from host.\r\n");
        socket->flush();
        socket->disconnectFromHost();

        if(socket->state() == QAbstractSocket::ConnectedState || socket->state() == QAbstractSocket::ClosingState) {
            socket->waitForDisconnected(1000);
        }
        socket->close();
        //delete socket;
        //socket = nullptr;
    }

    if(server) {
        qDebug() << "Server is closed.";
        server->close();
    }

    //delete server;
    //server = nullptr;
}

int SocketHandler::displayMenuOptions() {
    // determine if the user wants peer-to-peer or local connection (create GUI)
    QDialog initialWindow;
    initialWindow.setWindowTitle("Initial Window");

    QLabel *promptMessage = new QLabel("Select a game option");
    promptMessage->setAlignment(Qt::AlignCenter);
    QPushButton *hostButton = new QPushButton("Start Server");
    QPushButton *connectButton = new QPushButton("Connect");
    QPushButton *localButton = new QPushButton("Local");
    QPushButton *closeButton = new QPushButton("Exit");

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(promptMessage);
    layout->addWidget(hostButton);
    layout->addWidget(connectButton);
    layout->addWidget(localButton);
    layout->addWidget(closeButton);
    initialWindow.setLayout(layout);

    // host a connection
    // note: specify who is white/black. based on color, there is a lock in place
    connect(hostButton, &QPushButton::clicked, &initialWindow, [this, &initialWindow]() {
        qDebug() << "process host (display host window)";
        // Display input field for port + color
        QDialog *inputDialog = new QDialog(&initialWindow);
        QFormLayout *inputForm = new QFormLayout();

        // port input
        QLineEdit *portInput = new QLineEdit();
        portInput->setPlaceholderText("12345");
        portInput->setMaxLength(5);
        inputForm->addRow("Port:", portInput);

        // color type input
        QWidget *colorWidget = new QWidget();
        QHBoxLayout *colorLayout = new QHBoxLayout();
        unique_ptr<King> wKing = std::make_unique<King>("white", 0, 0);
        unique_ptr<King> bKing = std::make_unique<King>("black", 0, 0);

        QPushButton *blackButton = new QPushButton();
        blackButton->setIcon(QPixmap::fromImage(bKing->getImage()));
        blackButton->setIconSize(QSize(40, 40));

        QPushButton *whiteButton = new QPushButton();
        whiteButton->setIcon(QPixmap::fromImage(wKing->getImage()));
        whiteButton->setIconSize(QSize(40, 40));
        colorLayout->addWidget(whiteButton);
        colorLayout->addWidget(blackButton);
        colorWidget->setLayout(colorLayout);
        inputForm->addRow("\nHost Color:", colorWidget);

        // selected style
        auto defaultStyle = [](QPushButton *button) {
            button->setStyleSheet("border: 3px solid none; border-radius: 5px;");
        };
        auto selectedStyle = [](QPushButton *button) {
            button->setStyleSheet("border: 3px solid #0078d7; border-radius: 5px;");
        };

        QString selectedColor = "white";
        selectedStyle(whiteButton);
        defaultStyle(blackButton);

        connect(whiteButton, &QPushButton::clicked, [&]() {
            selectedColor = "white";
            defaultStyle(blackButton);
            selectedStyle(whiteButton);
        });
        connect(blackButton, &QPushButton::clicked, [&]() {
            selectedColor = "black";
            defaultStyle(whiteButton);
            selectedStyle(blackButton);
        });

        // submit button
        QPushButton *submitButton = new QPushButton("Open Connection");
        QVBoxLayout *layout = new QVBoxLayout();
        layout->addItem(inputForm);
        layout->addWidget(submitButton);
        inputDialog->setLayout(layout);

        // when the open connection button is pressed, verify the input is valid
        connect(submitButton, &QPushButton::clicked, [this, &portInput, &selectedColor, &initialWindow, &inputDialog]() {
            if(isValidInput(false, *portInput, *portInput)) {
                // save the color state
                // qDebug() << "Selected color:" << selectedColor;
                // set the host's color
                playerColor = selectedColor;

                // start the server
                quint16 port = portInput->text().toUShort();
                this->startServer(port);

                // Display waiting for client to connect...
                QDialog *waitingDialog = new QDialog(&initialWindow);  // Stack-based dialog
                QLabel *waitText = new QLabel("Waiting for a client to connect...", waitingDialog);
                QVBoxLayout *waitLayout = new QVBoxLayout(waitingDialog);
                waitLayout->addWidget(waitText);
                waitingDialog->setLayout(waitLayout);
                waitingDialog->resize(100, 50);
                waitingDialog->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
                waitingDialog->setModal(true);

                // After client connects, close waiting dialog and accept initialWindow
                connect(this->server, &QTcpServer::newConnection, [&]() {
                    qDebug() << "Client Connected";
                    waitingDialog->accept();
                    inputDialog->accept();
                    initialWindow.accept();
                });

                waitingDialog->exec();
            }
        });

        inputDialog->exec();
    });

    // connect to a host note: add a pop up asking for IP + Port input
    connect(connectButton, &QPushButton::clicked, &initialWindow, [this, &initialWindow]() {
        //qDebug() << "process connection (display connection window)";
        QDialog dialog;
        dialog.setWindowTitle("Connect to Host");

        QLineEdit *ipInput = new QLineEdit();
        ipInput->setPlaceholderText("127.0.0.1");

        QLineEdit *portInput = new QLineEdit();
        portInput->setPlaceholderText("12345");

        QFormLayout *formLayout = new QFormLayout;
        formLayout->addRow("IP Address:", ipInput);
        formLayout->addRow("Port:", portInput);

        QPushButton *connectNow = new QPushButton("Connect");
        QVBoxLayout *layout = new QVBoxLayout;
        layout->addLayout(formLayout);
        layout->addWidget(connectNow);
        dialog.setLayout(layout);

        connect(connectNow, &QPushButton::clicked, &dialog, [&]{
            QString ip = ipInput->text();
            quint16 port = portInput->text().toUInt();
            if(port == 0) {
            }
            else {
                this->joinServer(ip, port);
                dialog.accept();
                initialWindow.accept();
            }
        });
        dialog.exec();
    });

    connect(localButton, &QPushButton::clicked, &initialWindow, &QDialog::accept);
    connect(closeButton, &QPushButton::clicked, &initialWindow, &QDialog::reject);
    // exit the application if the user clicks the x or Exit button
    if(initialWindow.exec() == QDialog::Rejected) {
        return 0;
    }

    return 1;
}

bool SocketHandler::isValidInput(bool isClient, QLineEdit& ipInput, QLineEdit& portInput) {
    // verify valid IP address when client connects
    QHostAddress ipAdd;
    if(isClient && !ipAdd.setAddress(ipInput.text())) {
        QMessageBox::warning(nullptr, "Invalid Input", QString("Error: Invalid IP address: %1").arg(ipInput.text()));
        return false;
    }

    // verify valid port
    quint16 port = portInput.text().toUShort();
    if(port == 0 || (port < 1024) || port >= 65535) {
        QMessageBox::warning(nullptr, "Invalid Input", QString("Error: Invalid port: %1").arg(portInput.text()));
        return false;
    }

    return true;
}
