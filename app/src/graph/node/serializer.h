#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <QObject>
#include <QList>
#include <QPair>
#include <QMap>
#include <QPointF>
#include <QJsonObject>
#include <QJsonArray>

class Datum;
class Node;
class Graph;
class GraphScene;

namespace SceneSerializer
{
    QJsonObject run(const Graph* root, const GraphScene* scene);

    QJsonArray serializeNodes(const Graph* r, const GraphScene* scene);
    QJsonObject serializeNode(const Node* node, const GraphScene* scene);
    QJsonObject serializeDatum(const Datum* datum);

    extern int PROTOCOL_VERSION;
};

#endif // SERIALIZER_H
