#ifndef BACKSTAGEPOSTRENDERER_H
#define BACKSTAGEPOSTRENDERER_H
#include <QWidget>

namespace InnertubeObjects
{
    class BackstageImage;
    class BackstagePost;
    class Poll;
    class Video;
}

class HttpReply;
class IconLabel;
class QHBoxLayout;
class QLabel;
class QVBoxLayout;
class TubeLabel;

class BackstagePostRenderer : public QWidget
{
    Q_OBJECT
public:
    explicit BackstagePostRenderer(QWidget* parent = nullptr);
    void setData(const InnertubeObjects::BackstagePost& post);
private:
    QHBoxLayout* actionButtons;
    TubeLabel* channelIconLabel;
    QString channelId;
    TubeLabel* channelLabel;
    QHBoxLayout* channelTimeLayout;
    QLabel* contentText;
    IconLabel* dislikeLabel;
    QVBoxLayout* innerLayout;
    QHBoxLayout* layout;
    IconLabel* likeLabel;
    QString postId;
    TubeLabel* publishedTimeLabel;
    TubeLabel* readMoreLabel;
    QString readMoreText;
    IconLabel* replyLabel;
    QString showLessText;
    QString surface;

    void setImage(const InnertubeObjects::BackstageImage& image);
    void setPoll(const InnertubeObjects::Poll& poll);
    void setVideo(const InnertubeObjects::Video& video);
private slots:
    void copyChannelUrl();
    void copyPostUrl();
    void linkActivated(const QString& url);
    void navigateChannel();
    void setChannelIcon(const HttpReply& reply);
    void setImageLabelData(const HttpReply& reply, QLabel* imageLabel);
    void showChannelContextMenu(const QPoint& pos);
    void showPublishedTimeContextMenu(const QPoint& pos);
    void toggleReadMore();
};

#endif // BACKSTAGEPOSTRENDERER_H
