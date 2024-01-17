#include "messagetext.h"
#include "emojielement.h"
#include "textelement.h"
#include <QApplication>
#include <QJsonArray>
#include <QJsonObject>
#include <QPainter>

MessageText::MessageText(const QJsonValue& data, QWidget* parent) : QWidget(parent)
{
    QFontMetrics fontMetrics(qApp->font());
    spaceWidth = fontMetrics.horizontalAdvance(' ');

    /*
    const QJsonArray runs = data["runs"].toArray();
    for (const QJsonValue& run : runs)
    {
        const QJsonObject obj = run.toObject();
        auto begin = obj.begin();
        if (begin == obj.end())
            continue;

        if (begin.key() == "emoji")
            elements.push_back(std::make_unique<EmojiElement>(begin.value()));
        else if (begin.key() == "text")
            elements.push_back(std::make_unique<TextElement>(begin->toString()));
    }
    */
}

void MessageText::addElement(MessageElement* element)
{
    if (!elementFitsInCurrentLine(element))
        breakLine();
    addElement(element, -2);
}

void MessageText::addElement(MessageElement* element, int prevIndex)
{
    bool isAdding = prevIndex == -2;
    QRect elementRect = element->rect();
    int elementLineHeight = elementRect.height();
    lineHeight = std::max(lineHeight, elementLineHeight);

    int xOffset = 0;
    int yOffset = 0;

    element->setPosition(QPoint(currentX + xOffset, currentY - elementRect.height() + yOffset));
    element->setLine(line);

    if (prevIndex == -2)
        elements.push_back(std::unique_ptr<MessageElement>(element));

    currentX = elementRect.width();
    if (element->hasTrailingSpace())
        currentX += spaceWidth;
}

void MessageText::breakLine()
{
    int xOffset = 0;
    for (size_t i = lineStart; i < elements.size(); i++)
    {
        MessageElement* element = elements.at(i).get();
        QRect elementRect = element->rect();
        element->setPosition(QPoint(elementRect.x() + xOffset, elementRect.y() + lineHeight));
    }

    if (!lines.empty())
    {
        MessageLine& lastLine = lines.back();
        lastLine.endCharIndex = charIndex;
        lastLine.endIndex = lineStart;
    }

    lines.push_back({
        .rect = QRect(-100000, currentY, 200000, lineHeight),
        .startCharIndex = charIndex,
        .startIndex = lineStart
    });

    for (size_t i = lineStart; i < elements.size(); i++)
        charIndex = elements[i]->getSelectionIndexCount();

    lineStart = elements.size();
    currentX = 0;
    currentY = lineHeight;
    height = currentY;
    lineHeight = 0;
    line++;
}

bool MessageText::elementFitsInCurrentLine(MessageElement* element)
{
    return element->rect().width() <= remainingWidth();
}

void MessageText::paintEvent(QPaintEvent* paintEvent)
{
    QPainter painter(this);
    for (const std::unique_ptr<MessageElement>& element : elements)
        element->paint(painter);
}

int MessageText::remainingWidth() const
{
    return width() - currentX;
}
