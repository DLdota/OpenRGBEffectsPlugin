#include "Mask.h"
#include "ColorUtils.h"

REGISTER_EFFECT(Mask);

Mask::Mask(QWidget *parent) :
    RGBEffect(parent),
    ui(new Ui::Mask)
{
    ui->setupUi(this);

    EffectDetails.EffectName = "Mask";
    EffectDetails.EffectClassName = ClassName();
    EffectDetails.EffectDescription = "A simple mask for using in layers.";

    EffectDetails.IsReversable = false;
    EffectDetails.MaxSpeed     = 0;
    EffectDetails.MinSpeed     = 0;
    EffectDetails.UserColors   = 2;
    EffectDetails.AllowOnlyFirst = false;

    EffectDetails.MaxSlider2Val = 0;
    EffectDetails.MinSlider2Val = 0;
    EffectDetails.Slider2Name   = "";

    EffectDetails.HasCustomWidgets = true;
    EffectDetails.HasCustomSettings = true;
}

Mask::~Mask()
{
    delete ui;
}

void Mask::DefineExtraOptions(QLayout* layout)
{
    layout->addWidget(this);
}

void Mask::StepEffect(std::vector<ControllerZone*> controller_zones)
{
    for(unsigned int i = 0; i < controller_zones.size(); i++)
    {
        int start_idx = controller_zones[i]->start_idx();
        zone_type ZT = controller_zones[i]->type();
        int leds_count = controller_zones[i]->leds_count();

        if (ZT == ZONE_TYPE_SINGLE || ZT == ZONE_TYPE_LINEAR)
        {
            for (int LedID = 0; LedID < leds_count; LedID++)
            {
                RGBColor color = GetColor(LedID, 0.5, leds_count - 1, 1);
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
                    RGBColor color = GetColor(col_id, row_id, cols - 1, rows - 1);

                    int LedID = controller_zones[i]->controller->zones[controller_zones[i]->zone_idx].matrix_map->map[((row_id * cols) + col_id)];
                    controller_zones[i]->SetLED(start_idx + LedID, color, Brightness);
                }
            }

        }
    }
}

RGBColor Mask::GetColor(float x, float y, float w, float h)
{
    if(w == 0 || h == 0)
    {
        return UserColors[invert_colors ? 1 : 0];
    }

    return restrict.contains(x/w, y/h) ? UserColors[invert_colors? 1: 0] : UserColors[invert_colors? 0: 1];
}

void Mask::on_x_valueChanged(double value)
{
    restrict.setLeft(value);
}

void Mask::on_y_valueChanged(double value)
{
    restrict.setTop(value);
}

void Mask::on_w_valueChanged(double value)
{
    restrict.setWidth(value);
}

void Mask::on_h_valueChanged(double value)
{
    restrict.setHeight(value);
}

void Mask::on_invert_colors_stateChanged(int state)
{
    invert_colors = state;
}

void Mask::LoadCustomSettings(json j)
{
    if (j.contains("x")) ui->x->setValue(j["x"]);
    if (j.contains("y")) ui->y->setValue(j["y"]);
    if (j.contains("w")) ui->w->setValue(j["w"]);
    if (j.contains("h")) ui->h->setValue(j["h"]);
    if (j.contains("invert_colors")) ui->invert_colors->setChecked(j["invert_colors"]);
}

json Mask::SaveCustomSettings(json j)
{
    j["x"] = restrict.left();
    j["y"] = restrict.top();
    j["w"] = restrict.width();
    j["h"] = restrict.height();
    j["invert_colors"] =  ui->invert_colors->isChecked();

    return j;
}
