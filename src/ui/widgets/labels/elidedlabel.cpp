#include "elidedlabel.h"
#include <QCoreApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QTextLayout>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void ElidedLabel::enterEvent(QEnterEvent*)
#else
void ElidedLabel::enterEvent(QEvent*)
#endif
{
    if (m_clickable)
        setCursor(QCursor(Qt::PointingHandCursor));
    if (m_underline)
        setStyleSheet("QFrame { text-decoration: underline; }");
}

void ElidedLabel::leaveEvent(QEvent*)
{
    if (m_clickable)
        setCursor(QCursor());
    if (m_underline)
        setStyleSheet(QString());
}

void ElidedLabel::mousePressEvent(QMouseEvent* event)
{
    if (m_clickable && event->button() == Qt::LeftButton)
        emit clicked();
}

void ElidedLabel::paintEvent(QPaintEvent* event)
{
    QFrame::paintEvent(event);

    QPainter painter(this);
    QFontMetrics fm = painter.fontMetrics();

    int lineSpacing = fm.lineSpacing();
    int y = 0;

    QTextLayout textLayout(m_text, painter.font());
    textLayout.beginLayout();

    forever
    {
        QTextLine line = textLayout.createLine();
        if (!line.isValid())
            break;

        line.setLineWidth(width());
        int nextLineY = y + lineSpacing;

        if (height() >= nextLineY + lineSpacing)
        {
            line.draw(&painter, QPoint(0, y));
            y = nextLineY;
        }
        else
        {
            QString lastLine = m_text.mid(line.textStart());
            QString elidedLastLine = fm.elidedText(lastLine, Qt::ElideRight, width());
            painter.drawText(QPoint(0, y + fm.ascent()), elidedLastLine);
            line = textLayout.createLine();
            break;
        }
    }

    textLayout.endLayout();
}

void ElidedLabel::resizeEvent(QResizeEvent* event)
{
    QFrame::resizeEvent(event);
    QSize hint = sizeHint();
    if (hint.height() < height())
        resize(width(), hint.height());
}

void ElidedLabel::setText(const QString& text)
{
    m_text = text;
    update();
    adjustSize();
}

QSize ElidedLabel::sizeHint() const
{
    QFontMetrics fm(font());
    QRect r = fm.boundingRect(QRect(QPoint(0, 0), size()), Qt::TextWordWrap | Qt::ElideRight, m_text);
    return QSize(width(), r.height());
}
