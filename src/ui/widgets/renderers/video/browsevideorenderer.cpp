#include "browsevideorenderer.h"
#include "ui/widgets/labels/channellabel.h"
#include "ui/widgets/labels/elidedtubelabel.h"
#include "videothumbnailwidget.h"
#include <QBoxLayout>

BrowseVideoRenderer::BrowseVideoRenderer(QWidget* parent)
    : VideoRenderer(parent), hbox(new QHBoxLayout(this)), textVbox(new QVBoxLayout)
{
    setThumbnailSize(QSize(240, 0));
    textVbox->addWidget(titleLabel);
    textVbox->addWidget(channelLabel);
    textVbox->addWidget(metadataLabel);

    hbox->addWidget(thumbnail);
    hbox->addLayout(textVbox, 1);
}
