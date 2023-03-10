/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-11 by Raw Material Software Ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the GNU General
   Public License (Version 2), as published by the Free Software Foundation.
   A copy of the license is included in the JUCE distribution, or can be found
   online at www.gnu.org/licenses.

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.rawmaterialsoftware.com/juce for more information.

  ==============================================================================
*/

class CodeDocumentLine
{
public:
    CodeDocumentLine (const String::CharPointerType& line_,
                      const int lineLength_,
                      const int numNewLineChars,
                      const int lineStartInFile_)
        : line (line_, (size_t) lineLength_),
          lineStartInFile (lineStartInFile_),
          lineLength (lineLength_),
          lineLengthWithoutNewLines (lineLength_ - numNewLineChars)
    {
    }

    static void createLines (Array <CodeDocumentLine*>& newLines, const String& text)
    {
        String::CharPointerType t (text.getCharPointer());
        int charNumInFile = 0;
        bool finished = false;

        while (! (finished || t.isEmpty()))
        {
            String::CharPointerType startOfLine (t);
            int startOfLineInFile = charNumInFile;
            int lineLength = 0;
            int numNewLineChars = 0;

            for (;;)
            {
                const juce_wchar c = t.getAndAdvance();

                if (c == 0)
                {
                    finished = true;
                    break;
                }

                ++charNumInFile;
                ++lineLength;

                if (c == '\r')
                {
                    ++numNewLineChars;

                    if (*t == '\n')
                    {
                        ++t;
                        ++charNumInFile;
                        ++lineLength;
                        ++numNewLineChars;
                    }

                    break;
                }

                if (c == '\n')
                {
                    ++numNewLineChars;
                    break;
                }
            }

            newLines.add (new CodeDocumentLine (startOfLine, lineLength,
                                                numNewLineChars, startOfLineInFile));
        }

        jassert (charNumInFile == text.length());
    }

    bool endsWithLineBreak() const noexcept
    {
        return lineLengthWithoutNewLines != lineLength;
    }

    void updateLength() noexcept
    {
        lineLength = 0;
        lineLengthWithoutNewLines = 0;

        String::CharPointerType t (line.getCharPointer());

        for (;;)
        {
            const juce_wchar c = t.getAndAdvance();

            if (c == 0)
                break;

            ++lineLength;

            if (c != '\n' && c != '\r')
                lineLengthWithoutNewLines = lineLength;
        }
    }

    String line;
    int lineStartInFile, lineLength, lineLengthWithoutNewLines;
};

//==============================================================================
CodeDocument::Iterator::Iterator (const CodeDocument& document_) noexcept
    : document (&document_),
      charPointer (nullptr),
      line (0),
      position (0)
{
}

CodeDocument::Iterator::Iterator (const CodeDocument::Iterator& other) noexcept
    : document (other.document),
      charPointer (other.charPointer),
      line (other.line),
      position (other.position)
{
}

CodeDocument::Iterator& CodeDocument::Iterator::operator= (const CodeDocument::Iterator& other) noexcept
{
    document = other.document;
    charPointer = other.charPointer;
    line = other.line;
    position = other.position;

    return *this;
}

CodeDocument::Iterator::~Iterator() noexcept
{
}

juce_wchar CodeDocument::Iterator::nextChar() noexcept
{
    for (;;)
    {
        if (charPointer.getAddress() == nullptr)
        {
            const CodeDocumentLine* const l = document->lines[line];

            if (l == nullptr)
                return 0;

            charPointer = l->line.getCharPointer();
        }

        const juce_wchar result = charPointer.getAndAdvance();

        if (result == 0)
        {
            ++line;
            charPointer = nullptr;
        }
        else
        {
            ++position;
            return result;
        }
    }
}

void CodeDocument::Iterator::skip() noexcept
{
    nextChar();
}

void CodeDocument::Iterator::skipToEndOfLine() noexcept
{
    if (charPointer.getAddress() == nullptr)
    {
        const CodeDocumentLine* const l = document->lines[line];

        if (l == nullptr)
            return;

        charPointer = l->line.getCharPointer();
    }

    position += (int) charPointer.length();
    ++line;
    charPointer = nullptr;
}

