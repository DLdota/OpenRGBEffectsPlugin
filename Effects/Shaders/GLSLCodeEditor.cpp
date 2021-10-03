#include "GLSLCodeEditor.h"
#include "ui_GLSLCodeEditor.h"
#include "QGLSLHighlighter.hpp"
#include "QGLSLCompleter.hpp"
#include "NewShaderPassTabHeader.h"
#include "ShaderFileTabHeader.h"

#include <QDirIterator>
#include <QSyntaxStyle>
#include <QToolButton>
#include <QDesktopServices>
#include <QUrl>

GLSLCodeEditor::GLSLCodeEditor(QWidget *parent, ShaderProgram* program) :
    QDialog(parent),
    ui(new Ui::GLSLCodeEditor)
{

    ui->setupUi(this);

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

    LoadStyleFromFile();

    SetProgram(program);
}

void GLSLCodeEditor::RecreateFileTabs()
{
    pass_editors.clear();

    /*-----------------------------------------------*\
    | Remove intial dummy tabs                        |
    \*-----------------------------------------------*/
    ui->files->clear();

    ui->files->tabBar()->setStyleSheet("QTabBar::tab:hover {text-decoration: underline;}");

    /*-----------------------------------------------*\
    | Create add button                               |
    \*-----------------------------------------------*/
    NewShaderPassTabHeader* new_header_tab_header = new NewShaderPassTabHeader();
    ui->files->addTab(new QLabel(""), QString(""));
    ui->files->tabBar()->setTabButton(0, QTabBar::RightSide, new_header_tab_header);
    ui->files->setTabEnabled(0, false);

    new_header_tab_header->setDisabled(shader_program->passes.size() >= 4);

    connect(new_header_tab_header, &NewShaderPassTabHeader::Added, [=](ShaderPass::Type t){
        shader_program->passes.push_back(new ShaderPass(t));
        RecreateFileTabs();
        ui->files->setCurrentIndex(ui->files->count() - 2);
    });

    /*-----------------------------------------------*\
    | Create passes files                             |
    \*-----------------------------------------------*/
    int i = 0;
    for(ShaderPass* pass: shader_program->passes)
    {
        ShaderPassEditor* editor = new ShaderPassEditor(this, pass);
        int tab_size = ui->files->count();
        int tab_position = tab_size;

        ShaderFileTabHeader* tab_header = new ShaderFileTabHeader(this, "iChannel" + std::to_string(i++), true);
        ui->files->insertTab(tab_position, editor, "");
        ui->files->tabBar()->setTabButton(tab_position, QTabBar::RightSide, tab_header);

        editor->setText(QString::fromStdString(pass->data.fragment_shader));
        editor->setHighlighter(new QGLSLHighlighter());
        editor->setCompleter(new QGLSLCompleter());

        pass_editors[pass] = editor;

        connect(tab_header, &ShaderFileTabHeader::CloseRequest, [=](){
            shader_program->passes.erase(std::find(shader_program->passes.begin(), shader_program->passes.end(), pass));
            RecreateFileTabs();
        });
    }

    /*-----------------------------------------------*\
    | Create main pass file                           |
    \*-----------------------------------------------*/
    ShaderPassEditor* editor = new ShaderPassEditor(this, shader_program->main_pass);
    editor->setText(QString::fromStdString(shader_program->main_pass->data.fragment_shader));

    int tab_size = ui->files->count();
    int tab_position = tab_size;
    ShaderFileTabHeader* tab_header = new ShaderFileTabHeader(this, "Main shader", false);

    ui->files->insertTab(tab_position, editor, "");
    ui->files->tabBar()->setTabButton(tab_position, QTabBar::RightSide, tab_header);

    editor->setText(QString::fromStdString(shader_program->main_pass->data.fragment_shader));
    editor->setHighlighter(new QGLSLHighlighter());
    editor->setCompleter(new QGLSLCompleter());

    pass_editors[shader_program->main_pass] = editor;
    ui->files->setCurrentIndex(tab_position);

    /*-----------------------------------------------*\
    | Make sure the style is applied                  |
    \*-----------------------------------------------*/
    ApplyCurrentStyle();
}

void GLSLCodeEditor::SetProgram(ShaderProgram* program)
{
    shader_program = program->Copy();
    ui->version->setText(QString::fromStdString(shader_program->GetVersion()));
    RecreateFileTabs();
}

void GLSLCodeEditor::LoadStyleFromFile()
{
    style = new QSyntaxStyle(this);

    QFile file(":styles/" + ui->styles->currentText());

    if (!file.open(QIODevice::ReadOnly))
    {
        return;
    }

    if (!style->load(file.readAll()))
    {
        delete style;
        return;
    }
}

void GLSLCodeEditor::ApplyCurrentStyle()
{
    std::map<ShaderPass*, ShaderPassEditor*>::iterator it;

    for (it = pass_editors.begin(); it != pass_editors.end(); it++)
    {
        it->second->UpdateStyle(style);
    }
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


void GLSLCodeEditor::on_styles_currentIndexChanged(int)
{
    LoadStyleFromFile();
    ApplyCurrentStyle();
}

void GLSLCodeEditor::on_apply_clicked()
{
    std::string version = ui->version->text().toStdString();

    shader_program->SetVersion(version);

    for(ShaderPass* pass: shader_program->passes)
    {
        pass->data.fragment_shader = pass_editors[pass]->toPlainText().toStdString();
    }

    shader_program->main_pass->data.fragment_shader = pass_editors[shader_program->main_pass]->toPlainText().toStdString();

    emit Applied(shader_program->Copy());
}

void GLSLCodeEditor::on_help_styles_clicked()
{
    QDesktopServices::openUrl(QUrl(readme+"#styles"));
}

void GLSLCodeEditor::on_help_versions_clicked()
{
    QDesktopServices::openUrl(QUrl(readme+"#versions"));
}

void GLSLCodeEditor::on_help_shaders_clicked()
{
    QDesktopServices::openUrl(QUrl(readme+"#shaders"));
}
