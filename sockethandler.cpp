#include "sockethandler.h"
#include "mainwindow.h"
#include "QLineEdit"
#include "QFormLayout"
#include "QMessageBox"

/*
 * currently:
 *      1. allows for a server to be open (locally).
 *      2. can connect using netcat and see moves being played (using moveUpdated and sendMove)
 * to do:
 *      1. allow for a client to connect to the server (locally).
 *      2. add a waiting screen popup until a p2p connection is established (waiting for connection)
 *      3. relay moves played between host & client
 *          3.1 either set host to white and client to black, or add a menu screen for host to choose
 *          3.2 update board locally and wait for the next move to be played
 *          methods: joinServer(quint16 port), receiveMove(QString pieceID, QString coords)
 *              note: sockethandler has a reference to the board, when moveUpdated is called, it is a valid move
*/
SocketHandler::SocketHandler(QObject *parent): QObject(parent){
    this->socket = nullptr;
    this->server = nullptr;
}

// sets up the chessboard + host/server connections
void SocketHandler::setChessBoard(std::shared_ptr<ChessBoard> board) {
    this->chessBoard = board;
    connect(chessBoard.get(), &ChessBoard::sendCoordinates, this, &SocketHandler::sendMove);
}

void SocketHandler::sendMove(std::array<int, 2> startCoords, std::array<int, 2> endCoords) {
        qDebug() << "Sending move: " << startCoords[0] << "," << startCoords[1] << " -> " << endCoords[0] << "," << endCoords[1];
    if (socket && socket->isOpen()) {
        int startX = startCoords[0];
        int startY = startCoords[1];
        int endX = endCoords[0];
        int endY = endCoords[1];

        socket->write(QString::number(startX).toUtf8() + " " + QString::number(startY).toUtf8() + " " +
                      QString::number(endX).toUtf8() + " " + QString::number(endY).toUtf8() + "\r\n");
        socket->flush();
    }
    else {
        qDebug() << "socket not open";
    }
}

void SocketHandler::receiveMove() {
    qDebug() << " Received move";
    while (socket->canReadLine()) {
        QByteArray line = socket->readLine().trimmed();
        QString data = QString::fromUtf8(line);

        bool x1,y1,x2,y2;
        QStringList coordinates = data.split(" ");
        if(coordinates.size() != 4) {
            qDebug() << "Received malformed move data";
            return;
        }

        int startX = coordinates[0].toInt(&x1);
        int startY = coordinates[1].toInt(&y1);
        int endX = coordinates[2].toInt(&x2);
        int endY = coordinates[3].toInt(&y2);

        // verify the data was received correctly
        if(x1 && x2 && y1 && y2) {
            chessBoard->handleMove(startX, startY, endX, endY);
        }
        else {
            qDebug() << "Recieved invalid coordinate data";
        }
    }
}

// ************ client logic ********************
void SocketHandler::joinServer(QString hostIp, quint16 port) {
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &SocketHandler::receiveMove);
    socket->connectToHost(hostIp, port);
    if(socket->waitForConnected(3000)) {
        qDebug() << "Connected to host";
    }
    else {
        qDebug() << "Connection Failed";
    }
}

// ************ server-side logic ***************
// accepts client connections
void SocketHandler::newConnection() {
    qDebug() << "New connection received!";
    QTcpSocket *clientSocket = server->nextPendingConnection();
    socket = clientSocket;
    qDebug() << "Connected to:" << clientSocket->peerAddress().toString();

    clientSocket->write("New Connection Established\r\n");
    clientSocket->flush();
    clientSocket->waitForBytesWritten(3000);
    //clientSocket->close();
}

// initiates the server
void SocketHandler::startServer(quint16 port) {
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
    }

    if(server) {
        qDebug() << "Server is closed.";
        server->close();
    }
}

int SocketHandler::displayMenuOptions() {
    // determine if the user wants peer-to-peer or local connection (create GUI)
    QDialog initialWindow;
    initialWindow.setWindowTitle("Initial Window");

    // port input field
    QLineEdit *portInput = new QLineEdit();
    portInput->setPlaceholderText("Enter port to host");

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Host Port:", portInput);

    QLabel *promptMessage = new QLabel("Select a game option");
    promptMessage->setAlignment(Qt::AlignCenter);
    QPushButton *hostButton = new QPushButton("Start Server");
    QPushButton *connectButton = new QPushButton("Connect");
    QPushButton *localButton = new QPushButton("Local");
    QPushButton *closeButton = new QPushButton("Exit");

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(promptMessage);
    layout->addLayout(formLayout);
    layout->addWidget(hostButton);
    layout->addWidget(connectButton);
    layout->addWidget(localButton);
    layout->addWidget(closeButton);
    initialWindow.setLayout(layout);

    // host a connection
    connect(hostButton, &QPushButton::clicked, &initialWindow, [this, portInput]() {
        qDebug() << "process host (display host window)";
        quint16 port = portInput->text().toUShort(); //note: short = 16bits (2bytes)
        qDebug() << portInput->text();
        if(port == 0 || (port < 1024) && port < pow(2,16)) {
            QMessageBox::warning(nullptr, "Invalid Input", "Please enter a valid port number");
        }
        else {
            this->startServer(port);
        }
    });

    // connect to a host
    connect(connectButton, &QPushButton::clicked, &initialWindow, [this]() {
        qDebug() << "process connection (display connection window)";
    });

    connect(localButton, &QPushButton::clicked, &initialWindow, &QDialog::accept);
    connect(closeButton, &QPushButton::clicked, &initialWindow, &QDialog::reject);
    // exit the application if the user clicks the x or Exit button
    if(initialWindow.exec() == QDialog::Rejected) {
        return 0;
    }

    return 1;
}
