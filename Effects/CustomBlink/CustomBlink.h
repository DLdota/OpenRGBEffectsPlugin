#ifndef CUSTOMBLINK_H
#define CUSTOMBLINK_H

#include <QWidget>
#include <QStringListModel>
#include "ui_CustomBlink.h"
#include "RGBEffect.h"
#include "EffectRegisterer.h"

namespace Ui {
class CustomBlink;
}

class PatternsModel : public QStringListModel
{
    Q_OBJECT

public:
    PatternsModel(QObject* parent = Q_NULLPTR): QStringListModel(parent){}
    PatternsModel(const QStringList & strings, QObject* parent = Q_NULLPTR):QStringListModel(strings, parent){}
    Qt::ItemFlags flags (const QModelIndex & index) const {
        Qt::ItemFlags defaultFlags = QStringListModel::flags(index);
        if (index.isValid()){
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
        }
        return defaultFlags;
    }
};

class CustomBlink : public RGBEffect
{
    Q_OBJECT

public:
    explicit CustomBlink(QWidget *parent = nullptr);
    ~CustomBlink();

    EFFECT_REGISTERER(ClassName(), CAT_ADVANCED, [](){return new CustomBlink;});

    static std::string const ClassName() {return "CustomBlink";}
    void DefineExtraOptions(QLayout*) override;
    void StepEffect(std::vector<ControllerZone*>) override;
    void LoadCustomSettings(json) override;
    json SaveCustomSettings() override;

private slots:
    void on_add_clicked();
    void on_clear_clicked();
    void on_remove_clicked();
    void on_reset_clicked();
    void on_interval_valueChanged(int);
    void OnCurrentPatternChanged();

signals:
    void CurrentPatternChanged();

private:
    Ui::CustomBlink *ui;

    std::vector<RGBColor> colors;

    double time = 0.;
    double time_sine = 0.;
    double time_2sine = 0.;
    double time_4sine = 0.;
    double time_8sine = 0.;
    double time_16sine = 0.;
    unsigned int time_int = 0;
    bool even = true;

    unsigned int interval = 10;

    RGBColor GetColor(unsigned int, unsigned int, unsigned int, unsigned int);
    std::map<QString, std::function<RGBColor(unsigned int, unsigned int, unsigned int, unsigned int)>> patterns;
    PatternsModel* selected_patterns_model;

    QString current_pattern = "";

    void InitPatterns();
    void GenerateRandomColors();
};

#endif // CUSTOMBLINK_H
