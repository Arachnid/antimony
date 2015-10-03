#pragma once

#include "ui/main_window.h"

#include "graph/watchers.h"

class Canvas;
class GraphNode;

class GraphWindow : public MainWindow, GraphWatcher
{
public:
    GraphWindow(Canvas* c);
    virtual ~GraphWindow();

    void trigger(const GraphState& state);

protected:
    GraphNode* parent;
};
