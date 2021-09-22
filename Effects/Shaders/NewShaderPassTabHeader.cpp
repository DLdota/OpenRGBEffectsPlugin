#include "NewShaderPassTabHeader.h"
#include "ui_NewShaderPassTabHeader.h"

NewShaderPassTabHeader::NewShaderPassTabHeader(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NewShaderPassTabHeader)
{
    ui->setupUi(this);

    ui->type->addItem("Texture");
    ui->type->addItem("Audio");
    ui->type->addItem("Buffer");
}

NewShaderPassTabHeader::~NewShaderPassTabHeader()
{
    delete ui;
}

void NewShaderPassTabHeader::on_add_clicked()
{
    emit Added(static_cast<ShaderPass::Type>(ui->type->currentIndex()));
}
