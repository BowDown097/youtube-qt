#ifndef MESSAGETEXT_H
#define MESSAGETEXT_H
#include <QWidget>

class MessageElement;

struct MessageLine
{
    size_t endCharIndex = 0;
    size_t endIndex = 0;
    QRect rect;
    size_t startCharIndex = 0;
    size_t startIndex = 0;
};

class MessageText : public QWidget
{
public:
    explicit MessageText(const QJsonValue& data, QWidget* parent = nullptr);
protected:
    void paintEvent(QPaintEvent* paintEvent) override;
private:
    std::vector<std::unique_ptr<MessageElement>> elements;
    std::vector<MessageLine> lines;

    size_t charIndex = 0;
    int currentX = 0;
    int currentY = 0;
    int height = 0;
    size_t line = 0;
    int lineHeight = 0;
    size_t lineStart = 0;
    int spaceWidth = 4;

    void addElement(MessageElement* element);
    void addElement(MessageElement* element, int prevIndex);
    void breakLine();
    bool elementFitsInCurrentLine(MessageElement* element);
    int remainingWidth() const;
};

#endif // MESSAGETEXT_H
