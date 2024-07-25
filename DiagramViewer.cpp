#include "DiagramViewer.h"

#include <functional>
#include <QWheelEvent>

#include <QDebug>

DiagramViewer::DiagramViewer(char dimension,
                             QWidget *parent)
    : QGraphicsView(parent)
    , dimension_counter_{dimension}
    //, dimension_counter_{'D'}
{
    diagram_scene_ = new QGraphicsScene();
    // Делаем верхний левый угол точкой 0, 0
    this->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    //Отключаем вретикальный сроллбар
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    this->setScene(diagram_scene_);
    // Заплняем хэш-таблицу значениями (функциями)
    this->SetHatFactory();

    connect(&dimension_counter_, SIGNAL(DimensionChanged()), this, SLOT(ProcessChangeDimension()));
}

void DiagramViewer::BuildCoordinateGrid(qreal width,
                                        qreal height,
                                        qreal head_height,
                                        qreal tree_item_height) {
    //TODO: Единственное место, где используется const_canst. Вынести инициализацию в конструктор пока не выходит
    *const_cast<qreal*>(&screen_width_) = width;
    *const_cast<qreal*>(&screen_height_) = height;
    *const_cast<qreal*>(&head_height_) = head_height;

    horizontal_lines_ = new DiagramHorizontalLines(diagram_scene_->width(), height - head_height, tree_item_height);

    //Находим из введёных узлов самый первый день и последний
    SetFirstAndLastDateTime();

    emit dimension_counter_.DimensionChanged();

    horizontal_lines_->setY(head_height);
    diagram_scene_->addItem(horizontal_lines_);

    //scroll_bar_ = this->horizontalScrollBar();
    //connect(scroll_bar_, SIGNAL(valueChanged(int)), this, SLOT(TestSetValue(int)));

    //Центрируем экран на графическом итеме
    //this->centerOn(*(bufer_headers_.begin() + half_buffer_length_));
}

void DiagramViewer::SetSecondsPerPixel(qreal width) {
    auto days_count = first_.daysTo(last_) + 1;

    QDateTime start;
    QDateTime end;

    switch (dimension_counter_.GetDimension()) {
    case 'S':
        //seconds_per_pixel_ = width / days_count * 24;
        start = {first_.date(), {first_.time().hour(), first_.time().minute(), 0}};
        end = {last_.date(), {last_.time().hour(), last_.time().minute(), 0}};

        end = end.addSecs(60);

        seconds_per_pixel_ = width / start.secsTo(end);

        break;
    case 'M':
        start = {first_.date(), {first_.time().hour(), 0, 0}};
        end = {last_.date(), {last_.time().hour(), 0, 0}};

        end = end.addSecs(60 * 60);

        seconds_per_pixel_ = width / start.secsTo(end);
        break;
    case 'H':
        seconds_per_pixel_ = width / (60. * 60. * days_count * 24);
        break;
    case 'd':
    case 'D':
        start = {{first_.date().year(), first_.date().month(), 1}, {0, 0, 0}};
        end = {{last_.date().year(), last_.date().month(), 1}, {0, 0, 0}};

        end = end.addMonths(1);

        seconds_per_pixel_ = width / start.secsTo(end);

        break;
    case 'm':
    case 'y':
        start = {{first_.date().year(), 1, 1}, {0, 0, 0}};
        end = {{last_.date().year() + 1, 1, 1}, {0, 0, 0}};

        seconds_per_pixel_ = width / start.secsTo(end);

        break;
    }
}

