#pragma once

#include "ui/main_window.h"

class ScriptNode;

class ScriptWindow : public MainWindow
{
public:
    ScriptWindow(ScriptNode* n);
protected slots:
    void openShapesLibrary() const;
};
