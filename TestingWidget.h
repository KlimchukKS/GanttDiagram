#ifndef TESTINGWIDGET_H
#define TESTINGWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>

#include "GanttViewer.h"

class TestingWidget : public QWidget
{
    Q_OBJECT
public:
    TestingWidget(GanttViewer* w);
private:
    QVBoxLayout* v_layout_;
    QHBoxLayout* h_top_layout_;
    QHBoxLayout* h_bottom_layout_;
    QHBoxLayout* remove_item_layout_;

    QLineEdit* line_edit_;
    QPushButton* remove_node_;

    QPushButton* second_;
    QPushButton* minute_;
    QPushButton* hours_;
    QPushButton* day_;
    QPushButton* month_;
    QPushButton* year_;

    QPushButton* add_new_root_node_;
    QPushButton* add_new_branch_node_;
    QPushButton* add_new_leaf_node_;

    QString last_branch_node_id_;
    QString last_root_node_id_ = "план_1";

    int root_counter_ = 0;
    int branch_counter_ = 0;
    int leaf_counter_ = 0;

    GanttViewer* w_;
signals:
    void ButtonPressed(char value);
private slots:
    void SecondPressed() {
        emit ButtonPressed('S');
    }
    void MinutePressed() {
        emit ButtonPressed('M');
    }
    void HoursPressed() {
        emit ButtonPressed('H');
    }
    void DayPressed() {
        emit ButtonPressed('d');
    }
    void MonthPressed() {
        emit ButtonPressed('m');
    }
    void YearPressed() {
        emit ButtonPressed('y');
    }

    void BottonAddNewRootNodeIsPressed() {
        QString new_root = "root_id_";

        new_root += QString{}.setNum(++root_counter_);

        w_->AddNode(new_root, new_root);

        last_root_node_id_ = new_root;
    }

    void BottonAddNewBranchNodeIsPressed() {
        QString new_branch = "branch_id_";

        new_branch += QString{}.setNum(++branch_counter_);

        w_->AddNode({new_branch, new_branch, {{2024, 1, 1}, {0, 0}}, {{2024, 2, 1}, {0, 0}}}, DiagramViewer::NodeType::Branch, last_root_node_id_);

        last_branch_node_id_ = new_branch;
        leaf_counter_ = 0;
    }

    void BottonAddNewLeafNodeIsPressed() {
        QString new_leaf = QString{}.setNum(branch_counter_) + "_leaf_id_";

        new_leaf += QString{}.setNum(++leaf_counter_);

        w_->AddNode({new_leaf, new_leaf, {{2024, 1, 1}, {0, 0}}, {{2024, 2, 1}, {0, 0}}}, DiagramViewer::NodeType::Leaf, last_branch_node_id_);
    }

    void RemoveNode() {
        w_->RemoveNode(line_edit_->text());
    }
};

#endif // TESTINGWIDGET_H
