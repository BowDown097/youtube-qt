#include "watchviewplayer.h"
#include "innertube.h"

#ifdef QTTUBE_USE_MPV
#include "lib/media/mpv/mediampv.h"
#include "stores/settingsstore.h"
#include "utils/statsutils.h"
#include <QMessageBox>
#else
#include "ui/widgets/webengineplayer/webengineplayer.h"
#endif

WatchViewPlayer::WatchViewPlayer(QWidget* watchView, const QSize& maxSize) : QObject(watchView)
{
#ifdef QTTUBE_USE_MPV
    media = new MediaMPV(watchView);
    media->init();
    media->setVolume(SettingsStore::instance()->preferredVolume);

    connect(media, &Media::error, this, [](const QString& message) { qWarning() << "Media error:" << message; });
    connect(media, &Media::stateChanged, this, &WatchViewPlayer::mediaStateChanged);
    connect(media, &Media::volumeChanged, this, &WatchViewPlayer::volumeChanged);
#else
    wePlayer = new WebEnginePlayer(watchView);
    wePlayer->setAuthStore(InnerTube::instance()->authStore());
    wePlayer->setContext(InnerTube::instance()->context());

    connect(wePlayer, &WebEnginePlayer::progressChanged, this, &WatchViewPlayer::progressChanged);
#endif

    calcAndSetSize(maxSize);
}

void WatchViewPlayer::calcAndSetSize(const QSize& maxSize)
{
    PlayerScaleMode currentScaleMode = m_scaleMode;
    int playerWidth = maxSize.width();
    int playerHeight = playerWidth * 9/16;
    int maxHeightBeforeScale = maxSize.height() - 150;

    if (playerHeight > maxHeightBeforeScale)
    {
        playerWidth = std::min(maxHeightBeforeScale * 16/9, maxSize.width() - 500);
        playerHeight = playerWidth * 9/16;
        m_scaleMode = PlayerScaleMode::Scaled;
    }
    else
    {
        m_scaleMode = PlayerScaleMode::NoScale;
    }

    if (m_scaleMode != currentScaleMode)
        emit scaleModeChanged(m_scaleMode);

    QSize sz(playerWidth, playerHeight);
    m_size = sz;
    widget()->setFixedSize(sz);
}

void WatchViewPlayer::play(const QString& videoId, int progress)
{
#ifdef QTTUBE_USE_MPV
    media->play("https://www.youtube.com/watch?v=" + videoId);
    media->seek(progress);
#else
    wePlayer->play(videoId, progress);
#endif
}

void WatchViewPlayer::seek(int progress)
{
#ifdef QTTUBE_USE_MPV
    media->seek(progress);
#else
    wePlayer->seek(progress);
#endif
}

void WatchViewPlayer::startTracking(const InnertubeEndpoints::PlayerResponse& playerResp)
{
#ifdef QTTUBE_USE_MPV
    if (SettingsStore::instance()->playbackTracking)
        StatsUtils::reportPlayback(playerResp);

    if (SettingsStore::instance()->watchtimeTracking)
    {
        watchtimeTimer = new QTimer(this);
        watchtimeTimer->setInterval(5000);
        watchtimeTimer->start();
        QString pos = QString::number(media->position());
        connect(watchtimeTimer, &QTimer::timeout, this, std::bind(&StatsUtils::reportWatchtime, playerResp, pos, pos));
    }
#else
    wePlayer->setPlayerResponse(playerResp);
#endif
}

void WatchViewPlayer::stopTracking()
{
#ifdef QTTUBE_USE_MPV
    if (watchtimeTimer)
        watchtimeTimer->deleteLater();
#endif
}

QWidget* WatchViewPlayer::widget()
{
#ifdef QTTUBE_USE_MPV
    return media->videoWidget();
#else
    return wePlayer;
#endif
}

#ifdef QTTUBE_USE_MPV
void WatchViewPlayer::mediaStateChanged(Media::State state)
{
    if (state == Media::ErrorState)
        QMessageBox::critical(nullptr, "Media error", media->errorString());
}

void WatchViewPlayer::volumeChanged(double volume)
{
    Q_UNUSED(volume);
    if (media->volumeMuted())
        media->setVolumeMuted(false);
}
#endif
