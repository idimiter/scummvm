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

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL$
 * $Id$
 *
 */

#include <stdlib.h>
#include <string.h>

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

	// FIXME: Temporary
    _player.selectedChar = _player.selectedQuest = 0;
}

Game::~Game() {
}

void Game::enterLocation(uint16 locId) {
	char filename[50];

	_vm->panel()->showLoading(true);

	// Unload last room elements
	for (uint i = 0; i < _roomObjects.size(); i++) {
		_vm->actorMan()->unload(_roomObjects[i].actorId);
	}
	_roomObjects.clear();

	for (uint i = 0; i < _roomDoors.size(); i++) {
		_vm->actorMan()->unload(_roomDoors[i].actorId);
	}
	_roomDoors.clear();

	if (locId == 0) {
		_vm->panel()->showLoading(false);
		return;
	}

	Location *loc = _vm->database()->getLoc(locId);
	String locName(loc->name);
	locName.toLowercase();
	FilesystemNode locNode(_vm->dataDir()->getChild("kom").getChild("locs").getChild(String(locName.c_str(), 2)).getChild(locName));

	// Load room background and mask

	sprintf(filename, "%s%db.flc", locName.c_str(), loc->xtend + _player.isNight);
	_vm->screen()->loadBackground(locNode.getChild(filename));

	// TODO - init some other flic var
	_vm->_flicLoaded = 2;

	filename[strlen(filename) - 6] = '0';
	filename[strlen(filename) - 5] = 'm';
	FlicPlayer mask(locNode.getChild(filename));
	mask.decodeFrame();
	_vm->screen()->setMask(mask.getOffscreen());

	Database *db = _vm->database();

	// Load room objects
	Common::List<int> objList = loc->objects;
	for (Common::List<int>::iterator objId = objList.begin(); objId != objList.end(); ++objId) {
		Object *obj = db->object(*objId);
		RoomObject roomObj;
		roomObj.actorId = -1;
		roomObj.objectId = *objId;

		if (obj->isSprite) {
			sprintf(filename, "%s%d", obj->name, _player.isNight);
			roomObj.actorId = _vm->actorMan()->load(locNode, String(filename));
			roomObj.priority = db->getBox(locId, obj->box)->priority;
			Actor *act = _vm->actorMan()->get(roomObj.actorId);
			act->defineScope(0, 0, act->getFramesNum() - 1, 0);
			act->setScope(0, 3);
			act->setPos(0, SCREEN_H - 1);

			// TODO - move this to processGraphics?
			act->setMaskDepth(roomObj.priority, 32767);
		}

		_roomObjects.push_back(roomObj);
	}

	// Load room doors
	const Exit *exits = db->getExits(locId);
	for (int i = 0; i < 6; ++i) {
		// FIXME: room 45 has one NULL exit. what's it for?
		if (exits[i].exit > 0) {
			String exitName(db->getLoc(exits[i].exitLoc)->name);
			exitName.toLowercase();

			sprintf(filename, "%s%dd", exitName.c_str(), loc->xtend + _player.isNight);

			// The exit can have no door
			if (!(locNode.getChild(filename + String(".act")).exists()))
				continue;

			RoomDoor roomDoor;
			roomDoor.actorId = _vm->actorMan()->load(locNode, String(filename));
			Actor *act = _vm->actorMan()->get(roomDoor.actorId);
			act->enable(true);
			act->setEffect(4);
			act->setPos(0, SCREEN_H - 1);
			act->setMaskDepth(0, 32766);

			// Find door-opening boxes (two of them)
			for (int j = 0; j < 6; ++j) {
				roomDoor.boxOpenFast = db->getBoxLink(locId, exits[i].exit, j);
				if (db->getBox(locId, roomDoor.boxOpenFast)->attrib == 0)
					break;
			}

			for (int j = 0; j < 6; ++j) {
				roomDoor.boxOpenSlow = db->getBoxLink(locId, roomDoor.boxOpenFast, j);
				if (roomDoor.boxOpenSlow != exits[i].exit &&
					db->getBox(locId, roomDoor.boxOpenSlow)->attrib == 0)
					break;
			}

			_roomDoors.push_back(roomDoor);
		}
	}

	_vm->panel()->setLocationDesc(loc->desc);
	_vm->panel()->setActionDesc("");
	_vm->panel()->showLoading(false);
}

