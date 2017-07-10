#include "LinePaintHandler.h"

#include <QPlainTextEdit>
#include <QTextCursor>
#include <texteditor/texteditor.h>

static const Core::Id CODECOVERAGE_SELECTION_ID("code-coverage");

LinePaintHandler::LinePaintHandler(TextEditor::TextEditorWidget *textEdit, const QMap<int, int> &lineCoverage) :
    textEdit(textEdit),
    lineCoverage(lineCoverage)
{
}

LinePaintHandler::~LinePaintHandler()
{
}

void LinePaintHandler::render()
{
    QTextCursor cursor = textEdit->cursorForPosition(QPoint(0, 0));
    int startBlockNumber = cursor.blockNumber();
    QTextCursor endCursor = textEdit->cursorForPosition(textEdit->viewport()->rect().bottomLeft());
    int endBlockNumber = endCursor.blockNumber();

    QMap<int, int>::Iterator beginIterator = lineCoverage.lowerBound(startBlockNumber);
    QMap<int, int>::Iterator endIterator = lineCoverage.upperBound(endBlockNumber + 1);
    QList<QTextEdit::ExtraSelection> selections;
    selections.reserve(endBlockNumber + 1 - startBlockNumber);
    int prevPos = startBlockNumber;
    for (auto it = beginIterator; it != endIterator; ++it) {
        int pos = it.key();
        int value = it.value();
        if (pos == prevPos)
            continue;

        QTextEdit::ExtraSelection selection;
        selection.format.setBackground(getColorForValue(value));
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, pos - prevPos - 1);
        cursor.movePosition(QTextCursor::StartOfBlock);
        cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
        selection.cursor = cursor;
        selections.append(selection);
        prevPos = pos;
    }

    textEdit->setExtraSelections(CODECOVERAGE_SELECTION_ID, selections);
}

void LinePaintHandler::clear()
{
    textEdit->setExtraSelections(CODECOVERAGE_SELECTION_ID, QList<QTextEdit::ExtraSelection>());
}

QColor LinePaintHandler::getColorForValue(int value) const {
    const QColor green(0, 255, 0, 20);
    const QColor red(255, 0, 0, 20);
    return value > 0 ? green : red;
}
