#ifndef DIAGRAMVIEWER_H
#define DIAGRAMVIEWER_H

#include "DiagramRect.h"
#include "DiagramHat.h"
#include "DiagramHorizontalLines.h"
#include "DimensionCounter.h"

#include <QGraphicsView>
#include <QWheelEvent>
#include <QTreeWidgetItem>
#include <QScrollBar>

#include <unordered_map>
#include <deque>

#ifdef GANTTDEBUG

#include <QDebug>

#endif

class DiagramViewer : public QGraphicsView
{
    Q_OBJECT
public:
    DiagramViewer(char dimension = 'H',
                  QWidget *parent = nullptr);

    enum class NodeType {
        Root,
        Branch,
        Leaf
    };

    void BuildCoordinateGrid(qreal width,
                             qreal height,
                             qreal head_height,
                             qreal tree_item_height);

    void AddTreeWidgetItem(QTreeWidgetItem* node, NodeType type, std::pair<QDateTime, QDateTime>&& start_and_end) {
        node_to_node_type_[node] = type;

        tree_item_to_start_and_end_date_time_.emplace(node, std::move(start_and_end));

        if (node->parent()->isExpanded()) {
            MakeDiagramRect(node);

            QRect rect = node->treeWidget()->visualItemRect(node);
            rect.moveTop(rect.y() + head_height_);
            tree_item_to_diagram_rect_[node]->setY(rect.y());
        }
    }

    void AddTreeWidgetItem(const QTreeWidgetItem* node) {
        node_to_node_type_[node] = NodeType::Root;
    }

    void RemoveTreeWidgetItem(QTreeWidgetItem* node) {
        if (tree_item_to_diagram_rect_.count(node)) {
            diagram_scene_->removeItem(tree_item_to_diagram_rect_[node]);
        }

        UpdateNodesBelow(node);

        node_to_node_type_.erase(node);
        tree_item_to_start_and_end_date_time_.erase(node);

        tree_item_to_diagram_rect_.erase(node);
    }

    void UpdateNodesBelow(QTreeWidgetItem* node) {
        // Обходим все родительсие ветви
        while(QTreeWidgetItem* parent = node->parent()) {
            //Перерисовываем соседей по иерархии ниже
            for (auto i = parent->indexOfChild(node) + 1; i < parent->childCount(); ++i) {
                QTreeWidgetItem* child = parent->child(i);

                if (!tree_item_to_diagram_rect_.count(child)) {
                    continue;
                }

                QRect rect = child->treeWidget()->visualItemRect(child);
                rect.moveTop(rect.y() + head_height_ - rect.height());

                tree_item_to_diagram_rect_.at(child)->setVisible(true);
                tree_item_to_diagram_rect_.at(child)->setY(rect.y());
            }

            parent = parent->parent();
        }

        if (!node->parent()) {
            auto tmp1 = node->treeWidget()->topLevelItemCount();
            auto tmp2 = node->treeWidget()->indexOfTopLevelItem(node);

            // Обходим все корни ниже по иерархии
            for (auto i = node->treeWidget()->indexOfTopLevelItem(node) + 1; i < node->treeWidget()->topLevelItemCount(); ++i) {
                std::deque<QTreeWidgetItem*> deq;
                deq.push_back(node->treeWidget()->topLevelItem(i));

                while (!deq.empty()) {
                    // Сдвигаем координаты, если корень раскрыт
                    if (deq.front()->isExpanded()) {
                        // Обходим всех детей и сдвигаем их
                        for (auto i = 0; i < deq.front()->childCount(); ++i) {
                            QTreeWidgetItem* child = deq.front()->child(i);

                            // Если у ребёнка есть дети
                            if (child->childCount()) {
                                // То добавляем его в очередь
                                deq.push_back(child);
                            }

                            // Сдвигаем координаты узла
                            QRect rect = child->treeWidget()->visualItemRect(child);
                            rect.moveTop(rect.y() + head_height_ - rect.height());

                            tree_item_to_diagram_rect_.at(child)->setVisible(true);
                            tree_item_to_diagram_rect_.at(child)->setY(rect.y());
                        }

                    }

                    deq.pop_front();
                }
            }
        }
    }

    void Reprint() {
        SetFirstAndLastDateTime();
        ProcessChangeDimension();
    }

private:
    using TreeItemToStartDateTimeAndEndDateTime = std::unordered_map<const QTreeWidgetItem*, std::pair<QDateTime, QDateTime>>;

    using NodeIdToNodeType = std::unordered_map<const QTreeWidgetItem*, NodeType>;

    //Размер буфера слева и справа
    //const int half_buffer_length_ = 5;

    //QScrollBar* scroll_bar_;

    QGraphicsScene* diagram_scene_;

    DiagramHorizontalLines* horizontal_lines_;

    std::deque<DiagramHat*> bufer_headers_;

    //TODO: В методе BuildCoordinateGrid задаются эти переменные, через const_cast. Вынести в конструкор инициализацию пока не выходит
    const qreal screen_width_ = 0.;
    const qreal screen_height_ = 0.;
    const qreal head_height_ = 0.;

    qreal min_hat_width_;

    qreal scale_factor_ = 1;

    qreal seconds_per_pixel_;

    QDateTime first_;
    QDateTime last_;

    QDateTime zero_pos_time_point_;
    //Крайняя единица времени
    //S - секунды, M - минуты, H - часы, d, D - дни, m - месяцы, y - года
    DimensionCounter dimension_counter_;

    NodeIdToNodeType node_to_node_type_;

    std::unordered_map<const QTreeWidgetItem*, DiagramRect*> tree_item_to_diagram_rect_;

    TreeItemToStartDateTimeAndEndDateTime tree_item_to_start_and_end_date_time_;

    // Фабрика шапок
    std::unordered_map<char, std::function<DiagramHat*(const QDateTime& date_time, int i)>> get_diagram_hat_;

    //--------------------------------------------------------------------------------------------------------------//
    void MakeDiagramRect(const QTreeWidgetItem* item);

    void SetFirstAndLastDateTime();

    std::pair<qreal, qreal> GetStartEndTreeItemTime(const QTreeWidgetItem* tree_item) const;

    void MakeDiagramHat();

    void SetSecondsPerPixel(qreal width);

    void SetHatFactory();

    qint64 GetStartPageHatCount() const;

    void SetMinHatWidth();

    void SetZeroPosTimePoint();

    void DeleteDiagramHat();

private slots:
    void ProcessChangeDimension();
public slots:
    void wheelEvent(QWheelEvent* event) override;
    void TreeItemExpanded(QTreeWidgetItem* item);
    void TreeItemCollapsed(QTreeWidgetItem *item);

    void SetDimension(char value);

    //void TestSetValue(int value);
};

#endif // DIAGRAMVIEWER_H
