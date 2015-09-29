#include <Python.h>

#include "app/undo/undo_delete_node.h"
#include "app/undo/stack.h"

#include "app/app.h"

#include "graph/node.h"
#include "graph/graph.h"
#include "graph/graph_node.h"
#include "graph/datum.h"

#include "graph/node/serializer.h"
#include "graph/node/deserializer.h"

#include "ui/canvas/graph_scene.h"

UndoDeleteNodeCommand::UndoDeleteNodeCommand(Node* n, QUndoCommand* parent)
    : UndoCommand(parent), n(n), parent(n->parentGraph()->parentNode())
{
    setText("'delete node'");
}

void UndoDeleteNodeCommand::redo()
{
    // Find and save flat lists of all nodes and datums that will be deleted
    // (so that we can do a pointer swap when they're re-created)
    datums = getDatums();

    // Serialize n into data byte array
    data = SceneSerializer::serializeNode(
            n, App::instance()->getGraphScene(n->parentGraph())
                              ->inspectorPositions());

    // Tell the system to delete the node
    n->parentGraph()->uninstall(n);
}

void UndoDeleteNodeCommand::undo()
{
    // Deserialize the saved node.
    Graph* g = parent ? parent->getGraph()
                      : App::instance()->getGraph();

    SceneDeserializer::Info ds;
    SceneDeserializer::deserializeNode(data, g, &ds);

    // Extract the node from the temporary root
    auto old_node = n;
    n = g->childNodes().back();

    // Find the new lists of node and datum pointers
    auto new_datums = getDatums();

    // Swap the node pointers!
    stack->swapPointer(old_node, n);

    for (auto a = datums.begin(); a != datums.end(); ++a)
    {
        Q_ASSERT(new_datums.contains(a.key()));
        stack->swapPointer(a.value(), new_datums[a.key()]);
    }

    App::instance()->getGraphScene(g)->setInspectorPositions(ds.inspectors);
}

QMap<QString, Datum*> UndoDeleteNodeCommand::getDatums() const
{
    QMap<QString, Datum*> out;
    for (auto d : n->childDatums())
        out[QString::fromStdString(d->getName())] = d;
    return out;
}

void UndoDeleteNodeCommand::swapNode(Node* a, Node* b) const
{
    if (n == a)
        n = b;

    if (parent == a)
    {
        Q_ASSERT(dynamic_cast<GraphNode*>(b));
        parent = static_cast<GraphNode*>(b);
    }
}
