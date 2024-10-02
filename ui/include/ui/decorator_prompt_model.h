#pragma once

#include <ui/model_interface.h>
#include <ui/named_object_model.h>

#include <QJsonObject>
#include <QJsonValue>


struct DecoratorPromptData {
    DecoratorPromptData() = default;
    DecoratorPromptData(QString before, QString after, QString comment)
        : m_before(std::move(before))
        , m_after(std::move(after))
        , m_comment(std::move(comment))
    {
    }

    DecoratorPromptData(const QJsonValue &json)
    {
        auto obj  = json.toObject();
        m_before  = obj["before"].toString();
        m_after   = obj["after"].toString();
        m_comment = obj["comment"].toString();
    }
    QJsonValue toJson() const
    {
        return QJsonObject{ { "before", m_before },
                            { "after", m_after },
                            { "comment", m_comment } };
    }

    QString decoratorBefore() const { return m_before; }
    QString decoratorAfter() const { return m_after; }
    QString comment() const { return m_comment; }

private:
    QString m_before;
    QString m_after;
    QString m_comment;
};

class DecoratorPromptModel : public NamedObjectModelWithData<DecoratorPromptData>,
                             public ModelInterface
{
    using Base = NamedObjectModelWithData<DecoratorPromptData>;

public:
    using Base::Base;

    TileChildData tileData() const override { return { tr("Prompt decorators") }; }

    void readSettings() override { Base::readSettings(cId); }
    void storeSettings() const override { Base::storeSettings(cId); }

protected:
    QString newObjectName() const override { return tr("new decorator prompt"); }

private:
    static constexpr const char *cId = "decoratorPrompts";
};
