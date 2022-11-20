#include "CustomBlink.h"
#include "ColorUtils.h"

REGISTER_EFFECT(CustomBlink);

CustomBlink::CustomBlink(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::CustomBlink)
{
    ui->setupUi(this);

    EffectDetails.EffectName = ClassName();
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "Make your own blinking sequence";
    EffectDetails.MaxSpeed     = 50;
    EffectDetails.MinSpeed     = 1;
    EffectDetails.UserColors = 4;
    EffectDetails.HasCustomSettings = true;

    colors.resize(EffectDetails.UserColors);

    SetSpeed(10);

    InitPatterns();

    for(auto const& entry: patterns)
    {
        ui->patterns->addItem(entry.first);
    }

    selected_patterns_model = new PatternsModel();
    ui->selected_patterns->setModel(selected_patterns_model);
    ui->selected_patterns->setDragDropMode(QAbstractItemView::InternalMove);

    connect(this, SIGNAL(CurrentPatternChanged()), this, SLOT(OnCurrentPatternChanged()));
}


CustomBlink::~CustomBlink()
{
    delete ui;
}

void CustomBlink::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    if(selected_patterns_model->stringList().isEmpty())
    {
        if("" != current_pattern)
        {
            current_pattern = "";
            emit CurrentPatternChanged();
        }

        return;
    }

    unsigned int number_steps = selected_patterns_model->stringList().size();
    int step = (time_int / interval) % number_steps;
    QString pattern = selected_patterns_model->stringList()[step];

    if(pattern != current_pattern)
    {
        if(RandomColorsEnabled)
        {
            GenerateRandomColors();
        }
        else
        {
            colors = UserColors;
        }

        emit CurrentPatternChanged();
    }

    current_pattern = pattern;

    for(unsigned int i = 0; i < controller_zones.size(); i++)
    {
        int start_idx = controller_zones[i]->start_idx();
        zone_type ZT = controller_zones[i]->type();
        int leds_count = controller_zones[i]->leds_count();

        if (ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            for (int LedID = 0; LedID < leds_count; LedID++)
            {
                RGBColor color = GetColor(LedID, 0, leds_count, 1);
                controller_zones[i]->SetLED(start_idx + LedID, color, Brightness);
            }
        }

        else if (ZT == ZONE_TYPE_MATRIX)
        {
            int cols = controller_zones[i]->matrix_map_width();
            int rows = controller_zones[i]->matrix_map_height();

            for (int col_id = 0; col_id < cols; col_id++)
            {
                for (int row_id = 0; row_id < rows; row_id++)
                {
                    int LedID = controller_zones[i]->controller->zones[controller_zones[i]->zone_idx].matrix_map->map[((row_id * cols) + col_id)];
                    RGBColor color = GetColor(col_id, row_id, cols, rows);
                    controller_zones[i]->SetLED(start_idx + LedID, color, Brightness);
                }
            }

        }
    }

    time += 0.1 * Speed / (float) FPS;
    time_int = (unsigned int) time;
    even = time_int % 2 == 0;
    time_sine = 0.5+(0.5 * sin(time));
    time_2sine = 0.5+(0.5 * sin(2*time));
    time_4sine = 0.5+(0.5 * sin(4*time));
    time_8sine = 0.5+(0.5 * sin(8*time));
    time_16sine = 0.5+(0.5 * sin(16*time));
}

