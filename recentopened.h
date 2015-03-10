#ifndef RECENTOPENED_H
#define RECENTOPENED_H

#include <QObject>
class QMenu;
class QAction;

class RecentOpened : public QObject
{
    Q_OBJECT
public:
    explicit RecentOpened(QMenu *menu, QObject *parent = 0);

    void initRecent();

    void UpdateList(const QString &file);

    void clearActions();

signals:
    void SelectedItem(int i);
public slots:

    void actionTriggered();

private:
    QMenu *m_menu;
    QAction *m_clear;
};

#endif // RECENTOPENED_H
