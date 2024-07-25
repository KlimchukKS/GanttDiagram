#include "DiagramBody.h"

#include <QPainter>

DiagramBody::DiagramBody(double width,
                         double height,
                         double head_height,
                         double tree_item_height,
                         QDateTime first,
                         QDateTime last,
                         char dimension,
                         QObject *parent)
    : QObject{parent}
    , width_{width}
    , height_{height}
    , head_height_{head_height}
    , tree_item_height_{tree_item_height}
    , first_{first}
    , last_{last}
    , dimension_{dimension}
{

}

QRectF DiagramBody::boundingRect() const {
    return {0, 0, width_, height_ - tree_item_height_};
}

void DiagramBody::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    painter->setPen({{Qt::black}, 1});

    auto days_count = first_.daysTo(last_) + 1;
    QDate date_counter = first_.date();
    double day_rect_width_step = width_ * scaleFactor / days_count;

    for (auto day = 0; day < days_count; ++day) {
        QRectF day_rect(day * day_rect_width_step, 0, day_rect_width_step, head_height_ / 2.);
        painter->drawRect(day_rect);
        painter->drawText(day_rect, Qt::AlignCenter, date_counter.toString("dd.MM.yyyy"));
        date_counter = date_counter.addDays(1);

        auto step = day_rect_width_step / 24.;
        auto left_pos_x = day * day_rect_width_step;

        for(auto i = 0; i < 24; ++i) {
            QRectF hour_rect(left_pos_x, head_height_ / 2., step, head_height_ / 2.);

            painter->drawText(hour_rect, Qt::AlignCenter, QString::number(i));

            painter->drawRect(hour_rect);

            left_pos_x += step;
        }

        double step_wertical_line = day_rect_width_step / 24.;
        double count = day * day_rect_width_step;
        //Рисуем вертикальные линии
        for (auto i = 0; i < 25; ++i) {
            painter->drawLine(QLineF(count, head_height_, count, height_ - head_height_ - tree_item_height_));
            count += step_wertical_line;
        }
    }

    //Рисуем горизонтальные линии
    /*for (double count = head_height_; count < height_ - head_height_; count += tree_item_height_) {
        painter->drawLine(QLineF(0, count, width_, count));
    }*/

    //Принт шапки диаграммы
    /*{
        QRectF day_rect(0, 0, width_ * scaleFactor, head_height_ / 2.);
        painter->drawRect(day_rect);
        painter->drawText(day_rect, Qt::AlignCenter, "29.05.2024 - Временный пример оси x");

        auto step = width_ * scaleFactor / 24.;
        //auto step = width_ / 24.;
        auto left_pos_x = 0.;
        for(auto i = 0; i < 24; ++i) {
            QRectF hour_rect(left_pos_x, head_height_ / 2., step, head_height_ / 2.);

            painter->drawText(hour_rect, Qt::AlignCenter, QString::number(i));

            painter->drawRect(hour_rect);

            left_pos_x += step;
        }
    }

    double step_wertical_line = width_ * scaleFactor / 24.;
    double count = 0.;
    //Рисуем вертикальные линии
    for (auto i = 0; i < 25; ++i) {
        painter->drawLine(QLineF(count, head_height_, count, height_ - head_height_ - tree_item_height_));
        count += step_wertical_line;
    }

    //Рисуем горизонтальные линии
    for (double count = head_height_; count < height_ - head_height_; count += tree_item_height_) {
        painter->drawLine(QLineF(0, count, width_, count));
    }*/
}
