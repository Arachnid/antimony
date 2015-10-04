#include "ui/graph_window.h"
#include "ui_main_window.h"

#include "app/app.h"
#include "ui/canvas/canvas.h"

#include "graph/node.h"
#include "graph/graph_node.h"
#include "graph/graph.h"

GraphWindow::GraphWindow(Canvas* c)
    : MainWindow("Graph", c->graph->parentNode())
{
    setCentralWidget(c);

    ui->menuView->deleteLater();
    ui->menuReference->deleteLater();

    if (c->graph->parentNode())
        c->populateDatumCommands(ui->menuAdd, c->graph->parentNode());

    connect(ui->actionCopy, &QAction::triggered,
            c, &Canvas::onCopy);
    connect(ui->actionCut, &QAction::triggered,
            c, &Canvas::onCut);
    connect(ui->actionPaste, &QAction::triggered,
            c, &Canvas::onPaste);

    connect(App::instance(), &App::jumpToInGraph,
            c, &Canvas::onJumpTo);
    show();
}
