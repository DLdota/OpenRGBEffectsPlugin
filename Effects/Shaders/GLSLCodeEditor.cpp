#include "GLSLCodeEditor.h"
#include "ui_GLSLCodeEditor.h"
#include "QGLSLHighlighter.hpp"
#include "QGLSLCompleter.hpp"

#include <QDirIterator>
#include <QSyntaxStyle>

GLSLCodeEditor::GLSLCodeEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GLSLCodeEditor)
{
    ui->setupUi(this);

    ui->content->setHighlighter(new QGLSLHighlighter());
    ui->content->setCompleter(new QGLSLCompleter());

    setWindowTitle("Shader editor");

    /*-----------------------------------------------*\
    | List the embeded styles, add them to the        |
    | combo box                                       |
    \*-----------------------------------------------*/
    QDirIterator it(":/styles");

    while (it.hasNext())
    {
        QString path = it.next();
        ui->styles->addItem(path.split( "/" ).last());
    }

}


void GLSLCodeEditor::on_apply_clicked()
{
    emit Applied();
}

void GLSLCodeEditor::on_styles_currentIndexChanged(int)
{

    QFile file(":styles/" + ui->styles->currentText());

    if (!file.open(QIODevice::ReadOnly))
    {
        return;
    }

    auto style = new QSyntaxStyle(this);

    if (!style->load(file.readAll()))
    {
        delete style;
        return;
    }

    ui->content->setSyntaxStyle(style);
}

std::string GLSLCodeEditor::GetContent()
{
    return ui->content->toPlainText().toStdString();
}

void GLSLCodeEditor::SetContent(QString content)
{
    ui->content->clear();
    ui->content->setText(content);
}

void GLSLCodeEditor::SetLog(QString log)
{
    ui->logs->clear();
    ui->logs->setText(log);
}

GLSLCodeEditor::~GLSLCodeEditor()
{
    delete ui;
}
