#include "uiutils.h"
#include "http.h"
#include "innertube/objects/backstage/backstagepost.h"
#include "innertube/objects/channel/channel.h"
#include "innertube/objects/video/reel.h"
#include "innertube/objects/video/video.h"
#include "mainwindow.h"
#include "qttubeapplication.h"
#include "ui/widgets/dynamiclistwidgetitem.h"
#include "ui/widgets/renderers/backstage/backstagepostrenderer.h"
#include "ui/widgets/renderers/browsechannelrenderer.h"
#include "ui/widgets/renderers/video/browsevideorenderer.h"
#include "ui/widgets/renderers/video/gridvideorenderer.h"
#include <QClipboard>
#include <QLayout>
#include <QPainter>
#include <QStyleFactory>

#ifdef Q_OS_LINUX
#include <QThread>
#endif

constexpr const char* darkStylesheet = R"(
    QLineEdit {
        background: rgb(42,42,42);
        border: 1px solid rgb(30,30,30);
    }
    QListView::item {
        background: rgb(49,49,49);
    }
    QComboBox, QMessageBox, QPushButton, QScrollBar::vertical, QSpinBox, QTabBar::tab {
        background: rgb(42,42,42);
    }
    QPushButton::hover, QTabBar::tab::hover, QTabBar::tab::selected {
        background: rgb(30,30,30);
    }
    QTabWidget::pane {
        border-color: rgb(30,30,30);
    }
    QToolButton {
        background: transparent;
        border: 1px solid rgb(30,30,30);
    }
)";

void UIUtils::addBackstagePostToList(QListWidget* list, const InnertubeObjects::BackstagePost& post)
{
    if (qtTubeApp->settings().channelIsFiltered(post.authorEndpoint["browseEndpoint"]["browseId"].toString()))
        return;

    BackstagePostRenderer* renderer = new BackstagePostRenderer;
    renderer->setData(post);

    DynamicListWidgetItem* item = new DynamicListWidgetItem(renderer, list);
    item->addToList();
}

void UIUtils::addBoldLabelToList(QListWidget* list, const QString& text)
{
    QLabel* label = new QLabel(text);
    label->setFont(QFont(qApp->font().toString(), -1, QFont::Bold));
    addWidgetToList(list, label);
}

void UIUtils::addChannelRendererToList(QListWidget* list, const InnertubeObjects::Channel& channel)
{
    if (qtTubeApp->settings().channelIsFiltered(channel.channelId))
        return;

    BrowseChannelRenderer* renderer = new BrowseChannelRenderer;
    renderer->setData(channel.channelId, channel.descriptionSnippet.text, channel.title.text, channel.subscribeButton,
                      channel.subscriberCountText.text, channel.videoCountText.text);
    addWidgetToList(list, renderer);

    HttpReply* reply = Http::instance().get("https:" + channel.thumbnail.recommendedQuality(QSize(80, 80)).url);
    QObject::connect(reply, &HttpReply::finished, renderer, &BrowseChannelRenderer::setThumbnail);
}

void UIUtils::addSeparatorToList(QListWidget* list)
{
    QFrame* line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    addWidgetToList(list, line);
}

void UIUtils::addShelfTitleToList(QListWidget* list, const QJsonValue& shelf)
{
    addShelfTitleToList(list, InnertubeObjects::InnertubeString(shelf["title"]).text);
}

void UIUtils::addShelfTitleToList(QListWidget* list, const QString& title)
{
    TubeLabel* shelfLabel = new TubeLabel(title);
    shelfLabel->setFont(QFont(qApp->font().toString(), qApp->font().pointSize() + 2));
    addWidgetToList(list, shelfLabel);
}

void UIUtils::addVideoRendererToList(QListWidget* list, const InnertubeObjects::Reel& reel)
{
    if (qtTubeApp->settings().videoIsFiltered(reel))
        return;

    VideoRenderer* renderer = constructVideoRenderer(list);
    renderer->setData(reel);
    addWidgetToList(list, renderer);
}

void UIUtils::addVideoRendererToList(QListWidget* list, const InnertubeObjects::Video& video)
{
    if (qtTubeApp->settings().videoIsFiltered(video))
        return;

    VideoRenderer* renderer = constructVideoRenderer(list);
    renderer->setData(video);
    addWidgetToList(list, renderer);
}

QListWidgetItem* UIUtils::addWidgetToList(QListWidget* list, QWidget* widget)
{
    QListWidgetItem* item = new QListWidgetItem(list);
    item->setSizeHint(widget->sizeHint());
    list->addItem(item);
    list->setItemWidget(item, widget);
    return item;
}

void UIUtils::addWrappedLabelToList(QListWidget* list, const QString& text)
{
    QLabel* label = new QLabel(text);
    label->setWordWrap(true);
    addWidgetToList(list, label);
}

void UIUtils::clearLayout(QLayout* layout)
{
    while (QLayoutItem* item = layout->takeAt(0))
    {
        if (QWidget* widget = item->widget())
            widget->deleteLater();
        if (QLayout* childLayout = item->layout())
            clearLayout(childLayout);
        delete item;
    }
}

