#ifndef LNGWIDGET_H
#define LNGWIDGET_H

#include <QDialog>

class QLabel;
class QPushButton;
class QComboBox;
class QGroupBox;

class LngWidget : public QDialog
{
    Q_OBJECT
public:
    explicit LngWidget(QDialog *parent = 0);
    
    int choose;
    enum
    {
        English,
        Chinese
    };
signals:
    
public slots:
    void okPB_clicked();

protected:
    void paintEvent(QPaintEvent *);
    
private:
    QLabel *label;
    QComboBox *combo;
    QPushButton *okPB;
    QPushButton *quitPB;

};

#endif // LNGWIDGET_H
