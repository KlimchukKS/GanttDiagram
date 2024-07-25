#ifndef DIAGRAMHAT_H
#define DIAGRAMHAT_H

#include <QGraphicsItem>
#include <QPainter>
#include <QDateTime>

class DiagramHat : public QGraphicsItem
{
public:
    DiagramHat() = delete;

    DiagramHat(qreal width,
               qreal height,
               qreal head_height);

    virtual ~DiagramHat() = default;

    void Scale(double value);

    void SetWidth(qreal width);

    qreal GetWidth() const;

    virtual QDateTime GetTimeDate() const = 0;

protected:
    qreal width_;
    qreal height_;
    qreal head_height_;

    qreal scale_factor_ = 1.;
private:
    QRectF boundingRect() const override final;
};

class YearDiagramHat : public DiagramHat {
public:
    YearDiagramHat() = delete;

    YearDiagramHat(qreal width,
                   qreal height,
                   qreal head_height,
                   QDate date);

    QDateTime GetTimeDate() const override;
private:
    QDate date_;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option [[maybe_unused]], QWidget *widget [[maybe_unused]]) override final;
};

class MonthDiagramHat : public DiagramHat {
public:
    MonthDiagramHat() = delete;

    MonthDiagramHat(qreal width,
                    qreal height,
                    qreal head_height,
                    QDate date);

    QDateTime GetTimeDate() const override;
private:
    QDate date_;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option [[maybe_unused]], QWidget *widget [[maybe_unused]]) override final;
};

class DayDiagramHat : public DiagramHat {
public:
    DayDiagramHat() = delete;

    DayDiagramHat(qreal width,
                  qreal height,
                  qreal head_height,
                  QDate date);

    QDateTime GetTimeDate() const override;
private:
    QDate date_;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option [[maybe_unused]], QWidget *widget [[maybe_unused]]) override final;
};

class HourDiagramHat : public DiagramHat {
public:
    HourDiagramHat() = delete;

    HourDiagramHat(qreal width,
                   qreal height,
                   qreal head_height,
                   QDate date);

    QDateTime GetTimeDate() const override;
private:
    QDate date_;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option [[maybe_unused]], QWidget *widget [[maybe_unused]]) override final;
};

class MinuteDiagramHat : public DiagramHat {
public:
    MinuteDiagramHat() = delete;

    MinuteDiagramHat(qreal width,
                     qreal height,
                     qreal head_height,
                     QDateTime date);

    QDateTime GetTimeDate() const override;
private:
    QDateTime date_;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option [[maybe_unused]], QWidget *widget [[maybe_unused]]) override final;
};

class SecondDiagramHat : public DiagramHat {
public:
    SecondDiagramHat() = delete;

    SecondDiagramHat(qreal width,
                     qreal height,
                     qreal head_height,
                     QDateTime date);

    QDateTime GetTimeDate() const override;
private:
    QDateTime date_;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option [[maybe_unused]], QWidget *widget [[maybe_unused]]) override final;
};

#endif // DIAGRAMHAT_H