void DiagramViewer::MakeDiagramHat() {
    //Заполнение буфера диапазоном [first_, last_]
    auto add_new_hat = [this](DiagramHat* hat) {
        bufer_headers_.emplace_back(hat);

        hat->setPos(diagram_scene_->width() - hat->GetWidth(), 0);
        diagram_scene_->addItem(hat);
    };

    qint64 hat_number = GetStartPageHatCount();

    if (dimension_counter_ == 'd' || dimension_counter_ == 'D') {
        seconds_per_pixel_ = screen_width_ / (first_.date().daysInMonth() * 24 * 60 * 60);
    }

    if (dimension_counter_ == 'm' || dimension_counter_ == 'y') {
        seconds_per_pixel_ = screen_width_ / (366 * 24 * 60 * 60);
    }

    for (int i = 0; i <= hat_number; ++i) {
        add_new_hat(get_diagram_hat_[dimension_counter_.GetDimension()](first_, i));
    }

    /*if (!(dimension_counter_ == 'd' || dimension_counter_ == 'D')) {
        diagram_scene_->setSceneRect({0, 0, screen_width_ * (hat_number + 1), diagram_scene_->sceneRect().height()});
    }*/

    /* - Заполнение буфера относительно текущей даты
    QDateTime current = QDateTime::currentDateTime();
    // Заполнение только с положительными координатами
    diagram_scene_->setSceneRect({0, 0, width_ * (2 * half_buffer_length_ + 1), diagram_scene_->sceneRect().height()});

    for (int i = -half_buffer_length_, pos = 0; i <= half_buffer_length_; ++i, ++pos) {
        add_new_hat(get_diagram_hat_[dimension_couner_.GetDimension()](current, i), pos);
    }


    //Заполнение с отрицательными координатами
    diagram_scene_->setSceneRect({width_ * -half_buffer_length_, 0, width_ * (2 * half_buffer_length_ + 1), diagram_scene_->sceneRect().height()});

    for (int i = -half_buffer_length_; i <= half_buffer_length_; ++i) {
        add_new_hat(get_diagram_hat_[dimension_couner_.GetDimension()](current, i), i);
    }
     - */
}

void DiagramViewer::DeleteDiagramHat() {
    for (DiagramHat* hat : bufer_headers_) {
        diagram_scene_->removeItem(hat);

        //delete hat;
    }

    bufer_headers_.clear();
}

