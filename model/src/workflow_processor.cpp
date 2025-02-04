#include <model/workflow_processor.h>

#include <model/llm_interface.h>

#include <common/log.h>

#include <QJsonDocument>
#include <QNetworkReply>


bool WorkflowProcessor::start(std::unique_ptr<WorkflowInterface> workflow)
{
    if (m_workflow || !workflow) {
        return false;
    }
    m_workflow = std::move(workflow);
    return beginNextStep();
}

void WorkflowProcessor::stop()
{
    if (m_reply) {
        cleanUpReply();
        finish();
    }
}

bool WorkflowProcessor::isActive() const
{
    return m_reply;
}

bool WorkflowProcessor::beginNextStep()
{
    if (m_workflow->hasNext()) {
        auto request = m_workflow->nextRequest();
        if (request.isValid()) {
            ++m_blockIndex;
            if (m_workflow->isComplexWorkflow()) {
                Q_EMIT beginBlock(m_blockIndex, request.title);
            }
            m_responses.push_back(QString());

            m_reply = request.backend->asyncChat(
                std::move(request.model), std::move(request.ollamaMessages), request.options);
            if (m_reply && m_reply->isOpen()) {
                connect(m_reply, &QNetworkReply::readyRead, this, &WorkflowProcessor::readyRead);
                connect(m_reply, &QNetworkReply::finished, this, &WorkflowProcessor::finishedReply);
                connect(m_reply,
                        &QNetworkReply::errorOccurred,
                        this,
                        &WorkflowProcessor::errorOccurred);

                return true;
            } else {
                cleanUpReply();
            }
        }
        return false;
    }

    finish();

    return true;
}

void WorkflowProcessor::cleanUpReply()
{
    if (m_workflow->isComplexWorkflow()) {
        Q_EMIT endBlock(m_blockIndex);
    }

    if (m_reply) {
        m_reply->disconnect(this);
        m_reply->deleteLater();
        m_reply = nullptr;
    }
}

void WorkflowProcessor::finish()
{
    Q_EMIT finished();

    assert(!m_reply);
    m_blockIndex = -1;
    m_responses.clear();
    m_workflow = nullptr;
}

void WorkflowProcessor::errorOccurred(QNetworkReply::NetworkError error)
{
    QString errorString = m_reply->errorString();
    log(tr("Network error '%1' (%2)").arg(errorString).arg(error));
}

void WorkflowProcessor::finishedReply()
{
    stop();
}

void WorkflowProcessor::readyRead()
{
    const auto text = m_reply->readAll();
    const auto doc  = QJsonDocument::fromJson(text);
    if (doc.isNull() || !doc.isObject()) {
        log("Could not decode expected JSON from server response.");
        return;
    }

    // the check for "done" needs to come first
    if (doc.object()["done"].toBool()) {
        // final part of current request
        m_workflow->finishRequest(ChatResponse{ m_responses.back() });

        cleanUpReply();

        /*{
          "model": "llama3",
          "created_at": "2023-08-04T19:22:45.499127Z",
          "done": true,
          "total_duration": 4883583458,
          "load_duration": 1334875,
          "prompt_eval_count": 26,
          "prompt_eval_duration": 342546000,
          "eval_count": 282,
          "eval_duration": 4535599000
        }*/

        const auto model         = doc.object().value("model").toString();
        const auto promptCount   = doc.object().value("prompt_eval_count").toInt();
        const auto responseCount = doc.object().value("eval_count").toInt();
        log(QString("Model: %3. Token count: query: %1, response: %2.")
                .arg(promptCount)
                .arg(responseCount)
                .arg(model));

        beginNextStep();
    } else if (doc.object()["message"].isObject()) {
        const auto content = doc.object()["message"].toObject()["content"].toString();
        m_responses.back().append(content);
        Q_EMIT newContent(std::move(content));
    }
}
