#include "playerinterceptor.h"
#include "qttubeapplication.h"
#include "utils/statsutils.h"
#include <QUrlQuery>

void PlayerInterceptor::interceptRequest(QWebEngineUrlRequestInfo& info)
{
    if (!m_authStore || !m_context)
    {
        qDebug() << "Auth store or InnerTube context is null - using YouTube's default tracking.";
        return;
    }

    // block for privacy, should not have any impact on the operation of the player or program
    const QUrl url = info.requestUrl();
    if (url.host().contains("doubleclick.net") || url.host() == "jnn-pa.googleapis.com" ||
        url.path() == "/youtubei/v1/log_event" || url.path() == "/api/stats/qoe" ||
        url.path() == "/ptracking" || url.toString().contains("play.google.com/log"))
    {
        info.block(true);
    }

    // modify based on settings
    if (url.path() == "/api/stats/watchtime")
    {
        info.block(true);
        if (qtTubeApp->settings().watchtimeTracking)
        {
            QUrlQuery query(url);
            StatsUtils::reportWatchtime(m_playerResponse, query.queryItemValue("st"), query.queryItemValue("et"));
        }
    }
    else if (url.path() == "/api/stats/playback")
    {
        info.block(true);
        if (qtTubeApp->settings().playbackTracking)
            StatsUtils::reportPlayback(m_playerResponse);
    }
}
