#include "keyboardstate.h"
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QRegularExpression>
#include <QUrl>

KeyboardState::KeyboardState(QObject *parent)
    : QObject(parent)
{
}

KeyboardState::~KeyboardState()
{
    if (m_listenerProcess) {
        m_listenerProcess->terminate();
        m_listenerProcess->waitForFinished(1000);
        if (m_listenerProcess->state() != QProcess::NotRunning) {
            m_listenerProcess->kill();
        }
    }
}

void KeyboardState::setPackageBasePath(const QString &path)
{
    QString p = path;
    if (p.startsWith(QLatin1String("file://"))) {
        p = QUrl(p).toLocalFile();
    }
    m_packageBasePath = QDir(p).canonicalPath();
}

void KeyboardState::setScriptsPath(const QString &path)
{
    m_scriptsPath = path.trimmed();
    if (m_scriptsPath.isEmpty()) return;
    if (m_scriptsPath.startsWith(QLatin1String("file://"))) {
        m_scriptsPath = QUrl(m_scriptsPath).toLocalFile();
    }
    m_scriptsPath = QDir(m_scriptsPath).canonicalPath();
}

QString KeyboardState::listenerPath() const
{
    if (!m_scriptsPath.isEmpty()) {
        return QDir(m_scriptsPath).absoluteFilePath(QStringLiteral("hidraw_listener.sh"));
    }
    if (m_packageBasePath.isEmpty()) return QString();
    QDir dir(m_packageBasePath);
    dir.cdUp();
    return dir.absoluteFilePath(QStringLiteral("hidraw_listener.sh"));
}

QString KeyboardState::setScriptPath() const
{
    if (!m_scriptsPath.isEmpty()) {
        return QDir(m_scriptsPath).absoluteFilePath(QStringLiteral("keyboard_state.sh"));
    }
    if (m_packageBasePath.isEmpty()) return QString();
    QDir dir(m_packageBasePath);
    dir.cdUp();
    return dir.absoluteFilePath(QStringLiteral("keyboard_state.sh"));
}

void KeyboardState::startListener()
{
    const QString path = listenerPath();
    if (path.isEmpty() || !QFileInfo::exists(path)) {
        qWarning() << "hidraw_listener.sh not found at" << path;
        return;
    }
    if (m_listenerProcess) {
        m_listenerProcess->terminate();
        m_listenerProcess->waitForFinished(500);
        delete m_listenerProcess;
    }
    m_listenerProcess = new QProcess(this);
    m_listenerProcess->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_listenerProcess, &QProcess::readyReadStandardOutput, this, [this]() {
        while (m_listenerProcess->canReadLine()) {
            QByteArray line = m_listenerProcess->readLine().trimmed();
            if (!line.isEmpty()) parseLine(line);
        }
    });
    connect(m_listenerProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this](int code, QProcess::ExitStatus) {
        if (code != 0) {
            m_currentLayer = -1;
            emit currentLayerChanged();
        }
    });
    m_listenerProcess->start(path, {}, QProcess::ReadOnly);
    if (!m_listenerProcess->waitForStarted(2000)) {
        qWarning() << "Failed to start hidraw_listener.sh:" << m_listenerProcess->errorString();
    }
}

void KeyboardState::parseLine(const QByteArray &line)
{
    // "layer=N fn_lock=on" or "layer=N fn_lock=off"
    const QRegularExpression re(QStringLiteral("layer=(\\d+)\\s+fn_lock=(on|off)"));
    QRegularExpressionMatch match = re.match(QString::fromUtf8(line));
    if (!match.hasMatch()) return;
    int layer = match.captured(1).toInt();
    bool fn = (match.captured(2) == QLatin1String("on"));
    if (layer != m_currentLayer) {
        m_currentLayer = layer;
        emit currentLayerChanged();
    }
    if (fn != m_fnLockOn) {
        m_fnLockOn = fn;
        emit fnLockOnChanged();
    }
}

void KeyboardState::setLayer(int layer)
{
    if (layer < 0 || layer > 14) return;
    const QString script = setScriptPath();
    if (script.isEmpty() || !QFileInfo::exists(script)) return;
    if (m_setProcess && m_setProcess->state() != QProcess::NotRunning) return;
    if (!m_setProcess) m_setProcess = new QProcess(this);
    m_setProcess->start(script, {QStringLiteral("set"), QStringLiteral("--layer"), QString::number(layer)}, QProcess::ReadOnly);
}

void KeyboardState::setFnLock(bool on)
{
    const QString script = setScriptPath();
    if (script.isEmpty() || !QFileInfo::exists(script)) return;
    if (m_setProcess && m_setProcess->state() != QProcess::NotRunning) return;
    if (!m_setProcess) m_setProcess = new QProcess(this);
    m_setProcess->start(script, {QStringLiteral("set"), QStringLiteral("--fn-lock"), on ? QStringLiteral("on") : QStringLiteral("off")}, QProcess::ReadOnly);
}
