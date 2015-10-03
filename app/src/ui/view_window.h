#pragma once

#include "ui/main_window.h"

class Viewport;

class ViewportWindow : public MainWindow
{
public:
    ViewportWindow(Viewport* v);
};
