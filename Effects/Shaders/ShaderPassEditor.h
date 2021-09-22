#ifndef SHADERPASSEDITOR_H
#define SHADERPASSEDITOR_H

#include <QWidget>
#include "QGLSLHighlighter.hpp"
#include "QGLSLCompleter.hpp"
#include "ShaderPass.h"

namespace Ui {
class ShaderPassEditor;
}

class ShaderPassEditor : public QWidget
{
    Q_OBJECT

public:
    ShaderPassEditor(QWidget*, ShaderPass*);
    ~ShaderPassEditor();

    void setText(QString);
    void setHighlighter(QGLSLHighlighter*);
    void setCompleter(QGLSLCompleter*);

    QString toPlainText();

    void UpdateStyle(QSyntaxStyle*);

private slots:
    void on_choose_texture_clicked();

private:
    Ui::ShaderPassEditor *ui;
    ShaderPass* pass;
};

#endif // SHADERPASSEDITOR_H
