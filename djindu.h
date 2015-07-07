#ifndef DJINDU_H
#define DJINDU_H

#include <QDialog>

class QLabel;
class QProgressBar;

class DJindu : public QDialog
{
    Q_OBJECT
public:
    explicit DJindu(QWidget *parent = 0);
    
signals:
    
public slots:
    void setRange(int current,int max);
    
private:
    QProgressBar *progressBar;
    QLabel *label;
};

#endif // DJINDU_H
