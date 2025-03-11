#ifndef DRAGGABLELABEL_H
#define DRAGGABLELABEL_H

#include <QLabel>
#include <QDrag>
#include <QMimeData>
#include <QDropEvent>
#include <QGridLayout>
#include "chessboard.h"

//extend Qlabel to make it a draggable component
class DraggableLabel : public QLabel {
    Q_OBJECT

public:
    explicit DraggableLabel(std::shared_ptr<ChessBoard>, QWidget *parent = nullptr);
    void setPieceImage(const QPixmap& image);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    std::shared_ptr<ChessBoard> chessBoard;
};
#endif // DRAGGABLELABEL_H
