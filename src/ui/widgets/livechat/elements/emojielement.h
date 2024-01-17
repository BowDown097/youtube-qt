#ifndef EMOJIELEMENT_H
#define EMOJIELEMENT_H
#include "messageelement.h"
#include <QObject>
#include <QPixmap>

class HttpReply;

class EmojiElement : public QObject, public MessageElement
{
    Q_OBJECT
public:
    explicit EmojiElement(const QJsonValue& json, const QSize& size, QObject* parent = nullptr);
protected:
    size_t getSelectionIndexCount() const override { return 1; }
    void paint(QPainter& painter) override;
private:
    QString m_firstShortcut;
    bool m_loaded;
    QPixmap m_pixmap;
    QString m_thumbnail;

    std::optional<QPixmap> pixmap();
private slots:
    void onError(const QString& message);
    void setData(const HttpReply& reply);
};

#endif // EMOJIELEMENT_H
