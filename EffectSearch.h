#ifndef EFFECTSEARCH_H
#define EFFECTSEARCH_H

#include <QWidget>
#include <QListWidgetItem>

namespace Ui {
class EffectSearch;
}

class EffectSearch : public QWidget
{
    Q_OBJECT

public:
    explicit EffectSearch(QWidget *parent = nullptr);
    ~EffectSearch();

    void add(std::string);

private slots:
    void on_search_textChanged(const QString&);
    void on_results_itemClicked(QListWidgetItem*);

signals:
    void EffectClicked(std::string);
    void Searching(bool);

private:
    Ui::EffectSearch *ui;
    std::vector<std::string> strings;
};

#endif // EFFECTSEARCH_H
