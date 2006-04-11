#ifndef TIMEFORMATWIDGET_H
#define TIMEFORMATWIDGET_H

#include <QWidget>
class Ui_TimeDateFormatWidgetPrototype;

class TimeFormatWidget : public QWidget
{
    Q_OBJECT

public:
    TimeFormatWidget( QWidget* parent );
    ~TimeFormatWidget();
    QString resultString() const;
    int correctValue() const;
public slots:
    void updateLabel();
    void comboActivated();
    void slotPersonalizeChanged(bool b);
    void slotDefaultValueChanged(const QString & );
    void slotOffsetChanged(int);

private:
    Ui_TimeDateFormatWidgetPrototype* m_ui;
};

#endif // TIMEFORMATWIDGET_H
