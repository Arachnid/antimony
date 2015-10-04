#ifndef NODE_CONSTRUCTOR_H
#define NODE_CONSTRUCTOR_H

#include <Python.h>
#include <functional>

#include "graph/graph.h"
#include "graph/node.h"

typedef Node* (*NodeConstructor)(Graph*);
typedef std::function<Node* (Graph*)> NodeConstructorFunction;

#endif
