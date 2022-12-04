#include "OpenRGBPluginsFont.h"
#include <QFontDatabase>

OpenRGBPluginsFont* OpenRGBPluginsFont::instance;

OpenRGBPluginsFont::OpenRGBPluginsFont(){}

OpenRGBPluginsFont *OpenRGBPluginsFont::Get()
{
    if(!instance)
    {
        instance = new OpenRGBPluginsFont();
        instance->fontId = QFontDatabase::addApplicationFont(":/OpenRGBPlugins.ttf");

        if(instance->fontId == -1)
        {
            printf("Cannot load requested font.\n");
        }
        else
        {
            QString family = QFontDatabase::applicationFontFamilies(instance->fontId).at(0);
            instance->font = QFont(family, 13);
            instance->font.setStyleStrategy(QFont::PreferAntialias);
        }
    }

    return instance;
}

QString OpenRGBPluginsFont::icon(int glyph)
{
    return QChar(glyph);
}

QFont OpenRGBPluginsFont::GetFont()
{
    return Get()->font;
}