void Game::hitExit(uint16 charId, bool something) {
	int exitLoc, exitBox;
	Character *chr = _vm->database()->getChar(charId);

	_vm->database()->getExitInfo(chr->_lastLocation, chr->_lastBox,
			&exitLoc, &exitBox);

	if (charId == 0) {
		enterLocation(exitLoc);
	} else if (something) {
		// TODO? used for cheat codes?
		// housingProblem(charId);
	}

	chr->_gotoBox = -1;
	chr->_lastLocation = exitLoc;
	chr->_lastBox = exitBox;

	for (int i = 0; i < 6; ++i) {
		int8 linkBox = _vm->database()->getBoxLink(exitLoc, exitBox, i);

		if (linkBox == -1 || (_vm->database()->getBox(exitLoc, linkBox)->attrib & 0xe) != 0)
			continue;

		chr->_screenX = _vm->database()->getMidX(exitLoc, linkBox);
		chr->_screenY = _vm->database()->getMidY(exitLoc, linkBox);

		if (chr->_spriteCutState == 0) {
			chr->_gotoX = chr->_screenX;
			chr->_gotoY = chr->_screenY;
			chr->_gotoLoc = exitLoc;
		} else if (chr->_lastLocation == chr->_gotoLoc) {
			chr->_gotoX = _vm->database()->getMidX(chr->_lastLocation, chr->_spriteBox);
			chr->_gotoY = _vm->database()->getMidY(chr->_lastLocation, chr->_spriteBox);
		}

		chr->_start3 = chr->_screenX * 256;
		chr->_start4 = chr->_screenY * 256;
		chr->_start3PrevPrev = chr->_start3Prev;
		chr->_start3Prev = chr->_start3;
		chr->_start4PrevPrev = chr->_start4Prev;
		chr->_start4Prev = chr->_start4;
		chr->_start5 = 65280;
		chr->_start5Prev = 65536;
		chr->_start5PrevPrev = 66048;
		chr->_lastDirection = 4;
	}
}

void Game::processTime() {
	if (_settings.dayMode == 0) {
		if (_vm->database()->getChar(0)->_isBusy && _settings.gameCycles >= 6000)
			_settings.gameCycles = 5990;

		if (_vm->database()->getLoc(_settings.currLocation)->allowedTime == 2) {
			_settings.dayMode = 3;
			doActionDusk();
			processChars();
			_settings.dayMode = 1;
			_settings.gameCycles = 0;
		}

		if (_settings.gameCycles < 6000) {

			if (!_vm->database()->getChar(0)->_isBusy)
				(_settings.gameCycles)++;

		} else if (_vm->database()->getLoc(_settings.currLocation)->allowedTime == 0) {
			_settings.dayMode = 3;
			doActionDusk();
			processChars();
			_settings.dayMode = 1;
			_settings.gameCycles = 0;
		}

	} else if (_settings.dayMode == 1) {

		if (_vm->database()->getChar(0)->_isBusy && _settings.gameCycles >= 3600)
			_settings.gameCycles = 3590;

		if (_vm->database()->getLoc(_settings.currLocation)->allowedTime == 1) {
			_settings.dayMode = 2;
			doActionDawn();
			processChars();
			_settings.dayMode = 0;
			_settings.gameCycles = 0;
		}

		if (_settings.gameCycles < 3600) {

			if (!_vm->database()->getChar(0)->_isBusy)
				(_settings.gameCycles)++;

		} else if (_vm->database()->getLoc(_settings.currLocation)->allowedTime == 0) {
			_settings.dayMode = 2;
			doActionDawn();
			processChars();
			_settings.dayMode = 0;
			_settings.gameCycles = 0;

			// TODO - increase hit points and spell points
		}
	}

	processChars();
}

void Game::processChars() {
	for (int i = 0; i < _vm->database()->charactersNum(); ++i) {
		Character *ch = _vm->database()->getChar(i);
		if (ch->_isAlive && ch->_proc != -1 && ch->_mode < 6) {
			switch (ch->_mode) {
			case 0:
			case 3:
				processChar(ch->_proc);
				break;
			case 1:
				if (ch->_modeCount ==  0)
					ch->_mode = 0;
				else
					ch->_modeCount--;

				if (ch->_spellMode != 0) {
					if (ch->_spellDuration == 0) {
						ch->unsetSpell();
					}

					if (ch->_spellDuration > 0)
						ch->_spellDuration--;
				}
				break;
			case 2:
				warning("TODO: processChars 2");
				break;
			case 4:
			case 5:
				warning("TODO: processChars 4/5");
				break;
			}
		}
	}
}

void Game::processChar(int proc) {
	bool stop = false;
	Process *p = _vm->database()->getProc(proc);

	for (Common::List<Command>::iterator i = p->commands.begin();
			i != p->commands.end() && !stop; ++i) {
		if (i->cmd == 313) { // Character
			debug(1, "Processing char in %s", p->name);
			stop = doStat(&(*i));
		}
	}
}

bool Game::doProc(int id, int type, int cmd, int value) {
	int proc;
	Process *p;
	bool stop;

	switch (type) {
	case 1: // Object
		proc = _vm->database()->getObj(id)->proc;
		break;
	case 2: // Char
		proc = _vm->database()->getChar(id)->_proc;
		break;
	case 3: // Specific proc
		proc = id;
		break;
	default:
		return false;
	}

	p = _vm->database()->getProc(proc);

	for (Common::List<Command>::iterator i = p->commands.begin();
			i != p->commands.end() && !stop; ++i) {
		if (i->cmd == cmd) {
			switch (cmd) {
			case 318:
				if(doStat(&(*i)))
					return true;
				break;
			default:
				warning("Unhandled proc type: %d\n", type);
				return true;
			}
		}
	}

	return false;
}

