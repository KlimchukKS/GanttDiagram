#include "DiagramRect.h"

#include <QPainter>

DiagramRect::DiagramRect(QObject *parent)
    : QObject{parent}
{

}

DiagramBranchRect::DiagramBranchRect(const QString& name, qreal width, qreal height, QObject *parent)
    : DiagramRect{parent}
    , width_{width}
    , height_{height}
    , name_{name}
{

}

QRectF DiagramBranchRect::boundingRect() const {
    return QRectF{0, 0, width_, height_};
}

void DiagramBranchRect::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QColor rect_fill_{179, 179, 193};

    painter->fillRect(this->boundingRect(), rect_fill_);

    painter->setFont(QFont("Intel [Regular]", 12));
    painter->setPen(QPen({76, 76, 115}));

    painter->drawText(this->boundingRect(), Qt::AlignCenter, name_);

    painter->drawRect(this->boundingRect());
}

DiagramLeafRect::DiagramLeafRect(unsigned char completion_rate, qreal width, qreal height, QObject *parent)
    : DiagramRect{parent}
    , width_{width}
    , height_{height}
    , completion_rate_{completion_rate}
{

}

QRectF DiagramLeafRect::boundingRect() const {
    return QRectF{0, 0, width_, height_};
}

void DiagramLeafRect::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QColor rect_fill_{230, 230, 230};

    painter->fillRect(this->boundingRect(), rect_fill_);

    QString tmp = QString().setNum(completion_rate_);
    tmp.append('%');

    QRectF progress_rect = this->boundingRect();
    progress_rect.setWidth((completion_rate_ / 100.) * width_);

    painter->fillRect(progress_rect, {181, 178, 231});

    painter->setFont(QFont("Intel [Regular]", 12));
    painter->setPen(QPen({76, 76, 115}));

    painter->drawText(this->boundingRect(), Qt::AlignCenter, tmp);

    painter->drawRect(this->boundingRect());
}