juce_wchar CodeDocument::Iterator::peekNextChar() const noexcept
{
    if (charPointer.getAddress() == nullptr)
    {
        const CodeDocumentLine* const l = document->lines[line];

        if (l == nullptr)
            return 0;

        charPointer = l->line.getCharPointer();
    }

    const juce_wchar c = *charPointer;

    if (c != 0)
        return c;

    const CodeDocumentLine* const l = document->lines [line + 1];
    return l == nullptr ? 0 : l->line[0];
}

void CodeDocument::Iterator::skipWhitespace() noexcept
{
    while (CharacterFunctions::isWhitespace (peekNextChar()))
        skip();
}

bool CodeDocument::Iterator::isEOF() const noexcept
{
    return charPointer.getAddress() == nullptr && line >= document->lines.size();
}

//==============================================================================
CodeDocument::Position::Position() noexcept
    : owner (nullptr), characterPos (0), line (0),
      indexInLine (0), positionMaintained (false)
{
}

CodeDocument::Position::Position (const CodeDocument& ownerDocument,
                                  const int line_, const int indexInLine_) noexcept
    : owner (const_cast <CodeDocument*> (&ownerDocument)),
      characterPos (0), line (line_),
      indexInLine (indexInLine_), positionMaintained (false)
{
    setLineAndIndex (line_, indexInLine_);
}

CodeDocument::Position::Position (const CodeDocument& ownerDocument, const int characterPos_) noexcept
    : owner (const_cast <CodeDocument*> (&ownerDocument)),
      positionMaintained (false)
{
    setPosition (characterPos_);
}

CodeDocument::Position::Position (const Position& other) noexcept
    : owner (other.owner), characterPos (other.characterPos), line (other.line),
      indexInLine (other.indexInLine), positionMaintained (false)
{
    jassert (*this == other);
}

CodeDocument::Position::~Position()
{
    setPositionMaintained (false);
}

CodeDocument::Position& CodeDocument::Position::operator= (const Position& other)
{
    if (this != &other)
    {
        const bool wasPositionMaintained = positionMaintained;
        if (owner != other.owner)
            setPositionMaintained (false);

        owner = other.owner;
        line = other.line;
        indexInLine = other.indexInLine;
        characterPos = other.characterPos;
        setPositionMaintained (wasPositionMaintained);

        jassert (*this == other);
    }

    return *this;
}

bool CodeDocument::Position::operator== (const Position& other) const noexcept
{
    jassert ((characterPos == other.characterPos)
               == (line == other.line && indexInLine == other.indexInLine));

    return characterPos == other.characterPos
            && line == other.line
            && indexInLine == other.indexInLine
            && owner == other.owner;
}

bool CodeDocument::Position::operator!= (const Position& other) const noexcept
{
    return ! operator== (other);
}

void CodeDocument::Position::setLineAndIndex (const int newLineNum, const int newIndexInLine)
{
    jassert (owner != nullptr);

    if (owner->lines.size() == 0)
    {
        line = 0;
        indexInLine = 0;
        characterPos = 0;
    }
    else
    {
        if (newLineNum >= owner->lines.size())
        {
            line = owner->lines.size() - 1;

            const CodeDocumentLine& l = *owner->lines.getUnchecked (line);
            indexInLine = l.lineLengthWithoutNewLines;
            characterPos = l.lineStartInFile + indexInLine;
        }
        else
        {
            line = jmax (0, newLineNum);

            const CodeDocumentLine& l = *owner->lines.getUnchecked (line);

            if (l.lineLengthWithoutNewLines > 0)
                indexInLine = jlimit (0, l.lineLengthWithoutNewLines, newIndexInLine);
            else
                indexInLine = 0;

            characterPos = l.lineStartInFile + indexInLine;
        }
    }
}

void CodeDocument::Position::setPosition (const int newPosition)
{
    jassert (owner != nullptr);

    line = 0;
    indexInLine = 0;
    characterPos = 0;

    if (newPosition > 0)
    {
        int lineStart = 0;
        int lineEnd = owner->lines.size();

        for (;;)
        {
            if (lineEnd - lineStart < 4)
            {
                for (int i = lineStart; i < lineEnd; ++i)
                {
                    const CodeDocumentLine& l = *owner->lines.getUnchecked (i);
                    const int index = newPosition - l.lineStartInFile;

                    if (index >= 0 && (index < l.lineLength || i == lineEnd - 1))
                    {
                        line = i;
                        indexInLine = jmin (l.lineLengthWithoutNewLines, index);
                        characterPos = l.lineStartInFile + indexInLine;
                    }
                }

                break;
            }
            else
            {
                const int midIndex = (lineStart + lineEnd + 1) / 2;

                if (newPosition >= owner->lines.getUnchecked (midIndex)->lineStartInFile)
                    lineStart = midIndex;
                else
                    lineEnd = midIndex;
            }
        }
    }
}

