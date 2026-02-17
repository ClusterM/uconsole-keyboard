#pragma once

#include <QObject>
#include <QProcess>
#include <QUrl>

class KeyboardState : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int currentLayer READ currentLayer NOTIFY currentLayerChanged)
    Q_PROPERTY(bool fnLockOn READ fnLockOn NOTIFY fnLockOnChanged)
    Q_PROPERTY(QString packageBasePath WRITE setPackageBasePath)
    Q_PROPERTY(QString scriptsPath WRITE setScriptsPath)

public:
    explicit KeyboardState(QObject *parent = nullptr);
    ~KeyboardState() override;

    int currentLayer() const { return m_currentLayer; }
    bool fnLockOn() const { return m_fnLockOn; }
    void setPackageBasePath(const QString &path);
    void setScriptsPath(const QString &path);

    Q_INVOKABLE void startListener();
    Q_INVOKABLE void setLayer(int layer);
    Q_INVOKABLE void setFnLock(bool on);

Q_SIGNALS:
    void currentLayerChanged();
    void fnLockOnChanged();

private:
    void parseLine(const QByteArray &line);
    QString listenerPath() const;
    QString setScriptPath() const;

    QString m_packageBasePath;
    QString m_scriptsPath;
    int m_currentLayer = -1;
    bool m_fnLockOn = false;
    QProcess *m_listenerProcess = nullptr;
    QProcess *m_setProcess = nullptr;
};