bool Game::doStat(const Command *cmd) {
	bool keepProcessing = true;
	bool rc = false;
	Database *db = _vm->database();

	debug(1, "Trying to execute Command %d - value %hd", cmd->cmd, cmd->value);

	for (Common::List<OpCode>::const_iterator j = cmd->opcodes.begin();
			j != cmd->opcodes.end() && keepProcessing; ++j) {

		switch (j->opcode) {
		case 327:
			db->setVar(j->arg2, j->arg3);
			break;
		case 328:
			db->setVar(j->arg2, db->getVar(j->arg3));
			break;
		case 331:
			db->setVar(j->arg2, 0);
			break;
		case 334:
			db->setVar(j->arg2, _rnd.getRandomNumber(j->arg3 - 1));
			break;
		case 337:
			keepProcessing = db->getVar(j->arg2) == 0;
			break;
		case 338:
			keepProcessing = db->getVar(j->arg2) != 0;
			break;
		case 340:
			keepProcessing = db->getVar(j->arg2) == j->arg3;
			break;
		case 345:
			keepProcessing = db->getVar(j->arg2) != j->arg3;
			break;
		case 346:
			keepProcessing = db->getVar(j->arg2) !=
				db->getVar(j->arg3);
			break;
		case 350:
			keepProcessing = db->getVar(j->arg2) > j->arg3;
			break;
		case 353:
			keepProcessing = db->getVar(j->arg2) >= j->arg3;
			break;
		case 356:
			keepProcessing = db->getVar(j->arg2) < j->arg3;
			break;
		case 359:
			keepProcessing = db->getVar(j->arg2) <= j->arg3;
			break;
		case 373:
			db->setVar(j->arg2, db->getVar(j->arg2) + 1);
			break;
		case 374:
			db->setVar(j->arg2, db->getVar(j->arg2) - 1);
			break;
		case 376:
			db->setVar(j->arg2,
				db->getVar(j->arg2) + db->getVar(j->arg3));
			break;
		case 377:
			db->setVar(j->arg2, db->getVar(j->arg2) + j->arg3);
			break;
		case 379:
			db->setVar(j->arg2,
				db->getVar(j->arg2) - db->getVar(j->arg3));
			break;
		case 380:
			db->setVar(j->arg2, db->getVar(j->arg2) - j->arg3);
			break;
		case 381:
			keepProcessing = db->getChar(0)->_locationId == j->arg2;
			break;
		case 382:
			keepProcessing = db->getChar(0)->_locationId != j->arg2;
			break;
		case 383:
			keepProcessing = false;
			if (db->getObj(j->arg2)->ownerType == 3)
				keepProcessing = db->getObj(j->arg2)->ownerId == 0;
			break;
		case 384:
			keepProcessing = true;
			if (db->getObj(j->arg2)->ownerType == 3)
				keepProcessing = db->getObj(j->arg2)->ownerId != 0;
			break;
		case 387:
			keepProcessing = db->giveObject(j->arg2, 0, false);
			break;
		case 391:
			keepProcessing = db->getChar(0)->_gold != 0;
			break;
		case 392:
			keepProcessing = db->getChar(0)->_gold == 0;
			break;
		case 393:
			if (db->getChar(0)->_gold >= j->arg3) {
				db->getChar(0)->_gold -= j->arg3;
				db->getChar(j->arg2)->_gold += j->arg3;
			}
			break;
		case 394:
			keepProcessing = false;
			if (j->arg4 == -1)
				keepProcessing = db->getChar(j->arg2)->_locationId == j->arg3;
			else if (db->getChar(j->arg2)->_locationId == j->arg3)
				keepProcessing = db->getChar(j->arg2)->_box == j->arg4;
			break;
		case 395:
			keepProcessing = true;
			if (j->arg4 == -1)
				keepProcessing = db->getChar(j->arg2)->_locationId != j->arg3;
			else if (db->getChar(j->arg2)->_locationId == j->arg3)
				keepProcessing = db->getChar(j->arg2)->_box != j->arg4;
			break;
		case 398:
			keepProcessing =
				db->getObj(j->arg3)->ownerType == 3 &&
				db->getObj(j->arg3)->ownerId == j->arg2;
			break;
		case 399:
			keepProcessing = true;
			if (db->getObj(j->arg3)->ownerType == 3)
				keepProcessing = db->getObj(j->arg3)->ownerId != j->arg2;
			break;
		case 403:
			db->setCharPos(j->arg2, j->arg3, j->arg4);
			doActionMoveChar(j->arg2, j->arg3, j->arg4);
			db->getChar(j->arg2)->_destLoc =
			db->getChar(j->arg2)->_destBox = -4;
			break;
		case 402:
			db->getChar(j->arg2)->_destLoc = j->arg3;
			db->getChar(j->arg2)->_destBox = j->arg4;
			break;
		case 405:
			db->getChar(j->arg2)->_destLoc =
			db->getChar(j->arg2)->_destBox = -2;
			break;
		case 406:
			db->getChar(j->arg2)->_destLoc =
			db->getChar(j->arg2)->_destBox = -3;
			break;
		case 407:
			db->getChar(j->arg2)->_destLoc =
			db->getChar(j->arg2)->_destBox = -4;
			break;
		case 408:
			db->getChar(j->arg2)->_destLoc =
			db->getChar(j->arg2)->_destBox = -5;
			break;
		case 409:
			keepProcessing = db->getChar(j->arg2)->_isBusy;
			break;
		case 410:
			keepProcessing = !(db->getChar(j->arg2)->_isBusy);
			break;
		case 411:
			keepProcessing = db->getChar(j->arg2)->_isAlive;
			break;
		case 412:
			keepProcessing = !(db->getChar(j->arg2)->_isAlive);
			break;
		case 414:
			db->getChar(j->arg2)->unsetSpell();
			db->getChar(j->arg2)->_isAlive = false;
			break;
		case 416:
			db->getChar(j->arg2)->_hitPoints =
				(db->getVar(j->arg3) ? db->getVar(j->arg3) : 0);
			if (db->getChar(j->arg2)->_hitPoints > db->getChar(j->arg2)->_hitPointsMax)
				db->getChar(j->arg2)->_hitPoints = db->getChar(j->arg2)->_hitPointsMax;
			break;
		case 417:
			db->getChar(j->arg2)->_gold =
				db->getVar(j->arg3);
			break;
		case 418:
			db->getChar(j->arg2)->_spellPoints =
				(db->getVar(j->arg3) ? db->getVar(j->arg3) : 0);
			if (db->getChar(j->arg2)->_spellPoints > db->getChar(j->arg2)->_spellPointsMax)
				db->getChar(j->arg2)->_spellPoints = db->getChar(j->arg2)->_spellPointsMax;
			break;
		case 422:
			db->setVar(j->arg2, db->getChar(j->arg3)->_hitPoints);
			break;
		case 423:
			db->setVar(j->arg2, db->getChar(j->arg3)->_spellMode);
			break;
		case 424:
			db->setVar(j->arg2, db->getChar(j->arg3)->_gold);
			break;
		case 425:
			db->setVar(j->arg2, db->getChar(j->arg3)->_spellPoints);
			break;
		case 426:
			keepProcessing = db->getChar(j->arg2)->_locationId ==
			    db->getChar(j->arg3)->_locationId;
			break;
		case 427:
			keepProcessing = db->getChar(j->arg2)->_locationId !=
			    db->getChar(j->arg3)->_locationId;
			break;
		case 428:
			keepProcessing = db->getChar(j->arg2)->_box ==
			    db->getChar(j->arg3)->_box;
			break;
		case 430:
			db->getChar(j->arg2)->_mode = 1;
			db->getChar(j->arg2)->_modeCount = j->arg3;
			break;
		case 431:
			db->getChar(j->arg2)->_mode = 1;
			db->getChar(j->arg2)->_modeCount = db->getVar(j->arg3);
			break;
		case 432:
			db->getChar(j->arg2)->_isVisible = true;
			break;
		case 433:
			db->getChar(j->arg2)->_isVisible = false;
			break;
		case 434:
			//warning("TODO: doActionCollide(%d, %d)", j->arg2, j->arg3);
			keepProcessing = false;
			break;
		case 438:
			db->getChar(j->arg2)->_strength = j->arg3;
			db->getChar(j->arg2)->_defense = j->arg4;
			db->getChar(j->arg2)->_damageMin = j->arg5;
			db->getChar(j->arg2)->_damageMax = j->arg6;
			break;
		case 441:
			keepProcessing = db->giveObject(j->arg2, j->arg3, j->arg4);
			break;
		case 444:
			db->getObj(j->arg2)->isCarryable = 1;
			break;
		case 445:
			db->getObj(j->arg2)->isVisible = 1;
			break;
		case 446:
			db->getObj(j->arg2)->data4 = 0;
			break;
		case 448:
			db->getObj(j->arg2)->isVisible = 0;
			break;
		case 449:
			keepProcessing = _settings.dayMode == 0;
			break;
		case 450:
			keepProcessing = _settings.dayMode == 1;
			break;
		case 451:
			keepProcessing = _settings.dayMode == 2;
			break;
		case 452:
			keepProcessing = _settings.dayMode == 3;
			break;
		case 453:
			setDay();
			break;
		case 454:
			setNight();
			break;
		case 458:
			db->getChar(j->arg2)->_xtend = j->arg3;
			changeMode(j->arg2, 2);
			break;
		case 459:
			db->getLoc(j->arg2)->xtend = j->arg3;
			changeMode(j->arg2, 3);
			break;
		case 465:
			db->setVar(j->arg2, db->getChar(j->arg3)->_xtend);
			break;
		case 466:
			db->setVar(j->arg2, db->getLoc(j->arg3)->xtend);
			break;
		case 467:
			warning("TODO: PlayVideo(%s)", j->arg1);
			break;
		case 468:
			doActionSpriteScene(j->arg1, j->arg2, j->arg3, j->arg4);
			break;
		case 469:
			warning("TODO: PlaySample(%s)", j->arg1);
			break;
		case 473:
			db->getChar(0)->_start3 = db->getChar(0)->_start3PrevPrev;
			db->getChar(0)->_start4 = db->getChar(0)->_start4PrevPrev;
			db->getChar(0)->_start5 = db->getChar(0)->_start5Prev;
			db->getChar(0)->_screenX = db->getChar(0)->_start3 / 256;
			db->getChar(0)->_screenY = db->getChar(0)->_start4 / 256;
			db->getChar(0)->_gotoX = db->getChar(0)->_screenX;
			db->getChar(0)->_gotoY = db->getChar(0)->_screenY;
			db->getChar(0)->_lastBox = db->whatBox(db->getChar(0)->_lastLocation,
				db->getChar(0)->_screenX, db->getChar(0)->_screenY);

			_player.command = CMD_NOTHING;
			_player.commandState = 0;

			break;
		case 474:
			if (strcmp(j->arg1, "REFRESH") == 0) {
				warning("TODO: Unhandled external action: REFRESH");
			} else {
				db->setVar(j->arg2, doExternalAction(j->arg1));
			}
			break;
		case 475:
			keepProcessing = false;
			rc = true;
			break;
		case 485:
			_settings.fightEnabled = true;
			break;
		case 486:
			_settings.fightEnabled = false;
			break;
		case 487:
			// warning("TODO: npcFight(%d, %d)", j->arg2, j->arg3);
			keepProcessing = false;
			break;
		case 488:
			warning("TODO: castSpell(%d, %d, %d)", j->arg2, j->arg3, j->arg4);
			break;
		case 494:
			_vm->endGame();
			break;
		default:
			warning("Unhandled OpCode: %d - (%s, %d, %d, %d, %d, %d)", j->opcode,
				j->arg1, j->arg2, j->arg3, j->arg4, j->arg5, j->arg6);
		}
	}

	return rc;
}

