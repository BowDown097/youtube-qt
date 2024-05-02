#include "watchnextfeed.h"
#include "continuablelistwidget.h"
#include "innertube/endpoints/video/next.h"
#include "ui/widgets/renderers/video/browsevideorenderer.h"
#include "utils/uiutils.h"
#include <QCoreApplication>

WatchNextFeed::WatchNextFeed(QWidget* parent)
    : QTabWidget(parent),
      comments(new ContinuableListWidget(this)),
      recommended(new ContinuableListWidget(this))
{
    addTab(recommended, "Recommended");
    addTab(comments, "Comments");
    setTabVisible(1, false);
}

void WatchNextFeed::continueComments()
{
}

void WatchNextFeed::continueRecommended()
{
}

void WatchNextFeed::reset()
{
    setCurrentIndex(0);
    setTabVisible(1, false);
    comments->clear();
    recommended->clear();
}

void WatchNextFeed::setData(const InnertubeEndpoints::Next& endpoint)
{
    if (endpoint.response.results.results.commentsSectionContinuation.has_value())
    {
        setTabVisible(1, true);
        commentsContinuation = endpoint.response.results.results.commentsSectionContinuation.value();
        connect(comments, &ContinuableListWidget::continuationReady, this, &WatchNextFeed::continueComments);
    }

    if (endpoint.response.results.secondaryResults.feedContinuation.has_value())
    {
        recommendedContinuation = endpoint.response.results.secondaryResults.feedContinuation.value();
        connect(recommended, &ContinuableListWidget::continuationReady, this, &WatchNextFeed::continueRecommended);
    }

    for (const InnertubeObjects::Video& recommendedVideo : endpoint.response.results.secondaryResults.feed)
    {
        BrowseVideoRenderer* renderer = new BrowseVideoRenderer(recommended);
        renderer->thumbnail->setPreferredSize(QSize(168, 94));
        renderer->titleLabel->setMaximumWidth(recommended->width() - 220);
        UIUtils::setMaximumLines(renderer->titleLabel, 2);
        renderer->setData(recommendedVideo);

        UIUtils::addWidgetToList(recommended, renderer);
        QCoreApplication::processEvents();
    }
}
