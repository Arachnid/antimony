#include <Python.h>

#include <QKeySequence>
#include <QMouseEvent>
#include <QDebug>
#include <QDirIterator>
#include <QRegExp>

#include "app/app.h"

#include "graph/node.h"
#include "graph/graph.h"
#include "graph/datum.h"

#include "graph/node/finder.h"
#include "ui_main_window.h"
#include "ui/main_window.h"
#include "ui/canvas/canvas.h"
#include "ui/canvas/inspector/inspector.h"
#include "ui/viewport/viewport.h"
#include "ui/script/script_pane.h"

#include "control/proxy.h"

MainWindow::MainWindow(QString type, Node* n, QWidget *parent) :
    QMainWindow(parent), window_type(type), ui(new Ui::MainWindow),
    root(n)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);

    connectActions(App::instance());
    ui->menuEdit->addAction(App::instance()->undoAction());
    ui->menuEdit->addAction(App::instance()->redoAction());
    setShortcuts();

    Finder::populateMenu(ui->menuAdd, this);

    // Make the 'Shaded' and 'Heightmap' menu items exclusive
    QActionGroup* view_actions = new QActionGroup(this);
    view_actions->addAction(ui->actionShaded);
    view_actions->addAction(ui->actionHeightmap);
    view_actions->setExclusive(true);

    // Accept the global command-line argument '--heightmap'
    // to always open scenes in height-map view.
    if (App::instance()->arguments().contains("--heightmap"))
        ui->actionHeightmap->setChecked(true);

    if (root)
        root->parentGraph()->installWatcher(this);
}

MainWindow::~MainWindow()
{
    delete ui;
    if (root)
        root->parentGraph()->uninstallWatcher(this);
}

void MainWindow::trigger(const GraphState& state)
{
    if (state.nodes.count(root) == 0)
        close();
}

void MainWindow::connectActions(App* app)
{
    // File menu
    connect(ui->actionSave, &QAction::triggered,
            app, &App::onSave);
    connect(ui->actionSaveAs, &QAction::triggered,
            app, &App::onSaveAs);
    connect(ui->actionNew, &QAction::triggered,
            app, &App::onNew);
    connect(ui->actionOpen, &QAction::triggered,
            app, &App::onOpen);
    connect(ui->actionQuit, &QAction::triggered,
            app, &App::onQuit);
    connect(ui->actionClose, &QAction::triggered,
            this, &MainWindow::deleteLater);

    // View window
    connect(ui->actionNewCanvas, &QAction::triggered,
            app, &App::newCanvasWindow);
    connect(ui->actionNewViewport, &QAction::triggered,
            app, &App::newViewportWindow);
    connect(ui->actionNewQuad, &QAction::triggered,
            app, &App::newQuadWindow);

    // Help menu
    connect(ui->actionAbout, &QAction::triggered,
            app, &App::onAbout);
    connect(ui->actionCheckUpdate, &QAction::triggered,
            app, &App::startUpdateCheck);

    // Window title
    setWindowTitle(app->getWindowTitle().arg(window_type));
    connect(app, &App::windowTitleChanged, this,
            [=](QString title){
                this->setWindowTitle(title.arg(window_type));});
}

void MainWindow::setShortcuts()
{
    ui->actionNew->setShortcuts(QKeySequence::New);
    ui->actionOpen->setShortcuts(QKeySequence::Open);
    ui->actionSave->setShortcuts(QKeySequence::Save);
    ui->actionSaveAs->setShortcuts(QKeySequence::SaveAs);
    ui->actionClose->setShortcuts(QKeySequence::Close);
    ui->actionQuit->setShortcuts(QKeySequence::Quit);
    ui->actionCut->setShortcuts(QKeySequence::Cut);
    ui->actionCopy->setShortcuts(QKeySequence::Copy);
    ui->actionPaste->setShortcuts(QKeySequence::Paste);
}

bool MainWindow::isShaded() const
{
    return ui->actionShaded->isChecked();
}
