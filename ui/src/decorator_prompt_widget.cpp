#include "ui_decorator_prompt_widget.h"
#include <ui/decorator_prompt_widget.h>

#include <ui/decorator_prompt_model.h>
#include <ui/named_object_view.h>

DecoratorPromptWidget::DecoratorPromptWidget(DecoratorPromptModel *decoratorPromptModel,
                                             QWidget *parent)
    : TileChildInterface(parent)
    , ui(new Ui::DecoratorPromptWidget)
    , m_model(decoratorPromptModel)
    , m_tree(new NamedObjectView(m_model, this))
{
    ui->setupUi(this);

    QFont f = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    f.setPointSize(13);
    ui->edtBefore->setFont(f);
    ui->edtAfter->setFont(f);
    ui->edtComment->setFont(f);

    connect(ui->edtBefore,
            &QPlainTextEdit::textChanged,
            this,
            &DecoratorPromptWidget::writeTextToModel);

    connect(
        ui->edtAfter, &QPlainTextEdit::textChanged, this, &DecoratorPromptWidget::writeTextToModel);
    connect(ui->edtComment,
            &QPlainTextEdit::textChanged,
            this,
            &DecoratorPromptWidget::writeTextToModel);

    connect(m_tree,
            &NamedObjectView::currentIndexChanged,
            this,
            &DecoratorPromptWidget::currentIndexChanged);

    currentIndexChanged(QModelIndex());
}

DecoratorPromptWidget::~DecoratorPromptWidget()
{
    delete ui;
}

PageData DecoratorPromptWidget::pageData() const
{
    return PageData{ tr("Decorators"),
                     QIcon(":/icons/decorator-prompt.svg"),
                     tr("Decorator prompts") };
}

QWidget *DecoratorPromptWidget::sideView()
{
    return m_tree->asTile();
}

TileChildData DecoratorPromptWidget::data() const
{
    return { tr("Decorator prompt") };
}

void DecoratorPromptWidget::currentIndexChanged(const QModelIndex &idx)
{
    if (!idx.isValid() || m_model->isFolder(idx)) {
        ui->edtBefore->clear();
        ui->edtBefore->setDisabled(true);
        ui->edtAfter->clear();
        ui->edtAfter->setDisabled(true);
        ui->edtComment->clear();
        ui->edtComment->setDisabled(true);
        return;
    }
    const auto &data = m_model->userData(idx);
    QSignalBlocker b1(ui->edtBefore);
    ui->edtBefore->setPlainText(data.decoratorBefore());
    ui->edtBefore->setDisabled(false);
    QSignalBlocker b2(ui->edtAfter);
    ui->edtAfter->setPlainText(data.decoratorAfter());
    ui->edtAfter->setDisabled(false);
    QSignalBlocker b3(ui->edtComment);
    ui->edtComment->setPlainText(data.comment());
    ui->edtComment->setDisabled(false);
}

void DecoratorPromptWidget::writeTextToModel()
{
    m_model->setUserData(m_tree->currentIndex(),
                         { ui->edtBefore->toPlainText(),
                           ui->edtAfter->toPlainText(),
                           ui->edtComment->toPlainText() });
}
