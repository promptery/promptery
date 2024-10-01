#include "ui_log_widget.h"
#include <ui/log_widget.h>

#include <ui/pushbutton.h>

#include <QScrollBar>

LogWidget::LogWidget(QWidget *parent)
    : TileChildInterface(parent)
    , ui(new Ui::LogWidget)
{
    ui->setupUi(this);

    const QFont f = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    ui->edtOutput->setFont(f);
}

LogWidget::~LogWidget()
{
    delete ui;
}

void LogWidget::append(const QString &text)
{
    auto *scrollBar    = ui->edtOutput->verticalScrollBar();
    const bool isAtMax = scrollBar->value() == scrollBar->maximum();
    ui->edtOutput->appendPlainText(text);
    if (isAtMax) {
        scrollBar->setValue(scrollBar->maximum());
    }
}

void LogWidget::clear()
{
    ui->edtOutput->clear();
}

std::vector<QWidget *> LogWidget::actionWidgetsRight()
{
    auto *btnClear = new PushButton(QIcon(":/icons/cancel"), "", this);
    btnClear->setToolTip(tr("Clear"));
    connect(btnClear, &QPushButton::clicked, this, &LogWidget::clear);
    return { btnClear };
}