void DiagramViewer::wheelEvent(QWheelEvent* event) {
    setTransformationAnchor(AnchorUnderMouse);

    qreal scale_factor = 0.05;
    qreal new_width = 0.;

    //if (event->delta() < 0) {
    if (event->angleDelta().y() < 0) {
        scale_factor = -scale_factor;
    }

    qreal pos = 0;

    // Вычисляем новую ширину
    for (auto hat : bufer_headers_) {
        new_width += hat->GetWidth() + (hat->GetWidth() * scale_factor);
    }

    // проверка на выход в видимую часть границы шапок
    if (screen_width_ > new_width) {
        return;
    }
    // Проверка на наезд заполнителей таблицы на саму таблицу
    if (bufer_headers_.front()->GetWidth() + (bufer_headers_.front()->GetWidth() * scale_factor) < min_hat_width_) {
        return;
    }

    for (auto hat : bufer_headers_) {
        hat->SetWidth(hat->GetWidth() + (hat->GetWidth() * scale_factor));

        hat->setX(pos);

        pos = hat->x() + hat->GetWidth();
    }

    /*

    qreal scale_widht = bufer_headers_.front()->GetWidth() * scale_factor;
    qreal steep_pos = 1;

    if (event->delta() < 0) {
        scale_widht = -scale_widht;
        steep_pos = -steep_pos;
    }

    qreal new_hat_width = bufer_headers_.front()->GetWidth() + scale_widht;

    // проверка на выход в видимую часть границы шапок
    if (screen_width_ > new_hat_width * bufer_headers_.size()) {
        new_hat_width = screen_width_ / bufer_headers_.size();

        auto i = 0;

        for (auto hat : bufer_headers_) {
            hat->SetWidth(new_hat_width);
            hat->setX(i * new_hat_width);
            ++i;
        }

        new_width = new_hat_width * bufer_headers_.size();
    } else if (new_hat_width < min_hat_width_) {
        return;
    } else {
        for (auto hat : bufer_headers_) {
            hat->SetWidth(new_hat_width);
            //hat->SetWidth(hat->GetWidth() + scale_widht);

            hat->setX(hat->x() + (hat->x() * scale_factor * steep_pos));

            new_width += hat->GetWidth();
        }
    }*/

    this->setSceneRect({0, 0, new_width, diagram_scene_->sceneRect().height()});

    horizontal_lines_->SetWidth(new_width);

    SetSecondsPerPixel(new_width);

    for (auto [tree_item, diagram_rect] : tree_item_to_diagram_rect_) {
        auto [start, end] = GetStartEndTreeItemTime(tree_item);

        diagram_rect->SetWidthAndRepaint((seconds_per_pixel_) * (end - start));

        diagram_rect->setX(seconds_per_pixel_ * start);
    }

    /*
    if ((*headers_.begin())->GetWidth() / this->rect().width() < 0.25) {
        DeleteItemsFromGroup(group_);
        headers_.clear();

        ++dimension_couner_;

        new_width = MakeDiagramHat();
    } else if ((*headers_.begin())->GetWidth() / this->rect().width() > 1.25) {
        DeleteItemsFromGroup(group_);
        headers_.clear();

        --dimension_couner_;

        new_width = MakeDiagramHat();

    } else {
        for (auto hat : headers_) {
            auto steep = hat->GetWidth() * scaleFactor;
            auto steep_x = hat->x() * scaleFactor;

            if (event->delta() > 0) {
                hat->SetWidth(hat->GetWidth() + steep);

                hat->setX(hat->x() + steep_x);
            } else {
                hat->SetWidth(hat->GetWidth() - steep);

                hat->setX(hat->x() - steep_x);
            }
            new_width += hat->GetWidth();
        }
    }

    SetSecondsPerPixel(new_width);

    for (auto [tree_item, diagram_rect] : tree_item_to_diagram_rect_) {
        auto [start, end] = GetStartEndTreeItemTime(tree_item);

        diagram_rect->SetWidthAndRepaint((seconds_per_pixel_) * (end - start));

        diagram_rect->setX(seconds_per_pixel_ * start);
    }

    //new_width = (this->rect().width() < new_width) ? this->rect().width() : new_width;

    horizontal_lines_->SetWidth(new_width);
    this->setSceneRect({0, 0, new_width, diagram_scene_->sceneRect().height()});*/
}

void DiagramViewer::TreeItemExpanded(QTreeWidgetItem* item) {
    if (tree_item_to_diagram_rect_.count(item)) {
        QRect rect = item->treeWidget()->visualItemRect(item);
        rect.moveTop(rect.y() + head_height_);
        tree_item_to_diagram_rect_[item]->setVisible(true);
        tree_item_to_diagram_rect_[item]->setY(rect.y());
    }

    for (auto i = 0; i < item->childCount(); ++i) {
        QTreeWidgetItem* child = item->child(i);

        QRect rect = child->treeWidget()->visualItemRect(child);

        rect.moveTop(rect.y() + head_height_);

        //отрисовываем детей
        if (!tree_item_to_diagram_rect_.count(child)) {
            MakeDiagramRect(child);
        }

        tree_item_to_diagram_rect_[child]->setPos(tree_item_to_diagram_rect_[child]->x(), rect.y());
        tree_item_to_diagram_rect_[child]->setVisible(true);

        if(child->isExpanded()) {
            TreeItemExpanded(child);
        }
    }

    while (QTreeWidgetItem* parrent = item->parent()) {
        for (auto i = parrent->indexOfChild(item) + 1; i < parrent->childCount(); ++i) {
            QTreeWidgetItem* child = parrent->child(i);

            QRect rect = child->treeWidget()->visualItemRect(child);
            rect.moveTop(rect.y() + head_height_);
            tree_item_to_diagram_rect_[child]->setVisible(true);
            tree_item_to_diagram_rect_[child]->setY(rect.y());

            if (child->childCount() && child->isExpanded()) {
                TreeItemExpanded(child);
            }
        }

        item =  parrent;
    }

    if (!item->parent()) {
        for (auto i = item->treeWidget()->indexOfTopLevelItem(item) + 1; i < item->treeWidget()->topLevelItemCount(); ++i) {
            auto neighbour = item->treeWidget()->topLevelItem(i);

            if (neighbour->isExpanded()) {
                TreeItemExpanded(neighbour);
            }
        }
    }
}



