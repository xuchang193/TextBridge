#pragma cone

#include <QString>

class QLockFile;

class SingleInstanceGuard {
public:
    SingleInstanceGuard();
    ~SingleInstanceGuard();

    SingleInstanceGuard(const SingleInstanceGuard&) = delete;
    SingleInstanceGuard& operator=(const SingleInstanceGuard&) = delete;

    bool tryAcquire();

private:
    QString lockFilePath() const;
    QLockFile* m_lockFile = nullptr;
    bool m_locked = false;
};
