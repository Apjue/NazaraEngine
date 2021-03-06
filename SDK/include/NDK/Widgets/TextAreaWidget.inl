// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/Widgets/TextAreaWidget.hpp>

namespace Ndk
{
	inline void TextAreaWidget::Clear()
	{
		m_cursorPositionBegin.MakeZero();
		m_cursorPositionEnd.MakeZero();
		m_drawer.Clear();
		m_text.Clear();
		m_textSprite->Update(m_drawer);
		SetPreferredSize(Nz::Vector2f(m_textSprite->GetBoundingVolume().obb.localBox.GetLengths()));

		RefreshCursor();
		OnTextChanged(this, m_text);
	}

	inline void TextAreaWidget::EnableMultiline(bool enable)
	{
		m_multiLineEnabled = enable;
	}

	inline void TextAreaWidget::EnableTabWriting(bool enable)
	{
		m_tabEnabled = enable;
	}

	inline void TextAreaWidget::Erase(std::size_t glyphPosition)
	{
		Erase(glyphPosition, glyphPosition + 1U);
	}

	inline const TextAreaWidget::CharacterFilter& TextAreaWidget::GetCharacterFilter() const
	{
		return m_characterFilter;
	}

	inline unsigned int TextAreaWidget::GetCharacterSize() const
	{
		return m_drawer.GetCharacterSize();
	}

	inline const Nz::Vector2ui& TextAreaWidget::GetCursorPosition() const
	{
		return m_cursorPositionBegin;
	}

	Nz::Vector2ui TextAreaWidget::GetCursorPosition(std::size_t glyphIndex) const
	{
		glyphIndex = std::min(glyphIndex, m_drawer.GetGlyphCount());

		std::size_t lineCount = m_drawer.GetLineCount();
		std::size_t line = 0U;
		for (std::size_t i = line + 1; i < lineCount; ++i)
		{
			if (m_drawer.GetLine(i).glyphIndex > glyphIndex)
				break;

			line = i;
		}

		const auto& lineInfo = m_drawer.GetLine(line);

		Nz::Vector2ui cursorPos;
		cursorPos.y = static_cast<unsigned int>(line);
		cursorPos.x = static_cast<unsigned int>(glyphIndex - lineInfo.glyphIndex);

		return cursorPos;
	}

	inline const Nz::String& TextAreaWidget::GetDisplayText() const
	{
		return m_drawer.GetText();
	}

	inline EchoMode TextAreaWidget::GetEchoMode() const
	{
		return m_echoMode;
	}

	inline std::size_t TextAreaWidget::GetGlyphIndex() const
	{
		return GetGlyphIndex(m_cursorPositionBegin);
	}

	inline std::size_t TextAreaWidget::GetGlyphIndex(const Nz::Vector2ui& cursorPosition) const
	{
		std::size_t glyphIndex = m_drawer.GetLine(cursorPosition.y).glyphIndex + cursorPosition.x;
		if (m_drawer.GetLineCount() > cursorPosition.y + 1)
			glyphIndex = std::min(glyphIndex, m_drawer.GetLine(cursorPosition.y + 1).glyphIndex - 1);
		else
			glyphIndex = std::min(glyphIndex, m_drawer.GetGlyphCount());

		return glyphIndex;
	}

	inline const Nz::String& TextAreaWidget::GetText() const
	{
		return m_text;
	}

	inline const Nz::Color& TextAreaWidget::GetTextColor() const
	{
		return m_drawer.GetColor();
	}

	inline Nz::Font* TextAreaWidget::GetTextFont() const
	{
		return m_drawer.GetFont();
	}

	inline const Nz::Color& TextAreaWidget::GetTextOulineColor() const
	{
		return m_drawer.GetOutlineColor();
	}

	inline float TextAreaWidget::GetTextOulineThickness() const
	{
		return m_drawer.GetOutlineThickness();
	}

	inline bool TextAreaWidget::HasSelection() const
	{
		return m_cursorPositionBegin != m_cursorPositionEnd;
	}

	inline bool TextAreaWidget::IsLineWrapEnabled() const
	{
		return m_isLineWrapEnabled;
	}

	inline bool TextAreaWidget::IsMultilineEnabled() const
	{
		return m_multiLineEnabled;
	}

	inline bool TextAreaWidget::IsTabWritingEnabled() const
	{
		return m_tabEnabled;
	}

	inline bool TextAreaWidget::IsReadOnly() const
	{
		return m_readOnly;
	}

	inline void TextAreaWidget::MoveCursor(int offset)
	{
		std::size_t cursorGlyph = GetGlyphIndex(m_cursorPositionBegin);
		if (offset >= 0)
			SetCursorPosition(cursorGlyph + static_cast<std::size_t>(offset));
		else
		{
			std::size_t nOffset = static_cast<std::size_t>(-offset);
			if (nOffset >= cursorGlyph)
				SetCursorPosition(0);
			else
				SetCursorPosition(cursorGlyph - nOffset);
		}
	}