void CodeDocument::Position::moveBy (int characterDelta)
{
    jassert (owner != nullptr);

    if (characterDelta == 1)
    {
        setPosition (getPosition());

        // If moving right, make sure we don't get stuck between the \r and \n characters..
        if (line < owner->lines.size())
        {
            const CodeDocumentLine& l = *owner->lines.getUnchecked (line);

            if (indexInLine + characterDelta < l.lineLength
                 && indexInLine + characterDelta >= l.lineLengthWithoutNewLines + 1)
                ++characterDelta;
        }
    }

    setPosition (characterPos + characterDelta);
}

CodeDocument::Position CodeDocument::Position::movedBy (const int characterDelta) const
{
    CodeDocument::Position p (*this);
    p.moveBy (characterDelta);
    return p;
}

CodeDocument::Position CodeDocument::Position::movedByLines (const int deltaLines) const
{
    CodeDocument::Position p (*this);
    p.setLineAndIndex (getLineNumber() + deltaLines, getIndexInLine());
    return p;
}

juce_wchar CodeDocument::Position::getCharacter() const
{
    const CodeDocumentLine* const l = owner->lines [line];
    return l == nullptr ? 0 : l->line [getIndexInLine()];
}

String CodeDocument::Position::getLineText() const
{
    const CodeDocumentLine* const l = owner->lines [line];
    return l == nullptr ? String::empty : l->line;
}

void CodeDocument::Position::setPositionMaintained (const bool isMaintained)
{
    if (isMaintained != positionMaintained)
    {
        positionMaintained = isMaintained;

        if (owner != nullptr)
        {
            if (isMaintained)
            {
                jassert (! owner->positionsToMaintain.contains (this));
                owner->positionsToMaintain.add (this);
            }
            else
            {
                // If this happens, you may have deleted the document while there are Position objects that are still using it...
                jassert (owner->positionsToMaintain.contains (this));
                owner->positionsToMaintain.removeFirstMatchingValue (this);
            }
        }
    }
}

//==============================================================================
CodeDocument::CodeDocument()
    : undoManager (std::numeric_limits<int>::max(), 10000),
      currentActionIndex (0),
      indexOfSavedState (-1),
      maximumLineLength (-1),
      newLineChars ("\r\n")
{
}

CodeDocument::~CodeDocument()
{
}

String CodeDocument::getAllContent() const
{
    return getTextBetween (Position (*this, 0),
                           Position (*this, lines.size(), 0));
}

String CodeDocument::getTextBetween (const Position& start, const Position& end) const
{
    if (end.getPosition() <= start.getPosition())
        return String::empty;

    const int startLine = start.getLineNumber();
    const int endLine = end.getLineNumber();

    if (startLine == endLine)
    {
        CodeDocumentLine* const line = lines [startLine];
        return (line == nullptr) ? String::empty : line->line.substring (start.getIndexInLine(), end.getIndexInLine());
    }

    MemoryOutputStream mo;
    mo.preallocate ((size_t) (end.getPosition() - start.getPosition() + 4));

    const int maxLine = jmin (lines.size() - 1, endLine);

    for (int i = jmax (0, startLine); i <= maxLine; ++i)
    {
        const CodeDocumentLine& line = *lines.getUnchecked(i);
        int len = line.lineLength;

        if (i == startLine)
        {
            const int index = start.getIndexInLine();
            mo << line.line.substring (index, len);
        }
        else if (i == endLine)
        {
            len = end.getIndexInLine();
            mo << line.line.substring (0, len);
        }
        else
        {
            mo << line.line;
        }
    }

    return mo.toString();
}

int CodeDocument::getNumCharacters() const noexcept
{
    const CodeDocumentLine* const lastLine = lines.getLast();
    return (lastLine == nullptr) ? 0 : lastLine->lineStartInFile + lastLine->lineLength;
}

String CodeDocument::getLine (const int lineIndex) const noexcept
{
    const CodeDocumentLine* const line = lines [lineIndex];
    return (line == nullptr) ? String::empty : line->line;
}

