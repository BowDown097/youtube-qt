#ifndef SPECIALMESSAGE_H
#define SPECIALMESSAGE_H
#include <QJsonValue>
#include <QWidget>

class QLabel;
class QVBoxLayout;

class SpecialMessage : public QWidget
{
public:
    SpecialMessage(const QJsonValue& renderer, QWidget* parent = nullptr,
                   const QString& headerKey = "text", const QString& subtextKey = "subtext",
                   bool subtextItalic = true, const QString& background = "black");
private:
    QLabel* header;
    QVBoxLayout* layout;
    QLabel* subtext;
};

#endif // SPECIALMESSAGE_H
