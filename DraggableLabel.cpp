#include "DraggableLabel.h"
#include <QDrag>
#include <QMimeData>
#include <QDropEvent>
#include <QMouseEvent>
#include <QWidget>
#include "mainwindow.h"

DraggableLabel::DraggableLabel(MainWindow *mainWindow, QWidget *parent)
    : QLabel(parent), mainWindow(mainWindow) {
    setAcceptDrops(true); // Allow drag and drop
}

void DraggableLabel::mousePressEvent(QMouseEvent *event) {
    if (!pixmap().isNull()) {
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
    if (parts.size() != 2) return;  // Incorrect data format

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
    mainWindow->handleMove(startX, startY, endX, endY);
    event->acceptProposedAction();
}
