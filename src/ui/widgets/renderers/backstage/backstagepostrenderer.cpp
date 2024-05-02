#include "backstagepostrenderer.h"
#include "backstagepollrenderer.h"
#include "http.h"
#include "innertube/objects/backstage/backstagepost.h"
#include "qttubeapplication.h"
#include "ui/views/viewcontroller.h"
#include "ui/widgets/labels/iconlabel.h"
#include "ui/widgets/labels/tubelabel.h"
#include "ui/widgets/renderers/video/browsevideorenderer.h"
#include "utils/stringutils.h"
#include "utils/uiutils.h"
#include <QBoxLayout>
#include <QDesktopServices>
#include <QMenu>

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; void operator()(std::monostate) const {}};
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

BackstagePostRenderer::BackstagePostRenderer(QWidget* parent)
    : QWidget(parent),
      actionButtons(new QHBoxLayout),
      channelIconLabel(new TubeLabel(this)),
      channelLabel(new TubeLabel(this)),
      channelTimeLayout(new QHBoxLayout),
      contentText(new QLabel(this)),
      dislikeLabel(new IconLabel("dislike")),
      innerLayout(new QVBoxLayout),
      layout(new QHBoxLayout(this)),
      likeLabel(new IconLabel("like")),
      publishedTimeLabel(new TubeLabel(this)),
      readMoreLabel(new TubeLabel(this)),
      replyLabel(new IconLabel("live-chat"))
{
    channelIconLabel->setClickable(true, false);
    channelIconLabel->setFixedSize(40, 40);
    layout->addWidget(channelIconLabel, 0, Qt::AlignTop);

    channelLabel->setClickable(true, true);
    channelLabel->setContextMenuPolicy(Qt::CustomContextMenu);
    channelLabel->setFont(QFont(qApp->font().toString(), qApp->font().pointSize() - 1, QFont::Bold));
    channelTimeLayout->addWidget(channelLabel);

    publishedTimeLabel->setClickable(true, false);
    publishedTimeLabel->setContextMenuPolicy(Qt::CustomContextMenu);
    publishedTimeLabel->setFont(QFont(qApp->font().toString(), qApp->font().pointSize() - 2));
    channelTimeLayout->addWidget(publishedTimeLabel);
    channelTimeLayout->addStretch();
    innerLayout->addLayout(channelTimeLayout);

    contentText->setTextFormat(Qt::RichText);
    contentText->setWordWrap(true);
    UIUtils::setMaximumLines(contentText, 3);
    innerLayout->addWidget(contentText);

    readMoreLabel->setClickable(true, false);
    readMoreLabel->setFont(QFont(qApp->font().toString(), -1, QFont::Bold));
    innerLayout->addWidget(readMoreLabel);
    layout->addLayout(innerLayout);

    actionButtons->addWidget(likeLabel);
    actionButtons->addWidget(dislikeLabel);
    actionButtons->addWidget(replyLabel);
    actionButtons->addStretch();

    connect(channelIconLabel, &TubeLabel::clicked, this, &BackstagePostRenderer::navigateChannel);
    connect(channelLabel, &TubeLabel::clicked, this, &BackstagePostRenderer::navigateChannel);
    connect(channelLabel, &TubeLabel::customContextMenuRequested, this, &BackstagePostRenderer::showChannelContextMenu);
    connect(contentText, &QLabel::linkActivated, this, &BackstagePostRenderer::linkActivated);
    connect(publishedTimeLabel, &TubeLabel::customContextMenuRequested, this,
            &BackstagePostRenderer::showPublishedTimeContextMenu);
    connect(readMoreLabel, &TubeLabel::clicked, this, &BackstagePostRenderer::toggleReadMore);
}

void BackstagePostRenderer::copyChannelUrl()
{
    UIUtils::copyToClipboard("https://www.youtube.com/channel/" + channelId);
}

void BackstagePostRenderer::copyPostUrl()
{
    UIUtils::copyToClipboard("https://www.youtube.com/post/" + postId);
}

void BackstagePostRenderer::linkActivated(const QString& url)
{
    QUrl qUrl(url);
    if (url.startsWith("http"))
    {
        QDesktopServices::openUrl(qUrl);
    }
    else if (url.startsWith("/channel"))
    {
        QString funnyPath = qUrl.path().replace("/channel/", "");
        ViewController::loadChannel(funnyPath.left(funnyPath.indexOf('/')));
    }
    else if (url.startsWith("/watch"))
    {
        QUrlQuery query(qUrl);
        int progress = query.queryItemValue("t").replace("s", "").toInt();
        ViewController::loadVideo(query.queryItemValue("v"), progress);
    }
    else
    {
        qDebug() << "Ran into unsupported description link:" << url;
    }
}

void BackstagePostRenderer::navigateChannel()
{
    ViewController::loadChannel(channelId);
}

