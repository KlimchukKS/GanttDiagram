#ifndef DIAGRAMBODY_H
#define DIAGRAMBODY_H

#include <QObject>
#include <QGraphicsItem>
#include <QDateTime>

class DiagramBody : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    DiagramBody(double width,
                double height,
                double head_height,
                double tree_item_height,
                QDateTime first,
                QDateTime last,
                char dimension,
                QObject *parent = nullptr);

    void Scale(double value) {
        if ((scaleFactor + value) < 0.2) {
            return;
        }
        scaleFactor += value;

        this->update(boundingRect());
    }
    double GetWidth() const {
        return width_ * scaleFactor;
    }

    double SecondsPerPixel() const {
        auto days_count = first_.daysTo(last_) + 1;

        return (width_ * scaleFactor) / (days_count * 24. * 60. * 60.);
    }

private:
    double width_;
    double height_;
    double head_height_;
    double tree_item_height_;

    QDateTime first_;
    QDateTime last_;

    //Колонка в самом низу шапки
    //S - секунды, M - минуты, H - часы, d, D - дни, m - месяцы, y - года
    char dimension_;

    double scaleFactor = 1;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option [[maybe_unused]], QWidget *widget [[maybe_unused]]) override;
};

#endif // DIAGRAMBODY_H