void Game::doCommand(int command, int type, int id, int thingy) {
	Common::List<EventLink> events;
	switch (command) {
	case 2:
		break;
	case 3:
		break;
	case 4:
		break;
	case 5:
		break;
	case 6:
		break;

	// Enter room
	case 7:
		events = _vm->database()->getLoc(_settings.currLocation)->events;
		for (Common::List<EventLink>::iterator j = events.begin(); j != events.end(); ++j) {
			if (j->exitBox == id) {
				doProc(j->proc, 3, 318, -1);
				break;
			}
		}
		break;
	case 9:
		break;
	case 10:
		break;
	}
}

void Game::loopMove() {
	Character *chr = _vm->database()->getChar(0);

	if (chr->_spriteTimer == 0)
		chr->moveChar(true);
	chr->moveCharOther();

	if (chr->_spriteTimer > 0) {
		// TODO
	}

	// TODO: something with cbMemory

	if (chr->_gotoBox != chr->_lastBox) {
		chr->_gotoBox = chr->_lastBox;

		_vm->database()->setCharPos(0, chr->_lastLocation, chr->_lastBox);

		// Run "enter room" script
		doCommand(7, -1, chr->_lastBox, -1);
	}

	for (uint16 i = 1; i < _vm->database()->charactersNum(); ++i) {
		chr = _vm->database()->getChar(i);

		if (!(chr->_isAlive)) {
			// TODO - set some stuff
			chr->_screenH = 0;
			chr->_offset10 = 0;
			chr->_offset14 = chr->_offset20 = 262144;
			chr->_scopeWanted = 100;
			chr->moveCharOther();

		} else if (chr->_walkSpeed == 0) {
			chr->_scopeWanted = 17;
			chr->_priority = _vm->database()->getPriority(chr->_lastLocation, chr->_lastBox);
			chr->moveCharOther();

		} else {
			if (chr->_spriteCutState == 0 && chr->_fightPartner < 0) {
				int16 destBox = chr->_destBox;
				chr->_gotoLoc = chr->_destLoc;

				if (destBox + 5 <= 3)  {
					switch (destBox + 5) {
					case 0:
					case 1:
					case 2:
					case 3:
						break;
					}
					// TODO
				} else {
					chr->_gotoX = _vm->database()->getMidX(chr->_gotoLoc, destBox);
					chr->_gotoY = _vm->database()->getMidY(chr->_gotoLoc, destBox);
				}
			}

			// TODO - collision
			// TODO - fight-related thing

			if (chr->_mode == 1)
				chr->stopChar();

			if (chr->_spriteTimer <= 0 && chr->_fightPartner < 0)
				chr->moveChar(true);
			chr->moveCharOther();

			if (chr->_gotoBox != chr->_lastBox) {
				chr->_gotoBox = chr->_lastBox;
				_vm->database()->setCharPos(i, chr->_lastLocation, chr->_lastBox);
			}
		}
	}

	// TODO - handle magic actors
}

