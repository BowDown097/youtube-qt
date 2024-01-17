#include "emojielement.h"
#include "http.h"
#include <QPainter>

EmojiElement::EmojiElement(const QJsonValue& json, const QSize& size, QObject* parent)
    : QObject(parent),
      MessageElement(size),
      m_firstShortcut(json["shortcuts"][0].toString()),
      m_loaded(false),
      m_thumbnail(json["image"]["thumbnails"][0]["url"].toString()) {}

void EmojiElement::onError(const QString& message)
{
    qDebug() << "Failed to get emoji" << m_firstShortcut << "for drawing, error message:" << message;
}

void EmojiElement::paint(QPainter& painter)
{
    std::optional<QPixmap> pixmapOpt = pixmap();
    if (pixmapOpt.has_value())
        painter.drawPixmap(rect(), pixmapOpt.value(), QRect());
}

std::optional<QPixmap> EmojiElement::pixmap()
{
    if (!m_loaded)
    {
        m_loaded = true;
        HttpReply* reply = Http::instance().get(m_thumbnail);
        connect(reply, &HttpReply::error, this, &EmojiElement::onError);
        connect(reply, &HttpReply::finished, this, &EmojiElement::setData);
    }

    return !m_pixmap.isNull() ? std::optional(m_pixmap) : std::nullopt;
}

void EmojiElement::setData(const HttpReply& reply)
{
    if (!reply.isSuccessful())
    {
        qDebug() << "Failed to get emoji" << m_firstShortcut << "for drawing, status code:" << reply.statusCode();
        return;
    }

    m_pixmap.loadFromData(reply.body());
}
