#include "control.h"
#include <QPainter>
#include <QTextStream>
#include <QMouseEvent>
#include <QPixmap>
#include <QDebug>
#include <QLabel>
#include <QVBoxLayout>
#include <iostream>
#include <string>
#include <QUndoStack>

static const int setShapeRectCommandId = 1;
static const int setShapeColorCommandId = 2;


/******************************************************************************
** SetShapeColorCommand
*/
SetShapeRectCommand::SetShapeRectCommand(control *doc, const QString &shapeName, const QRect &rect, QUndoCommand *parent):QUndoCommand (parent)
{
    m_doc = doc;
    m_shapeName = shapeName;
    m_oldRect = doc->shape(shapeName).rect();
    m_newRect = rect;
    m_scale=doc->scaleFactor;
}

void SetShapeRectCommand::undo()
{
    m_doc->latestScale=m_scale;
    m_doc->setShapeRect(m_shapeName, m_oldRect);
}

void SetShapeRectCommand::redo()
{
    m_doc->latestScale=m_scale;
    m_doc->setShapeRect(m_shapeName, m_newRect);
}

bool SetShapeRectCommand::mergeWith(const QUndoCommand *command)
{
    if (command->id() != setShapeRectCommandId)
        return false;

    const SetShapeRectCommand *other = static_cast<const SetShapeRectCommand*>(command);
    if (m_shapeName != other->m_shapeName)
        return false;

    m_newRect = other->m_newRect;
    return true;
}

int SetShapeRectCommand::id() const
{
    return setShapeRectCommandId;
}


/******************************************************************************
** AddShapeCommand
*/

AddShapeCommand::AddShapeCommand(control *doc, const Shape &shape, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    m_doc = doc;
    m_shape = shape;
    m_scale= doc->scaleFactor;
}

void AddShapeCommand::undo()
{
    m_doc->deleteShape(m_shapeName);
}

void AddShapeCommand::redo()
{
    m_doc->latestScale=m_scale;
    m_shapeName = m_doc->addShape(m_shape);
}

/******************************************************************************
** RemoveShapeCommand
*/

RemoveShapeCommand::RemoveShapeCommand(control *doc, const QString &shapeName, int &shapeIndex,
                                        QUndoCommand *parent)
    : QUndoCommand(parent)
{
    m_doc = doc;
    m_shape = doc->shape(shapeName);
    m_shapeName = shapeName;
    m_shapeIndex = shapeIndex;
    m_scale= doc->scaleFactor;
}

void RemoveShapeCommand::undo()
{
    m_doc->removed=true;
    m_doc->latestScale=m_scale;
    m_doc->addShape(m_shape);

}

void RemoveShapeCommand::redo()
{
    m_doc->deleteShape(m_shapeName);
}

/******************************************************************************
** RemoveShapesCommand (remove for multiple shapes
*/

RemoveShapesCommand::RemoveShapesCommand(control *doc, QList<Shape> deleted_shapes,
                                        QUndoCommand *parent)
    : QUndoCommand(parent)
{
    m_doc = doc;
    m_deletedList=deleted_shapes;
    m_scale= doc->scaleFactor;
}

void RemoveShapesCommand::undo()
{
    m_doc->removed=true;
    m_doc->latestScale=m_scale;
    m_deletedList=m_doc->addShapes(m_deletedList);
}

void RemoveShapesCommand::redo()
{
    m_doc->deleteShapes(m_deletedList);
}


/******************************************************************************
** SetShapeColorCommand
*/

SetShapeColorCommand::SetShapeColorCommand(control *doc, const QString &shapeName,
                                            const QColor &color, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    m_doc = doc;
    m_shapeName = shapeName;
    m_oldColor = doc->shape(shapeName).color();
    m_newColor = color;
}

void SetShapeColorCommand::undo()
{
    m_doc->setShapeColor(m_shapeName, m_oldColor);
}

void SetShapeColorCommand::redo()
{
    m_doc->setShapeColor(m_shapeName, m_newColor);
}

bool SetShapeColorCommand::mergeWith(const QUndoCommand *command)
{
    if (command->id() != setShapeColorCommandId)
        return false;

    const SetShapeColorCommand *other = static_cast<const SetShapeColorCommand*>(command);
    if (m_shapeName != other->m_shapeName)
        return false;

    m_newColor = other->m_newColor;
    return true;
}

int SetShapeColorCommand::id() const
{
    return setShapeColorCommandId;
}
