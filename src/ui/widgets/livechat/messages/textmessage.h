#ifndef TEXTMESSAGE_H
#define TEXTMESSAGE_H
#include <QJsonValue>
#include <QWidget>

class HttpReply;
class QHBoxLayout;
class QLabel;
class QVBoxLayout;

class TextMessage : public QWidget
{
public:
    TextMessage(const QJsonValue& renderer, QWidget* parent);
private:
    QLabel* authorIcon;
    QLabel* authorLabel;
    QVBoxLayout* contentLayout;
    QHBoxLayout* headerLayout;
    QHBoxLayout* layout;
    QLabel* messageLabel;
    QLabel* timestampLabel;
private slots:
    void insertEmojiIntoMessage(const HttpReply& reply, const QString& placeholder);
    void setAuthorIcon(const HttpReply& reply);
};

#endif // TEXTMESSAGE_H
