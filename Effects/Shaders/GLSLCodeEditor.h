#ifndef GLSLCODEEDITOR_H
#define GLSLCODEEDITOR_H

#include <QDialog>
#include "GLSLHighlighter.h"
#include "ShaderProgram.h"
#include "QCodeEditor.hpp"
#include "ShaderPassEditor.h"

namespace Ui {
class GLSLCodeEditor;
}

class GLSLCodeEditor : public QDialog
{
    Q_OBJECT

public:
    GLSLCodeEditor(QWidget*, ShaderProgram*);
    ~GLSLCodeEditor();

//    void SetContent(QString);
//    void SetVersion(QString);

//    std::string GetContent();
//    std::string GetVersion();

    void SetProgram(ShaderProgram*);

signals:
    void Applied(ShaderProgram*);

public slots:
    void SetLog(QString);

private slots:
    void on_apply_clicked();
    void on_styles_currentIndexChanged(int);

    void on_help_styles_clicked();
    void on_help_versions_clicked();
    void on_help_shaders_clicked();

private:
    Ui::GLSLCodeEditor *ui;
    GLSLHighlighter* highlighter;
    ShaderProgram* shader_program;

    void ApplyCurrentStyle();
    void LoadStyleFromFile();
    void RecreateFileTabs();

    std::map<ShaderPass*, ShaderPassEditor*> pass_editors;

    QSyntaxStyle* style = nullptr;

    QString readme = SHADERS_README;
};

#endif // GLSLCODEEDITOR_H
