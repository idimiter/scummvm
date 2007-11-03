/* ScummVM - Scumm Interpreter
 * Copyright (C) 2004-2006 The ScummVM project
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL$
 * $Id$
 *
 */

#include <memory.h>
#include <stdlib.h>

#include "common/system.h"
#include "common/file.h"
#include "common/fs.h"
#include "common/endian.h"
#include "common/list.h"
#include "common/rect.h"
#include "graphics/cursorman.h"

#include "kom/screen.h"
#include "kom/kom.h"
#include "kom/panel.h"
#include "kom/actor.h"

using Common::File;
using Common::List;
using Common::Rect;

namespace Kom {

Screen::Screen(KomEngine *vm, OSystem *system)
	: _system(system), _vm(vm), _roomBackground(0) {

	_lastFrameTime = 0;

	_screenBuf = new uint8[SCREEN_W * SCREEN_H];
	memset(_screenBuf, 0, SCREEN_W * SCREEN_H);

	_mouseBuf = new uint8[MOUSE_W * MOUSE_H];
	memset(_mouseBuf, 0, MOUSE_W * MOUSE_H);

	_c0ColorSet = loadColorSet(_vm->dataDir()->getChild("kom").getChild("oneoffs").getChild("c0_127.cl"));

	_mask = new uint8[SCREEN_W * (SCREEN_H - PANEL_H)];
	memset(_mask, 0, SCREEN_W * (SCREEN_H - PANEL_H));

	_font = new Font(_vm->dataDir()->getChild("kom").getChild("oneoffs").getChild("packfont.fnt"));

	_dirtyRects = new List<Rect>();
	_prevDirtyRects = new List<Rect>();
}

Screen::~Screen() {
	delete[] _screenBuf;
	delete[] _mouseBuf;
	delete[] _c0ColorSet;
	delete[] _mask;
	delete _roomBackground;
	delete _font;
	delete _dirtyRects;
	delete _prevDirtyRects;
}

bool Screen::init() {
	_system->beginGFXTransaction();
		_vm->initCommonGFX(false);
		_system->initSize(SCREEN_W, SCREEN_H);
	_system->endGFXTransaction();

	_system->setPalette(_c0ColorSet, 0, 128);

	return true;
}

void Screen::processGraphics() {
	//memset(_screenBuf, 0, SCREEN_W * SCREEN_H);

	// handle screen objects
	// handle dust clouds
	
	// unload actors in other rooms
	for (int i = 1; i < _vm->database()->charactersNum(); ++i) {
		if (_vm->database()->getCharScope(0)->lastLocation !=
		    _vm->database()->getCharScope(i)->lastLocation ||
			!_vm->database()->getChar(i)->isVisible) {

			// TODO - init scope stuff
			if (_vm->database()->getCharScope(i)->actorId >= 0) {
				_vm->actorMan()->unload(_vm->database()->getCharScope(i)->actorId);
				_vm->database()->getCharScope(i)->actorId = -1;
			}
		}
	}

	// load actors in this room
	for (int i = 1; i < _vm->database()->charactersNum(); ++i) {
		// TODO - init scope stuff
		// TODO - disable actor if in fight
		if (_vm->database()->getCharScope(0)->lastLocation ==
		    _vm->database()->getCharScope(i)->lastLocation &&
			_vm->database()->getChar(i)->isVisible) {

			_vm->game()->setScope(i, _vm->database()->getCharScope(i)->scopeInUse);
		}
	}

	updateCursor();
	displayMouse();
	updateBackground();
	drawBackground();
	_vm->actorMan()->displayAll();

	// Copy dirty rects to screen
	copyRectListToScreen(_prevDirtyRects);
	copyRectListToScreen(_dirtyRects);
	delete _prevDirtyRects;
	_prevDirtyRects = _dirtyRects;
	_dirtyRects = new List<Rect>();

	// No need to save a prev, since the background reports
	// all changes
	if (_roomBackground)
		copyRectListToScreen(_bgDirtyRects);

	if (_vm->panel()->isDirty())
		_vm->panel()->update();

	gfxUpdate();
}

void Screen::copyRectListToScreen(const Common::List<Common::Rect> *list) {

	for (Common::List<Rect>::const_iterator rect = list->begin(); rect != list->end(); ++rect) {
		debug(9, "copyRectToScreen(%hu, %hu, %hu, %hu)", rect->left, rect->top,
				rect->width(), rect->height());
		_system->copyRectToScreen(_screenBuf + SCREEN_W * rect->top + rect->left,
				SCREEN_W, rect->left, rect->top, rect->width(), rect->height());
	}
}

void Screen::gfxUpdate() {
	while (_system->getMillis() < _lastFrameTime + 41 /* 24 fps */) {
		_vm->input()->checkKeys();
		_system->delayMillis(10);
	}
	_system->updateScreen();
	_lastFrameTime = _system->getMillis();
}

void Screen::drawActorFrame(const int8 *data, uint16 width, uint16 height, int16 xStart, int16 yStart,
                            uint16 xEnd, uint16 yEnd, int maskDepth) {

	uint16 startCol = (xStart < 0 ? -xStart : 0);
	uint16 startLine = (yStart < 0 ? -yStart : 0);
	uint16 endCol = (xStart + width > SCREEN_W ? SCREEN_W - xStart : width);
	uint16 endLine = (yStart + height - 1 > SCREEN_H ? SCREEN_H - yStart - 1: height - 2);

	int16 pixelWidth = xEnd - xStart, fullPixelWidth = pixelWidth;
	int16 pixelHeight = yEnd - yStart, fullPixelHeight = pixelHeight;
	int32 help1, help2, help3;
	const int8 *dataPtr = data;
	div_t d;

	if (xStart < 0) {
		// frame is entirely off-screen
		if ((pixelWidth += xStart) < 0)
			return;

		d = div(-xStart * width, xEnd - xStart);
		help1 = d.quot;
		help2 = d.rem;

		xStart = 0;
	}

	// frame is entirely off-screen
	if (xStart >= SCREEN_W) return;

	// check if frame spills over the edge
	if (pixelWidth + xStart >= SCREEN_W)
		if ((pixelWidth -= pixelWidth + xStart - SCREEN_W) <= 0)
			return;

	if (yStart < 0) {
		// frame is entirely off-screen
		if ((pixelHeight += yStart) < 0)
			return;

		d = div(-yStart * height, yEnd - yStart);
		dataPtr += d.quot * 2;
		help3 = d.rem;

		yStart = 0;
	}

	// frame is entirely off-screen
	if (yStart >= SCREEN_H) return;

	// check if frame spills over the edge
	if (pixelHeight + yStart >= SCREEN_H)
		if ((pixelHeight -= pixelHeight + yStart - SCREEN_H + 1) <= 0)
			return;

	// TODO

	for (int line = startLine; line < endLine; ++line) {
		uint16 lineOffset = READ_LE_UINT16(data + line * 2);

		drawActorFrameLine(_screenBuf, SCREEN_W, data + lineOffset, (xStart < 0 ? 0 : xStart),
		                   (yStart < 0 ? 0 : yStart) + line - startLine, xStart, xEnd, maskDepth);
	}
	_dirtyRects->push_back(Rect(xStart, yStart, xEnd, yEnd));
}

/*
void Screen::drawActorFrame(const int8 *data, uint16 width, uint16 height, uint16 xPos, uint16 yPos,
                            int16 xOffset, int16 yOffset, int maskDepth, uint16 xRatio, uint16 yRatio) {

	// Check which lines and columns to draw
	int16 realX = xPos + xOffset;
	int16 realY = yPos + yOffset;

	uint16 startCol = (realX < 0 ? -realX : 0);
	uint16 startLine = (realY < 0 ? -realY : 0);
	uint16 endCol = (realX + width > SCREEN_W ? SCREEN_W - realX : width);
	uint16 endLine = (realY + height - 1 > SCREEN_H ? SCREEN_H - realY - 1: height - 2);

	for (int line = startLine; line < endLine; ++line) {
		uint16 lineOffset = READ_LE_UINT16(data + line * 2);

		drawActorFrameLine(_screenBuf, SCREEN_W, data + lineOffset, (realX < 0 ? 0 : realX),
		                   (realY < 0 ? 0 : realY) + line - startLine, startCol, endCol, maskDepth);
	}
	_dirtyRects->push_back(Rect(realX + startCol, realY + startLine, realX + endCol, realY + endLine));
}
*/

void Screen::drawMouseFrame(const int8 *data, uint16 width, uint16 height, int16 xOffset, int16 yOffset) {

	memset(_mouseBuf, 0, MOUSE_W * MOUSE_H);

	for (int line = 0; line <= height - 1; ++line) {
		uint16 lineOffset = READ_LE_UINT16(data + line * 2);

		drawActorFrameLine(_mouseBuf, MOUSE_W, data + lineOffset, 0, line, 0, width, 0);
	}

	setMouseCursor(_mouseBuf, MOUSE_W, MOUSE_H, -xOffset, -yOffset);
}

void Screen::drawActorFrameLine(uint8 *buf, uint16 bufWidth, const int8 *data,
                                uint16 xPos, uint16 yPos, uint16 startPixel, uint16 endPixel,
                                int maskDepth) {
	uint16 dataIndex = 0;
	uint16 pixelsDrawn = 0;
	uint16 pixelsParsed = 0;
	int8 imageData;
	bool shouldDraw = false;

	if (startPixel == 0)
		shouldDraw = true;

	while (pixelsDrawn < endPixel - startPixel) {

		imageData = data[dataIndex];
		dataIndex++;

		// Handle transparent pixels
		if (imageData > 0) {

			// Skip pixels
			if (!shouldDraw && pixelsParsed < startPixel && pixelsParsed + imageData >= startPixel) {
				uint8 pixelsToSkip = startPixel - pixelsParsed;

				pixelsParsed += imageData;
				imageData -= pixelsToSkip;
				shouldDraw = true;

			// Not at start pos yet
			} else {
				pixelsParsed += imageData;
			}

			if (shouldDraw) {
				pixelsDrawn += imageData;
			}

		// Handle visible pixels
		} else {

			imageData &= 0x7F;

			// Skip pixels
			if (!shouldDraw && pixelsParsed < startPixel && pixelsParsed + imageData >= startPixel) {
				uint8 pixelsToSkip = startPixel - pixelsParsed;

				pixelsParsed += imageData;
				imageData -= pixelsToSkip;
				dataIndex += pixelsToSkip;

				shouldDraw = true;

			// Not at start pos yet
			} else {
				pixelsParsed += imageData;
			}

			if (shouldDraw) {
				// Don't draw over the edge!
				if (pixelsDrawn + imageData > endPixel)
					imageData = endPixel - pixelsDrawn;

				for (int i = 0; i < imageData; ++i) {
					// Check with background mask
					if (yPos > SCREEN_H - PANEL_H ||
					    _mask[(yPos * bufWidth) + xPos + pixelsDrawn + i] >= maskDepth)

						// FIXME: valgrind sometimes reports invalid write of 1 byte
						buf[(yPos * bufWidth) + xPos + pixelsDrawn + i] =
							data[dataIndex + i];
				}

				dataIndex += imageData;
				pixelsDrawn += imageData;
			} else {
				dataIndex += imageData;
			}
		}
	}
}

byte *Screen::loadColorSet(FilesystemNode fsNode) {
	File f;

	f.open(fsNode);

	byte *pal = new byte[128 * 4];

	for (int i = 0; i < 128; ++i) {
		pal[4 * i + 0] = f.readByte();
		pal[4 * i + 1] = f.readByte();
		pal[4 * i + 2] = f.readByte();
		pal[4 * i + 3] = 0;
	}

	f.close();
	return pal;
}

void Screen::setMouseCursor(const byte *buf, uint w, uint h, int hotspotX, int hotspotY) {
	CursorMan.replaceCursor(buf, w, h, hotspotX, hotspotY, 0);
}

void Screen::showMouseCursor(bool show) {
	CursorMan.showMouse(show);
}

void Screen::displayMouse() {
	_vm->actorMan()->getMouse()->display();
}

void Screen::updateCursor() {
	Actor *mouse = _vm->actorMan()->getMouse();

	if (_vm->input()->getMouseY() >= SCREEN_H - PANEL_H) {
		mouse->switchScope(5, 2);
	} else {
		mouse->switchScope(0, 2);
	}

	// The scopes:
	// setScope(0, 2);
	// setScope(1, 2);
	// setScope(2, 2);
	// setScope(3, 2);
	// setScope(4, 2);
	// setScope(5, 2);
	// setScope(6, 0);
}

void Screen::drawPanel(const byte *panelData) {
	memcpy(_screenBuf + SCREEN_W * (SCREEN_H - PANEL_H), panelData, SCREEN_W * PANEL_H);
}

void Screen::refreshPanelArea() {
	_system->copyRectToScreen(_screenBuf + SCREEN_W * (SCREEN_H - PANEL_H),
		SCREEN_W, 0, SCREEN_H - PANEL_H, SCREEN_W, PANEL_H);
	_system->updateScreen();
}

void Screen::copyPanelToScreen(const byte *data) {
	memcpy(_screenBuf + SCREEN_W * (SCREEN_H - PANEL_H), data, SCREEN_W * PANEL_H);
}

void Screen::loadBackground(FilesystemNode node) {
	delete _roomBackground;
	_roomBackground = new FlicPlayer(node);
	_roomBackgroundTime = 0;

	// Redraw everything
	_dirtyRects->clear();
	_prevDirtyRects->clear();
	// No need to report the rect, since the flic player will report it
}

void Screen::updateBackground() {
	if (_roomBackground != 0) {
		if (_system->getMillis() >= _roomBackgroundTime) {
			_roomBackgroundTime = _system->getMillis() + _roomBackground->speed();

			_roomBackground->decodeFrame();

			if (_roomBackground->paletteDirty()) {
				_system->setPalette(_roomBackground->getPalette(), 0, 1);
				_system->setPalette(_roomBackground->getPalette() + 4 * 128, 128, 128);
			}
		}
	}
}

void Screen::drawBackground() {
	if (_roomBackground != 0) {
		_bgDirtyRects = _roomBackground->getDirtyRects();
		memcpy(_screenBuf, _roomBackground->getOffscreen(), SCREEN_W * (SCREEN_H - PANEL_H));
	}
}

void Screen::setMask(const uint8 *data) {
	memcpy(_mask, data, SCREEN_W * (SCREEN_H - PANEL_H));
}

uint16 Screen::getTextWidth(const char *text) {
	uint16 w = 0;
	for (int i = 0; text[i] != '\0'; ++i) {
		switch (text[i]) {
		case ' ':
			w += 4;
			break;
		// This check isn't in the original engine
		/*case '\t':
			w += 12;
			break;*/
		default:
			w += *((const uint8 *)(_font->getCharData(text[i])));
		}
		++w;
	}
	if (w > 0) --w;

	return w;
}

void Screen::writeTextCentered(byte *buf, const char *text, uint8 row, uint8 color, bool isEmbossed) {
	uint8 col = (SCREEN_W - getTextWidth(text)) / 2;
	writeText(buf, text, row, col, color, isEmbossed);
}

void Screen::writeText(byte *buf, const char *text, uint8 row, uint16 col, uint8 color, bool isEmbossed) {
	if (isEmbossed)
		writeTextStyle(buf, text, row, col, 0, true);

	writeTextStyle(buf, text, row, col, color, false);
}
void Screen::writeTextStyle(byte *buf, const char *text, uint8 startRow, uint16 startCol, uint8 color, bool isBackground) {
	uint16 col = startCol;
	uint8 charWidth;
	const byte *data;

	for (int i = 0; text[i] != '\0'; ++i) {
		switch (text[i]) {
		case ' ':
			col += 4;
			break;
		case '\t':
			col += 12;
			break;
		default:
			data = _font->getCharData(text[i]);
			charWidth = (uint8)*data;
			++data;

			for (uint w = 0; w < charWidth; ++w) {
				for (uint8 h = 0; h < 8; ++h) {
					if (*data != 0)
						if (isBackground) {
							buf[SCREEN_W * (startRow + h) + col + w - 1] = 3;
							buf[SCREEN_W * (startRow + h - 1) + col + w] = 3;
							buf[SCREEN_W * (startRow + h - 1) + col + w - 1] = 3;
							buf[SCREEN_W * (startRow + h) + col + w + 1] = 53;
							buf[SCREEN_W * (startRow + h + 1) + col + w] = 53;
							buf[SCREEN_W * (startRow + h + 1) + col + w + 1] = 53;
						} else {
							buf[SCREEN_W * (startRow + h) + col + w] = color;
						}
					++data;
				}
			}
			col += charWidth;
		}

		col += 1;
	}
}

} // End of namespace Kom
