#include <common/job.h>

#include <QStandardPaths>

Job::Job(const QUrl &url)
    : m_url(url)
{
    connect(&m_process, &QProcess::readyReadStandardError, this, &Job::readyReadStandardError);
    connect(&m_process, &QProcess::readyReadStandardOutput, this, &Job::readyReadStandardOutput);
    connect(&m_process, &QProcess::errorOccurred, this, &Job::errorOccurred);
    connect(&m_process, &QProcess::finished, this, &Job::finishedProcess);
}

Job::~Job()
{
    m_process.terminate();
    m_process.waitForFinished(1000);
    m_process.kill();
}

bool Job::start(VideoQuality quality)
{
    if (m_url.isValid()) {
        const auto home = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);

        auto env  = QProcessEnvironment::systemEnvironment();
        auto path = env.value("PATH", QString());
        path      = "/opt/homebrew/bin:" + path;
        env.insert("PATH", path);
        m_process.setProcessEnvironment(env);
        m_process.setWorkingDirectory(home[0] + "/Downloads/testing");
        auto q = "1080";
        switch (quality) {
        case VideoQuality::q2160p: q = "2160"; break;
        case VideoQuality::q1440p: q = "1440"; break;
        case VideoQuality::q1080p: q = "1080"; break;
        case VideoQuality::q720p: q = "720"; break;
        }

        m_process.start("/opt/homebrew/bin/yt-dlp",
                        { "-f", QString("bv[height<=%1]+ba/best").arg(q), m_url.toString() });
        return true;
    }
    return false;
}

void Job::terminate()
{
    m_process.terminate();
}

void Job::readyReadStandardError()
{
    appendOutput("<font color = \"orange\">" + m_process.readAllStandardError().trimmed() +
                 "</ font>");
}

void Job::readyReadStandardOutput()
{
    appendOutput(m_process.readAllStandardOutput().trimmed());
}

void Job::errorOccurred(QProcess::ProcessError error)
{
    appendOutput(QString("<font color = \"red\"> QProcess error: %1</ font>").arg(error));
}

void Job::finishedProcess(int exitCode, QProcess::ExitStatus exitStatus)
{
    appendOutput(QString("Process finished. Code: %1 Status: %2").arg(exitCode).arg(exitStatus));
    if ((exitCode != 0) || (exitStatus != QProcess::NormalExit)) {
        m_state = State::error;
    } else {
        m_state = State::finished;
    }

    Q_EMIT finished(m_state == State::error);
}

void Job::appendOutput(const QString &output)
{
    m_output.push_back(output);
    Q_EMIT newOutput(output);
}
