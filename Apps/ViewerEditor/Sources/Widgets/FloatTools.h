#ifndef FLOATTOOLS_H
#define FLOATTOOLS_H

#include <QWidget>

namespace Ui {
class FloatTools;
}

class FloatTools : public QWidget
{
    Q_OBJECT

public:
    explicit FloatTools(QWidget* parent = nullptr);
    ~FloatTools();

private:
    void initConnect();

private:
    Ui::FloatTools* ui;
};

#endif   // FLOATTOOLS_H
