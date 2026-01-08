#include "historymanager.h"

HistoryManager::HistoryManager(QObject *parent)
    : QObject(parent), m_currentIndex(0)
{
}

void HistoryManager::addHistory(const QString &text)
{
    if (text.trimmed().isEmpty()) return;

    m_history.removeAll(text); // Remove duplicates
    m_history.append(text);
    
    if (m_history.size() > MAX_HISTORY_COUNT) {
        m_history.removeFirst();
    }
    
    // Reset to "new input" state (index = size)
    m_currentIndex = m_history.size();
    m_currentDraft.clear();
}

void HistoryManager::setDraft(const QString &draft)
{
    // Only save draft if we are currently at the "new input" position
    if (m_currentIndex == m_history.size()) {
        m_currentDraft = draft;
    }
}

QString HistoryManager::getDraft() const
{
    return m_currentDraft;
}

QString HistoryManager::getPrevious()
{
    if (m_history.isEmpty()) return QString();

    if (m_currentIndex > 0) {
        m_currentIndex--;
    }
    
    return m_history.at(m_currentIndex);
}

QString HistoryManager::getNext()
{
    if (m_history.isEmpty()) return m_currentDraft;

    if (m_currentIndex < m_history.size()) {
        m_currentIndex++;
    }

    if (m_currentIndex == m_history.size()) {
        return m_currentDraft;
    }
    
    return m_history.at(m_currentIndex);
}

bool HistoryManager::hasPrevious() const
{
    return m_currentIndex > 0;
}

bool HistoryManager::hasNext() const
{
    return m_currentIndex < m_history.size();
}

bool HistoryManager::isAtNewest() const
{
    return m_currentIndex == m_history.size();
}

int HistoryManager::currentIndex() const
{
    return m_currentIndex;
}

int HistoryManager::count() const
{
    return m_history.size();
}
