#ifndef DIAGRAMRECT_H
#define DIAGRAMRECT_H

#include <QObject>
#include <QGraphicsItem>

#include <QDebug>

class DiagramRect : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    DiagramRect() = delete;
    DiagramRect(QObject *parent = nullptr);
    virtual ~DiagramRect() = default;
public slots:
    virtual void SetWidthAndRepaint(double width) = 0;
private:
    virtual QRectF boundingRect() const = 0;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) = 0;
};

class DiagramBranchRect : public DiagramRect
{
    Q_OBJECT
public:
    DiagramBranchRect() = delete;
    DiagramBranchRect(QObject *parent = nullptr) = delete;

    DiagramBranchRect(const QString& name, qreal width, qreal height, QObject *parent = nullptr);
public slots:
    void SetWidthAndRepaint(double width) override final {
        width_ = width;

        update(0, 0, width_, height_);
    }

private:
    double width_;
    double height_;

    QString name_;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

class DiagramLeafRect : public DiagramRect
{
    Q_OBJECT
public:
    DiagramLeafRect() = delete;
    DiagramLeafRect(QObject *parent = nullptr) = delete;

    DiagramLeafRect(unsigned char completion_rate, qreal width, qreal height, QObject *parent = nullptr);

    void SetValue(unsigned char value);
public slots:
    void SetWidthAndRepaint(double width) override final {
        width_ = width;

        update(0, 0, width_, height_);
    }
private:
    double width_;
    double height_;

    unsigned char completion_rate_;

    QRectF boundingRect() const override final;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override final;
};

#endif // DIAGRAMRECT_H
