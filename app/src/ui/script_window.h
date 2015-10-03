#pragma once

#include "ui/main_window.h"
#include "graph/watchers.h"

class ScriptNode;
class ScriptPane;

class ScriptWindow : public MainWindow, GraphWatcher
{
public:
    ScriptWindow(ScriptNode* n);
    virtual ~ScriptWindow();

    void trigger(const GraphState& state) override;
protected slots:
    void openShapesLibrary() const;

    Graph* graph;
    ScriptPane* pane;
};
