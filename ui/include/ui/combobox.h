#pragma once

#include <QComboBox>
#include <QEvent>
#include <QHelpEvent>
#include <QToolTip>

class ComboBox : public QComboBox
{
public:
    explicit ComboBox(QWidget *parent = nullptr)
        : QComboBox(parent)
    {
        setSizeAdjustPolicy(QComboBox::AdjustToContents);
    }

    bool event(QEvent *event) override
    {
        // Tooltip for combobox with dynamic models is not working without this:
        if (event->type() == QEvent::ToolTip) {
            QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
            auto tt               = this->currentData(Qt::ToolTipRole).toString();
            if (!tt.isEmpty()) {
                QToolTip::showText(helpEvent->globalPos(), tt);
            } else {
                QToolTip::hideText();
                event->ignore();
            }
            return true;
        }
        return QComboBox::event(event);
    }
};
