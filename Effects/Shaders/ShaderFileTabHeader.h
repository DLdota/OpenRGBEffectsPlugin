#ifndef SHADERFILETABHEADER_H
#define SHADERFILETABHEADER_H

#include <QWidget>

namespace Ui {
class ShaderFileTabHeader;
}

class ShaderFileTabHeader : public QWidget
{
    Q_OBJECT

public:
    ShaderFileTabHeader(QWidget*, std::string, bool);
    ~ShaderFileTabHeader();

private slots:
    void on_close_clicked();

private:
    Ui::ShaderFileTabHeader *ui;

signals:
    void CloseRequest();
};

#endif // SHADERFILETABHEADER_H
