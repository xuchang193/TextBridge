#include "singleinstanceguard.h"

#include <QCoreApplication>
#include <QDir>
#include <QLockFile>

namespace {
    const char* LOCK_FILE_NAME = "TextBridge.lock";
    const int LOCK_TIMEOUT_MS = 100;
}

SingleInstanceGuard::SingleInstanceGuard() {}

SingleInstanceGuard::~SingleInstanceGuard() {
    if (m_lockFile && m_locked) {
        m_lockFile->unlock();
    }
    delete m_lockFile;
}

QString SingleInstanceGuard::lockFilePath() const {
    const QString appDir = QCoreApplication::applicationDirPath();
    return QDir(appDir).filePath(QLatin1String(LOCK_FILE_NAME));
}

bool SingleInstanceGuard::tryAcquire() {
    if (m_locked) {
        return true;
    }
    if (!m_lockFile) {
        m_lockFile = new QLockFile(lockFilePath());
    }
    if (m_lockFile->tryLock(LOCK_TIMEOUT_MS)) {
        m_locked = true;
        return true;
    }
    return false;
}