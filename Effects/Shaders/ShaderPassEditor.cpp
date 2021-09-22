#include "ShaderPassEditor.h"
#include "ui_ShaderPassEditor.h"
#include <QFileDialog>

ShaderPassEditor::ShaderPassEditor(QWidget *parent, ShaderPass* pass) :
    QWidget(parent),
    ui(new Ui::ShaderPassEditor),
    pass(pass)
{
    ui->setupUi(this);

    ShaderPass::Type type = pass->GetType();

    ui->code->setVisible(type == ShaderPass::BUFFER);
    ui->choose_texture->setVisible(type == ShaderPass::TEXTURE);
    ui->texture_preview->setVisible(type == ShaderPass::TEXTURE);
    ui->audio_message->setVisible(type == ShaderPass::AUDIO);

    if(type ==  ShaderPass::TEXTURE)
    {
        QImage img(QString::fromStdString(pass->data.texture_path));
        ui->texture_preview->setPixmap(QPixmap::fromImage(img));
    }
}

ShaderPassEditor::~ShaderPassEditor()
{
    delete ui;
}

void ShaderPassEditor::UpdateStyle(QSyntaxStyle* style)
{
    if(pass->GetType() == ShaderPass::BUFFER)
    {
       ui->code->setSyntaxStyle(style);
    }
}

void ShaderPassEditor::setText(QString text)
{
    ui->code->setText(text);
}

void ShaderPassEditor::setHighlighter(QGLSLHighlighter* h)
{
    ui->code->setHighlighter(h);
}

void ShaderPassEditor::setCompleter(QGLSLCompleter* c)
{
    ui->code->setCompleter(c);
}

QString ShaderPassEditor::toPlainText()
{
    return ui->code->toPlainText();
}

void ShaderPassEditor::on_choose_texture_clicked()
{
    QString new_texture_path = QFileDialog::getOpenFileName(this,
        tr("Open Image"), "", tr("Image Files (*.png *.jpg *.bmp)"));

    if(!new_texture_path.isEmpty())
    {
        pass->data.texture_path = new_texture_path.toStdString();
        ui->texture_preview->setPixmap(QPixmap::fromImage(QImage(new_texture_path)));
    }
}
