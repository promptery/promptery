#include "ui_system_prompt_widget.h"
#include <ui/system_prompt_widget.h>

#include <ui/named_object_view.h>
#include <ui/system_prompt_model.h>


SystemPromptWidget::SystemPromptWidget(SystemPromptModel *systemPromptModel, QWidget *parent)
    : TileChildInterface(parent)
    , ui(new Ui::SystemPromptWidget)
    , m_model(systemPromptModel)
    , m_tree(new NamedObjectView(m_model, this))
{
    ui->setupUi(this);

    QFont f = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    f.setPointSize(13);
    ui->edtSystemPrompt->setFont(f);
    ui->edtComment->setFont(f);

    connect(ui->edtSystemPrompt,
            &QPlainTextEdit::textChanged,
            this,
            &SystemPromptWidget::writeTextToModel);

    connect(
        ui->edtComment, &QPlainTextEdit::textChanged, this, &SystemPromptWidget::writeTextToModel);

    connect(m_tree,
            &NamedObjectView::currentIndexChanged,
            this,
            &SystemPromptWidget::currentIndexChanged);

    currentIndexChanged(QModelIndex());
}

SystemPromptWidget::~SystemPromptWidget()
{
    delete ui;
}

PageData SystemPromptWidget::pageData() const
{
    return PageData{ tr("System"),
                     QIcon(":/icons/system-prompt.svg"),
                     tr("System prompts, personas") };
}

QWidget *SystemPromptWidget::sideView()
{
    return m_tree->asTile();
}

TileChildData SystemPromptWidget::data() const
{
    return { tr("System prompt") };
}

void SystemPromptWidget::currentIndexChanged(const QModelIndex &idx)
{
    if (!idx.isValid() || m_model->isFolder(idx)) {
        ui->edtSystemPrompt->clear();
        ui->edtSystemPrompt->setDisabled(true);
        ui->edtComment->clear();
        ui->edtComment->setDisabled(true);
        return;
    }
    const auto &data = m_model->userData(idx);
    QSignalBlocker b1(ui->edtSystemPrompt);
    ui->edtSystemPrompt->setPlainText(data.systemPrompt());
    ui->edtSystemPrompt->setDisabled(false);
    QSignalBlocker b2(ui->edtComment);
    ui->edtComment->setPlainText(data.comment());
    ui->edtComment->setDisabled(false);
}

void SystemPromptWidget::writeTextToModel()
{
    m_model->setUserData(m_tree->currentIndex(),
                         { ui->edtSystemPrompt->toPlainText(), ui->edtComment->toPlainText() });
}
