#include <Python.h>

#include <QPainter>
#include <QMenu>
#include <QAction>

#include "ui/canvas/inspector/inspector_buttons.h"
#include "ui/canvas/inspector/inspector.h"
#include "ui/canvas/inspector/inspector_title.h"

#include "ui/util/colors.h"
#include "app/app.h"

#include "graph/script_node.h"
#include "graph/graph_node.h"

InspectorScriptButton::InspectorScriptButton(ScriptNode* n, QGraphicsItem* parent)
    : GraphicsButton(parent), node(n)
{
    setToolTip("Edit script");
    connect(this, &GraphicsButton::pressed,
            this, &InspectorScriptButton::onPressed);

    n->installScriptWatcher(this);
    trigger(n->getScriptState());
}

QRectF InspectorScriptButton::boundingRect() const
{
    return QRectF(0, 0, 16, 15);
}

void InspectorScriptButton::paint(QPainter* painter,
                                const QStyleOptionGraphicsItem* option,
                                QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(Qt::NoPen);
    const QColor base = script_valid ? Colors::base04 : Colors::red;
    painter->setBrush(hover ? Colors::highlight(base) : base);

    painter->drawRect(0, 0, 16, 3);
    painter->drawRect(0, 6, 16, 3);
    painter->drawRect(0, 12, 16, 3);
}

void InspectorScriptButton::trigger(const ScriptState& state)
{
    script_valid = (state.error_lineno == -1);
    prepareGeometryChange();
}

void InspectorScriptButton::onPressed()
{
    App::instance()->newEditorWindow(node);
}

////////////////////////////////////////////////////////////////////////////////

InspectorGraphButton::InspectorGraphButton(GraphNode* n, QGraphicsItem* parent)
    : GraphicsButton(parent), node(n)
{
    setToolTip("Edit graph");
    connect(this, &GraphicsButton::pressed,
            this, &InspectorGraphButton::onPressed);
}

QRectF InspectorGraphButton::boundingRect() const
{
    return QRectF(0, 0, 16, 15);
}

void InspectorGraphButton::paint(QPainter* painter,
                                const QStyleOptionGraphicsItem* option,
                                QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(Qt::NoPen);
    const QColor base = Colors::base04;
    painter->setBrush(hover ? Colors::highlight(base) : base);

    painter->drawEllipse(0, 5, 5, 5);
    painter->drawEllipse(10, 11, 5, 5);
    painter->drawEllipse(10, 0, 5, 5);

    painter->setBrush(Qt::NoBrush);
    painter->setPen(QPen(hover ? Colors::highlight(base) : base, 2));
    painter->drawLine(QLineF(2.5, 7.5, 12.5, 13.5));
    painter->drawLine(QLineF(2.5, 7.5, 12.5, 2.5));
}

void InspectorGraphButton::onPressed()
{
    App::instance()->newCanvasWindowFor(node->getGraph());
}

////////////////////////////////////////////////////////////////////////////////

InspectorViewButton::InspectorViewButton(GraphNode* n, QGraphicsItem* parent)
    : GraphicsButton(parent), node(n)
{
    setToolTip("Open viewport");
    connect(this, &GraphicsButton::pressed,
            this, &InspectorViewButton::onPressed);
}

QRectF InspectorViewButton::boundingRect() const
{
    return QRectF(0, 0, 16, 15);
}

void InspectorViewButton::paint(QPainter* painter,
                                const QStyleOptionGraphicsItem* option,
                                QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setBrush(Qt::NoBrush);
    const QColor base = Colors::base04;
    painter->setPen(QPen(hover ? Colors::highlight(base) : base, 2));

    painter->drawRect(1, 1, 14, 14);
    painter->drawLine(1, 8, 15, 8);
    painter->drawLine(8, 1, 8, 15);
}

void InspectorViewButton::onPressed()
{
    App::instance()->newViewportWindowFor(node->getGraph());
}

////////////////////////////////////////////////////////////////////////////////

InspectorQuadButton::InspectorQuadButton(GraphNode* n, QGraphicsItem* parent)
    : GraphicsButton(parent), node(n)
{
    setToolTip("Open quad window");
    connect(this, &GraphicsButton::pressed,
            this, &InspectorQuadButton::onPressed);
}

QRectF InspectorQuadButton::boundingRect() const
{
    return QRectF(0, 0, 16, 16);
}

void InspectorQuadButton::paint(QPainter* painter,
                                const QStyleOptionGraphicsItem* option,
                                QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setBrush(Qt::NoBrush);
    const QColor base = Colors::base04;
    painter->setPen(QPen(hover ? Colors::highlight(base) : base, 2));

    painter->drawRect(1, 15, 15, 1);
    painter->drawLine(1, 15, 1, 1);
    painter->drawLine(1, 15, 8, 8);
}

void InspectorQuadButton::onPressed()
{
    App::instance()->newQuadWindowFor(node->getGraph());
}

////////////////////////////////////////////////////////////////////////////////
InspectorShowHiddenButton::InspectorShowHiddenButton(
        QGraphicsItem* parent, NodeInspector* inspector)
    : GraphicsButton(parent), toggled(false), inspector(inspector)
{
    connect(this, &GraphicsButton::pressed,
            this, &InspectorShowHiddenButton::onPressed);
    setToolTip("Show hidden datums");

    inspector->getNode()->installWatcher(this);
    trigger(inspector->getNode()->getState());
}

QRectF InspectorShowHiddenButton::boundingRect() const
{
    return QRectF(0, 0, 10, 15);
}

void InspectorShowHiddenButton::paint(QPainter* painter,
                                const QStyleOptionGraphicsItem* option,
                                QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(Qt::NoPen);
    painter->setBrush(hover ? Colors::base06 :
                      toggled ? Colors::base04 : Colors::base02);
    painter->drawRect(0, 12, 10, 3);
    painter->drawEllipse(3, 4, 4, 4);
}

void InspectorShowHiddenButton::onPressed()
{
    toggled = !toggled;
    inspector->setShowHidden(toggled);
}

void InspectorShowHiddenButton::trigger(const NodeState& state)
{
    for (auto d : state.datums)
    {
        if (d->getName().find("_") == 0 &&
            d->getName().find("__") != 0)
        {
            if (!isVisible())
                show();
            return;
        }
    }

    if (isVisible())
        hide();
}

