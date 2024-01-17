#include "specialmessage.h"
#include "innertube/objects/innertubestring.h"
#include <QApplication>
#include <QBoxLayout>
#include <QLabel>

constexpr const char* stylesheet = R"(
    background: %1;
    border: 1px solid transparent;
    border-radius: 4px;
    color: white;
)";

SpecialMessage::SpecialMessage(const QJsonValue& renderer, QWidget* parent, const QString& headerKey,
                               const QString& subtextKey, bool subtextItalic, const QString& background)
    : QWidget(parent),
      header(new QLabel(this)),
      layout(new QVBoxLayout(this)),
      subtext(new QLabel(InnertubeObjects::InnertubeString(renderer[subtextKey]).text, this))
{
    setAutoFillBackground(true);
    setStyleSheet(QString(stylesheet).arg(background));
    layout->setContentsMargins(0, 0, 0, 0);

    InnertubeObjects::InnertubeString headerString(renderer[headerKey]);
    if (!headerString.text.isEmpty())
    {
        header->setAlignment(Qt::AlignCenter);
        header->setFont(QFont(qApp->font().toString(), -1, QFont::Bold));
        header->setText(headerString.text);
        header->setWordWrap(true);
        layout->addWidget(header);
    }

    subtext->setAlignment(Qt::AlignCenter);
    subtext->setFont(QFont(qApp->font().toString(), -1, -1, subtextItalic));
    subtext->setWordWrap(true);
    layout->addWidget(subtext);
}
