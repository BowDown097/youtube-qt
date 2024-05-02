#ifndef DYNAMICLISTWIDGETITEM_H
#define DYNAMICLISTWIDGETITEM_H
#include <QListWidgetItem>

class DynamicListWidgetItem : public QObject, public QListWidgetItem
{
    Q_OBJECT
public:
    DynamicListWidgetItem(QWidget* widget, QListWidget* listView);
    void addToList();
private:
    QWidget* widget;
};

#endif // DYNAMICLISTWIDGETITEM_H
