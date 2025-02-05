#include <ui/workflow_basic.h>

#include <ui/chat_request_config_model.h>

#include <model/defines.h>

#include <common/log.h>

#include <QAbstractItemModel>
#include <QFile>

WorkflowBasic::WorkflowBasic(ChatRequestConfigModel *config,
                             QAbstractItemModel *contentModel,
                             QString query,
                             ContextFiles contextFiles,
                             ContextPages contextPages,
                             ChatData chat,
                             DecoratorPromptData decoratorPrompt,
                             RequestOptions options)
    : m_currentConfig(config)
    , m_contentModel(contentModel)
    , m_query(std::move(query))
    , m_contextFiles(std::move(contextFiles))
    , m_contextPages(std::move(contextPages))
    , m_chat(std::move(chat))
    , m_decoratorPrompt(std::move(decoratorPrompt))
    , m_options(std::move(options))
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
                auto content = QString("\n```` {#%1}\n").arg(name.toString());
                content += data.toString();
                content += "\n````\n\n";
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
                auto content = QString("\n```` {#%1}\n").arg(f);
                content += QString::fromLocal8Bit(file.readAll());
                content += "\n````\n\n";
                m_query = content + m_query;
            } else {
                log(QObject::tr("Could not open file '%1'").arg(file.fileName()));
            }
        }
    }
    // end load file context

    if (!m_contextFiles.empty() || !m_contextPages.empty()) {
        m_query = "Context:\n" + m_query;
    }

    auto json = chatAsJson();
    json.append(QJsonObject{ { "role", "user" }, { "content", m_query } });

    return ChatRequest{ m_title,
                        m_currentConfig->selectedBackend().value(),
                        m_currentConfig->modelId(m_currentConfig->selectedModelIdx()),
                        std::move(json),
                        m_options };
}

QJsonArray WorkflowBasic::chatAsJson() const
{
    QJsonArray json;

    auto system = m_currentConfig->selectedSystemPrompt().systemPrompt();
    if (!system.isEmpty()) {
        json.append(QJsonObject{ { "role", "system" }, { "content", std::move(system) } });
    }

    for (const auto &data : m_chat.interactions()) {
        if (data.enabled) {
            json.append(QJsonObject{ { "role", "user" }, { "content", data.input } });
            json.append(QJsonObject{ { "role", "assistant" }, { "content", data.finalOutput } });
        }
    }
    return json;
}