	inline void TextAreaWidget::MoveCursor(const Nz::Vector2i& offset)
	{
		auto ClampOffset = [] (unsigned int cursorPosition, int cursorOffset) -> unsigned int
		{
			if (cursorOffset >= 0)
				return cursorPosition + cursorOffset;
			else
			{
				unsigned int nOffset = static_cast<unsigned int>(-cursorOffset);
				if (nOffset >= cursorPosition)
					return 0;
				else
					return cursorPosition - nOffset;
			}
		};

		Nz::Vector2ui cursorPosition = m_cursorPositionBegin;
		cursorPosition.x = ClampOffset(static_cast<unsigned int>(cursorPosition.x), offset.x);
		cursorPosition.y = ClampOffset(static_cast<unsigned int>(cursorPosition.y), offset.y);

		SetCursorPosition(cursorPosition);
	}

	inline Nz::Vector2ui TextAreaWidget::NormalizeCursorPosition(Nz::Vector2ui cursorPosition) const
	{
		std::size_t lineCount = m_drawer.GetLineCount();
		if (cursorPosition.y >= lineCount)
			cursorPosition.y = static_cast<unsigned int>(lineCount - 1);

		const auto& lineInfo = m_drawer.GetLine(cursorPosition.y);
		if (cursorPosition.y + 1 < lineCount)
		{
			const auto& nextLineInfo = m_drawer.GetLine(cursorPosition.y + 1);
			cursorPosition.x = std::min(cursorPosition.x, static_cast<unsigned int>(nextLineInfo.glyphIndex - lineInfo.glyphIndex - 1));
		}

		return cursorPosition;
	}

	inline void TextAreaWidget::SetCharacterFilter(CharacterFilter filter)
	{
		m_characterFilter = std::move(filter);
	}

	inline void TextAreaWidget::SetCursorPosition(std::size_t glyphIndex)
	{
		Nz::Vector2ui position = GetCursorPosition(glyphIndex);
		Nz::Vector2ui newPosition = position;

		OnTextAreaCursorMove(this, &newPosition);

		if (position == newPosition)
			SetCursorPositionInternal(position);
		else
			SetCursorPositionInternal(GetGlyphIndex(newPosition));
	}

	inline void TextAreaWidget::SetCursorPosition(Nz::Vector2ui cursorPosition)
	{
		OnTextAreaCursorMove(this, &cursorPosition);

		return SetCursorPositionInternal(NormalizeCursorPosition(cursorPosition));
	}

	inline void TextAreaWidget::SetEchoMode(EchoMode echoMode)
	{
		m_echoMode = echoMode;

		UpdateDisplayText();
	}

	inline void TextAreaWidget::SetReadOnly(bool readOnly)
	{
		m_readOnly = readOnly;
		m_cursorEntity->Enable(!m_readOnly && HasFocus());
	}

	inline void TextAreaWidget::SetSelection(Nz::Vector2ui fromPosition, Nz::Vector2ui toPosition)
	{
		// Ensure begin is before end
		if (toPosition.y < fromPosition.y || (toPosition.y == fromPosition.y && toPosition.x < fromPosition.x))
			std::swap(fromPosition, toPosition);

		if (m_cursorPositionBegin != fromPosition || m_cursorPositionEnd != toPosition)
		{
			OnTextAreaSelection(this, &fromPosition, &toPosition);

			// Ensure begin is before end a second time (in case signal changed it)
			if (toPosition.y < fromPosition.y || (toPosition.y == fromPosition.y && toPosition.x < fromPosition.x))
				std::swap(fromPosition, toPosition);

			m_cursorPositionBegin = NormalizeCursorPosition(fromPosition);
			m_cursorPositionEnd = NormalizeCursorPosition(toPosition);

			RefreshCursor();
		}
	}

	inline void TextAreaWidget::SetText(const Nz::String& text)
	{
		m_text = text;
		OnTextChanged(this, m_text);

		UpdateDisplayText();
	}

	inline void TextAreaWidget::SetTextColor(const Nz::Color& text)
	{
		m_drawer.SetColor(text);

		UpdateDisplayText();
	}

	inline void TextAreaWidget::SetTextFont(Nz::FontRef font)
	{
		m_drawer.SetFont(font);

		UpdateDisplayText();
	}

	inline void TextAreaWidget::SetTextOutlineColor(const Nz::Color& color)
	{
		m_drawer.SetOutlineColor(color);

		UpdateDisplayText();
	}

	inline void TextAreaWidget::SetTextOutlineThickness(float thickness)
	{
		m_drawer.SetOutlineThickness(thickness);

		UpdateDisplayText();
	}

	inline void TextAreaWidget::Write(const Nz::String& text)
	{
		Write(text, GetGlyphIndex(m_cursorPositionBegin));
	}

	inline void TextAreaWidget::Write(const Nz::String& text, const Nz::Vector2ui& glyphPosition)
	{
		Write(text, GetGlyphIndex(glyphPosition));
	}

	void TextAreaWidget::SetCursorPositionInternal(std::size_t glyphIndex)
	{
		return SetCursorPositionInternal(GetCursorPosition(glyphIndex));
	}

	inline void TextAreaWidget::SetCursorPositionInternal(Nz::Vector2ui cursorPosition)
	{
		m_cursorPositionBegin = cursorPosition;
		m_cursorPositionEnd = m_cursorPositionBegin;

		RefreshCursor();
	}
}
