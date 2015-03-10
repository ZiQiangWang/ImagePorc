#ifndef TOOLLAYERATTRIBUTE_H
#define TOOLLAYERATTRIBUTE_H

#include <QDialog>

class QStandardItem;
class Picture;
class Layer;

namespace Ui {
class ToolLayerAttribute;
}

class ToolLayerAttribute : public QDialog
{
    Q_OBJECT

public:
    explicit ToolLayerAttribute(Picture *pic,QStandardItem *item, QWidget *parent = 0);
    ~ToolLayerAttribute();

    void initComboBox();
private slots:
    void on_horizontalSlider_Transparent_valueChanged(int value);

    void on_comboBox_mode_activated(int index);

    void proc_accept();

    void proc_reject();

    void on_checkBox_visible_toggled(bool checked);

signals:
    void state_changed();

private:
    Ui::ToolLayerAttribute *ui;

    QStandardItem *m_item;

    Layer *m_layer;

    Picture *m_pic;
    QString m_name;
    int m_type,m_transparent;
    bool m_checkState;
};

#endif // TOOLLAYERATTRIBUTE_H
