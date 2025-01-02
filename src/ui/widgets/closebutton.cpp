#include "closebutton.h"

CloseButton::CloseButton(QWidget* parent) : QToolButton(parent), action(new QAction(this))
{
    action->setText("X");
    setDefaultAction(action);
    setToolTip(QString());
}
