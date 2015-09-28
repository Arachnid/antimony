#include <Python.h>

#include "ui/viewport/viewport_scene.h"
#include "ui/viewport/viewport.h"
#include "ui/util/scene.h"
#include "render/render_worker.h"

#include "graph/node.h"
#include "graph/graph_node.h"
#include "graph/datum.h"
#include "graph/graph.h"

#include "control/control.h"
#include "control/control_root.h"
#include "control/proxy.h"

ViewportScene::ViewportScene(Graph* root, QObject* parent)
    : QObject(parent), loading(false)
{
    root->installWatcher(this);
}

void ViewportScene::registerControl(Node* n, long index, Control* c)
{
    connect(c, &Control::glowChanged,
            this, &ViewportScene::onGlowChange);
    connect(c, &Control::glowChanged,
            this, &ViewportScene::glowChanged);

    if (!controls.contains(n))
        controls[n].reset(new ControlRoot(n, this));
    controls[n]->registerControl(index, c);

    for (auto v : viewports)
        new ControlProxy(c, v);
}

Control* ViewportScene::getControl(Node* n, long index) const
{
    if (!controls.contains(n))
        return NULL;
    return controls[n]->get(index);
}

ViewportScene* ViewportScene::getSubscene(GraphNode* n) const
{
    return subgraphs[n].data();
}

Viewport* ViewportScene::newViewport()
{
    auto s = new QGraphicsScene;
    auto v = new Viewport(s);
    connect(v, &QObject::destroyed, s, &QObject::deleteLater);
    connect(s, &QObject::destroyed, [=](){ this->viewports.remove(v); });
    viewports.insert(v);

    for (auto itr = controls.begin(); itr != controls.end(); ++itr)
        itr.value()->makeProxiesFor(v);

    return v;
}

void ViewportScene::trigger(const GraphState& state)
{
    // Store nodes in a Set for O(1) checking
    QSet<Node*> nodes;
    for (auto n : state.nodes)
    {
        nodes.insert(n);
        // If this node is one that we don't have a ControlRoot for,
        // create one (except in the special case when loading is set,
        // which indicates that we're loading the scene)
        if (!loading && !controls.contains(n))
            controls[n].reset(new ControlRoot(n, this),
                              &QObject::deleteLater);

        if (auto gn = dynamic_cast<GraphNode*>(n))
            if (!subgraphs.contains(gn))
                subgraphs[gn].reset(new ViewportScene(gn->getGraph()),
                                    &QObject::deleteLater);
    }

    pruneHash(nodes, &controls);
    pruneHash(nodes, &subgraphs);
}

void ViewportScene::onGlowChange(Node* n, bool g)
{
    if (controls.contains(n))
        controls[n]->setGlow(g);
}

void ViewportScene::checkRender(Node* n, Datum* d)
{
    if (controls.contains(n))
        controls[n]->checkRender(d);
}
