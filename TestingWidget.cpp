#include "TestingWidget.h"

#include <QAbstractButton>

TestingWidget::TestingWidget(GanttViewer* w)
    : v_layout_(new QVBoxLayout(this))
    , second_(new QPushButton("seconds"))
    , minute_(new QPushButton("minutes"))
    , hours_(new QPushButton("hours"))
    , day_(new QPushButton("days"))
    , month_(new QPushButton("months"))
    , year_(new QPushButton("years"))
    , add_new_root_node_(new QPushButton("Добавить новый корень"))
    , add_new_branch_node_(new QPushButton("Добавить новую ветку"))
    , add_new_leaf_node_(new QPushButton("Добавить новый лист"))
    , w_{w}
{
    h_top_layout_ = new QHBoxLayout();
    h_bottom_layout_ = new QHBoxLayout();
    remove_item_layout_ = new QHBoxLayout();

    line_edit_ = new QLineEdit();
    remove_node_ = new QPushButton("Удалить запись");

    remove_item_layout_->addWidget(line_edit_);
    remove_item_layout_->addWidget(remove_node_);

    h_top_layout_->addWidget(second_);
    h_top_layout_->addWidget(minute_);
    h_top_layout_->addWidget(hours_);
    h_top_layout_->addWidget(day_);
    h_top_layout_->addWidget(month_);
    h_top_layout_->addWidget(year_);

    h_bottom_layout_->addWidget(add_new_root_node_);
    h_bottom_layout_->addWidget(add_new_branch_node_);
    h_bottom_layout_->addWidget(add_new_leaf_node_);

    v_layout_->addLayout(h_top_layout_);
    v_layout_->addSpacing(10);
    v_layout_->addLayout(h_bottom_layout_);
    v_layout_->addSpacing(20);
    v_layout_->addLayout(remove_item_layout_);

    connect(second_, SIGNAL(pressed()), this, SLOT(SecondPressed()));
    connect(minute_, SIGNAL(pressed()), this, SLOT(MinutePressed()));
    connect(hours_,  SIGNAL(pressed()), this, SLOT(HoursPressed()));
    connect(day_,    SIGNAL(pressed()), this, SLOT(DayPressed()));
    connect(month_,  SIGNAL(pressed()), this, SLOT(MonthPressed()));
    connect(year_,   SIGNAL(pressed()), this, SLOT(YearPressed()));

    connect(this, SIGNAL(ButtonPressed(char)), w, SIGNAL(SetDimension(char)));

    connect(add_new_branch_node_,   SIGNAL(pressed()), this, SLOT(BottonAddNewBranchNodeIsPressed()));
    connect(add_new_leaf_node_,   SIGNAL(pressed()), this, SLOT(BottonAddNewLeafNodeIsPressed()));
    connect(add_new_root_node_,   SIGNAL(pressed()), this, SLOT(BottonAddNewRootNodeIsPressed()));
    connect(remove_node_,   SIGNAL(pressed()), this, SLOT(RemoveNode()));
}
