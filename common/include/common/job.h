#ifndef JOB_H
#define JOB_H

#include <QObject>
#include <QProcess>
#include <QUrl>

enum class VideoQuality { q2160p, q1440p, q1080p, q720p };
inline const char *videoQualityStr(VideoQuality quality)
{
    switch (quality) {
    case VideoQuality::q2160p: return "2160p";
    case VideoQuality::q1440p: return "1440p";
    case VideoQuality::q1080p: return "1080p";
    case VideoQuality::q720p: return "720p";
    }
    return "";
}

class Job : public QObject
{
    Q_OBJECT
public:
    enum class State { open, error, finished };

    Job(const QUrl &url);
    ~Job();

    bool start(VideoQuality quality);
    void terminate();

    QString name() const { return m_url.toString(); }
    QUrl url() const { return m_url; }

    const std::vector<QString> &output() const { return m_output; }

    bool hasError() const { return m_state == State::error; }
    bool isFinished() const { return m_state == State::finished; }

    Q_SIGNAL void newOutput(const QString &output);
    Q_SIGNAL void finished(bool withError);

private:
    Q_SLOT void readyReadStandardError();
    Q_SLOT void readyReadStandardOutput();

    Q_SLOT void errorOccurred(QProcess::ProcessError error);
    Q_SLOT void finishedProcess(int exitCode, QProcess::ExitStatus exitStatus);

    void appendOutput(const QString &output);

    QProcess m_process;
    std::vector<QString> m_output;

    QUrl m_url;
    State m_state{ State::open };
};

#endif // JOB_H