void DiagramViewer::MakeDiagramRect(const QTreeWidgetItem* item) {
    QRect rect = item->treeWidget()->visualItemRect(item);

    auto [start, end] = GetStartEndTreeItemTime(item);

    //TODO: переделать под фабрику
    if (node_to_node_type_[item] == NodeType::Branch) {
        DiagramBranchRect* dbr = new DiagramBranchRect(item->text(0)
                                                       , (seconds_per_pixel_) * (end - start)
                                                       , rect.height()
                                                       );
        diagram_scene_->addItem(dbr);
        dbr->setPos(seconds_per_pixel_ * start, rect.y());

        tree_item_to_diagram_rect_[item] = dbr;
    } else if (node_to_node_type_[item] == NodeType::Leaf) {
        DiagramLeafRect* dlr = new DiagramLeafRect(item->text(3).toLong()
                                                   , (seconds_per_pixel_) * (end - start)
                                                   , rect.height()
                                                   );

        diagram_scene_->addItem(dlr);

        dlr->setPos(seconds_per_pixel_ * start, rect.y());

        tree_item_to_diagram_rect_[item] = dlr;
    }
}

void DiagramViewer::TreeItemCollapsed(QTreeWidgetItem* item) {
    //Скрываем всех детей
    for (auto i = 0; i < item->childCount(); ++i) {
        QTreeWidgetItem* child = item->child(i);

        if (tree_item_to_diagram_rect_.count(child)) {
            tree_item_to_diagram_rect_[child]->setVisible(false);

            TreeItemCollapsed(child);
        }
    }

    while (QTreeWidgetItem* parrent = item->parent()) {
        //Перерисовываем соседей по иерархии ниже
        for (auto i = parrent->indexOfChild(item) + 1; i < parrent->childCount(); ++i) {
            QTreeWidgetItem* child = parrent->child(i);

            if (child->childCount() && child->isExpanded()) {
                TreeItemExpanded(child);
                return;
            }

            QRect rect = child->treeWidget()->visualItemRect(child);
            rect.moveTop(rect.y() + head_height_);
            tree_item_to_diagram_rect_[child]->setVisible(true);
            tree_item_to_diagram_rect_[child]->setY(rect.y());
        }

        item =  parrent;
    }

    if (!item->parent()) {
        for (auto i = item->treeWidget()->indexOfTopLevelItem(item) + 1; i < item->treeWidget()->topLevelItemCount(); ++i) {
            auto neighbour = item->treeWidget()->topLevelItem(i);

            if (neighbour->isExpanded()) {
                TreeItemExpanded(neighbour);
            }
        }
    }
}

void DiagramViewer::SetFirstAndLastDateTime() {
    first_ = tree_item_to_start_and_end_date_time_.begin()->second.first;
    last_ = tree_item_to_start_and_end_date_time_.begin()->second.second;

    for (const auto& [_, first_and_last] : tree_item_to_start_and_end_date_time_) {
        const auto& [lhs, rhs] = first_and_last;

        first_ = (first_ > lhs) ? lhs : first_;

        last_ = (last_ < rhs) ? rhs : last_;
    }
}

std::pair<qreal, qreal> DiagramViewer::GetStartEndTreeItemTime(const QTreeWidgetItem* tree_item) const {
    const auto& [s_t, e_t] = tree_item_to_start_and_end_date_time_.at(tree_item);

    auto s = std::abs(zero_pos_time_point_.secsTo(s_t));
    auto e = std::abs(zero_pos_time_point_.secsTo(e_t));

    return std::make_pair(s, e);
}

