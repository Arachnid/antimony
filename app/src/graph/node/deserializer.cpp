#include <Python.h>

#include <QDataStream>
#include <QJsonDocument>
#include <QBuffer>
#include <QFile>
#include <QTextStream>

#include "app/app.h"

#include "graph/node/deserializer.h"
#include "graph/node.h"
#include "graph/script_node.h"
#include "graph/graph.h"

PyObject* SceneDeserializer::globals = NULL;

bool SceneDeserializer::run(QJsonObject in, Graph* graph, Info* info)
{
    // Check that the "type" field is "sb"
    if (in.find("type") == in.end() || in["type"].toString() != "sb")
    {
        if (info)
            info->error_message = "Could not recognize file.<br><br>"
                                  "If this file was saved in Antimony 0.7.7 or earlier, "
                                  "open it in Antimony 0.7.8 and re-save to upgrade to the current file format.";
        return false;
    }

    // Check file saving protocol
    if (in.find("protocol") == in.end())
    {
        if (info)
            info->error_message = "Could not detect protocol";
        return false;
    }
    else
    {
        auto protocol_version = in["protocol"].toDouble();
        if (protocol_version < 6)
        {
            if (info)
                info->error_message = "File was saved with a older protocol and can no longer be read.";
            return false;
        }
        else if (protocol_version > 6)
        {
            if (info)
                info->error_message = "File was saved with a newer protocol and cannot yet be read.";
            return false;
        }
    }

    // Make sure there's a "nodes" array
    if (in.find("nodes") == in.end() || !in["nodes"].isArray())
    {
        if (info)
            info->error_message = "File does not contain any nodes.";
        return false;
    }

    for (auto n : in["nodes"].toArray())
        deserializeNode(n.toObject(), graph, info);
    return true;
}

void SceneDeserializer::deserializeNode(QJsonObject in, Graph* p, Info* info)
{
    Node* node = NULL;
    if (in.contains("script"))
    {
        node = new ScriptNode(in["name"].toString().toStdString(),
                              in["uid"].toDouble(), p);
    }
    else
    {
        assert(false);
    }

    // Deserialize inspector position
    auto a = in["inspector"].toArray();
    if (info)
        info->inspectors[node] = QPointF(a[0].toDouble(), a[1].toDouble());

    for (auto d : in["datums"].toArray())
        deserializeDatum(d.toObject(), node);

    if (auto script_node = dynamic_cast<ScriptNode*>(node))
    {
        QStringList s;
        for (auto line : in["script"].toArray())
            s.append(line.toString());
        script_node->setScript(s.join("\n").toStdString());
    }
}

void SceneDeserializer::deserializeDatum(QJsonObject in, Node* node)
{
    if (!globals)
    {
        globals = Py_BuildValue("{sO}", "__builtins__", PyEval_GetBuiltins());
        PyDict_SetItemString(globals, "_fabtypes",
                             PyImport_ImportModule("_fabtypes"));
    }

    auto t = PyRun_String(
                  in["type"].toString().toStdString().c_str(),
                  Py_eval_input, globals, globals);
    Q_ASSERT(t);
    Q_ASSERT(PyType_Check(t));

    new Datum(in["name"].toString().toStdString(),
              in["uid"].toDouble(),
              in["expr"].toString().toStdString(),
              (PyTypeObject*)t, node);
}
