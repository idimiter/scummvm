/* ScummVM - Scumm Interpreter
 * Copyright (C) 2005-2006 The ScummVM project
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

#include "common/stdafx.h"
#include "common/fs.h"
#include "common/str.h"

#include "kom/kom.h"
#include "kom/game.h"
#include "kom/panel.h"
#include "kom/database.h"
#include "kom/flicplayer.h"

namespace Kom {

using Common::String;

Game::Game(KomEngine *vm, OSystem *system) : _system(system), _vm(vm) {
}

Game::~Game() {
}

void Game::enterLocation(uint16 locId) {
	char filename[50];

	_vm->panel()->showLoading(true);

	Location *loc = _vm->database()->location(locId);
	String locName(loc->name);
	locName.toLowercase();
	FilesystemNode locNode(_vm->dataDir()->getChild("kom").getChild("locs").getChild(String(locName.c_str(), 2)).getChild(locName));

	// Load room background and mask

	// TODO: add day/night modifier to xtend
	sprintf(filename, "%s%db.flc", locName.c_str(), loc->xtend);
	_vm->screen()->loadBackground(locNode.getChild(filename));

	filename[strlen(filename) - 5] = 'm';
	FlicPlayer mask(locNode.getChild(filename));
	mask.decodeFrame();
	_vm->screen()->setMask(mask.getOffscreen());

	// Load room objects
	Common::List<int> objList = loc->objects;
	Database *db = _vm->database();
	for (Common::List<int>::iterator objId = objList.begin(); objId != objList.end(); ++objId) {
		Object *obj = db->object(*objId);
		if (obj->isSprite) {
			sprintf(filename, "%s%d", obj->name, loc->xtend);
			int actId = _vm->actorMan()->load(locNode, String(filename));
			Actor *act = _vm->actorMan()->get(actId);
			act->defineScope(0, 0, act->getFramesNum() - 1, 0);
			act->setScope(0, 3);
			act->setPos(0, SCREEN_H - 1);

			// TODO:
			// * store actor in screenObjects
			// * load doors
		}
	}

	_vm->panel()->setLocationDesc(loc->desc);
	_vm->panel()->showLoading(false);
}

} // End of namespace Kom
