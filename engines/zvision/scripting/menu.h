/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef ZVISION_MENU_H
#define ZVISION_MENU_H

#include "graphics/surface.h"
#include "common/rect.h"

#include "zvision/zvision.h"
#include "zvision/scripting/script_manager.h"

namespace ZVision {

enum menuBar {
	kMenubarExit = 0x1,
	kMenubarSettings = 0x2,
	kMenubarRestore = 0x4,
	kMenubarSave = 0x8,
	kMenubarItems = 0x100,
	kMenubarMagic = 0x200
};

class MenuHandler {
public:
	MenuHandler(ZVision *engine);
	virtual ~MenuHandler() {};
	virtual void onMouseMove(const Common::Point &Pos) {};
	virtual void onMouseDown(const Common::Point &Pos) {};
	virtual void onMouseUp(const Common::Point &Pos) {};
	virtual void process(uint32 deltaTimeInMillis) {};

	void setEnable(uint16 flags) {
		menuBarFlag = flags;
	}
	uint16 getEnable() {
		return menuBarFlag;
	}
protected:
	uint16 menuBarFlag;
	ZVision *_engine;
};

class MenuZGI: public MenuHandler {
public:
	MenuZGI(ZVision *engine);
	~MenuZGI();
	void onMouseMove(const Common::Point &Pos);
	void onMouseUp(const Common::Point &Pos);
	void process(uint32 deltaTimeInMillis);
private:
	Graphics::Surface menuback[3][2];
	Graphics::Surface menubar[4][2];
	Graphics::Surface *items[50][2];
	uint itemId[50];

	Graphics::Surface *magic[12][2];
	uint magicId[12];

	int menuMouseFocus;
	bool inmenu;

	int mouseOnItem;

	bool   scrolled[3];
	int16 scrollPos[3];

	bool clean;
	bool redraw;

};

class MenuNemesis: public MenuHandler {
public:
	MenuNemesis(ZVision *engine);
	~MenuNemesis();
	void onMouseMove(const Common::Point &Pos);
	void onMouseUp(const Common::Point &Pos);
	void process(uint32 deltaTimeInMillis);
private:
	Graphics::Surface but[4][6];
	Graphics::Surface menubar;

	bool inmenu;

	int mouseOnItem;

	bool   scrolled;
	int16 scrollPos;

	bool redraw;

	int frm;
	int16 delay;

};

}

#endif