int CodeDocument::getMaximumLineLength() noexcept
{
    if (maximumLineLength < 0)
    {
        maximumLineLength = 0;

        for (int i = lines.size(); --i >= 0;)
            maximumLineLength = jmax (maximumLineLength, lines.getUnchecked(i)->lineLength);
    }

    return maximumLineLength;
}

void CodeDocument::deleteSection (const Position& startPosition, const Position& endPosition)
{
    deleteSection (startPosition.getPosition(), endPosition.getPosition());
}

void CodeDocument::deleteSection (const int start, const int end)
{
    remove (start, end, true);
}

void CodeDocument::insertText (const Position& position, const String& text)
{
    insertText (position.getPosition(), text);
}

void CodeDocument::insertText (const int insertIndex, const String& text)
{
    insert (text, insertIndex, true);
}

void CodeDocument::replaceSection (const int start, const int end, const String& newText)
{
    insertText (start, newText);
    const int newTextLen = newText.length();
    deleteSection (start + newTextLen, end + newTextLen);
}

void CodeDocument::applyChanges (const String& newContent)
{
    TextDiff diff (getAllContent(), newContent);

    for (int i = 0; i < diff.changes.size(); ++i)
    {
        const TextDiff::Change& c = diff.changes.getReference(i);

        if (c.isDeletion())
            remove (c.start, c.start + c.length, true);
        else
            insert (c.insertedText, c.start, true);
    }
}

void CodeDocument::replaceAllContent (const String& newContent)
{
    remove (0, getNumCharacters(), true);
    insert (newContent, 0, true);
}

bool CodeDocument::loadFromStream (InputStream& stream)
{
    remove (0, getNumCharacters(), false);
    insert (stream.readEntireStreamAsString(), 0, false);
    setSavePoint();
    clearUndoHistory();
    return true;
}

bool CodeDocument::writeToStream (OutputStream& stream)
{
    for (int i = 0; i < lines.size(); ++i)
    {
        String temp (lines.getUnchecked(i)->line); // use a copy to avoid bloating the memory footprint of the stored string.
        const char* utf8 = temp.toUTF8();

        if (! stream.write (utf8, (int) strlen (utf8)))
            return false;
    }

    return true;
}

void CodeDocument::setNewLineCharacters (const String& newLineChars_) noexcept
{
    jassert (newLineChars_ == "\r\n" || newLineChars_ == "\n" || newLineChars_ == "\r");
    newLineChars = newLineChars_;
}

void CodeDocument::newTransaction()
{
    undoManager.beginNewTransaction (String::empty);
}

void CodeDocument::undo()
{
    newTransaction();
    undoManager.undo();
}

void CodeDocument::redo()
{
    undoManager.redo();
}

void CodeDocument::clearUndoHistory()
{
    undoManager.clearUndoHistory();
}

void CodeDocument::setSavePoint() noexcept
{
    indexOfSavedState = currentActionIndex;
}

bool CodeDocument::hasChangedSinceSavePoint() const noexcept
{
    return currentActionIndex != indexOfSavedState;
}

//==============================================================================
namespace CodeDocumentHelpers
{
    static int getCharacterType (const juce_wchar character) noexcept
    {
        return (CharacterFunctions::isLetterOrDigit (character) || character == '_')
                    ? 2 : (CharacterFunctions::isWhitespace (character) ? 0 : 1);
    }

    static bool isTokenCharacter (const juce_wchar c) noexcept
    {
        return CharacterFunctions::isLetterOrDigit (c) || c == '.' || c == '_';
    }
}

CodeDocument::Position CodeDocument::findWordBreakAfter (const Position& position) const noexcept
{
    Position p (position);
    const int maxDistance = 256;
    int i = 0;

    while (i < maxDistance
            && CharacterFunctions::isWhitespace (p.getCharacter())
            && (i == 0 || (p.getCharacter() != '\n'
                            && p.getCharacter() != '\r')))
    {
        ++i;
        p.moveBy (1);
    }

    if (i == 0)
    {
        const int type = CodeDocumentHelpers::getCharacterType (p.getCharacter());

        while (i < maxDistance && type == CodeDocumentHelpers::getCharacterType (p.getCharacter()))
        {
            ++i;
            p.moveBy (1);
        }

        while (i < maxDistance
                && CharacterFunctions::isWhitespace (p.getCharacter())
                && (i == 0 || (p.getCharacter() != '\n'
                                && p.getCharacter() != '\r')))
        {
            ++i;
            p.moveBy (1);
        }
    }

    return p;
}

