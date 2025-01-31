#pragma once

#include <QDoubleSpinBox>
#include <QSpinBox>

inline QSpinBox *newCtxSb(QWidget *parent)
{
    auto *res = new QSpinBox(parent);
    res->setMinimum(0);
    res->setMaximum(512);
    res->setValue(4);
    res->setSingleStep(2);
    res->setSuffix(QObject::tr("k"));
    res->setAlignment(Qt::AlignRight);
    res->setToolTip(QObject::tr("Context length (*1024)"));
    return res;
}

inline QSpinBox *newSeedSb(QWidget *parent)
{
    auto *res = new QSpinBox(parent);
    res->setMinimum(-1);
    res->setMaximum(9999);
    res->setValue(-1);
    res->setSingleStep(1);
    res->setToolTip(QObject::tr("Seed"));
    res->setAlignment(Qt::AlignRight);
    return res;
}

inline QDoubleSpinBox *newTemperatureSb(QWidget *parent)
{
    auto *res = new QDoubleSpinBox(parent);
    res->setMinimum(0.);
    res->setMaximum(99.99);
    res->setValue(0.8);
    res->setDecimals(2);
    res->setSingleStep(0.05);
    res->setToolTip(QObject::tr("Temperature"));
    res->setAlignment(Qt::AlignRight);
    return res;
}
