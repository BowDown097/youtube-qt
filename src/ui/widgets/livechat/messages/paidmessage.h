#ifndef PAIDMESSAGE_H
#define PAIDMESSAGE_H
#include <QJsonValue>
#include <QWidget>

class HttpReply;
class QHBoxLayout;
class QLabel;
class QVBoxLayout;

class PaidMessage : public QWidget
{
public:
    explicit PaidMessage(const QJsonValue& renderer, QWidget* parent = nullptr);
private:
    QLabel* amountLabel;
    QLabel* authorIcon;
    QLabel* authorLabel;
    QWidget* header;
    QHBoxLayout* headerLayout;
    QVBoxLayout* innerHeaderLayout;
    QVBoxLayout* layout;
    QLabel* messageLabel;
private slots:
    void setAuthorIcon(const HttpReply& reply);
};

#endif // PAIDMESSAGE_H
