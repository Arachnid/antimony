#include "ui/quad_window.h"
#include "ui_main_window.h"

#include "app/app.h"
#include "ui/viewport/viewport.h"
#include "ui/viewport/viewport_scene.h"
#include "ui/util/colors.h"

#include "graph/node.h"

QuadWindow::QuadWindow(ViewportScene* scene)
    : MainWindow("Quad")
{
    auto g = new QGridLayout();

    ui->menuReference->deleteLater();

    auto top = scene->newViewport();
    auto front = scene->newViewport();
    auto side = scene->newViewport();
    auto other = scene->newViewport();

    for (auto a : {top, front, side})
        for (auto b : {top, front, side})
            if (a != b)
            {
                connect(a, &Viewport::scaleChanged,
                        b, &Viewport::setScale);
                connect(a, &Viewport::centerChanged,
                        b, &Viewport::setCenter);
            }

    for (auto v : {top, front, side, other})
    {
        connect(v, &Viewport::jumpTo,
                App::instance(), &App::jumpToInGraph);
        connect(App::instance(), &App::jumpToInViewport,
                v, &Viewport::onJumpTo);

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
    }

    top->lockAngle(0, 0);
    front->lockAngle(0, -M_PI/2);
    side->lockAngle(-M_PI/2, -M_PI/2);
    other->spinTo(-M_PI/4, -M_PI/4);

    top->hideViewSelector();
    front->hideViewSelector();
    side->hideViewSelector();
    other->hideViewSelector();

    g->addWidget(top, 1, 0);
    g->addWidget(front, 0, 0);
    g->addWidget(side, 0, 1);
    g->addWidget(other, 1, 1);
    g->setContentsMargins(0, 0, 0, 0);
    g->setSpacing(2);

    auto w = new QWidget();
    w->setStyleSheet(QString(
                "QWidget {"
                "   background-color: %1;"
                "}").arg(Colors::base01.name()));
    w->setLayout(g);
    setCentralWidget(w);

    show();
}
