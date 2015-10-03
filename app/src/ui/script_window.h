#pragma once

#include "ui/main_window.h"
#include "graph/watchers.h"

class ScriptNode;
class ScriptPane;

class ScriptWindow : public MainWindow
{
public:
    ScriptWindow(ScriptNode* n);

protected slots:
    void openShapesLibrary() const;

    ScriptPane* pane;
};