void Game::loopCollide() {
	// FIXME: start from 0 because "room" debug command fails. re-add later
	for (uint16 i = 0; i < _vm->database()->charactersNum(); ++i) {
		Character *chr = _vm->database()->getChar(i);

		if ((_vm->database()->getBox(chr->_lastLocation, chr->_lastBox)->attrib & 1) != 0) {
			hitExit(i, true);

			_vm->database()->setCharPos(i, chr->_lastLocation, chr->_lastBox);

			// If in the same room as the player
			if (chr->_lastLocation == _vm->database()->getChar(0)->_lastLocation) {

				int counter = 0;
				//for (uint16 j = 1; j < _vm->database()->charactersNum(); ++j) {
				//	Character *chr2 = _vm->database()->getChar(j);

				//	if (chr2->_isAlive && chr2->_isVisible)
				//		if (_vm->database()->getChar(0)->_lastLocation == chr2->_lastLocation)
				//			++counter;
				//}

				if (counter > 1) {
					// TODO: komdbDoCommand(9, ...) -- call the character's "collide" script
				}
			}
		}
	}

	// TODO: collide magic actors

	// Collide characters with doors
	for (uint16 i = 0; i < _vm->database()->charactersNum(); ++i) {
		Character *chr = _vm->database()->getChar(i);
		if (chr->_lastLocation != _vm->database()->getChar(0)->_lastLocation)
			continue;

		for (uint j = 0; j < _roomDoors.size(); j++) {
			if (_roomDoors[j].boxHit < 2)
				if (chr->_lastBox == _roomDoors[j].boxOpenSlow)
					_roomDoors[j].boxHit = 1;
				else if (chr->_lastBox == _roomDoors[j].boxOpenFast)
					_roomDoors[j].boxHit = 2;
		}
	}

	// Animate doors
	for (uint i = 0; i < _roomDoors.size(); i++) {
		int newState;
		Actor *act = _vm->actorMan()->get(_roomDoors[i].actorId);

		switch (_roomDoors[i].boxHit) {
		// Close the door
		case 0:
			newState = 1;
			if (_roomDoors[i].frame > 0)
				(_roomDoors[i].frame)--;
			break;
		// Open slowly
		case 1:
			newState = 0;
			if (_roomDoors[i].frame < act->getFramesNum() - 1)
				(_roomDoors[i].frame)++;
			break;
		// Open fast
		case 2:
			newState = 2;
			_roomDoors[i].frame = act->getFramesNum() - 1;
		}

		_roomDoors[i].boxHit = 0;
		if (_roomDoors[i].frame > 0) {
			switch (newState) {
			case 0:
				if (_roomDoors[i].state == 1 && _vm->_flicLoaded != 2) {
					// TODO: play door sound
				}
				break;
			case 1:
				if (_roomDoors[i].state != 1 && _vm->_flicLoaded != 2) {
					// TODO: play door sound
				}
			}
		}

		_roomDoors[i].state = newState;
	}
}

