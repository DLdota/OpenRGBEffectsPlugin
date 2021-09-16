// QCodeEditor
#include <QSyntaxStyle>
#include "QtCompat.hpp"

// Qt
#include <QDebug>
#include <QFile>
#include <QXmlStreamReader>


QSyntaxStyle::QSyntaxStyle(QObject *parent) : QObject(parent), m_name(), m_data(), m_loaded(false)
{
}

bool QSyntaxStyle::load(const QString &fl)
{
    QXmlStreamReader reader(fl);

    while (!reader.atEnd() && !reader.hasError())
    {
        auto token = reader.readNext();

        if (token == QXmlStreamReader::StartElement)
        {
            if (reader.name() == u"style-scheme")
            {
                if (reader.attributes().hasAttribute("name"))
                {
                    m_name = reader.attributes().value("name").toString();
                }
            }
            else if (reader.name() == u"style")
            {
                auto attributes = reader.attributes();

                auto name = attributes.value("name");

                QTextCharFormat format;

                if (attributes.hasAttribute("background"))
                {
                    format.setBackground(QColor(attributes.value("background").toString()));
                }

                if (attributes.hasAttribute("foreground"))
                {
                    format.setForeground(QColor(attributes.value("foreground").toString()));
                }

                if (attributes.hasAttribute("bold") && attributes.value("bold") == u"true")
                {
                    format.setFontWeight(QFont::Weight::Bold);
                }

                if (attributes.hasAttribute("italic") && attributes.value("italic") == u"true")
                {
                    format.setFontItalic(true);
                }

                if (attributes.hasAttribute("underlineStyle"))
                {
                    auto underline = attributes.value("underlineStyle");

                    auto s = QTextCharFormat::UnderlineStyle::NoUnderline;

                    if (underline == u"SingleUnderline")
                    {
                        s = QTextCharFormat::UnderlineStyle::SingleUnderline;
                    }
                    else if (underline == u"DashUnderline")
                    {
                        s = QTextCharFormat::UnderlineStyle::DashUnderline;
                    }
                    else if (underline == u"DotLine")
                    {
                        s = QTextCharFormat::UnderlineStyle::DotLine;
                    }
                    else if (underline == u"DashDotLine")
                    {
                        s = QTextCharFormat::DashDotLine;
                    }
                    else if (underline == u"DashDotDotLine")
                    {
                        s = QTextCharFormat::DashDotDotLine;
                    }
                    else if (underline == u"WaveUnderline")
                    {
                        s = QTextCharFormat::WaveUnderline;
                    }
                    else if (underline == u"SpellCheckUnderline")
                    {
                        s = QTextCharFormat::SpellCheckUnderline;
                    }
                    else
                    {
                        qDebug() << "Unknown underline value " << underline;
                    }

                    format.setUnderlineStyle(s);
                }

                if (attributes.hasAttribute("underlineColor"))
                {
                    auto color = attributes.value("underlineColor");

                    format.setUnderlineColor(QColor(color.toString()));
                }

                m_data[name.toString()] = format;
            }
        }
    }

    m_loaded = !reader.hasError();

    return m_loaded;
}

QString QSyntaxStyle::name() const
{
    return m_name;
}

QTextCharFormat QSyntaxStyle::getFormat(const QString &name) const
{
    auto result = m_data.find(name);

    if (result == m_data.end())
    {
        return QTextCharFormat();
    }

    return result.value();
}

bool QSyntaxStyle::isLoaded() const
{
    return m_loaded;
}

QSyntaxStyle *QSyntaxStyle::defaultStyle()
{
    static QSyntaxStyle style;

    if (!style.isLoaded())
    {
        Q_INIT_RESOURCE(qcodeeditor_resources);
        QFile fl(":/default_style.xml");

        if (!fl.open(QIODevice::ReadOnly))
        {
            return &style;
        }

        if (!style.load(fl.readAll()))
        {
            qDebug() << "Can't load default style.";
        }
    }

    return &style;
}
