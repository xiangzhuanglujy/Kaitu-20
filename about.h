#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>

class QLabel;

class About : public QDialog
{
    Q_OBJECT
public:
    explicit About(QWidget *parent = 0);
    
signals:
    
public slots:
    
private:
    QLabel *productTipLB;
    QLabel *productNameLB;
    QLabel *versionTipLB;
    QLabel *versionLB;
//    QLabel *imgLB;
//    QLabel *authorTipLB;
//    QLabel *authorLB;
};

#endif // ABOUT_H
