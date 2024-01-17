#ifndef MESSAGEELEMENT_H
#define MESSAGEELEMENT_H
#include "qtclasshelpermacros.h"
#include <QRect>

class QPainter;

class MessageElement
{
    Q_DISABLE_COPY_MOVE(MessageElement)
public:
    explicit MessageElement(const QSize& size) : m_rect(0, 0, size.width(), size.height()) {}
    bool hasTrailingSpace() const { return trailingSpace; }
    QRect rect() const { return m_rect; }
    void setLine(size_t line) { m_line = line; }
    void setPosition(const QPoint& point) { m_rect.moveTopLeft(point); }

    virtual size_t getSelectionIndexCount() const = 0;
    virtual void paint(QPainter& painter) = 0;
protected:
    bool trailingSpace = true;
private:
    size_t m_line;
    QRect m_rect;
};

#endif // MESSAGEELEMENT_H
