#ifndef HISTORYMANAGER_H
#define HISTORYMANAGER_H

#include <QStringList>
#include <QObject>

class HistoryManager : public QObject
{
    Q_OBJECT
public:
    explicit HistoryManager(QObject *parent = nullptr);

    void addHistory(const QString &text);
    QString getPrevious();
    QString getNext();
    bool hasPrevious() const;
    bool hasNext() const;
    
    // Draft management
    void setDraft(const QString &draft);
    QString getDraft() const;
    
    // State query
    bool isAtNewest() const;
    int currentIndex() const;
    int count() const;

private:
    QStringList m_history;
    QString m_currentDraft;
    int m_currentIndex;
    const int MAX_HISTORY_COUNT = 50;
};

#endif // HISTORYMANAGER_H
