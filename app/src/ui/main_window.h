#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <Python.h>
#include <QMainWindow>

#include "graph/watchers.h"

class Canvas;
class Viewport;
class ScriptDatum;
class Node;
class NodeRoot;
class GraphNode;
class App;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, GraphWatcher
{
    Q_OBJECT
public:
    /*
     *  Constructs a MainWindow object.
     *
     *  type is a string used in the window's title.
     *  n is a node which this window depends on:
     *    If this is a script window, it's the script's parent node
     *    If this is a canvas or viewport, it's the parent node of the
     *      target subgraph (or NULL if we're using the root graph)
     */
    explicit MainWindow(QString type, Node* n, QWidget *parent=0);
    ~MainWindow();

    /** Returns True if the Shaded option is checked.
     */
    bool isShaded() const;

    void trigger(const GraphState& state) override;

    /*
     *  Populates a menu with node creation functions.
     */
    virtual void populateMenu(QMenu* menu, bool recenter, Viewport* v=NULL);

protected:
    /*
     *  Connects menu actions to App slots.
     */
    void connectActions(App* app);

    /** Sets up main keyboard shortcuts
     *  (because Qt Designer doesn't have a good way to do so)
     */
    void setShortcuts();

    QString window_type;
    Ui::MainWindow *ui;
    Node* root;
};

#endif // MAIN_WINDOW_H
