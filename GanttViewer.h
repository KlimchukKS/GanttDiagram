#ifndef GANTTVIEWER_H
#define GANTTVIEWER_H

#include "DiagramViewer.h"

#include <QGraphicsView>
#include <QTreeWidget>
#include <QDateTime>

#include <unordered_map>
#include <utility>

class GanttViewer : public QGraphicsView
{
    Q_OBJECT
public:
    struct NodeData {
        QString id;
        QString name;
        QDateTime start_time;
        QDateTime end_time;
        uint8_t completion_rate;
        bool status;
    };
    //Массив id родителей и id его потомков
    using NodeLinks = std::vector<std::pair<QString, std::vector<QString>>>;
    // id узла и его имя
    using RootNodes = std::vector<std::pair<QString, QString>>;
    using Nodes = std::vector<NodeData>;

    GanttViewer(QWidget *parent = nullptr);

    void EstablishNodeLinks(NodeLinks& node_links);

    void SetRootNodes(RootNodes& root_nodes);

    void SetBranchNodes(Nodes& nodes);

    void SetLeafNodes(Nodes& nodes);

    void AddNode(const NodeData& node_data, DiagramViewer::NodeType node_type, const QString& parent_id) {
        // Если нода уже есть в дереве или родителя нет в дереве, то ничего не делаем
        if (node_id_to_tree_widget_item_.count(node_data.id) || !node_id_to_tree_widget_item_.count(parent_id)) {
            return;
        }

        //Устанавливаем связь
        node_id_to_tree_widget_item_[node_data.id] = new QTreeWidgetItem(node_id_to_tree_widget_item_[parent_id]);

        auto item = node_id_to_tree_widget_item_[node_data.id];

        tree_widget_item_to_node_id_[item] = node_data.id;

        SetNodeData(item, node_data);

        diagram_viewer_->AddTreeWidgetItem(item, node_type, std::make_pair(node_data.start_time, node_data.end_time));

        diagram_viewer_->Reprint();
    }

    void AddNode(const QString& id, const QString& name) {
        if (node_id_to_tree_widget_item_.count(id)) {
            return;
        }

        node_id_to_tree_widget_item_[id] = new QTreeWidgetItem(tree_widget_);

        tree_widget_item_to_node_id_[node_id_to_tree_widget_item_[id]] = id;

        node_id_to_tree_widget_item_[id]->setText(0, name);

        diagram_viewer_->AddTreeWidgetItem(node_id_to_tree_widget_item_[id]);

        tree_widget_->setCurrentItem(node_id_to_tree_widget_item_[id]);

        step_horizontal_line_ = tree_widget_->visualItemRect(node_id_to_tree_widget_item_[id]).height();
    }

    void RemoveNode(const QString& id) {
        //Если объекта с таким id нет
        if (!node_id_to_tree_widget_item_.count(id)) {
            //просто выходим
            return;
        }

        ProcessRemoveNode(id);

        diagram_viewer_->Reprint();
    }

    void ProcessRemoveNode(const QString& id) {
        QTreeWidgetItem* node_ptr = node_id_to_tree_widget_item_[id];

        std::vector<QTreeWidgetItem*> children;
        children.reserve(node_ptr->childCount());
        // Заполняем буфер children, необходимый для исключения удаления детей ноды, во время итерирования, по её детям
        for (auto i = 0; i < node_ptr->childCount(); ++i) {
            children.push_back(node_ptr->child(i));
        }

        // Рекурсивно отбираем детей
        for (auto& item : children) {
            // Удаляем детей из tree_widget_ и diagram_viewer_
            ProcessRemoveNode(tree_widget_item_to_node_id_[item]);
        }

        auto is_top_item = tree_widget_->takeTopLevelItem(tree_widget_->indexOfTopLevelItem(node_ptr));

        if (!is_top_item) {
            // Удаляем бокс со сцены
            diagram_viewer_->RemoveTreeWidgetItem(node_id_to_tree_widget_item_[id]);

            //удаляем связь у родителя с ребёнком
            auto parrent = node_id_to_tree_widget_item_[id]->parent();
            parrent->takeChild(parrent->indexOfChild(node_id_to_tree_widget_item_[id]));
        } else {
            diagram_viewer_->UpdateNodesBelow(node_id_to_tree_widget_item_[id]);
            tree_widget_->takeTopLevelItem(tree_widget_->indexOfTopLevelItem(node_id_to_tree_widget_item_[id]));
        }

        //Очищаем все контейнеры от этого объекта
        node_id_to_tree_widget_item_.erase(id);

        tree_widget_item_to_node_id_.erase(node_ptr);
    }

    void PrepareForShow();
private:
    DiagramViewer* diagram_viewer_;

    QTreeWidget* tree_widget_;

    QGraphicsProxyWidget* gpw_tree_;

    QGraphicsScene* scene_;

    qreal step_horizontal_line_;

    //Крайняя единица времени
    //S - секунды, M - минуты, H - часы, d, D - дни, m - месяцы, y - года
    char dimension_ = 'H';

    QString date_format_{"dd.MM.yyyy hh"};

    std::unordered_map<QString, QTreeWidgetItem*> node_id_to_tree_widget_item_;
    std::unordered_map<QTreeWidgetItem*, QString> tree_widget_item_to_node_id_;

    void SetNodeData(QTreeWidgetItem* tree_item, const NodeData& node_data) const;

signals:
    void SetDimension(char dimension);
private slots:
};
#endif // GANTTVIEWER_H
