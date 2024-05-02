#ifndef ELIDEDLABEL_H
#define ELIDEDLABEL_H
#include <QFrame>

class ElidedLabel : public QFrame
{
    Q_OBJECT
public:
    explicit ElidedLabel(QWidget* parent = nullptr) : QFrame(parent) {}
    ElidedLabel(const QString& text, QWidget* parent = nullptr) : ElidedLabel(parent) { setText(text); }

    void setClickable(bool clickable, bool underline) { m_clickable = clickable; m_underline = underline; }

    void setText(const QString& text);
    QString text() const { return m_text; }
protected:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void enterEvent(QEnterEvent*) override;
#else
    void enterEvent(QEvent*) override;
#endif
    void leaveEvent(QEvent*) override;
    void mousePressEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    QSize sizeHint() const override;
signals:
    void clicked();
private:
    bool m_clickable = false;
    QString m_text;
    bool m_underline = false;
};

#endif // ELIDEDLABEL_H
