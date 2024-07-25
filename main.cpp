#include "GanttViewer.h"

#include <QApplication>

#include <vector>
#include <utility>

#ifdef GANTTDEBUG
#include "TestingWidget.h"
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GanttViewer w;
    TestingWidget tw(&w);

    //Массив с id родителей и id его потомков, формирует связи дерева
    std::vector<std::pair<QString, std::vector<QString>>> tree { {"план_1", {"1_работы_1", "1_работы_2", "1_работы_3"}}
                                                               , {"1_работы_1", {"1_1_работа_1"}}
                                                               , {"1_1_работа_1", {"1_1_работа_2"}}
                                                               , {"1_1_работа_2", {"1_1_работа_3"}}
                                                               , {"1_работы_2", {"1_2_работа_1", "1_2_работа_2", "1_2_работа_3", "1_2_работа_4"}}
                                                               };
    w.EstablishNodeLinks(tree);

    // id узла и его имя
    std::vector<std::pair<QString, QString>>  root_nodes{{"план_1", "План_1"}};

    w.SetRootNodes(root_nodes);

    std::vector<GanttViewer::NodeData> branch_nodes{  {"1_работы_1", "Работы_1", {{2024, 6, 11}, {1, 0}},  {{2024, 6, 11}, {13, 20}}, 7,  true}
                                                    , {"1_работы_2", "Работы_2", {{2024, 6, 11}, {0, 0}},  {{2024, 6, 11}, {16, 40}}, 34, true}
                                                    , {"1_работы_3", "Работы_3", {{2024, 6, 10}, {3, 0}},  {{2024, 6, 12}, {22, 00}}, 0, true}
                                                   };

    w.SetBranchNodes(branch_nodes);

    std::vector<GanttViewer::NodeData> leaf_nodes{ {"1_1_работа_1", "Работа_1", {{2024, 6, 11}, {1, 0}},   {{2024, 6, 11}, {4, 10}}, 100, true}
                                                    , {"1_1_работа_2", "Работа_2", {{2024, 6, 11}, {4, 10}},  {{2024, 6, 11}, {8, 20}}, 77,  true}
                                                    , {"1_1_работа_3", "Работа_3", {{2024, 6, 11}, {8, 20}},  {{2024, 6, 11}, {13, 20}}, 0,  true}
                                                    , {"1_2_работа_1", "Работа_1", {{2024, 6, 11}, {0, 0}},   {{2024, 6, 11}, {2, 0}}, 100,  true}
                                                    , {"1_2_работа_2", "Работа_2", {{2024, 6, 11}, {2, 0}},   {{2024, 6, 11}, {4, 0}}, 53,   true}
                                                    , {"1_2_работа_3", "Работа_3", {{2024, 6, 11}, {4, 0}},   {{2024, 6, 11}, {11, 27}}, 0,  false}
                                                    , {"1_2_работа_4", "Работа_4", {{2024, 6, 11}, {11, 27}}, {{2024, 6, 11}, {16, 40}}, 0,  false}
                                                  };

    w.SetLeafNodes(leaf_nodes);

    w.PrepareForShow();

    tw.setWindowTitle("Окно с интрументами для тестирования");

    w.show();
    tw.show();

    return a.exec();
}
