#include "DiagramHorizontalLines.h"

#include <QPainter>

DiagramHorizontalLines::DiagramHorizontalLines(qreal width, qreal height, qreal steep)
    : width_{width}
    , height_{height}
    , steep_{steep}
{

}

QRectF DiagramHorizontalLines::boundingRect() const {
    return {0, 0, width_, height_};
}

void DiagramHorizontalLines::paint(QPainter *painter, const QStyleOptionGraphicsItem *option [[maybe_unused]], QWidget *widget [[maybe_unused]]) {
    for (qreal count = 0.; count < height_; count += steep_) {
        painter->drawLine(QLineF(0, count, width_, count));
    }
}
