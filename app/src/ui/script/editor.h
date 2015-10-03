#ifndef SCRIPT_H
#define SCRIPT_H

#include <QPlainTextEdit>
#include <QPointer>

#include "graph/watchers.h"

class ScriptNode;
namespace Ui { class MainWindow; }

class ScriptEditor: public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit ScriptEditor(ScriptNode* node, QWidget* parent=NULL);

    /*
     *  The ScriptEditor isn't a ScriptWatcher directly, but
     *  its parent ScriptPane is and will call trigger when needed.
     */
    void trigger(const ScriptState& state);

    void setNode(ScriptNode* n);

    /*
     *  Applies fixed-width font and Python syntax highlighter.
     */
    static void styleEditor(QPlainTextEdit* ed);

public slots:
    void onTextChanged();

protected slots:
    void onUndoCommandAdded();

protected:

    /*
     *  Filter out control+Z events.
     */
    bool eventFilter(QObject* obj, QEvent* event);

    /*
     *  Returns the bounding rectangle of the given line.
     */
    void highlightError(int lineno);

    ScriptNode* node;
};

#endif
