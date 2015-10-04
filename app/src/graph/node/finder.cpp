#include <Python.h>

#include <QObject>
#include <QDirIterator>

#include "graph/node/finder.h"
#include "app/app.h"

#include "ui/viewport/viewport.h"
#include "ui/canvas/canvas.h"

#include "graph/script_node.h"
#include "graph/graph_node.h"

void Finder::createNew(bool recenter, NodeConstructorFunction f,
                       QWidget* window, Viewport* v)
{
    v = v ? v : window->findChild<Viewport*>();
    auto c = window->findChild<Canvas*>();

    Q_ASSERT((v != NULL) ^ (c != NULL));

    QGraphicsView* view = (v != NULL) ?
        static_cast<QGraphicsView*>(v) :
        static_cast<QGraphicsView*>(c);

    if (recenter)
        QCursor::setPos(view->rect().center());

    if (v)
        v->makeNodeAtCursor(f);
    else if (c)
        c->makeNodeAtCursor(f);
}

void Finder::addNodeToMenu(QStringList category, QString name, QMenu* menu,
                           bool recenter, NodeConstructorFunction f,
                           QWidget* window, Viewport* v)
{
    while (!category.isEmpty())
    {
        bool found = false;
        for (auto m : menu->findChildren<QMenu*>(
                    QString(), Qt::FindDirectChildrenOnly))
        {
            if (m->title() == category.first())
            {
                menu = m;
                found = true;
                break;
            }
        }

        if (!found)
            menu = menu->addMenu(category.first());

        category.removeFirst();
    }
    menu->connect(menu->addAction(name), &QAction::triggered,
                  [=]{ createNew(recenter, f, window, v); });
}

void Finder::populateNodeMenu(QMenu* menu, QWidget* window, bool recenter, Viewport* v)
{
    QDirIterator bitr(App::instance()->bundledNodePath(),
                      QDirIterator::Subdirectories);
    QDirIterator uitr(App::instance()->userNodePath(),
                      QDirIterator::Subdirectories);
    QList<QRegExp> title_regexs= {QRegExp(".*title\\('+([^']+)'+\\).*"),
                                  QRegExp(".*title\\(\"+([^\"]+)\"+\\).*")};

    // Extract all of valid filenames into a QStringList.
    QStringList node_filenames;
    for (auto itr : {&bitr, &uitr})
    {
        while (itr->hasNext())
        {
            auto n = itr->next();
            if (n.endsWith(".node"))
                node_filenames.append(n);
        }
    }

    // Sort the list, then populate menus.
    QMap<QString, QPair<QStringList, NodeConstructorFunction>> nodes;
    QStringList node_titles;
    for (auto n : node_filenames)
    {
        QFile file(n);
        if (!file.open(QIODevice::ReadOnly))
            continue;

        QTextStream in(&file);
        QString txt = in.readAll();

        // Find the menu structure for this node
        auto split = n.split('/');
        while (split.first() != "nodes")
            split.removeFirst();
        split.removeFirst();

        // Attempt to extract the title with a regex;
        // falling back to the node's filename otherwise.
        QString title = split.last().replace(".node","");
        split.removeLast();
        for (auto& regex : title_regexs)
            if (regex.exactMatch(txt))
                title = regex.capturedTexts()[1];

        QString name = "n*";
        if (title.size() > 0 && title.at(0).isLetter())
            name = title.at(0).toLower() + QString("*");
        NodeConstructorFunction constructor =
            [=](Graph *r){ return new ScriptNode(name.toStdString(),
                                                 txt.toStdString(), r); };
        nodes[title] = QPair<QStringList, NodeConstructorFunction>(
                split, constructor);
        node_titles.append(title);
    }

    // Put all of the nodes into the Add menu, deferring Export nodes
    // until the end (after a separator).
    node_titles.sort();
    QStringList deferred;
    for (auto title : node_titles)
        if (nodes[title].first.contains("Export"))
            deferred << title;
        else
            addNodeToMenu(nodes[title].first, title, menu,
                          recenter, nodes[title].second, window, v);

    menu->addSeparator();
    for (auto title : deferred)
        addNodeToMenu(nodes[title].first, title, menu,
                      recenter, nodes[title].second, window, v);
}
void Finder::populateMenu(QMenu* menu, QWidget* window,
                          bool recenter, Viewport* v)
{
    // Hard-code important menu names to set their order.
    for (auto c : {"2D", "3D", "2D → 3D", "3D → 2D", "CSG"})
        menu->addMenu(c);
    menu->addSeparator();

    populateNodeMenu(menu, window, recenter, v);

    menu->addSeparator();

    addNodeToMenu(QStringList(), "Script", menu, recenter,
                [](Graph *r){ return new ScriptNode("s*",
                    "import fab\n\n"
                    "title('script')\n"
                    "input('r', float, 1)\n"
                    "output('c', fab.shapes.circle(0, 0, r))",
                    r); }, window, v);
    addNodeToMenu(QStringList(), "Graph", menu, recenter,
            [](Graph *r){ return new GraphNode("g*", r); }, window, v);
}
