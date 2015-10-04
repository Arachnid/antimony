#pragma once

#include <QMenu>
#include <QWidget>

#include "graph/node/constructor.h"

class Viewport;

namespace Finder
{
    /** Populate a menu with all of the widgets.
     */
    void populateMenu(QMenu* menu, QWidget* window, bool recenter=true, Viewport* v=NULL);

    /*
     *  Makes a new object of the given class.
     *
     *  If recenter is true, snaps object to center of canvas or viewport
     *  If v is given, use it as the viewport in which to add the object
     *      (which enables Shift+A adding objects in quad windows)
     */
    void createNew(bool recenter, NodeConstructorFunction f,
                   QWidget* window, Viewport* v=NULL);

    /** Adds a particular node to the "Add" menu.
     */
    void addNodeToMenu(QStringList category, QString name, QMenu* menu,
                       bool recenter, NodeConstructorFunction f,
                       QWidget* window, Viewport* v=NULL);

    void populateNodeMenu(QMenu* menu, QWidget* window, bool recenter=true, Viewport* v=NULL);
}
