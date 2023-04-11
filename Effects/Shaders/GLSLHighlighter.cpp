#include "GLSLHighlighter.h"

GLSLHighlighter::GLSLHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(keywordFormatColor);
    keywordFormat.setFontWeight(QFont::Bold);

    QStringList keywordPatterns = {
        // Preprocessors
        QStringLiteral("\\#version\\b"),
        QStringLiteral("\\#define\\b"),
        QStringLiteral("\\#undef\\b"),
        QStringLiteral("\\#if\\b"),
        QStringLiteral("\\#ifdef\\b"),
        QStringLiteral("\\#ifndef\\b"),
        QStringLiteral("\\#else\\b"),
        QStringLiteral("\\#elif\\b"),
        QStringLiteral("\\#endif\\b"),
        QStringLiteral("\\#error\\b"),
        QStringLiteral("\\#pragma\\b"),
        QStringLiteral("\\#extension\\b"),
        QStringLiteral("\\#line\\b"),

        // Keywords
        QStringLiteral("\\battribute\\b"),
        QStringLiteral("\\bconst\\b"),
        QStringLiteral("\\buniform\\b"),
        QStringLiteral("\\bvarying\\b"),
        QStringLiteral("\\bcentroid\\b"),
        QStringLiteral("\\bbreak\\b"),
        QStringLiteral("\\bcontinue\\b"),
        QStringLiteral("\\bdo\\b"),
        QStringLiteral("\\bfor\\b"),
        QStringLiteral("\\bwhile\\b"),
        QStringLiteral("\\bif\\b"),
        QStringLiteral("\\belse\\b"),
        QStringLiteral("\\bin\\b"),
        QStringLiteral("\\bout\\b"),
        QStringLiteral("\\binout\\b"),
        QStringLiteral("\\bfloat\\b"),
        QStringLiteral("\\bint\\b"),
        QStringLiteral("\\bvoid\\b"),
        QStringLiteral("\\bbool\\b"),
        QStringLiteral("\\btrue\\b"),
        QStringLiteral("\\bfalse\\b"),
        QStringLiteral("\\binvariant\\b"),
        QStringLiteral("\\bdiscard\\b"),
        QStringLiteral("\\breturn\\b"),
        QStringLiteral("\\bmat2\\b"),
        QStringLiteral("\\bmat3\\b"),
        QStringLiteral("\\bmat4\\b"),
        QStringLiteral("\\bmat2x2\\b"),
        QStringLiteral("\\bmat2x3\\b"),
        QStringLiteral("\\bmat2x4\\b"),
        QStringLiteral("\\bmat3x2\\b"),
        QStringLiteral("\\bmat3x3\\b"),
        QStringLiteral("\\bmat3x4\\b"),
        QStringLiteral("\\bmat4x2\\b"),
        QStringLiteral("\\bmat4x3\\b"),
        QStringLiteral("\\bmat4x4\\b"),
        QStringLiteral("\\bvec2\\b"),
        QStringLiteral("\\bvec3\\b"),
        QStringLiteral("\\bvec4\\b"),
        QStringLiteral("\\bivec2\\b"),
        QStringLiteral("\\bivec3\\b"),
        QStringLiteral("\\bivec4\\b"),
        QStringLiteral("\\bbvec2\\b"),
        QStringLiteral("\\bbvec3\\b"),
        QStringLiteral("\\bbvec4\\b"),
        QStringLiteral("\\bsampler1D\\b"),
        QStringLiteral("\\bsampler2D\\b"),
        QStringLiteral("\\bsampler3D\\b"),
        QStringLiteral("\\bsamplerCube\\b"),
        QStringLiteral("\\bsampler1DShadow\\b"),
        QStringLiteral("\\bsampler2DShadow\\b"),
        QStringLiteral("\\bstruct\\b"),

        // Future keywords
        QStringLiteral("\\basm\\b"),
        QStringLiteral("\\bclass\\b"),
        QStringLiteral("\\bunion\\b"),
        QStringLiteral("\\benum\\b"),
        QStringLiteral("\\btypedef\\b"),
        QStringLiteral("\\btemplate\\b"),
        QStringLiteral("\\bthis\\b"),
        QStringLiteral("\\bpacked\\b"),
        QStringLiteral("\\bgoto\\b"),
        QStringLiteral("\\bswitch\\b"),
        QStringLiteral("\\bdefault\\b"),
        QStringLiteral("\\binline\\b"),
        QStringLiteral("\\bnoinline\\b"),
        QStringLiteral("\\bvolatile\\b"),
        QStringLiteral("\\bpublic\\b"),
        QStringLiteral("\\bstatic\\b"),
        QStringLiteral("\\bextern\\b"),
        QStringLiteral("\\bexternal\\b"),
        QStringLiteral("\\binterface\\b"),
        QStringLiteral("\\blong\\b"),
        QStringLiteral("\\bshort\\b"),
        QStringLiteral("\\bdouble\\b"),
        QStringLiteral("\\bhalf\\b"),
        QStringLiteral("\\bfixed\\b"),
        QStringLiteral("\\bunsigned\\b"),
        QStringLiteral("\\blowp\\b"),
        QStringLiteral("\\bmediump\\b"),
        QStringLiteral("\\bhighp\\b"),
        QStringLiteral("\\bprecision\\b"),
        QStringLiteral("\\binput\\b"),
        QStringLiteral("\\boutput\\b"),
        QStringLiteral("\\bhvec2\\b"),
        QStringLiteral("\\bhvec3\\b"),
        QStringLiteral("\\bhvec4\\b"),
        QStringLiteral("\\bdvec2\\b"),
        QStringLiteral("\\bdvec3\\b"),
        QStringLiteral("\\bdvec4\\b"),
        QStringLiteral("\\bfvec2\\b"),
        QStringLiteral("\\bfvec3\\b"),
        QStringLiteral("\\bfvec4\\b"),
        QStringLiteral("\\bsampler2DRect\\b"),
        QStringLiteral("\\bsampler3DRect\\b"),
        QStringLiteral("\\bsampler2DRectShadow\\b"),
        QStringLiteral("\\bsizeof\\b"),
        QStringLiteral("\\bcast\\b"),
        QStringLiteral("\\bnamespace\\b"),
        QStringLiteral("\\busing\\b")
    };

    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(classFormatColor);
    rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
    rule.format = classFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(singleLineCommentFormatColor);
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(multiLineCommentFormatColor);

    quotationFormat.setForeground(quotationFormatColor);
    rule.pattern = QRegExp("\".*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    functionFormat.setFontItalic(true);
    functionFormat.setForeground(functionFormatColor);
    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");
}

void GLSLHighlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);

    while (startIndex >= 0) {
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                    + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}
