#ifndef LAYERENTRY_H
#define LAYERENTRY_H

#include <QWidget>
#include "json.hpp"

using json = nlohmann::json;

namespace Ui {
class LayerEntry;
}

class LayerEntry : public QWidget
{
    Q_OBJECT

public:
    explicit LayerEntry(QWidget *parent, std::map<std::string, std::vector<std::string>>, std::vector<std::string>);
    ~LayerEntry(); 

    json ToJson();
    void FromJson(json);

signals:
    void BlendingUpdated(int);
    void PresetUpdated(std::string, std::string);
    void Remove();

private slots:
    void on_effect_currentIndexChanged(int);
    void on_preset_currentIndexChanged(int);
    void on_composer_fn_currentIndexChanged(int);
    void on_remove_clicked();

private:
    Ui::LayerEntry *ui;

    std::map<std::string, std::vector<std::string>> effects_and_presets;
    std::vector<std::string> composer_functions;
};

#endif // LAYERENTRY_H
