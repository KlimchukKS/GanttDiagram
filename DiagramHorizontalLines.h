#ifndef DIAGRAMHORIZONTALLINES_H
#define DIAGRAMHORIZONTALLINES_H

#include <QGraphicsItem>

class DiagramHorizontalLines : public QGraphicsItem
{
public:
    DiagramHorizontalLines() = delete;
    DiagramHorizontalLines(qreal width, qreal height, qreal steep);

    void SetWidth(qreal width) {
        width_ = width;

        this->update(boundingRect());
    }
private:
    qreal width_, height_, steep_;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option [[maybe_unused]], QWidget *widget [[maybe_unused]]) override;
};

#endif // DIAGRAMHORIZONTALLINES_H