CodeDocument::Position CodeDocument::findWordBreakBefore (const Position& position) const noexcept
{
    Position p (position);
    const int maxDistance = 256;
    int i = 0;
    bool stoppedAtLineStart = false;

    while (i < maxDistance)
    {
        const juce_wchar c = p.movedBy (-1).getCharacter();

        if (c == '\r' || c == '\n')
        {
            stoppedAtLineStart = true;

            if (i > 0)
                break;
        }

        if (! CharacterFunctions::isWhitespace (c))
            break;

        p.moveBy (-1);
        ++i;
    }

    if (i < maxDistance && ! stoppedAtLineStart)
    {
        const int type = CodeDocumentHelpers::getCharacterType (p.movedBy (-1).getCharacter());

        while (i < maxDistance && type == CodeDocumentHelpers::getCharacterType (p.movedBy (-1).getCharacter()))
        {
            p.moveBy (-1);
            ++i;
        }
    }

    return p;
}

void CodeDocument::findTokenContaining (const Position& pos, Position& start, Position& end) const noexcept
{
    end = pos;
    while (CodeDocumentHelpers::isTokenCharacter (end.getCharacter()))
        end.moveBy (1);

    start = end;
    while (start.getIndexInLine() > 0
            && CodeDocumentHelpers::isTokenCharacter (start.movedBy (-1).getCharacter()))
        start.moveBy (-1);
}

void CodeDocument::findLineContaining  (const Position& pos, Position& s, Position& e) const noexcept
{
    s.setLineAndIndex (pos.getLineNumber(), 0);
    e.setLineAndIndex (pos.getLineNumber() + 1, 0);
}

void CodeDocument::checkLastLineStatus()
{
    while (lines.size() > 0
            && lines.getLast()->lineLength == 0
            && (lines.size() == 1 || ! lines.getUnchecked (lines.size() - 2)->endsWithLineBreak()))
    {
        // remove any empty lines at the end if the preceding line doesn't end in a newline.
        lines.removeLast();
    }

    const CodeDocumentLine* const lastLine = lines.getLast();

    if (lastLine != nullptr && lastLine->endsWithLineBreak())
    {
        // check that there's an empty line at the end if the preceding one ends in a newline..
        lines.add (new CodeDocumentLine (String::empty.getCharPointer(), 0, 0, lastLine->lineStartInFile + lastLine->lineLength));
    }
}

//==============================================================================
void CodeDocument::addListener    (CodeDocument::Listener* const l) noexcept   { listeners.add (l); }
void CodeDocument::removeListener (CodeDocument::Listener* const l) noexcept   { listeners.remove (l); }

//==============================================================================
class CodeDocumentInsertAction   : public UndoableAction
{
public:
    CodeDocumentInsertAction (CodeDocument& doc, const String& t, const int pos) noexcept
        : owner (doc), text (t), insertPos (pos)
    {
    }

    bool perform()
    {
        owner.currentActionIndex++;
        owner.insert (text, insertPos, false);
        return true;
    }

    bool undo()
    {
        owner.currentActionIndex--;
        owner.remove (insertPos, insertPos + text.length(), false);
        return true;
    }

    int getSizeInUnits()        { return text.length() + 32; }

private:
    CodeDocument& owner;
    const String text;
    const int insertPos;

    JUCE_DECLARE_NON_COPYABLE (CodeDocumentInsertAction);
};

