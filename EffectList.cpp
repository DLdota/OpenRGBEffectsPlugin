#include "EffectList.h"
#include "EffectListDelegate.h"
#include "ui_EffectList.h"
#include "ColorUtils.h"
#include <QStandardItemModel>

std::map<std::string, std::function<RGBEffect*()>> EffectList::effects_construtors = {};
std::map<std::string, std::vector<std::string>> EffectList::categorized_effects;

EffectList::EffectList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EffectList)
{
    ui->setupUi(this);
    ui->start_stop_all_button->setVisible(false);

    QStandardItemModel* model = new QStandardItemModel();

    for(auto const& entry: categorized_effects)
    {
        std::string category = entry.first;
        std::vector<std::string> effect_names = entry.second;

        std::sort(effect_names.begin(), effect_names.end());

        addParentItem(model, QString::fromStdString(category));

        for(auto const& effect_name: effect_names)
        {
            addChildItem(model, QString::fromStdString(effect_name), QString::fromStdString(effect_name));
        }
    }

    ui->effects->setModel(model);
    ui->effects->setItemDelegate(new EffectListDelegate());
}

EffectList::~EffectList()
{
    delete ui;
}

void EffectList::RegisterEffect(std::string name, std::string category, std::function<RGBEffect*()> constructor)
{    
    effects_construtors[name] = constructor;

    if (!categorized_effects.count(category))
    {
        std::vector<std::string> effects;
        categorized_effects[category] = effects;
    }

    categorized_effects[category].push_back(name);
}

void EffectList::addParentItem(QStandardItemModel * model, const QString& text )
{
    QStandardItem* item = new QStandardItem( text );
    item->setFlags( item->flags() & ~( Qt::ItemIsEnabled | Qt::ItemIsSelectable ) );
    item->setData( "parent", Qt::AccessibleDescriptionRole );
    QFont font = item->font();
    font.setBold( true );
    font.setItalic( true );
    item->setFont( font );
    model->appendRow( item );
}

void EffectList::addChildItem(QStandardItemModel * model, const QString& text, const QVariant& data )
{
    QStandardItem* item = new QStandardItem( text );
    item->setData( data, Qt::UserRole );
    item->setData( "child", Qt::AccessibleDescriptionRole );
    model->appendRow( item );
}

void EffectList::on_add_effect_clicked()
{
    if(ui->effects->itemData(ui->effects->currentIndex(), Qt::AccessibleDescriptionRole).toString() == QLatin1String( "child" ))
    {
        QString selected_effect = ui->effects->currentText();
        RGBEffect* effect = effects_construtors[selected_effect.toStdString()]();

        // Add some random colors, so we already see something fancy
        std::vector<RGBColor> random_colors;

        for(unsigned int i = 0; i < effect->EffectDetails.UserColors; i ++)
        {
            random_colors.push_back(ColorUtils::RandomRGBColor());
        }

        effect->SetUserColors(random_colors);

        emit EffectAdded(effect);
    }
}

void EffectList::on_start_stop_all_button_clicked()
{
    emit ToggleAllEffectsState();
}

void EffectList::ShowStartStopButton(bool visible)
{
    ui->start_stop_all_button->setVisible(visible);
}