void Game::loopSpriteCut() {
	for (uint16 i = 0; i < _vm->database()->charactersNum(); ++i) {
		Character *chr = _vm->database()->getChar(i);

		if (chr->_actorId < 0)
			continue;

		switch (chr->_spriteCutState) {
		case 0:
			break;
		case 1:
			chr->_spriteCutState = 2;
			break;
		case 2:
			if (chr->_stopped) {
				chr->_lastDirection = 4;
				if (chr->_sprite8c != 0 || strlen(chr->_spriteName) >= 2) {
					chr->_spriteTimer = 0;
					chr->setScope(101);
					chr->_spriteCutState = 3;
				} else {
					chr->_spriteCutState = 0;
					chr->_isBusy = false;
				}
			}
			break;
		case 3:
			if (chr->_spriteTimer > 0)
				chr->_spriteTimer--;
			if (chr->_spriteTimer <= 1)
				chr->_isBusy = false;

			if (chr->_spriteTimer == 0) {
				chr->_spriteCutState = 0;
				chr->_lastDirection = 4;
			}
			break;
		default:
			break;
		}
	}
}

/** Play idle animations */
void Game::loopTimeouts() {
	if (_vm->_flicLoaded != 0)
		return;

	// TODO: something with spriteCutNum

	for (uint16 i = 1; i < _vm->database()->charactersNum(); ++i) {
		Character *chr = _vm->database()->getChar(i);

		if (chr->_isAlive) {
			if (!(chr->_stopped)) {
				chr->_stoppedTime = 0;
			} else {
				if (chr->_spriteTimer == 0)
					chr->_stoppedTime++;

				if (chr->_timeout == chr->_stoppedTime)
					chr->_lastDirection = 4;

				if (chr->_timeout > 0 && chr->_stoppedTime >= chr->_timeout) {
					chr->_stoppedTime = 0;
					chr->_scopeWanted = 12;
				}
			}
		}

		if (chr->_lastLocation == _vm->database()->getChar(0)->_lastLocation &&
			chr->_spriteTimer > 0 && chr->_scopeInUse == 12)
				_player.spriteCutNum++;
	}
}