void CodeDocument::insert (const String& text, const int insertPos, const bool undoable)
{
    if (text.isNotEmpty())
    {
        if (undoable)
        {
            undoManager.perform (new CodeDocumentInsertAction (*this, text, insertPos));
        }
        else
        {
            Position pos (*this, insertPos);
            const int firstAffectedLine = pos.getLineNumber();

            CodeDocumentLine* const firstLine = lines [firstAffectedLine];
            String textInsideOriginalLine (text);

            if (firstLine != nullptr)
            {
                const int index = pos.getIndexInLine();
                textInsideOriginalLine = firstLine->line.substring (0, index)
                                         + textInsideOriginalLine
                                         + firstLine->line.substring (index);
            }

            maximumLineLength = -1;
            Array <CodeDocumentLine*> newLines;
            CodeDocumentLine::createLines (newLines, textInsideOriginalLine);
            jassert (newLines.size() > 0);

            CodeDocumentLine* const newFirstLine = newLines.getUnchecked (0);
            newFirstLine->lineStartInFile = firstLine != nullptr ? firstLine->lineStartInFile : 0;
            lines.set (firstAffectedLine, newFirstLine);

            if (newLines.size() > 1)
                lines.insertArray (firstAffectedLine + 1, newLines.getRawDataPointer() + 1, newLines.size() - 1);

            int lineStart = newFirstLine->lineStartInFile;
            for (int i = firstAffectedLine; i < lines.size(); ++i)
            {
                CodeDocumentLine& l = *lines.getUnchecked (i);
                l.lineStartInFile = lineStart;
                lineStart += l.lineLength;
            }

            checkLastLineStatus();

            const int newTextLength = text.length();
            for (int i = 0; i < positionsToMaintain.size(); ++i)
            {
                CodeDocument::Position& p = *positionsToMaintain.getUnchecked(i);

                if (p.getPosition() >= insertPos)
                    p.setPosition (p.getPosition() + newTextLength);
            }

            listeners.call (&CodeDocument::Listener::codeDocumentTextInserted, text, insertPos);
        }
    }
}

//==============================================================================
class CodeDocumentDeleteAction  : public UndoableAction
{
public:
    CodeDocumentDeleteAction (CodeDocument& doc, const int start, const int end) noexcept
        : owner (doc), startPos (start), endPos (end),
          removedText (doc.getTextBetween (CodeDocument::Position (doc, start),
                                           CodeDocument::Position (doc, end)))
    {
    }

    bool perform()
    {
        owner.currentActionIndex++;
        owner.remove (startPos, endPos, false);
        return true;
    }

    bool undo()
    {
        owner.currentActionIndex--;
        owner.insert (removedText, startPos, false);
        return true;
    }

    int getSizeInUnits()    { return (endPos - startPos) + 32; }

private:
    CodeDocument& owner;
    const int startPos, endPos;
    const String removedText;

    JUCE_DECLARE_NON_COPYABLE (CodeDocumentDeleteAction);
};

void CodeDocument::remove (const int startPos, const int endPos, const bool undoable)
{
    if (endPos <= startPos)
        return;

    if (undoable)
    {
        undoManager.perform (new CodeDocumentDeleteAction (*this, startPos, endPos));
    }
    else
    {
        Position startPosition (*this, startPos);
        Position endPosition (*this, endPos);

        maximumLineLength = -1;
        const int firstAffectedLine = startPosition.getLineNumber();
        const int endLine = endPosition.getLineNumber();
        CodeDocumentLine& firstLine = *lines.getUnchecked (firstAffectedLine);

        if (firstAffectedLine == endLine)
        {
            firstLine.line = firstLine.line.substring (0, startPosition.getIndexInLine())
                           + firstLine.line.substring (endPosition.getIndexInLine());
            firstLine.updateLength();
        }
        else
        {
            CodeDocumentLine& lastLine = *lines.getUnchecked (endLine);

            firstLine.line = firstLine.line.substring (0, startPosition.getIndexInLine())
                            + lastLine.line.substring (endPosition.getIndexInLine());
            firstLine.updateLength();

            int numLinesToRemove = endLine - firstAffectedLine;
            lines.removeRange (firstAffectedLine + 1, numLinesToRemove);
        }

        for (int i = firstAffectedLine + 1; i < lines.size(); ++i)
        {
            CodeDocumentLine& l = *lines.getUnchecked (i);
            const CodeDocumentLine& previousLine = *lines.getUnchecked (i - 1);
            l.lineStartInFile = previousLine.lineStartInFile + previousLine.lineLength;
        }

        checkLastLineStatus();

        const int totalChars = getNumCharacters();

        for (int i = 0; i < positionsToMaintain.size(); ++i)
        {
            CodeDocument::Position& p = *positionsToMaintain.getUnchecked(i);

            if (p.getPosition() > startPosition.getPosition())
                p.setPosition (jmax (startPos, p.getPosition() + startPos - endPos));

            if (p.getPosition() > totalChars)
                p.setPosition (totalChars);
        }

        listeners.call (&CodeDocument::Listener::codeDocumentTextDeleted, startPos, endPos);
    }
}
