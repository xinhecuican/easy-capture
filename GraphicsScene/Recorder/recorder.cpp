#include "recorder.h"
#include<QDebug>

Recorder::Recorder(QObject* parent) : QObject(parent)
{
}

Recorder::~Recorder()
{
    stack.free();
}

void Recorder::back()
{
    stack.pop();
    emit recordChange();
}

void Recorder::forward()
{
    stack.forward();
    emit recordChange();
}

void Recorder::record(RecordBase* element)
{
    stack.push(element);
    emit recordChange();
}


void Recorder::reset()
{
    stack.reset();
    emit recordChange();
}

bool Recorder::undoAvailiable()
{
    return stack.canUndo();
}

bool Recorder::redoAvailiable()
{
    return stack.canRedo();
}