void Game::loopInterfaceCollide() {
	Character *playerChar = _vm->database()->getChar(0);
	int boxId;

	if (playerChar->_lastLocation == 0)
		return;

	_settings.collideBoxX = _settings.mouseX = _vm->input()->getMouseX() * 2;
	_settings.collideBoxY = _settings.mouseY = _vm->input()->getMouseY() * 2;

	_settings.collideBox = _settings.mouseBox =
		_vm->database()->whatBox(playerChar->_lastLocation,
			_settings.collideBoxX, _settings.collideBoxY);

	if (_settings.collideBox < 0) {
		_settings.mouseOverExit = false;

		_vm->database()->getClosestBox(playerChar->_lastLocation,
				_settings.mouseX, _settings.mouseY,
				playerChar->_screenX, playerChar->_screenY,
				&_settings.collideBox, &_settings.collideBoxX,
				&_settings.collideBoxY);
	} else {
		Box *box = _vm->database()->getBox(playerChar->_lastLocation,
				_settings.collideBox);

		boxId = _settings.collideBox;

		if ((box->attrib & 8) != 0) {
			int link = _vm->database()->getFirstLink(
						playerChar->_lastLocation,
						_settings.collideBox);
			if (link != -1)
				boxId = link;
		}

		box = _vm->database()->getBox(playerChar->_lastLocation,
				boxId);

		if ((box->attrib & 1) == 0) {
			_settings.mouseOverExit = false;
		} else {
			_settings.collideBox = boxId;

			_settings.collideBoxX =
				_vm->database()->getMidX(playerChar->_lastLocation,
						_settings.collideBox);

			_settings.collideBoxY =
				_vm->database()->getMidY(playerChar->_lastLocation,
						_settings.collideBox);

			_settings.mouseOverExit = true;
		}

		box = _vm->database()->getBox(playerChar->_lastLocation,
				_settings.collideBox);

		if ((box->attrib & 8) != 0) {
			_settings.collideBox = _vm->database()->getFirstLink(
					playerChar->_lastLocation,
					_settings.collideBox);

			_settings.collideBoxX =
				_vm->database()->getMidX(playerChar->_lastLocation,
						_settings.collideBox);

			_settings.collideBoxY =
				_vm->database()->getMidY(playerChar->_lastLocation,
						_settings.collideBox);
		}
	}

	// Hover over objects

	_settings.collideObj = -1;
	_settings.collideObjZ = 1073741824;

	Common::Array<RoomObject> *roomObjects = _vm->game()->getObjects();

	for (uint i = 0; i < roomObjects->size(); i++) {
		Object *obj = _vm->database()->object((*roomObjects)[i].objectId);
		int32 z = 0;
		int16 x, y;

		if (!obj->isVisible)
			continue;

		// If the mouse box is on the object box
		if (obj->box == _settings.mouseBox) {
			int8 link = _vm->database()->getFirstLink(
					playerChar->_lastLocation, obj->box);
			Box *box = _vm->database()->getBox(playerChar->_lastLocation, link);
			x = _vm->database()->getMidX(playerChar->_lastLocation, link);
			y = _vm->database()->getMidY(playerChar->_lastLocation, link);
			z = box->z1;
		}

		// If the mouse is over the drawn object
		if ((*roomObjects)[i].actorId >= 0) {

			Actor *act = _vm->actorMan()->get((*roomObjects)[i].actorId);
			if (act->inPos(_settings.mouseX / 2, _settings.mouseY / 2)) {
				int8 link = _vm->database()->getFirstLink(
						playerChar->_lastLocation, obj->box);
				Box *box = _vm->database()->getBox(playerChar->_lastLocation, link);
				x = _vm->database()->getMidX(playerChar->_lastLocation, link);
				y = _vm->database()->getMidY(playerChar->_lastLocation, link);
				z = box->z1;
			}
		}

		if (z > 0 && z < _settings.collideObjZ) {
			_settings.collideObj = (*roomObjects)[i].objectId;
			_settings.collideObjX = x;
			_settings.collideObjY = y;
			_settings.collideObjZ = z;
		}
	}

	// Hover over characters

	_settings.collideChar = -1;
	_settings.collideCharZ = 1073741824;

	for (int i = 1; i < _vm->database()->charactersNum(); ++i) {
		Character *chr = _vm->database()->getChar(i);

		if (playerChar->_lastLocation != chr->_lastLocation ||
			!chr->_isVisible)
			continue;

		if (chr->_spriteCutState != 0 && chr->_scopeInUse != 12)
			continue;

		// If the mouse is over the drawn character
		Actor *act = _vm->actorMan()->get(chr->_actorId);
		if (!act->inPos(_settings.mouseX / 2, _settings.mouseY / 2))
			continue;

		if (chr->_start5 >= _settings.collideCharZ)
			continue;

		Box *box = _vm->database()->getBox(chr->_lastLocation,
				chr->_lastBox);

		if (box->attrib == 8) {
			int link = _vm->database()->getFirstLink(chr->_lastLocation,
				chr->_lastBox);
			_settings.collideCharX =
				_vm->database()->getMidX(chr->_lastLocation, link);
			_settings.collideCharY =
				_vm->database()->getMidY(chr->_lastLocation, link);
		} else {
			_settings.collideCharX = chr->_screenX;
			_settings.collideCharY = chr->_screenY;
		}

		_settings.collideChar = i;
		_settings.collideCharZ = chr->_start5;
	}

	// Special handling of the Ninja Baker.
	// His idle animation moves outside his box, so don't allow focus.
	// (This actually makes sense -- ninjas move very fast)
	if (_settings.collideChar == 62 &&
			_vm->database()->getChar(62)->_spriteTimer > 0) {
		_settings.collideChar = -1;
	}

	_settings.mouseBox = _settings.collideBox;

	if (_settings.mouseOverExit) {
		_settings.collideBoxZ = 32000;
	} else {
		_settings.collideBoxZ =
			_vm->database()->getZValue(playerChar->_lastLocation,
					_settings.collideBox, _settings.collideBoxY * 256);
	}

	// TODO: handle inventory
	if (_vm->_flicLoaded != 0)
		return;

	_settings.oldOverType = _settings.overType;
	_settings.oldOverNum = _settings.overNum;
	_settings.overType = 0;
	_settings.overNum = -1;

	// FIXME - some code duplication in this horrible tree
	if (_settings.objectNum < 0) {
		if (_settings.mouseOverExit &&
		    _settings.collideBoxZ < _settings.collideCharZ &&
		    _settings.collideBoxZ < _settings.collideObjZ) {

			_settings.overType = 1;
			_settings.overNum = _settings.collideBox;
			_settings.mouseState = 2; // Exit

		} else if (_settings.collideChar >= 0 &&
			_settings.collideCharZ < _settings.collideObjZ) {

			_settings.overType = 2;
			_settings.overNum = _settings.collideChar;
			_settings.mouseState = 3; // Hotspot

		} else if (_settings.collideObj >= 0) {

			_settings.overType = 3;
			_settings.overNum = _settings.collideObj;
			_settings.mouseState = 3; // Hotspot

		} else {
			_settings.mouseState = 0; // Walk
		}

	} else {
		if (_player.command == CMD_THING1) {

			if (_settings.collideChar >= 0 &&
				_settings.collideCharZ < _settings.collideObjZ) {

				_settings.overType = 2;
				_settings.overNum = _settings.collideChar;
				_settings.mouseState = 3; // Hotspot

			} else if (_settings.collideObj >= 0) {

				_settings.overType = 3;
				_settings.overNum = _settings.collideObj;
				_settings.mouseState = 3; // Hotspot

			} else {
				_settings.mouseState = 0; // Walk
			}

		} else if (_player.command == CMD_THING2 ||
		           _player.command == CMD_THING3) {
			if (_settings.collideChar < 0)
				_settings.mouseState = 0; // Walk
			else {
				_settings.overType = 2;
				_settings.overNum = _settings.collideChar;
				_settings.mouseState = 3; // Hotspot
			}
		}
	}
}

