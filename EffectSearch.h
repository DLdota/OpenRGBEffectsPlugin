#ifndef EFFECTSEARCH_H
#define EFFECTSEARCH_H

#include <QWidget>
#include <QListWidgetItem>
#include <QKeyEvent>

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
    void FocusSearch();

private slots:
    void on_search_textChanged(const QString&);
    void on_results_itemClicked(QListWidgetItem*);

signals:
    void EffectClicked(std::string);
    void Searching(bool);


protected:
    void keyPressEvent(QKeyEvent*);

private:
    Ui::EffectSearch *ui;
    std::vector<std::string> strings;
};

#endif // EFFECTSEARCH_H
