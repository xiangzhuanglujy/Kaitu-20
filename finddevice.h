#ifndef FINDDEVICE_H
#define FINDDEVICE_H

#include <QDialog>

class QLabel;
//class QMovie;
class QProgressBar;

class FindDevice : public QDialog
{
    Q_OBJECT
public:
    explicit FindDevice(QWidget *parent = 0);
    
signals:

protected:
    void closeEvent(QCloseEvent *);
    
public slots:
    void setMax(int);
    void setValue(int);
    void setQString(bool);
    
private:
    QLabel *LB_device;
//    QLabel *LB_movie;
//    QMovie *movie;
    QProgressBar *progressBar;
};

#endif // FINDDEVICE_H
