#include "giftredemptionmessage.h"
#include "innertube/objects/innertubestring.h"
#include <QApplication>
#include <QBoxLayout>
#include <QLabel>

GiftRedemptionMessage::GiftRedemptionMessage(const QJsonValue& renderer, QWidget* parent)
    : QWidget(parent),
      authorLabel(new QLabel(renderer["authorName"]["simpleText"].toString(), this)),
      layout(new QHBoxLayout(this)),
      messageLabel(new QLabel(" " + InnertubeObjects::InnertubeString(renderer["message"]).text, this))
{
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    authorLabel->setStyleSheet("font-weight: bold; color: #2ba640");
    layout->addWidget(authorLabel);

    messageLabel->setFixedWidth(parent->width() - 40);
    messageLabel->setFont(QFont(qApp->font().toString(), -1, -1, true));
    messageLabel->setWordWrap(true);
    layout->addWidget(messageLabel);
}