void Game::changeMode(int value, int mode) {
	warning("TODO: changeMode - unsupported mode");
}

int16 Game::doExternalAction(const char *action) {
	if (strcmp(action, "getquest") == 0) {
		return _player.selectedQuest;
	} else {
		// TODO - warning("Unknown external action: %s", action);
		return 0;
	}
}
void Game::doActionDusk() {
	_player.isNight = 1;
	enterLocation(_vm->database()->getChar(0)->_locationId);
}

void Game::doActionDawn() {
	_player.isNight = 0;
	enterLocation(_vm->database()->getChar(0)->_locationId);
}

void Game::setDay() {
	if (_vm->database()->getLoc(_settings.currLocation)->allowedTime != 2 && _settings.dayMode != 0) {
		_settings.gameCycles = 3600;
		_settings.dayMode = 1;
	}
}

void Game::setNight() {
	if (_vm->database()->getLoc(_settings.currLocation)->allowedTime != 1 && _settings.dayMode != 1) {
		_settings.gameCycles = 6000;
		_settings.dayMode = 0;
	}
}

void Game::doActionMoveChar(uint16 charId, int16 loc, int16 box) {
	Character *chr = _vm->database()->getChar(charId);
	if (charId == 0 && loc > 0) {
		enterLocation(loc);
		// TODO: flicUpdateBG()
	}

	if (_vm->database()->loc2loc(loc, chr->_lastLocation) == -1) {
		// TODO: magicActors thing
		//for (int i = 0; i < 10; ++i) {
		//}
	}

	chr->_lastLocation = loc;
	chr->_lastBox = box;
	chr->_gotoBox = -1;

	chr->_screenX = _vm->database()->getMidX(loc, box);
	chr->_start3 = chr->_screenX * 256;
	chr->_screenY = _vm->database()->getMidY(loc, box);
	chr->_start4 = chr->_screenY * 256;
	chr->_start5 = _vm->database()->getZValue(loc, box, chr->_start4);
	chr->stopChar();
	chr->_lastDirection = 4;
}

void Game::doActionSpriteScene(const char *name, int charId, int loc, int box) {
	static const char *prefixes[] = { "DLC001", "DLC006", "EWC000" };
	static int tabs[] = { 0, 1, 2 }; // TODO - check real values
	static int nums[] = { 29, 30, 29 };
	Character *chr = _vm->database()->getChar(charId);
	String spriteName(name);

	spriteName.toUppercase();

	if (loc < 0)
		chr->_gotoLoc = chr->_lastLocation;
	else
		chr->_gotoLoc = loc;

	if (box < 0) {
		chr->_gotoX = chr->_screenX;
		chr->_gotoY = chr->_screenY;
	} else {
		chr->_spriteBox = box;
		chr->_gotoX = _vm->database()->getMidX(loc, box);
		chr->_gotoY = _vm->database()->getMidY(loc, box);
	}

	_player.spriteCutMoving = false;

	// Check if it's a moving sprite
	if (charId == 0 && name != 0) {

		_player.command = CMD_SPRITE_SCENE;

		int8 match = -1;
		for (int8 i = 0; i < 3; ++i) {
			if (spriteName.hasPrefix(prefixes[i])) {
				_player.spriteCutTab = -1;
				match = i;
				break;
			}
		}

		if (match != -1) {
			_player.spriteCutNum = nums[match];
			_player.spriteCutTab = tabs[match];
			_player.spriteCutPos = 0;

			_player.spriteCutMoving = true;
			_player.spriteCutX = chr->_screenX / 2;
			_player.spriteCutY = chr->_screenY / 2;
		}
	}

	// Scope 12 is the idle animation
	if (chr->_spriteCutState == 0 || chr->_scopeInUse == 12)
		chr->_spriteCutState = 1;

	chr->_spriteName = name;
	chr->_sprite8c = 0;
	chr->_isBusy = true;
}

} // End of namespace Kom
