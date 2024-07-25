#include "DiagramHat.h"

DiagramHat::DiagramHat(qreal width,
                       qreal height,
                       qreal head_height)
    : width_{width}
    , height_{height}
    , head_height_{head_height}
{
    this->setZValue(1);
}

void DiagramHat::Scale(double value) {
    scale_factor_ = value;

    this->setX(this->x() * scale_factor_);

    this->update(boundingRect());
}

void DiagramHat::SetWidth(qreal width) {
    width_ = width;

    this->update(boundingRect());
}

qreal DiagramHat::GetWidth() const {
    return width_ * scale_factor_;
}

QRectF DiagramHat::boundingRect() const {
    return {0., 0., width_ * scale_factor_, height_};
}

YearDiagramHat::YearDiagramHat(qreal width,
                               qreal height,
                               qreal head_height,
                               QDate date)
    : DiagramHat(width, height, head_height)
    , date_{date}
{

}

QDateTime YearDiagramHat::GetTimeDate() const {
    return {date_, {0,0,0}};
}

void YearDiagramHat::paint(QPainter *painter, const QStyleOptionGraphicsItem *option [[maybe_unused]], QWidget *widget [[maybe_unused]]) {
    painter->setPen({QColor{76, 76, 115}, 1});
    painter->setFont({"Arial [Bold]", 12});

    auto scaled_width = width_ * scale_factor_;

    QRectF day_rect(0, 0, scaled_width, head_height_);
    painter->drawRect(day_rect);
    painter->drawText(day_rect, Qt::AlignCenter, date_.toString("yyyy"));

    painter->drawLine(QLineF(0, head_height_, 0, height_));
    painter->drawLine(QLineF(scaled_width, head_height_, scaled_width, height_));
}

MonthDiagramHat::MonthDiagramHat(qreal width,
                                 qreal height,
                                 qreal head_height,
                                 QDate date)
    : DiagramHat(width, height, head_height)
    , date_{date}
{

}

QDateTime MonthDiagramHat::GetTimeDate() const {
    return {date_, {0,0,0}};
}

void MonthDiagramHat::paint(QPainter *painter, const QStyleOptionGraphicsItem *option [[maybe_unused]], QWidget *widget [[maybe_unused]]) {
    painter->setPen({QColor{76, 76, 115}, 1});
    painter->setFont({"Arial [Bold]", 12});

    qreal scaled_width = width_ * scale_factor_;

    QRectF year_rect(0, 0, scaled_width, head_height_ / 2.);
    painter->drawRect(year_rect);
    painter->drawText(year_rect, Qt::AlignCenter, date_.toString("yyyy"));

    qreal pos_x = 0;

    QDate date_counter = {date_.year(), 1, 1};

    qreal days_per_pixel_ = scaled_width / date_counter.daysInYear();

    for(auto month = 0; month < 12; ++month) {
        qreal month_width = date_counter.daysInMonth() * days_per_pixel_;

        QRectF day_rect(pos_x, head_height_ / 2., month_width, head_height_ / 2.);

        painter->drawText(day_rect, Qt::AlignCenter, QString::number(month + 1));
        painter->drawRect(day_rect);
        painter->drawLine(QLineF(pos_x, head_height_, pos_x, height_));

        date_counter = date_counter.addMonths(1);
        pos_x += month_width;
    }

    painter->drawLine(QLineF(scaled_width, head_height_, scaled_width, height_));
}

DayDiagramHat::DayDiagramHat(qreal width,
                             qreal height,
                             qreal head_height,
                             QDate date)
    : DiagramHat(width, height, head_height)
    , date_{date}
{

}

QDateTime DayDiagramHat::GetTimeDate() const {
    return {date_, {0,0,0}};
}

void DayDiagramHat::paint(QPainter *painter, const QStyleOptionGraphicsItem *option [[maybe_unused]], QWidget *widget [[maybe_unused]]) {
    painter->setPen({QColor{76, 76, 115}, 1});
    painter->setFont({"Arial [Bold]", 12});

    qreal scaled_width = width_ * scale_factor_;

    QRectF day_rect(0, 0, scaled_width, head_height_ / 2.);
    painter->drawRect(day_rect);
    painter->drawText(day_rect, Qt::AlignCenter, date_.toString("MM.yyyy"));

    int days = date_.daysInMonth();

    qreal day_rect_width_step = scaled_width / days;

    for(auto day = 0; day < days; ++day) {
        qreal pos_x = day * day_rect_width_step;

        QRectF hour_rect(pos_x, head_height_ / 2., day_rect_width_step, head_height_ / 2.);

        painter->drawText(hour_rect, Qt::AlignCenter, QString::number(day + 1));

        painter->drawRect(hour_rect);

        painter->drawLine(QLineF(pos_x, head_height_, pos_x, height_));
    }

    painter->drawLine(QLineF(scaled_width, head_height_, scaled_width, height_));
}

