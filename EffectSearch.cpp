#include "EffectSearch.h"
#include "ui_EffectSearch.h"

#include <QAction>

EffectSearch::EffectSearch(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EffectSearch)
{
    ui->setupUi(this);
    ui->results->hide();
    ui->no_results->hide();
}

EffectSearch::~EffectSearch()
{
    delete ui;
}

void EffectSearch::on_search_textChanged(const QString& search)
{
    ui->results->clear();
    ui->results->hide();
    ui->no_results->hide();

    if(search.isEmpty())
    {
        emit Searching(false);
        return;
    }

    bool results = false;

    for(const std::string& s: strings)
    {
        if(QString::fromStdString(s).toLower().contains(search.toLower()))
        {
            results = true;
            ui->results->addItem(QString::fromStdString(s));
        }
    }

    ui->no_results->setVisible(!results);
    ui->results->setVisible(results);
    emit Searching(true);
}

void EffectSearch::add(std::string s)
{
    strings.push_back(s);
}

void EffectSearch::FocusSearch()
{
    ui->search->setFocus();
}

void EffectSearch::on_results_itemClicked(QListWidgetItem* item)
{
    std::string effect_name = item->text().toStdString();

    ui->results->clear();
    ui->results->hide();
    ui->no_results->hide();
    ui->search->clear();
    emit EffectClicked(effect_name);
}