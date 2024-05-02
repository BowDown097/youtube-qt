#include "gridvideorenderer.h"
#include "utils/uiutils.h"
#include <QApplication>
#include <QBoxLayout>

GridVideoRenderer::GridVideoRenderer(QWidget* parent) : VideoRenderer(parent), vbox(new QVBoxLayout(this))
{
    setThumbnailSize(QSize(205, 115));

    titleLabel->setFont(QFont(qApp->font().toString(), qApp->font().pointSize() + 1, QFont::Bold));
    UIUtils::setMaximumLines(titleLabel, 2);

    channelLabel->text->setFont(QFont(qApp->font().toString(), qApp->font().pointSize() - 1));
    UIUtils::setMaximumLines(channelLabel->text, 2);

    metadataLabel->setFont(QFont(qApp->font().toString(), qApp->font().pointSize() - 1));
    metadataLabel->setWordWrap(true);
    UIUtils::setMaximumLines(metadataLabel, 2);

    vbox->addWidget(thumbnail);
    vbox->addWidget(titleLabel);
    vbox->addWidget(channelLabel);
    vbox->addWidget(metadataLabel);
    vbox->addStretch();
}
