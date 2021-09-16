#ifndef GLSLCODEEDITOR_H
#define GLSLCODEEDITOR_H

#include <QDialog>
#include "GLSLHighlighter.h"

namespace Ui {
class GLSLCodeEditor;
}

class GLSLCodeEditor : public QDialog
{
    Q_OBJECT

public:
    explicit GLSLCodeEditor(QWidget *parent = nullptr);
    ~GLSLCodeEditor();

    void SetContent(QString);
    std::string GetContent();

signals:
    void Applied();

public slots:
    void SetLog(QString);

private slots:
    void on_apply_clicked();
    void on_styles_currentIndexChanged(int);

private:
    Ui::GLSLCodeEditor *ui;

    GLSLHighlighter* highlighter;
};

#endif // GLSLCODEEDITOR_H
