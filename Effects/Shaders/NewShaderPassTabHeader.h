#ifndef NEWSHADERPASSTABHEADER_H
#define NEWSHADERPASSTABHEADER_H

#include <QWidget>
#include "ShaderPass.h"

namespace Ui {
class NewShaderPassTabHeader;
}

class NewShaderPassTabHeader : public QWidget
{
    Q_OBJECT

public:
    explicit NewShaderPassTabHeader(QWidget *parent = nullptr);
    ~NewShaderPassTabHeader();

private slots:
    void on_add_clicked();

private:
    Ui::NewShaderPassTabHeader *ui;

signals:
    void Added(ShaderPass::Type);
};

#endif // NEWSHADERPASSTABHEADER_H
