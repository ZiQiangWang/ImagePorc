#include "UndoHistory.h"
#include "ui_UndoHistory.h"
#include <QUndoGroup>
#include <QUndoStack>
UndoHistory::UndoHistory(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::UndoHistory)
{
    ui->setupUi(this);
}

UndoHistory::~UndoHistory()
{
    delete ui;
}

void UndoHistory::setUndoGroup(QUndoGroup *undoGroup)
{
    ui->undoView->setGroup(undoGroup);
    m_undoGroup = undoGroup;
}

QUndoGroup *UndoHistory::getUndoGroup() const
{
    return m_undoGroup;
}

void UndoHistory::on_pushButton_undo_clicked()
{
    int index = m_undoGroup->activeStack()->index();
    m_undoGroup->activeStack()->setIndex(index-1);

}

void UndoHistory::on_pushButton_redo_clicked()
{
    int index = m_undoGroup->activeStack()->index();
    m_undoGroup->activeStack()->setIndex(index+1);
}
