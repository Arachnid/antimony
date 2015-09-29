#ifndef UNDO_DELETE_NODE_H
#define UNDO_DELETE_NODE_H

#include <QList>
#include <QMap>
#include <QJsonObject>

#include "app/undo/undo_command.h"

class GraphNode;

class UndoDeleteNodeCommand : public UndoCommand
{
public:
    /*
     *  n is the target node to delete
     */
    UndoDeleteNodeCommand(Node* n, QUndoCommand* parent=NULL);

    void redo() override;
    void undo() override;

    void swapNode(Node* a, Node* b) const override;

protected:
    QMap<QString, Datum*> getDatums() const;

    mutable Node* n;
    mutable GraphNode* parent;

    QList<Node*> nodes;
    QMap<QString, Datum*> datums;

    QJsonObject data;
};

#endif
