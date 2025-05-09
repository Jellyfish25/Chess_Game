#include "DraggableLabel.h"

DraggableLabel::DraggableLabel(shared_ptr<ChessBoard> chessBoard, QWidget *parent)
    : chessBoard(chessBoard),QLabel(parent) {
    setAcceptDrops(true); // Allow drag and drop
}

void DraggableLabel::mousePressEvent(QMouseEvent *event) {
    if (!pixmap().isNull()) {
        int posX = this->property("posX").toInt();
        int posY = this->property("posY").toInt();

        // Check if it's the player's turn
        if (!chessBoard->isPlayerTurn(posX, posY)) {
            qDebug() << "Not the current player's turn!";
            return; // Prevent drag
        }
        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData();

        // Set the current position as the drag data
        mimeData->setText(QString::number(this->property("posX").toInt()) + "," +
                          QString::number(this->property("posY").toInt()));
        drag->setMimeData(mimeData);
        drag->setPixmap(pixmap());

        QPoint hotspot = event->pos();
        drag->setHotSpot(hotspot);

        // Execute the drag action
        drag->exec(Qt::MoveAction);
    }
}

void DraggableLabel::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    }
}

void DraggableLabel::dropEvent(QDropEvent *event) {

    qDebug() << "drop";
    QString data = event->mimeData()->text();
    QStringList parts = data.split(",");
    if(parts.size() != 2) {
        return;  // Incorrect data format
    }

    int startX = parts[0].toInt();
    int startY = parts[1].toInt();
    qDebug() << "start position: (" << startX << "," << startY << ")";

    // Convert drop position to global then to board-relative position
    QPoint globalDropPos = mapToParent(event->position().toPoint());
    //qDebug() << "Global drop position: " << globalDropPos;

    int cellSize = 40;  // Adjust to match actual board cell size
    int endX = globalDropPos.y() / cellSize;
    int endY = globalDropPos.x() / cellSize;

    qDebug() << "end position: (" << endX << "," << endY << ")";

    // Now call the handleMove function
    //mainWindow->handleMove(startX, startY, endX, endY);
    // play the move locally
    chessBoard->handleMove(startX, startY, endX, endY, false);
    event->acceptProposedAction();
}
