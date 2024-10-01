#pragma once

#include <ui/defines.h>

#include <QApplication>
#include <QPushButton>
#include <QStyleHints>

class PushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit PushButton(const QString &text, QWidget *parent = nullptr)
        : QPushButton(text, parent)
    {
    }

    PushButton(const QIcon &icon, const QString &text, QWidget *parent = nullptr)
        : QPushButton(icon, text, parent)
    {
        setMaximumSize(QSize(cHeaderHeight, cHeaderHeight));
        setIconSize(QSize(cHeaderHeight, cHeaderHeight));
        setFlat(true);
        connect(qApp->styleHints(),
                &QStyleHints::colorSchemeChanged,
                this,
                &PushButton::colorThemeChanged);

        colorThemeChanged(qApp->styleHints()->colorScheme());
    }

    Q_SLOT void colorThemeChanged(Qt::ColorScheme /*colorScheme*/)
    {
        setStyleSheet(customStyleSheet());
    }

    static QString customStyleSheet()
    {
        static const auto styleSheet = QString(R"(
            QPushButton {
                border-radius: 1px;
                border: none;
            }
            QPushButton:checked {
                background-color: transparent;
            }
            QPushButton:hover, QPushButton:checked:hover {
                background-color: %1;
            }
            QPushButton:pressed, QPushButton:checked:pressed {
                background-color: %2;
            }
        )");

        return styleSheet.arg(qApp->palette().color(QPalette::Mid).name(), "#ffe600");
    }
};