VideoRenderer* UIUtils::constructVideoRenderer(QListWidget* list)
{
    VideoRenderer* renderer;
    if (list->flow() == QListWidget::LeftToRight)
    {
        renderer = new GridVideoRenderer;
    }
    else
    {
        renderer = new BrowseVideoRenderer;
        renderer->titleLabel->setMaximumWidth(list->width() - 240);
    }

    return renderer;
}

void UIUtils::copyToClipboard(const QString& text)
{
    QClipboard* clipboard = qApp->clipboard();
    clipboard->setText(text, QClipboard::Clipboard);

    if (clipboard->supportsSelection())
        clipboard->setText(text, QClipboard::Selection);

// apparently a workaround for copying to clipboard sometimes not working on linux. see https://www.medo64.com/2019/12/copy-to-clipboard-in-qt/
#ifdef Q_OS_LINUX
    QThread::msleep(1);
#endif
}

QIcon UIUtils::iconThemed(const QString& name, const QPalette& pal)
{
    return QIcon(resolveThemedIconName(name, pal));
}

QPixmap UIUtils::pixmapRounded(const QPixmap& pixmap, double xRadius, double yRadius)
{
    QPixmap rounded(pixmap.size());
    rounded.fill(Qt::transparent);

    QPainter painter(&rounded);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush(pixmap));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(pixmap.rect(), 15, 15);
    painter.end();

    return rounded;
}

QPixmap UIUtils::pixmapThemed(const QString& name, bool fromQIcon, const QSize& size, const QPalette& pal)
{
    return fromQIcon ? QIcon(resolveThemedIconName(name, pal)).pixmap(size) : QPixmap(resolveThemedIconName(name, pal));
}

bool UIUtils::preferDark(const QPalette& pal)
{
    return pal == QPalette()
            ? qApp->palette().alternateBase().color().lightness() < 60
            : pal.alternateBase().color().lightness() < 60;
}

QString UIUtils::resolveThemedIconName(const QString& name, const QPalette& pal)
{
    const QString baseFile = ":/" + name + ".svg";
    const QString lightFile = ":/" + name + "-light.svg";
    return QFileInfo::exists(lightFile) && UIUtils::preferDark(pal) ? lightFile : baseFile;
}

void UIUtils::setAppStyle(const QString& styleName, bool dark)
{
    if (styleName == "Default" && qApp->style()->objectName() != defaultStyle)
        qApp->setStyle(QStyleFactory::create(defaultStyle));
    else if (QStyle* style = QStyleFactory::create(styleName))
        qApp->setStyle(style);

    if (dark)
    {
        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window, QColor(49,49,49));
        darkPalette.setColor(QPalette::WindowText, Qt::white);
        darkPalette.setColor(QPalette::Base, QColor(34,34,34));
        darkPalette.setColor(QPalette::AlternateBase, QColor(42,42,42));
        darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
        darkPalette.setColor(QPalette::ToolTipText, Qt::white);
        darkPalette.setColor(QPalette::Text, Qt::white);
        darkPalette.setColor(QPalette::Button, QColor(48,48,48));
        darkPalette.setColor(QPalette::ButtonText, Qt::white);
        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Link, QColor(47,163,198));
        darkPalette.setColor(QPalette::Highlight, QColor(145,205,92));
        darkPalette.setColor(QPalette::HighlightedText, Qt::black);
        darkPalette.setColor(QPalette::PlaceholderText, Qt::darkGray);
        qApp->setPalette(darkPalette);
        qApp->setStyleSheet(darkStylesheet);
        MainWindow::topbar()->updatePalette(darkPalette);
    }
    else if (qApp->styleSheet() == darkStylesheet)
    {
        qApp->setPalette(qApp->style()->standardPalette());
        qApp->setStyleSheet(QString());
        MainWindow::topbar()->updatePalette(qApp->palette());
    }

#ifdef Q_OS_WIN // for some reason, wrong palette is applied to topbar on windows
    MainWindow::topbar()->updatePalette(qApp->palette());
#endif
}

// this will be used for the description and perhaps elsewhere
void UIUtils::setMaximumLines(QWidget* widget, int lines)
{
    QFontMetrics fm(widget->font());
    widget->setMaximumHeight(fm.lineSpacing() * lines);
}

void UIUtils::setTabsEnabled(QTabWidget* widget, bool enabled, std::initializer_list<int> indexes)
{
    for (int i : indexes)
        widget->setTabEnabled(i, enabled);
}

void UIUtils::setThumbnail(QLabel* label, const QJsonArray& thumbsArr, bool getBest)
{
    auto thumbsBegin = getBest
        ? std::ranges::max_element(thumbsArr, [](const QJsonValue& a, const QJsonValue& b) { return a["height"].toInt() < b["height"].toInt(); })
        : thumbsArr.begin();
    if (thumbsBegin == thumbsArr.end())
        return;

    const QJsonValue& thumbnail = *thumbsBegin;
    HttpReply* reply = Http::instance().get(QUrl(thumbnail["url"].toString()));
    QObject::connect(reply, &HttpReply::finished, reply, [label](const HttpReply& reply)
    {
        QPixmap pixmap;
        pixmap.loadFromData(reply.body());
        label->setPixmap(pixmap.scaled(label->width(), label->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    });
}