void DiagramViewer::SetHatFactory() {
    get_diagram_hat_['S'] = [this](const QDateTime& date_time, int i) {
        auto minutes_to_secs = i * 60;

        diagram_scene_->setSceneRect({0, 0, diagram_scene_->width() + screen_width_, diagram_scene_->sceneRect().height()});

        return new SecondDiagramHat(screen_width_, screen_height_, head_height_, date_time.addSecs(minutes_to_secs));
    };

    get_diagram_hat_['M'] = [this](const QDateTime& date_time, int i) {
        auto hours_to_secs = i * 60 * 60;

        diagram_scene_->setSceneRect({0, 0, diagram_scene_->width() + screen_width_, diagram_scene_->sceneRect().height()});

        return new MinuteDiagramHat(screen_width_, screen_height_, head_height_, date_time.addSecs(hours_to_secs));
    };

    get_diagram_hat_['H'] = [this](const QDateTime& date_time, int i) {
        diagram_scene_->setSceneRect({0, 0, diagram_scene_->width() + screen_width_, diagram_scene_->sceneRect().height()});

        return new HourDiagramHat(screen_width_, screen_height_, head_height_, date_time.date().addDays(i));
    };

    get_diagram_hat_['d'] = [this](const QDateTime& date_time, int i) {
        QDateTime new_date_time = date_time.addMonths(i);

        auto hat_width = seconds_per_pixel_ * (new_date_time.date().daysInMonth() * 24 * 60 * 60);

        diagram_scene_->setSceneRect({0, 0, diagram_scene_->width() + hat_width, diagram_scene_->sceneRect().height()});

        return new DayDiagramHat(hat_width, screen_height_, head_height_, new_date_time.date());
    };

    get_diagram_hat_['D'] = get_diagram_hat_['d'];

    get_diagram_hat_['m'] = [this](const QDateTime& date_time, int i) {
        QDate new_date_time = date_time.date().addYears(i);

        auto hat_width = seconds_per_pixel_ * (new_date_time.daysInYear() * 24 * 60 * 60);

        diagram_scene_->setSceneRect({0, 0, diagram_scene_->width() + hat_width, diagram_scene_->sceneRect().height()});

        return new MonthDiagramHat(hat_width, screen_height_, head_height_, new_date_time);
    };

    get_diagram_hat_['y'] = [this](const QDateTime& date_time, int i) {
        QDate new_date_time = date_time.date().addYears(i);

        auto hat_width = seconds_per_pixel_ * (new_date_time.daysInYear() * 24 * 60 * 60);

        diagram_scene_->setSceneRect({0, 0, diagram_scene_->width() + hat_width, diagram_scene_->sceneRect().height()});

        return new YearDiagramHat(hat_width, screen_height_, head_height_, new_date_time);
    };
}

qint64 DiagramViewer::GetStartPageHatCount() const {
    qint64 hat_number = 0;

    switch (dimension_counter_.GetDimension()) {
    case 'S':
        hat_number = (first_.secsTo(last_) / 60);
        break;
    case 'M':
        hat_number = (first_.secsTo(last_) / (60 * 60));
        break;
    case 'H':
        hat_number = first_.daysTo(last_);
        break;
    case 'd':
    case 'D':
        if (first_.date().year() == last_.date().year()) {
            hat_number = first_.date().month() - last_.date().month();
        } else {
            hat_number = ((12 - first_.date().month()) + last_.date().month()) + ((first_.date().year() - last_.date().year() - 1) * 12);
        }
        break;
    case 'm':
    case 'y':
        hat_number = first_.date().year() - last_.date().year();
        break;
    }

    return std::abs(hat_number);
}

