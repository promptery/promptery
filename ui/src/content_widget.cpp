#include "ui_content_widget.h"
#include <ui/content_widget.h>

#include <ui/content_model.h>
#include <ui/named_object_view.h>
#include <ui/tile_widget.h>

ContentWidget::ContentWidget(ContentModel *systemPromptModel, QWidget *parent)
    : TileChildInterface(parent)
    , ui(new Ui::ContentWidget)
    , m_model(systemPromptModel)
    , m_tree(new NamedObjectView(m_model, this))
{
    ui->setupUi(this);

    QFont f = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    f.setPointSize(13);
    ui->edtContent->setFont(f);

    connect(ui->edtContent, &QPlainTextEdit::textChanged, this, &ContentWidget::writeTextToModel);

    connect(
        m_tree, &NamedObjectView::currentIndexChanged, this, &ContentWidget::currentIndexChanged);

    currentIndexChanged(QModelIndex());
}

ContentWidget::~ContentWidget()
{
    delete ui;
}

PageData ContentWidget::pageData() const
{
    return PageData{ tr("Content"), QIcon(":/icons/content.svg"), tr("Content pages") };
}

QWidget *ContentWidget::sideView()
{
    return m_tree->asTile();
}

TileChildData ContentWidget::data() const
{
    return { tr("Content") };
}

void ContentWidget::currentIndexChanged(const QModelIndex &idx)
{
    if (!idx.isValid() || m_model->isFolder(idx)) {
        ui->edtContent->clear();
        ui->edtContent->setDisabled(true);
        return;
    }
    ui->edtContent->setPlainText(m_model->userData(idx).text);
    ui->edtContent->setDisabled(false);
}

void ContentWidget::writeTextToModel()
{
    m_model->setUserData(m_tree->currentIndex(), { ui->edtContent->toPlainText() });
}
