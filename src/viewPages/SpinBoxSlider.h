/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#ifndef SPINBOXSLIDER_H
#define SPINBOXSLIDER_H

#include <QWidget>
#include <QHBoxLayout>
#include <QSpinBox>
#include "qxtspanslider.h"

class SpinBoxSlider : public QWidget
{
    Q_OBJECT

public:
    
    explicit SpinBoxSlider(QWidget *parent = 0);
    virtual ~SpinBoxSlider();
    void setToolTip(const QString &str);

    void setMaximumValue(int max);
    void setMinimumValue(int min);
    void setTickInterval(int interval);

signals:

    void lowerValueChanged(int);
    void upperValueChanged(int);

public slots:
    
    void setLowerValue(int min);
    void setUpperValue(int max);

protected slots:

private:

    void setLowerValuePrivate(int min);
    void setUpperValuePrivate(int max);

    QxtSpanSlider *m_spanslider = nullptr;
    QSpinBox *m_left_spinbox = nullptr;
    QSpinBox *m_right_spinbox = nullptr;
    QHBoxLayout *m_layout = nullptr;

    int m_upper_value = 0;
    int m_lower_value = 0;
};

#endif

