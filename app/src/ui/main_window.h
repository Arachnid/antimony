#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <Python.h>
#include <QMainWindow>

#include "graph/node/constructor.h"
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

    /** Populate a menu with all of the widgets.
     */
    void populateMenu(QMenu* menu, bool recenter=true, Viewport* v=NULL);

    /** Returns True if the Shaded option is checked.
     */
    bool isShaded() const;

    void trigger(const GraphState& state) override;

protected:
    /*
     *  Connects menu actions to App slots.
     */
    void connectActions(App* app);

    /** Sets up main keyboard shortcuts
     *  (because Qt Designer doesn't have a good way to do so)
     */
    void setShortcuts();

    /*
     *  Makes a new object of the given class.
     *
     *  If recenter is true, snaps object to center of canvas or viewport
     *  If v is given, use it as the viewport in which to add the object
     *      (which enables Shift+A adding objects in quad windows)
     */
    void createNew(bool recenter, NodeConstructorFunction f, Viewport* v=NULL);

    /** Adds a particular node to the "Add" menu.
     */
    void addNodeToMenu(QStringList category, QString name, QMenu* menu,
                       bool recenter, NodeConstructorFunction f,
                       Viewport* v=NULL);

    void populateNodeMenu(QMenu* menu, bool recenter=true, Viewport* v=NULL);

    QString window_type;
    Ui::MainWindow *ui;
    Node* root;
};

#endif // MAIN_WINDOW_H
