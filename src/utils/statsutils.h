#ifndef STATSUTILS_H
#define STATSUTILS_H
#include <QString>

namespace InnertubeEndpoints { class PlayerResponse; }

class Http;
class InnertubeAuthStore;
class InnertubeContext;

namespace StatsUtils
{
    void reportPlayback(const InnertubeEndpoints::PlayerResponse& playerResp);
    void reportWatchtime(const InnertubeEndpoints::PlayerResponse& playerResp, const QString& st, const QString& et = "");
}

#endif // STATSUTILS_H