HourDiagramHat::HourDiagramHat(qreal width,
                               qreal height,
                               qreal head_height,
                               QDate date)
    : DiagramHat(width, height, head_height)
    , date_{date}
{

}

QDateTime HourDiagramHat::GetTimeDate() const {
    return {date_, {0,0,0}};
}

void HourDiagramHat::paint(QPainter *painter, const QStyleOptionGraphicsItem *option [[maybe_unused]], QWidget *widget [[maybe_unused]]) {
    painter->setPen({QColor{76, 76, 115}, 1});
    painter->setFont({"Arial [Bold]", 12});

    qreal scaled_width = width_ * scale_factor_;

    QRectF day_rect(0, 0, scaled_width, head_height_ / 2.);
    painter->drawRect(day_rect);
    painter->drawText(day_rect, Qt::AlignCenter, date_.toString("dd.MM.yyyy"));

    int hours = 24;

    qreal hour_rect_width_step = scaled_width / hours;

    for(auto hour = 0; hour < hours; ++hour) {
        qreal pos_x = hour * hour_rect_width_step;

        QRectF hour_rect(pos_x, head_height_ / 2., hour_rect_width_step, head_height_ / 2.);

        painter->drawText(hour_rect, Qt::AlignCenter, QString::number(hour));

        painter->drawRect(hour_rect);

        painter->drawLine(QLineF(pos_x, head_height_, pos_x, height_));
    }

    painter->drawLine(QLineF(scaled_width, head_height_, scaled_width, height_));
}

MinuteDiagramHat::MinuteDiagramHat(qreal width,
                                   qreal height,
                                   qreal head_height,
                                   QDateTime date)
    : DiagramHat(width, height, head_height)
    , date_{date}
{

}

QDateTime MinuteDiagramHat::GetTimeDate() const {
    return date_;
}

void MinuteDiagramHat::paint(QPainter *painter, const QStyleOptionGraphicsItem *option [[maybe_unused]], QWidget *widget [[maybe_unused]]) {
    painter->setPen({QColor{76, 76, 115}, 1});
    painter->setFont({"Arial [Bold]", 12});

    qreal scaled_width = width_ * scale_factor_;

    QRectF day_rect(0, 0, scaled_width, head_height_ / 2.);
    painter->drawRect(day_rect);
    painter->drawText(day_rect, Qt::AlignCenter, date_.toString("hh dd.MM.yyyy"));

    int minutes = 60;

    qreal minute_rect_width_step = scaled_width / minutes;

    for(auto minute = 0; minute < minutes; ++minute) {
        qreal pos_x = minute * minute_rect_width_step;

        QRectF hour_rect(pos_x, head_height_ / 2., minute_rect_width_step, head_height_ / 2.);

        painter->drawText(hour_rect, Qt::AlignCenter, QString::number(minute));

        painter->drawRect(hour_rect);

        painter->drawLine(QLineF(pos_x, head_height_, pos_x, height_));
    }

    painter->drawLine(QLineF(scaled_width, head_height_, scaled_width, height_));
}

SecondDiagramHat::SecondDiagramHat(qreal width,
                                   qreal height,
                                   qreal head_height,
                                   QDateTime date)
    : DiagramHat(width, height, head_height)
    , date_{date}
{

}

QDateTime SecondDiagramHat::GetTimeDate() const {
    return date_;
}

void SecondDiagramHat::paint(QPainter *painter, const QStyleOptionGraphicsItem *option [[maybe_unused]], QWidget *widget [[maybe_unused]]) {
    painter->setPen({QColor{76, 76, 115}, 1});
    painter->setFont({"Arial [Bold]", 12});

    qreal scaled_width = width_ * scale_factor_;

    QRectF day_rect(0, 0, scaled_width, head_height_ / 2.);
    painter->drawRect(day_rect);
    painter->drawText(day_rect, Qt::AlignCenter, date_.toString("hh:mm dd.MM.yyyy"));

    int seconds = 60;

    qreal second_rect_width_step = scaled_width / seconds;

    for(auto second = 0; second < seconds; ++second) {
        qreal pos_x = second * second_rect_width_step;

        QRectF hour_rect(pos_x, head_height_ / 2., second_rect_width_step, head_height_ / 2.);

        painter->drawText(hour_rect, Qt::AlignCenter, QString::number(second));

        painter->drawRect(hour_rect);

        painter->drawLine(QLineF(pos_x, head_height_, pos_x, height_));
    }

    painter->drawLine(QLineF(scaled_width, head_height_, scaled_width, height_));
}
