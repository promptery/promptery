#pragma once

#include <common/settings.h>
#include <common/tree_to_list_model_adapter.h>

#include <model/defines.h>

#include <QObject>

class ComboboxListAdapter : public TreeToListModelAdapter
{
public:
    using TreeToListModelAdapter::TreeToListModelAdapter;

    QVariant data(const QModelIndex &index, int role) const override
    {
        switch (role) {
        case Qt::DecorationRole: return {};
        case Qt::DisplayRole: {
            QString name = TreeToListModelAdapter::data(index, role).toString();
            auto current = mapToSource(index).parent();
            while (current.isValid()) {
                name    = m_sourceModel->data(current, role).toString() + "/" + name;
                current = current.parent();
            }
            return name;
        }
        default: return TreeToListModelAdapter::data(index, role);
        }
    }
};


class AdapterBase : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

    Q_SIGNAL void selectedChanged();
};

template <typename T>
class WorkflowAdapter : public AdapterBase
{
public:
    WorkflowAdapter(T *model, QString settingsKey, QObject *parent = nullptr)
        : AdapterBase(parent)
        , m_settingsKey(std::move(settingsKey))
        , m_model(model)
        , m_adapter(new ComboboxListAdapter(m_model->itemModel(), this))
    {
        // Workaround since the TreeToListModelAdapter doesn't support fine grained updates
        connect(m_adapter, &ComboboxListAdapter::beginModelChange, this, [this]() {
            m_peristentId = idxToUuid(m_idx);
        });
        connect(m_adapter, &ComboboxListAdapter::endModelChange, this, [this]() {
            if (m_peristentId.isValid()) {
                m_idx = idxFromUuid(m_peristentId);
            }
        });
    }

    QAbstractItemModel *model() const { return m_adapter; }

    void setSelectedIdx(int index)
    {
        m_idx = index;
        storeSettings();
        Q_EMIT selectedChanged();
    }
    int selectedIdx() const { return m_idx; }
    T::DataType selected() const
    {
        const auto idx = m_adapter->mapToSource(m_adapter->index(m_idx, 0));
        return idx.isValid() ? m_model->userData(idx) : typename T::DataType{};
    }

    void readSettings()
    {
        auto &s = Settings::global();
        s.sync();
        if (s.contains(m_settingsKey)) {
            m_idx = idxFromUuid(QUuid(s.value(m_settingsKey).toString()));
        }
    }
    void storeSettings() const
    {
        auto &s = Settings::global();
        s.setValue(m_settingsKey, idxToUuid(m_idx).toString());
        s.sync();
    }

private:
    QVariant idxToUuid(int idx) const { return m_adapter->index(idx, 0).data(cUuidRole); }
    int idxFromUuid(const QVariant &uuid) const
    {
        bool found = false;
        auto i     = 0;
        while (!found && i < m_adapter->rowCount()) {
            found = idxToUuid(i) == uuid;
            ++i;
        }
        return found ? i - 1 : -1;
    }

    QString m_settingsKey;
    T *m_model;
    ComboboxListAdapter *m_adapter;
    QVariant m_peristentId;
    int m_idx{ -1 };
};
