#include "videorenderer.h"
#include "http.h"
#include "innertube.h"
#include "qttubeapplication.h"
#include "utils/uiutils.h"
#include "ui/views/viewcontroller.h"
#include <QMenu>
#include <QMessageBox>

VideoRenderer::VideoRenderer(QWidget* parent)
    : QWidget(parent),
      channelLabel(new ChannelLabel(this)),
      metadataLabel(new TubeLabel(this)),
      thumbnail(new VideoThumbnailWidget(this)),
      titleLabel(new ElidedTubeLabel(this))
{
    titleLabel->setClickable(true, true);
    titleLabel->setContextMenuPolicy(Qt::CustomContextMenu);
    titleLabel->setFont(QFont(qApp->font().toString(), qApp->font().pointSize() + 2, QFont::Bold));

    connect(channelLabel->text, &TubeLabel::clicked, this, &VideoRenderer::navigateChannel);
    connect(channelLabel->text, &TubeLabel::customContextMenuRequested, this, &VideoRenderer::showChannelContextMenu);
    connect(thumbnail, &VideoThumbnailWidget::clicked, this, &VideoRenderer::navigateVideo);
    connect(thumbnail, &VideoThumbnailWidget::thumbnailSet, this, &VideoRenderer::elideTitle);
    connect(titleLabel, &ElidedTubeLabel::clicked, this, &VideoRenderer::navigateVideo);
    connect(titleLabel, &TubeLabel::customContextMenuRequested, this, &VideoRenderer::showTitleContextMenu);
}

void VideoRenderer::copyChannelUrl()
{
    UIUtils::copyToClipboard("https://www.youtube.com/channel/" + channelId);
}

void VideoRenderer::copyDirectUrl()
{
    auto reply = InnerTube::instance().get<InnertubeEndpoints::Player>(videoId);
    connect(reply, &InnertubeReply<InnertubeEndpoints::Player>::exception, this, [this] {
        QMessageBox::critical(this, "Failed to copy to clipboard", "Failed to copy the direct video URL to the clipboard. The video is likely unavailable.");
    });
    connect(reply, &InnertubeReply<InnertubeEndpoints::Player>::finished, this, [this](const InnertubeEndpoints::Player& endpoint) {
        if (endpoint.response.videoDetails.isLive || endpoint.response.videoDetails.isLiveContent)
        {
            UIUtils::copyToClipboard(endpoint.response.streamingData.hlsManifestUrl);
        }
        else
        {
            QList<InnertubeObjects::StreamingFormat>::const_iterator best = std::ranges::max_element(
                endpoint.response.streamingData.formats,
                [](const auto& a, const auto& b) { return a.bitrate < b.bitrate; }
            );

            if (best == endpoint.response.streamingData.formats.end())
            {
                QMessageBox::critical(this, "Failed to copy to clipboard", "Failed to copy the direct video URL to the clipboard. The video is likely unavailable.");
                return;
            }

            UIUtils::copyToClipboard(best->url);
        }
    });
}

void VideoRenderer::copyVideoUrl()
{
    UIUtils::copyToClipboard("https://www.youtube.com/watch?v=" + videoId);
}

void VideoRenderer::elideTitle()
{
    if (targetElisionWidth > 0)
        UIUtils::elide(titleLabel, targetElisionWidth);
}

void VideoRenderer::navigateChannel()
{
    ViewController::loadChannel(channelId);
}

void VideoRenderer::navigateVideo()
{
    ViewController::loadVideo(videoId, progress);
}

void VideoRenderer::setData(const InnertubeObjects::Reel& reel)
{
    channelId = reel.owner.id;
    videoId = reel.videoId;

    channelLabel->setInfo(reel.owner.name, reel.owner.badges);
    metadataLabel->setText(reel.viewCountText.text);
    thumbnail->setLengthText("SHORTS");

    QString title = QString(reel.headline).replace("\r\n", " ");
    titleLabel->setText(title);
    titleLabel->setToolTip(title);
}

