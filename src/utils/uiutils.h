#ifndef UIUTILS_H
#define UIUTILS_H
#include <initializer_list>
#include <QWidget>

namespace InnertubeObjects
{
class Channel;
class Reel;
class Video;
}

class QLabel;
class QLayout;
class QListWidget;
class QListWidgetItem;
class QTabWidget;
class VideoRenderer;

class UIUtils
{
public:
    static inline QString defaultStyle;

    static void addBoldLabelToList(QListWidget* list, const QString& text);
    static void addChannelRendererToList(QListWidget* list, const InnertubeObjects::Channel& channel);
    static void addSeparatorToList(QListWidget* list);
    static void addShelfTitleToList(QListWidget* list, const QJsonValue& shelf);
    static void addShelfTitleToList(QListWidget* list, const QString& title);
    static void addVideoRendererToList(QListWidget* list, const InnertubeObjects::Reel& reel);
    static void addVideoRendererToList(QListWidget* list, const InnertubeObjects::Video& video);
    static QListWidgetItem* addWidgetToList(QListWidget* list, QWidget* widget);
    static void addWrappedLabelToList(QListWidget* list, const QString& text);
    static void clearLayout(QLayout* layout);
    static void copyToClipboard(const QString& text);
    static void elide(QLabel* label, int targetWidth);
    static QString extractDigits(const QString& str, bool useLocale = true, bool reserve = true);
    static QIcon iconThemed(const QString& name, const QPalette& pal = QPalette());
    static QPixmap pixmapRounded(const QPixmap& pixmap, double xRadius, double yRadius);
    static QPixmap pixmapThemed(const QString& name, bool fromQIcon = false, const QSize& size = QSize(), const QPalette& pal = QPalette());
    static bool preferDark(const QPalette& pal = QPalette());
    static QString resolveThemedIconName(const QString& name, const QPalette& pal = QPalette());
    static void setAppStyle(const QString& styleName, bool dark);
    static void setMaximumLines(QWidget* widget, int lines);
    static void setTabsEnabled(QTabWidget* widget, bool enabled, std::initializer_list<int> indexes);
    static void setThumbnail(QLabel* label, const QJsonArray& thumbsArr, bool getBest = false);

#ifdef QTTUBE_HAS_ICU
    static QString condensedNumericString(qint64 num, int precision = 1);
#endif

    template<typename T>
    static T findParent(QWidget* widget)
    {
        QWidget* parentWidget = widget->parentWidget();
        while (parentWidget)
        {
            if (T castedWidget = qobject_cast<T>(parentWidget))
                return castedWidget;
            parentWidget = parentWidget->parentWidget();
        }

        return nullptr;
    }
private:
    static VideoRenderer* constructVideoRenderer(QListWidget* list);
};

#endif // UIUTILS_H
