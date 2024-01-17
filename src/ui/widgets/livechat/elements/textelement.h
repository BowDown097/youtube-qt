#ifndef TEXTELEMENT_H
#define TEXTELEMENT_H
#include "messageelement.h"
#include <QString>

class TextElement : public MessageElement
{
public:
    explicit TextElement(const QSize& size, const QString& text) : MessageElement(size), text(text) {}
protected:
    void paint(QPainter& painter) override;
private:
    QString text;
};


#endif // TEXTELEMENT_H
