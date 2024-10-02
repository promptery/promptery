#pragma once

#include <ui/model_interface.h>
#include <ui/named_object_model.h>

#include <QJsonObject>
#include <QJsonValue>


struct SystemPromptData {
    SystemPromptData() = default;
    SystemPromptData(QString prompt, QString comment)
        : m_prompt(std::move(prompt))
        , m_comment(std::move(comment))
    {
    }

    SystemPromptData(const QJsonValue &json)
    {
        auto obj  = json.toObject();
        m_prompt  = obj["prompt"].toString();
        m_comment = obj["comment"].toString();
    }
    QJsonValue toJson() const
    {
        return QJsonObject{ { "prompt", m_prompt }, { "comment", m_comment } };
    }

    QString systemPrompt() const { return m_prompt; }
    QString comment() const { return m_comment; }

private:
    QString m_prompt;
    QString m_comment;
};

class SystemPromptModel : public NamedObjectModelWithData<SystemPromptData>, public ModelInterface
{
    using Base = NamedObjectModelWithData<SystemPromptData>;

public:
    using Base::Base;

    TileChildData tileData() const override { return { tr("System prompts") }; }

    void readSettings() override { Base::readSettings(cId); }
    void storeSettings() const override { Base::storeSettings(cId); }

protected:
    QString newObjectName() const override { return tr("new system prompt"); }

private:
    static constexpr const char *cId = "systemPrompts";
};
