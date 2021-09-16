#ifndef GLSLHIGHLIGHTER_H
#define GLSLHIGHLIGHTER_H

#include <QSyntaxHighlighter>

class GLSLHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    GLSLHighlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text);

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;

    QColor keywordFormatColor = QColor("#f9c80e");
    QColor classFormatColor = QColor("#f86624");
    QColor singleLineCommentFormatColor = QColor("#662e9b");
    QColor multiLineCommentFormatColor = QColor("#662e9b");
    QColor quotationFormatColor = QColor("#ea3546");
    QColor functionFormatColor = QColor("#43bccd");
};

#endif // GLSLHIGHLIGHTER_H
