#include "ui/view_window.h"
#include "ui_main_window.h"

#include "app/app.h"
#include "ui/viewport/viewport.h"

#include "graph/node.h"
#include "graph/graph_node.h"
#include "graph/graph.h"

ViewportWindow::ViewportWindow(Viewport* v)
    : MainWindow("Viewport", v->graph->parentNode())
{
    setCentralWidget(v);

    ui->menuReference->deleteLater();

    connect(ui->actionShaded, &QAction::triggered,
            [&]{ v->scene->invalidate(); });
    connect(ui->actionHeightmap, &QAction::triggered,
            [&]{ v->scene->invalidate(); });

    connect(ui->actionHideUI, &QAction::triggered,
            [&](bool b){ if (b) v->hideUI();
                         else   v->showUI(); });

    connect(ui->actionCopy, &QAction::triggered,
            v, &Viewport::onCopy);
    connect(ui->actionCut, &QAction::triggered,
            v, &Viewport::onCut);
    connect(ui->actionPaste, &QAction::triggered,
            v, &Viewport::onPaste);

    connect(v, &Viewport::jumpTo,
            App::instance(), &App::jumpToInGraph);
    connect(App::instance(), &App::jumpToInViewport,
            v, &Viewport::onJumpTo);

    show();
}

