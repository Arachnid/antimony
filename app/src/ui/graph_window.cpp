#include "ui/graph_window.h"
#include "ui_main_window.h"

#include "app/app.h"
#include "ui/canvas/canvas.h"

#include "graph/node.h"

GraphWindow::GraphWindow(Canvas* c)
{
    setCentralWidget(c);
    window_type = "Graph";

    ui->menuView->deleteLater();
    ui->menuReference->deleteLater();

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
