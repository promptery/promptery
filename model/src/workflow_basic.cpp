#include <model/workflow_basic.h>

#include <model/defines.h>

#include <common/log.h>

#include <QAbstractItemModel>
#include <QFile>

WorkflowBasic::WorkflowBasic(LlmInterface *backend,
                             QAbstractItemModel *contentModel,
                             QString model,
                             QString query,
                             ContextFiles contextFiles,
                             ContextPages contextPages,
                             ChatData chat,
                             SystemPromptData systemPrompt,
                             DecoratorPromptData decoratorPrompt)
    : m_backend(backend)
    , m_contentModel(contentModel)
    , m_model(std::move(model))
    , m_query(std::move(query))
    , m_contextFiles(std::move(contextFiles))
    , m_contextPages(std::move(contextPages))
    , m_chat(std::move(chat))
    , m_systemPrompt(std::move(systemPrompt))
    , m_decoratorPrompt(std::move(decoratorPrompt))
{
}

ChatRequest WorkflowBasic::nextRequest()
{
    m_started = true;

    // decorator prompt
    const auto before = m_decoratorPrompt.decoratorBefore();
    if (!before.isEmpty()) {
        m_query = before + "\n" + m_query;
    }
    const auto after = m_decoratorPrompt.decoratorAfter();
    if (!after.isEmpty()) {
        m_query += "\n" + after;
    }

    // add context pages
    if (!m_contextPages.empty()) {
        for (const auto &id : m_contextPages.ids) {
            const auto data = dataAdapter(m_contentModel, id, cDataRole);
            const auto name = dataAdapter(m_contentModel, id, Qt::DisplayRole);
            if (data.isValid() && name.isValid()) {
                auto content =
                    QString("Additional context\nName: '%1'\n====\n").arg(name.toString());
                content += data.toString();
                content += "\n====\n\n";
                m_query = content + m_query;
            } else {
                log(QObject::tr("Could not read content for key '%1'").arg(id.toString()));
            }
        }
    }
    // end add context pages

    // load file context
    if (!m_contextFiles.empty()) {
        for (const auto &f : m_contextFiles.files) {
            QFile file(m_contextFiles.rootPath + "/" + f);
            if (file.open(QIODevice::ReadOnly)) {
                auto content = QString("Additional context\nUri: '%1'\n====\n").arg(f);
                content += QString::fromLocal8Bit(file.readAll());
                content += "\n====\n\n";
                m_query = content + m_query;
            } else {
                log(QObject::tr("Could not open file '%1'").arg(file.fileName()));
            }
        }
    }
    // end load file context

    if (!m_contextFiles.empty() || !m_contextPages.empty()) {
        m_query = "Consider the following context(s).\n\n" + m_query;
    }

    auto json = chatAsJson();
    json.append(QJsonObject{ { "role", "user" }, { "content", m_query } });

    return ChatRequest{ m_title, m_backend, m_model, std::move(json) };
}

QJsonArray WorkflowBasic::chatAsJson() const
{
    QJsonArray json;

    auto system = m_systemPrompt.systemPrompt();
    if (!system.isEmpty()) {
        json.append(QJsonObject{ { "role", "system" }, { "content", std::move(system) } });
    }

    for (const auto &data : m_chat.interactions()) {
        if (data.enabled) {
            json.append(QJsonObject{ { "role", "user" }, { "content", data.input } });
            json.append(QJsonObject{ { "role", "assistant" }, { "content", data.output } });
        }
    }
    return json;
}