void BackstagePostRenderer::setChannelIcon(const HttpReply& reply)
{
    QPixmap pixmap;
    pixmap.loadFromData(reply.body());
    channelIconLabel->setPixmap(UIUtils::pixmapRounded(pixmap, 20, 20));
}

void BackstagePostRenderer::setData(const InnertubeObjects::BackstagePost& post)
{
    channelId = post.authorEndpoint["browseEndpoint"]["browseId"].toString();
    postId = post.postId;
    readMoreText = post.expandButton.text.text;
    showLessText = post.collapseButton.text.text;
    surface = post.surface;

    channelLabel->setText(post.authorText.text);
    contentText->setText(StringUtils::innertubeStringToRichText(post.contentText, false));
    likeLabel->setText(qtTubeApp->settings().condensedCounts
                           ? post.voteCount.text
                           : StringUtils::extractDigits(post.actionButtons.likeButton.accessibilityLabel));
    publishedTimeLabel->setText(post.publishedTimeText.text);
    readMoreLabel->setText(readMoreText);
    readMoreLabel->setVisible(contentText->heightForWidth(width() - channelIconLabel->width()) > contentText->maximumHeight());
    replyLabel->setText(post.actionButtons.replyButton.text.text);

    HttpReply* reply = Http::instance().get("https:" + post.authorThumbnail.recommendedQuality(channelIconLabel->size()).url);
    connect(reply, &HttpReply::finished, this, &BackstagePostRenderer::setChannelIcon);

    std::visit(overloaded {
        [this](const InnertubeObjects::BackstageImage& image) { setImage(image); },
        [this](const InnertubeObjects::Poll& poll) { setPoll(poll); },
        [this](const InnertubeObjects::Video& video) { setVideo(video); }
    }, post.backstageAttachment);

    innerLayout->addLayout(actionButtons);
    innerLayout->addStretch();
}

void BackstagePostRenderer::setImage(const InnertubeObjects::BackstageImage& image)
{
    QLabel* imageLabel = new QLabel(this);
    imageLabel->setMaximumWidth(width());
    imageLabel->setScaledContents(true);

    if (surface != "BACKSTAGE_SURFACE_TYPE_STREAM")
        imageLabel->setMaximumHeight(420);

    innerLayout->addWidget(imageLabel);

    // absolutely filthy workaround for layout fucking up when there's no content
    if (contentText->text().isEmpty())
        contentText->setText(" ");

    HttpReply* reply = Http::instance().get(image.image.recommendedQuality(size()).url);
    connect(reply, &HttpReply::finished, this,
            std::bind(&BackstagePostRenderer::setImageLabelData, this, std::placeholders::_1, imageLabel));
}

void BackstagePostRenderer::setImageLabelData(const HttpReply& reply, QLabel* imageLabel)
{
    QPixmap pixmap;
    pixmap.loadFromData(reply.body());
    imageLabel->setPixmap(pixmap);
    adjustSize();
}

void BackstagePostRenderer::setPoll(const InnertubeObjects::Poll& poll)
{
    BackstagePollRenderer* pollRenderer = new BackstagePollRenderer(this);
    pollRenderer->setData(poll);
    pollRenderer->setFixedWidth(width());
    innerLayout->addWidget(pollRenderer);
}

void BackstagePostRenderer::setVideo(const InnertubeObjects::Video& video)
{
    if (qtTubeApp->settings().videoIsFiltered(video))
        return;

    BrowseVideoRenderer* videoRenderer = new BrowseVideoRenderer(this);
    videoRenderer->setData(video);
    videoRenderer->titleLabel->setFixedWidth(width() - 100);
    innerLayout->addWidget(videoRenderer);
}

void BackstagePostRenderer::showChannelContextMenu(const QPoint& pos)
{
    QMenu* menu = new QMenu(this);

    QAction* copyUrlAction = new QAction("Copy channel page URL", this);
    connect(copyUrlAction, &QAction::triggered, this, &BackstagePostRenderer::copyChannelUrl);

    menu->addAction(copyUrlAction);
    menu->popup(channelLabel->mapToGlobal(pos));
}

void BackstagePostRenderer::showPublishedTimeContextMenu(const QPoint& pos)
{
    QMenu* menu = new QMenu(this);

    QAction* copyUrlAction = new QAction("Copy post URL", this);
    connect(copyUrlAction, &QAction::triggered, this, &BackstagePostRenderer::copyPostUrl);

    menu->addAction(copyUrlAction);
    menu->popup(publishedTimeLabel->mapToGlobal(pos));
}

void BackstagePostRenderer::toggleReadMore()
{
    if (readMoreLabel->text() == readMoreText)
    {
        contentText->setMaximumHeight(QWIDGETSIZE_MAX);
        readMoreLabel->setText(showLessText);
    }
    else
    {
        UIUtils::setMaximumLines(contentText, 3);
        readMoreLabel->setText(readMoreText);
    }

    adjustSize();
}
