#include "dynamiclistwidgetitem.h"
#include "genericeventfilter.h"
#include <QEvent>
#include <QResizeEvent>

DynamicListWidgetItem::DynamicListWidgetItem(QWidget* widget, QListWidget* listView)
    : QListWidgetItem(listView), widget(widget)
{
    setSizeHint(widget->sizeHint());
    widget->installEventFilter(new GenericEventFilter(this, [this, widget](QObject*, QObject*, QEvent* event) {
        if (event->type() == QEvent::Resize)
        {
            QResizeEvent* resizeEvent = static_cast<QResizeEvent*>(event);
            setSizeHint(widget->sizeHint());
        }

        return false;
    }));
}

void DynamicListWidgetItem::addToList()
{
    listWidget()->addItem(this);
    listWidget()->setItemWidget(this, widget);
}

