#pragma once

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>

#include <vector>

class LlmInterface;
struct ContextFiles {
    QString rootPath;
    std::vector<QString> files;

    bool empty() const { return files.empty(); }
};

struct ContextPages {
    std::vector<QVariant> ids;

    bool empty() const { return ids.empty(); }
};

struct ChatData {
    struct Interaction {
        QString input;
        ContextFiles contextFiles;
        ContextPages pages;
        QString outputWithSteps;
        QString finalOutput;
        bool enabled;
    };

    ChatData() = default;

    ChatData(const QJsonValue &json)
    {
        const auto obj = json.toObject();
        m_scroll       = obj["scroll"].toInt();
        const auto arr = obj["interactions"].toArray();
        m_interactions.reserve(arr.size());
        for (const auto &i : arr) {
            const auto interaction = i.toObject();
            Interaction res;
            res.input       = interaction["input"].toString();
            res.finalOutput = interaction["output"].toString();
            if (interaction.contains("outputWithSteps")) {
                res.outputWithSteps = interaction["outputWithSteps"].toString();
            }
            if (res.outputWithSteps.isEmpty()) {
                res.outputWithSteps = res.finalOutput;
            }
            res.enabled = interaction["enabled"].toBool();

            if (!interaction["contextFiles"].isUndefined()) {
                ContextFiles contextFiles;
                const auto jsonContextFiles = interaction["contextFiles"];
                contextFiles.rootPath       = jsonContextFiles["rootPath"].toString();

                const auto arrFiles = jsonContextFiles["files"].toArray();
                contextFiles.files.reserve(arrFiles.size());
                for (const auto &f : arrFiles) {
                    contextFiles.files.push_back(f.toString());
                }
                res.contextFiles = std::move(contextFiles);
            }

            if (!interaction["contextPages"].isUndefined()) {
                ContextPages pages;
                const auto jsonContextPages = interaction["contextPages"];
                const auto arrIds           = jsonContextPages["ids"].toArray();
                pages.ids.reserve(arrIds.size());
                for (const auto &i : arrIds) {
                    pages.ids.push_back(i.toString());
                }
                res.pages = std::move(pages);
            }

            m_interactions.push_back(std::move(res));
        }
    }
    QJsonValue toJson() const
    {
        QJsonArray arr;
        for (const auto &i : m_interactions) {
            QJsonArray files;
            for (const auto &f : i.contextFiles.files) {
                files.append(f);
            }
            QJsonArray ids;
            for (const auto &i : i.pages.ids) {
                ids.append(i.toString());
            }

            arr.append(
                QJsonObject{ { "input", i.input },
                             { "contextFiles",
                               QJsonObject{ { "rootPath", i.contextFiles.rootPath },
                                            { "files", std::move(files) } } },
                             { "contextPages", QJsonObject{ { "ids", std::move(ids) } } },
                             { "outputWithSteps",
                               i.outputWithSteps == i.finalOutput ? QString() : i.outputWithSteps },
                             { "output", i.finalOutput },
                             { "enabled", i.enabled } });
        }

        return QJsonObject{ { "interactions", std::move(arr) }, { "scroll", m_scroll } };
    }

    void addInteraction(Interaction interaction)
    {
        m_interactions.push_back(std::move(interaction));
    }
    const std::vector<Interaction> &interactions() const { return m_interactions; }

    void setScroll(int scroll) { m_scroll = scroll; }
    int scroll() const { return m_scroll; }

private:
    std::vector<Interaction> m_interactions;
    int m_scroll;
};

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

struct ChatRequest {
    QString title; // the name of the workflow step
    LlmInterface *backend;
    QString model;
    QJsonArray ollamaMessages;
};

struct ChatResponse {
    QString response;
};

class WorkflowInterface
{
public:
    virtual ~WorkflowInterface() = default;

    virtual bool hasNext() const                      = 0;
    virtual ChatRequest nextRequest()                 = 0;
    virtual void finishRequest(ChatResponse response) = 0;

    virtual bool isComplexWorkflow() const = 0; // complex == several steps
};

struct WorkflowData {
    WorkflowData() = default;
    WorkflowData(QString name, QString comment)
        : m_name(std::move(name))
        , m_comment(std::move(comment))
    {
    }

    WorkflowData(const QJsonValue &json)
    {
        auto obj  = json.toObject();
        m_name    = obj["name"].toString();
        m_comment = obj["comment"].toString();
    }
    QJsonValue toJson() const
    {
        return QJsonObject{ { "name", m_name }, { "comment", m_comment } };
    }

    QString name() const { return m_name; }
    QString comment() const { return m_comment; }

private:
    QString m_name;
    QString m_comment;
};
