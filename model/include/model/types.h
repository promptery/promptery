#pragma once

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>

#include <vector>

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
        QString output;
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
            res.input   = interaction["input"].toString();
            res.output  = interaction["output"].toString();
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

            arr.append(QJsonObject{ { "input", i.input },
                                    { "contextFiles",
                                      QJsonObject{ { "rootPath", i.contextFiles.rootPath },
                                                   { "files", std::move(files) } } },
                                    { "contextPages", QJsonObject{ { "ids", std::move(ids) } } },
                                    { "output", i.output },
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
