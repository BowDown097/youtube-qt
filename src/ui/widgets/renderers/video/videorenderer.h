#ifndef VIDEORENDERER_H
#define VIDEORENDERER_H
#include "ui/widgets/labels/channellabel.h"
#include "ui/widgets/labels/elidedlabel.h"
#include "videothumbnailwidget.h"

namespace InnertubeObjects
{
class Reel;
class ResponsiveImage;
class Video;
}

namespace PreloadData { class WatchView; }

class HttpReply;

class VideoRenderer : public QWidget
{
    Q_OBJECT
public:
    ChannelLabel* channelLabel;
    TubeLabel* metadataLabel;
    VideoThumbnailWidget* thumbnail;
    ElidedLabel* titleLabel;

    explicit VideoRenderer(QWidget* parent = nullptr);
    void setData(const InnertubeObjects::Reel& reel);
    void setData(const InnertubeObjects::Video& video);
    void setThumbnailSize(const QSize& size);
private:
    QString channelId;
    int progress = 0;
    QString videoId;
    PreloadData::WatchView* watchPreloadData{};

    void setThumbnail(const QString& url);
private slots:
    void copyChannelUrl();
    void copyDirectUrl();
    void copyVideoUrl();
    void navigateChannel();
    void navigateVideo();
    void setDeArrowData(const HttpReply& reply, const QString& fallbackThumbUrl);
    void showChannelContextMenu(const QPoint& pos);
    void showTitleContextMenu(const QPoint& pos);
};

#endif // VIDEORENDERER_H