void DiagramViewer::ProcessChangeDimension() {
    SetMinHatWidth();
    SetZeroPosTimePoint();
    DeleteDiagramHat();
    diagram_scene_->setSceneRect({0, 0, 0, diagram_scene_->sceneRect().height()});
    //Строим и выводим в ui шапки диаграммы от first_ до last_
    MakeDiagramHat();

    SetSecondsPerPixel(diagram_scene_->width());

    for (auto [tree_item, diagram_rect] : tree_item_to_diagram_rect_) {
        auto [start, end] = GetStartEndTreeItemTime(tree_item);

        diagram_rect->SetWidthAndRepaint((seconds_per_pixel_) * (end - start));

        diagram_rect->setX(seconds_per_pixel_ * start);
    }

    horizontal_lines_->SetWidth(diagram_scene_->sceneRect().width());
}

void DiagramViewer::SetZeroPosTimePoint() {
    switch (dimension_counter_.GetDimension()) {
    case 'S':
        zero_pos_time_point_ = {first_.date(), {first_.time().hour(), first_.time().minute(), 0}};
        break;
    case 'M':
        zero_pos_time_point_ = {first_.date(), {first_.time().hour(), 0, 0}};
        break;
    case 'H':
        zero_pos_time_point_ = {first_.date(), {0,0,0}};
        break;
    case 'd':
    case 'D':
        zero_pos_time_point_ = QDateTime{QDate{first_.date().year(), first_.date().month(), 1}, QTime{0,0,0}};
        break;
    case 'm':
    case 'y':
        zero_pos_time_point_ = {{first_.date().year(), 1, 1}, {0,0,0}};
        break;
    }
}

void DiagramViewer::SetMinHatWidth() {
    switch (dimension_counter_.GetDimension()) {
    case 'S':
        min_hat_width_ = 10 * 2 * 60;
        break;
    case 'M':
        min_hat_width_ = 10 * 2 * 60;
        break;
    case 'H':
        min_hat_width_ = 10 * 2 * 24;
        break;
    case 'd':
    case 'D':
        // 12 - шрифт в пикселях, 2 - максимально возможно количество символов,  24 - количество столбцов
        min_hat_width_ = 10 * 2 * 31;
        break;
    case 'm':
        min_hat_width_ = 10 * 2 * 12;
    case 'y':
        min_hat_width_ = 10 * 4;
        break;
    }
}

void DiagramViewer::SetDimension(char value) {
    dimension_counter_ = value;
}

//Код отвечающий за добавление удаление новых шапок при крайнем положении скролл бара
/*void DiagramViewer::TestSetValue(int value [[maybe_unused]]) {
    auto pos_x = this->mapToScene(QRect(0, 0,
                                        this->viewport()->width(),
                                        this->viewport()->height())).boundingRect().x();
    //Добавляем элемент справа
    if (pos_x > diagram_scene_->width() - bufer_headers_.back()->GetWidth()) {
        QDateTime date = bufer_headers_.back()->GetTimeDate();

        auto hat = get_diagram_hat_[dimension_counter_.GetDimension()](date, 1);
        bufer_headers_.emplace_back(hat);
        diagram_scene_->addItem(hat);

        //Удаляем крайний левый
        diagram_scene_->removeItem(bufer_headers_.front());
        delete bufer_headers_.front();
        bufer_headers_.pop_front();

        //обновляем координаты шапок
        auto begin = bufer_headers_.begin();
        for (auto i = 0; i < bufer_headers_.size(); ++i, ++begin) {
            (*begin)->setPos(i * screen_width_, 0);
        }
    } // добовляем элемент слева
    else if (pos_x < 1) {
        QDateTime date = bufer_headers_.front()->GetTimeDate();

        auto hat = get_diagram_hat_[dimension_counter_.GetDimension()](date, -1);
        bufer_headers_.emplace_front(hat);
        diagram_scene_->addItem(hat);

        //Удаляем крайний правый
        diagram_scene_->removeItem(bufer_headers_.back());
        delete bufer_headers_.back();
        bufer_headers_.pop_back();

        //обновляем координаты шапок
        auto begin = bufer_headers_.begin();
        for (auto i = 0; i < bufer_headers_.size(); ++i, ++begin) {
            (*begin)->setPos(i * screen_width_, 0);
        }
    }
}*/
