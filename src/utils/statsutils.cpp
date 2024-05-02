#include "statsutils.h"
#include "http.h"
#include "innertube.h"
#include <QRandomGenerator>

namespace StatsUtils
{
    QString genCpn()
    {
        QString out;
        out.reserve(16);
        constexpr std::string_view chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
        for (int i = 0; i < 16; i++)
            out += chars[QRandomGenerator::global()->bounded((int)chars.size())];
        return out;
    }

    void setQueryItem(QUrlQuery& query, const QString& key, const QString& value)
    {
        query.removeQueryItem(key); // there should never be more than 1 of each item, so this should suffice
        query.addQueryItem(key, value);
    }

#if QT_VERSION < QT_VERSION_CHECK(6, 5, 0)
    QUrlQuery constructQuery(const QUrlQuery& base, const InnertubeClient& itc, const QString& st = "",
                             const QString& et = "")
#else
    QUrlQuery constructQuery(QUrlQuery&& base, const InnertubeClient& itc, const QString& st = "",
                             const QString& et = "")
#endif
    {
        QUrlQuery out(base);

        setQueryItem(out, "ns", "yt");
        setQueryItem(out, "cmt", et);
        setQueryItem(out, "cpn", genCpn());
        setQueryItem(out, "state", "playing");
        setQueryItem(out, "volume", "100");
        setQueryItem(out, "lact", QString::number(QRandomGenerator::global()->bounded(1000, 15000)));
        setQueryItem(out, "fmt", "136");
        setQueryItem(out, "afmt", "251");
        setQueryItem(out, "euri", "");
        setQueryItem(out, "cbr", itc.browserName);
        setQueryItem(out, "cbrver", itc.browserVersion);
        setQueryItem(out, "c", QString::number(static_cast<int>(itc.clientType)));
        setQueryItem(out, "cver", itc.clientVersion);
        setQueryItem(out, "cplayer", "UNIPLAYER");
        setQueryItem(out, "cos", itc.osName);
        setQueryItem(out, "cosver", itc.osVersion);
        setQueryItem(out, "cplatform", itc.platform);
        setQueryItem(out, "hl", itc.hl + "_" + itc.gl);
        setQueryItem(out, "gl", itc.gl);
        setQueryItem(out, "idpj", QString::number(-QRandomGenerator::global()->bounded(10)));
        setQueryItem(out, "ldpj", QString::number(-QRandomGenerator::global()->bounded(40)));
        setQueryItem(out, "rtn", et);
        setQueryItem(out, "rt", et);
        if (!et.isEmpty())
            setQueryItem(out, "rti", QString::number((int)et.toFloat()));
        setQueryItem(out, "st", st);
        setQueryItem(out, "et", et);
        setQueryItem(out, "ver", "2");
        setQueryItem(out, "muted", "0");

        return out;
    }

    void setNeededHeaders(Http& http, InnertubeContext* context, InnertubeAuthStore* authStore)
    {
        if (authStore->populated())
        {
            http.addRequestHeader("Cookie", authStore->toCookieString().toUtf8());
            http.addRequestHeader("X-Goog-AuthUser", "0");
        }

        http.addRequestHeader("X-Goog-Visitor-Id", context->client.visitorData.toLatin1());
        http.addRequestHeader("X-YouTube-Client-Name", QByteArray::number(static_cast<int>(context->client.clientType)));
        http.addRequestHeader("X-YouTube-Client-Version", context->client.clientVersion.toLatin1());
    }

    void reportPlayback(const InnertubeEndpoints::PlayerResponse& playerResp)
    {
        QUrlQuery playbackQuery(QUrl(playerResp.playbackTracking.videostatsPlaybackUrl));
        playbackQuery.removeQueryItem("fexp");

        QUrl outPlaybackUrl("https://www.youtube.com/api/stats/playback");
#if QT_VERSION < QT_VERSION_CHECK(6, 5, 0)
        outPlaybackUrl.setQuery(constructQuery(playbackQuery, InnerTube::instance()->context()->client));
#else
        outPlaybackUrl.setQuery(constructQuery(std::move(playbackQuery), InnerTube::instance()->context()->client));
#endif

        Http http;
        http.setMaxRetries(0);
        setNeededHeaders(http, InnerTube::instance()->context(), InnerTube::instance()->authStore());
        http.get(outPlaybackUrl);
    }

    void reportWatchtime(const InnertubeEndpoints::PlayerResponse& playerResp, const QString& st, const QString& et)
    {
        QUrlQuery watchtimeQuery(QUrl(playerResp.playbackTracking.videostatsWatchtimeUrl));
        watchtimeQuery.removeQueryItem("fexp");

        QUrl outWatchtimeUrl("https://www.youtube.com/api/stats/watchtime");
#if QT_VERSION < QT_VERSION_CHECK(6, 5, 0)
        outWatchtimeUrl.setQuery(constructQuery(watchtimeQuery, InnerTube::instance()->context()->client, st, et));
#else
        outWatchtimeUrl.setQuery(constructQuery(std::move(watchtimeQuery),
            InnerTube::instance()->context()->client, st, et));
#endif

        Http http;
        http.setMaxRetries(0);
        setNeededHeaders(http, InnerTube::instance()->context(), InnerTube::instance()->authStore());
        http.get(outWatchtimeUrl);
    }
}
