#include "channellabel.h"
#include "channelbadgelabel.h"
#include "innertube/objects/channel/metadatabadge.h"
#include "utils/uiutils.h"
#include <QBoxLayout>

ChannelLabel::ChannelLabel(QWidget* parent)
    : QWidget(parent), text(new TubeLabel(this)), badgeLayout(new QHBoxLayout), layout(new QHBoxLayout(this))
{
    text->setClickable(true, true);
    text->setContextMenuPolicy(Qt::CustomContextMenu);
    text->setWordWrap(true);
    UIUtils::setMaximumLines(text, 1);

    layout->addWidget(text);
    layout->setContentsMargins(0, 0, 0, 0);

    badgeLayout->addStretch();
    badgeLayout->setSpacing(2);
    layout->addLayout(badgeLayout);
}

void ChannelLabel::reset()
{
    text->clear();
    for (int i = 0; i < badgeLayout->count() - 1; i++)
    {
        QLayoutItem* item = badgeLayout->takeAt(i);
        if (QWidget* widget = item->widget())
            widget->deleteLater();
        delete item;
    }
}

void ChannelLabel::setInfo(const QString& channelName, const QList<InnertubeObjects::MetadataBadge>& badges)
{
    reset();
    text->setText(channelName);

    for (const InnertubeObjects::MetadataBadge& badge : badges)
    {
        ChannelBadgeLabel* badgeLabel = new ChannelBadgeLabel(this);
        badgeLabel->setData(badge);
        badgeLayout->insertWidget(badgeLayout->count() - 1, badgeLabel);
    }
}
