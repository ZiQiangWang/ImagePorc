#ifndef UNDOHISTORY_H
#define UNDOHISTORY_H

#include <QDockWidget>

class QUndoGroup;

namespace Ui {
class UndoHistory;
}

class UndoHistory : public QDockWidget
{
    Q_OBJECT

public:
    explicit UndoHistory(QWidget *parent = 0);
    ~UndoHistory();

    void setUndoGroup(QUndoGroup *undoGroup);

    QUndoGroup *getUndoGroup() const;

private slots:
    void on_pushButton_undo_clicked();

    void on_pushButton_redo_clicked();

private:
    Ui::UndoHistory *ui;

    QUndoGroup *m_undoGroup;
};

#endif // UNDOHISTORY_H
