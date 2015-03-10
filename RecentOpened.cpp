#include "RecentOpened.h"
#include <QMenu>
#include <QSettings>
#include <QFileInfo>
#include <QAction>
#include <QDebug>
RecentOpened::RecentOpened(QMenu *menu,QObject *parent) :
    QObject(parent)
{

    m_menu = menu;
    m_clear = m_menu->actions().at(0);
    initRecent();
}

void RecentOpened::initRecent()
{
    QSettings settings("settings.ini",QSettings::IniFormat);
    settings.beginGroup("RecentlyOpened");

    QStringList keys,values;
    keys = settings.childKeys();
    for(int i=0;i<keys.size();i++)
        values << settings.value(keys[i]).toString();

    settings.endGroup();

    if(keys.size())
    {
        m_clear->setEnabled(true);
        m_clear->setText("清除记录");
    }
    for(int i=0;i<values.size();i++)
    {
        QFileInfo info(values[i]);
        QString item = QString("%1 %2").arg(i+1).arg(info.fileName());
        QAction *recent = new QAction(QIcon(values[i]),item,this);
        recent->setToolTip(values[i]);
        connect(recent,&QAction::triggered,this,&RecentOpened::actionTriggered);
        m_menu->insertAction(m_clear,recent);
    }
}

void RecentOpened::UpdateList(const QString &file)
{
    QStringList keys,values;
    QSettings settings("settings.ini",QSettings::IniFormat);
    settings.beginGroup("RecentlyOpened");

    keys = settings.childKeys();
    for(int i=0;i<keys.size();i++)
        values << settings.value(keys[i]).toString();
    settings.endGroup();

    int index = values.indexOf(file);
    if(index != -1)
    {
        values.removeAt(index);
    }

    QFileInfo fi(file);
    if(fi.exists())
    {
        values.insert(0,file);
    }
    keys.clear();
    for(int i=0;i<values.size();i++)
    {
        keys << QString::number(i+1);
    }

    settings.remove("RecentlyOpened");
    settings.beginGroup("RecentlyOpened");
    for(int i=0;i<values.size();i++)
    {
        settings.setValue(keys[i],values[i]);
    }
    settings.endGroup();

    QList<QAction *> actions = m_menu->actions();
    for(int i=0;i<actions.size()-1;i++)
    {
        m_menu->removeAction(actions[i]);
    }

    for(int i=0;i<values.size();i++)
    {
        QFileInfo info(values[i]);
        QString item = QString("%1 %2").arg(i+1).arg(info.fileName());
        QAction *recent = new QAction(QIcon(values[i]),item,this);
        recent->setToolTip(values[i]);
        connect(recent,&QAction::triggered,this,&RecentOpened::actionTriggered);
        m_menu->insertAction(m_clear,recent);
    }

    m_clear->setEnabled(true);
    m_clear->setText("清除记录");
}

void RecentOpened::actionTriggered()
{
    int index=0;
    QList<QAction *> actions = m_menu->actions();
    for(int i=0;i<actions.size()-1;i++)
    {
        if(sender() == actions[i])
        {
            index = i+1;
            emit SelectedItem(i+1);
            break;
        }
    }
    QSettings settings("settings.ini",QSettings::IniFormat);
    settings.beginGroup("RecentlyOpened");
    this->UpdateList(settings.value(QString::number(index)).toString());
    settings.endGroup();
}

void RecentOpened::clearActions()
{
    QSettings settings("settings.ini",QSettings::IniFormat);
    settings.remove("RecentlyOpened");

    QList<QAction *> actions = m_menu->actions();

    if(actions.size()>1)
    {
        for(int i=0;i<actions.size()-1;i++)
        {
            m_menu->removeAction(actions[i]);
        }
        m_clear->setEnabled(false);
        m_clear->setText("(无)");
    }
}
