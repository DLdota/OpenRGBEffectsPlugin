#include "EffectSearch.h"
#include "ui_EffectSearch.h"

#include <QAction>
#include <QScrollBar>

EffectSearch::EffectSearch(QWidget *parent,  unsigned int w) :
    QWidget(parent),
    ui(new Ui::EffectSearch)
{
    ui->setupUi(this);
    ui->results->hide();
    ui->results->setMaximumWidth(w);
    ui->results->horizontalScrollBar()->setDisabled(true);
    ui->no_results->hide();
    ui->no_results->setMaximumWidth(w);
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

void EffectSearch::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Return)
    {
        if(ui->results->selectedItems().size() == 1)
        {
            emit EffectClicked(ui->results->currentItem()->text().toStdString());
        }
    }
    else if(event->key() == Qt::Key_Down)
    {
        if(!ui->results->hasFocus())
        {
            ui->results->setFocus();
            ui->results->setCurrentRow(0);
        }
    }
    else if(event->key() == Qt::Key_Up)
    {
        if(ui->results->hasFocus() && ui->results->currentRow() == 0)
        {
            ui->search->setFocus();
        }
    }
    else
    {
        if(!ui->search->hasFocus())
        {
            ui->search->setFocus();
        }

        QWidget::keyPressEvent(event);
    }
}
