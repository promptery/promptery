#pragma once

#include <QComboBox>

class ComboBox : public QComboBox
{
public:
    explicit ComboBox(QWidget *parent = nullptr)
        : QComboBox(parent)
    {
        setSizeAdjustPolicy(QComboBox::AdjustToContents);
    }
};
