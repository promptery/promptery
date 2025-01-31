#pragma once

#include <QApplication>
#include <QLabel>

inline QLabel *newLabel(QString &&text, QWidget *parent, int indent = 0)
{
    QFont font = qApp->font();
    auto *lbl  = new QLabel(std::move(text), parent);
    font.setBold(false);
    font.setPointSize(font.pointSize() - 2);
    lbl->setFont(font);
    lbl->setIndent(indent);
    return lbl;
};

inline QLabel *newTitleLabel(QString &&text, QWidget *parent, int indent = 0)
{
    QFont font = qApp->font();
    auto *lbl  = new QLabel(std::move(text), parent);
    font.setBold(true);
    font.setPointSize(font.pointSize());
    lbl->setFont(font);
    lbl->setIndent(indent);
    return lbl;
};

inline QLabel *newSectionLabel(QString &&text, QWidget *parent, int indent = 0)
{
    QFont font = qApp->font();
    auto *lbl  = new QLabel(std::move(text), parent);
    font.setBold(true);
    font.setPointSize(font.pointSize() + 1);
    lbl->setFont(font);
    lbl->setIndent(indent);
    return lbl;
};
