#include <Python.h>

#include <QStringList>

#include "graph/node/serializer.h"
#include "ui/canvas/graph_scene.h"

#include "graph/script_node.h"
#include "graph/graph_node.h"
#include "graph/graph.h"
#include "graph/datum.h"

// Protocol version change-log:
// 2 -> 3:
//   Change hard-coded nodes into matching scripts
// 3 -> 4:
//   Remove ShapeInputDatum (replace with ShapeDatum)
//   _name -> __name; _script -> __script
// 4 -> 5:
//   Switch to plain-text.
// 5 -> 6: (refactored graph engine)
//   Store scripts and names at node level
//   Remove explicit connections array
// 6 -> 7:
//   Add support for subgraphs (backwards-compatible with 6 for loading files)
int SceneSerializer::PROTOCOL_VERSION = 7;

QJsonObject SceneSerializer::run(const Graph* root, const GraphScene* scene)
{
    QJsonObject out;
    out["type"] = "sb";
    out["protocol"] = PROTOCOL_VERSION;
    out["nodes"] = serializeNodes(root, scene);

    return out;
}

QJsonArray SceneSerializer::serializeNodes(const Graph* r, const GraphScene* scene)
{
    QJsonArray out;
    for (auto node : r->childNodes())
        out.append(serializeNode(node, scene));
    return out;
}

QJsonObject SceneSerializer::serializeNode(const Node* node, const GraphScene* scene)
{
    QJsonObject out;

    auto inspectors = scene->inspectorPositions();
    out["inspector"] = QJsonArray({
            inspectors[const_cast<Node*>(node)].x(),
            inspectors[const_cast<Node*>(node)].y()});
    out["name"] = QString::fromStdString(node->getName());
    out["uid"] = int(node->getUID());

    if (auto script_node = dynamic_cast<const ScriptNode*>(node))
    {
        auto expr = QString::fromStdString(script_node->getScript());
        auto a = QJsonArray();
        for (auto line : expr.split("\n"))
            a.append(line);
        out["script"] = a;
    }
    else if (auto graph_node = dynamic_cast<const GraphNode*>(node))
    {
        out["subgraph"] = serializeNodes(graph_node->getGraph(),
                                         scene->getSubscene(graph_node));
    }


    QJsonArray datum_array;
    for (auto d : node->childDatums())
        datum_array.append(serializeDatum(d));
    out["datums"] = datum_array;

    return out;
}

QJsonObject SceneSerializer::serializeDatum(const Datum* datum)
{
    QJsonObject out;

    out["name"] = QString::fromStdString(datum->getName());
    out["uid"] = int(datum->getUID());
    out["expr"] = QString::fromStdString(datum->getText());

    auto t = PyObject_GetAttrString((PyObject*)datum->getType(), "__name__");
    auto m = PyObject_GetAttrString((PyObject*)datum->getType(), "__module__");
    auto type = QString::fromUtf8(PyUnicode_AsUTF8(t));
    auto module = QString::fromUtf8(PyUnicode_AsUTF8(m));
    Py_DECREF(t);
    Py_DECREF(m);

    out["type"] = (module == "builtins") ? type : (module + "." + type);

    return out;
}
