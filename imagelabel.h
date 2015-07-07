#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QLabel>

class ImageLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ImageLabel(QWidget *parent = 0);
    
signals:
    
protected:
    void paintEvent(QPaintEvent *);

public slots:

private:
    QString imagePath;
};

#endif // IMAGELABEL_H
