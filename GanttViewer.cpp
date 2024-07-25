#include "GanttViewer.h"

#include "DiagramRect.h"

#include <QTreeWidget>
#include <QHeaderView>
#include <QGraphicsProxyWidget>

#include <QItemDelegate>
#include <QApplication>
#include <QProgressBar>
#include <QAbstractItemView>
#include <QGuiApplication>
#include <QScreen>

#include <QDebug>

class ItemDelegate : public QItemDelegate
{
public:
    ItemDelegate(QObject *poParent = Q_NULLPTR) :
        QItemDelegate(poParent)
    {
    }

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QSize oSize = QItemDelegate::sizeHint(option, index);

        return oSize * 2;
    }
};

GanttViewer::GanttViewer(QWidget *parent)
    : QGraphicsView(parent)
{
    tree_widget_ = new QTreeWidget();

    tree_widget_->setSortingEnabled(false);
    // Устанавливаем струтуру изменяющую высоту строки с узлом
    tree_widget_->setItemDelegate(new ItemDelegate);

    QStringList lst;
    lst << "Имя" << "Старт" << "Окончание" << "Процент выполнения" << "Состояние";
    tree_widget_->setHeaderLabels(lst);

    QHeaderView* h = tree_widget_->header();
    h->setSectionsMovable(false);
    h->setCascadingSectionResizes(false);

    h->setMinimumHeight(h->height() * 2);

    tree_widget_->setMinimumWidth(h->length());
    tree_widget_->setMaximumWidth(h->length());

    tree_widget_->setMinimumHeight(QGuiApplication::primaryScreen()->geometry().height() - h->height() - 30);

    scene_ = new QGraphicsScene;
    gpw_tree_ = scene_->addWidget(tree_widget_);

    qreal size_h = h->viewport()->height();
    qreal size_w = h->viewport()->width();

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect  screenGeometry = screen->geometry();
    //int screen_height = screenGeometry.height();
    int screen_width = screenGeometry.width();

    /*DiagrammHead* dh = new DiagrammHead(size_h, screen_width - (size_w + 5));
    dh->setPos({size_w + 2, 0});
    scene_->addItem(dh);*/

    this->setScene(scene_);
    this->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    //this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    diagram_viewer_ = new DiagramViewer();
    connect(tree_widget_, SIGNAL(itemExpanded(QTreeWidgetItem*)), diagram_viewer_, SLOT(TreeItemExpanded(QTreeWidgetItem*)));
    connect(tree_widget_, SIGNAL(itemCollapsed(QTreeWidgetItem*)), diagram_viewer_, SLOT(TreeItemCollapsed(QTreeWidgetItem*)));
    //connect(this, SLOT(wheelEvent(QWheelEvent* event)), diagram_viewer_, SLOT(wheelEvent(QWheelEvent* event)));

    connect(this, SIGNAL(SetDimension(char)), diagram_viewer_, SLOT(SetDimension(char)));
}

void GanttViewer::EstablishNodeLinks(NodeLinks& node_links) {
    for (auto& [parent_id, children] : node_links) {
        if (!node_id_to_tree_widget_item_.count(parent_id)) {
            node_id_to_tree_widget_item_[parent_id] = new QTreeWidgetItem(tree_widget_);

            tree_widget_item_to_node_id_[node_id_to_tree_widget_item_[parent_id]] = parent_id;
        }

        for (auto& child_id : children) {
            if (!node_id_to_tree_widget_item_.count(child_id)) {
                node_id_to_tree_widget_item_[child_id] = new QTreeWidgetItem(node_id_to_tree_widget_item_[parent_id]);

                tree_widget_item_to_node_id_[node_id_to_tree_widget_item_[child_id]] = child_id;
            }
        }
    }
}

void GanttViewer::SetRootNodes(RootNodes& root_nodes) {
    for (auto& [id, name] : root_nodes) {
        if (!node_id_to_tree_widget_item_.count(id)) {
            continue;
        }

        node_id_to_tree_widget_item_[id]->setText(0, name);

        diagram_viewer_->AddTreeWidgetItem(node_id_to_tree_widget_item_[id]);

        tree_widget_->setCurrentItem(node_id_to_tree_widget_item_[id]);

        step_horizontal_line_ = tree_widget_->visualItemRect(node_id_to_tree_widget_item_[id]).height();
    }
}

void GanttViewer::SetNodeData(QTreeWidgetItem* tree_item, const NodeData& node_data) const {
    tree_item->setText(0, node_data.name);
    tree_item->setText(1, node_data.start_time.toString(date_format_));
    tree_item->setText(2, node_data.end_time.toString(date_format_));
    tree_item->setText(3, QString().setNum(node_data.completion_rate));
    tree_item->setText(4, QString().setNum(node_data.status));
}

void GanttViewer::SetBranchNodes(Nodes& nodes) {
    for (auto& node_data : nodes) {
        if (!node_id_to_tree_widget_item_.count(node_data.id)) {
            continue;
        }
        auto item = node_id_to_tree_widget_item_[node_data.id];
        SetNodeData(item, node_data);

        diagram_viewer_->AddTreeWidgetItem(item, DiagramViewer::NodeType::Branch, std::make_pair(node_data.start_time, node_data.end_time));
    }
}

void GanttViewer::SetLeafNodes(Nodes& nodes) {
    for (auto& node_data : nodes) {
        if (!node_id_to_tree_widget_item_.count(node_data.id)) {
            continue;
        }
        auto item = node_id_to_tree_widget_item_[node_data.id];
        SetNodeData(item, node_data);

        diagram_viewer_->AddTreeWidgetItem(item, DiagramViewer::NodeType::Leaf, std::make_pair(node_data.start_time, node_data.end_time));
    }
}

void GanttViewer::PrepareForShow() {
    double screen_width = QGuiApplication::primaryScreen()->geometry().width();
    double screen_height = QGuiApplication::primaryScreen()->geometry().height() - tree_widget_->header()->height() - 30;
    qreal tree_widget_width = tree_widget_->header()->viewport()->width();
    qreal tree_widget_height = tree_widget_->header()->viewport()->height();
    double width = screen_width - tree_widget_width - 5;

    diagram_viewer_->setMinimumWidth(width);
    diagram_viewer_->setMinimumHeight(screen_height);
    diagram_viewer_->BuildCoordinateGrid(width, screen_height, tree_widget_->header()->height(), step_horizontal_line_);

    auto tmpdv = scene_->addWidget(diagram_viewer_);
    tmpdv->setPos(tree_widget_width, 0);
}