void VideoRenderer::setData(const InnertubeObjects::Video& video)
{
    channelId = video.owner.id;
    progress = video.navigationEndpoint["watchEndpoint"]["startTimeSeconds"].toInt();
    videoId = video.videoId;

    QStringList metadataList {
        video.publishedTimeText.text,
        qtTubeApp->settings().condensedCounts ? video.shortViewCountText.text : video.viewCountText.text
    };
    metadataList.removeAll({});

    channelLabel->setInfo(video.owner.name, video.owner.badges);
    metadataLabel->setText(metadataList.join(" • "));

    thumbnail->setLengthText(video.lengthText.text);
    thumbnail->setProgress(progress, QTime(0, 0).secsTo(video.length()));

    QString title = QString(video.title.text).replace("\r\n", " ");
    titleLabel->setText(title);
    titleLabel->setToolTip(title);
}

void VideoRenderer::setDeArrowData(const HttpReply& reply, const QString& thumbFallbackUrl)
{
    if (!reply.isSuccessful())
    {
        thumbnail->setUrl(thumbFallbackUrl);
        return;
    }

    QJsonValue obj = QJsonDocument::fromJson(reply.body()).object();
    const QJsonArray titles = obj["titles"].toArray();
    const QJsonArray thumbs = obj["thumbnails"].toArray();

    auto validReplacement = [](const QJsonArray& a) {
        return !a.isEmpty() && (a[0]["locked"].toBool() || a[0]["votes"].toInt() >= 0);
    };

    if (qtTubeApp->settings().deArrowTitles && validReplacement(titles))
    {
        titleLabel->setText(titles[0]["title"].toString());
        titleLabel->setToolTip(titles[0]["title"].toString());
    }

    if (qtTubeApp->settings().deArrowThumbs && validReplacement(thumbs))
        thumbnail->setUrl(QStringLiteral("https://dearrow-thumb.ajay.app/api/v1/getThumbnail?videoID=%1&timestamp=%2").arg(videoId).arg(thumbs[0]["timestamp"].toDouble()));
    else
        thumbnail->setUrl(thumbFallbackUrl);
}

void VideoRenderer::setThumbnail(const QString& url)
{
    if (qtTubeApp->settings().deArrow)
    {
        HttpReply* arrowReply = Http::instance().get("https://sponsor.ajay.app/api/branding?videoID=" + videoId);
        connect(arrowReply, &HttpReply::finished, this, std::bind(&VideoRenderer::setDeArrowData, this, std::placeholders::_1, url));
    }
    else
    {
        thumbnail->setUrl(url);
    }
}

void VideoRenderer::setThumbnailSize(const QSize& size)
{
    thumbnail->setPreferredSize(size);
    if (size.height() > 0)
    {
        metadataLabel->setFixedWidth(size.width());
        titleLabel->setFixedWidth(size.width());
    }
}

void VideoRenderer::showChannelContextMenu(const QPoint& pos)
{
    QMenu* menu = new QMenu(this);

    QAction* copyUrlAction = new QAction("Copy channel page URL", this);
    connect(copyUrlAction, &QAction::triggered, this, &VideoRenderer::copyChannelUrl);

    menu->addAction(copyUrlAction);
    menu->popup(channelLabel->mapToGlobal(pos));
}

void VideoRenderer::showTitleContextMenu(const QPoint& pos)
{
    QMenu* menu = new QMenu(this);

    QAction* copyDirectAction = new QAction("Copy direct video URL", this);
    connect(copyDirectAction, &QAction::triggered, this, &VideoRenderer::copyDirectUrl);

    QAction* copyUrlAction = new QAction("Copy video page URL", this);
    connect(copyUrlAction, &QAction::triggered, this, &VideoRenderer::copyVideoUrl);

    menu->addAction(copyUrlAction);
    menu->addAction(copyDirectAction);
    menu->popup(titleLabel->mapToGlobal(pos));
}