void CustomBlink::InitPatterns()
{
    patterns["Blink 1"] = [=](unsigned int x, unsigned int, unsigned int, unsigned int){
        if(int(x) % 2 == 0)
            return ColorUtils::OFF();

        if(int(x+1) % 4 == 0)
            return colors[even?0:1];

        return colors[even?1:0];
    };

    patterns["Blink 2"] = [=](unsigned int x, unsigned int, unsigned int, unsigned int){
        if(int(x) % 2 == 0)
            return ColorUtils::OFF();

        if(int(x+1) % 4 == 0)
            return colors[even?2:3];

        return colors[even?3:2];
    };

    patterns["Marquee 1"] = [=](unsigned int x, unsigned int, unsigned int, unsigned int){
        if(int(time+x) % 2 == 0)
            return ColorUtils::OFF();

        if(int(time+x+1) % 4 == 0)
            return colors[even?0:1];

        return colors[even?1:0];
    };

    patterns["Marquee 2"] = [=](unsigned int x, unsigned int, unsigned int, unsigned int){
        if(int(-time+x) % 2 == 0)
            return ColorUtils::OFF();

        if(int(-time+x+1) % 4 == 0)
            return colors[even?2:3];

        return colors[even?3:2];
    };

    patterns["Marquee 3"] = [=](unsigned int x, unsigned int, unsigned int, unsigned int){
        if(int(2*time+x) % 2 == 0)
            return ColorUtils::OFF();

        if(int(2*time+x+1) % 4 == 0)
            return colors[even?0:1];

        return colors[even?1:0];
    };

    patterns["Marquee 4"] = [=](unsigned int x, unsigned int, unsigned int, unsigned int){
        if(int(-2*time+x) % 2 == 0)
            return ColorUtils::OFF();

        if(int(-2*time+x+1) % 4 == 0)
            return colors[even?2:3];

        return colors[even?3:2];
    };

    patterns["Breath 1"] = [=](unsigned int x, unsigned int, unsigned int, unsigned int){
        if(int(x) % 2 == 0)
            return ColorUtils::OFF();

        if(int(x+1) % 4 == 0)
            return ColorUtils::apply_brightness(colors[0], time_sine);

        return ColorUtils::apply_brightness(colors[1], time_2sine);
    };

    patterns["Breath 2"] = [=](unsigned int x, unsigned int, unsigned int, unsigned int){
        if(int(time+x) % 2 == 0)
            return ColorUtils::OFF();

        if(int(time+x+1) % 4 == 0)
            return ColorUtils::apply_brightness(colors[0], time_sine);

        return ColorUtils::apply_brightness(colors[1], time_2sine);
    };

    patterns["Breath 3"] = [=](unsigned int x, unsigned int, unsigned int, unsigned int){
        if(int(x) % 2 == 0)
            return ColorUtils::OFF();

        if(int(x+1) % 4 == 0)
            return ColorUtils::apply_brightness(colors[2], time_sine);

        return ColorUtils::apply_brightness(colors[3], time_2sine);
    };

    patterns["Breath 4"] = [=](unsigned int x, unsigned int, unsigned int, unsigned int){
        if(int(-time+x) % 2 == 0)
            return ColorUtils::OFF();

        if(int(-time+x+1) % 4 == 0)
            return ColorUtils::apply_brightness(colors[2], time_sine);

        return ColorUtils::apply_brightness(colors[3], time_2sine);
    };

    patterns["Breath 5"] = [=](unsigned int x, unsigned int, unsigned int, unsigned int){
        float s = int(x) % 2 == 0 ? time_sine : 1 - time_sine;
        return ColorUtils::apply_brightness(colors[int(x) % 4], s);
    };

    patterns["Moving band 1"] = [=](unsigned int x, unsigned int, unsigned int, unsigned int){
        return colors[int(time+x) % 4];
    };

    patterns["Moving band 2"] = [=](unsigned int x, unsigned int, unsigned int, unsigned int){
        return colors[abs(int(-time+x) % 4)];
    };

    patterns["Moving band 3"] = [=](unsigned int x, unsigned int, unsigned int, unsigned int){
        return colors[int(2*time+x) % 4];
    };

    patterns["Moving band 4"] = [=](unsigned int x, unsigned int, unsigned int, unsigned int){
        return colors[abs(int(-2*time+x) % 4)];
    };

    patterns["Flash 1"] = [=](unsigned int x, unsigned int, unsigned int, unsigned int){
        if (time_2sine > 0.5)
        {
            return colors[x % 4];
        }
        else
        {
            return ColorUtils::OFF();
        }
    };

    patterns["Flash 2"] = [=](unsigned int x, unsigned int, unsigned int, unsigned int){
        if (time_4sine > 0.5)
        {
            return colors[x % 4];
        }
        else
        {
            return ColorUtils::OFF();
        }
    };

    patterns["Flash 3"] = [=](unsigned int x, unsigned int, unsigned int, unsigned int){
        if (time_8sine > 0.5)
        {
            return colors[x % 4];
        }
        else
        {
            return ColorUtils::OFF();
        }
    };

    patterns["Flash 4"] = [=](unsigned int x, unsigned int, unsigned int, unsigned int){
        if (time_16sine > 0.5)
        {
            return colors[x % 4];
        }
        else
        {
            return ColorUtils::OFF();
        }
    };

    patterns["Alternate 1"] = [=](unsigned int x, unsigned int, unsigned int w, unsigned int){
        if (x >= w/2.)
        {
            return colors[even?0:1];
        }
        else
        {
            return colors[even?1:0];
        }
    };

    patterns["Alternate 2"] = [=](unsigned int x, unsigned int, unsigned int w, unsigned int){
        if (x >= w/2.)
        {
            return colors[even?2:3];
        }
        else
        {
            return colors[even?3:2];
        }
    };
}

RGBColor CustomBlink::GetColor(unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
    return patterns[current_pattern](x,y,w,h);
}

void CustomBlink::on_add_clicked()
{
    QStringList current = selected_patterns_model->stringList();
    current << ui->patterns->currentText();
    selected_patterns_model->setStringList(current);
}

void CustomBlink::on_clear_clicked()
{
    selected_patterns_model->setStringList(QStringList());
}

void CustomBlink::on_reset_clicked()
{
    time = 0.;
}

void CustomBlink::on_interval_valueChanged(int val)
{
    interval = val;
}

void CustomBlink::on_remove_clicked()
{
    QModelIndexList selection = ui->selected_patterns->selectionModel()->selectedIndexes();

    for(const QModelIndex& model: selection)
    {
        selected_patterns_model->removeRow(model.row());
    }
}

void CustomBlink::OnCurrentPatternChanged()
{
    ui->current->setText(current_pattern);
}

void CustomBlink::LoadCustomSettings(json j)
{
    if (j.contains("interval")) ui->interval->setValue(j["interval"]);

    if (j.contains("pattern_list"))
    {
        QStringList pattern_names;

        std::vector<std::string> pattern_list = j["pattern_list"];

        for(const std::string& str: pattern_list)
        {
            QString pattern_name = QString::fromStdString(str);

            if(patterns.find(pattern_name) != patterns.end())
            {
                pattern_names << pattern_name;
            }
        }

        selected_patterns_model->setStringList(pattern_names);
    }
}

json CustomBlink::SaveCustomSettings()
{
    json settings;

    std::vector<std::string> pattern_list;

    for(const QString& str: selected_patterns_model->stringList())
    {
        pattern_list.push_back(str.toStdString());
    }

    settings["interval"] = interval;
    settings["pattern_list"] = pattern_list;

    return settings;
}

void CustomBlink::GenerateRandomColors()
{
    for(unsigned int i = 0; i < EffectDetails.UserColors; i++)
    {
        colors[i] = ColorUtils::RandomRGBColor();
    }
}
