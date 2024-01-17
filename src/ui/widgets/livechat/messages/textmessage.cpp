#include "textmessage.h"
#include "http.h"
#include "utils/uiutils.h"
#include <QApplication>
#include <QBoxLayout>
#include <QLabel>

constexpr const char* imgPlaceholder = "<img src='data:%1;base64,%2' width='20' height='20'>";

TextMessage::TextMessage(const QJsonValue& renderer, QWidget* parent)
    : QWidget(parent),
      authorIcon(new QLabel(this)),
      authorLabel(new QLabel(renderer["authorName"]["simpleText"].toString(), this)),
      contentLayout(new QVBoxLayout),
      headerLayout(new QHBoxLayout),
      layout(new QHBoxLayout(this)),
      messageLabel(new QLabel(this)),
      timestampLabel(new QLabel(this))
{
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    authorIcon->setFixedSize(38, 32);
    layout->addWidget(authorIcon);

    HttpReply* iconReply = Http::instance().get(renderer["authorPhoto"]["thumbnails"][0]["url"].toString());
    connect(iconReply, &HttpReply::finished, this, &TextMessage::setAuthorIcon);

    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);
    contentLayout->addStretch();
    layout->addLayout(contentLayout);

    authorLabel->setMaximumWidth(parent->width() - 150);
    authorLabel->setStyleSheet(renderer["authorBadges"].isObject() ? "font-weight: bold; color: #2ba640" : "font-weight: bold");

    timestampLabel->setFont(QFont(qApp->font().toString(), qApp->font().pointSize() - 2));
    if (renderer["timestampText"].isObject())
    {
        timestampLabel->setText(renderer["timestampText"]["simpleText"].toString());
    }
    else
    {
        quint64 timestampUsec = renderer["timestampUsec"].toString().toULongLong();
        timestampLabel->setText(QDateTime::fromSecsSinceEpoch(timestampUsec / 1000000)
                                    .toString(QLocale::system().timeFormat(QLocale::ShortFormat)));
    }

    headerLayout->addWidget(authorLabel);
    headerLayout->addWidget(timestampLabel);
    headerLayout->addStretch();
    headerLayout->setSpacing(5);
    contentLayout->addLayout(headerLayout);

    messageLabel->setFixedWidth(parent->width() - 70);
    messageLabel->setTextFormat(Qt::RichText);
    messageLabel->setWordWrap(true);
    contentLayout->addWidget(messageLabel);

    const QJsonArray runs = renderer["message"]["runs"].toArray();
    for (const QJsonValue& v2 : runs)
    {
        if (v2["emoji"].isObject())
        {
            QString url = v2["emoji"]["image"]["thumbnails"][0]["url"].toString();
            QString placeholder = QString(imgPlaceholder)
                .arg(v2["emoji"]["shortcuts"][0].toString() + v2["emoji"]["searchTerms"][0].toString() + url);
            if (!messageLabel->text().contains(placeholder))
            {
                HttpReply* emojiReply = Http::instance().get(url);
                connect(emojiReply, &HttpReply::finished, this,
                        std::bind(&TextMessage::insertEmojiIntoMessage, this, std::placeholders::_1, placeholder));
            }

            messageLabel->setText(messageLabel->text() + placeholder);
        }
        else if (v2["text"].isString())
        {
            messageLabel->setText(messageLabel->text() + v2["text"].toString());
        }
    }
}

void TextMessage::insertEmojiIntoMessage(const HttpReply& reply, const QString& placeholder)
{
    QString data = QString(imgPlaceholder).arg(reply.header("content-type"), reply.body().toBase64());
    messageLabel->setText(messageLabel->text().replace(placeholder, data));
}

void TextMessage::setAuthorIcon(const HttpReply& reply)
{
    QPixmap pixmap;
    pixmap.loadFromData(reply.body());
    pixmap = pixmap.scaled(32, 32, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    authorIcon->setPixmap(UIUtils::pixmapRounded(pixmap, 9999, 9999));
}
