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

#include "ring/game/application_ring.h"

#include "ring/base/art.h"
#include "ring/base/sound.h"

#include "ring/game/bag.h"
#include "ring/game/saveload.h"

#include "ring/helpers.h"
#include "ring/ring.h"

#include "common/archive.h"

namespace Ring {

// List of animations
static const struct {
	Common::String filenameFrom;
	Common::String filenameTo;
	uint32 a3;
	uint32 ticksWait;
	LoadFrom loadFrom;
	ArchiveType archiveType;
} introScreens[11] = {
	{"beg0.bmp", "beg1.bmp", 20, 3000, kLoadFromDisk, kArchiveFile},
	{"beg1.bmp", "beg0.bmp", 20,    0, kLoadFromDisk, kArchiveFile},
	{"beg0.bmp", "beg2.bmp", 20, 3000, kLoadFromDisk, kArchiveFile},
	{"beg2.bmp", "beg0.bmp", 20,    0, kLoadFromDisk, kArchiveFile},
	{"beg0.bmp", "beg3.bmp", 20, 3000, kLoadFromDisk, kArchiveFile},
	{"beg3.bmp", "beg0.bmp", 20,    0, kLoadFromDisk, kArchiveFile},
	{"beg0.bmp", "beg4.bmp", 20, 3000, kLoadFromDisk, kArchiveFile},
	{"beg4.bmp", "beg0.bmp", 20,    0, kLoadFromDisk, kArchiveFile},
	{"beg0.bmp", "beg5.bmp", 20, 3000, kLoadFromDisk, kArchiveFile},
	{"beg5.bmp", "beg0.bmp", 20,    0, kLoadFromDisk, kArchiveFile},
	{"beg0.bmp", "beg6.bmp", 20, 6000, kLoadFromDisk, kArchiveFile}
};

ApplicationRing::ApplicationRing(RingEngine *engine) : Application(engine) {
}

ApplicationRing::~ApplicationRing() {
}

#pragma region Game setup

void ApplicationRing::setup() {
	setupZone(kZoneSY, 0);

	// Setup cursors
	ArchiveType archiveType = (_configuration.artCURSOR ? kArchiveArt : kArchiveFile);

	cursorAdd(kCursorDefault,       "",                kCursorTypeNormal,   1, kLoadFromCursor, archiveType);
	cursorAdd(kCursorBusy,          "cur_busy",        kCursorTypeImage,    1, kLoadFromCursor, archiveType);
	cursorAdd(kCursorHandSelection, "ni_handsel",      kCursorTypeImage,    1, kLoadFromCursor, archiveType);
	cursorAdd(kCursorIdle,          "cur_idle",        kCursorTypeAnimated, 1, 15, 12.5f, 4, kLoadFromCursor, archiveType);
	cursorAdd(kCursorMove,          "cur_muv",         kCursorTypeAnimated, 1, 20, 12.5f, 4, kLoadFromCursor, archiveType);
	cursorAdd(kCursorHotspot,       "cur_hotspot",     kCursorTypeAnimated, 1, 19, 12.5f, 4, kLoadFromCursor, archiveType);
	cursorAdd(kCursorBack,          "cur_back",        kCursorTypeImage,    1, kLoadFromCursor, archiveType);
	cursorAdd(kCursorMenuIdle,      "cur_menuidle",    kCursorTypeNormal,   1, kLoadFromCursor, archiveType);
	cursorAdd(kCursorMenuActive,    "cur_menuactive",  kCursorTypeImage,    1, kLoadFromCursor, archiveType);

	// Adjust offsets
	cursorSetOffset(kCursorHandSelection, Common::Point(15, 15));
	cursorSetOffset(kCursorIdle, Common::Point(10, 6));
	cursorSetOffset(kCursorMove, Common::Point(10, 6));
	cursorSetOffset(kCursorHotspot, Common::Point(10, 6));
	cursorSetOffset(kCursorBack, Common::Point(10, 20));

	// Setup subtitles
	subtitleSetColor(Color(255, 255, 255));
	subtitleSetBackgroundColor(Color(50, 50, 50));
}

#pragma endregion

#pragma region Startup

void ApplicationRing::showStartupScreen() {
	setZoneAndEnableBag(kZoneSY);
	playMovie("logo", 0.0);

	for (uint i = 0; i < ARRAYSIZE(introScreens); i++) {

		displayFade(introScreens[i].filenameFrom, introScreens[i].filenameTo, introScreens[i].a3, introScreens[i].ticksWait, introScreens[i].loadFrom, introScreens[i].archiveType);

		// Skip intro screens if ESCAPE is pressed
		Common::Event ev;
		g_engine->getEventManager()->pollEvent(ev);
		switch (ev.type) {
		default:
			break;

		case Common::EVENT_KEYDOWN:
			if (ev.kbd.keycode == Common::KEYCODE_ESCAPE)
				return;

			break;
		}
	}
}

void ApplicationRing::startMenu(bool load) {
	if (_field_6F)
		return;

	if (load) {
		warning("[ApplicationRing::startMenu] Loading not implemented");
	}

	_field_6F = _zone;

	//sub_406EA0(4);
	setZoneAndEnableBag(kZoneSY);
	initMenu(kPuzzleMenu, true, true);
	//puzzleSetMod(1, 1, 0);

	for (uint32 i = 0; i < 7; i++) {
		objectSetAccessibilityOff((ObjectId)i);
		objectPresentationHideAndRemove((ObjectId)i);
	}

	Bag *bag = getBag();
	if (bag && bag->getField94())
		bag->sub_419350();

	//cursorDeleteType();

	if (load) {
		objectSetAccessibilityOn(kObjectMenuContinue);
		objectSetAccessibilityOn(kObjectMenuSave);
	} else {
		objectSetAccessibilityOff(kObjectMenuContinue);
		objectSetAccessibilityOff(kObjectMenuSave);
	}
}

void ApplicationRing::initMenu(PuzzleId id, bool a2, bool a3) {
	error("[ApplicationRing::initMenu] Not implemented");
}

#pragma endregion


#pragma region Timer

void ApplicationRing::onZoneTimer(TimerId timerId) {
	switch (getCurrentZone()) {
	default:
	case kZoneSY:
	case kZoneWA:
		break;

	case kZoneNI:
		onZoneTimerNI(timerId);
		break;

	case kZoneRH:
		onZoneTimerRH(timerId);
		break;

	case kZoneFO:
		onZoneTimerFO(timerId);
		break;

	case kZoneRO:
		onZoneTimerRO(timerId);
		break;

	case kZoneAS:
		onZoneTimerAS(timerId);
		break;

	case kZoneN2:
		onZoneTimerN2(timerId);
		break;
	}
}

void ApplicationRing::onZoneTimerNI(TimerId id) {
	error("[ApplicationRing::onZoneTimerNI] Not implemented");
}

void ApplicationRing::onZoneTimerRH(TimerId id) {
	error("[ApplicationRing::onZoneTimerNI] Not implemented");
}

void ApplicationRing::onZoneTimerFO(TimerId id) {
	error("[ApplicationRing::onZoneTimerNI] Not implemented");
}

void ApplicationRing::onZoneTimerRO(TimerId id) {
	switch (id) {
	default:
		break;

	case kTimer0:
		varSetByte(40806, varGetByte(40806) + 1);

		if (varGetByte(40806) == 10) {
			timerStop(kTimer0);
			varSetByte(40601, 10);
		}

		objectPresentationShow(kObject40101, varGetByte(40806));
		break;

	case kTimer1:
		varSetByte(40807, varGetByte(40807) + 1);

		if (varGetByte(40807) == 90) {
			timerStop(kTimer1);
			varSetByte(40602, 90);
		}

		objectPresentationShow(kObject40102, varGetByte(40807));
		break;
	}

	error("[ApplicationRing::onZoneTimerNI] Not implemented");
}

void ApplicationRing::onZoneTimerAS(TimerId id) {
	error("[ApplicationRing::onZoneTimerNI] Not implemented");
}

void ApplicationRing::onZoneTimerN2(TimerId id) {
	if (id == kTimer0)
		noiceIdPlay(_vm->getRandom().getRandomNumber(12) + 70004, 1);
}

#pragma endregion

#pragma region Zone setup

void ApplicationRing::setupZone(Zone zone, uint32 a2)  {
	bool load = _saveManager->isLoaded(a2);

	// Check saved data for zone and/or puzzle id
	if (!load) {
		if (zone == kZoneSY) {
			load = true;
		} else {
			error("[ApplicationRing::setupZone] Zone CD check not implemented");
		}
	}

	puzzleReset();
	warning("[ApplicationRing::setupZone] Missing function call");

	getSoundHandler()->reset();

	if (zone != kZoneSY)
		getArtHandler()->remove();

	if (load) {
		setZoneAndEnableBag(zone);
		setZone(zone, a2);
	} else {
		error("[ApplicationRing::setupZone] CD check not implemented");
	}
}

void ApplicationRing::setZoneAndEnableBag(Zone zone) {
	_zone = zone;
	_zoneString = getZoneString(zone);

	// Enable or disable bag
	if (zone == kZoneSY || zone == kZoneAS)
		getBag()->disable();
	else
		getBag()->enable();
}

void ApplicationRing::setZone(Zone zone, uint32 a2) {
	bool load = _saveManager->isLoaded(a2);

	if (zone != kZoneSY && !load) {
		if (getReadFrom(zone) == kArchiveArt) {
			if (!getArtHandler())
				error("[ApplicationRing::setZone] Art handler is not initialized properly");

			getArtHandler()->open(zone, kLoadFromCd);
		}
	}

	if (a2 == 1000) {
		error("[ApplicationRing::setZone] Not implemented (a2 == 1000)");
	}

	// Set zone
	switch (zone) {
	default:
	case kZoneSY:
		break;

	case kZoneNI:
		setZoneNI(zone, a2);
		break;

	case kZoneRH:
		setZoneRH(zone, a2);
		break;

	case kZoneFO:
		setZoneFO(zone, a2);
		break;

	case kZoneRO:
		setZoneRO(zone, a2);
		break;

	case kZoneWA:
		setZoneWA(zone, a2);
		break;

	case kZoneAS:
		setZoneAS(zone, a2);
		break;

	case kZoneN2:
		setZoneN2(zone, a2);
		break;
	}
}

void ApplicationRing::setZoneNI(Zone zone, uint32 a2) {
	error("[ApplicationRing::setZoneNI] Not implemented");
}

void ApplicationRing::setZoneRH(Zone zone, uint32 a2) {
	error("[ApplicationRing::setZoneRH] Not implemented");
}

void ApplicationRing::setZoneFO(Zone zone, uint32 a2) {
	error("[ApplicationRing::setZoneFO] Not implemented");
}

void ApplicationRing::setZoneRO(Zone zone, uint32 a2) {
	error("[ApplicationRing::setZoneRO] Not implemented");
}

void ApplicationRing::setZoneWA(Zone zone, uint32 a2) {
	error("[ApplicationRing::setZoneWA] Not implemented");
}

void ApplicationRing::setZoneAS(Zone zone, uint32 a2) {
	error("[ApplicationRing::setZoneAS] Not implemented");
}

void ApplicationRing::setZoneN2(Zone zone, uint32 a2) {
	error("[ApplicationRing::setZoneN2] Not implemented");
}

#pragma endregion

#pragma region Zone full names, short string and ReadFrom

Common::String ApplicationRing::getZoneString(Zone zone) const {
	switch (zone) {
	default:
		break;

	case kZoneSY:
		return "sy";

	case kZoneNI:
		return "ni";

	case kZoneRH:
		return "rh";

	case kZoneFO:
		return "fo";

	case kZoneRO:
		return "ro";

	case kZoneWA:
		return "wa";

	case kZoneAS:
		return "as";

	case kZoneN2:
		return "n2";
	}

	error("[Application::getZone] Invalid zone (%d)", zone);
}

Common::String ApplicationRing::getZoneLongName(Zone zone) const {
	switch (zone) {
	default:
		break;

	case kZoneSY:
		return "";

	case kZoneNI:
	case kZoneRH:
		return "Alberich";

	case kZoneFO:
		return "Siegmund";

	case kZoneWA:
		return "Brnnnhilde";

	case kZoneAS:
		return "Dril";

	case kZoneRO:
	case kZoneN2:
		return "Loge";
	}

	error("[Application::getZoneName] Invalid zone (%d)", zone);
}

ArchiveType ApplicationRing::getReadFrom(Zone zone) const {
	if (_archiveType == kArchiveFile)
		return kArchiveFile;

	switch (zone) {
	default:
		break;

	case kZoneSY:
		return _configuration.artSY ? kArchiveArt : kArchiveFile;

	case kZoneNI:
		return _configuration.artNI ? kArchiveArt : kArchiveFile;

	case kZoneRH:
		return _configuration.artRH ? kArchiveArt : kArchiveFile;

	case kZoneFO:
		return _configuration.artFO ? kArchiveArt : kArchiveFile;

	case kZoneRO:
		return _configuration.artRO ? kArchiveArt : kArchiveFile;

	case kZoneWA:
		return _configuration.artWA ? kArchiveArt : kArchiveFile;

	case kZoneAS:
		return _configuration.artAS ? kArchiveArt : kArchiveFile;

	case kZoneN2:
		return _configuration.artN2 ? kArchiveArt : kArchiveFile;
	}

	error("[ApplicationRing::getReadFrom] Invalid zone (%d)", zone);
}

#pragma endregion

#pragma region Zone initialization

void ApplicationRing::initZones() {
	_loadFrom = kLoadFromDisk;

	_archiveType = _configuration.artSY ? kArchiveArt : kArchiveFile;
	initZoneSY();

	_loadFrom = kLoadFromCd;

	_archiveType = _configuration.artAS ? kArchiveArt : kArchiveFile;
	initZoneAS();

	_archiveType = _configuration.artNI ? kArchiveArt : kArchiveFile;
	initZoneNI();

	_archiveType = _configuration.artN2 ? kArchiveArt : kArchiveFile;
	initZoneN2();

	_archiveType = _configuration.artRO ? kArchiveArt : kArchiveFile;
	initZoneRO();

	_archiveType = _configuration.artRH ? kArchiveArt : kArchiveFile;
	initZoneRH();

	_archiveType = _configuration.artFO ? kArchiveArt : kArchiveFile;
	initZoneFO();

	_archiveType = _configuration.artWA ? kArchiveArt : kArchiveFile;
	initZoneWA();

	if (_configuration.artSY || _configuration.artAS || _configuration.artNI || _configuration.artN2
	 || _configuration.artRO || _configuration.artRH || _configuration.artFO || _configuration.artWA)
		_archiveType = kArchiveArt;
	else
		_archiveType = kArchiveFile;

	_field_66 = 0;
}

void ApplicationRing::initZoneSY() {
	debugC(1, kRingDebugLogic, "Initializing System zone (SY)...");

	setZoneAndEnableBag(kZoneSY);

	//////////////////////////////////////////////////////////////////////////
	// Setup system and dialog boxes
	puzzleAdd(kPuzzle1);

	objectAdd(kObject1, "", "", 1);
	objectAddPresentation(kObject1);
	objectPresentationAddTextToPuzzle(kObject1, 0, kPuzzle1, "", 1, 16, kFontDefault, 0, -1, 0, -1, -1, -1);

	objectAdd(kObject6, "", "", 1);
	objectAddPresentation(kObject6);
	objectPresentationAddImageToPuzzle(kObject6, 0, kPuzzle1, "FO_WOT1_W_A.tga", Common::Point(0, 96), true, 3, 1000);
	objectAddPresentation(kObject6);
	objectPresentationAddImageToPuzzle(kObject6, 1, kPuzzle1, "FO_WOT1_W_B.tga", Common::Point(0, 176), true, 3, 1000);
	objectAddPresentation(kObject6);
	objectPresentationAddImageToPuzzle(kObject6, 2, kPuzzle1, "FO_WOT1_W_C.tga", Common::Point(0, 126), true, 3, 1000);
	objectAddPresentation(kObject6);
	objectPresentationAddImageToPuzzle(kObject6, 3, kPuzzle1, "FO_WOT1_W_D.tga", Common::Point(0, 208), true, 3, 1000);
	objectAddPresentation(kObject6);
	objectPresentationAddImageToPuzzle(kObject6, 4, kPuzzle1, "FO_WOT1_W_E.tga", Common::Point(0, 88), true, 3, 1000);
	objectAddPresentation(kObject6);
	objectPresentationAddImageToPuzzle(kObject6, 5, kPuzzle1, "FO_WOT1_B_A.tga", Common::Point(415, 281), true, 3, 1000);
	objectAddPresentation(kObject6);
	objectPresentationAddImageToPuzzle(kObject6, 6, kPuzzle1, "FO_WOT1_B_C.tga", Common::Point(488, 180), true, 3, 1000);
	objectAddPresentation(kObject6);
	objectPresentationAddImageToPuzzle(kObject6, 7, kPuzzle1, "FO_WOT1_B_E.tga", Common::Point(195, 94), true, 3, 1000);
	objectAddPresentation(kObject6);
	objectPresentationAddImageToPuzzle(kObject6, 8, kPuzzle1, "FO_BRU1_W_A.tga", Common::Point(0, 45), true, 3, 1000);
	objectAddPresentation(kObject6);
	objectPresentationAddImageToPuzzle(kObject6, 9, kPuzzle1, "FO_BRU1_B_A.tga", Common::Point(440, 66), true, 3, 1000);
	objectAddPresentation(kObject6);
	objectPresentationAddImageToPuzzle(kObject6, 10, kPuzzle1, "FO_BRU1_B_B.tga", Common::Point(490, 208), true, 3, 1000);
	objectAddPresentation(kObject6);
	objectPresentationAddImageToPuzzle(kObject6, 11, kPuzzle1, "FO_WOT2_W_A.tga", Common::Point(0, 28), true, 3, 1000);
	objectAddPresentation(kObject6);
	objectPresentationAddImageToPuzzle(kObject6, 12, kPuzzle1, "FO_WOT2_W_B.tga", Common::Point(0, 22), true, 3, 1000);
	objectAddPresentation(kObject6);
	objectPresentationAddImageToPuzzle(kObject6, 13, kPuzzle1, "FO_WOT2_W_C.tga", Common::Point(0, 67), true, 3, 1000);
	objectAddPresentation(kObject6);
	objectPresentationAddImageToPuzzle(kObject6, 14, kPuzzle1, "FO_WOT2_W_D.tga", Common::Point(0, 194), true, 3, 1000);
	objectAddPresentation(kObject6);
	objectPresentationAddImageToPuzzle(kObject6, 15, kPuzzle1, "FO_WOT2_B_A.tga", Common::Point(477, 200), true, 3, 1000);
	objectAddPresentation(kObject6);
	objectPresentationAddImageToPuzzle(kObject6, 16, kPuzzle1, "FO_WOT2_B_C.tga", Common::Point(496, 251), true, 3, 1000);
	objectAddPresentation(kObject6);
	objectPresentationAddImageToPuzzle(kObject6, 17, kPuzzle1, "FO_BRU2_W_A.tga", Common::Point(0, 186), true, 3, 1000);
	objectAddPresentation(kObject6);
	objectPresentationAddImageToPuzzle(kObject6, 18, kPuzzle1, "FO_BRU2_B_A.tga", Common::Point(443, 222), true, 3, 1000);
	objectAddPresentation(kObject6);
	objectPresentationAddImageToPuzzle(kObject6, 19, kPuzzle1, "FO_BRU2_B_B.tga", Common::Point(493, 219), true, 3, 1000);
	objectAddPresentation(kObject6);
	objectPresentationAddImageToPuzzle(kObject6, 20, kPuzzle1, "FO_WOT3_W_A.tga", Common::Point(0, 16), true, 3, 1000);
	objectAddPresentation(kObject6);
	objectPresentationAddImageToPuzzle(kObject6, 21, kPuzzle1, "FO_WOT3_W_B.tga", Common::Point(0, 107), true, 3, 1000);
	objectAddPresentation(kObject6);
	objectPresentationAddImageToPuzzle(kObject6, 22, kPuzzle1, "FO_WOT3_W_C.tga", Common::Point(0, 46), true, 3, 1000);
	objectAddPresentation(kObject6);
	objectPresentationAddImageToPuzzle(kObject6, 23, kPuzzle1, "FO_WOT3_W_D.tga", Common::Point(0, 26), true, 3, 1000);
	objectAddPresentation(kObject6);
	objectPresentationAddImageToPuzzle(kObject6, 24, kPuzzle1, "FO_WOT3_B_A.tga", Common::Point(508, 238), true, 3, 1000);
	objectAddPresentation(kObject6);
	objectPresentationAddImageToPuzzle(kObject6, 25, kPuzzle1, "FO_WOT3_B_B.tga", Common::Point(381, 216), true, 3, 1000);
	objectAddPresentation(kObject6);
	objectPresentationAddImageToPuzzle(kObject6, 26, kPuzzle1, "FO_WOT3_B_D.tga", Common::Point(408, 246), true, 3, 1000);

	objectAdd(kObject7, "", "", 1);
	objectAddPresentation(kObject7);
	objectPresentationAddImageToPuzzle(kObject7, 0, kPuzzle1, "SY_ISHA.bmp", Common::Point(0, 16), true, 1, 1000);

	objectAdd(kObject2, "", "", 1);
	objectAddPresentation(kObject2);
	objectPresentationAddImageToPuzzle(kObject2, 0, kPuzzle1, "Exit.bmp", Common::Point(160, 165), true, 1, 1000);
	objectAddPuzzleAccessibility(kObject2, kPuzzle1, Common::Rect(262, 270, 321, 306), false, 57, 1);
	objectAddPuzzleAccessibility(kObject2, kPuzzle1, Common::Rect(310, 270, 370, 306), false, 57, 0);
	objectSetPuzzleAccessibilityKey(kObject2, 0, Common::KEYCODE_RETURN);
	objectSetPuzzleAccessibilityKey(kObject2, 1, Common::KEYCODE_ESCAPE);
	objectAddPresentation(kObject2);
	objectPresentationAddImageToPuzzle(kObject2, 1, kPuzzle1, "ex_yes.bmp", Common::Point(261, 279), true, 1, 1000);
	objectAddPresentation(kObject2);
	objectPresentationAddImageToPuzzle(kObject2, 2, kPuzzle1, "ex_no.bmp", Common::Point(318, 279), true, 1, 1000);

	objectAdd(kObject3, "", "", 1);
	objectAddPresentation(kObject3);
	objectPresentationAddImageToPuzzle(kObject3, 0, kPuzzle1, "Warning.bmp", Common::Point(160, 165), true, 1, 1000);
	objectPresentationAddTextToPuzzle(kObject3, 0, kPuzzle1, "", 200, 200, kFontDefault, 255u, 95, 0, -1, -1, -1);
	objectPresentationAddTextToPuzzle(kObject3, 0, kPuzzle1, "", 300, 280, kFontDefault, 255u, 95, 0, -1, -1, -1);
	objectAddPresentation(kObject3);
	objectPresentationAddImageToPuzzle(kObject3, 1, kPuzzle1, "wr_ok.tga", Common::Point(313, 281), true, 3, 1000);
	objectAddPuzzleAccessibility(kObject3, kPuzzle1, Common::Rect(286, 269, 363, 307), false, 57, 0);
	objectSetPuzzleAccessibilityKey(kObject3, 0, Common::KEYCODE_RETURN);

	objectAdd(kObject4, "", "", 1);
	objectAddPresentation(kObject4);
	objectPresentationAddImageToPuzzle(kObject4, 0, kPuzzle1, "Question.bmp", Common::Point(160, 165), true, 1, 1000);
	objectPresentationAddTextToPuzzle(kObject4, 0, kPuzzle1, "", 200, 200, kFontDefault, 255u, 95, 0, -1, -1, -1);
	objectPresentationAddTextToPuzzle(kObject4, 0, kPuzzle1, "", 200, 280, kFontDefault, 255u, 95, 0, -1, -1, -1);
	objectAddPresentation(kObject4);
	objectPresentationAddImageToPuzzle(kObject4, 1, kPuzzle1, "g_ok.tga", Common::Point(181, 257), true, 3, 1000);
	objectAddPresentation(kObject4);
	objectPresentationAddImageToPuzzle(kObject4, 2, kPuzzle1, "qu_cancel.tga", Common::Point(181, 282), true, 3, 1000);
	objectAddPuzzleAccessibility(kObject4, kPuzzle1, Common::Rect(277, 300, 347, 340), false, 57, 0);
	objectAddPuzzleAccessibility(kObject4, kPuzzle1, Common::Rect(350, 300, 380, 340), false, 57, 1);
	objectSetPuzzleAccessibilityKey(kObject4, 0, Common::KEYCODE_RETURN);
	objectSetPuzzleAccessibilityKey(kObject4, 1, Common::KEYCODE_ESCAPE);
	objectAddPuzzleAccessibility(kObject4, kPuzzle1, Common::Rect(180, 250, 250, 281), false, 57, 2);
	objectAddPuzzleAccessibility(kObject4, kPuzzle1, Common::Rect(180, 283, 250, 309), false, 57, 3);
	objectSetPuzzleAccessibilityKey(kObject4, 2, Common::KEYCODE_RETURN);
	objectSetPuzzleAccessibilityKey(kObject4, 3, Common::KEYCODE_ESCAPE);
	objectAddPuzzleAccessibility(kObject4, kPuzzle1, Common::Rect(180, 250, 250, 281), false, 57, 4);
	objectAddPuzzleAccessibility(kObject4, kPuzzle1, Common::Rect(180, 283, 250, 309), false, 57, 5);
	objectSetPuzzleAccessibilityKey(kObject4, 4, Common::KEYCODE_RETURN);
	objectSetPuzzleAccessibilityKey(kObject4, 5, Common::KEYCODE_ESCAPE);

	objectAdd(kObject5, "", "", 1);
	puzzleAdd(kPuzzleInsertCd);
	puzzleAddBackgroundImage(kPuzzleInsertCd, "insertcd.bmp", Common::Point(0, 16), true);

	objectAdd(kObject90912, "", "", 1);
	objectAddPresentation(kObject90912);
	objectPresentationAddImageToPuzzle(kObject90912, 0, kPuzzleInsertCd, "g_ok.tga", Common::Point(306, 274), true, 3, 1000);
	objectAddPresentation(kObject90912);
	objectPresentationAddTextToPuzzle(kObject90912, 1, kPuzzleInsertCd, "", 320, 240, kFontDefault, 255, 95, 0, -1, -1, -1);
	objectPresentationAddTextToPuzzle(kObject90912, 1, kPuzzleInsertCd, "", 320, 260, kFontDefault, 255, 95, 0, -1, -1, -1);
	objectPresentationShow(kObject90912, 1);
	objectAddPuzzleAccessibility(kObject90912, kPuzzleInsertCd, Common::Rect(266, 268, 388, 311), true, 57, 0);
	objectAddPuzzleAccessibility(kObject90912, kPuzzleInsertCd, Common::Rect(266, 268, 388, 311), true, 57, 1);
	objectAddPuzzleAccessibility(kObject90912, kPuzzleInsertCd, Common::Rect(266, 268, 388, 311), true, 57, 2);
	objectAddPuzzleAccessibility(kObject90912, kPuzzleInsertCd, Common::Rect(266, 268, 388, 311), true, 57, 3);
	objectAddPuzzleAccessibility(kObject90912, kPuzzleInsertCd, Common::Rect(266, 268, 388, 311), true, 57, 4);
	objectAddPuzzleAccessibility(kObject90912, kPuzzleInsertCd, Common::Rect(266, 268, 388, 311), true, 57, 5);
	objectAddPuzzleAccessibility(kObject90912, kPuzzleInsertCd, Common::Rect(266, 268, 388, 311), true, 57, 6);
	objectAddPuzzleAccessibility(kObject90912, kPuzzleInsertCd, Common::Rect(266, 268, 388, 311), true, 57, 7);
	objectAddPuzzleAccessibility(kObject90912, kPuzzleInsertCd, Common::Rect(266, 268, 388, 311), true, 57, 8);
	objectSetPuzzleAccessibilityKey(kObject90912, 0, Common::KEYCODE_RETURN);
	objectSetPuzzleAccessibilityKey(kObject90912, 1, Common::KEYCODE_RETURN);
	objectSetPuzzleAccessibilityKey(kObject90912, 2, Common::KEYCODE_RETURN);
	objectSetPuzzleAccessibilityKey(kObject90912, 3, Common::KEYCODE_RETURN);
	objectSetPuzzleAccessibilityKey(kObject90912, 4, Common::KEYCODE_RETURN);
	objectSetPuzzleAccessibilityKey(kObject90912, 5, Common::KEYCODE_RETURN);
	objectSetPuzzleAccessibilityKey(kObject90912, 6, Common::KEYCODE_RETURN);
	objectSetPuzzleAccessibilityKey(kObject90912, 7, Common::KEYCODE_RETURN);
	objectSetPuzzleAccessibilityKey(kObject90912, 8, Common::KEYCODE_RETURN);

	//////////////////////////////////////////////////////////////////////////
	// Menu
	puzzleAdd(kPuzzleMenu);
	puzzleAddBackgroundImage(kPuzzleMenu, "GenMen.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzlePreferences);
	puzzleAddBackgroundImage(kPuzzlePreferences, "Preferences.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzleSave);
	puzzleAddBackgroundImage(kPuzzleSave, "Save.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzleLoad);
	puzzleAddBackgroundImage(kPuzzleLoad, "Load.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzleGameStatus);
	puzzleAddBackgroundImage(kPuzzleGameStatus, "GameStat.bmp", Common::Point(0, 16), true);

	// Menu objects
	objectAdd(kObjectMenuNewGame, "", "", 1);
	objectAddPuzzleAccessibility(kObjectMenuNewGame, kPuzzleMenu, Common::Rect(148, 69, 500, 99), true, 57, 0);
	objectAddPresentation(kObjectMenuNewGame);
	objectPresentationAddImageToPuzzle(kObjectMenuNewGame, 0, kPuzzleMenu, "gm_new.bmp", Common::Point(148, 85), true, 1, 1000);
	objectAdd(kObjectMenuPreferences, "", "", 1);
	objectAddPuzzleAccessibility(kObjectMenuPreferences, kPuzzleMenu, Common::Rect(148, 105, 500, 135), true, 57, 0);
	objectAddPresentation(kObjectMenuPreferences);
	objectPresentationAddImageToPuzzle(kObjectMenuPreferences, 0, kPuzzleMenu, "gm_pre.bmp", Common::Point(148, 121), true, 1, 1000);
	objectAdd(kObjectMenuLoad, "", "", 1);
	objectAddPuzzleAccessibility(kObjectMenuLoad, kPuzzleMenu, Common::Rect(148, 168, 500, 198), true, 57, 0);
	objectAddPresentation(kObjectMenuLoad);
	objectPresentationAddImageToPuzzle(kObjectMenuLoad, 0, kPuzzleMenu, "gm_loa.bmp", Common::Point(148, 184), true, 1, 1000);
	objectAdd(kObjectMenuSave, "", "", 1);
	objectAddPuzzleAccessibility(kObjectMenuSave, kPuzzleMenu, Common::Rect(148, 236, 500, 266), true, 57, 0);
	objectAddPresentation(kObjectMenuSave);
	objectPresentationAddImageToPuzzle(kObjectMenuSave, 0, kPuzzleMenu, "gm_sav.bmp", Common::Point(148, 252), true, 1, 1000);
	objectAdd(kObjectMenuContinue, "", "", 1);
	objectAddPuzzleAccessibility(kObjectMenuContinue, kPuzzleMenu, Common::Rect(148, 303, 500, 333), true, 57, 0);
	objectAddPresentation(kObjectMenuContinue);
	objectPresentationAddImageToPuzzle(kObjectMenuContinue, 0, kPuzzleMenu, "gm_con.bmp", Common::Point(148, 319), true, 1, 1000);
	objectAdd(kObjectMenuStatus, "", "", 1);
	objectAddPuzzleAccessibility(kObjectMenuStatus, kPuzzleMenu, Common::Rect(148, 342, 500, 372), true, 57, 0);
	objectAddPresentation(kObjectMenuStatus);
	objectPresentationAddImageToPuzzle(kObjectMenuStatus, 0, kPuzzleMenu, "gm_sta.bmp", Common::Point(148, 358), true, 1, 1000);
	objectAdd(kObjectMenuExit, "", "", 1);
	objectAddPuzzleAccessibility(kObjectMenuExit, kPuzzleMenu, Common::Rect(148, 380, 500, 410), true, 57, 0);
	objectAddPresentation(kObjectMenuExit);
	objectPresentationAddImageToPuzzle(kObjectMenuExit, 0, kPuzzleMenu, "gm_exi.bmp", Common::Point(148, 396), true, 1, 1000);

	// Preferences objects
	objectAdd(kObjectPreferencesCancel, "", "", 1);
	objectAddPuzzleAccessibility(kObjectPreferencesCancel, kPuzzlePreferences, Common::Rect(410, 420, 490, 445), true, 57, 0);
	objectAddPresentation(kObjectPreferencesCancel);
	objectPresentationAddImageToPuzzle(kObjectPreferencesCancel, 0, kPuzzlePreferences, "g_cancel.tga", Common::Point(407, 421), true, 3, 1000);
	objectSetPuzzleAccessibilityKey(kObjectPreferencesCancel, 0, Common::KEYCODE_ESCAPE);
	objectAdd(kObjectPreferencesOk, "", "", 1);
	objectAddPuzzleAccessibility(kObjectPreferencesOk, kPuzzlePreferences, Common::Rect(320, 420, 370, 445), true, 57, 0);
	objectAddPresentation(kObjectPreferencesOk);
	objectPresentationAddImageToPuzzle(kObjectPreferencesOk, 0, kPuzzlePreferences, "g_ok.tga", Common::Point(328, 421), true, 3, 1000);
	objectSetPuzzleAccessibilityKey(kObjectPreferencesOk, 0, Common::KEYCODE_RETURN);
	objectAdd(kObjectPreferencesSubtitles, "", "", 1);
	objectAddPuzzleAccessibility(kObjectPreferencesSubtitles, kPuzzlePreferences, Common::Rect(310, 315, 370, 350), true, 57, 0);
	objectAddPuzzleAccessibility(kObjectPreferencesSubtitles, kPuzzlePreferences, Common::Rect(400, 315, 460, 350), true, 57, 1);
	objectAddPresentation(kObjectPreferencesSubtitles);
	objectPresentationAddImageToPuzzle(kObjectPreferencesSubtitles, 0, kPuzzlePreferences, "pr_on.bmp", Common::Point(317, 326), true, 1, 1000);
	objectAddPresentation(kObjectPreferencesSubtitles);
	objectPresentationAddImageToPuzzle(kObjectPreferencesSubtitles, 1, kPuzzlePreferences, "pr_off.bmp", Common::Point(402, 326), true, 1, 1000);
	objectAdd(kObjectPreferences3dSound, "", "", 1);
	objectAddPuzzleAccessibility(kObjectPreferences3dSound, kPuzzlePreferences, Common::Rect(355, 260, 420, 295), true, 57, 1);
	objectAddPresentation(kObjectPreferences3dSound);
	objectPresentationAddImageToPuzzle(kObjectPreferences3dSound, 0, kPuzzlePreferences, "pr_left.tga", Common::Point(336, 288), true, 3, 1000);
	objectPresentationAddImageToPuzzle(kObjectPreferences3dSound, 0, kPuzzlePreferences, "pr_right.tga", Common::Point(428, 288), true, 3, 1000);
	objectAddPresentation(kObjectPreferences3dSound);
	objectPresentationAddImageToPuzzle(kObjectPreferences3dSound, 1, kPuzzlePreferences, "pr_right.tga", Common::Point(336, 288), true, 3, 1000);
	objectPresentationAddImageToPuzzle(kObjectPreferences3dSound, 1, kPuzzlePreferences, "pr_left.tga", Common::Point(428, 288), true, 3, 1000);
	objectAddPresentation(kObjectPreferences3dSound);
	objectPresentationAddImageToPuzzle(kObjectPreferences3dSound, 2, kPuzzlePreferences, "pr_3ds.tga", Common::Point(356, 281), true, 3, 1000);
	objectAdd(kObjectPreferencesSliderVolume, "", "ni_handsel", 4);
	objectAddPuzzleAccessibility(kObjectPreferencesSliderVolume, kPuzzlePreferences, Common::Rect(300, 140, 600, 180), true, 57, 1);
	objectAddPresentation(kObjectPreferencesSliderVolume);
	objectPresentationAddImageToPuzzle(kObjectPreferencesSliderVolume, 0, kPuzzlePreferences, "pr_slider.tga", Common::Point(314, 155), true, 3, 1000);
	objectPresentationShow(kObjectPreferencesSliderVolume);
	objectSetActiveDrawCursor(kObjectPreferencesSliderVolume, 15, 15, 0, 3, 0, 0, 3);
	objectSetPassiveDrawCursor(kObjectPreferencesSliderVolume, 15, 15, 0, 3, 0, 0, 3);
	objectAdd(kObjectPreferencesSliderDialog, "", "ni_handsel", 4);
	objectAddPuzzleAccessibility(kObjectPreferencesSliderDialog, kPuzzlePreferences, Common::Rect(300, 197, 600, 237), true, 57, 1);
	objectAddPresentation(kObjectPreferencesSliderDialog);
	objectPresentationAddImageToPuzzle(kObjectPreferencesSliderDialog, 0, kPuzzlePreferences, "pr_slider.tga", Common::Point(314, 212), true, 3, 1000);
	objectPresentationShow(kObjectPreferencesSliderDialog);
	objectSetActiveDrawCursor(kObjectPreferencesSliderDialog, 15, 15, 0, 3, 0, 0, 3);
	objectSetPassiveDrawCursor(kObjectPreferencesSliderDialog, 15, 15, 0, 3, 0, 0, 3);
	objectAdd(kObject90107, "", "", 1);
	objectAddPuzzleAccessibility(kObject90107, kPuzzlePreferences, Common::Rect(0, 448, 20, 640), true, 57, 1);

	// Loading screen objects
	objectAdd(kObjectLoadOk, "", "", 1);
	objectAddPuzzleAccessibility(kObjectLoadOk, kPuzzleLoad, Common::Rect(325, 418, 375, 461), true, 57, 0);
	objectAddPresentation(kObjectLoadOk);
	objectPresentationAddImageToPuzzle(kObjectLoadOk, 0, kPuzzleLoad, "g_ok.tga", Common::Point(328, 421), true, 3, 1000);
	objectSetPuzzleAccessibilityKey(kObjectLoadOk, 0, Common::KEYCODE_RETURN);
	objectAdd(kObjectLoadCancel, "", "", 1);
	objectAddPuzzleAccessibility(kObjectLoadCancel, kPuzzleLoad, Common::Rect(416, 418, 498, 461), true, 57, 0);
	objectAddPresentation(kObjectLoadCancel);
	objectPresentationAddImageToPuzzle(kObjectLoadCancel, 0, kPuzzleLoad, "g_cancel.tga", Common::Point(407, 421), true, 3, 1000);
	objectSetPuzzleAccessibilityKey(kObjectLoadCancel, 0, Common::KEYCODE_ESCAPE);

	// Saving screen objects
	objectAdd(kObjectSaveOk, "", "", 1);
	objectAddPuzzleAccessibility(kObjectSaveOk, kPuzzleSave, Common::Rect(325, 418, 375, 461), true, 57, 0);
	objectAdd(kObjectSaveCancel, "", "", 1);
	objectAddPuzzleAccessibility(kObjectSaveCancel, kPuzzleSave, Common::Rect(416, 418, 498, 461), true, 57, 0);
	objectSetPuzzleAccessibilityKey(kObjectSaveCancel, 0, Common::KEYCODE_ESCAPE);
	objectAddPresentation(kObjectSaveOk);
	objectPresentationAddImageToPuzzle(kObjectSaveOk, 0, kPuzzleSave, "g_ok.tga", Common::Point(328, 421), true, 3, 1000);
	objectAddPresentation(kObjectSaveCancel);
	objectPresentationAddImageToPuzzle(kObjectSaveCancel, 0, kPuzzleSave, "g_cancel.tga", Common::Point(407, 421), true, 3, 1000);
	objectSetPuzzleAccessibilityKey(kObjectSaveOk, 0, Common::KEYCODE_RETURN);
	objectAdd(kObjectSave90313, "", "", 1);
	objectAddPresentation(kObjectSave90313);
	objectPresentationAddTextToPuzzle(kObjectSave90313, 0, kPuzzleSave, "", 344, 181, kFontDefault, 255, 95, 0, -1, -1, -1);
	objectPresentationAddTextToPuzzle(kObjectSave90313, 0, kPuzzleSave, "", 0, 0, kFontDefault, 255, 95, 0, -1, -1, -1);
	objectPresentationAddAnimationToPuzzle(kObjectSave90313, 0, kPuzzleSave, "kybcur", 0, Common::Point(0, 0), 1, 1000, 6, 12.5f, 16);
	objectPresentationSetAnimationCoordinatesOnPuzzle(kObjectSave90313, 0, Common::Point(346, 181));

	_archiveType = kArchiveFile;
	objectPresentationAddImageToPuzzle(kObjectSave90313, 0, kPuzzleSave, "osc.bmp", Common::Point(0, 0), true, 1, 1000);
	if ( _configuration.artSY )
		_archiveType = kArchiveArt;

	objectPresentationShow(kObjectSave90313, 0);

	visualListAddToPuzzle(1, kPuzzleLoad, 65,
	                      "", "" /* save folder */, "", "up_gun.tga", "up_gur.tga", "", "up_gua.tga", "down_gun.tga", "down_gur.tga", "",
	                      "down_gua.tga", "load_gun.tga", "load_gua.tga",
	                      3, 0, 0, 0, 0, 335, 127, 300, 35, 45,
	                      3, 330, 349, 320, 339, 40, 40, 330, 380, 320,
	                      370, 40, 40, 0, 0, 0, 1, 311, 137, 4,
	                      255, 95, 0, 245, 235, 50, -1, -1,-1, kFontDefault,
	                      kArchiveFile);

	// Game status screen
	objectAdd(kObjectStatusOk, "", "", 1);
	objectAddPuzzleAccessibility(kObjectStatusOk, kPuzzleGameStatus, Common::Rect(28, 79, 107, 109), true, 57, 0);
	objectAddPresentation(kObjectStatusOk);
	objectPresentationAddImageToPuzzle(kObjectStatusOk, 0, kPuzzleGameStatus, "g_ok.tga", Common::Point(46, 95), true, 3, 1000);
	objectSetPuzzleAccessibilityKey(kObjectStatusOk, 0, Common::KEYCODE_RETURN);
	visualAddShowToPuzzle(2, kPuzzleGameStatus, 1, 4, 295, 343, 28, 4, 300, 38655);
	objectAdd(kObjectStatusProgress, "", "", 1);
	objectAddPresentation(kObjectStatusProgress);
	objectPresentationAddTextToPuzzle(kObjectStatusProgress, 0, kPuzzleGameStatus, "", 600, 327, kFontDefault, 255, -106, 0, -1, -1, -1);
	objectPresentationAddTextToPuzzle(kObjectStatusProgress, 0, kPuzzleGameStatus, "", 600, 356, kFontDefault, 255, -106, 0, -1, -1, -1);
	objectPresentationAddTextToPuzzle(kObjectStatusProgress, 0, kPuzzleGameStatus, "", 600, 384, kFontDefault, 255, -106, 0, -1, -1, -1);
	objectPresentationAddTextToPuzzle(kObjectStatusProgress, 0, kPuzzleGameStatus, "", 600, 410, kFontDefault, 255, -106, 0, -1, -1, -1);
	objectPresentationShow(kObjectStatusProgress);

	// Sounds
	soundAdd(90001, kSoundTypeDialog, "1844.wac", kLoadFromDisk, 2, _configuration.dialog.soundChunck);

	// Variables
	varDefineByte(90001, 0);
	varDefineByte(90002, 0);
	varDefineByte(90003, 0);
	varDefineByte(90004, 0);
	varDefineFloat(90005, 0);
	varDefineFloat(90006, 0);
	varDefineFloat(90007, 0);
	varDefineFloat(90008, 0);
	varDefineByte(90009, 0);
	varDefineByte(90010, 0);
	varDefineByte(90011, 0);
	varDefineByte(90012, 0);
	varDefineDword(90013, 0);
	varDefineDword(90014, 0);
	varDefineDword(90015, 0);
	varDefineDword(90016, 0);
	varDefineByte(90017, 0);
	varDefineByte(90018, 0);
	varDefineByte(90019, 0);
	varDefineByte(90020, 0);
	varDefineDword(90021, 0);
	varDefineDword(90022, 0);
	varDefineDword(90023, 0);
	varDefineDword(90024, 0);
	varDefineByte(90025, 0);
	varDefineByte(90026, 0);
	varDefineByte(90027, 0);
	varDefineByte(90028, 0);
}

void ApplicationRing::initZoneNI() {
	debugC(1, kRingDebugLogic, "Initializing Alberich zone (NI)...");

	setZoneAndEnableBag(kZoneNI);

	rotationAdd(10000, "NIS00N01", 0, 2);
	rotationAdd(10001, "NIS00N03", 0, 2);
	rotationAdd(10002, "NIS00N04", 0, 2);
	rotationSetComBufferLength(10002, 1300000);
	rotationAdd(10003, "NIS00N05", 0, 2);
	rotationAdd(10004, "NIS00N06", 0, 2);
	rotationAdd(10005, "NIS00N07", 0, 2);
	rotationAdd(10101, "NIS01N01", 0, 1);
	rotationSetComBufferLength(10101, 1300000);
	rotationAdd(10102, "NIS01N02", 0, 1);
	rotationAdd(10201, "NIS02N01", 0, 1);
	rotationAdd(10301, "NIS03N01", 0, 1);
	rotationAdd(10401, "NIS04N01", 0, 2);
	rotationAdd(10402, "NIS04N02", 0, 2);
	rotationSetComBufferLength(10402, 1300000);
	rotationAdd(10403, "NIS04N03", 0, 0);
	rotationAdd(10404, "NIS04N04", 0, 0);
	rotationAdd(10405, "NIS04N05", 0, 0);
	rotationAdd(10415, "NIS04N15", 0, 0);
	rotationAdd(10406, "NIS04N06", 0, 0);
	rotationSetJugOn(10406, 10.0f, 1.0);
	rotationAdd(10501, "NIS05N01", 0, 0);
	rotationAdd(10601, "NIS06N01", 0, 0);
	puzzleAdd(kPuzzle12001);
	puzzleAddBackgroundImage(kPuzzle12001, "TR_NI_RH_BP01.BMP", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle12002);
	puzzleAddBackgroundImage(kPuzzle12002, "TR_NI_RH_BP02.BMP", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle12003);
	puzzleAddBackgroundImage(kPuzzle12003, "TR_NI_RH_BP03.BMP", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle10000);
	puzzleAddBackgroundImage(kPuzzle10000, "NIS00N01P01.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle10001);
	puzzleAddBackgroundImage(kPuzzle10001, "NIS00N01P01L01.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle10002);
	puzzleAddBackgroundImage(kPuzzle10002, "NIS00N01P01L02.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle10100);
	puzzleAddBackgroundImage(kPuzzle10100, "NIS01N01P01.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle10102);
	puzzleAddBackgroundImage(kPuzzle10102, "NIS01N01P03.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle10200);
	puzzleAddBackgroundImage(kPuzzle10200, "NIS02N01P01.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle10201);
	puzzleAddBackgroundImage(kPuzzle10201, "NIS02N01P02.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle10202);
	puzzleAddBackgroundImage(kPuzzle10202, "NIS02N01P03S01.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle10203);
	puzzleAddBackgroundImage(kPuzzle10203, "NIS02N01P02S01.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle10204);
	puzzleAddBackgroundImage(kPuzzle10204, "NIS02N01P02S02.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle10205);
	puzzleAddBackgroundImage(kPuzzle10205, "NIS02N01P02S03.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle10391);
	puzzleAddBackgroundImage(kPuzzle10391, "NIS03N01P01S01.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle10390);
	puzzleAddBackgroundImage(kPuzzle10390, "NIS03N01P01S02.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle10392);
	puzzleAddBackgroundImage(kPuzzle10392, "NIS03N01P03S01.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle10300);
	puzzleAddBackgroundImage(kPuzzle10300, "NIS03N01P02.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle10400);
	puzzleAddBackgroundImage(kPuzzle10400, "NIS04N02P01L01.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle10401);
	puzzleAddBackgroundImage(kPuzzle10401, "NIS04N02P02L01.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle10410);
	puzzleAddBackgroundImage(kPuzzle10410, "NIS04N03P01.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle10411);
	puzzleAddBackgroundImage(kPuzzle10411, "NIS04N03P03.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle10500);
	puzzleAddBackgroundImage(kPuzzle10500, "NIS05N01P01.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle10501);
	puzzleAddBackgroundImage(kPuzzle10501, "NIS05N01P01L01.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle10510);
	puzzleAddBackgroundImage(kPuzzle10510, "NIS05N01P02.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle10511);
	puzzleAddBackgroundImage(kPuzzle10511, "NIS05N01P02L01.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle10520);
	puzzleAddBackgroundImage(kPuzzle10520, "NIS05N01P03.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle10521);
	puzzleAddBackgroundImage(kPuzzle10521, "NIS05N01P03L01.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle10600);
	puzzleAddBackgroundImage(kPuzzle10600, "NIS06N01P01.0001.bmp", Common::Point(0, 16), true);
	rotationAddMovabilityToRotation(10000, 10001, "1551", Common::Rect(1211, -137, 1494, 151), false, 53, 1);
	rotationSetMovabilityToRotation(10000, 0, 90.0f, 0.3f, 85.7f, 0, 0, 90.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(10000, 10401, "1552", Common::Rect(2847, -364, 3394, 97), false, 53, 41);
	rotationSetMovabilityToRotation(10000, 1, 270.0f, 0.3f, 85.7f, 0, 0, 180.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(10000, 10201, "1553", Common::Rect(300, -244, 1000, 209), false, 53, 21);
	rotationSetMovabilityToRotation(10000, 2, 90.0f, 0.3f, 85.7f, 0, 0, 270.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(10001, 10002, "1554", Common::Rect(1239, -116, 1485, 127), false, 53, 1);
	rotationSetMovabilityToRotation(10001, 0, 90.0f, 0.3f, 85.7f, 0, 0, 90.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(10001, 10000, "1555", Common::Rect(2998, -212, 3315, 163), false, 53, 2);
	rotationSetMovabilityToRotation(10001, 1, 270.0f, 0.3f, 85.7f, 0, 0, 270.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(10002, 10004, "1556", Common::Rect(1212, -132, 1500, 190), false, 53, 1);
	rotationSetMovabilityToRotation(10002, 0, 90.0f, 0.3f, 85.7f, 0, 0, 90.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(10002, 10001, "1557", Common::Rect(2951, -167, 3327, 218), false, 53, 2);
	rotationSetMovabilityToRotation(10002, 1, 270.0f, 0.3f, 85.7f, 0, 0, 270.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(10002, 10003, "1558", Common::Rect(1600, -214, 1923, 169), false, 53, 5);
	rotationSetMovabilityToRotation(10002, 2, 90.0f, 0.3f, 85.7f, 0, 0, 90.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(10003, 10002, "1559", Common::Rect(0, -156, 492, 219), false, 53, 2);
	rotationSetMovabilityToRotation(10003, 0, 328.3f, 0.3f, 85.7f, 0, 0, 270.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(10003, 10301, "1560", Common::Rect(2141, -190, 2666, 265), true, 53, 110);
	rotationSetMovabilityToRotation(10003, 1, 195.0f, 0.3f, 85.7f, 0, 2, 150.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(10004, 10005, "1561", Common::Rect(1263, -47, 1610, 221), false, 53, 1);
	rotationSetMovabilityToRotation(10004, 0, 90.0f, 0.3f, 85.7f, 0, 0, 90.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(10004, 10002, "1562", Common::Rect(3009, -146, 3301, 160), false, 53, 2);
	rotationSetMovabilityToRotation(10004, 1, 270.0f, 0.3f, 85.7f, 0, 0, 270.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(10005, 10101, "1563", Common::Rect(1121, -195, 1606, 331), false, 53, 4);
	rotationSetMovabilityToRotation(10005, 0, 90.0f, 0.3f, 85.7f, 0, 2, 270.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(10005, 10004, "1564", Common::Rect(3009, -319, 3487, 172), false, 53, 2);
	rotationSetMovabilityToRotation(10005, 1, 270.0f, 0.3f, 85.7f, 0, 0, 270.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(10101, 10102, "1565", Common::Rect(1186, -143, 1534, 195), true, 53, 0);
	rotationSetMovabilityToRotation(10101, 0, 90.0f, 0.3f, 85.7f, 0, 2, 90.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(10102, 10101, "1566", Common::Rect(2951, -125, 3350, 193), true, 53, 0);
	rotationSetMovabilityToRotation(10102, 0, 270.0f, 0.3f, 85.7f, 0, 2, 270.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(10102, 10601, "1567", Common::Rect(1323, -57, 1587, 225), true, 53, 16);
	rotationSetMovabilityToRotation(10102, 1, 90.0f, 0.3f, 85.7f, 0, 2, 130.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(10102, 10601, "1568", Common::Rect(1323, -57, 1587, 225), false, 53, 0);
	rotationSetMovabilityToRotation(10102, 2, 90.0f, 0.3f, 85.7f, 0, 2, 224.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(10201, 10000, "1509", Common::Rect(1947, -158, 2418, 301), false, 53, 2);
	rotationSetMovabilityToRotation(10201, 0, 190.1f, 0.3f, 85.7f, 0, 0, 270.0f, 0.3f, 85.7f);
	rotationAddMovabilityToPuzzle(10201, kPuzzle10200, "1569", Common::Rect(198, -34, 495, 366), true, 52, 110);
	rotationSetMovabilityToPuzzle(10201, 1, 345.0f, 15.0f, 80.0f, 0, 2);
	rotationAddMovabilityToRotation(10301, 10003, "1570", Common::Rect(318, -202, 789, 226), true, 53, 100);
	rotationSetMovabilityToRotation(10301, 0, 12.0f, 0.3f, 85.7f, 0, 2, 90.0f, 0.3f, 85.7f);
	rotationAddMovabilityToPuzzle(10301, kPuzzle10300, "", Common::Rect(2272, -8, 2418, 144), true, 53, 0);
	rotationSetMovabilityToPuzzle(10301, 1, 190.0f, 0.3f, 85.7f, 0, 0);
	rotationAddMovabilityToRotation(10401, 10000, "1571", Common::Rect(1123, -153, 1599, 279), false, 53, 1);
	rotationSetMovabilityToRotation(10401, 0, 90.0f, 0.3f, 85.7f, 0, 0, 90.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(10401, 10402, "1572", Common::Rect(2953, -223, 3359, 155), true, 53, 110);
	rotationSetMovabilityToRotation(10401, 1, 270.0f, 0.3f, 85.7f, 0, 2, 270.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(10402, 10401, "1573", Common::Rect(1081, 13, 1657, 286), true, 53, 100);
	rotationSetMovabilityToRotation(10402, 0, 90.0f, 0.3f, 85.7f, 0, 2, 180.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(10402, 10403, "1574", Common::Rect(3002, -139, 3306, 190), true, 53, 0);
	rotationSetMovabilityToRotation(10402, 1, 270.0f, 0.3f, 85.7f, 0, 2, 270.0f, 0.3f, 85.7f);
	rotationAddMovabilityToPuzzle(10402, kPuzzle10400, "", Common::Rect(2213, 87, 2371, 202), true, 52, 41);
	rotationSetMovabilityToPuzzle(10402, 2, 185.0f, 15.0f, 85.7f, 0, 2);
	rotationAddMovabilityToPuzzle(10402, kPuzzle10401, "", Common::Rect(326, 87, 485, 198), true, 52, 42);
	rotationSetMovabilityToPuzzle(10402, 3, 0, 15.0f, 85.7f, 0, 2);
	rotationAddMovabilityToRotation(10403, 10402, "1575", Common::Rect(1146, -94, 1560, 221), true, 53, 0);
	rotationSetMovabilityToRotation(10403, 0, 90.0f, 0.3f, 85.7f, 0, 2, 90.0f, 0.3f, 85.7f);
	rotationAddMovabilityToPuzzle(10403, kPuzzle10410, "", Common::Rect(3041, -265, 3290, 0), true, 52, 0);
	rotationSetMovabilityToPuzzle(10403, 1, 270.0f, 0.3f, 85.7f, 0, 2);
	rotationAddMovabilityToRotation(10404, 10403, "1576", Common::Rect(207, 249, 564, 423), true, 52, 0);
	rotationSetMovabilityToRotation(10404, 0, 90.0f, 0, 85.7f, 0, 2, 90.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(10405, 10404, "1577", Common::Rect(1165, -221, 1527, 284), true, 53, 0);
	rotationSetMovabilityToRotation(10405, 0, 90.0f, 0.3f, 85.7f, 0, 2, 90.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(10415, 10404, "1577", Common::Rect(1165, -221, 1527, 284), true, 53, 0);
	rotationSetMovabilityToRotation(10415, 0, 90.0f, 0.3f, 85.7f, 0, 2, 90.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(10501, 10601, "1579", Common::Rect(546, -345, 980, 120), true, 53, 0);
	rotationSetMovabilityToRotation(10501, 0, 34.0f, 0.3f, 85.7f, 0, 2, 44.0f, 0.3f, 85.7f);
	rotationAddMovabilityToPuzzle(10501, kPuzzle10500, "", Common::Rect(1139, -204, 1520, 156), true, 52, 0);
	rotationSetMovabilityToPuzzle(10501, 1, 90.0f, 0.3f, 85.7f, 0, 2);
	rotationAddMovabilityToPuzzle(10501, kPuzzle10510, "", Common::Rect(1787, -143, 2033, 108), true, 52, 0);
	rotationSetMovabilityToPuzzle(10501, 2, 145.0f, 0.3f, 85.7f, 0, 2);
	rotationAddMovabilityToPuzzle(10501, kPuzzle10520, "", Common::Rect(2228, -118, 2411, 80), true, 52, 0);
	rotationSetMovabilityToPuzzle(10501, 3, 185.0f, 0.3f, 85.7f, 0, 2);
	rotationAddMovabilityToRotation(10601, 10102, "1580", Common::Rect(863, -211, 1123, 101), true, 53, 0);
	rotationSetMovabilityToRotation(10601, 0, 44.0f, 0.3f, 85.7f, 0, 2, 270.0f, 0.3f, 85.7f);
	rotationAddMovabilityToPuzzle(10601, kPuzzle10600, "", Common::Rect(2436, -256, 2951, 284), true, 52, 61);
	rotationSetMovabilityToPuzzle(10601, 1, 224.4, 0.3f, 85.7f, 0, 0);
	puzzleAddMovabilityToPuzzle(kPuzzle10102, kPuzzle10100, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleAddMovabilityToRotation(kPuzzle10200, 10201, "1549", Common::Rect(0, 420, 640, 464), true, 55, 100);
	puzzleSetMovabilityToRotation(kPuzzle10200, 0, 345.0f, 0.3f, 85.7f);
	puzzleAddMovabilityToRotation(kPuzzle10300, 10301, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle10300, 0, 190.0f, 0.3f, 85.7f);
	puzzleAddMovabilityToRotation(kPuzzle10400, 10402, "", Common::Rect(0, 420, 640, 464), true, 55, 41);
	puzzleSetMovabilityToRotation(kPuzzle10400, 0, 190.0f, 0.3f, 85.7f);
	puzzleAddMovabilityToRotation(kPuzzle10401, 10402, "", Common::Rect(0, 420, 640, 464), true, 55, 42);
	puzzleSetMovabilityToRotation(kPuzzle10401, 0, 10.0f, 0.3f, 85.7f);
	puzzleAddMovabilityToPuzzle(kPuzzle10410, kPuzzle10411, "", Common::Rect(250, 280, 320, 370), false, 52, 0);
	puzzleAddMovabilityToRotation(kPuzzle10410, 10403, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle10410, 1, 270.0f, 0.3f, 85.7f);
	puzzleAddMovabilityToRotation(kPuzzle10410, 10404, "1581", Common::Rect(455, 185, 534, 230), true, 52, 0);
	puzzleSetMovabilityToRotation(kPuzzle10410, 2, 270.0f, 0.3f, 85.7f);
	puzzleAddMovabilityToPuzzle(kPuzzle10411, kPuzzle10410, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleAddMovabilityToRotation(kPuzzle10500, 10501, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle10500, 0, 90.0f, 0.3f, 85.7f);
	puzzleAddMovabilityToPuzzle(kPuzzle10500, kPuzzle10501, "", Common::Rect(200, 100, 400, 300), true, 52, 0);
	puzzleAddMovabilityToPuzzle(kPuzzle10501, kPuzzle10500, "", Common::Rect(430, 48, 640, 434), true, 52, 0);
	puzzleAddMovabilityToRotation(kPuzzle10510, 10501, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle10510, 0, 144.0f, 0.3f, 85.7f);
	puzzleAddMovabilityToPuzzle(kPuzzle10510, kPuzzle10511, "", Common::Rect(200, 100, 400, 300), true, 52, 0);
	puzzleAddMovabilityToPuzzle(kPuzzle10511, kPuzzle10510, "", Common::Rect(430, 48, 640, 434), true, 52, 0);
	puzzleAddMovabilityToRotation(kPuzzle10520, 10501, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle10520, 0, 184.0f, 0.3f, 85.7f);
	puzzleAddMovabilityToPuzzle(kPuzzle10520, kPuzzle10521, "", Common::Rect(200, 100, 400, 300), true, 52, 0);
	puzzleAddMovabilityToPuzzle(kPuzzle10521, kPuzzle10520, "", Common::Rect(430, 48, 640, 434), true, 52, 0);
	puzzleAddMovabilityToPuzzle(kPuzzle10521, kPuzzle10521, "1582", Common::Rect(216, 124, 406, 327), true, 52, 55);
	objectAdd(kObject12000, "", "", 1);
	objectAddPresentation(kObject12000);
	objectPresentationAddAnimationToPuzzle(kObject12000, 0, kPuzzle12001, "TR_NI_RH_BP01S01", 0, Common::Point(239, 208), 1, 1000, 20, 12.5f, 4);
	objectAddPresentation(kObject12000);
	objectPresentationAddAnimationToPuzzle(kObject12000, 1, kPuzzle12002, "TR_NI_RH_BP02S01", 0, Common::Point(187, 94), 1, 1000, 20, 12.5f, 4);
	objectAddPresentation(kObject12000);
	objectPresentationAddAnimationToPuzzle(kObject12000, 2, kPuzzle12003, "TR_NI_RH_BP03S01", 0, Common::Point(207, 134), 1, 1000, 20, 12.5f, 4);
	objectAdd(kObject10003, "Bike", "", 1);
	objectAddPresentation(kObject10003);
	objectPresentationAddImageToRotation(kObject10003, 0, 10003, 1);
	objectAddPresentation(kObject10003);
	objectPresentationAddImageToRotation(kObject10003, 1, 10201, 0);
	objectAddPresentation(kObject10003);
	objectAddPresentation(kObject10003);
	objectPresentationAddImageToRotation(kObject10003, 3, 10000, 0);
	objectAddPresentation(kObject10003);
	objectPresentationAddImageToRotation(kObject10003, 4, 10001, 0);
	objectAddPresentation(kObject10003);
	objectPresentationAddImageToRotation(kObject10003, 5, 10002, 0);
	objectAddPresentation(kObject10003);
	objectPresentationAddImageToRotation(kObject10003, 6, 10004, 0);
	objectAddPresentation(kObject10003);
	objectPresentationAddImageToRotation(kObject10003, 7, 10005, 0);
	objectAddPresentation(kObject10003);
	objectPresentationAddImageToRotation(kObject10003, 8, 10000, 1);
	objectAddPresentation(kObject10003);
	objectPresentationAddImageToRotation(kObject10003, 9, 10001, 1);
	objectAddPresentation(kObject10003);
	objectPresentationAddImageToRotation(kObject10003, 10, 10002, 1);
	objectAddPresentation(kObject10003);
	objectPresentationAddImageToRotation(kObject10003, 11, 10004, 1);
	objectAddPresentation(kObject10003);
	objectPresentationAddImageToRotation(kObject10003, 12, 10005, 1);
	objectPresentationShow(kObject10003, 0);
	objectPresentationShow(kObject10003, 1);
	objectAdd(kObjectBrutality, "SP/Brutality", "NI_Brutality", 1);
	objectAddBagAnimation(kObjectBrutality, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectBrutality, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectBrutality, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObjectMinerals, "Minerals", "NI_Minerals", 1);
	objectAddBagAnimation(kObjectMinerals, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectMinerals, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectMinerals, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObjectGlug, "Glug", "NI_Glug", 1);
	objectAddBagAnimation(kObjectGlug, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectGlug, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectGlug, 22, 22, 0, 3, 0, 0, 3);
	objectAddPuzzleAccessibility(kObjectGlug, kPuzzle10000, Common::Rect(227, 206, 410, 394), true, 52, 0);
	objectAddPuzzleAccessibility(kObjectGlug, kPuzzle10001, Common::Rect(48, 40, 567, 416), true, 52, 1);
	objectAddPuzzleAccessibility(kObjectGlug, kPuzzle10002, Common::Rect(246, 188, 383, 322), true, 52, 2);
	objectAddRotationAccessibility(kObjectGlug, 10003, Common::Rect(1089, 390, 1664, 680), true, 52, 3);
	objectAddRotationAccessibility(kObjectGlug, 10401, Common::Rect(1963, 390, 2615, 680), true, 52, 3);
	objectAddRotationAccessibility(kObjectGlug, 10201, Common::Rect(2830, 390, 3505, 680), true, 52, 3);
	objectAddPuzzleAccessibility(kObjectGlug, kPuzzle10000, Common::Rect(0, 420, 640, 464), true, 55, 4);
	objectAddPuzzleAccessibility(kObjectGlug, kPuzzle10001, Common::Rect(0, 420, 640, 464), true, 55, 4);
	objectAddPuzzleAccessibility(kObjectGlug, kPuzzle10002, Common::Rect(0, 420, 640, 464), true, 55, 4);
	objectAddPresentation(kObjectGlug);
	objectPresentationAddAnimationToPuzzle(kObjectGlug, 0, kPuzzle10002, "NIS00N01P01S01", 0, Common::Point(203, 191), 1, 1000, 5, 12.5f, 4);
	objectPresentationSetAnimationOnPuzzle(kObjectGlug, 0, 0, kObjectBrutality);
	objectAdd(kObject10110, "", "", 1);
	objectAddPuzzleAccessibility(kObject10110, kPuzzle10100, Common::Rect(236, 79, 289, 118),  false, 52, 1);
	objectAddPuzzleAccessibility(kObject10110, kPuzzle10100, Common::Rect(171, 124, 229, 159), false, 52, 2);
	objectAddPuzzleAccessibility(kObject10110, kPuzzle10100, Common::Rect(272, 126, 342, 165), false, 52, 3);
	objectAddPuzzleAccessibility(kObject10110, kPuzzle10100, Common::Rect(195, 169, 259, 213), false, 52, 4);
	objectAddPuzzleAccessibility(kObject10110, kPuzzle10100, Common::Rect(107, 204, 188, 285), false, 52, 5);
	objectAdd(kObject10100, "Console", "", 1);
	objectAddPuzzleAccessibility(kObject10100, kPuzzle10100, Common::Rect(0, 16, 640, 66),    true, 53, 0);
	objectAddPuzzleAccessibility(kObject10100, kPuzzle10100, Common::Rect(0, 414, 640, 464),  true, 53, 0);
	objectAddPuzzleAccessibility(kObject10100, kPuzzle10100, Common::Rect(0, 66, 50, 414),    true, 53, 0);
	objectAddPuzzleAccessibility(kObject10100, kPuzzle10100, Common::Rect(610, 66, 640, 414), true, 53, 0);
	objectAddRotationAccessibility(kObject10100, 10101, Common::Rect(3406, 26, 3541, 155), true, 52, 1);
	objectAddPresentation(kObject10100);
	objectPresentationAddImageToPuzzle(kObject10100, 0, kPuzzle10100, "NIS01N01P01L01.0001.bmp", Common::Point(101, 68), true, 1, 1000);
	objectAddPresentation(kObject10100);
	objectPresentationAddImageToPuzzle(kObject10100, 1, kPuzzle10100, "NIS01N01P01L02.0001.bmp", Common::Point(80, 63), true, 1, 1000);
	objectAddPresentation(kObject10100);
	objectPresentationAddImageToPuzzle(kObject10100, 2, kPuzzle10100, "NIS01N01P01L03.0001.bmp", Common::Point(116, 159), true, 1, 1000);
	objectAdd(kObject10101, "CCButton1", "", 2);
	objectAddRotationAccessibility(kObject10101, 10101, Common::Rect(3208, 109, 3322, 184), true, 52, 1);
	objectAddPuzzleAccessibility(kObject10101, kPuzzle10100, Common::Rect(359, 265, 440, 304), false, 52, 0);
	objectAddPuzzleAccessibility(kObject10101, kPuzzle10100, Common::Rect(359, 265, 440, 304), false, 52, 1);
	objectAddPresentation(kObject10101);
	objectPresentationAddImageToPuzzle(kObject10101, 0, kPuzzle10100, "NIS01N01P01S03.0001.bmp", Common::Point(361, 259), true, 1, 1000);
	objectAddPresentation(kObject10101);
	objectPresentationAddAnimationToPuzzle(kObject10101, 1, kPuzzle10100, "NIS01N01P01S01", 0, Common::Point(85, 68), 1, 1000, 36, 12.5f, 6);
	objectPresentationSetAnimationOnPuzzle(kObject10101, 1, 0, kObject10101);
	objectAddPresentation(kObject10101);
	objectPresentationAddAnimationToPuzzle(kObject10101, 2, kPuzzle10100, "NIS01N01P01L02S01", 0, Common::Point(80, 63), 1, 1000, 36, 12.5f, 10);
	objectPresentationSetAnimationOnPuzzle(kObject10101, 2, 0, kObject10103);
	objectAddPresentation(kObject10101);
	objectPresentationAddAnimationToPuzzle(kObject10101, 3, kPuzzle10100, "NIS01N01P01S01", 0, Common::Point(85, 68), 1, 1000, 36, 12.5f, 10);
	objectPresentationSetAnimationOnPuzzle(kObject10101, 3, 0, kObject10100);
	objectAddPresentation(kObject10101);
	objectPresentationAddAnimationToPuzzle(kObject10101, 4, kPuzzle10100, "NIS01N01P01L02S01", 0, Common::Point(80, 63), 1, 1000, 36, 12.5f, 6);
	objectPresentationSetAnimationOnPuzzle(kObject10101, 4, 0, kObject10102);
	objectAddPresentation(kObject10101);
	objectPresentationAddImageToPuzzle(kObject10101, 5, kPuzzle10100, "NIS01N01P01S03.0003.bmp", Common::Point(361, 259), true, 1, 1000);
	objectAddPresentation(kObject10101);
	objectPresentationAddImageToRotation(kObject10101, 6, 10101, 0);
	objectPresentationAddImageToRotation(kObject10101, 6, 10102, 0);
	objectAdd(kObject10102, "CCButton2", "", 2);
	objectAddRotationAccessibility(kObject10102, 10101, Common::Rect(3250, 195, 3357, 245), true, 52, 1);
	objectAddPuzzleAccessibility(kObject10102, kPuzzle10100, Common::Rect(382, 308, 472, 352), true, 52, 0);
	objectAddPuzzleAccessibility(kObject10102, kPuzzle10100, Common::Rect(382, 308, 472, 352), true, 52, 1);
	objectAddPresentation(kObject10102);
	objectPresentationAddImageToPuzzle(kObject10102, 0, kPuzzle10100, "NIS01N01P01S03.0004.bmp", Common::Point(385, 299), true, 1, 1000);
	objectAddPresentation(kObject10102);
	objectPresentationAddAnimationToPuzzle(kObject10102, 1, kPuzzle10100, "NIS01N01P01S02", 0, Common::Point(40, 65), 1, 1000, 46, 12.5f, 6);
	objectPresentationSetAnimationOnPuzzle(kObject10102, 1, 0, kObject10105);
	objectAddPresentation(kObject10102);
	objectPresentationAddAnimationToPuzzle(kObject10102, 2, kPuzzle10100, "NIS01N01P01S02", 0, Common::Point(40, 65), 1, 1000, 23, 12.5f, 6);
	objectPresentationSetAnimationOnPuzzle(kObject10102, 2, 0, kObject10104);
	objectAddPresentation(kObject10102);
	objectPresentationAddAnimationToPuzzle(kObject10102, 3, kPuzzle10100, "NIS01N01P01S02", 0, Common::Point(40, 65), 1, 1000, 23, 12.5f, 10);
	objectPresentationSetAnimationOnPuzzle(kObject10102, 3, 0, kObject10106);
	objectAddPresentation(kObject10102);
	objectPresentationAddImageToPuzzle(kObject10102, 4, kPuzzle10100, "NIS01N01P01S03.0002.bmp", Common::Point(385, 299), true, 1, 1000);
	objectAdd(kObject10103, "CCHandle", "NI_HandSel", 4);
	objectAddPuzzleAccessibility(kObject10103, kPuzzle10100, Common::Rect(497, 192, 543, 249), true, 10000, 1);
	objectSetActiveDrawCursor(kObject10103, 15, 15, 0, 3, 0, 0, 3);
	objectSetPassiveDrawCursor(kObject10103, 15, 15, 0, 3, 0, 0, 3);

	for (uint32 i = 0; i < 14; i++) {
		objectAddPresentation(kObject10103);
		objectPresentationAddImageToPuzzle(kObject10103, i, kPuzzle10100, Common::String::format("NIS01N01P02S01.%04d.bmp", i + 1), Common::Point(504, 194), true, 1, 1000);
	}

	objectAdd(kObject10104, "CCTileHold", "", 1);
	objectAddPuzzleAccessibility(kObject10104, kPuzzle10100, Common::Rect(60, 175, 187, 320), false, 52, 0);
	objectAdd(kObject10109, "CCMosaicBtns", "", 3);
	objectAddPuzzleAccessibility(kObject10109, kPuzzle10100, Common::Rect(267, 257, 284, 272), false, 52, 0);
	objectAddPuzzleAccessibility(kObject10109, kPuzzle10100, Common::Rect(265, 205, 282, 220), false, 52, 1);
	objectAddPuzzleAccessibility(kObject10109, kPuzzle10100, Common::Rect(244, 220, 261, 235), false, 52, 2);
	objectAddPuzzleAccessibility(kObject10109, kPuzzle10100, Common::Rect(278, 220, 295, 235), false, 52, 3);
	objectAddPuzzleAccessibility(kObject10109, kPuzzle10100, Common::Rect(256, 235, 273, 250), false, 52, 4);
	objectAddPuzzleAccessibility(kObject10109, kPuzzle10100, Common::Rect(230, 257, 247, 272), false, 52, 5);
	objectAddPresentation(kObject10109);
	objectPresentationAddImageToPuzzle(kObject10109, 0, kPuzzle10100, "NIS01N01P01L02S02.0006.bmp", Common::Point(221, 196), true, 1, 1000);
	objectAddPresentation(kObject10109);
	objectPresentationAddImageToPuzzle(kObject10109, 1, kPuzzle10100, "NIS01N01P01L02S02.0001.bmp", Common::Point(221, 196), true, 1, 1000);
	objectAddPresentation(kObject10109);
	objectPresentationAddImageToPuzzle(kObject10109, 2, kPuzzle10100, "NIS01N01P01L02S02.0002.bmp", Common::Point(221, 196), true, 1, 1000);
	objectAddPresentation(kObject10109);
	objectPresentationAddImageToPuzzle(kObject10109, 3, kPuzzle10100, "NIS01N01P01L02S02.0003.bmp", Common::Point(221, 196), true, 1, 1000);
	objectAddPresentation(kObject10109);
	objectPresentationAddImageToPuzzle(kObject10109, 4, kPuzzle10100, "NIS01N01P01L02S02.0004.bmp", Common::Point(221, 196), true, 1, 1000);
	objectAddPresentation(kObject10109);
	objectPresentationAddImageToPuzzle(kObject10109, 5, kPuzzle10100, "NIS01N01P01L02S02.0005.bmp", Common::Point(221, 196), true, 1, 1000);
	objectAdd(kObject10105, "CCHolo", "", 1);
	objectAddPuzzleAccessibility(kObject10105, kPuzzle10100, Common::Rect(77, 132, 340, 298),  false, 52, 0);
	objectAddPuzzleAccessibility(kObject10105, kPuzzle10100, Common::Rect(140, 170, 430, 380), false, 52, 0);
	objectAdd(kObject10106, "CCHoloCross", "NI_HandSel", 4);
	objectAddPuzzleAccessibility(kObject10106, kPuzzle10102, Common::Rect(203, 236, 283, 316), true, 10000, 0);
	objectSetActiveDrawCursor(kObject10106, 15, 15, 0, 3, 0, 0, 3);
	objectSetPassiveDrawCursor(kObject10106, 15, 15, 0, 3, 0, 0, 3);

	for (uint32 i = 0; i < 20; i++) {
		objectAddPresentation(kObject10106);
		objectPresentationAddImageToPuzzle(kObject10106, i, kPuzzle10102, Common::String::format("NIS01N01P03L01S01.%04d.bmp", i + 1), Common::Point(213, 254), true, 1, 1000);
	}

	for (uint32 i = 0; i < 20; i++) {
		objectAddPresentation(kObject10106);
		objectPresentationAddImageToPuzzle(kObject10106, i, kPuzzle10102, Common::String::format("NIS01N01P03L01S01_a.%04d.bmp", i + 1), Common::Point(213, 254), true, 1, 1000);
	}

	objectAddPresentation(kObject10106);
	objectPresentationAddImageToPuzzle(kObject10106, 38, kPuzzle10102, "NIS01N01P03L02.0001.bmp", Common::Point(267, 283), true, 1, 1000);
	objectAddPresentation(kObject10106);
	objectPresentationAddImageToPuzzle(kObject10106, 39, kPuzzle10102, "NIS01N01P03L01.0001.bmp", Common::Point(56, 139), true, 1, 1000);
	objectAdd(kObject10107, "CCHoloDam1", "NI_HandSel", 4);
	objectAddPuzzleAccessibility(kObject10107, kPuzzle10102, Common::Rect(295, 255, 345, 315), true,  10000, 0);
	objectAddPuzzleAccessibility(kObject10107, kPuzzle10102, Common::Rect(295, 315, 345, 375), false, 10000, 1);
	objectSetActiveDrawCursor(kObject10107, 15, 15, 0, 3, 0, 0, 3);
	objectSetPassiveDrawCursor(kObject10107, 15, 15, 0, 3, 0, 0, 3);

	for (uint32 i = 0; i < 15; i++) {
		objectAddPresentation(kObject10107);
		objectPresentationAddImageToPuzzle(kObject10107, i, kPuzzle10102, Common::String::format("NIS01N01P03L01S02.%04d.bmp", i + 1), Common::Point(302, 264), true, 1, 1000);
	}

	objectAdd(kObject10200, "Speaker", "", 1);
	objectAddPuzzleAccessibility(kObject10200, kPuzzle10200, Common::Rect(150, 110, 470, 200), true, 52, 0);
	objectAddPresentation(kObject10200);
	objectPresentationAddAnimationToPuzzle(kObject10200, 0, kPuzzle10202, "NIS02N01P03S01", 0, Common::Point(229, 188), 1, 1000, 20, 12.5f, 4);
	objectAddPresentation(kObject10200);
	objectPresentationAddAnimationToPuzzle(kObject10200, 1, kPuzzle10203, "NIS02N01P02S01", 0, Common::Point(197, 44), 1, 1000, 10, 12.5f, 4);
	objectPresentationSetAnimationOnPuzzle(kObject10200, 1, 0, kObject10200);
	objectAddPresentation(kObject10200);
	objectPresentationAddAnimationToPuzzle(kObject10200, 2, kPuzzle10204, "NIS02N01P02S02", 0, Common::Point(311, 166), 1, 1000, 20, 12.5f, 4);
	objectAddPresentation(kObject10200);
	objectPresentationAddAnimationToPuzzle(kObject10200, 3, kPuzzle10205, "NIS02N01P02S03", 0, Common::Point(97, 185), 1, 1000, 20, 12.5f, 4);
	objectAdd(kObject10201, "Handle", "NI_HandSel", 4);
	objectAddPuzzleAccessibility(kObject10201, kPuzzle10200, Common::Rect(300, 210, 430, 270), false, 10000, 0);
	objectSetActiveDrawCursor(kObject10201, 15, 15, 0, 3, 0, 0, 3);
	objectSetPassiveDrawCursor(kObject10201, 15, 15, 0, 3, 0, 0, 3);

	for (uint32 i = 0; i < 15; i++) {
		objectAddPresentation(kObject10201);
		objectPresentationAddImageToPuzzle(kObject10201, i, kPuzzle10200, Common::String::format("NIS02N01P01S01.%04d.bmp", i + 1), Common::Point(305, 213), true, 1, 1000);
	}

	objectAdd(kObject10300, "Mime", "", 1);
	objectAddRotationAccessibility(kObject10300, 10301, Common::Rect(1937, -26, 2079, 88), true, 52, 0);
	objectAddPresentation(kObject10300);
	objectPresentationAddAnimationToRotation(kObject10300, 0, 10301, 0, 25, 10.0f, 4);
	objectPresentationSetAnimationOnRotation(kObject10300, 0, 0, kObject10300);
	objectPresentationShow(kObject10300, 0);
	objectAddPresentation(kObject10300);
	objectPresentationAddAnimationToPuzzle(kObject10300, 1, kPuzzle10390, "NIS03N01P01S02", 0, Common::Point(289, 79), 1, 1000, 20, 12.5f, 4);
	objectAddPresentation(kObject10300);
	objectPresentationAddAnimationToPuzzle(kObject10300, 2, kPuzzle10391, "NIS03N01P01S01", 0, Common::Point(260, 73), 1, 1000, 20, 12.5f, 4);
	objectAddPresentation(kObject10300);
	objectPresentationAddAnimationToPuzzle(kObject10300, 3, kPuzzle10392, "NIS03N01P03S01", 0, Common::Point(246, 183), 1, 1000, 20, 12.5f, 4);
	objectAdd(kObjectLogeTear, "Loge's Tear", "NI_Tear", 1);
	objectAddBagAnimation(kObjectLogeTear, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectLogeTear, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectLogeTear, 22, 22, 0, 3, 0, 0, 3);
	objectAddPuzzleAccessibility(kObjectLogeTear, kPuzzle10300, Common::Rect(470, 254, 541, 328), true, 52, 0);
	objectAddPresentation(kObjectLogeTear);
	objectPresentationAddImageToPuzzle(kObjectLogeTear, 0, kPuzzle10300, "NIS03N01P02L02.0001.bmp", Common::Point(466, 249), true, 1, 1000);
	objectAddPresentation(kObjectLogeTear);
	objectPresentationAddAnimationToPuzzle(kObjectLogeTear, 1, kPuzzle10300, "NIS03N01P02S01", 0, Common::Point(466, 250), 1, 1000, 21, 12.5f, 4);
	objectPresentationShow(kObjectLogeTear, 1);
	objectAdd(kObjectGold, "Rhine Gold", "RhineGold", 1);
	objectAdd(kObjectTile, "Tile", "NI_Mosaic", 1);
	objectAddBagAnimation(kObjectTile, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectTile, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectTile, 22, 22, 0, 3, 0, 0, 3);
	objectAddPuzzleAccessibility(kObjectTile, kPuzzle10300, Common::Rect(247, 24, 337, 126), true, 52, 0);
	objectAddPresentation(kObjectTile);
	objectPresentationAddImageToPuzzle(kObjectTile, 0, kPuzzle10300, "NIS03N01P02L01.0001.bmp", Common::Point(251, 45), true, 1, 1000);
	objectAdd(kObjectMagicFrog, "Frog", "NI_Frog", 1);
	objectAddBagAnimation(kObjectMagicFrog, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectMagicFrog, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectMagicFrog, 22, 22, 0, 3, 0, 0, 3);
	objectAddPuzzleAccessibility(kObjectMagicFrog, kPuzzle10300, Common::Rect(300, 230, 430, 430), true, 52, 0);
	objectAdd(kObject10420, "LValve", "NI_HandSel", 4);
	objectAddPuzzleAccessibility(kObject10420, kPuzzle10400, Common::Rect(287, 223, 344, 252), true, 10000, 0);
	objectAddPuzzleAccessibility(kObject10420, kPuzzle10400, Common::Rect(312, 274, 368, 300), false, 10000, 1);
	objectSetActiveDrawCursor(kObject10420, 15, 15, 0, 3, 0, 0, 3);
	objectSetPassiveDrawCursor(kObject10420, 15, 15, 0, 3, 0, 0, 3);
	objectAddPresentation(kObject10420);
	objectPresentationAddImageToPuzzle(kObject10420, 0, kPuzzle10400, "NIS04N02P01L01S01.0012.bmp", Common::Point(257, 211), true, 1, 1000);

	for (uint32 i = 1; i < 13; i++) {
		objectAddPresentation(kObject10420);
		objectPresentationAddImageToPuzzle(kObject10420, i, kPuzzle10400, Common::String::format("NIS04N02P01L01S01.%04d.bmp", i), Common::Point(257, 211), true, 1, 1000);
	}

	objectAdd(kObject10421, "RValve", "NI_HandSel", 4);
	objectAddPuzzleAccessibility(kObject10421, kPuzzle10401, Common::Rect(291, 221, 357, 257), true,  10000, 0);
	objectAddPuzzleAccessibility(kObject10421, kPuzzle10401, Common::Rect(271, 268, 336, 308), false, 10000, 1);
	objectSetActiveDrawCursor(kObject10421, 15, 15, 0, 3, 0, 0, 3);
	objectSetPassiveDrawCursor(kObject10421, 15, 15, 0, 3, 0, 0, 3);
	objectAddPresentation(kObject10421);
	objectPresentationAddImageToPuzzle(kObject10421, 0, kPuzzle10401, "NIS04N02P02L01S01.0012.bmp", Common::Point(227, 204), true, 1, 1000);

	for (uint32 i = 1; i < 13; i++) {
		objectAddPresentation(kObject10421);
		objectPresentationAddImageToPuzzle(kObject10421, i, kPuzzle10401, Common::String::format("NIS04N02P02L01S01.%04d.bmp", i), Common::Point(227, 204), true, 1, 1000);
	}

	objectAdd(kObject10430, "TearCasing", "", 3);
	objectAddPuzzleAccessibility(kObject10430, kPuzzle10411, Common::Rect(189, 189, 362, 377), true, 52, 0);
	objectAddPuzzleAccessibility(kObject10430, kPuzzle10411, Common::Rect(224, 90, 362, 209),  false, 52, 1);
	objectAddPuzzleAccessibility(kObject10430, kPuzzle10411, Common::Rect(293, 241, 475, 462), false, 52, 2);
	objectAddPresentation(kObject10430);
	objectPresentationAddImageToPuzzle(kObject10430, 0, kPuzzle10411, "NIS04N03P03L01.0001.bmp", Common::Point(214, 173), true, 1, 1000);
	objectAddPresentation(kObject10430);
	objectPresentationAddImageToPuzzle(kObject10430, 1, kPuzzle10411, "NIS04N03P03L02.0001.bmp", Common::Point(270, 116), true, 1, 1000);
	objectAddPresentation(kObject10430);
	objectPresentationAddAnimationToPuzzle(kObject10430, 2, kPuzzle10411, "NIS04N03P03L01S01", 0, Common::Point(217, 171), 1, 1000, 27, 12.5f, 10);
	objectPresentationSetAnimationOnPuzzle(kObject10430, 2, 0, kObject10422);
	objectAddPresentation(kObject10430);
	objectPresentationAddAnimationToPuzzle(kObject10430, 3, kPuzzle10411, "NIS04N03P03L01S01", 0, Common::Point(217, 171), 1, 1000, 27, 12.5f, 6);
	objectPresentationSetAnimationOnPuzzle(kObject10430, 3, 0, kObject10423);
	objectAddPresentation(kObject10430);
	objectPresentationAddAnimationToPuzzle(kObject10430, 4, kPuzzle10411, "NIS04N03P03L02S01", 0, Common::Point(215, 111), 1, 1000, 26, 12.5f, 10);
	objectPresentationSetAnimationOnPuzzle(kObject10430, 4, 0, kObject10424);
	objectAddPresentation(kObject10430);
	objectPresentationAddAnimationToPuzzle(kObject10430, 5, kPuzzle10411, "NIS04N03P03L02S01", 0, Common::Point(215, 111), 1, 1000, 26, 12.5f, 6);
	objectPresentationSetAnimationOnPuzzle(kObject10430, 5, 0, kObject10425);
	objectAddPresentation(kObject10430);
	objectPresentationAddAnimationToPuzzle(kObject10430, 6, kPuzzle10411, "NIS04N03P03L02S02", 0, Common::Point(275, 126), 1, 1000, 20, 12.5f, 4);
	objectAdd(kObject10440, "Door", "", 1);
	objectAddRotationAccessibility(kObject10440, 10404, Common::Rect(2923, -370, 3549, 460), true, 53, 0);
	objectAdd(kObject10450, "Water", "", 1);
	objectAddRotationAccessibility(kObject10450, 10405, Common::Rect(2000, 400, 3600, 680), true, 52, 0);
	objectAddRotationAccessibility(kObject10450, 10415, Common::Rect(2000, 400, 3600, 680), true, 52, 0);
	objectAdd(kObject10460, "Water", "", 1);
	objectAddRotationAccessibility(kObject10460, 10406, Common::Rect(0, -600, 3599, 600), true, 53, 1);
	objectAdd(kObject10432, "", "", 1);
	objectAddPresentation(kObject10432);
	objectPresentationAddAnimationToPuzzle(kObject10432, 0, kPuzzle10410, "NIS04N03P01S02", 0, Common::Point(171, 285), 1, 1000, 13, 10.0f, 32);
	objectPresentationAddAnimationToPuzzle(kObject10432, 0, kPuzzle10410, "NIS04N03P01S03", 0, Common::Point(244, 268), 1, 1000, 12, 10.0f, 32);
	objectPresentationAddAnimationToPuzzle(kObject10432, 0, kPuzzle10410, "NIS04N03P01S04", 0, Common::Point(317, 261), 1, 1000, 7, 10.0f, 32);
	objectAddPresentation(kObject10432);
	objectPresentationAddAnimationToRotation(kObject10432, 1, 10401, 0, 25, 12.5f, 6);
	objectPresentationAddAnimationToRotation(kObject10432, 1, 10402, 0, 26, 12.5f, 6);
	objectAddPresentation(kObject10432);
	objectPresentationAddAnimationToRotation(kObject10432, 2, 10401, 1, 25, 12.5f, 6);
	objectPresentationAddAnimationToRotation(kObject10432, 2, 10402, 1, 26, 12.5f, 6);
	objectAdd(kObject10431, "Temperat", "", 1);
	objectAddPresentation(kObject10431);

	for (uint32 i = 1; i < 13; i++) {
		objectAddPresentation(kObject10431);
		objectPresentationAddImageToPuzzle(kObject10431, i, kPuzzle10410, Common::String::format("NIS04N03P01S01.%04d.bmp", i + 1), Common::Point(107, 155), true, 1, 1000);
	}

	objectAdd(kObjectDivingHelmet2, "Helmet&Frog", "NI_HelmetFrog", 1);
	objectAddBagAnimation(kObjectDivingHelmet2, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectDivingHelmet2, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectDivingHelmet2, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObjectDivingHelmet, "Helmet", "NI_Helmet", 1);
	objectAddBagAnimation(kObjectDivingHelmet, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectDivingHelmet, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectDivingHelmet, 22, 22, 0, 3, 0, 0, 3);
	objectAddPuzzleAccessibility(kObjectDivingHelmet, kPuzzle10501, Common::Rect(218, 249, 419, 346), true, 52, 0);
	objectAddPresentation(kObjectDivingHelmet);
	objectPresentationAddImageToPuzzle(kObjectDivingHelmet, 0, kPuzzle10501, "NIS05N01P01L02.0001.bmp", Common::Point(229, 252), true, 1, 1000);
	objectAdd(kObjectAntiGCells, "AGCells", "RH_AntiG", 1);
	objectAddBagAnimation(kObjectAntiGCells, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectAntiGCells, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectAntiGCells, 22, 22, 0, 3, 0, 0, 3);
	objectAddRotationAccessibility(kObjectAntiGCells, 10501, Common::Rect(2921, 68, 3185, 477), true, 52, 0);
	objectAdd(kObject10600, "UTiles", "", 1);
	objectAddPuzzleAccessibility(kObject10600, kPuzzle10600, Common::Rect(0, 420, 640, 464),  true, 55, 0);
	objectAdd(kObject10601, "UTile1", "NI_HandSel", 4);
	objectAddPuzzleAccessibility(kObject10601, kPuzzle10600, Common::Rect(215, 73, 430, 207), true, 10000, 0);
	objectSetActiveDrawCursor(kObject10601, 15, 15, 0, 3, 0, 0, 3);
	objectSetPassiveDrawCursor(kObject10601, 15, 15, 0, 3, 0, 0, 3);

	for (uint32 i = 0; i < 49; i++) {
		objectAddPresentation(kObject10601);
		objectPresentationAddImageToPuzzle(kObject10601, i, kPuzzle10600, Common::String::format("NIS06N01P01S01.%04d.bmp", i + 1), Common::Point(203, 82), true, 1, 1000);
	}

	objectAdd(kObject10602, "UTile2", "NI_HandSel", 4);
	objectAddPuzzleAccessibility(kObject10602, kPuzzle10600, Common::Rect(222, 211, 420, 266), true, 10000, 0);
	objectSetActiveDrawCursor(kObject10602, 15, 15, 0, 3, 0, 0, 3);
	objectSetPassiveDrawCursor(kObject10602, 15, 15, 0, 3, 0, 0, 3);

	for (uint32 i = 0; i < 49; i++) {
		objectAddPresentation(kObject10602);
		objectPresentationAddImageToPuzzle(kObject10602, i, kPuzzle10600, Common::String::format("NIS06N01P01S02.%04d.bmp", i + 1), Common::Point(245, 211), true, 1, 1000);
	}

	objectAdd(kObject10603, "UTile3", "NI_HandSel", 4);
	objectAddPuzzleAccessibility(kObject10603, kPuzzle10600, Common::Rect(216, 272, 424, 398), true, 10000, 0);
	objectSetActiveDrawCursor(kObject10603, 15, 15, 0, 3, 0, 0, 3);
	objectSetPassiveDrawCursor(kObject10603, 15, 15, 0, 3, 0, 0, 3);

	for (uint32 i = 0; i < 49; i++) {
		objectAddPresentation(kObject10603);
		objectPresentationAddImageToPuzzle(kObject10603, i, kPuzzle10600, Common::String::format("NIS06N01P01S03.%04d.bmp", i + 1), Common::Point(199, 272), true, 1, 1000);
	}

	soundAdd(14001, kSoundTypeBackgroundMusic, "1583.was", _configuration.backgroundMusic.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(14002, kSoundTypeBackgroundMusic, "1584.was", _configuration.backgroundMusic.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(14003, kSoundTypeBackgroundMusic, "1585.was", _configuration.backgroundMusic.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundSetVolume(14003, 90);
	soundSetVolume(14001, 90);
	soundSetVolume(14002, 90);
	soundAdd(10101, kSoundTypeAmbientEffect, "1586.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(10300, kSoundTypeAmbientEffect, "1587.was", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(10301, kSoundTypeAmbientEffect, "1588.wav", _configuration.ambientEffect.loadFrom);
	soundAdd(10800, kSoundTypeAmbientEffect, "1589.wac", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(10801, kSoundTypeAmbientEffect, "1590.wac", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(10900, kSoundTypeAmbientEffect, "1591.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(10901, kSoundTypeAmbientEffect, "1592.was", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(10902, kSoundTypeAmbientEffect, "1593.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(10803, kSoundTypeAmbientEffect, "1594.wav", _configuration.ambientEffect.loadFrom);
	soundAdd(10804, kSoundTypeAmbientEffect, "1595.wav", _configuration.ambientEffect.loadFrom);
	soundAdd(10409, kSoundTypeAmbientEffect, "1596.was", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(10410, kSoundTypeAmbientEffect, "1597.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(10411, kSoundTypeAmbientEffect, "1598.was", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(10412, kSoundTypeAmbientEffect, "1599.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(10413, kSoundTypeAmbientEffect, "1600.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(10501, kSoundTypeAmbientEffect, "1601.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(10502, kSoundTypeAmbientEffect, "1602.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(10102, kSoundTypeAmbientEffect, "1603.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(10103, kSoundTypeAmbientEffect, "1604.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(10104, kSoundTypeAmbientEffect, "1605.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(10105, kSoundTypeAmbientEffect, "1606.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(13001, kSoundTypeAmbientEffect, "1607.wac", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(13002, kSoundTypeAmbientEffect, "1608.wac", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(13003, kSoundTypeAmbientEffect, "1609.wac", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(13004, kSoundTypeAmbientEffect, "1610.wac", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(13005, kSoundTypeAmbientEffect, "1611.wac", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(13006, kSoundTypeAmbientEffect, "1612.wac", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(13007, kSoundTypeAmbientEffect, "1613.wac", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(13008, kSoundTypeAmbientEffect, "1614.wac", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(13009, kSoundTypeAmbientEffect, "1615.wac", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(10415, kSoundTypeAmbientEffect, "1616.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(10106, kSoundTypeEffect, "1617.wav", _configuration.effect.loadFrom);
	soundAdd(10414, kSoundTypeEffect, "1618.wav", _configuration.effect.loadFrom);
	soundAdd(10401, kSoundTypeEffect, "1619.wav", _configuration.effect.loadFrom);
	soundAdd(10402, kSoundTypeEffect, "1620.wav", _configuration.effect.loadFrom);
	soundAdd(10403, kSoundTypeEffect, "1621.wav", _configuration.effect.loadFrom);
	soundAdd(10404, kSoundTypeEffect, "1616.wav", _configuration.effect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(10405, kSoundTypeEffect, "1622.wav", _configuration.effect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(10406, kSoundTypeEffect, "1623.wav", _configuration.effect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(10407, kSoundTypeEffect, "1624.wav", _configuration.effect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(10408, kSoundTypeEffect, "1625.wav", _configuration.effect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(12001, kSoundTypeDialog, "1626.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(12002, kSoundTypeDialog, "1627.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(12003, kSoundTypeDialog, "1628.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10001, kSoundTypeDialog, "1629.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10002, kSoundTypeDialog, "1630.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10003, kSoundTypeDialog, "1631.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10004, kSoundTypeDialog, "1632.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10005, kSoundTypeDialog, "1633.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10006, kSoundTypeDialog, "1634.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10007, kSoundTypeDialog, "1635.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10008, kSoundTypeDialog, "1636.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10009, kSoundTypeDialog, "1637.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10010, kSoundTypeDialog, "1638.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10011, kSoundTypeDialog, "1639.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10012, kSoundTypeDialog, "1640.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10013, kSoundTypeDialog, "1641.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10014, kSoundTypeDialog, "1642.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10015, kSoundTypeDialog, "1643.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10018, kSoundTypeDialog, "1644.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10016, kSoundTypeDialog, "1645.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10019, kSoundTypeDialog, "1646.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10017, kSoundTypeDialog, "1647.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10020, kSoundTypeDialog, "1648.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10021, kSoundTypeDialog, "1649.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10022, kSoundTypeDialog, "1650.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10023, kSoundTypeDialog, "1651.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10024, kSoundTypeDialog, "1652.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10025, kSoundTypeDialog, "1653.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10026, kSoundTypeDialog, "1654.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10027, kSoundTypeDialog, "1655.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10028, kSoundTypeDialog, "1656.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10029, kSoundTypeDialog, "1657.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10030, kSoundTypeDialog, "1658.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10031, kSoundTypeDialog, "1659.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10032, kSoundTypeDialog, "1660.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10037, kSoundTypeDialog, "1661.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10034, kSoundTypeDialog, "1662.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10035, kSoundTypeDialog, "1663.wav", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10036, kSoundTypeDialog, "1664.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(10033, kSoundTypeDialog, "1665.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	rotationAdd3DSound(10301, 10301, 1, 1, 20, 100, 160.0f, 20);
	rotationSet3DSoundOff(10301, 10301);
	rotationAdd3DSound(10301, 10300, 1, 1, 20, 100, 160.0f, 5);
	rotationAdd3DSound(10003, 10300, 1, 1, 20, 95, 195.0f, 10);
	puzzleAdd3DSound(kPuzzle10300, 10300, 1, 1, 2, 93, 200.0f, 5);
	rotationAdd3DSound(10403, 10412, 1, 1, 20, 90, 270.0f, 10);
	rotationSet3DSoundOff(10403, 10412);
	rotationAdd3DSound(10402, 10412, 1, 1, 20, 85, 270.0f, 10);
	rotationSet3DSoundOff(10402, 10412);
	rotationAdd3DSound(10401, 10412, 1, 1, 20, 80, 270.0f, 10);
	rotationSet3DSoundOff(10401, 10412);
	rotationAdd3DSound(10404, 10412, 1, 1, 20, 90, 270.0f, 10);
	rotationSet3DSoundOff(10404, 10412);
	puzzleAdd3DSound(kPuzzle10410, 10412, 1, 1, 2, 93, 300.0f, 5);
	puzzleSet3DSoundOff(kPuzzle10410, 10412);
	puzzleAdd3DSound(kPuzzle10411, 10412, 1, 1, 2, 95, 300.0f, 5);
	puzzleSet3DSoundOff(kPuzzle10411, 10412);
	puzzleAdd3DSound(kPuzzle10400, 10412, 1, 1, 2, 95, 205.0f, 5);
	puzzleSet3DSoundOff(kPuzzle10400, 10412);
	puzzleAdd3DSound(kPuzzle10401, 10412, 1, 1, 2, 95, 345.0f, 5);
	puzzleSet3DSoundOff(kPuzzle10401, 10412);
	varDefineWord(10100, 0);
	varDefineWord(10101, 0);
	varDefineWord(10600, 12);
	varDefineWord(10601, 0);
	varDefineWord(10602, 24);
	varDefineDword(10000, 0);
	varDefineDword(10001, 0);
	varDefineByte(10432, 0);
	varDefineByte(10200, 0);
	varDefineByte(10000, 0);
	varDefineByte(10300, 0);
	varDefineByte(10301, 0);
	varDefineByte(10302, 0);
	varDefineByte(10303, 0);
	varDefineByte(10420, 0);
	varDefineByte(10421, 0);
	varDefineByte(10430, 0);
	varDefineByte(10431, 0);
	varDefineByte(10100, 0);
	varDefineByte(10101, 0);
	varDefineByte(10102, 0);
	varDefineByte(10103, 0);
	varDefineByte(10104, 0);
	varDefineByte(10105, 0);
	varDefineByte(10500, 0);
	varDefineByte(10501, 1);
	varDefineByte(10106, 0);
	varDefineByte(10107, 0);
	varDefineByte(10113, 0);
	varDefineByte(10108, 0);
	varDefineByte(10109, 0);
	varDefineByte(10110, 0);
	varDefineByte(10111, 0);
	varDefineByte(10112, 0);
}

void ApplicationRing::initZoneRH() {
	debugC(1, kRingDebugLogic, "Initializing Alberich zone (RH)...");

	setZoneAndEnableBag(kZoneRH);

	puzzleAdd(kPuzzle22001);
	puzzleAddBackgroundImage(kPuzzle22001, "TR_NI_RH_BP01.BMP", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle22002);
	puzzleAddBackgroundImage(kPuzzle22002, "TR_NI_RH_BP02.BMP", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle22003);
	puzzleAddBackgroundImage(kPuzzle22003, "TR_NI_RH_BP03.BMP", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle20000);
	puzzleAddBackgroundImage(kPuzzle20000, "RHS00N01P01S02.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle20011);
	puzzleAddBackgroundImage(kPuzzle20011, "RHS00N01P01L00.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle20021);
	puzzleAddBackgroundImage(kPuzzle20021, "RHS00N02P01L00.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle20031);
	puzzleAddBackgroundImage(kPuzzle20031, "RHS00N03P01L00.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle20202);
	puzzleAddBackgroundImage(kPuzzle20202, "RHS02N01P02.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle20203);
	puzzleAddBackgroundImage(kPuzzle20203, "RHS02N01P02DI01.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle20204);
	puzzleAddBackgroundImage(kPuzzle20204, "RHS02N02P01.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle20301);
	puzzleAddBackgroundImage(kPuzzle20301, "RHS03N01P01.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle20302);
	puzzleAddBackgroundImage(kPuzzle20302, "RHS03N02P01.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle20303);
	puzzleAddBackgroundImage(kPuzzle20303, "RHS03N03P01.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle20401);
	puzzleAddBackgroundImage(kPuzzle20401, "RHS04N01P01L01.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle20501);
	puzzleAddBackgroundImage(kPuzzle20501, "RHS05N01P01.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle20502);
	puzzleAddBackgroundImage(kPuzzle20502, "RHS05N01P02.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle20503);
	puzzleAddBackgroundImage(kPuzzle20503, "RHS05N02P01.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle20701);
	puzzleAddBackgroundImage(kPuzzle20701, "RHS07N01P01L01.bmp", Common::Point(0, 16), true);
	rotationAdd(20010, "RHS00N01", 0, 0);
	rotationAdd(20020, "RHS00N02", 0, 0);
	rotationAdd(20030, "RHS00N03", 0, 0);
	rotationAdd(20101, "RHS01N01", 0, 2);
	rotationSetComBufferLength(20101, 1300000);
	rotationAdd(20201, "RHS02N01", 0, 1);
	rotationAdd(20202, "RHS02N02", 0, 1);
	rotationAdd(20203, "RHS02N03", 0, 1);
	rotationAdd(20301, "RHS03N01", 0, 2);
	rotationSetComBufferLength(20301, 1300000);
	rotationAdd(20302, "RHS03N02", 0, 0);
	rotationAdd(20303, "RHS03N03", 0, 0);
	rotationAdd(20304, "RHS03N04", 0, 1);
	rotationAdd(20305, "RHS03N05", 0, 1);
	rotationAdd(20401, "RHS04N01", 0, 1);
	rotationAdd(20402, "RHS04N02", 0, 1);
	rotationAdd(20403, "RHS04N03", 0, 1);
	rotationAdd(20501, "RHS05N01", 0, 1);
	rotationAdd(20503, "RHS05N03", 0, 1);
	rotationAdd(20504, "RHS05N04", 0, 0);
	rotationAdd(20601, "RHS06N01", 0, 0);
	rotationAdd(20701, "RHS07N01", 0, 0);
	rotationSetJugOn(20701, 10.0f, 1.0);
	rotationAddMovabilityToPuzzle(20010, kPuzzle20011, "1708", Common::Rect(274, -289, 836, 102), true, 53, 0);
	rotationSetMovabilityToPuzzle(20010, 0, 1.5f, -4.3f, 79.4f, 0, 0);
	rotationAddMovabilityToRotation(20010, 20020, "1709", Common::Rect(274, -289, 836, 102), false, 53, 1);
	rotationSetMovabilityToRotation(20010, 1, 1.5f, -4.3f, 79.4f, 0, 0, 1.5f, -4.3f, 79.4f);
	rotationAddMovabilityToPuzzle(20020, kPuzzle20021, "1713", Common::Rect(274, -289, 836, 102), true, 53, 0);
	rotationSetMovabilityToPuzzle(20020, 0, 1.5f, -4.3f, 79.4f, 0, 0);
	rotationAddMovabilityToRotation(20020, 20030, "1709", Common::Rect(274, -289, 836, 102), false, 53, 1);
	rotationSetMovabilityToRotation(20020, 1, 1.5f, -4.3f, 79.4f, 0, 0, 1.5f, -4.3f, 79.4f);
	rotationAddMovabilityToRotation(20020, 20010, "1709", Common::Rect(2074, -289, 2636, 102), true, 53, 1);
	rotationSetMovabilityToRotation(20020, 2, 181.5f, -4.3f, 79.4f, 0, 0, 181.5f, -4.3f, 79.4f);
	rotationAddMovabilityToPuzzle(20030, kPuzzle20031, "1717", Common::Rect(274, -289, 836, 102), true, 53, 0);
	rotationSetMovabilityToPuzzle(20030, 0, 1.5f, -4.3f, 79.4f, 0, 0);
	rotationAddMovabilityToRotation(20030, 20101, "1718", Common::Rect(0, -289, 836, 102), false, 53, 1);
	rotationSetMovabilityToRotation(20030, 1, 325.0f, -4.3f, 79.4f, 0, 2, 342.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(20030, 20020, "1709", Common::Rect(2074, -289, 2636, 102), true, 53, 1);
	rotationSetMovabilityToRotation(20030, 2, 181.5f, -4.3f, 79.4f, 0, 0, 181.5f, -4.3f, 79.4f);
	rotationAddMovabilityToRotation(20101, 20030, "1709", Common::Rect(2239, -62, 2902, 240), true, 53, 1);
	rotationSetMovabilityToRotation(20101, 0, 215.0f, 0.3f, 85.7f, 0, 2, 181.5f, -4.3f, 79.4f);
	rotationAddMovabilityToRotation(20101, 20201, "1695", Common::Rect(82, -97, 362, 268), false, 53, 1);
	rotationSetMovabilityToRotation(20101, 1, 215.0f, 0.3f, 85.7f, 0, 2, 0, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(20201, 20202, "1722", Common::Rect(356, -162, 553, 238), false, 53, 0);
	rotationSetMovabilityToRotation(20201, 0, 0, 0.3f, 85.7f, 0, 2, 0, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(20201, 20101, "1723", Common::Rect(2098, -130, 2371, 221), true, 53, 0);
	rotationSetMovabilityToRotation(20201, 1, 185.0f, 0.3f, 85.7f, 0, 0, 180.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(20202, 20203, "1724", Common::Rect(346, -88, 569, 191), true, 53, 0);
	rotationSetMovabilityToRotation(20202, 0, 0, 0.3f, 85.7f, 0, 2, 270.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(20202, 20203, "1725", Common::Rect(346, -88, 569, 191), false, 53, 0);
	rotationSetMovabilityToRotation(20202, 1, 0, 0.3f, 85.7f, 0, 2, 270.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(20202, 20201, "1726", Common::Rect(1921, -251, 2515, 450), true, 53, 0);
	rotationSetMovabilityToRotation(20202, 2, 180.0f, 0.3f, 85.7f, 0, 0, 180.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(20203, 20202, "1727", Common::Rect(1218, -88, 1499, 204), true, 53, 0);
	rotationSetMovabilityToRotation(20203, 0, 90.0f, 0.3f, 85.7f, 0, 2, 180.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(20203, 20202, "1728", Common::Rect(1218, -88, 1499, 204), false, 53, 0);
	rotationSetMovabilityToRotation(20203, 1, 90.0f, 0.3f, 85.7f, 0, 2, 180.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(20203, 20301, "1707", Common::Rect(3300, -560, 3600, 600), false, 53, 0);
	rotationSetMovabilityToRotation(20203, 2, 270.0f, 0.3f, 85.7f, 0, 2, 42.0f, 23.0f, 85.7f);
	rotationAddMovabilityToRotation(20203, 20301, "1707", Common::Rect(0, -600, 50, 560), false, 53, 0); // Updated to valid rect
	rotationSetMovabilityToRotation(20203, 3, 270.0f, 0.3f, 85.7f, 0, 2, 42.0f, 23.0f, 85.7f);
	rotationAddMovabilityToRotation(20304, 20305, "1729", Common::Rect(346, -88, 569, 191), true, 53, 0);
	rotationSetMovabilityToRotation(20304, 0, 0, 0.3f, 85.7f, 0, 2, 90.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(20304, 20305, "1730", Common::Rect(346, -88, 569, 191), false, 53, 0);
	rotationSetMovabilityToRotation(20304, 1, 0, 0.3f, 85.7f, 0, 2, 90.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(20304, 20303, "1731", Common::Rect(1933, -350, 2634, 430), true, 53, 0);
	rotationSetMovabilityToRotation(20304, 2, 180.0f, 0.3f, 85.7f, 0, 0, 180.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(20305, 20304, "1732", Common::Rect(2009, -55, 2369, 320), true, 53, 0);
	rotationSetMovabilityToRotation(20305, 0, 90.0f, 0.3f, 85.7f, 0, 2, 180.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(20305, 20304, "1733", Common::Rect(2009, -55, 2369, 320), false, 53, 0);
	rotationSetMovabilityToRotation(20305, 1, 90.0f, 0.3f, 85.7f, 0, 2, 180.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(20305, 20401, "1734", Common::Rect(680, -270, 1430, 530), false, 53, 0);
	rotationSetMovabilityToRotation(20305, 2, 270.0f, 0.3f, 85.7f, 0, 2, 180.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(20402, 20403, "1735", Common::Rect(346, -88, 569, 191), true, 53, 0);
	rotationSetMovabilityToRotation(20402, 0, 1.0f, 0.3f, 85.7f, 0, 2, 270.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(20402, 20403, "1736", Common::Rect(346, -88, 569, 191), false, 53, 0);
	rotationSetMovabilityToRotation(20402, 1, 1.0f, 0.3f, 85.7f, 0, 2, 270.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(20403, 20402, "1737", Common::Rect(1209, -68, 1474, 207), true, 53, 0);
	rotationSetMovabilityToRotation(20403, 0, 90.0f, 0.3f, 85.7f, 0, 2, 180.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(20403, 20402, "1738", Common::Rect(1209, -68, 1474, 207), false, 53, 0);
	rotationSetMovabilityToRotation(20403, 1, 90.0f, 0.3f, 85.7f, 0, 2, 180.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(20403, 20501, "1739", Common::Rect(3000, -600, 3170, 460), false, 53, 0);
	rotationSetMovabilityToRotation(20403, 2, 270.0f, 0.3f, 85.7f, 0, 2, 10.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(20503, 20501, "1740", Common::Rect(1933, -350, 2634, 430), true, 53, 0);
	rotationSetMovabilityToRotation(20503, 0, 180.0f, 0.3f, 85.7f, 0, 0, 180.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(20503, 20504, "1741", Common::Rect(346, -88, 569, 191), true, 53, 0);
	rotationSetMovabilityToRotation(20503, 1, 0, 0.3f, 85.7f, 0, 2, 180.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(20504, 20503, "1742", Common::Rect(263, -186, 717, 218), true, 53, 0);
	rotationSetMovabilityToRotation(20504, 0, 0, 0.3f, 85.7f, 0, 2, 180.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(20504, 20503, "1743", Common::Rect(263, -186, 717, 218), false, 53, 0);
	rotationSetMovabilityToRotation(20504, 1, 0, 0.3f, 85.7f, 0, 2, 180.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(20504, 20601, "1744", Common::Rect(2125, -151, 2355, 570), true, 53, 0);
	rotationSetMovabilityToRotation(20504, 2, 270.0f, 0.3f, 85.7f, 0, 2, 0, 0.3f, 85.7f);
	objectAdd(kObject22000, "", "", 1);
	objectAddPresentation(kObject22000);
	objectPresentationAddAnimationToPuzzle(kObject22000, 0, kPuzzle22001, "TR_NI_RH_BP01S01", 0, Common::Point(239, 208), 1, 1000, 20, 12.5f, 4);
	objectAddPresentation(kObject22000);
	objectPresentationAddAnimationToPuzzle(kObject22000, 1, kPuzzle22002, "TR_NI_RH_BP02S01", 0, Common::Point(187, 94), 1, 1000, 20, 12.5f, 4);
	objectAddPresentation(kObject22000);
	objectPresentationAddAnimationToPuzzle(kObject22000, 2, kPuzzle22003, "TR_NI_RH_BP03S01", 0, Common::Point(207, 134), 1, 1000, 20, 12.5f, 4);
	objectAdd(kObject21003, "", "", 1);
	objectAddPresentation(kObject21003);
	objectPresentationAddImageToRotation(kObject21003, 0, 20203, 0);
	objectAddPresentation(kObject21003);
	objectPresentationAddImageToRotation(kObject21003, 1, 20305, 0);
	objectAddPresentation(kObject21003);
	objectPresentationAddImageToRotation(kObject21003, 2, 20403, 0);
	objectAdd(kObject21001, "", "", 1);
	objectAddPresentation(kObject21001);
	objectPresentationAddAnimationToPuzzle(kObject21001, 0, kPuzzle20000, "RHS00N01P01S02", 0, Common::Point(215, 155), 1, 1000, 20, 12.5f, 4);
	objectAdd(kObject20001, "", "", 1);
	objectAddPuzzleAccessibility(kObject20001, kPuzzle20011, Common::Rect(339, 22, 453, 396), false, 52, 0);
	objectAddPuzzleAccessibility(kObject20001, kPuzzle20011, Common::Rect(  0, 16, 639, 463), false, 53, 1);
	objectAddPresentation(kObject20001);
	objectPresentationAddImageToPuzzle(kObject20001, 0, kPuzzle20011, "RHS00N01P01S01SD.0001.tga", Common::Point(320, 16), true, 3, 997);
	objectAddPresentation(kObject20001);
	objectPresentationAddAnimationToPuzzle(kObject20001, 1, kPuzzle20011, "RHS00N01P01s01sd", 0, Common::Point(334, 16), 1, 1000, 20, 12.5f, 4);
	objectAddPresentation(kObject20001);
	objectPresentationAddAnimationToPuzzle(kObject20001, 2, kPuzzle20011, "RHS00N01P01a01_a", 0, Common::Point(334, 16), 1, 999, 5, 12.5f, 6);
	objectAddPresentation(kObject20001);
	objectPresentationAddAnimationToPuzzle(kObject20001, 3, kPuzzle20011, "RHS00N01P01a01_a", 0, Common::Point(334, 16), 1, 999, 5, 12.5f, 10);
	objectAddPresentation(kObject20001);
	objectPresentationAddImageToPuzzle(kObject20001, 4, kPuzzle20011, "RHS00N01P01S01SD.0001.BMP", Common::Point(334, 16), true, 1, 998);
	objectAdd(kObject20002, "", "", 1);
	objectAddPuzzleAccessibility(kObject20002, kPuzzle20021, Common::Rect(204, 99, 314, 436), false, 52, 0);
	objectAddPuzzleAccessibility(kObject20002, kPuzzle20021, Common::Rect(  0, 16, 639, 463), false, 53, 1);
	objectAddPresentation(kObject20002);
	objectPresentationAddImageToPuzzle(kObject20002, 0, kPuzzle20021, "RHS00N02P01S01SD.0001.tga", Common::Point(170, 86), true, 3, 997);
	objectAddPresentation(kObject20002);
	objectPresentationAddAnimationToPuzzle(kObject20002, 1, kPuzzle20021, "RHS00N02P01s01sd", 0, Common::Point(199, 81), 1, 1000, 20, 12.5f, 4);
	objectAddPresentation(kObject20002);
	objectPresentationAddAnimationToPuzzle(kObject20002, 2, kPuzzle20021, "RHS00N02P01a01_a", 0, Common::Point(199, 81), 1, 999, 5, 12.5f, 6);
	objectAddPresentation(kObject20002);
	objectPresentationAddAnimationToPuzzle(kObject20002, 3, kPuzzle20021, "RHS00N02P01a01_a", 0, Common::Point(199, 81), 1, 999, 5, 12.5f, 10);
	objectAddPresentation(kObject20002);
	objectPresentationAddImageToPuzzle(kObject20002, 4, kPuzzle20021, "RHS00N02P01S01SD.0001.BMP", Common::Point(199, 81), true, 1, 998);
	objectAdd(kObject20003, "", "", 1);
	objectAddPuzzleAccessibility(kObject20003, kPuzzle20031, Common::Rect(85, 347, 471, 420), false, 52, 0);
	objectAddPuzzleAccessibility(kObject20003, kPuzzle20031, Common::Rect( 0,  16, 639, 463), false, 53, 1);
	objectAddPresentation(kObject20003);
	objectPresentationAddImageToPuzzle(kObject20003, 0, kPuzzle20031, "RHS00N03P01S01SD.0001.bmp", Common::Point(0, 16), true, 1, 997);
	objectAddPresentation(kObject20003);
	objectPresentationAddAnimationToPuzzle(kObject20003, 1, kPuzzle20031, "RHS00N03P01s01sd", 0, Common::Point(62, 353), 1, 1000, 20, 12.5f, 4);
	objectAddPresentation(kObject20003);
	objectPresentationAddAnimationToPuzzle(kObject20003, 2, kPuzzle20031, "RHS00N03P01a01_a", 0, Common::Point(60, 351), 1, 999, 5, 12.5f, 6);
	objectAddPresentation(kObject20003);
	objectPresentationAddAnimationToPuzzle(kObject20003, 3, kPuzzle20031, "RHS00N03P01a01_a", 0, Common::Point(60, 351), 1, 999, 5, 12.5f, 10);
	objectAddPresentation(kObject20003);
	objectPresentationAddImageToPuzzle(kObject20003, 4, kPuzzle20031, "RHS00N03P01S01SD1.0001.BMP", Common::Point(62, 353), true, 1, 998);
	objectAdd(kObjectKeyIndifference, "Indifference", "RH_Key1", 9);
	objectAddBagAnimation(kObjectKeyIndifference, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectKeyIndifference, 2, 43, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectKeyIndifference, 2, 43, 0, 3, 0, 0, 3);
	objectAddPuzzleAccessibility(kObjectKeyIndifference, kPuzzle20011, Common::Rect(288, 284, 362, 422), false, 52, 0);
	objectAddPresentation(kObjectKeyIndifference);
	objectPresentationAddAnimationToPuzzle(kObjectKeyIndifference, 0, kPuzzle20011, "RHS00N01P01SA01", 1, Common::Point(270, 286), 3, 1000, 20, 12.5f, 4);
	objectAdd(kObjectKeyMistrust, "Mistrust", "RH_Key1", 9);
	objectAddBagAnimation(kObjectKeyMistrust, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectKeyMistrust, 2, 43, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectKeyMistrust, 2, 43, 0, 3, 0, 0, 3);
	objectAddPuzzleAccessibility(kObjectKeyMistrust, kPuzzle20021, Common::Rect(288, 284, 362, 422), false, 52, 0);
	objectAddPresentation(kObjectKeyMistrust);
	objectPresentationAddAnimationToPuzzle(kObjectKeyMistrust, 0, kPuzzle20021, "RHS00N01P01SA01", 1, Common::Point(270, 286), 3, 1000, 20, 12.5f, 4);
	objectAdd(kObjectKeySelfishness, "Selfishness", "RH_Key1", 9);
	objectAddBagAnimation(kObjectKeySelfishness, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectKeySelfishness, 2, 43, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectKeySelfishness, 2, 43, 0, 3, 0, 0, 3);
	objectAddPuzzleAccessibility(kObjectKeySelfishness, kPuzzle20031, Common::Rect(288, 284, 362, 422), false, 52, 0);
	objectAddPresentation(kObjectKeySelfishness);
	objectPresentationAddAnimationToPuzzle(kObjectKeySelfishness, 0, kPuzzle20031, "RHS00N01P01SA01", 1, Common::Point(270, 286), 3, 1000, 20, 12.5f, 4);
	objectAdd(kObjectKeyDisgust, "Disgust", "RH_Key1", 9);
	objectAddBagAnimation(kObjectKeyDisgust, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectKeyDisgust, 2, 43, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectKeyDisgust, 2, 43, 0, 3, 0, 0, 3);
	objectAddRotationAccessibility(kObjectKeyDisgust, 20101, Common::Rect(3529, 151, 3600, 296), true, 52, 0);
	objectAddRotationAccessibility(kObjectKeyDisgust, 20101, Common::Rect(0, 151, 52, 296), true, 52, 0);
	objectAddPresentation(kObjectKeyDisgust);
	objectPresentationAddImageToRotation(kObjectKeyDisgust, 0, 20101, 0);
	objectAddPresentation(kObjectKeyDisgust);
	objectPresentationAddAnimationToRotation(kObjectKeyDisgust, 1, 20101, 1, 34, 12.5f, 4);
	objectPresentationSetAnimationOnRotation(kObjectKeyDisgust, 1, 0, kObject20003);
	objectPresentationShow(kObjectKeyDisgust, 0);
	objectPresentationShow(kObjectKeyDisgust, 1);
	objectAdd(kObjectRedfish, "GoldFish", "RH_GoldFish", 9);
	objectAddBagAnimation(kObjectRedfish, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectRedfish, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectRedfish, 22, 22, 0, 3, 0, 0, 3);
	objectAddRotationAccessibility(kObjectRedfish, 20201, Common::Rect(792, 141, 1140, 291), true, 52, 1);
	objectAddPresentation(kObjectRedfish);
	objectPresentationAddAnimationToRotation(kObjectRedfish, 0, 20201, 0, 30, 12.5f, 4);
	objectPresentationShow(kObjectRedfish, 0);
	objectAdd(kObjectDolphin, "", "", 1);
	objectAddPuzzleAccessibility(kObjectDolphin, kPuzzle20202, Common::Rect(374, 291, 480, 429), true,  52, 0);
	objectAddPuzzleAccessibility(kObjectDolphin, kPuzzle20202, Common::Rect(374, 291, 480, 429), false, 52, 0);
	objectAddPuzzleAccessibility(kObjectDolphin, kPuzzle20202, Common::Rect(  0, 420, 640, 464), true,  55, 9);
	objectAddRotationAccessibility(kObjectDolphin, 20201, Common::Rect(3524,  108, 3600, 247), false, 52, 2);
	objectAddRotationAccessibility(kObjectDolphin, 20201, Common::Rect(   0,  108,   50, 247), false, 52, 2);
	objectAddRotationAccessibility(kObjectDolphin, 20201, Common::Rect(3104, -322, 3600, 485), true,  53, 1);
	objectAddPresentation(kObjectDolphin);
	objectPresentationAddAnimationToPuzzle(kObjectDolphin, 0, kPuzzle20202, "RHS02N01P02S01", 0, Common::Point(175, 25), 1, 1000, 13, 12.5f, 4);
	objectAddPresentation(kObjectDolphin);
	objectPresentationAddAnimationToPuzzle(kObjectDolphin, 1, kPuzzle20202, "RHS02N01P02S04", 0, Common::Point(173, 21), 1, 1000, 13, 12.5f, 4);
	objectAddPresentation(kObjectDolphin);
	objectPresentationAddAnimationToPuzzle(kObjectDolphin, 2, kPuzzle20202, "RHS02N01P02S02", 0, Common::Point(178, 23), 1, 1000, 13, 12.5f, 4);
	objectAddPresentation(kObjectDolphin);
	objectPresentationAddAnimationToPuzzle(kObjectDolphin, 3, kPuzzle20203, "RHS02N01P02S03SD", 0, Common::Point(274, 94), 1, 1000, 20, 12.5f, 4);
	objectAdd(kObjectMedallion, "Necklace", "RH_Medallion", 1);
	objectAddBagAnimation(kObjectMedallion, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectMedallion, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectMedallion, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObject20204, "", "", 1);
	objectAddRotationAccessibility(kObject20204, 20202, Common::Rect(2343, 326, 2487, 408), true, 52, 0);
	objectAddPresentation(kObject20204);
	objectPresentationAddImageToRotation(kObject20204, 0, 20202, 0);
	objectAddPresentation(kObject20204);
	objectPresentationAddAnimationToPuzzle(kObject20204, 1, kPuzzle20204, "RHS02N02P01S01", 0, Common::Point(226, 186), 1, 1000, 20, 12.5f, 4);
	objectAdd(kObject20301, "", "", 1);
	objectAddRotationAccessibility(kObject20301, 20301, Common::Rect(377, -139, 611, 108), true, 53, 0);
	objectAddRotationAccessibility(kObject20301, 20301, Common::Rect(641, 368, 1121, 551), true, 52, 1);
	objectAddPuzzleAccessibility(kObject20301, kPuzzle20301, Common::Rect(240, 247, 386, 426), true, 52, 2);
	objectAddPuzzleAccessibility(kObject20301, kPuzzle20301, Common::Rect(  0, 420, 640, 464), true, 55, 9);
	objectAddPresentation(kObject20301);
	objectPresentationAddAnimationToRotation(kObject20301, 0, 20301, 0, 20, 12.5f, 4);
	objectPresentationAddAnimationToRotation(kObject20301, 0, 20301, 1, 20, 12.5f, 4);
	objectPresentationShow(kObject20301, 0);
	objectAddPresentation(kObject20301);
	objectPresentationAddAnimationToPuzzle(kObject20301, 1, kPuzzle20301, "RHS03N01P01S01SD", 0, Common::Point(280, 246), 1, 1000, 20, 12.5f, 4);
	objectAdd(kObject20302, "", "", 1);
	objectAddRotationAccessibility(kObject20302, 20302, Common::Rect(170, -169, 558, 193), true, 53, 0);
	objectAddRotationAccessibility(kObject20302, 20302, Common::Rect(3471, 343, 3600, 566), true, 52, 1);
	objectAddRotationAccessibility(kObject20302, 20302, Common::Rect(0, 343, 298, 566), true, 52, 1);
	objectAddPresentation(kObject20302);
	objectAddPresentation(kObject20302);
	objectPresentationAddAnimationToPuzzle(kObject20302, 1, kPuzzle20302, "RHS03N02P01S01SD", 0, Common::Point(294, 243), 1, 1000, 20, 12.5f, 4);
	objectAdd(kObject20303, "", "", 1);
	objectAddRotationAccessibility(kObject20303, 20303, Common::Rect(511, -235, 798, 298), false, 53, 0);
	objectAddRotationAccessibility(kObject20303, 20303, Common::Rect(1694, 350, 2116, 566), true, 53, 1);
	objectAddPuzzleAccessibility(kObject20303, kPuzzle20303, Common::Rect(243, 161, 376, 442), true, 52, 2);
	objectAddPresentation(kObject20303);
	objectAddPresentation(kObject20303);
	objectPresentationAddAnimationToPuzzle(kObject20303, 1, kPuzzle20303, "RHS03N03P01S01SD", 0, Common::Point(230, 124), 1, 1000, 20, 12.5f, 4);
	objectAdd(kObject20304, "", "", 1);
	objectAddRotationAccessibility(kObject20304, 20304, Common::Rect(2343, 326, 2487, 408), true, 52, 0);
	objectAddPresentation(kObject20304);
	objectPresentationAddImageToRotation(kObject20304, 0, 20304, 0);
	objectAddPresentation(kObject20304);
	objectPresentationAddAnimationToPuzzle(kObject20304, 1, kPuzzle20204, "RHS02N02P01S01", 0, Common::Point(226, 186), 1, 1000, 20, 12.5f, 4);
	objectAdd(kObject20401, "", "", 1);
	objectAddRotationAccessibility(kObject20401, 20401, Common::Rect(1114, -160, 1564, 390), true, 52, 0);
	objectAddPresentation(kObject20401);
	objectPresentationAddAnimationToRotation(kObject20401, 0, 20401, 0, 28, 12.5f, 4);
	objectPresentationShow(kObject20401, 0);
	objectPresentationPauseAnimation(kObject20401, 0);
	objectAdd(kObject20402, "", "", 1);
	objectAddPresentation(kObject20402);
	objectPresentationAddAnimationToPuzzle(kObject20402, 0, kPuzzle20401, "RHS04N01P01S01SD", 0, Common::Point(277, 124), 1, 1000, 20, 12.5f, 4);
	objectAddPresentation(kObject20402);
	objectPresentationAddAnimationToPuzzle(kObject20402, 1, kPuzzle20401, "RHS04N01P01S02", 0, Common::Point(216, 116), 1, 1000, 25, 12.5f, 4);
	objectPresentationShow(kObject20402, 1);
	objectAdd(kObjectAntiGCells2, "Anti gravitation cells", "RH_AntiG", 1);
	objectAddBagAnimation(kObjectAntiGCells2, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectAntiGCells2, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectAntiGCells2, 22, 22, 0, 3, 0, 0, 3);
	objectAddPuzzleAccessibility(kObjectAntiGCells2, kPuzzle20401, Common::Rect(250, 121, 345, 346), true, 52, 0);
	objectAdd(kObject20404, "", "", 1);
	objectAddRotationAccessibility(kObject20404, 20402, Common::Rect(2315, 207, 2409, 272), true, 52, 0);
	objectAddPresentation(kObject20404);
	objectPresentationAddImageToRotation(kObject20404, 0, 20402, 0);
	objectAddPresentation(kObject20404);
	objectPresentationAddAnimationToPuzzle(kObject20404, 1, kPuzzle20204, "RHS02N02P01S01", 0, Common::Point(226, 186), 1, 1000, 20, 12.5f, 4);
	objectAdd(kObject20501, "Daughter of the Rhine", "", 1);
	objectAddRotationAccessibility(kObject20501, 20501, Common::Rect(270, 317, 592, 516), true, 52, 0);
	objectAddRotationAccessibility(kObject20501, 20501, Common::Rect(341, -122, 553, 200), true, 53, 1);
	objectAddRotationAccessibility(kObject20501, 20501, Common::Rect(2053, -148, 2429, 324), false, 53, 2);
	objectAddPuzzleAccessibility(kObject20501, kPuzzle20502, Common::Rect(226, 184, 462, 458), false, 52, 3);
	objectAddPuzzleAccessibility(kObject20501, kPuzzle20502, Common::Rect(  0, 420, 640, 464), true,  55, 9);
	objectAddPresentation(kObject20501);
	objectPresentationAddImageToRotation(kObject20501, 0, 20501, 0);
	objectPresentationShow(kObject20501, 0);
	objectAddPresentation(kObject20501);
	objectPresentationAddAnimationToPuzzle(kObject20501, 1, kPuzzle20501, "RHS05N01P01S01SF", 0, Common::Point(197, 116), 1, 1000, 20, 12.5f, 4);
	objectAddPresentation(kObject20501);
	objectPresentationAddAnimationToPuzzle(kObject20501, 2, kPuzzle20502, "RHS05N01P02S01SD", 0, Common::Point(247, 161), 1, 1000, 20, 12.5f, 4);
	objectAddPresentation(kObject20501);
	objectPresentationAddAnimationToPuzzle(kObject20501, 3, kPuzzle20503, "RHS05N02P01S01", 0, Common::Point(184, 226), 1, 1000, 20, 12.5f, 4);
	objectAdd(kObject20502, "", "", 1);
	objectAddRotationAccessibility(kObject20502, 20503, Common::Rect(2343, 326, 2487, 408), true, 52, 0);
	objectAddPresentation(kObject20502);
	objectPresentationAddImageToRotation(kObject20502, 0, 20503, 0);
	objectAddPresentation(kObject20502);
	objectPresentationAddAnimationToPuzzle(kObject20502, 1, kPuzzle20204, "RHS02N02P01S01", 0, Common::Point(226, 186), 1, 1000, 20, 12.5f, 4);
	objectAddRotationAccessibility(kObjectDivingHelmet2, 20601, Common::Rect(337, -75, 557, 195), true, 53, 0);
	objectAdd(kObject20700, "Rhine Gold", "RH_Gold", 1);
	objectAddBagAnimation(kObject20700, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObject20700, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObject20700, 22, 22, 0, 3, 0, 0, 3);
	objectAddRotationAccessibility(kObject20700, 20701, Common::Rect(381, 61, 530, 231), true, 52, 0);
	soundAdd(23005, kSoundTypeAmbientMusic, "1746.was", _configuration.ambientMusic.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	rotationAddAmbientSound(20010, 23005, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(20020, 23005, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(20030, 23005, 100, 0, 1, 1, 10);
	soundAdd(23013, kSoundTypeAmbientMusic, "1747.was", _configuration.ambientMusic.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	puzzleAddAmbientSound(kPuzzle20000, 23013, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle20011, 23013, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle20021, 23013, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle20031, 23013, 100, 0, 1, 1, 10);
	soundAdd(23004, kSoundTypeAmbientMusic, "1748.was", _configuration.ambientMusic.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	rotationAddAmbientSound(20101, 23004, 90, 0, 1, 1, 10);
	soundAdd(23002, kSoundTypeAmbientMusic, "1749.was", _configuration.ambientMusic.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	rotationAddAmbientSound(20201, 23002, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(20202, 23002, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(20203, 23002, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle20202, 23002, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle20203, 23002, 100, 0, 1, 1, 10);
	soundAdd(23007, kSoundTypeAmbientMusic, "1750.was", _configuration.ambientMusic.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	rotationAddAmbientSound(20301, 23007, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(20302, 23007, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(20303, 23007, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(20304, 23007, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(20305, 23007, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle20301, 23007, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle20302, 23007, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle20303, 23007, 100, 0, 1, 1, 10);
	soundAdd(23003, kSoundTypeAmbientMusic, "1751.was", _configuration.ambientMusic.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	rotationAddAmbientSound(20401, 23003, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(20402, 23003, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(20403, 23003, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle20401, 23003, 100, 0, 1, 1, 10);
	soundAdd(23006, kSoundTypeAmbientMusic, "1752.was", _configuration.ambientMusic.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	rotationAddAmbientSound(20501, 23006, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(20503, 23006, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(20504, 23006, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle20501, 23006, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle20502, 23006, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle20503, 23006, 100, 0, 1, 1, 10);
	soundAdd(23001, kSoundTypeAmbientMusic, "1753.was", _configuration.ambientMusic.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	rotationAddAmbientSound(20601, 23001, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(20701, 23001, 100, 0, 1, 1, 10);
	soundAdd(23014, kSoundTypeAmbientEffect, "1754.wac", _configuration.ambientEffect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	rotationAdd3DSound(20101, 23014, 1, 1, 20, 100, 228.0f, 15);
	rotationSet3DSoundOff(20101, 23014);
	soundAdd(23009, kSoundTypeAmbientEffect, "1755.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(23010, kSoundTypeAmbientEffect, "1756.was", _configuration.ambientEffect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(23011, kSoundTypeAmbientEffect, "1757.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(23012, kSoundTypeAmbientEffect, "1758.wac", _configuration.ambientEffect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(22001, kSoundTypeDialog, "1626.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(22002, kSoundTypeDialog, "1627.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(22003, kSoundTypeDialog, "1628.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(20011, kSoundTypeDialog, "1759.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(20021, kSoundTypeDialog, "1760.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(20031, kSoundTypeDialog, "1761.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(20012, kSoundTypeDialog, "1762.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(20022, kSoundTypeDialog, "1763.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(20032, kSoundTypeDialog, "1764.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(20013, kSoundTypeDialog, "1765.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(20023, kSoundTypeDialog, "1766.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(20033, kSoundTypeDialog, "1767.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(20201, kSoundTypeDialog, "1768.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(20202, kSoundTypeDialog, "1769.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(20301, kSoundTypeDialog, "1770.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(20302, kSoundTypeDialog, "1771.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(20303, kSoundTypeDialog, "1772.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(20304, kSoundTypeDialog, "1773.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(20401, kSoundTypeDialog, "1774.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(20402, kSoundTypeDialog, "1775.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(20501, kSoundTypeDialog, "1776.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(20502, kSoundTypeDialog, "1777.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(20503, kSoundTypeDialog, "1778.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(20504, kSoundTypeDialog, "1779.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	varDefineByte(21000, 0);
	varDefineByte(21001, 0);
	varDefineByte(20200, 1);
	varDefineByte(20201, 1);
	varDefineByte(20202, 0);
	varDefineByte(20301, 0);
	varDefineByte(20500, 0);
	varDefineByte(20009, 0);
	varDefineWord(20000, 20000);
}

void ApplicationRing::initZoneFO() {
	debugC(1, kRingDebugLogic, "Initializing Siegmund zone (FO)...");

	setZoneAndEnableBag(kZoneFO);

	puzzleAdd(kPuzzle35001);
	puzzleAddBackgroundImage(kPuzzle35001, "FOS03N02P01S00.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle35002);
	puzzleAddBackgroundImage(kPuzzle35002, "FOS08N01P01.BMP", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle35003);
	puzzleAddBackgroundImage(kPuzzle35003, "FOS06N01P06.BMP", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle35004);
	puzzleAddBackgroundImage(kPuzzle35004, "FOS06N01P01.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle35005);
	puzzleAddBackgroundImage(kPuzzle35005, "FOS06N01P05.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle35006);
	puzzleAddBackgroundImage(kPuzzle35006, "FOS06N01P02.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle35007);
	puzzleAddBackgroundImage(kPuzzle35007, "FOS06N01P03.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle35008);
	puzzleAddBackgroundImage(kPuzzle35008, "FOS06N01P04.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle35009);
	puzzleAddBackgroundImage(kPuzzle35009, "FOS06N02P01.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle35010);
	puzzleAddBackgroundImage(kPuzzle35010, "FOS07N03P01.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle35011);
	puzzleAddBackgroundImage(kPuzzle35011, "FOS03N01P01.BMP", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle35019);
	puzzleAddBackgroundImage(kPuzzle35019, "FOS00N04P01.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle35020);
	puzzleAddBackgroundImage(kPuzzle35020, "FOS07N04P01.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle35100);
	puzzleAddBackgroundImage(kPuzzle35100, "FOS00N03P02.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle35101);
	puzzleAddBackgroundImage(kPuzzle35101, "FOS00N03P02L02.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle35103);
	puzzleAddBackgroundImage(kPuzzle35103, "FOS01N01P07.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle35104);
	puzzleAddBackgroundImage(kPuzzle35104, "FOS01N01P03.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle35105);
	puzzleAddBackgroundImage(kPuzzle35105, "FOS01N01P04.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle35110);
	puzzleAddBackgroundImage(kPuzzle35110, "FOS01N01P09.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle35106);
	puzzleAddBackgroundImage(kPuzzle35106, "FOS01N01P05.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle35107);
	puzzleAddBackgroundImage(kPuzzle35107, "FOS01N01P06.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle35108);
	puzzleAddBackgroundImage(kPuzzle35108, "FOS01N01P10.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle35111);
	puzzleAddBackgroundImage(kPuzzle35111, "FOS01N01P02.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle35109);
	puzzleAddBackgroundImage(kPuzzle35109, "FOS01N01P11.bmp", Common::Point(0, 16), true);
	rotationAdd(30001, "FOS00N01", 0, 1);
	rotationAdd(30002, "FOS00N02", 0, 0);
	rotationAdd(30003, "FOS00N03", 0, 1);
	rotationSetComBufferLength(30003, 1400000);
	rotationAdd(30004, "FOS00N04", 0, 0);
	rotationAdd(30005, "FOS00N05", 0, 0);
	rotationAdd(30006, "FOS00N06", 0, 0);
	rotationAdd(30008, "FOS00N08", 0, 1);
	rotationAdd(30009, "FOS00N09", 0, 1);
	rotationAdd(30010, "FOS00N10", 0, 1);
	rotationAdd(30011, "FOS00N11", 0, 3);
	rotationSetComBufferLength(30011, 1500000);
	rotationAdd(30012, "FOS00N12", 0, 0);
	rotationAdd(30101, "FOS01N01", 0, 4);
	rotationSetComBufferLength(30101, 2000000);
	rotationAdd(30301, "FOS03N01", 0, 1);
	rotationAdd(30302, "FOS03N02", 0, 0);
	rotationAdd(30303, "FOS03N03", 0, 0);
	rotationAdd(30401, "FOS04N01", 0, 0);
	rotationAdd(30402, "FOS04N02", 0, 1);
	rotationAdd(30501, "FOS05N01", 0, 3);
	rotationAdd(30601, "FOS06N01", 0, 1);
	rotationAdd(30602, "FOS06N02", 0, 1);
	rotationSetComBufferLength(30602, 1750000);
	rotationAdd(30701, "FOS07N01", 0, 0);
	rotationAdd(30702, "FOS07N02", 0, 1);
	rotationAdd(30703, "FOS07N03", 0, 1);
	rotationAdd(30704, "FOS07N04", 0, 0);
	rotationAdd(30801, "FOS08N01", 0, 0);
	rotationAddMovabilityToRotation(30001, 30003, "1219", Common::Rect(2787, -143, 3113, 169), true, 53, 0);
	rotationSetMovabilityToRotation(30001, 0, 247.0f, 0, 85.3f, 0, 2, 247.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30001, 30004, "1219", Common::Rect(3406, -125, 3599, 73), true, 53, 0);
	rotationSetMovabilityToRotation(30001, 1, 312.0f, 0, 85.3f, 0, 2, 312.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30001, 30004, "1219", Common::Rect(0, -125, 72, 73), true, 53, 0);
	rotationSetMovabilityToRotation(30001, 2, 312.0f, 0, 85.3f, 0, 2, 312.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30002, 30003, "1220", Common::Rect(272, -81, 437, 118), true, 53, 0);
	rotationSetMovabilityToRotation(30002, 0, 356.0f, 0, 85.3f, 0, 2, 356.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30002, 30004, "1220", Common::Rect(89, -104, 254, 45), true, 53, 0);
	rotationSetMovabilityToRotation(30002, 1, 338.0f, 0, 85.3f, 0, 2, 338.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30003, 30001, "1221", Common::Rect(954, -129, 1320, 190), true, 53, 0);
	rotationSetMovabilityToRotation(30003, 0, 75.0f, 0, 85.3f, 0, 2, 75.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30003, 30002, "1222", Common::Rect(2002, -97, 2364, 156), true, 53, 0);
	rotationSetMovabilityToRotation(30003, 1, 167.0f, 0, 85.3f, 0, 2, 167.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30003, 30004, "1223", Common::Rect(45, -104, 261, 136), true, 53, 0);
	rotationSetMovabilityToRotation(30003, 2, 330.0f, 0, 85.3f, 0, 2, 330.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30004, 30001, "1224", Common::Rect(1620, -75, 1773, 52), true, 53, 0);
	rotationSetMovabilityToRotation(30004, 0, 128.0f, 0, 85.3f, 0, 2, 128.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30004, 30002, "1224", Common::Rect(1928, -48, 2037, 38), true, 53, 0);
	rotationSetMovabilityToRotation(30004, 1, 149.0f, 0, 85.3f, 0, 2, 149.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30004, 30003, "1224", Common::Rect(1791, -54, 1900, 73), true, 53, 0);
	rotationSetMovabilityToRotation(30004, 2, 138.0f, 0, 85.3f, 0, 2, 138.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30004, 30005, "1225", Common::Rect(40, -115, 274, 146), true, 53, 0);
	rotationSetMovabilityToRotation(30004, 3, 331.0f, 0, 85.3f, 0, 2, 331.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30004, 30009, "1226", Common::Rect(402, -116, 601, 75), true, 53, 0);
	rotationSetMovabilityToRotation(30004, 4, 0, 0, 85.3f, 0, 2, 0, 0, 85.3f);
	rotationAddMovabilityToPuzzle(30004, kPuzzle35019, "", Common::Rect(1033, -116, 1233, 120), true, 52, 0);
	rotationSetMovabilityToPuzzle(30004, 5, 70.0f, 0, 85.3f, 0, 2);
	rotationAddMovabilityToRotation(30005, 30004, "1227", Common::Rect(1587, -134, 1829, 87), true, 53, 0);
	rotationSetMovabilityToRotation(30005, 0, 123.0f, 0, 85.3f, 0, 2, 123.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30005, 30006, "1228", Common::Rect(3311, -127, 3513, 113), true, 53, 0);
	rotationSetMovabilityToRotation(30005, 1, 292.0f, 0, 85.3f, 0, 2, 356.0f, -15.0f, 85.3f);
	rotationAddMovabilityToRotation(30006, 30005, "1229", Common::Rect(1817, -179, 2149, 144), true, 53, 0);
	rotationSetMovabilityToRotation(30006, 0, 152.0f, 0, 85.3f, 0, 2, 152.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30006, 30008, "1230", Common::Rect(249, -385, 580, 26), true, 53, 0);
	rotationSetMovabilityToRotation(30006, 1, 356.0f, -15.0f, 85.3f, 0, 2, 356.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30008, 30006, "1231", Common::Rect(2037, 130, 2455, 362), true, 53, 0);
	rotationSetMovabilityToRotation(30008, 0, 180.0f, 15.0f, 85.3f, 0, 2, 180.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30008, 30301, "1232", Common::Rect(358, -43, 567, 113), true, 53, 0);
	rotationSetMovabilityToRotation(30008, 1, 0, 0, 85.3f, 0, 2, 0, 0, 85.3f);
	rotationAddMovabilityToRotation(30009, 30004, "1233", Common::Rect(2536, -68, 2698, 87), true, 53, 0);
	rotationSetMovabilityToRotation(30009, 0, 216.0f, 0, 85.3f, 0, 2, 216.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30009, 30010, "1234", Common::Rect(924, -136, 1121, 123), true, 53, 0);
	rotationSetMovabilityToRotation(30009, 1, 58.0f, 0, 85.3f, 0, 2, 58.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30009, 30005, "1233", Common::Rect(2726, -88, 2940, 90), true, 53, 0);
	rotationSetMovabilityToRotation(30009, 2, 300.0f, 0, 85.3f, 0, 2, 58.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30010, 30009, "1235", Common::Rect(2583, -137, 2845, 160), true, 53, 0);
	rotationSetMovabilityToRotation(30010, 0, 230.0f, 0, 85.3f, 0, 2, 230.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30010, 30011, "1236", Common::Rect(1469, -47, 1668, 134), true, 53, 0);
	rotationSetMovabilityToRotation(30010, 1, 106.0f, 0, 85.3f, 0, 2, 106.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30010, 30012, "1237", Common::Rect(573, -52, 703, 108), true, 53, 0);
	rotationSetMovabilityToRotation(30010, 2, 21.0f, 0, 85.3f, 0, 2, 21.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30011, 30010, "1238", Common::Rect(3107, -45, 3309, 163), true, 53, 0);
	rotationSetMovabilityToRotation(30011, 0, 265.0f, 0, 85.3f, 0, 2, 265.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30011, 30401, "1239", Common::Rect(3517, 195, 3600, 418), true, 53, 0);
	rotationSetMovabilityToRotation(30011, 1, 344.0f, 15.0f, 85.3f, 0, 2, 330.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30011, 30401, "1239", Common::Rect(0, 195, 769, 418), true, 53, 0);
	rotationSetMovabilityToRotation(30011, 2, 344.0f, 15.0f, 85.3f, 0, 2, 330.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30012, 30010, "1240", Common::Rect(2290, -64, 2599, 111), true, 53, 0);
	rotationSetMovabilityToRotation(30012, 0, 207.0f, 0, 85.3f, 0, 2, 207.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30012, 30501, "1241", Common::Rect(543, -83, 768, 329), true, 53, 0);
	rotationSetMovabilityToRotation(30012, 1, 21.0f, 6.0f, 85.3f, 0, 2, 21.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30101, 30003, "1242", Common::Rect(3594, -76, 3599, 423), true, 53, 0);
	rotationSetMovabilityToRotation(30101, 0, 327.0f, 15.0f, 85.3f, 0, 2, 0, 0, 85.3f);
	rotationAddMovabilityToRotation(30101, 30003, "1242", Common::Rect(0, -76, 469, 423), true, 53, 0);
	rotationSetMovabilityToRotation(30101, 1, 327.0f, 15.0f, 85.3f, 0, 2, 0, 0, 85.3f);
	rotationAddMovabilityToRotation(30301, 30008, "1243", Common::Rect(1979, -71, 2418, 214), true, 53, 0);
	rotationSetMovabilityToRotation(30301, 0, 180.0f, 15.0f, 85.3f, 0, 2, 180.0f, 15.0f, 85.3f);
	rotationAddMovabilityToPuzzle(30301, kPuzzle35011, "", Common::Rect(1907, -127, 1965, 13), true, 52, 0);
	rotationSetMovabilityToPuzzle(30301, 1, 150.0f, 3.0f, 85.3f, 0, 2);
	rotationAddMovabilityToPuzzle(30303, kPuzzle35001, "", Common::Rect(2023, 62, 2255, 252), true, 52, 0);
	rotationSetMovabilityToPuzzle(30303, 0, 170.0f, 15.0f, 85.3f, 0, 2);
	rotationAddMovabilityToRotation(30401, 30402, "1244", Common::Rect(442, -134, 782, 153), true, 53, 0);
	rotationSetMovabilityToRotation(30401, 0, 354.0f, 0, 85.3f, 0, 2, 261.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30401, 30011, "1245", Common::Rect(2074, 141, 2808, 345), true, 53, 0);
	rotationSetMovabilityToRotation(30401, 1, 200.0f, 15.0f, 85.3f, 0, 2, 190.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30401, 30801, "1246", Common::Rect(1014, -104, 1239, 41), true, 53, 0);
	rotationSetMovabilityToRotation(30401, 2, 70.0f, 0, 85.3f, 0, 2, 73.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30402, 30401, "1247", Common::Rect(1745, -59, 2100, 104), true, 53, 0);
	rotationSetMovabilityToRotation(30402, 0, 170.0f, 0, 85.3f, 0, 2, 186.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30402, 30801, "1248", Common::Rect(1318, -92, 1504, 17), true, 53, 0);
	rotationSetMovabilityToRotation(30402, 1, 92.0f, 0, 85.3f, 0, 2, 80.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30402, 30501, "1249", Common::Rect(2967, -102, 3237, 190), true, 53, 0);
	rotationSetMovabilityToRotation(30402, 2, 261.0f, 0, 85.3f, 0, 2, 30.0f, 15.0f, 85.3f);
	rotationSetMovabilityOnOrOffDisableHotspot(30402, 2, 2);
	rotationAddMovabilityToRotation(30501, 30012, "1250", Common::Rect(1981, -64, 2218, 324), true, 53, 0);
	rotationSetMovabilityToRotation(30501, 0, 165.0f, 8.0f, 85.3f, 0, 2, 207.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30501, 30601, "1251", Common::Rect(390, -247, 766, 179), true, 53, 0);
	rotationSetMovabilityToRotation(30501, 1, 2.0f, -15.0f, 85.3f, 0, 2, 207.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30601, 30602, "1253", Common::Rect(1571, -68, 1916, 132), true, 53, 0);
	rotationSetMovabilityToRotation(30601, 0, 142.0f, -15.0f, 85.3f, 0, 2, 256.0f, -15.0f, 85.3f);
	rotationAddMovabilityToRotation(30601, 30602, "1253", Common::Rect(1274, 73, 1778, 525), true, 53, 0);
	rotationSetMovabilityToRotation(30601, 1, 142.0f, -15.0f, 85.3f, 0, 2, 256.0f, -15.0f, 85.3f);
	rotationAddMovabilityToRotation(30601, 30501, "1254", Common::Rect(2654, 191, 3427, 436), true, 53, 0);
	rotationSetMovabilityToRotation(30601, 2, 260.0f, 15.0f, 85.3f, 0, 2, 81.0f, 15.0f, 85.3f);
	rotationAddMovabilityToPuzzle(30601, kPuzzle35003, "", Common::Rect(3529, -177, 3599, 108), true, 52, 0);
	rotationSetMovabilityToPuzzle(30601, 3, 315.0f, 1.0f, 85.3f, 0, 2);
	rotationAddMovabilityToPuzzle(30601, kPuzzle35003, "", Common::Rect(0, -177, 96, 108), true, 52, 0);
	rotationSetMovabilityToPuzzle(30601, 4, 315.0f, 1.0f, 85.3f, 0, 2);
	rotationAddMovabilityToPuzzle(30601, kPuzzle35004, "", Common::Rect(2077, -43, 2466, 251), true, 52, 0);
	rotationSetMovabilityToPuzzle(30601, 5, 180.0f, 13.0f, 85.3f, 0, 2);
	rotationAddMovabilityToPuzzle(30601, kPuzzle35005, "", Common::Rect(530, -193, 703, 102), true, 52, 0);
	rotationSetMovabilityToPuzzle(30601, 6, 17.0f, 0, 85.3f, 0, 2);
	rotationAddMovabilityToPuzzle(30601, kPuzzle35006, "", Common::Rect(174, -111, 451, 52), true, 52, 0);
	rotationSetMovabilityToPuzzle(30601, 7, 347.0f, 0, 85.3f, 0, 2);
	rotationAddMovabilityToPuzzle(30601, kPuzzle35007, "", Common::Rect(1775, 127, 2065, 375), true, 52, 0);
	rotationSetMovabilityToPuzzle(30601, 8, 145.0f, 27.0f, 85.3f, 0, 2);
	rotationAddMovabilityToPuzzle(30601, kPuzzle35008, "", Common::Rect(251, 123, 305, 181), true, 52, 0);
	rotationSetMovabilityToPuzzle(30601, 9, 343.0f, 14.0f, 85.3f, 0, 2);
	rotationAddMovabilityToRotation(30602, 30601, "1255", Common::Rect(2536, 303, 3188, 511), true, 53, 0);
	rotationSetMovabilityToRotation(30602, 0, 245.0f, 15.0f, 85.3f, 0, 2, 0, 0, 85.3f);
	rotationAddMovabilityToRotation(30602, 30701, "1256", Common::Rect(1423, -125, 1617, 139), true, 53, 0);
	rotationSetMovabilityToRotation(30602, 1, 110.0f, 0, 85.3f, 0, 2, 89.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30701, 30702, "1257", Common::Rect(1269, -69, 1436, 261), true, 53, 0);
	rotationSetMovabilityToRotation(30701, 0, 87.0f, 0, 85.3f, 0, 2, 90.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30701, 30602, "1258", Common::Rect(2990, -174, 3348, 225), true, 53, 0);
	rotationSetMovabilityToRotation(30701, 1, 268.0f, 0, 85.3f, 0, 2, 254.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30702, 30703, "1259", Common::Rect(1451, 144, 1773, 439), true, 53, 0);
	rotationSetMovabilityToRotation(30702, 0, 116.0f, 15.0f, 85.3f, 0, 2, 0, 0, 85.3f);
	rotationAddMovabilityToRotation(30702, 30701, "1260", Common::Rect(3091, -71, 3202, 209), true, 53, 0);
	rotationSetMovabilityToRotation(30702, 1, 271.0f, 0, 85.3f, 0, 2, 270.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30702, 30704, "1261", Common::Rect(1265, -92, 1455, 219), true, 53, 0);
	rotationSetMovabilityToRotation(30702, 2, 90.0f, 0, 85.3f, 0, 2, 90.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30703, 30702, "1262", Common::Rect(219, -530, 639, -404), true, 53, 0);
	rotationSetMovabilityToRotation(30703, 0, 284.0f, -15.0f, 85.3f, 0, 2, 284.0f, 0, 85.3f);
	rotationAddMovabilityToPuzzle(30703, kPuzzle35010, "", Common::Rect(105, 266, 857, 456), true, 52, 0);
	rotationSetMovabilityToPuzzle(30703, 1, 0, 15.0f, 85.3f, 0, 2);
	rotationAddMovabilityToRotation(30704, 30702, "1263", Common::Rect(3070, -68, 3253, 225), true, 53, 0);
	rotationSetMovabilityToRotation(30704, 0, 273.0f, 0, 85.3f, 0, 2, 270.0f, 0, 85.3f);
	rotationAddMovabilityToPuzzle(30704, kPuzzle35020, "", Common::Rect(1214, -8, 1462, 176), true, 52, 0);
	rotationSetMovabilityToPuzzle(30704, 1, 89.0f, 7.0f, 85.3f, 0, 2);
	rotationAddMovabilityToRotation(30801, 30401, "1264", Common::Rect(2847, -75, 2991, 17), true, 53, 0);
	rotationSetMovabilityToRotation(30801, 0, 251.0f, 0, 85.3f, 0, 2, 186.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(30801, 30402, "1265", Common::Rect(3183, -85, 3280, 5), true, 53, 0);
	rotationSetMovabilityToRotation(30801, 1, 280.0f, 0, 85.3f, 0, 2, 261.0f, 0, 85.3f);
	rotationAddMovabilityToPuzzle(30801, kPuzzle35002, "", Common::Rect(984, -69, 1840, 256), true, 52, 0);
	rotationSetMovabilityToPuzzle(30801, 2, 93.0f, 15.0f, 85.3f, 0, 2);
	puzzleAddMovabilityToRotation(kPuzzle35001, 30302, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle35001, 0, 170.0f, 15.0f, 85.3f);
	puzzleAddMovabilityToRotation(kPuzzle35002, 30801, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle35002, 0, 93.0f, 15.0f, 85.3f);
	puzzleAddMovabilityToRotation(kPuzzle35003, 30601, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle35003, 0, 315.0f, 1.0f, 85.3f);
	puzzleAddMovabilityToRotation(kPuzzle35004, 30601, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle35004, 0, 180.0f, 13.0f, 85.3f);
	puzzleAddMovabilityToRotation(kPuzzle35005, 30601, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle35005, 0, 17.0f, 0, 85.3f);
	puzzleAddMovabilityToRotation(kPuzzle35006, 30601, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle35006, 0, 347.0f, 0, 85.3f);
	puzzleAddMovabilityToRotation(kPuzzle35007, 30601, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle35007, 0, 145.0f, 15.0f, 85.3f);
	puzzleAddMovabilityToRotation(kPuzzle35008, 30601, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle35008, 0, 343.0f, 0, 85.3f);
	puzzleAddMovabilityToRotation(kPuzzle35009, 30602, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle35009, 0, 175.0f, -23.0f, 85.3f);
	puzzleAddMovabilityToRotation(kPuzzle35010, 30703, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle35010, 0, 0, 15.0f, 85.3f);
	puzzleAddMovabilityToRotation(kPuzzle35011, 30301, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle35011, 0, 150.0f, 3.0f, 85.3f);
	puzzleAddMovabilityToRotation(kPuzzle35019, 30004, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle35019, 0, 70.0f, 0, 85.3f);
	puzzleAddMovabilityToRotation(kPuzzle35020, 30704, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle35020, 0, 89.0f, 7.0f, 85.3f);
	puzzleAddMovabilityToPuzzle(kPuzzle35109, kPuzzle35111, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleAddMovabilityToRotation(kPuzzle35100, 30003, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	objectAdd(kObjectWolfInstinct, "Wolf Vision", "FO_WolfVision", 1);
	objectAddBagAnimation(kObjectWolfInstinct, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectWolfInstinct, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectWolfInstinct, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObjectIngot, "Panel", "FO_Panel01_", 1);
	objectAddBagAnimation(kObjectIngot, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectIngot, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectIngot, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObjectIngot2, "Panel", "FO_Panel01_", 1);
	objectAddBagAnimation(kObjectIngot2, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectIngot2, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectIngot2, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObjectIngot3, "Panel", "FO_Panel01_", 1);
	objectAddBagAnimation(kObjectIngot3, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectIngot3, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectIngot3, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObjectLogeTear2, "Loge's Tear", "NI_Tear", 1);
	objectAddBagAnimation(kObjectLogeTear2, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectLogeTear2, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectLogeTear2, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObject30001, "", "", 1);
	objectAdd(kObjectWolfBadge, "Medallion", "FO_WolfLogo", 1);
	objectAddBagAnimation(kObjectWolfBadge, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectWolfBadge, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectWolfBadge, 22, 22, 0, 3, 0, 0, 3);
	objectAddPresentation(kObject30001);
	objectPresentationAddImageToPuzzle(kObject30001, 0, kPuzzle35011, "FOS03N01P01L01.bmp", Common::Point(170, 133), true, 1, 1000);
	objectAddPresentation(kObject30001);
	objectPresentationAddImageToPuzzle(kObject30001, 1, kPuzzle35011, "FOS03N01P01L02.bmp", Common::Point(266, 178), true, 1, 1000);
	objectAddPresentation(kObject30001);
	objectPresentationAddImageToPuzzle(kObject30001, 2, kPuzzle35011, "FOS03N01P01L03.bmp", Common::Point(268, 211), true, 1, 1000);
	objectAddPresentation(kObject30001);
	objectPresentationAddImageToPuzzle(kObject30001, 3, kPuzzle35011, "FOS03N01P01L04.bmp", Common::Point(266, 249), true, 1, 1000);
	objectAddPresentation(kObject30001);
	objectPresentationAddImageToPuzzle(kObject30001, 4, kPuzzle35011, "FOS03N01P01L05.bmp", Common::Point(293, 332), true, 1, 1000);
	objectAddPuzzleAccessibility(kObject30001, kPuzzle35011, Common::Rect(260, 171, 376, 374), true, 52, 0);
	objectAddPuzzleAccessibility(kObject30001, kPuzzle35011, Common::Rect(273, 182, 361, 202), true, 52, 1);
	objectAddPuzzleAccessibility(kObject30001, kPuzzle35011, Common::Rect(276, 218, 359, 238), true, 52, 2);
	objectAddPuzzleAccessibility(kObject30001, kPuzzle35011, Common::Rect(275, 257, 361, 312), true, 52, 3);
	objectAddPuzzleAccessibility(kObject30001, kPuzzle35011, Common::Rect(292, 333, 345, 388), true, 52, 4);
	objectSetAccessibilityOff(kObject30001, 1, 4);
	objectAdd(kObjectPatience, "Saturn", "FO_Scroll01_", 1);
	objectAddBagAnimation(kObjectPatience, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectPatience, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectPatience, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObjectMovementAndIntuition, "Mercury", "FO_Scroll01_", 1);
	objectAddBagAnimation(kObjectMovementAndIntuition, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectMovementAndIntuition, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectMovementAndIntuition, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObjectLove, "Venus", "FO_Scroll01_", 1);
	objectAddBagAnimation(kObjectLove, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectLove, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectLove, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObjectImagination, "Moon", "FO_Scroll01_", 1);
	objectAddBagAnimation(kObjectImagination, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectImagination, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectImagination, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObjectDestruction, "Mars", "FO_Scroll01_", 1);
	objectAddBagAnimation(kObjectDestruction, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectDestruction, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectDestruction, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObjectJudgementAndDirection, "Jupiter", "FO_Scroll01_", 1);
	objectAddBagAnimation(kObjectJudgementAndDirection, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectJudgementAndDirection, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectJudgementAndDirection, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObjectWill, "Sun", "FO_Scroll01_", 1);
	objectAddBagAnimation(kObjectWill, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectWill, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectWill, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObject30002, "", "", 1);
	objectAddRotationAccessibility(kObject30002, 30301, Common::Rect(1858, 280, 2021, 366), true, 52, 0);
	objectAdd(kObject30003, "", "", 1);
	objectAddRotationAccessibility(kObject30003, 30301, Common::Rect(2371, 280, 2540, 366), true, 52, 0);
	objectAdd(kObject30004, "", "", 1);
	objectAddRotationAccessibility(kObject30004, 30301, Common::Rect(2884, 280, 3048, 366), true, 52, 0);
	objectAdd(kObject30005, "", "", 1);
	objectAddRotationAccessibility(kObject30005, 30301, Common::Rect(3401, 280, 3561, 366), true, 52, 0);
	objectAdd(kObject30006, "", "", 1);
	objectAddRotationAccessibility(kObject30006, 30301, Common::Rect(311, 280, 481, 366), true, 52, 0);
	objectAdd(kObject30007, "", "", 1);
	objectAddRotationAccessibility(kObject30007, 30301, Common::Rect(827, 280, 998, 366), true, 52, 0);
	objectAdd(kObject30008, "", "", 1);
	objectAddRotationAccessibility(kObject30008, 30301, Common::Rect(1337, 280, 1509, 366), true, 52, 0);
	objectAdd(kObject30027, "", "", 1);
	objectAddPuzzleAccessibility(kObject30027, kPuzzle35001, Common::Rect(410, 230, 466, 273), true, 53, 0);
	objectAdd(kObject30016, "", "", 4);
	objectSetPassiveDrawCursor(kObject30016, 20, 20, 0, 3, 0, 0, 3);
	objectSetActiveDrawCursor(kObject30016, 20, 20, 0, 3, 0, 0, 3);

	for (uint32 i = 0; i < 51; i++) {
		objectAddPresentation(kObject30016);
		objectPresentationAddImageToPuzzle(kObject30016, i, kPuzzle35001, Common::String::format("FOS03N02P01S01.%04d.bmp", i + 1), Common::Point(341, 150), true, 1, 1000);
	}

	objectPresentationShow(kObject30016, 25);
	objectAddPuzzleAccessibility(kObject30016, kPuzzle35001, Common::Rect(349, 164, 532, 332), true, 10000, 0);
	objectAdd(kObjectBerries, "Berries", "FO_Berries", 1);
	objectAddBagAnimation(kObjectBerries, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectBerries, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectBerries, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObjectSleepingBerries, "Poison berries", "FO_BerriesPoison", 1);
	objectAddBagAnimation(kObjectSleepingBerries, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectSleepingBerries, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectSleepingBerries, 22, 22, 0, 3, 0, 0, 3);
	objectAddPuzzleAccessibility(kObjectBerries, kPuzzle35002, Common::Rect(232, 322, 346, 436), true, 52, 0);
	objectAddPuzzleAccessibility(kObjectBerries, kPuzzle35002, Common::Rect(110, 120, 492, 305), true, 52, 1);
	objectAddPuzzleAccessibility(kObjectBerries, kPuzzle35002, Common::Rect(116, 296, 205, 426), true, 52, 2);
	objectAddPuzzleAccessibility(kObjectBerries, kPuzzle35002, Common::Rect(348, 251, 636, 414), true, 52, 3);
	objectAddPuzzleAccessibility(kObjectBerries, kPuzzle35002, Common::Rect(505, 194, 556, 238), true, 52, 4);
	objectAddPresentation(kObjectBerries);
	objectPresentationAddImageToPuzzle(kObjectBerries, 0, kPuzzle35002, "FOS08N01P01L01.BMP", Common::Point(508, 197), true, 1, 1000);
	objectAddPresentation(kObjectBerries);
	objectPresentationAddImageToPuzzle(kObjectBerries, 1, kPuzzle35002, "FOS08N01P01L02.BMP", Common::Point(0, 16), true, 1, 1000);
	objectAddPresentation(kObjectBerries);
	objectPresentationAddImageToPuzzle(kObjectBerries, 2, kPuzzle35002, "FOS08N01P01L03.BMP", Common::Point(510, 198), true, 1, 1000);
	objectAddPresentation(kObjectBerries);
	objectPresentationAddImageToPuzzle(kObjectBerries, 3, kPuzzle35002, "FOS08N01P01L04.BMP", Common::Point(209, 319), true, 1, 1000);
	objectAdd(kObjectBerriesJuice, "Berries juice", "FO_BerriesJuice", 1);
	objectAddBagAnimation(kObjectBerriesJuice, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectBerriesJuice, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectBerriesJuice, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObjectSleepingPotion2, "Poison berries juice", "FO_BerriesPoisonJuice", 1);
	objectAddBagAnimation(kObjectSleepingPotion2, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectSleepingPotion2, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectSleepingPotion2, 22, 22, 0, 3, 0, 0, 3);
	objectAddPuzzleAccessibility(kObjectBerries, kPuzzle35005, Common::Rect(155, 283, 407, 447), true, 52, 5);
	objectAddPresentation(kObjectBerries);
	objectPresentationAddImageToPuzzle(kObjectBerries, 4, kPuzzle35005, "FOS06N01P05L01.BMP", Common::Point(209, 281), true, 1, 1000);
	objectAddPresentation(kObjectBerries);
	objectPresentationAddImageToPuzzle(kObjectBerries, 5, kPuzzle35005, "FOS06N01P05L02.BMP", Common::Point(243, 402), true, 1, 1000);
	objectAdd(kObjectSleepingPotion, "", "", 1);
	objectAddPresentation(kObjectSleepingPotion);
	objectPresentationAddAnimationToRotation(kObjectSleepingPotion, 0, 30003, 0, 26, 20.0f, 4);
	objectPresentationSetAnimationOnRotation(kObjectSleepingPotion, 0, 0, kObject30006);
	objectPresentationShow(kObjectSleepingPotion, 0);
	objectAddRotationAccessibility(kObjectSleepingPotion, 30003, Common::Rect(1931, 76, 2112, 263), true, 52, 0);
	objectAddRotationAccessibility(kObjectSleepingPotion, 30003, Common::Rect(1476, -50, 1741, 245), true, 53, 1);
	objectAdd(kObjectHare, "Rabbit hare", "FO_Hares", 1);
	objectAddBagAnimation(kObjectHare, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectHare, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectHare, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObjectBow, "Hunting bow", "FO_HuntingBow", 1);
	objectAddBagAnimation(kObjectBow, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectBow, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectBow, 22, 22, 0, 3, 0, 0, 3);
	objectAddPuzzleAccessibility(kObjectBow, kPuzzle35003, Common::Rect(240, 200, 400, 410), true, 52, 0);
	objectAddPuzzleAccessibility(kObjectBow, kPuzzle35003, Common::Rect(230, 219, 278, 275), true, 52, 1);
	objectAddPuzzleAccessibility(kObjectBow, kPuzzle35003, Common::Rect(237, 280, 273, 334), true, 52, 2);
	objectAddPuzzleAccessibility(kObjectBow, kPuzzle35003, Common::Rect(353, 228, 387, 283), true, 52, 3);
	objectAddPuzzleAccessibility(kObjectBow, kPuzzle35003, Common::Rect(384, 226, 415, 281), true, 52, 4);
	objectAddPuzzleAccessibility(kObjectBow, kPuzzle35003, Common::Rect(328, 286, 366, 452), true, 52, 5);
	objectAddPuzzleAccessibility(kObjectBow, kPuzzle35003, Common::Rect(277, 174, 326, 447), true, 52, 6);
	objectAddPresentation(kObjectBow);
	objectPresentationAddImageToPuzzle(kObjectBow, 0, kPuzzle35003, "FOS06N01P06L01.bmp", Common::Point(72, 16), true, 1, 1000);
	objectAddPresentation(kObjectBow);
	objectPresentationAddImageToPuzzle(kObjectBow, 1, kPuzzle35003, "FOS06N01P06L02.bmp", Common::Point(233, 219), true, 1, 1000);
	objectAddPresentation(kObjectBow);
	objectPresentationAddImageToPuzzle(kObjectBow, 2, kPuzzle35003, "FOS06N01P06L03.bmp", Common::Point(241, 273), true, 1, 1000);
	objectAddPresentation(kObjectBow);
	objectPresentationAddImageToPuzzle(kObjectBow, 3, kPuzzle35003, "FOS06N01P06L04.bmp", Common::Point(357, 221), true, 1, 1000);
	objectAddPresentation(kObjectBow);
	objectPresentationAddImageToPuzzle(kObjectBow, 4, kPuzzle35003, "FOS06N01P06L05.bmp", Common::Point(387, 222), true, 1, 1000);
	objectAddPresentation(kObjectBow);
	objectPresentationAddImageToPuzzle(kObjectBow, 5, kPuzzle35003, "FOS06N01P06L06.bmp", Common::Point(326, 282), true, 1, 1000);
	objectAddPresentation(kObjectBow);
	objectPresentationAddImageToPuzzle(kObjectBow, 6, kPuzzle35003, "FOS06N01P06L07.bmp", Common::Point(274, 170), true, 1, 1000);
	objectAdd(kObject30028, "", "", 1);
	objectAddPuzzleAccessibility(kObject30028, kPuzzle35004, Common::Rect(450, 265, 512, 340), true, 52, 0);
	objectAddPuzzleAccessibility(kObject30028, kPuzzle35004, Common::Rect( 70, 350, 240, 415), true, 52, 1);
	objectAddPresentation(kObject30028);
	objectPresentationAddImageToPuzzle(kObject30028, 0, kPuzzle35004, "FOS06N01P01L01.bmp", Common::Point(0, 339), true, 1, 1000);
	objectAddPresentation(kObject30028);
	objectPresentationAddAnimationToPuzzle(kObject30028, 1, kPuzzle35004, "FOS06N01P01LS01", 0, Common::Point(1, 250), 1, 1000, 13, 12.5f, 4);
	objectAddPresentation(kObject30028);
	objectPresentationAddAnimationToPuzzle(kObject30028, 2, kPuzzle35004, "FOS06N01P01LS02", 0, Common::Point(1, 242), 1, 1000, 13, 12.5f, 4);
	objectAddPresentation(kObject30028);
	objectPresentationAddImageToRotation(kObject30028, 3, 30601, 0);
	objectAdd(kObject30040, "", "", 1);
	objectAdd(kObjectMetals, "Ignots", "FO_Ignots", 1);
	objectAddBagAnimation(kObjectMetals, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectMetals, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectMetals, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObjectMould, "Pan", "FO_Pan", 1);
	objectAddBagAnimation(kObjectMould, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectMould, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectMould, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObjectMeltedGold, "Gold", "FO_IgnotGld", 1);
	objectAddBagAnimation(kObjectMeltedGold, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectMeltedGold, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectMeltedGold, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObjectMeltedSilver, "Silver", "FO_IgnotSil", 1);
	objectAddBagAnimation(kObjectMeltedSilver, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectMeltedSilver, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectMeltedSilver, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObjectMeltedCopper, "Copper", "FO_IgnotCop", 1);
	objectAddBagAnimation(kObjectMeltedCopper, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectMeltedCopper, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectMeltedCopper, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObjectMeltedLead, "Lead", "FO_IgnotLed", 1);
	objectAddBagAnimation(kObjectMeltedLead, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectMeltedLead, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectMeltedLead, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObjectMeltedSteel, "Steel", "FO_IgnotSte", 1);
	objectAddBagAnimation(kObjectMeltedSteel, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectMeltedSteel, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectMeltedSteel, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObjectMeltedTin, "Tin", "FO_IgnotTin", 1);
	objectAddBagAnimation(kObjectMeltedTin, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectMeltedTin, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectMeltedTin, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObjectQuicksilver, "Mercury", "FO_IgnotMer", 1);
	objectAddBagAnimation(kObjectQuicksilver, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectQuicksilver, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectQuicksilver, 22, 22, 0, 3, 0, 0, 3);
	objectAddPuzzleAccessibility(kObject30040, kPuzzle35006, Common::Rect(293, 132, 351, 173), true, 52, 0);
	objectAddPuzzleAccessibility(kObject30040, kPuzzle35006, Common::Rect(285, 207, 354, 245), true, 52, 1);
	objectAddPuzzleAccessibility(kObject30040, kPuzzle35006, Common::Rect(173, 229, 244, 271), true, 52, 2);
	objectAddPuzzleAccessibility(kObject30040, kPuzzle35006, Common::Rect(201, 289, 284, 329), true, 52, 3);
	objectAddPuzzleAccessibility(kObject30040, kPuzzle35006, Common::Rect(214, 135, 364, 189), true, 52, 4);
	objectAddPuzzleAccessibility(kObject30040, kPuzzle35006, Common::Rect(222, 210, 353, 277), true, 52, 5);
	objectSetAccessibilityOff(kObject30040, 2, 2);
	objectSetAccessibilityOff(kObject30040, 3, 3);
	objectSetAccessibilityOff(kObject30040, 4, 4);
	objectSetAccessibilityOff(kObject30040, 5, 5);
	objectAddPresentation(kObject30040);
	objectPresentationAddImageToPuzzle(kObject30040, 0, kPuzzle35006, "FOS06N01P02L02.bmp", Common::Point(1, 44), true, 1, 1000);
	objectAddPresentation(kObject30040);
	objectPresentationAddImageToPuzzle(kObject30040, 1, kPuzzle35006, "FOS06N01P02L01.bmp", Common::Point(2, 45), true, 1, 1000);
	objectAddPresentation(kObject30040);
	objectPresentationAddImageToPuzzle(kObject30040, 2, kPuzzle35006, "FOS06N01P02L03.bmp", Common::Point(58, 151), true, 1, 1000);
	objectAddPresentation(kObject30040);
	objectPresentationAddImageToPuzzle(kObject30040, 3, kPuzzle35006, "FOS06N01P02L04.bmp", Common::Point(54, 132), true, 1, 1000);
	objectAddPresentation(kObject30040);
	objectPresentationAddAnimationToPuzzle(kObject30040, 4, kPuzzle35006, "FOS06N01P02LS01", 0, Common::Point(157, 168), 1, 1000, 25, 12.5f, 6);
	objectPresentationSetAnimationOnPuzzle(kObject30040, 4, 0, kObject30008);
	objectAdd(kObject30042, "", "", 1);
	objectAddPuzzleAccessibility(kObject30042, kPuzzle35007, Common::Rect(248, 261, 287, 278), true, 52, 0);
	objectAddPuzzleAccessibility(kObject30042, kPuzzle35007, Common::Rect(405,  79, 453, 115), true, 52, 1);
	objectAddPuzzleAccessibility(kObject30042, kPuzzle35007, Common::Rect(337,  47, 379,  77), true, 52, 2);
	objectAddPuzzleAccessibility(kObject30042, kPuzzle35007, Common::Rect(453, 199, 492, 228), true, 52, 3);
	objectAddPuzzleAccessibility(kObject30042, kPuzzle35007, Common::Rect(245,  48, 285,  74), true, 52, 4);
	objectAddPuzzleAccessibility(kObject30042, kPuzzle35007, Common::Rect(285,  46, 330,  73), true, 52, 5);
	objectAddPuzzleAccessibility(kObject30042, kPuzzle35007, Common::Rect(419, 120, 457, 151), true, 52, 6);
	objectAddPuzzleAccessibility(kObject30042, kPuzzle35007, Common::Rect(441, 266, 476, 294), true, 52, 7);
	objectSetAccessibilityOff(kObject30042, 1, 7);
	objectAddPresentation(kObject30042);
	objectPresentationAddImageToPuzzle(kObject30042, 0, kPuzzle35007, "FOS06N01P03L02.bmp", Common::Point(0, 16), true, 1, 1000);
	objectAddPresentation(kObject30042);
	objectPresentationAddImageToPuzzle(kObject30042, 1, kPuzzle35007, "FOS06N01P03L03_03.bmp", Common::Point(334, 200), true, 1, 1000);
	objectAddPresentation(kObject30042);
	objectPresentationAddImageToPuzzle(kObject30042, 2, kPuzzle35007, "FOS06N01P03L03_02.bmp", Common::Point(328, 182), true, 1, 1000);
	objectAddPresentation(kObject30042);
	objectPresentationAddImageToPuzzle(kObject30042, 3, kPuzzle35007, "FOS06N01P03L03_05.bmp", Common::Point(335, 229), true, 1, 1000);
	objectAddPresentation(kObject30042);
	objectPresentationAddImageToPuzzle(kObject30042, 4, kPuzzle35007, "FOS06N01P03L03_06.bmp", Common::Point(326, 252), true, 1, 1000);
	objectAddPresentation(kObject30042);
	objectPresentationAddImageToPuzzle(kObject30042, 5, kPuzzle35007, "FOS06N01P03L03_01.bmp", Common::Point(294, 221), true, 1, 1000);
	objectAddPresentation(kObject30042);
	objectPresentationAddImageToPuzzle(kObject30042, 6, kPuzzle35007, "FOS06N01P03L03_04.bmp", Common::Point(368, 229), true, 1, 1000);
	objectAddPresentation(kObject30042);
	objectPresentationAddImageToPuzzle(kObject30042, 7, kPuzzle35007, "FOS06N01P03L03_07.bmp", Common::Point(312, 307), true, 1, 1000);
	objectAddPresentation(kObject30042);
	objectPresentationAddImageToPuzzle(kObject30042, 8, kPuzzle35007, "FOS06N01P03L01.bmp", Common::Point(0, 16), true, 1, 1000);
	objectAdd(kObjectGolem, "Golem", "FO_Golem", 1);
	objectAddBagAnimation(kObjectGolem, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectGolem, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectGolem, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObject30044, "", "", 1);
	objectAddPuzzleAccessibility(kObject30044, kPuzzle35008, Common::Rect(253, 206, 333, 300), true, 52, 0);
	objectAddPuzzleAccessibility(kObject30044, kPuzzle35008, Common::Rect(203, 262, 300, 379), true, 52, 1);
	objectSetAccessibilityOff(kObject30044, 1, 1);
	objectAddPresentation(kObject30044);
	objectPresentationAddImageToPuzzle(kObject30044, 0, kPuzzle35008, "FOS06N01P04L01.bmp", Common::Point(189, 229), true, 1, 1000);
	objectAdd(kObject30045, "", "", 1);
	objectAddPuzzleAccessibility(kObject30045, kPuzzle35009, Common::Rect(283, 238, 347, 299), true, 52, 0);
	objectAddPuzzleAccessibility(kObject30045, kPuzzle35009, Common::Rect(273, 315, 306, 352), true, 52, 1);
	objectAddPuzzleAccessibility(kObject30045, kPuzzle35009, Common::Rect(293, 198, 330, 236), true, 52, 2);
	objectAddPuzzleAccessibility(kObject30045, kPuzzle35009, Common::Rect(342, 217, 387, 265), true, 52, 3);
	objectAddPuzzleAccessibility(kObject30045, kPuzzle35009, Common::Rect(225, 241, 261, 281), true, 52, 4);
	objectAddPuzzleAccessibility(kObject30045, kPuzzle35009, Common::Rect(250, 268, 288, 306), true, 52, 5);
	objectAddPuzzleAccessibility(kObject30045, kPuzzle35009, Common::Rect(351, 267, 403, 325), true, 52, 6);
	objectAddRotationAccessibility(kObject30045, 30602, Common::Rect(1856, -381, 2445, -20), true, 52, 7);
	objectAddPresentation(kObject30045);
	objectPresentationAddImageToPuzzle(kObject30045, 0, kPuzzle35009, "FOS06N02P01L01.bmp", Common::Point(195, 34), true, 1, 1000);
	objectAddPresentation(kObject30045);
	objectPresentationAddImageToPuzzle(kObject30045, 1, kPuzzle35009, "FOS06N02P01L02.bmp", Common::Point(193, 32), true, 1, 1000);
	objectAddPresentation(kObject30045);
	objectPresentationAddImageToPuzzle(kObject30045, 2, kPuzzle35009, "FOS06N02P01L03.bmp", Common::Point(148, 16), true, 1, 1000);
	objectAddPresentation(kObject30045);
	objectPresentationAddImageToPuzzle(kObject30045, 3, kPuzzle35009, "FOS06N02P01L06.bmp", Common::Point(174, 16), true, 1, 1000);
	objectAddPresentation(kObject30045);
	objectPresentationAddImageToPuzzle(kObject30045, 4, kPuzzle35009, "FOS06N02P01L05.bmp", Common::Point(203, 16), true, 1, 1000);
	objectAddPresentation(kObject30045);
	objectPresentationAddImageToPuzzle(kObject30045, 5, kPuzzle35009, "FOS06N02P01L04.bmp", Common::Point(171, 22), true, 1, 1000);
	objectAddPresentation(kObject30045);
	objectPresentationAddImageToPuzzle(kObject30045, 6, kPuzzle35009, "FOS06N02P01L07.bmp", Common::Point(161, 37), true, 1, 1000);
	objectAddPresentation(kObject30045);
	objectPresentationAddAnimationToRotation(kObject30045, 7, 30602, 0, 75, 30.0f, 4);
	objectPresentationSetAnimationOnRotation(kObject30045, 7, 0, kObject30000);
	objectPresentationShow(kObject30045, 7);
	objectAdd(kObjectFishingRod, "Fishing pole", "FO_Fishing", 1);
	objectAddBagAnimation(kObjectFishingRod, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectFishingRod, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectFishingRod, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObjectFish, "Fish", "FO_Fish", 1);
	objectAddBagAnimation(kObjectFish, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectFish, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectFish, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObjectFishingRodWithWorms, "Fishing pole and worms", "FO_FishingWorms", 1);
	objectAddBagAnimation(kObjectFishingRodWithWorms, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectFishingRodWithWorms, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectFishingRodWithWorms, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObjectKey, "Key form the fish", "FO_FishKey", 1);
	objectAddBagAnimation(kObjectKey, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectKey, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectKey, 22, 22, 0, 3, 0, 0, 3);
	objectAddPuzzleAccessibility(kObjectFishingRod, kPuzzle35010, Common::Rect(0, 208, 639, 384), true, 52, 0);
	objectAddPuzzleAccessibility(kObjectFishingRod, kPuzzle35010, Common::Rect(0, 208, 639, 384), true, 52, 1);
	objectAddPuzzleAccessibility(kObjectFishingRod, kPuzzle35010, Common::Rect(0,  63, 639, 197), true, 52, 2);
	objectSetAccessibilityOff(kObjectFishingRod, 1, 2);
	objectAddRotationAccessibility(kObjectFishingRod, 30703, Common::Rect(1126, 141, 3373, 523), true, 52, 3);
	objectAddPresentation(kObjectFishingRod);
	objectPresentationAddImageToPuzzle(kObjectFishingRod, 0, kPuzzle35010, "FOS07N03P01L01.bmp", Common::Point(0, 16), true, 1, 1000);
	objectAddPresentation(kObjectFishingRod);
	objectPresentationAddImageToPuzzle(kObjectFishingRod, 1, kPuzzle35010, "FOS07N03P01L02.bmp", Common::Point(0, 16), true, 1, 1000);
	objectAdd(kObjectWorms, "Worms", "FO_Worms", 1);
	objectAddBagAnimation(kObjectWorms, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectWorms, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectWorms, 22, 22, 0, 3, 0, 0, 3);
	objectAddPresentation(kObjectWorms);
	objectPresentationAddImageToPuzzle(kObjectWorms, 0, kPuzzle35020, "FOS07N04P01L01.bmp", Common::Point(0, 16), true, 1, 1000);
	objectAddPresentation(kObjectWorms);
	objectPresentationAddAnimationToPuzzle(kObjectWorms, 1, kPuzzle35020, "FOS07N04P01S01", 0, Common::Point(312, 209), 1, 1000, 12, 12.5f, 4);
	objectAddPuzzleAccessibility(kObjectWorms, kPuzzle35020, Common::Rect(285, 196, 391, 330), false, 52, 0);
	objectAdd(kObject30050, "", "", 1);
	objectAddRotationAccessibility(kObject30050, 30501, Common::Rect(1177, 298, 1263, 530), true, 52, 0);
	objectAddPresentation(kObject30050);
	objectPresentationAddImageToRotation(kObject30050, 0, 30501, 0);
	objectAddPresentation(kObject30050);
	objectPresentationAddAnimationToRotation(kObject30050, 1, 30501, 1, 13, 12.5f, 6);
	objectPresentationSetAnimationOnRotation(kObject30050, 1, 0, kObjectPatience);
	objectAddPresentation(kObject30050);
	objectPresentationAddImageToRotation(kObject30050, 2, 30402, 0);
	objectAddPresentation(kObject30050);
	objectPresentationAddImageToRotation(kObject30050, 3, 30501, 2);
	objectAdd(kObject30051, "", "", 1);
	objectAdd(kObject30052, "", "", 1);
	objectAdd(kObjectBurningArrow, "Inflamed arrow and bow", "FO_BowFire", 1);
	objectAddBagAnimation(kObjectBurningArrow, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectBurningArrow, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectBurningArrow, 22, 22, 0, 3, 0, 0, 3);
	objectAddRotationAccessibility(kObject30051, 30011, Common::Rect(411, 0, 511, 57), false, 52, 0);
	objectAddRotationAccessibility(kObject30051, 30011, Common::Rect(958, 102, 1105, 333), true, 52, 1);
	objectAddRotationAccessibility(kObject30051, 30011, Common::Rect(0, 0, 0, 0), true, 52, 2);
	objectAddRotationAccessibility(kObject30051, 30011, Common::Rect(421, 15, 764, 170), false, 52, 3);
	objectAddPresentation(kObject30051);
	objectPresentationAddImageToRotation(kObject30051, 0, 30011, 0);
	objectAddPresentation(kObject30051);
	objectPresentationAddAnimationToRotation(kObject30051, 1, 30011, 1, 22, 20.0f, 4);
	objectAddPresentation(kObject30051);
	objectPresentationAddImageToRotation(kObject30051, 2, 30011, 2);
	objectPresentationShow(kObject30051, 1);
	objectAdd(kObject30100, "", "", 1);
	objectAddPresentation(kObject30100);
	objectPresentationAddImageToPuzzle(kObject30100, 0, kPuzzle35100, "FOS00N03P02L01.bmp", Common::Point(117, 70), true, 1, 1000);
	objectAddPuzzleAccessibility(kObject30100, kPuzzle35100, Common::Rect(114, 106, 406, 463), true,  52, 0);
	objectAddPuzzleAccessibility(kObject30100, kPuzzle35100, Common::Rect(311, 249, 397, 463), false, 52, 1);
	objectAdd(kObjectWolfBrooch, "Sieglinde's Medallion", "FO_WolfLogo", 1);
	objectAddBagAnimation(kObjectWolfBrooch, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectWolfBrooch, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectWolfBrooch, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObject30101, "", "", 1);
	objectAddPresentation(kObject30101);
	objectPresentationAddAnimationToPuzzle(kObject30101, 0, kPuzzle35101, "FOS00N03P02S02", 0, Common::Point(297, 209), 1, 1000, 20, 12.5f, 4);
	objectAdd(kObject30102, "", "", 1);
	objectAdd(kObject30103, "", "", 1);
	objectAddPresentation(kObject30103);
	objectPresentationAddAnimationToPuzzle(kObject30103, 0, kPuzzle35103, "FOS01N01P07", 0, Common::Point(380, 143), 1, 1000, 20, 12.5f, 4);
	objectAdd(kObject30104, "", "", 1);
	objectAddPresentation(kObject30104);
	objectPresentationAddAnimationToPuzzle(kObject30104, 0, kPuzzle35104, "FOS01N01P03", 0, Common::Point(165, 129), 1, 1000, 20, 12.5f, 4);
	objectAdd(kObject30105, "", "", 1);
	objectAddPresentation(kObject30105);
	objectPresentationAddAnimationToPuzzle(kObject30105, 0, kPuzzle35105, "FOS01N01P04", 0, Common::Point(383, 171), 1, 1000, 20, 12.5f, 4);
	objectAdd(kObject30106, "", "", 1);
	objectAddPresentation(kObject30106);
	objectPresentationAddAnimationToPuzzle(kObject30106, 0, kPuzzle35106, "FOS01N01P05", 0, Common::Point(443, 94), 1, 1000, 20, 12.5f, 4);
	objectAdd(kObject30107, "", "", 1);
	objectAddPresentation(kObject30107);
	objectPresentationAddAnimationToPuzzle(kObject30107, 0, kPuzzle35107, "FOS01N01P06", 0, Common::Point(303, 161), 1, 1000, 20, 12.5f, 4);
	objectAdd(kObject30109, "", "", 1);
	objectAddPresentation(kObject30109);
	objectPresentationAddAnimationToPuzzle(kObject30109, 0, kPuzzle35111, "FOS01N01P02S01", 0, Common::Point(190, 141), 1, 1000, 202, 12.5f, 4);
	objectPresentationSetAnimationOnPuzzle(kObject30109, 0, 0, kObject30007);
	objectAddPuzzleAccessibility(kObject30109, kPuzzle35111, Common::Rect(  0, 334, 463, 418), false, 52, 0);
	objectAdd(kObject30108, "", "", 1);
	objectAddPuzzleAccessibility(kObject30108, kPuzzle35109, Common::Rect(181, 165, 256, 259), true, 52, 0);
	objectAddRotationAccessibility(kObject30102, 30101, Common::Rect(0, 0, 0, 0), true, 52, 0);
	objectAddRotationAccessibility(kObject30102, 30101, Common::Rect(1741, 134, 1826, 221), true, 52, 1);
	objectAdd(kObject30110, "", "", 1);
	objectAddPresentation(kObject30110);
	objectPresentationAddImageToRotation(kObject30110, 0, 30101, 0);
	objectAddPresentation(kObject30110);
	objectPresentationAddAnimationToRotation(kObject30110, 1, 30101, 1, 200, 12.5f, 4);
	objectPresentationSetAnimationOnRotation(kObject30110, 1, 0, kObject30001);
	objectAddPresentation(kObject30110);
	objectPresentationAddImageToRotation(kObject30110, 2, 30101, 2);
	objectAddPresentation(kObject30110);
	objectPresentationAddImageToRotation(kObject30110, 3, 30101, 3);
	objectAdd(kObjectNotung, "Sword Notung", "", 1);
	objectAddPuzzleAccessibility(kObjectNotung, kPuzzle35019, Common::Rect(290, 167, 363, 232), false, 52, 0);
	objectAddPresentation(kObjectNotung);
	objectPresentationAddImageToPuzzle(kObjectNotung, 0, kPuzzle35019, "FOS00N04P01L01.bmp", Common::Point(0, 16), true, 1, 1000);
	objectAddPresentation(kObjectNotung);
	objectPresentationAddImageToPuzzle(kObjectNotung, 1, kPuzzle35019, "FOS00N04P01L02.bmp", Common::Point(303, 192), true, 1, 1000);
	objectAddPresentation(kObjectNotung);
	objectPresentationAddImageToPuzzle(kObjectNotung, 2, kPuzzle35019, "FOS00N04P01L03.bmp", Common::Point(252, 41), true, 1, 1000);
	objectAddPresentation(kObjectNotung);
	objectPresentationAddImageToPuzzle(kObjectNotung, 3, kPuzzle35019, "FOS00N04P01L04.bmp", Common::Point(284, 193), true, 1, 1000);
	objectAdd(kObject30059, "", "", 1);
	objectAddRotationAccessibility(kObject30059, 30302, Common::Rect(2183, -268, 2752, 33), true, 52, 0);
	objectAddRotationAccessibility(kObject30059, 30303, Common::Rect(2183, -268, 2752, 33), true, 52, 1);
	objectAdd(kObject30061, "", "", 1);
	objectAddPresentation(kObject30061);
	objectPresentationAddImageToRotation(kObject30061, 0, 30001, 0);
	objectAddPresentation(kObject30061);
	objectPresentationAddImageToRotation(kObject30061, 1, 30010, 0);
	objectAddPresentation(kObject30061);
	objectPresentationAddImageToRotation(kObject30061, 2, 30301, 0);
	objectAddPresentation(kObject30061);
	objectPresentationAddImageToRotation(kObject30061, 3, 30008, 0);
	objectAddPresentation(kObject30061);
	objectPresentationAddImageToRotation(kObject30061, 4, 30009, 0);
	objectAddPresentation(kObject30061);
	objectPresentationAddImageToRotation(kObject30061, 5, 30702, 0);
	objectAddPresentation(kObject30061);
	objectPresentationAddImageToRotation(kObject30061, 6, 30703, 0);
	objectAdd(kObject30200, "", "", 1);
	objectAddRotationAccessibility(kObject30200, 30301, Common::Rect(3452, 64, 3505, 109), true, 52, 0);
	objectAddRotationAccessibility(kObject30200, 30301, Common::Rect(2939, 57, 2995, 99), true, 52, 1);
	objectAddRotationAccessibility(kObject30200, 30301, Common::Rect(873, 68, 949, 120), true, 52, 2);
	objectAddRotationAccessibility(kObject30200, 30301, Common::Rect(2425, 71, 2478, 118), true, 52, 3);
	objectAddRotationAccessibility(kObject30200, 30301, Common::Rect(1390, 62, 1451, 108), true, 52, 4);
	objectAddRotationAccessibility(kObject30200, 30301, Common::Rect(1912, 54, 1959, 118), true, 52, 5);
	objectAddRotationAccessibility(kObject30200, 30301, Common::Rect(367, 62, 425, 111), true, 52, 6);
	soundAdd(30007, kSoundTypeBackgroundMusic, "1267.was", _configuration.backgroundMusic.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(30000, kSoundTypeAmbientMusic,    "1269.was", _configuration.backgroundMusic.loadFrom, 2, 18);
	soundAdd(30001, kSoundTypeAmbientMusic,    "1270.was", _configuration.backgroundMusic.loadFrom, 2, 18);
	soundAdd(30002, kSoundTypeAmbientMusic,    "1271.was", _configuration.backgroundMusic.loadFrom, 2, 18);
	soundAdd(30003, kSoundTypeAmbientMusic,    "1272.was", _configuration.backgroundMusic.loadFrom, 2, 18);
	soundAdd(30004, kSoundTypeAmbientMusic,    "1273.was", _configuration.backgroundMusic.loadFrom, 2, 18);
	soundAdd(30005, kSoundTypeAmbientMusic,    "1274.was", _configuration.backgroundMusic.loadFrom, 2, 18);
	soundAdd(30006, kSoundTypeAmbientMusic,    "1275.was", _configuration.backgroundMusic.loadFrom, 2, 18);
	soundSetVolume(30005, 90);
	soundSetVolume(30002, 95);
	rotationAddAmbientSound(30001, 30000, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(30002, 30000, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(30003, 30000, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(30004, 30000, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(30005, 30000, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(30006, 30000, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(30008, 30000, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(30009, 30000, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(30010, 30000, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(30011, 30000, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(30012, 30000, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle35019, 30000, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle35100, 30000, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(30101, 30003, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle35101, 30003, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle35103, 30003, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle35104, 30003, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle35105, 30003, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle35106, 30003, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle35107, 30003, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle35108, 30003, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle35109, 30003, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle35110, 30003, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle35111, 30003, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(30301, 30004, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(30302, 30004, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(30303, 30004, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle35001, 30004, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle35011, 30004, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(30401, 30005, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(30402, 30005, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(30801, 30005, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(30401, 30006, 90, 0, 1, 1, 10);
	rotationAddAmbientSound(30402, 30006, 90, 0, 1, 1, 10);
	rotationAddAmbientSound(30801, 30006, 90, 0, 1, 1, 10);
	rotationAddAmbientSound(30011, 30006, 87, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle35002, 30006, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(30501, 30002, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(30601, 30002, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(30602, 30002, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle35009, 30002, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle35005, 30002, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle35003, 30002, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle35004, 30002, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle35006, 30002, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle35007, 30002, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle35008, 30002, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(30701, 30001, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(30702, 30001, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(30703, 30001, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(30704, 30001, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle35010, 30001, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle35020, 30001, 100, 0, 1, 1, 10);
	soundAdd(30201, kSoundTypeAmbientEffect, "1276.wav", _configuration.ambientEffect.loadFrom);
	soundAdd(30202, kSoundTypeAmbientEffect, "1277.wav", _configuration.ambientEffect.loadFrom);
	soundAdd(30300, kSoundTypeAmbientEffect, "1278.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(30301, kSoundTypeAmbientEffect, "1279.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(30302, kSoundTypeAmbientEffect, "1280.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(30303, kSoundTypeAmbientEffect, "1281.wav", _configuration.ambientEffect.loadFrom);
	soundAdd(30304, kSoundTypeAmbientEffect, "1282.wav", _configuration.ambientEffect.loadFrom);
	soundAdd(30305, kSoundTypeAmbientEffect, "1283.wav", _configuration.ambientEffect.loadFrom);
	soundAdd(30306, kSoundTypeAmbientEffect, "1284.wav", _configuration.ambientEffect.loadFrom);
	soundAdd(30307, kSoundTypeAmbientEffect, "1285.wav", _configuration.ambientEffect.loadFrom);
	soundAdd(30308, kSoundTypeAmbientEffect, "1286.wav", _configuration.ambientEffect.loadFrom);
	soundAdd(30309, kSoundTypeAmbientEffect, "1287.wav", _configuration.ambientEffect.loadFrom);
	soundAdd(30310, kSoundTypeAmbientEffect, "1288.wav", _configuration.ambientEffect.loadFrom);
	soundAdd(30311, kSoundTypeAmbientEffect, "1289.wav", _configuration.ambientEffect.loadFrom);
	soundAdd(30312, kSoundTypeAmbientEffect, "1290.wav", _configuration.ambientEffect.loadFrom);
	soundAdd(30313, kSoundTypeAmbientEffect, "1291.wav", _configuration.ambientEffect.loadFrom);
	soundAdd(30314, kSoundTypeAmbientEffect, "1292.wav", _configuration.ambientEffect.loadFrom);
	soundAdd(30501, kSoundTypeAmbientEffect, "1293.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(30502, kSoundTypeAmbientEffect, "1294.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(30315, kSoundTypeAmbientEffect, "1295.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(30316, kSoundTypeAmbientEffect, "1296.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(30317, kSoundTypeAmbientEffect, "1297.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(30318, kSoundTypeAmbientEffect, "1298.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(30319, kSoundTypeAmbientEffect, "1299.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(30320, kSoundTypeAmbientEffect, "1300.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(30321, kSoundTypeAmbientEffect, "1301.was", _configuration.ambientEffect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(30322, kSoundTypeAmbientEffect, "1302.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(30323, kSoundTypeAmbientEffect, "1303.was", _configuration.ambientEffect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(30324, kSoundTypeAmbientEffect, "1304.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(30325, kSoundTypeAmbientEffect, "1305.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(30326, kSoundTypeAmbientEffect, "1306.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(30503, kSoundTypeAmbientEffect, "1307.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(30504, kSoundTypeAmbientEffect, "1308.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(30505, kSoundTypeAmbientEffect, "1309.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(30506, kSoundTypeAmbientEffect, "1310.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(30508, kSoundTypeAmbientEffect, "1311.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(30509, kSoundTypeAmbientEffect, "1312.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(30510, kSoundTypeAmbientEffect, "1313.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(30511, kSoundTypeAmbientEffect, "1314.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(30512, kSoundTypeAmbientEffect, "1315.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(30513, kSoundTypeAmbientEffect, "1316.was", _configuration.ambientEffect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(30514, kSoundTypeAmbientEffect, "1317.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(30500, kSoundTypeEffect, "1320.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(30100, kSoundTypeDialog, "1322.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30101, kSoundTypeDialog, "1323.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30102, kSoundTypeDialog, "1324.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30103, kSoundTypeDialog, "1325.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30104, kSoundTypeDialog, "1326.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30105, kSoundTypeDialog, "1327.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30106, kSoundTypeDialog, "1328.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30107, kSoundTypeDialog, "1329.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30108, kSoundTypeDialog, "1330.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30161, kSoundTypeDialog, "1331.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30109, kSoundTypeDialog, "1332.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30118, kSoundTypeDialog, "1333.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30111, kSoundTypeDialog, "1334.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30112, kSoundTypeDialog, "1335.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30113, kSoundTypeDialog, "1336.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30114, kSoundTypeDialog, "1337.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30115, kSoundTypeDialog, "1338.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30116, kSoundTypeDialog, "1340.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30110, kSoundTypeDialog, "1339.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30117, kSoundTypeDialog, "1341.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30120, kSoundTypeDialog, "1342.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30121, kSoundTypeDialog, "1343.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30122, kSoundTypeDialog, "1344.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30123, kSoundTypeDialog, "1345.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30124, kSoundTypeDialog, "1346.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30125, kSoundTypeDialog, "1347.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30126, kSoundTypeDialog, "1348.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30127, kSoundTypeDialog, "1349.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30128, kSoundTypeDialog, "1350.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30129, kSoundTypeDialog, "1351.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30130, kSoundTypeDialog, "1352.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30131, kSoundTypeDialog, "1353.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30132, kSoundTypeDialog, "1354.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30133, kSoundTypeDialog, "1355.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30134, kSoundTypeDialog, "1356.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30135, kSoundTypeDialog, "1357.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30136, kSoundTypeDialog, "1358.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30150, kSoundTypeDialog, "1359.wav", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30151, kSoundTypeDialog, "1360.wav", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30152, kSoundTypeDialog, "1361.wav", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30153, kSoundTypeDialog, "1362.wav", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30154, kSoundTypeDialog, "1363.wav", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30155, kSoundTypeDialog, "1364.wav", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30156, kSoundTypeDialog, "1365.wav", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30162, kSoundTypeDialog, "1366.wav", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30163, kSoundTypeDialog, "1367.wav", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30164, kSoundTypeDialog, "1368.wav", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30165, kSoundTypeDialog, "1369.wav", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30166, kSoundTypeDialog, "1370.wav", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30167, kSoundTypeDialog, "1371.wav", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(30168, kSoundTypeDialog, "1372.wav", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	rotationAdd3DSound(30602, 30202, 1, 1, 15, 100, 174.0f, 10);
	rotationAdd3DSound(30601, 30202, 1, 1, 15, 80, 140.0f, 10);
	rotationAdd3DSound(30001, 30501, 1, 1, 20, 75, 244.0f, 3);
	rotationAdd3DSound(30002, 30501, 1, 1, 20, 75, 347.0f, 6);
	rotationAdd3DSound(30003, 30501, 1, 1, 20, 85, 158.0f, 3);
	rotationAdd3DSound(30004, 30501, 1, 1, 20, 70, 142.0f, 2);
	rotationAdd3DSound(30011, 30513, 1, 1, 40, 90, 56.0f, 10);
	varDefineByte(30009, 2);
	varDefineByte(30010, 1);
	varDefineByte(30011, 3);
	varDefineByte(30012, 4);
	varDefineByte(30013, 5);
	varDefineByte(30014, 6);
	varDefineByte(30015, 7);
	varDefineByte(30016, 25);
	varDefineByte(30017, 0);
	varDefineByte(30050, 0);
	varDefineByte(30051, 0);
	varDefineByte(30052, 0);
	varDefineByte(30053, 0);
	varDefineByte(30054, 0);
	varDefineByte(30055, 0);
	varDefineByte(30019, 0);
	varDefineByte(30020, 0);
	varDefineByte(30021, 0);
	varDefineByte(30022, 0);
	varDefineByte(30023, 0);
	varDefineByte(30024, 0);
	varDefineByte(30025, 0);
	varDefineByte(30026, 0);
	varDefineByte(30027, 0);
	varDefineByte(30028, 0);
	varDefineByte(30029, 0);
	varDefineByte(30030, 0);
	varDefineByte(30031, 0);
	varDefineByte(30032, 0);
	varDefineByte(30033, 0);
	varDefineByte(30038, 0);
	varDefineByte(30034, 0);
	varDefineByte(30035, 0);
	varDefineByte(30036, 0);
	varDefineByte(30037, 0);
	varDefineByte(30039, 0);
	varDefineByte(30040, 0);
	varDefineByte(30041, 0);
	varDefineByte(30042, 0);
	varDefineByte(30043, 0);
	varDefineByte(30044, 0);
	varDefineByte(30045, 0);
	varDefineByte(30047, 0);
	varDefineByte(30056, 1);
	varDefineByte(30060, 0);
	varDefineByte(30061, 0);
	varDefineByte(30062, 0);
	varDefineByte(30063, 0);
	varDefineByte(30064, 0);
	varDefineByte(30065, 0);
	varDefineByte(30070, 0);
	varDefineByte(30071, 0);
	varDefineByte(30072, 0);
	varDefineByte(30066, 0);
	varDefineByte(30067, 0);
	varDefineByte(30068, 0);
	varDefineByte(30069, 1);
	varDefineByte(30073, 0);
	varDefineByte(30074, 0);
	varDefineByte(30075, 0);
	varDefineByte(30076, 0);
	varDefineByte(30077, 0);
	varDefineByte(30078, 0);
	varDefineByte(30201, 0);
	varDefineByte(30203, 0);
	varDefineByte(30204, 0);
	varDefineByte(30205, 0);
	varDefineByte(30206, 0);
	varDefineByte(30207, 0);
	varDefineByte(30208, 0);
	varDefineByte(30209, 0);
	varDefineByte(30210, 0);
	varDefineByte(30211, 0);
	varDefineFloat(30046, -10.0);
	varDefineFloat(30042, 1.0);
}

void ApplicationRing::initZoneRO() {
	debugC(1, kRingDebugLogic, "Initializing Loge zone (RO)...");

	setZoneAndEnableBag(kZoneRO);

	puzzleAdd(kPuzzle40010);
	puzzleAddBackgroundImage(kPuzzle40010, "ROS00N01P01.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle40011);
	puzzleAddBackgroundImage(kPuzzle40011, "ROS00N01P01L02.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle40013);
	puzzleAddBackgroundImage(kPuzzle40013, "ROS00N01P01L01.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle40012);
	puzzleAddBackgroundImage(kPuzzle40012, "ROS00N01P01L03.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle40060);
	puzzleAddBackgroundImage(kPuzzle40060, "ROS00N06P01.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle40100);
	puzzleAddBackgroundImage(kPuzzle40100, "ROS00N01A01_A.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle40101);
	puzzleAddBackgroundImage(kPuzzle40101, "ROS00N01A01_B.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle40102);
	puzzleAddBackgroundImage(kPuzzle40102, "ROS00N01A01_C.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle40103);
	puzzleAddBackgroundImage(kPuzzle40103, "ROS00N01A01_D.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle40104);
	puzzleAddBackgroundImage(kPuzzle40104, "RO_ERDA.bmp", Common::Point(0, 16), true);
	rotationAdd(40000, "ROS00N01", 0, 1);
	rotationAdd(40001, "ROS00N02", 0, 1);
	rotationAdd(40002, "ROS00N03", 0, 1);
	rotationAdd(40003, "ROS00N04", 0, 1);
	rotationAdd(40004, "ROS00N05", 0, 1);
	rotationAdd(40005, "ROS00N06", 0, 0);
	rotationAddMovabilityToRotation(40000, 40001, "1789", Common::Rect(300, -250, 600, 300), true, 53, 0);
	rotationSetMovabilityToRotation(40000, 0, 0, 0, 85.7f, 0, 0, 0, 0, 85.7f);
	rotationAddMovabilityToPuzzle(40000, kPuzzle40010, "", Common::Rect(3192, -104, 3378, 90), true, 52, 0);
	rotationSetMovabilityToPuzzle(40000, 1, 290.0f, 0, 85.0f, 0, 0);
	rotationAddMovabilityToRotation(40001, 40004, "1790", Common::Rect(300, -250, 600, 300), true, 53, 0);
	rotationSetMovabilityToRotation(40001, 0, 0, 0, 85.7f, 0, 0, 0, 0, 85.7f);
	rotationAddMovabilityToRotation(40001, 40003, "1791", Common::Rect(1200, -250, 1500, 300), true, 53, 0);
	rotationSetMovabilityToRotation(40001, 1, 90.0f, 0, 85.7f, 0, 0, 90.0f, 0, 85.7f);
	rotationAddMovabilityToRotation(40001, 40000, "1792", Common::Rect(2100, -250, 2400, 300), true, 53, 0);
	rotationSetMovabilityToRotation(40001, 2, 180.0f, 0, 85.7f, 0, 0, 180.0f, 0, 85.7f);
	rotationAddMovabilityToRotation(40001, 40002, "1793", Common::Rect(3000, -250, 3300, 300), true, 53, 0);
	rotationSetMovabilityToRotation(40001, 3, 270.0f, 0, 85.7f, 0, 0, 270.0f, 0, 85.7f);
	rotationAddMovabilityToRotation(40002, 40001, "1794", Common::Rect(1200, -250, 1500, 300), true, 53, 0);
	rotationSetMovabilityToRotation(40002, 0, 90.0f, 0, 85.7f, 0, 0, 90.0f, 0, 85.7f);
	rotationAddMovabilityToRotation(40003, 40001, "1795", Common::Rect(3000, -250, 3300, 300), true, 53, 0);
	rotationSetMovabilityToRotation(40003, 0, 270.0f, 0, 85.7f, 0, 0, 270.0f, 0, 85.7f);
	rotationAddMovabilityToRotation(40004, 40005, "1796", Common::Rect(300, -250, 600, 300), false, 53, 0);
	rotationSetMovabilityToRotation(40004, 0, 0, 0, 85.7f, 0, 0, 0, 0, 85.7f);
	rotationAddMovabilityToRotation(40004, 40001, "1797", Common::Rect(2100, -250, 2400, 300), true, 53, 0);
	rotationSetMovabilityToRotation(40004, 1, 180.0f, 0, 85.7f, 0, 0, 180.0f, 0, 85.7f);
	rotationAddMovabilityToRotation(40005, 40004, "1798", Common::Rect(2100, -250, 2400, 300), true, 53, 0);
	rotationSetMovabilityToRotation(40005, 0, 180.0f, 0, 85.7f, 0, 0, 180.0f, 0, 85.7f);
	rotationAddMovabilityToPuzzle(40005, kPuzzle40060, "", Common::Rect(246, -90, 715, 376), true, 52, 0);
	rotationSetMovabilityToPuzzle(40005, 1, 0, 22.0f, 85.7f, 0, 0);
	puzzleAddMovabilityToRotation(kPuzzle40010, 40000, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle40010, 0, 287.0f, 10.0f, 85.7f);
	puzzleAddMovabilityToRotation(kPuzzle40011, 40000, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle40011, 0, 287.0f, 10.0f, 85.7f);
	puzzleAddMovabilityToRotation(kPuzzle40012, 40000, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle40012, 0, 287.0f, 10.0f, 85.7f);
	puzzleAddMovabilityToRotation(kPuzzle40013, 40000, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle40013, 0, 287.0f, 10.0f, 85.7f);
	puzzleAddMovabilityToRotation(kPuzzle40060, 40005, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle40060, 0, 0, 22.0f, 85.7f);
	objectAdd(kObjectFirePower, "Fire Power", "RO_Fire_", 1);
	objectAddBagAnimation(kObjectFirePower, 1, 3, 13, 12.5f, 4);
	objectSetActiveCursor(kObjectFirePower, 22, 22, 13, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectFirePower, 22, 22, 1, 4, 1.0f, 4, 4);
	objectAdd(kObjectRing, "Ring", "RO_Ring", 1);
	objectAddBagAnimation(kObjectRing, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectRing, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectRing, 22, 22, 1, 4, 1.0f, 4, 4);
	objectAdd(kObjectCrown, "Crown", "RO_Crown", 1);
	objectAddBagAnimation(kObjectCrown, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectCrown, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectCrown, 22, 22, 1, 4, 1.0f, 4, 4);
	objectAdd(kObject40010, "The Egg", "RO_Egg", 1);
	objectAddPuzzleAccessibility(kObject40010, kPuzzle40010, Common::Rect(200,  80, 440, 350), true,  52, 0);
	objectAddPuzzleAccessibility(kObject40010, kPuzzle40011, Common::Rect(200,  80, 440, 350), false, 52, 1);
	objectAddPuzzleAccessibility(kObject40010, kPuzzle40012, Common::Rect(219, 189, 430, 352), true,  52, 2);
	objectAddPresentation(kObject40010);
	objectPresentationAddAnimationToPuzzle(kObject40010, 0, kPuzzle40012, "ROS00N01P01s03", 0, Common::Point(217, 192), 1, 1000, 69, 25.0f, 4);
	objectAddPresentation(kObject40010);
	objectPresentationAddAnimationToPuzzle(kObject40010, 1, kPuzzle40012, "ROS00N01P01s04", 0, Common::Point(211, 180), 1, 1000, 69, 25.0f, 4);
	objectAdd(kObject40203, "", "", 1);
	objectAddPresentation(kObject40203);
	objectPresentationAddImageToRotation(kObject40203, 0, 40000, 0);
	objectAddPresentation(kObject40203);
	objectPresentationAddImageToRotation(kObject40203, 1, 40001, 0);
	objectAddPresentation(kObject40203);
	objectPresentationAddImageToRotation(kObject40203, 2, 40002, 0);
	objectAddPresentation(kObject40203);
	objectPresentationAddImageToRotation(kObject40203, 3, 40003, 0);
	objectAddPresentation(kObject40203);
	objectPresentationAddImageToRotation(kObject40203, 4, 40004, 0);
	objectAdd(kObject40101, "", "NI_HandSel", 4);
	objectSetActiveDrawCursor(kObject40101, 15, 15, 0, 3, 0, 0, 3);
	objectSetPassiveDrawCursor(kObject40101, 15, 15, 0, 3, 0, 0, 3);

	for (uint32 i = 0; i < 99; i++) {
		objectAddPresentation(kObject40101);
		objectPresentationAddImageToPuzzle(kObject40101, i, kPuzzle40013, Common::String::format("ROS00N01P01S02CF1.%04d.bmp", i + 1), Common::Point(232, 203), true, 1, 1000);
	}

	objectAdd(kObject40102, "", "NI_HandSel", 4);
	objectSetActiveDrawCursor(kObject40102, 15, 15, 0, 3, 0, 0, 3);
	objectSetPassiveDrawCursor(kObject40102, 15, 15, 0, 3, 0, 0, 3);

	for (uint32 i = 0; i < 99; i++) {
		objectAddPresentation(kObject40102);
		objectPresentationAddImageToPuzzle(kObject40102, i, kPuzzle40013, Common::String::format("ROS00N01P01S02CF2.%04d.bmp", i + 1), Common::Point(265, 204), true, 1, 1000);
	}

	objectAdd(kObject40103, "", "NI_HandSel", 4);
	objectSetActiveDrawCursor(kObject40103, 15, 15, 0, 3, 0, 0, 3);
	objectSetPassiveDrawCursor(kObject40103, 15, 15, 0, 3, 0, 0, 3);

	for (uint32 i = 0; i < 99; i++) {
		objectAddPresentation(kObject40103);
		objectPresentationAddImageToPuzzle(kObject40103, i, kPuzzle40013, Common::String::format("ROS00N01P01S02CF3.%04d.bmp", i + 1), Common::Point(295, 203), true, 1, 1000);
	}

	objectAdd(kObject40104, "", "NI_HandSel", 4);
	objectSetActiveDrawCursor(kObject40104, 15, 15, 0, 3, 0, 0, 3);
	objectSetPassiveDrawCursor(kObject40104, 15, 15, 0, 3, 0, 0, 3);

	for (uint32 i = 0; i < 99; i++) {
		objectAddPresentation(kObject40104);
		objectPresentationAddImageToPuzzle(kObject40104, i, kPuzzle40013, Common::String::format("ROS00N01P01S02CF4.%04d.bmp", i + 1), Common::Point(326, 203), true, 1, 1000);
	}

	objectAdd(kObject40105, "", "NI_HandSel", 4);
	objectSetActiveDrawCursor(kObject40105, 15, 15, 0, 3, 0, 0, 3);
	objectSetPassiveDrawCursor(kObject40105, 15, 15, 0, 3, 0, 0, 3);

	for (uint32 i = 0; i < 99; i++) {
		objectAddPresentation(kObject40105);
		objectPresentationAddImageToPuzzle(kObject40105, i, kPuzzle40013, Common::String::format("ROS00N01P01S02CF5.%04d.bmp", i + 1), Common::Point(356, 204), true, 1, 1000);
	}

	objectAddPuzzleAccessibility(kObject40101, kPuzzle40013, Common::Rect(  0,   0,   0,   0), true, 52, 0);
	objectAddPuzzleAccessibility(kObject40101, kPuzzle40013, Common::Rect(  0,   0,   0,   0), true, 52, 1);
	objectAddPuzzleAccessibility(kObject40101, kPuzzle40013, Common::Rect(298, 202, 328, 245), true, 52, 2);
	objectAddPuzzleAccessibility(kObject40101, kPuzzle40013, Common::Rect(329, 202, 359, 245), true, 52, 3);
	objectAddPuzzleAccessibility(kObject40101, kPuzzle40013, Common::Rect(360, 202, 390, 245), true, 52, 4);
	objectAdd(kObject40011, "", "", 1);
	objectAddPresentation(kObject40011);

	for (uint32 i = 0; i < 7; i++) {
		for (uint32 j = 0; j < 7; j++) {
			varDefineByte(40501 + (j + 1) * 10 + i, 99);
		}
	}

	objectAddPuzzleAccessibility(kObject40011, kPuzzle40011, Common::Rect(263, 71, 319, 127), true, 52, 12);
	varSetByte(40513, 0);

	for (uint32 i = 0; i < 5; i++) {
		for (uint32 j = 0; j < 5; j++) {
			objectPresentationAddImageToPuzzle(kObject40011, i, kPuzzle40011, Common::String::format("L01T%d%d.bmp", i, j), Common::Point(207 + j * 56, 127 + 56 * i), true, 1, 1000);
			objectAddPuzzleAccessibility(kObject40011, kPuzzle40011, Common::Rect(207 + j * 56, 127 + 56 * i, 207 + (j + 1) * 56, 127 + 56 * (i + 1)), true, 52, (21 + i * 10 + j));
			varSetByte(40501 + (21 + i * 10 + j), 10 * (i + 2) + j + 1);
		}

		objectAddPresentation(kObject40011);
	}

	objectPresentationShow(kObject40011, 0);
	objectPresentationShow(kObject40011, 1);
	objectPresentationShow(kObject40011, 2);
	objectPresentationShow(kObject40011, 3);
	objectAdd(kObject40060, "", "NI_HandSel", 4);
	objectSetActiveDrawCursor(kObject40060, 15, 15, 0, 3, 0, 0, 3);
	objectSetPassiveDrawCursor(kObject40060, 15, 15, 0, 3, 0, 0, 3);

	for (uint32 i = 0; i < 73; i++) {
		objectAddPresentation(kObject40060);
		objectPresentationAddImageToPuzzle(kObject40060, i, kPuzzle40060, Common::String::format("ROS00N06P01S02.%04d.bmp", i + 1), Common::Point(67, 34), true, 1, 1000);
	}

	objectAddPuzzleAccessibility(kObject40060, kPuzzle40060, Common::Rect(531, 112, 576, 135), true,  52, 0);
	objectAddPuzzleAccessibility(kObject40060, kPuzzle40060, Common::Rect(466, 110, 511, 135), false, 52, 1);
	objectAddPuzzleAccessibility(kObject40060, kPuzzle40060, Common::Rect(404, 110, 449, 135), false, 52, 2);
	objectAddPuzzleAccessibility(kObject40060, kPuzzle40060, Common::Rect(343, 110, 387, 135), false, 52, 3);
	objectAddPuzzleAccessibility(kObject40060, kPuzzle40060, Common::Rect(282, 110, 325, 135), false, 52, 4);
	objectAddPuzzleAccessibility(kObject40060, kPuzzle40060, Common::Rect(218, 110, 263, 135), false, 52, 5);
	objectAddPuzzleAccessibility(kObject40060, kPuzzle40060, Common::Rect(157, 110, 201, 135), false, 52, 6);
	objectAddPuzzleAccessibility(kObject40060, kPuzzle40060, Common::Rect( 93, 110, 140, 135), false, 52, 7);
	objectAdd(kObject40201, "", "", 1);
	objectAddPuzzleAccessibility(kObject40201, kPuzzle40060, Common::Rect(461,  63, 488, 103), true,  52, 0);
	objectAddPuzzleAccessibility(kObject40201, kPuzzle40060, Common::Rect(409,  63, 436, 103), true,  52, 1);
	objectAddPuzzleAccessibility(kObject40201, kPuzzle40060, Common::Rect(357,  63, 383, 103), true,  52, 2);
	objectAddPuzzleAccessibility(kObject40201, kPuzzle40060, Common::Rect(307,  63, 331, 103), true,  52, 3);
	objectAddPuzzleAccessibility(kObject40201, kPuzzle40060, Common::Rect(253,  63, 280, 103), true,  52, 4);
	objectAddPuzzleAccessibility(kObject40201, kPuzzle40060, Common::Rect(200,  63, 228, 103), true,  52, 5);
	objectAddPuzzleAccessibility(kObject40201, kPuzzle40060, Common::Rect(150,  63, 176, 103), true,  52, 6);
	objectAddPresentation(kObject40201);
	objectPresentationAddAnimationToPuzzle(kObject40201, 0, kPuzzle40060, "ROS00N06P01S03", 0, Common::Point(457, 17), 1, 1000, 70, 25.0f, 6);
	objectPresentationSetAnimationOnPuzzle(kObject40201, 0, 0, kObject40100);
	objectAddPresentation(kObject40201);
	objectPresentationAddAnimationToPuzzle(kObject40201, 1, kPuzzle40060, "ROS00N06P01S04", 0, Common::Point(412, 19), 1, 1000, 70, 25.0f, 6);
	objectPresentationSetAnimationOnPuzzle(kObject40201, 1, 0, kObject40101);
	objectAddPresentation(kObject40201);
	objectPresentationAddAnimationToPuzzle(kObject40201, 2, kPuzzle40060, "ROS00N06P01S05", 0, Common::Point(359, 20), 1, 1000, 70, 25.0f, 6);
	objectPresentationSetAnimationOnPuzzle(kObject40201, 2, 0, kObject40102);
	objectAddPresentation(kObject40201);
	objectPresentationAddAnimationToPuzzle(kObject40201, 3, kPuzzle40060, "ROS00N06P01S06", 0, Common::Point(298, 23), 1, 1000, 70, 25.0f, 6);
	objectPresentationSetAnimationOnPuzzle(kObject40201, 3, 0, kObject40103);
	objectAddPresentation(kObject40201);
	objectPresentationAddAnimationToPuzzle(kObject40201, 4, kPuzzle40060, "ROS00N06P01S07", 0, Common::Point(239, 20), 1, 1000, 70, 25.0f, 6);
	objectPresentationSetAnimationOnPuzzle(kObject40201, 4, 0, kObject40104);
	objectAddPresentation(kObject40201);
	objectPresentationAddAnimationToPuzzle(kObject40201, 5, kPuzzle40060, "ROS00N06P01S08", 0, Common::Point(173, 22), 1, 1000, 70, 25.0f, 6);
	objectPresentationSetAnimationOnPuzzle(kObject40201, 5, 0, kObject40105);
	objectAddPresentation(kObject40201);
	objectPresentationAddAnimationToPuzzle(kObject40201, 6, kPuzzle40060, "ROS00N06P01S09", 0, Common::Point(109, 22), 1, 1000, 70, 25.0f, 6);
	objectPresentationSetAnimationOnPuzzle(kObject40201, 6, 0, kObject40106);
	objectAddPresentation(kObject40201);
	objectPresentationAddAnimationToPuzzle(kObject40201, 7, kPuzzle40060, "ROS00N06P02S01", 0, Common::Point(454, 61), 1, 1000, 26, 25.0f, 6);
	objectPresentationSetAnimationOnPuzzle(kObject40201, 7, 0, kObject40201);
	objectAddPresentation(kObject40201);
	objectPresentationAddAnimationToPuzzle(kObject40201, 8, kPuzzle40060, "ROS00N06P02S02", 0, Common::Point(400, 62), 1, 1000, 26, 25.0f, 6);
	objectPresentationSetAnimationOnPuzzle(kObject40201, 8, 0, kObject40202);
	objectAddPresentation(kObject40201);
	objectPresentationAddAnimationToPuzzle(kObject40201, 9, kPuzzle40060, "ROS00N06P02S03", 0, Common::Point(349, 61), 1, 1000, 26, 25.0f, 6);
	objectPresentationSetAnimationOnPuzzle(kObject40201, 9, 0, kObject40203);
	objectAddPresentation(kObject40201);
	objectPresentationAddAnimationToPuzzle(kObject40201, 10, kPuzzle40060, "ROS00N06P02S04", 0, Common::Point(297, 63), 1, 1000, 26, 25.0f, 6);
	objectPresentationSetAnimationOnPuzzle(kObject40201, 10, 0, kObject40204);
	objectAddPresentation(kObject40201);
	objectPresentationAddAnimationToPuzzle(kObject40201, 11, kPuzzle40060, "ROS00N06P02S05", 0, Common::Point(247, 62), 1, 1000, 26, 25.0f, 6);
	objectPresentationSetAnimationOnPuzzle(kObject40201, 11, 0, kObject40205);
	objectAddPresentation(kObject40201);
	objectPresentationAddAnimationToPuzzle(kObject40201, 12, kPuzzle40060, "ROS00N06P02S06", 0, Common::Point(195, 63), 1, 1000, 26, 25.0f, 6);
	objectPresentationSetAnimationOnPuzzle(kObject40201, 12, 0, kObject40206);
	objectAddPresentation(kObject40201);
	objectPresentationAddAnimationToPuzzle(kObject40201, 13, kPuzzle40060, "ROS00N06P02S07", 0, Common::Point(142, 62), 1, 1000, 26, 25.0f, 6);
	objectPresentationSetAnimationOnPuzzle(kObject40201, 13, 0, kObject40207);
	objectAdd(kObject40202, "", "", 3);
	objectAddPresentation(kObject40202);
	objectPresentationAddImageToPuzzle(kObject40202, 0, kPuzzle40060, "ROS00N06P01L01.0001.bmp", Common::Point(0, 16), true, 1, 1000);
	objectAddPresentation(kObject40202);
	objectPresentationAddImageToPuzzle(kObject40202, 1, kPuzzle40060, "ROS00N06P01L01.0002.bmp", Common::Point(57, 360), true, 1, 1000);
	objectAddPresentation(kObject40202);
	objectPresentationAddImageToPuzzle(kObject40202, 2, kPuzzle40060, "ROS00N06P01L01.0003.bmp", Common::Point(100, 353), true, 1, 1000);
	objectAddPresentation(kObject40202);
	objectPresentationAddImageToPuzzle(kObject40202, 3, kPuzzle40060, "ROS00N06P01L01.0004.bmp", Common::Point(127, 353), true, 1, 1000);
	objectAddPresentation(kObject40202);
	objectPresentationAddImageToPuzzle(kObject40202, 4, kPuzzle40060, "ROS00N06P01L01.0005.bmp", Common::Point(165, 355), true, 1, 1000);
	objectAddPresentation(kObject40202);
	objectPresentationAddImageToPuzzle(kObject40202, 5, kPuzzle40060, "ROS00N06P01L01.0006.bmp", Common::Point(197, 355), true, 1, 1000);
	objectAddPresentation(kObject40202);
	objectPresentationAddImageToPuzzle(kObject40202, 6, kPuzzle40060, "ROS00N06P01L01.0007.bmp", Common::Point(234, 352), true, 1, 1000);
	objectAddPresentation(kObject40202);
	objectPresentationAddImageToPuzzle(kObject40202, 7, kPuzzle40060, "ROS00N06P01L01.0008.bmp", Common::Point(260, 342), true, 1, 1000);
	objectAddPresentation(kObject40202);
	objectPresentationAddImageToPuzzle(kObject40202, 8, kPuzzle40060, "ROS00N06P01L01.0009.bmp", Common::Point(299, 331), true, 1, 1000);
	objectAddPresentation(kObject40202);
	objectPresentationAddImageToPuzzle(kObject40202, 9, kPuzzle40060, "ROS00N06P01L01.0010.bmp", Common::Point(330, 349), true, 1, 1000);
	objectAddPresentation(kObject40202);
	objectPresentationAddImageToPuzzle(kObject40202, 10, kPuzzle40060, "ROS00N06P01L01.0011.bmp", Common::Point(364, 335), true, 1, 1000);
	objectAddPresentation(kObject40202);
	objectPresentationAddImageToPuzzle(kObject40202, 11, kPuzzle40060, "ROS00N06P01L01.0012.bmp", Common::Point(393, 354), true, 1, 1000);
	objectAddPresentation(kObject40202);
	objectPresentationAddImageToPuzzle(kObject40202, 12, kPuzzle40060, "ROS00N06P01L01.0013.bmp", Common::Point(421, 353), true, 1, 1000);
	objectAddPresentation(kObject40202);
	objectPresentationAddImageToPuzzle(kObject40202, 13, kPuzzle40060, "ROS00N06P01L01.0014.bmp", Common::Point(454, 358), true, 1, 1000);
	objectAddPresentation(kObject40202);
	objectPresentationAddImageToPuzzle(kObject40202, 14, kPuzzle40060, "ROS00N06P01L01.0015.bmp", Common::Point(484, 345), true, 1, 1000);
	objectAddPresentation(kObject40202);
	objectPresentationAddImageToPuzzle(kObject40202, 15, kPuzzle40060, "ROS00N06P01L01.0016.bmp", Common::Point(516, 349), true, 1, 1000);
	objectAddPuzzleAccessibility(kObject40202, kPuzzle40060, Common::Rect( 67, 384, 100, 438), false, 52, 0);
	objectAddPuzzleAccessibility(kObject40202, kPuzzle40060, Common::Rect(101, 384, 133, 438), false, 52, 1);
	objectAddPuzzleAccessibility(kObject40202, kPuzzle40060, Common::Rect(136, 384, 164, 438), false, 52, 2);
	objectAddPuzzleAccessibility(kObject40202, kPuzzle40060, Common::Rect(169, 384, 200, 438), false, 52, 3);
	objectAddPuzzleAccessibility(kObject40202, kPuzzle40060, Common::Rect(203, 384, 234, 438), false, 52, 4);
	objectAddPuzzleAccessibility(kObject40202, kPuzzle40060, Common::Rect(236, 384, 267, 438), false, 52, 5);
	objectAddPuzzleAccessibility(kObject40202, kPuzzle40060, Common::Rect(269, 384, 299, 438), false, 52, 6);
	objectAddPuzzleAccessibility(kObject40202, kPuzzle40060, Common::Rect(303, 384, 335, 438), false, 52, 7);
	objectAddPuzzleAccessibility(kObject40202, kPuzzle40060, Common::Rect(336, 384, 368, 438), false, 52, 8);
	objectAddPuzzleAccessibility(kObject40202, kPuzzle40060, Common::Rect(368, 384, 400, 438), false, 52, 9);
	objectAddPuzzleAccessibility(kObject40202, kPuzzle40060, Common::Rect(402, 384, 436, 438), false, 52, 10);
	objectAddPuzzleAccessibility(kObject40202, kPuzzle40060, Common::Rect(435, 384, 466, 438), false, 52, 11);
	objectAddPuzzleAccessibility(kObject40202, kPuzzle40060, Common::Rect(466, 384, 499, 438), false, 52, 12);
	objectAddPuzzleAccessibility(kObject40202, kPuzzle40060, Common::Rect(503, 384, 534, 438), false, 52, 13);
	objectAddPuzzleAccessibility(kObject40202, kPuzzle40060, Common::Rect(535, 384, 571, 438), false, 52, 14);
	objectAdd(kObject40300, "", "", 1);
	objectAddPresentation(kObject40300);
	objectPresentationAddAnimationToPuzzle(kObject40300, 0, kPuzzle40101, "ROS00N01A01_B", 0, Common::Point(301, 217), 1, 1000, 19, 12.5f, 4);
	soundAdd(40001, kSoundTypeAmbientMusic, "1799.was", _configuration.ambientMusic.loadFrom, 2, _configuration.ambientMusic.soundChunck);
	soundAdd(40604, kSoundTypeAmbientMusic, "1800.was", _configuration.ambientMusic.loadFrom, 2, _configuration.ambientMusic.soundChunck);
	rotationAddAmbientSound(40000, 40001, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(40001, 40001, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(40002, 40001, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(40003, 40001, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(40004, 40001, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(40005, 40001, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle40010, 40001, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle40011, 40001, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle40012, 40001, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle40013, 40001, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle40100, 40001, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle40101, 40001, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle40102, 40001, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle40103, 40001, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle40104, 40001, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(40000, 40604, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(40001, 40604, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(40002, 40604, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(40003, 40604, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(40004, 40604, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(40005, 40604, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle40060, 40604, 100, 0, 1, 1, 10);
	rotationSetAmbientSoundOff(40000, 40604);
	rotationSetAmbientSoundOff(40001, 40604);
	rotationSetAmbientSoundOff(40002, 40604);
	rotationSetAmbientSoundOff(40003, 40604);
	rotationSetAmbientSoundOff(40004, 40604);
	rotationSetAmbientSoundOff(40005, 40604);
	puzzleSetAmbientSoundOff(kPuzzle40060, 40604);
	soundAdd(40002, kSoundTypeAmbientEffect, "1802.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(40003, kSoundTypeAmbientEffect, "1803.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(40102, kSoundTypeEffect, "1318.wav", _configuration.effect.loadFrom, 2, _configuration.effect.soundChunck);
	soundSetVolume(40102, 80);
	soundAdd(40103, kSoundTypeEffect, "1804.wav", _configuration.effect.loadFrom, 2, _configuration.effect.soundChunck);
	soundSetVolume(40103, 80);
	soundAdd(40500, kSoundTypeEffect, "1805.wav", _configuration.effect.loadFrom, 2, _configuration.effect.soundChunck);
	soundAdd(40501, kSoundTypeEffect, "1806.wav", _configuration.effect.loadFrom, 2, _configuration.effect.soundChunck);
	soundAdd(40502, kSoundTypeEffect, "1807.wav", _configuration.effect.loadFrom, 2, _configuration.effect.soundChunck);
	soundAdd(40503, kSoundTypeEffect, "1808.wav", _configuration.effect.loadFrom, 2, _configuration.effect.soundChunck);
	soundAdd(40504, kSoundTypeEffect, "1809.wav", _configuration.effect.loadFrom, 2, _configuration.effect.soundChunck);
	soundAdd(40505, kSoundTypeEffect, "1810.wav", _configuration.effect.loadFrom, 2, _configuration.effect.soundChunck);
	soundAdd(40506, kSoundTypeEffect, "1811.wav", _configuration.effect.loadFrom, 2, _configuration.effect.soundChunck);
	soundAdd(40507, kSoundTypeEffect, "1805.wav", _configuration.effect.loadFrom, 2, _configuration.effect.soundChunck);
	soundAdd(40508, kSoundTypeEffect, "1806.wav", _configuration.effect.loadFrom, 2, _configuration.effect.soundChunck);
	soundAdd(40509, kSoundTypeEffect, "1807.wav", _configuration.effect.loadFrom, 2, _configuration.effect.soundChunck);
	soundAdd(40510, kSoundTypeEffect, "1811.wav", _configuration.effect.loadFrom, 2, _configuration.effect.soundChunck);
	soundAdd(40511, kSoundTypeEffect, "1812.wav", _configuration.effect.loadFrom, 2, _configuration.effect.soundChunck);
	soundAdd(40512, kSoundTypeEffect, "1810.wav", _configuration.effect.loadFrom, 2, _configuration.effect.soundChunck);
	soundAdd(40513, kSoundTypeEffect, "1809.wav", _configuration.effect.loadFrom, 2, _configuration.effect.soundChunck);
	soundAdd(40514, kSoundTypeEffect, "1808.wav", _configuration.effect.loadFrom, 2, _configuration.effect.soundChunck);
	soundAdd(40600, kSoundTypeEffect, "1813.wav", _configuration.effect.loadFrom, 2, _configuration.effect.soundChunck);
	soundAdd(40601, kSoundTypeEffect, "1814.wav", _configuration.effect.loadFrom, 2, _configuration.effect.soundChunck);
	soundAdd(40602, kSoundTypeEffect, "1815.wav", _configuration.effect.loadFrom, 2, _configuration.effect.soundChunck);
	soundAdd(40603, kSoundTypeEffect, "1816.was", _configuration.effect.loadFrom, 2, _configuration.effect.soundChunck);
	soundAdd(40400, kSoundTypeDialog, "1817.wav", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(40200, kSoundTypeDialog, "1818.wav", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(40201, kSoundTypeDialog, "1819.wav", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(40202, kSoundTypeDialog, "1820.wav", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(40203, kSoundTypeDialog, "1821.wav", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(40204, kSoundTypeDialog, "1822.wav", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(40205, kSoundTypeDialog, "1823.wav", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(40206, kSoundTypeDialog, "1824.wav", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(40300, kSoundTypeDialog, "1825.wav", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(40301, kSoundTypeDialog, "1826.wav", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(40302, kSoundTypeDialog, "1827.wav", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(40303, kSoundTypeDialog, "1828.wav", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(40304, kSoundTypeDialog, "1829.wav", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(40305, kSoundTypeDialog, "1830.wav", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(40306, kSoundTypeDialog, "1831.wav", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(40700, kSoundTypeDialog, "1832.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(40701, kSoundTypeDialog, "1833.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(40706, kSoundTypeDialog, "1834.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(40707, kSoundTypeDialog, "1835.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(40708, kSoundTypeDialog, "1836.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(40709, kSoundTypeDialog, "1837.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(40710, kSoundTypeDialog, "1838.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(40711, kSoundTypeDialog, "1839.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(40702, kSoundTypeDialog, "1840.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(40703, kSoundTypeDialog, "1841.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(40704, kSoundTypeDialog, "1842.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(40705, kSoundTypeDialog, "1843.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	rotationAdd3DSound(40000, 40002, 1, 1, 10, 87, 290.0f, 10);
	rotationAdd3DSound(40001, 40002, 1, 1, 10, 85, 194.0f, 5);
	rotationAdd3DSound(40002, 40002, 1, 1, 10, 83, 141.0f, 5);
	rotationAdd3DSound(40003, 40002, 1, 1, 10, 83, 233.0f, 5);
	rotationAdd3DSound(40004, 40002, 1, 1, 10, 80, 189.0f, 2);
	rotationAdd3DSound(40005, 40002, 1, 1, 10, 77, 187.0f, 2);
	puzzleAdd3DSound(kPuzzle40010, 40002, 1, 1, 10, 93, 290.0f, 2);
	puzzleAdd3DSound(kPuzzle40011, 40002, 1, 1, 10, 95, 290.0f, 2);
	puzzleAdd3DSound(kPuzzle40012, 40002, 1, 1, 10, 100, 290.0f, 2);
	puzzleAdd3DSound(kPuzzle40013, 40002, 1, 1, 10, 97, 290.0f, 2);
	puzzleAdd3DSound(kPuzzle40100, 40002, 1, 1, 10, 90, 290.0f, 2);
	puzzleAdd3DSound(kPuzzle40101, 40002, 1, 1, 10, 90, 290.0f, 2);
	puzzleAdd3DSound(kPuzzle40102, 40002, 1, 1, 10, 90, 290.0f, 2);
	puzzleAdd3DSound(kPuzzle40103, 40002, 1, 1, 10, 90, 290.0f, 2);
	puzzleAdd3DSound(kPuzzle40104, 40002, 1, 1, 10, 90, 290.0f, 2);
	rotationAdd3DSound(40000, 40003, 1, 1, 10, 70, 0.0f, 2);
	rotationAdd3DSound(40001, 40003, 1, 1, 10, 75, 0.0f, 5);
	rotationAdd3DSound(40002, 40003, 1, 1, 10, 72, 25.0f, 5);
	rotationAdd3DSound(40003, 40003, 1, 1, 10, 72, 333.0f, 5);
	rotationAdd3DSound(40004, 40003, 1, 1, 10, 76, 0.0f, 10);
	rotationAdd3DSound(40005, 40003, 1, 1, 10, 82, 0.0f, 10);
	varDefineByte(40000, 0);
	varDefineByte(40601, 0);
	varDefineByte(40602, 0);
	varDefineByte(40603, 0);
	varDefineByte(40604, 0);
	varDefineByte(40605, 0);
	varDefineByte(40701, 0);
	varDefineByte(40702, 0);
	varDefineByte(40703, 0);
	varDefineByte(40801, 0);
	varDefineByte(40200, 1);
	varDefineByte(40201, 0);
	varDefineByte(40202, 1);
	varDefineByte(40203, 0);
	varDefineByte(40204, 0);
	varDefineByte(40205, 1);
	varDefineByte(40206, 0);
	varDefineByte(40802, 0);
	varDefineByte(40803, 0);
	varDefineByte(40804, 0);
	varDefineByte(40805, 56);
	varDefineByte(40806, 0);
	varDefineByte(40807, 0);
	varDefineByte(40901, 26);
	varDefineByte(40902, 26);
	varDefineByte(40903, 26);
	varDefineByte(40904, 26);
	varDefineByte(40905, 26);
	varDefineByte(40906, 26);
	varDefineByte(40907, 26);
	varDefineByte(40911, 70);
	varDefineByte(40912, 70);
	varDefineByte(40913, 70);
	varDefineByte(40914, 70);
	varDefineByte(40915, 70);
	varDefineByte(40916, 70);
	varDefineByte(40917, 70);
	varDefineString(40901, "0000000");
	varDefineString(40902, "00000000");
}

void ApplicationRing::initZoneWA() {
	debugC(1, kRingDebugLogic, "Initializing Brnnnhilde zone (WA)...");

	setZoneAndEnableBag(kZoneWA);

	puzzleAdd(kPuzzle51001);
	puzzleAddBackgroundImage(kPuzzle51001, "TR_WA_A06.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle51002);
	puzzleAddBackgroundImage(kPuzzle51002, "TR_WA_A09.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle51003);
	puzzleAddBackgroundImage(kPuzzle51003, "TR_WA_AS.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle51004);
	puzzleAddBackgroundImage(kPuzzle51004, "TR_WA_BS.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle51005);
	puzzleAddBackgroundImage(kPuzzle51005, "TR_WA_CS.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle51006);
	puzzleAddBackgroundImage(kPuzzle51006, "TR_WA_DS.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle51007);
	puzzleAddBackgroundImage(kPuzzle51007, "TR_WA_ES.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle51008);
	puzzleAddBackgroundImage(kPuzzle51008, "TR_WA_FS.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle51009);
	puzzleAddBackgroundImage(kPuzzle51009, "TR_WA_GS.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle51010);
	puzzleAddBackgroundImage(kPuzzle51010, "TR_WA_HS.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle51011);
	puzzleAddBackgroundImage(kPuzzle51011, "TR_WA_IS.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle51012);
	puzzleAddBackgroundImage(kPuzzle51012, "TR_WA_JS.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle51013);
	puzzleAddBackgroundImage(kPuzzle51013, "TR_WA_KS.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle50100);
	puzzleAddBackgroundImage(kPuzzle50100, "WAS01N08P01.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle50601);
	puzzleAddBackgroundImage(kPuzzle50601, "WAS06N01P01.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle50701);
	puzzleAddBackgroundImage(kPuzzle50701, "WAS07N01P01S01.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle50702);
	puzzleAddBackgroundImage(kPuzzle50702, "WAS07N01P01S02.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle50703);
	puzzleAddBackgroundImage(kPuzzle50703, "WAS07N01P01S03.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle50602);
	puzzleAddBackgroundImage(kPuzzle50602, "WAS06N01P02.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle50001);
	puzzleAddBackgroundImage(kPuzzle50001, "WAS00N01P01L01.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle50002);
	puzzleAddBackgroundImage(kPuzzle50002, "WAS00N01P01.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle50303);
	puzzleAddBackgroundImage(kPuzzle50303, "WAS03N03P01.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle50304);
	puzzleAddBackgroundImage(kPuzzle50304, "WAS03N04P01.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle50400);
	puzzleAddBackgroundImage(kPuzzle50400, "WAS04N02P01L01.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle50401);
	puzzleAddBackgroundImage(kPuzzle50401, "WAS04N02P02.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle50501);
	puzzleAddBackgroundImage(kPuzzle50501, "WAS05N02P01.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle50502);
	puzzleAddBackgroundImage(kPuzzle50502, "WAS05N02P02.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle50503);
	puzzleAddBackgroundImage(kPuzzle50503, "WAS05N02P03.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle50504);
	puzzleAddBackgroundImage(kPuzzle50504, "WAS05N02P04.bmp", Common::Point(0, 16), true);
	rotationAdd(50001, "WAS00N01", 0, 0);
	rotationAdd(50101, "WAS01N01", 0, 0);
	rotationAdd(50102, "WAS01N02", 0, 0);
	rotationAdd(50103, "WAS01N03", 0, 2);
	rotationAdd(50104, "WAS01N04", 0, 0);
	rotationAdd(50105, "WAS01N05", 0, 0);
	rotationAdd(50106, "WAS01N06", 0, 0);
	rotationAdd(50107, "WAS01N07", 0, 0);
	rotationAdd(50108, "WAS01N08", 0, 0);
	rotationAdd(50201, "WAS02N01", 0, 3);
	rotationAdd(50202, "WAS02N02", 0, 3);
	rotationAdd(50301, "WAS03N01", 0, 1);
	rotationAdd(50302, "WAS03N02", 0, 1);
	rotationAdd(50303, "WAS03N03", 0, 1);
	rotationAdd(50304, "WAS03N04", 0, 6);
	rotationSetComBufferLength(50304, 1400000);
	rotationAdd(50401, "WAS04N01", 0, 1);
	rotationAdd(50402, "WAS04N02", 0, 2);
	rotationAdd(50501, "WAS05N01", 0, 2);
	rotationAdd(50502, "WAS05N02", 0, 7);
	rotationAdd(50601, "WAS06N01", 0, 0);
	rotationAdd(50602, "WAS06N02", 0, 0);
	rotationAdd(50701, "WAS07N01", 0, 2);
	rotationSetComBufferLength(50701, 1600000);
	rotationAddMovabilityToRotation(50001, 50101, "1883", Common::Rect(3341, -249, 3599, 242), true, 53, 0);
	rotationSetMovabilityToRotation(50001, 0, 312.0f, 0.3f, 85.3f, 0, 0, 354.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50001, 50101, "1883", Common::Rect(0, -249, 145, 242), true, 53, 0);
	rotationSetMovabilityToRotation(50001, 1, 312.0f, 0.3f, 85.3f, 0, 2, 354.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50101, 50102, "1884", Common::Rect(284, -170, 525, 127), true, 53, 0);
	rotationSetMovabilityToRotation(50101, 0, 357.0f, 0.3f, 85.3f, 0, 2, 357.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50101, 50001, "1885", Common::Rect(1747, -202, 1961, 179), true, 53, 0);
	rotationSetMovabilityToRotation(50101, 1, 140.0f, 0.3f, 85.3f, 0, 2, 140.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50102, 50103, "1886", Common::Rect(33, -280, 377, 52), true, 53, 0);
	rotationSetMovabilityToRotation(50102, 0, 329.0f, 0.3f, 85.3f, 0, 2, 254.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50102, 50101, "1887", Common::Rect(2049, -165, 2378, 198), true, 53, 0);
	rotationSetMovabilityToRotation(50102, 1, 171.0f, 0.3f, 85.3f, 0, 2, 142.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50102, 50201, "1888", Common::Rect(1158, -263, 1524, 240), true, 53, 0);
	rotationSetMovabilityToRotation(50102, 2, 90.0f, 0.3f, 85.3f, 0, 0, 90.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50103, 50104, "1889", Common::Rect(2729, -247, 3072, 81), true, 53, 0);
	rotationSetMovabilityToRotation(50103, 0, 237.0f, 0.3f, 85.3f, 0, 2, 164.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50103, 50102, "1890", Common::Rect(1453, -76, 1738, 293), true, 53, 0);
	rotationSetMovabilityToRotation(50103, 1, 122.0f, 0.3f, 85.3f, 0, 2, 174.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50103, 50701, "1891", Common::Rect(2033, -111, 2416, 340), true, 53, 0);
	rotationSetMovabilityToRotation(50103, 2, 180.0f, 0.3f, 85.3f, 0, 0, 180.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50103, 50701, "", Common::Rect(2033, -111, 2416, 340), false, 53, 0);
	rotationSetMovabilityToRotation(50103, 3, 180.0f, 0.3f, 85.3f, 0, 0, 180.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50104, 50105, "1892", Common::Rect(1882, -291, 2163, 109), true, 53, 0);
	rotationSetMovabilityToRotation(50104, 0, 148.0f, 0.3f, 85.3f, 0, 2, 70.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50104, 50103, "1893", Common::Rect(446, -85, 782, 301), true, 53, 0);
	rotationSetMovabilityToRotation(50104, 1, 25.0f, 0.3f, 85.3f, 0, 2, 117.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50104, 50301, "1894", Common::Rect(3005, -310, 3350, 221), true, 53, 0);
	rotationSetMovabilityToRotation(50104, 2, 274.0f, 0.3f, 85.3f, 0, 0, 270.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50105, 50106, "1895", Common::Rect(1000, -242, 1311, 115), true, 53, 0);
	rotationSetMovabilityToRotation(50105, 0, 68.0f, 0.3f, 85.3f, 0, 2, 335.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50105, 50104, "1896", Common::Rect(3248, -80, 3510, 272), true, 53, 0);
	rotationSetMovabilityToRotation(50105, 1, 294.0f, 0.3f, 85.3f, 0, 2, 14.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50105, 50401, "1897", Common::Rect(2056, -268, 2425, 244), true, 53, 0);
	rotationSetMovabilityToRotation(50105, 2, 180.0f, 0.3f, 85.3f, 0, 0, 180.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50106, 50107, "1898", Common::Rect(82, -261, 355, 59), true, 53, 0);
	rotationSetMovabilityToRotation(50106, 0, 330.0f, 0.3f, 85.3f, 0, 2, 250.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50106, 50105, "1899", Common::Rect(2337, -87, 2612, 268), true, 53, 0);
	rotationSetMovabilityToRotation(50106, 1, 206.0f, 0.3f, 85.3f, 0, 2, 295.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50106, 50501, "1900", Common::Rect(1175, -313, 1546, 223), true, 53, 0);
	rotationSetMovabilityToRotation(50106, 2, 90.0f, 0.3f, 85.3f, 0, 0, 90.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50107, 50108, "1901", Common::Rect(2715, -132, 3090, 104), true, 53, 0);
	rotationSetMovabilityToRotation(50107, 0, 250.0f, 0.3f, 85.3f, 0, 2, 250.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50107, 50106, "1902", Common::Rect(1281, -81, 1546, 277), true, 53, 0);
	rotationSetMovabilityToRotation(50107, 1, 100.0f, 0.3f, 85.3f, 0, 2, 200.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50108, 50107, "1904", Common::Rect(987, -216, 1350, 129), true, 53, 0);
	rotationSetMovabilityToRotation(50108, 0, 74.0f, 0.3f, 85.3f, 0, 2, 74.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50201, 50102, "1905", Common::Rect(2768, -516, 3506, 516), true, 53, 0);
	rotationSetMovabilityToRotation(50201, 0, 270.0f, 0.3f, 85.3f, 0, 0, 270.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50201, 50202, "1906", Common::Rect(1146, -118, 1437, 172), true, 53, 0);
	rotationSetMovabilityToRotation(50201, 1, 90.0f, 0.3f, 85.3f, 0, 0, 90.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50202, 50201, "1907", Common::Rect(2898, -209, 3267, 132), true, 53, 0);
	rotationSetMovabilityToRotation(50202, 0, 265.0f, 0.3f, 85.3f, 0, 2, 265.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50301, 50104, "1908", Common::Rect(975, -520, 1699, 520), true, 53, 0);
	rotationSetMovabilityToRotation(50301, 0, 90.0f, 0.3f, 85.3f, 0, 0, 90.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50301, 50302, "1909", Common::Rect(2896, -146, 3529, 231), true, 53, 0);
	rotationSetMovabilityToRotation(50301, 1, 272.0f, 0.3f, 85.3f, 0, 2, 272.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50301, 50104, "1910", Common::Rect(975, -520, 1699, 520), true, 53, 0);
	rotationSetMovabilityToRotation(50301, 2, 90.0f, 0.3f, 85.3f, 0, 0, 90.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50301, 50302, "1911", Common::Rect(2896, -146, 3529, 231), false, 53, 0);
	rotationSetMovabilityToRotation(50301, 3, 272.0f, 0.3f, 85.3f, 0, 2, 272.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50302, 50301, "1912", Common::Rect(1225, -209, 1634, 143), true, 53, 0);
	rotationSetMovabilityToRotation(50302, 0, 97.0f, 0.3f, 85.3f, 0, 2, 97.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50302, 50303, "1913", Common::Rect(548, -237, 762, 195), true, 53, 0);
	rotationSetMovabilityToRotation(50302, 1, 23.0f, 0.3f, 85.3f, 0, 2, 265.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50302, 50304, "1914", Common::Rect(2112, -78, 2288, 137), true, 53, 0);
	rotationSetMovabilityToRotation(50302, 2, 172.0f, 0.3f, 85.3f, 0, 2, 172.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50302, 50301, "1915", Common::Rect(1225, -209, 1634, 143), false, 53, 0);
	rotationSetMovabilityToRotation(50302, 3, 97.0f, 0.3f, 85.3f, 0, 2, 97.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50302, 50303, "1916", Common::Rect(548, -237, 762, 195), false, 53, 0);
	rotationSetMovabilityToRotation(50302, 4, 23.0f, 0.3f, 85.3f, 0, 2, 265.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50302, 50304, "1917", Common::Rect(2112, -78, 2288, 137), false, 53, 0);
	rotationSetMovabilityToRotation(50302, 5, 172.0f, 0.3f, 85.3f, 0, 2, 172.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50303, 50302, "1918", Common::Rect(2221, 216, 2647, 518), true, 53, 0);
	rotationSetMovabilityToRotation(50303, 0, 191.0f, 0.3f, 85.3f, 0, 2, 191.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50303, 50302, "1919", Common::Rect(2221, 216, 2647, 518), false, 53, 0);
	rotationSetMovabilityToRotation(50303, 1, 191.0f, 0.3f, 85.3f, 0, 2, 191.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50304, 50302, "", Common::Rect(217, -87, 506, 301), true, 53, 0);
	rotationSetMovabilityToRotation(50304, 0, 345.0f, 0.3f, 85.3f, 0, 2, 345.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50304, 50302, "", Common::Rect(217, -87, 506, 301), false, 53, 0);
	rotationSetMovabilityToRotation(50304, 0, 345.0f, 0.3f, 85.3f, 0, 2, 345.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50401, 50105, "1920", Common::Rect(123, -510, 866, 510), true, 53, 0);
	rotationSetMovabilityToRotation(50401, 0, 0, 0.3f, 85.3f, 0, 0, 0, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50401, 50402, "1921", Common::Rect(2148, -275, 2360, 106), true, 53, 0);
	rotationSetMovabilityToRotation(50401, 1, 180.0f, 0.3f, 85.3f, 0, 2, 180.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50401, 50402, "1922", Common::Rect(2148, -275, 2360, 106), false, 53, 0);
	rotationSetMovabilityToRotation(50401, 2, 180.0f, 0.3f, 85.3f, 0, 2, 180.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50402, 50401, "1923", Common::Rect(386, 122, 662, 331), true, 53, 0);
	rotationSetMovabilityToRotation(50402, 0, 0, 0.3f, 85.3f, 0, 0, 0, 0.3f, 85.3f);
	rotationAddMovabilityToPuzzle(50402, kPuzzle50400, "1924", Common::Rect(2191, 296, 2445, 481), true, 52, 0);
	rotationSetMovabilityToPuzzle(50402, 1, 180.0f, 26.0f, 85.3f, 0, 2);
	rotationAddMovabilityToPuzzle(50402, kPuzzle50401, "", Common::Rect(2169, 47, 2392, 200), false, 52, 0);
	rotationSetMovabilityToPuzzle(50402, 2, 180.0f, 26.0f, 85.3f, 0, 2);
	rotationAddMovabilityToRotation(50501, 50106, "1925", Common::Rect(2837, -480, 3327, 355), true, 53, 0);
	rotationSetMovabilityToRotation(50501, 0, 270.0f, 0.3f, 85.3f, 0, 0, 270.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50501, 50502, "1926", Common::Rect(1100, -202, 1420, 174), true, 53, 0);
	rotationSetMovabilityToRotation(50501, 1, 90.0f, 0.3f, 85.3f, 0, 2, 90.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50502, 50501, "1927", Common::Rect(2916, -277, 3179, 174), true, 53, 0);
	rotationSetMovabilityToRotation(50502, 0, 270.0f, 0.3f, 85.3f, 0, 0, 270.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50601, 50108, "1928", Common::Rect(907, -273, 1406, 247), true, 53, 0);
	rotationSetMovabilityToRotation(50601, 0, 70.0f, 0.3f, 85.3f, 0, 2, 70.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50601, 50602, "1929", Common::Rect(3387, -102, 3599, 348), true, 53, 0);
	rotationSetMovabilityToRotation(50601, 1, 296.0f, 0.3f, 85.3f, 0, 2, 296.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50601, 50602, "1929", Common::Rect(0, -102, 195, 348), true, 53, 0);
	rotationSetMovabilityToRotation(50601, 2, 296.0f, 0.3f, 85.3f, 0, 2, 296.0f, 0.3f, 85.3f);
	rotationAddMovabilityToPuzzle(50601, kPuzzle50601, "", Common::Rect(2782, 317, 3250, 552), true, 52, 0);
	rotationSetMovabilityToPuzzle(50601, 3, 180.0f, 26.0f, 85.3f, 0, 2);
	rotationAddMovabilityToPuzzle(50601, kPuzzle50602, "", Common::Rect(2643, -80, 3306, 177), true, 52, 0);
	rotationSetMovabilityToPuzzle(50601, 4, 180.0f, 26.0f, 85.3f, 0, 2);
	rotationAddMovabilityToRotation(50602, 50601, "1930", Common::Rect(1408, -181, 1666, 237), true, 53, 0);
	rotationSetMovabilityToRotation(50602, 0, 117.0f, 0.3f, 85.3f, 0, 2, 117.0f, 0.3f, 85.3f);
	rotationAddMovabilityToRotation(50701, 50103, "1931", Common::Rect(225, -136, 701, 378), true, 53, 0);
	rotationSetMovabilityToRotation(50701, 0, 0, 0.3f, 85.3f, 0, 0, 0, 0.3f, 85.3f);
	puzzleAddMovabilityToRotation(kPuzzle50400, 50402, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle50400, 0, 180.0f, 26.0f, 85.3f);
	puzzleAddMovabilityToRotation(kPuzzle50401, 50402, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle50401, 0, 180.0f, 26.0f, 85.3f);
	puzzleAddMovabilityToRotation(kPuzzle50303, 50303, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle50303, 0, 265.0f, 0.3f, 85.3f);
	puzzleAddMovabilityToRotation(kPuzzle50304, 50304, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle50304, 0, 130.0f, 0.3f, 85.3f);
	puzzleAddMovabilityToRotation(kPuzzle50601, 50601, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle50601, 0, 250.0f, 26.0f, 85.3f);
	puzzleAddMovabilityToPuzzle(kPuzzle50601, kPuzzle50602, "", Common::Rect(0, 0, 640, 32), true, 55, 0);
	puzzleAddMovabilityToRotation(kPuzzle50602, 50601, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle50602, 0, 250.0f, 0.3f, 85.3f);
	puzzleAddMovabilityToPuzzle(kPuzzle50602, kPuzzle50601, "", Common::Rect(0, 0, 640, 32), true, 55, 0);
	puzzleAddMovabilityToRotation(kPuzzle50501, 50502, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle50501, 0, 90.0f, 0.3f, 85.3f);
	puzzleAddMovabilityToRotation(kPuzzle50502, 50502, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle50502, 0, 90.0f, 0.3f, 85.3f);
	puzzleAddMovabilityToRotation(kPuzzle50503, 50502, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle50503, 0, 90.0f, 0.3f, 85.3f);
	puzzleAddMovabilityToRotation(kPuzzle50504, 50502, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle50504, 0, 90.0f, 0.3f, 85.3f);
	objectAdd(kObject51000, "", "", 1);
	objectAddPresentation(kObject51000);
	objectPresentationAddAnimationToPuzzle(kObject51000, 0, kPuzzle51001, "TR_WA_A06", 0, Common::Point(262, 151), 1, 1000, 19, 12.5f, 4);
	objectAddPresentation(kObject51000);
	objectPresentationAddAnimationToPuzzle(kObject51000, 1, kPuzzle51002, "TR_WA_A09", 0, Common::Point(139, 94), 1, 1000, 19, 12.5f, 4);
	objectAddPresentation(kObject51000);
	objectPresentationAddAnimationToPuzzle(kObject51000, 2, kPuzzle51004, "TR_WA_BS", 0, Common::Point(264, 189), 1, 1000, 19, 12.5f, 4);
	objectAddPresentation(kObject51000);
	objectPresentationAddAnimationToPuzzle(kObject51000, 3, kPuzzle51007, "TR_WA_ES", 0, Common::Point(218, 18), 1, 1000, 19, 12.5f, 4);
	objectAddPresentation(kObject51000);
	objectPresentationAddAnimationToPuzzle(kObject51000, 4, kPuzzle51009, "TR_WA_GS", 0, Common::Point(228, 82), 1, 1000, 19, 12.5f, 4);
	objectAddPresentation(kObject51000);
	objectPresentationAddAnimationToPuzzle(kObject51000, 5, kPuzzle51011, "TR_WA_IS", 0, Common::Point(323, 184), 1, 1000, 19, 12.5f, 4);
	objectAddPresentation(kObject51000);
	objectPresentationAddAnimationToPuzzle(kObject51000, 6, kPuzzle51012, "TR_WA_JS", 0, Common::Point(223, 16), 1, 1000, 19, 12.5f, 4);
	objectAddPresentation(kObject51000);
	objectPresentationAddAnimationToPuzzle(kObject51000, 7, kPuzzle51013, "TR_WA_KS", 0, Common::Point(210, 70), 1, 1000, 19, 12.5f, 4);
	objectAdd(kObjectMagicLance, "Beam of light", "WA_Lance", 1);
	objectAddBagAnimation(kObjectMagicLance, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectMagicLance, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectMagicLance, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObjectFeather, "Feather", "WA_Feather", 1);
	objectAddBagAnimation(kObjectFeather, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectFeather, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectFeather, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObjectDeadLeaf, "DeadLeaf", "WA_ItemLeaf", 1);
	objectAddBagAnimation(kObjectDeadLeaf, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectDeadLeaf, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectDeadLeaf, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObjectInk, "Ink", "WA_Ink", 8);
	objectAddBagAnimation(kObjectInk, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectInk, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectInk, 22, 22, 0, 3, 0, 0, 3);
	objectAddPuzzleAccessibility(kObjectInk, kPuzzle50100, Common::Rect(446, 399, 488, 457), true, 52, 0);
	objectAdd(kObjectPaper, "Paper", "WA_Paper", 8);
	objectAddBagAnimation(kObjectPaper, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectPaper, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectPaper, 22, 22, 0, 3, 0, 0, 3);
	objectAddPuzzleAccessibility(kObjectPaper, kPuzzle50100, Common::Rect(487, 386, 526, 453), true, 52, 1);
	objectAdd(kObjectStylet, "Stylet", "WA_Staylet", 8);
	objectAddBagAnimation(kObjectStylet, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectStylet, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectStylet, 22, 22, 0, 3, 0, 0, 3);
	objectAddPuzzleAccessibility(kObjectStylet, kPuzzle50100, Common::Rect(526, 414, 556, 450), true, 52, 2);
	objectAddPuzzleAccessibility(kObjectStylet, kPuzzle50100, Common::Rect(220, 395, 283, 456), true, 52, 8);
	objectAdd(kObjectInkedStylet, "Ink & Stylet", "WA_StyletInk", 8);
	objectAddBagAnimation(kObjectInkedStylet, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectInkedStylet, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectInkedStylet, 22, 22, 0, 3, 0, 0, 3);
	objectAddPuzzleAccessibility(kObjectInkedStylet, kPuzzle50100, Common::Rect(326, 399, 374, 446), true, 52, 9);
	objectAdd(kObject50105, "Ashes", "", 1);
	objectSetActiveCursor(kObject50105, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObject50105, 22, 22, 0, 3, 0, 0, 3);
	objectAddPuzzleAccessibility(kObject50105, kPuzzle50100, Common::Rect(318, 31, 379, 102), true, 52, 0);
	objectAdd(kObject50100, "", "", 1);
	objectAddRotationAccessibility(kObject50100, 50108, Common::Rect(2701, -231, 3260, 345), true, 52, 0);
	objectAddPresentation(kObject50100);
	objectPresentationAddImageToPuzzle(kObject50100, 0, kPuzzle50100, "WAS01N08P01L01_A.bmp", Common::Point(450, 401), true, 1, 1000);
	objectAddPresentation(kObject50100);
	objectPresentationAddImageToPuzzle(kObject50100, 1, kPuzzle50100, "WAS01N08P01L01_B.bmp", Common::Point(490, 388), true, 1, 1000);
	objectAddPresentation(kObject50100);
	objectPresentationAddImageToPuzzle(kObject50100, 2, kPuzzle50100, "WAS01N08P01L01_C.bmp", Common::Point(526, 414), true, 1, 1000);
	objectAddPresentation(kObject50100);
	objectPresentationAddImageToPuzzle(kObject50100, 3, kPuzzle50100, "WAS01N08P01L03.bmp", Common::Point(221, 397), true, 1, 1000);
	objectAddPresentation(kObject50100);
	objectPresentationAddImageToPuzzle(kObject50100, 4, kPuzzle50100, "WAS01N08P01L02.bmp", Common::Point(326, 401), true, 1, 1000);
	objectAddPresentation(kObject50100);
	objectPresentationAddImageToPuzzle(kObject50100, 5, kPuzzle50100, "WAS01N08P01L05.bmp", Common::Point(218, 398), true, 1, 1000);
	objectAddPresentation(kObject50100);
	objectPresentationAddImageToPuzzle(kObject50100, 6, kPuzzle50100, "WAS01N08P01L04.bmp", Common::Point(325, 400), true, 1, 1000);
	objectAddPresentation(kObject50100);
	objectPresentationAddAnimationToPuzzle(kObject50100, 7, kPuzzle50100, "WAS01N08P01S01", 0, Common::Point(270, 101), 1, 1000, 50, 12.5f, 6);
	objectPresentationSetAnimationStartFrame(kObject50100, 7, 1);
	objectPresentationSetAnimationOnPuzzle(kObject50100, 7, 0, kObject50003);
	objectAddPresentation(kObject50100);
	objectPresentationAddAnimationToPuzzle(kObject50100, 8, kPuzzle50100, "WAS01N08P01S02", 0, Common::Point(316, 30), 1, 1000, 50, 12.5f, 6);
	objectPresentationSetAnimationOnPuzzle(kObject50100, 8, 0, kObject50004);
	objectPresentationAddImageToPuzzle(kObject50100, 8, kPuzzle50100, "WAS01N08P01S01.bmp", Common::Point(270, 101), true, 1, 1000);
	objectPresentationShow(kObject50100, 0);
	objectPresentationShow(kObject50100, 1);
	objectPresentationShow(kObject50100, 2);
	objectAdd(kObject50600, "", "", 1);
	objectAddPresentation(kObject50600);
	objectPresentationAddAnimationToPuzzle(kObject50600, 0, kPuzzle50601, "WAS06N01P01S01", 0, Common::Point(171, 182), 1, 1000, 50, 12.5f, 4);
	objectPresentationSetAnimationOnPuzzle(kObject50600, 0, 0, kObject50001);
	objectAddPresentation(kObject50600);
	objectPresentationAddAnimationToPuzzle(kObject50600, 1, kPuzzle50602, "WAS06N01P02S01", 0, Common::Point(77, 134), 1, 1000, 30, 12.5f, 4);
	objectPresentationSetAnimationOnPuzzle(kObject50600, 1, 0, kObject50002);
	objectAddPresentation(kObject50600);
	objectPresentationAddImageToPuzzle(kObject50600, 2, kPuzzle50601, "WAS06N01P01L01.bmp", Common::Point(206, 149), true, 1, 1000);
	objectAddPresentation(kObject50600);
	objectPresentationAddImageToPuzzle(kObject50600, 3, kPuzzle50601, "WAS06N01P01L02.bmp", Common::Point(383, 146), true, 1, 1000);
	objectAddPresentation(kObject50600);
	objectPresentationAddImageToPuzzle(kObject50600, 4, kPuzzle50601, "WAS06N01P01L03.bmp", Common::Point(225, 282), true, 1, 1000);
	objectAddPresentation(kObject50600);
	objectPresentationAddImageToPuzzle(kObject50600, 5, kPuzzle50601, "WAS06N01P01L04.bmp", Common::Point(380, 294), true, 1, 1000);
	objectAddPresentation(kObject50600);
	objectPresentationAddImageToPuzzle(kObject50600, 6, kPuzzle50602, "WAS06N01P02L01.bmp", Common::Point(115, 178), true, 1, 1000);
	objectAddPresentation(kObject50600);
	objectPresentationAddImageToPuzzle(kObject50600, 7, kPuzzle50602, "WAS06N01P02L02.bmp", Common::Point(114, 158), true, 1, 1000);
	objectPresentationShow(kObject50600, 0);
	objectPresentationShow(kObject50600, 1);
	objectAddPuzzleAccessibility(kObject50600, kPuzzle50601, Common::Rect(208, 148, 237, 177), true, 52, 0);
	objectAddPuzzleAccessibility(kObject50600, kPuzzle50601, Common::Rect(384, 148, 411, 172), true, 52, 1);
	objectAddPuzzleAccessibility(kObject50600, kPuzzle50601, Common::Rect(227, 281, 253, 308), true, 52, 2);
	objectAddPuzzleAccessibility(kObject50600, kPuzzle50601, Common::Rect(381, 290, 410, 322), true, 52, 3);
	objectAdd(kObject50001, "", "", 1);
	objectAddPresentation(kObject50001);
	objectPresentationAddAnimationToPuzzle(kObject50001, 0, kPuzzle50002, "WAS00N01P01S01", 0, Common::Point(414, 63), 1, 1000, 20, 12.5f, 4);
	objectAddPresentation(kObject50001);
	objectPresentationAddAnimationToPuzzle(kObject50001, 1, kPuzzle50001, "WAS00N01P01S02", 0, Common::Point(256, 263), 1, 1000, 20, 12.5f, 4);
	objectAdd(kObjectGolem1, "Golem", "FO_Golem", 1);
	objectAddBagAnimation(kObjectGolem1, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectGolem1, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectGolem1, 22, 22, 0, 3, 0, 0, 3);
	objectAddPuzzleAccessibility(kObjectGolem1, kPuzzle50400, Common::Rect(232, 168, 422, 403), true, 52, 0);
	objectAddPresentation(kObjectGolem1);
	objectPresentationAddImageToRotation(kObjectGolem1, 0, 50402, 0);
	objectPresentationAddImageToRotation(kObjectGolem1, 0, 50401, 0);
	objectAddPresentation(kObjectGolem1);
	objectPresentationAddImageToPuzzle(kObjectGolem1, 1, kPuzzle50400, "WAS04N02P01L02.bmp", Common::Point(212, 178), true, 1, 998);
	objectAdd(kObjectFronthead, "Head front", "WA_GolemHeadF", 8);
	objectAddBagAnimation(kObjectFronthead, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectFronthead, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectFronthead, 22, 22, 0, 3, 0, 0, 3);
	objectAddPuzzleAccessibility(kObjectFronthead, kPuzzle50400, Common::Rect(298, 159, 355, 233), false, 52, 0);
	objectAddPuzzleAccessibility(kObjectFronthead, kPuzzle50400, Common::Rect(215,  92, 269, 160), false, 52, 1);
	objectAddPresentation(kObjectFronthead);
	objectPresentationAddImageToPuzzle(kObjectFronthead, 0, kPuzzle50400, "WAS04N02P01L09.bmp", Common::Point(296, 160), true, 1, 1000);
	objectAddPresentation(kObjectFronthead);
	objectPresentationAddImageToPuzzle(kObjectFronthead, 1, kPuzzle50400, "WAS04N02P01L03.bmp", Common::Point(209, 87), true, 1, 1000);
	objectAdd(kObjectBackhead, "Head back", "WA_GolemHeadB", 8);
	objectAddBagAnimation(kObjectBackhead, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectBackhead, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectBackhead, 22, 22, 0, 3, 0, 0, 3);
	objectAddPuzzleAccessibility(kObjectBackhead, kPuzzle50400, Common::Rect(298, 159, 355, 233), false, 52, 0);
	objectAddPuzzleAccessibility(kObjectBackhead, kPuzzle50400, Common::Rect(399,  93, 450, 158), false, 52, 20);
	objectAddPresentation(kObjectBackhead);
	objectPresentationAddImageToPuzzle(kObjectBackhead, 0, kPuzzle50400, "WAS04N02P01L21.bmp", Common::Point(301, 157), true, 1, 999);
	objectAddPresentation(kObjectBackhead);
	objectPresentationAddImageToPuzzle(kObjectBackhead, 1, kPuzzle50400, "WAS04N02P01L06.bmp", Common::Point(403, 89), true, 1, 1000);
	objectAdd(kObjectBelly, "Belly", "WA_GolemStomak", 8);
	objectAddBagAnimation(kObjectBelly, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectBelly, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectBelly, 22, 22, 0, 3, 0, 0, 3);
	objectAddPuzzleAccessibility(kObjectBelly, kPuzzle50400, Common::Rect(298, 257, 358, 299), false, 52, 0);
	objectAddPuzzleAccessibility(kObjectBelly, kPuzzle50400, Common::Rect(135, 301, 212, 358), false, 52, 300);
	objectAddPresentation(kObjectBelly);
	objectPresentationAddImageToPuzzle(kObjectBelly, 0, kPuzzle50400, "WAS04N02P01L13.bmp", Common::Point(292, 252), true, 1, 1000);
	objectPresentationAddImageToPuzzle(kObjectBelly, 0, kPuzzle50400, "WAS04N02P01L14.bmp", Common::Point(302, 293), true, 1, 1000);
	objectAddPresentation(kObjectBelly);
	objectPresentationAddImageToPuzzle(kObjectBelly, 1, kPuzzle50400, "WAS04N02P01L07.bmp", Common::Point(142, 302), true, 1, 1000);
	objectAdd(kObjectRightArm, "Arm right", "WA_GolemArmR", 8);
	objectAddBagAnimation(kObjectRightArm, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectRightArm, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectRightArm, 22, 22, 0, 3, 0, 0, 3);
	objectAddPuzzleAccessibility(kObjectRightArm, kPuzzle50400, Common::Rect(241, 213, 276, 253), false, 52, 0);
	objectAddPuzzleAccessibility(kObjectRightArm, kPuzzle50400, Common::Rect(151, 206, 200, 255), false, 52, 4000);
	objectAddPresentation(kObjectRightArm);
	objectPresentationAddImageToPuzzle(kObjectRightArm, 0, kPuzzle50400, "WAS04N02P01L10.bmp", Common::Point(224, 203), true, 1, 1000);
	objectAddPresentation(kObjectRightArm);
	objectPresentationAddImageToPuzzle(kObjectRightArm, 1, kPuzzle50400, "WAS04N02P01L04.bmp", Common::Point(147, 206), true, 1, 1000);
	objectAdd(kObjectLeftArm, "Arm left", "WA_GolemArmL", 8);
	objectAddBagAnimation(kObjectLeftArm, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectLeftArm, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectLeftArm, 22, 22, 0, 3, 0, 0, 3);
	objectAddPuzzleAccessibility(kObjectLeftArm, kPuzzle50400, Common::Rect(380, 217, 414, 249), false, 52, 0);
	objectAddPuzzleAccessibility(kObjectLeftArm, kPuzzle50400, Common::Rect(443, 211, 488, 253), false, 52, 50000);
	objectAddPresentation(kObjectLeftArm);
	objectPresentationAddImageToPuzzle(kObjectLeftArm, 0, kPuzzle50400, "WAS04N02P01L11.bmp", Common::Point(375, 210), true, 1, 1000);
	objectAddPresentation(kObjectLeftArm);
	objectPresentationAddImageToPuzzle(kObjectLeftArm, 1, kPuzzle50400, "WAS04N02P01L05.bmp", Common::Point(440, 208), true, 1, 1000);
	objectAdd(kObjectLegs, "Legs", "WA_GolemLeg", 8);
	objectAddBagAnimation(kObjectLegs, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectLegs, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectLegs, 22, 22, 0, 3, 0, 0, 3);
	objectAddPuzzleAccessibility(kObjectLegs, kPuzzle50400, Common::Rect(268, 306, 384, 391), false, 52, 0);
	objectAddPuzzleAccessibility(kObjectLegs, kPuzzle50400, Common::Rect(445, 297, 534, 349), false, 52, 600000);
	objectAddPresentation(kObjectLegs);
	objectPresentationAddImageToPuzzle(kObjectLegs, 0, kPuzzle50400, "WAS04N02P01L15.bmp", Common::Point(269, 282), true, 1, 1000);
	objectPresentationAddImageToPuzzle(kObjectLegs, 0, kPuzzle50400, "WAS04N02P01L16.bmp", Common::Point(268, 299), true, 1, 1000);
	objectPresentationAddImageToPuzzle(kObjectLegs, 0, kPuzzle50400, "WAS04N02P01L17.bmp", Common::Point(263, 307), true, 1, 1000);
	objectPresentationAddImageToPuzzle(kObjectLegs, 0, kPuzzle50400, "WAS04N02P01L18.bmp", Common::Point(365, 285), true, 1, 1000);
	objectPresentationAddImageToPuzzle(kObjectLegs, 0, kPuzzle50400, "WAS04N02P01L19.bmp", Common::Point(348, 301), true, 1, 1000);
	objectPresentationAddImageToPuzzle(kObjectLegs, 0, kPuzzle50400, "WAS04N02P01L20.bmp", Common::Point(334, 309), true, 1, 1000);
	objectAddPresentation(kObjectLegs);
	objectPresentationAddImageToPuzzle(kObjectLegs, 1, kPuzzle50400, "WAS04N02P01L08.bmp", Common::Point(440, 301), true, 1, 1000);
	objectAdd(kObjectHeart, "Heart", "WA_GolemHeart", 8);
	objectAddBagAnimation(kObjectHeart, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectHeart, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectHeart, 22, 22, 0, 3, 0, 0, 3);
	objectAddPuzzleAccessibility(kObjectHeart, kPuzzle50400, Common::Rect(342, 231, 363, 251), false, 52, 0);
	objectAddPuzzleAccessibility(kObjectHeart, kPuzzle50400, Common::Rect(325,  50, 340,  95), false, 52, 7000000);
	objectAddPresentation(kObjectHeart);
	objectPresentationAddImageToPuzzle(kObjectHeart, 0, kPuzzle50400, "WAS04N02P01L12.bmp", Common::Point(343, 231), true, 1, 1000);
	objectAddPresentation(kObjectHeart);
	objectPresentationAddImageToPuzzle(kObjectHeart, 1, kPuzzle50400, "WAS04N02P01L22.bmp", Common::Point(315, 74), true, 1, 1000);
	objectAdd(kObject50451, "", "WA_glava", 8);
	objectAdd(kObject50452, "", "WA_droka", 8);
	objectAdd(kObject50453, "", "WA_lroka", 8);
	objectAdd(kObject50454, "", "WA_trup", 8);
	objectAdd(kObject50455, "", "WA_lnoga", 8);
	objectAdd(kObject50456, "", "WA_dnoga", 8);
	objectAdd(kObject50457, "", "WA_srce", 8);

	uint32 val = 115;
	for (uint32 i = 0; i < 7; i++) {
		objectSetActiveCursor((ObjectId)(kObject50451 + i), 22, 22, 0, 3, 0, 0, 3);
		objectSetPassiveCursor((ObjectId)(kObject50451 + i), 22, 22, 0, 3, 0, 0, 3);
		objectAddPuzzleAccessibility((ObjectId)(kObject50451 + i), kPuzzle50401, Common::Rect(125, val, 160, i >= 6 ? val + 35 : val), i >= 6 ? false : true, 52, 0);
		objectAddPresentation((ObjectId)(kObject50451 + i));
		objectPresentationAddImageToPuzzle((ObjectId)(kObject50451 + i), 0, kPuzzle50401, Common::String::format("WAS04N02P02_L%d.tga", i + 1), Common::Point(125, val), true, 3, 1000);
		objectPresentationShow((ObjectId)(kObject50451 + i));
		val += 37;
	}

	for (uint32 i = 0; i < 7; i++) {
		uint32 l = 1;
		for (uint32 j = 115; j < (115 + 7 * 37); j += 37) {
			for (uint32 k = 162; k < (162 + 7 * 53); k += 53) {
				objectAddPuzzleAccessibility((ObjectId)(kObject50451 + i), kPuzzle50401, Common::Rect(k, j, k + 53, j + 37), false, 52, l * 10);
			}
			l++;
		}
	}

	objectAdd(kObjectMagnet, "", "", 8);

	uint32 l = 1;
	for (uint32 j = 115; j < (115 + 7 * 37); j += 37) {
		for (uint32 k = 162; k < (162 + 7 * 53); k += 53) {
			objectAddPuzzleAccessibility(kObjectMagnet, kPuzzle50401, Common::Rect(k, j, k + 53, j + 37), false, 52, l * 10);
		}
		l++;
	}

	objectAdd(kObjectThread, "Rope", "WA_Rope", 1);
	objectAddBagAnimation(kObjectThread, 1, 3, 1, 12.5f, 4);
	objectSetActiveCursor(kObjectThread, 22, 22, 1, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectThread, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObjectTree, "", "", 1);
	objectAddPresentation(kObjectTree);
	objectPresentationAddImageToRotation(kObjectTree, 0, 50502, 0);
	objectPresentationAddImageToRotation(kObjectTree, 0, 50501, 0);
	objectAddPresentation(kObjectTree);
	objectPresentationAddImageToRotation(kObjectTree, 1, 50502, 1);
	objectPresentationAddImageToPuzzle(kObjectTree, 1, kPuzzle50501, "WAS05N02P01L01.BMP", Common::Point(281, 164), true, 1, 1000);
	objectAddPresentation(kObjectTree);
	objectPresentationAddImageToRotation(kObjectTree, 2, 50502, 2);
	objectPresentationAddImageToPuzzle(kObjectTree, 2, kPuzzle50502, "WAS05N02P02L01.BMP", Common::Point(226, 189), true, 1, 1000);
	objectAddPresentation(kObjectTree);
	objectPresentationAddImageToRotation(kObjectTree, 3, 50502, 3);
	objectPresentationAddImageToPuzzle(kObjectTree, 3, kPuzzle50503, "WAS05N02P03L01.BMP", Common::Point(215, 200), true, 1, 1000);
	objectAddPresentation(kObjectTree);
	objectPresentationAddImageToRotation(kObjectTree, 4, 50502, 4);
	objectPresentationAddImageToPuzzle(kObjectTree, 4, kPuzzle50504, "WAS05N02P04L01.BMP", Common::Point(213, 140), true, 1, 1000);
	objectAddPresentation(kObjectTree);
	objectPresentationAddImageToRotation(kObjectTree, 5, 50502, 5);
	objectPresentationShow(kObjectTree, 5);
	objectAddPresentation(kObjectTree);
	objectPresentationAddImageToRotation(kObjectTree, 6, 50502, 6);
	objectPresentationAddImageToRotation(kObjectTree, 6, 50501, 1);
	objectAdd(kObject50700, "", "", 1);
	objectAddRotationAccessibility(kObject50700, 50701, Common::Rect(1975, -568, 2378, -240), true, 52, 0);
	objectAddRotationAccessibility(kObject50700, 50701, Common::Rect(2033, -172, 2411, 289), false, 52, 1);
	objectAddRotationAccessibility(kObject50700, 50701, Common::Rect(2079, 211, 2429, 537), false, 52, 2);
	objectAddPresentation(kObject50700);
	objectPresentationAddImageToRotation(kObject50700, 0, 50103, 0);
	objectPresentationAddImageToRotation(kObject50700, 0, 50701, 0);
	objectAddPresentation(kObject50700);
	objectPresentationAddImageToRotation(kObject50700, 1, 50103, 1);
	objectPresentationAddImageToRotation(kObject50700, 1, 50701, 1);
	objectAddPresentation(kObject50700);
	objectPresentationAddAnimationToPuzzle(kObject50700, 2, kPuzzle50701, "WAS07N01P01S01", 0, Common::Point(293, 187), 1, 1000, 20, 12.5f, 4);
	objectAddPresentation(kObject50700);
	objectPresentationAddAnimationToPuzzle(kObject50700, 3, kPuzzle50702, "WAS07N01P01S02", 0, Common::Point(304, 65), 1, 1000, 20, 12.5f, 4);
	objectAdd(kObjectCounch1, "Conch", "WA_Conch", 1);
	objectAddBagAnimation(kObjectCounch1, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectCounch1, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectCounch1, 22, 22, 0, 3, 0, 0, 3);
	objectAddRotationAccessibility(kObjectCounch1, 50304, Common::Rect(1582, -54, 2009, 247), true, 52, 0);
	objectAddRotationAccessibility(kObjectCounch1, 50304, Common::Rect(2424, -10, 2612, 136), false, 52, 1);
	objectAddPuzzleAccessibility(kObjectCounch1, kPuzzle50304, Common::Rect(184, 191, 389, 391), true, 52, 0);
	objectAddPresentation(kObjectCounch1);
	objectPresentationAddImageToRotation(kObjectCounch1, 0, 50304, 0);
	objectAddPresentation(kObjectCounch1);
	objectPresentationAddImageToRotation(kObjectCounch1, 1, 50304, 1);
	objectAddPresentation(kObjectCounch1);
	objectPresentationAddImageToRotation(kObjectCounch1, 2, 50304, 2);
	objectAddPresentation(kObjectCounch1);
	objectPresentationAddImageToRotation(kObjectCounch1, 3, 50304, 3);
	objectAddPresentation(kObjectCounch1);
	objectPresentationAddImageToRotation(kObjectCounch1, 4, 50304, 4);
	objectAddPresentation(kObjectCounch1);
	objectPresentationAddImageToRotation(kObjectCounch1, 5, 50304, 5);
	objectAdd(kObjectPhoenix, "", "", 1);
	objectAddRotationAccessibility(kObjectPhoenix, 50303, Common::Rect(2896, -146, 3529, 231), true, 52, 0);
	objectAddPuzzleAccessibility(kObjectPhoenix, kPuzzle50303, Common::Rect(178, 89, 443, 384), true, 52, 0);
	objectAddPresentation(kObjectPhoenix);
	objectPresentationAddImageToRotation(kObjectPhoenix, 0, 50301, 0);
	objectPresentationAddImageToRotation(kObjectPhoenix, 0, 50302, 0);
	objectPresentationAddImageToRotation(kObjectPhoenix, 0, 50303, 0);
	objectAdd(kObjectRopes, "", "", 1);
	objectAddRotationAccessibility(kObjectRopes, 50202, Common::Rect(653, -270, 775, 125), true, 52, 0);
	objectAddRotationAccessibility(kObjectRopes, 50202, Common::Rect(1001, -298, 1023, 130), true, 52, 1);
	objectAddRotationAccessibility(kObjectRopes, 50202, Common::Rect(1536, -287, 1633, 120), true, 52, 2);
	objectAddRotationAccessibility(kObjectRopes, 50202, Common::Rect(1745, -387, 1965, 211), true, 52, 3);
	objectAddPresentation(kObjectRopes);
	objectPresentationAddImageToRotation(kObjectRopes, 0, 50201, 0);
	objectPresentationAddImageToRotation(kObjectRopes, 0, 50202, 0);
	objectAddPresentation(kObjectRopes);
	objectPresentationAddImageToRotation(kObjectRopes, 1, 50201, 1);
	objectPresentationAddImageToRotation(kObjectRopes, 1, 50202, 1);
	objectAddPresentation(kObjectRopes);
	objectPresentationAddImageToRotation(kObjectRopes, 2, 50201, 2);
	objectPresentationAddImageToRotation(kObjectRopes, 2, 50202, 2);
	objectAdd(kObjectCloud, "", "", 1);
	objectAddRotationAccessibility(kObjectCloud, 50202, Common::Rect(0, -600, 1689, -219), true, 52, 0);
	objectAdd(kObjectCounch, "", "", 1);
	objectAdd(kObjectBark, "Bark", "WA_ItemBark", 9);
	objectAddBagAnimation(kObjectBark, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectBark, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectBark, 22, 22, 0, 3, 0, 0, 3);
	objectAddRotationAccessibility(kObjectBark, 50302, Common::Rect(2858, 251, 3011, 512), false, 52, 0);
	objectAdd(kObjectFlower, "Flower", "WA_ItemFlower", 9);
	objectAddBagAnimation(kObjectFlower, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectFlower, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectFlower, 22, 22, 0, 3, 0, 0, 3);
	objectAddRotationAccessibility(kObjectFlower, 50402, Common::Rect(2689, 190, 2937, 301), true, 52, 0);
	objectAddPresentation(kObjectFlower);
	objectPresentationAddImageToRotation(kObjectFlower, 0, 50402, 1);
	objectPresentationShow(kObjectFlower);
	objectAdd(kObjectDragonSword, "Sword", "WA_Sword", 1);
	objectAddBagAnimation(kObjectDragonSword, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectDragonSword, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectDragonSword, 22, 22, 0, 3, 0, 0, 3);
	objectAddRotationAccessibility(kObjectDragonSword, 50502, Common::Rect(1093, 176, 1383, 381), true, 52, 0);
	objectAdd(kObjectApple, "Apple", "WA_ItemFruit", 1);
	objectAddBagAnimation(kObjectApple, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectApple, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectApple, 22, 22, 0, 3, 0, 0, 3);
	objectAddRotationAccessibility(kObjectApple, 50502, Common::Rect(407, 190, 513, 289), true, 52, 0);
	objectAdd(kObjectTotems, "", "", 1);
	objectAddPuzzleAccessibility(kObjectTotems, kPuzzle50501, Common::Rect(223, 134, 430, 409), true, 52, 0);
	objectAddPuzzleAccessibility(kObjectTotems, kPuzzle50502, Common::Rect(195, 120, 388, 392), true, 52, 1);
	objectAddPuzzleAccessibility(kObjectTotems, kPuzzle50503, Common::Rect(217,  92, 423, 405), true, 52, 2);
	objectAddPuzzleAccessibility(kObjectTotems, kPuzzle50504, Common::Rect(196, 115, 413, 441), true, 52, 3);
	objectAddRotationAccessibility(kObjectTotems, 50502, Common::Rect( 947, -20, 1014, 43), true, 52, 10);
	objectAddRotationAccessibility(kObjectTotems, 50502, Common::Rect(1091, -29, 1154, 48), true, 52, 11);
	objectAddRotationAccessibility(kObjectTotems, 50502, Common::Rect(1469, -31, 1527, 45), true, 52, 12);
	objectAddRotationAccessibility(kObjectTotems, 50502, Common::Rect(1597, -24, 1650, 50), true, 52, 13);
	objectAdd(kObject50601, "", "", 1);
	objectAddRotationAccessibility(kObject50601, 50602, Common::Rect(3225, 17, 3364, 144), true, 52, 0);
	objectAddRotationAccessibility(kObject50601, 50602, Common::Rect(423, -205, 667, 8), true, 52, 1);
	objectAddRotationAccessibility(kObject50601, 50602, Common::Rect(868, 0, 996, 95), true, 52, 2);
	soundAdd(50017, kSoundTypeAmbientMusic, "1932.wav", _configuration.ambientMusic.loadFrom, 2, _configuration.ambientMusic.soundChunck);
	puzzleAddAmbientSound(kPuzzle50602, 50017, 100, 0, 1, 1, 10);
	soundSetVolume(50017, 90);
	soundAdd(51002, kSoundTypeAmbientMusic, "1938.was", _configuration.ambientMusic.loadFrom, 2, _configuration.ambientMusic.soundChunck);
	soundSetVolume(51002, 95);
	soundAdd(51004, kSoundTypeAmbientMusic, "1939.was", _configuration.ambientMusic.loadFrom, 2, _configuration.ambientMusic.soundChunck);
	soundSetVolume(51004, 100);
	soundAdd(51003, kSoundTypeAmbientMusic, "1940.was", _configuration.ambientMusic.loadFrom, 2, _configuration.ambientMusic.soundChunck);
	soundSetVolume(51003, 95);
	soundAdd(51001, kSoundTypeAmbientMusic, "1941.was", _configuration.ambientMusic.loadFrom, 2, _configuration.ambientMusic.soundChunck);
	soundSetVolume(51001, 95);
	soundAdd(51006, kSoundTypeAmbientMusic, "1942.wav", _configuration.ambientMusic.loadFrom, 2, _configuration.ambientMusic.soundChunck);
	soundSetVolume(51006, 95);
	soundAdd(51007, kSoundTypeAmbientMusic, "1943.was", _configuration.ambientMusic.loadFrom, 2, _configuration.ambientMusic.soundChunck);
	soundSetVolume(51007, 90);
	soundAdd(51008, kSoundTypeAmbientMusic, "1944.was", _configuration.ambientMusic.loadFrom, 2, _configuration.ambientMusic.soundChunck);
	soundSetVolume(51008, 95);
	soundAdd(51010, kSoundTypeAmbientMusic, "1945.was", _configuration.ambientMusic.loadFrom, 2, _configuration.ambientMusic.soundChunck);
	soundAdd(51011, kSoundTypeAmbientMusic, "1946.was", _configuration.ambientMusic.loadFrom, 2, _configuration.ambientMusic.soundChunck);
	soundAdd(51012, kSoundTypeAmbientMusic, "1947.was", _configuration.ambientMusic.loadFrom, 2, _configuration.ambientMusic.soundChunck);
	soundAdd(51013, kSoundTypeAmbientMusic, "1948.was", _configuration.ambientMusic.loadFrom, 2, _configuration.ambientMusic.soundChunck);
	soundAdd(50018, kSoundTypeAmbientEffect, "1949.was", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(50023, kSoundTypeDialog, "1933.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(50024, kSoundTypeDialog, "1934.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(50025, kSoundTypeDialog, "1935.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(50001, kSoundTypeDialog, "1950.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(50002, kSoundTypeDialog, "1951.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(50003, kSoundTypeDialog, "1952.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(50009, kSoundTypeDialog, "1953.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(50010, kSoundTypeDialog, "1954.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(50011, kSoundTypeDialog, "1955.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(50012, kSoundTypeDialog, "1956.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(50013, kSoundTypeDialog, "1957.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(50014, kSoundTypeDialog, "1958.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(50015, kSoundTypeDialog, "1959.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(50016, kSoundTypeDialog, "1960.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(50021, kSoundTypeDialog, "1961.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(50022, kSoundTypeDialog, "1962.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(50026, kSoundTypeDialog, "1963.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(50027, kSoundTypeDialog, "1964.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(50028, kSoundTypeDialog, "1965.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(50029, kSoundTypeDialog, "1966.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(50030, kSoundTypeDialog, "1967.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(50031, kSoundTypeDialog, "1968.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(50032, kSoundTypeDialog, "1969.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(50033, kSoundTypeDialog, "1970.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(50034, kSoundTypeDialog, "1971.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(50035, kSoundTypeDialog, "1972.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(50036, kSoundTypeDialog, "1973.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	varDefineByte(50001, 0);
	varDefineByte(50003, 0);
	varDefineWord(50000, 0);
	varDefineByte(50004, 0);
	varDefineByte(50005, 0);
	varDefineDword(50000, 0);
	varDefineWord(50101, 0);
	varDefineWord(50102, 0);
	varDefineWord(50103, 0);
	varDefineWord(50104, 0);
	varDefineWord(50105, 0);
	varDefineByte(50006, 0);
	varDefineByte(50007, 0);
	varDefineByte(50008, 0);
	varDefineByte(50009, 0);
	varDefineByte(50010, 0);
	varDefineByte(50011, 0);
	varDefineByte(50012, 0);

	for (uint32 i = 0; i < 78; i++)
		varDefineDword(51000 + i, 0);
}

void ApplicationRing::initZoneAS() {
	debugC(1, kRingDebugLogic, "Initializing Dril zone (AS)...");

	setZoneAndEnableBag(kZoneAS);

	_loadFrom = kLoadFromDisk;
	_archiveType = kArchiveFile;

	puzzleAdd(kPuzzle80001);
	puzzleAddBackgroundImage(kPuzzle80001, "Old_Ish.bma", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle80011);
	puzzleAddBackgroundImage(kPuzzle80011, "DRIL_02.bma", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle80012);
	puzzleAddBackgroundImage(kPuzzle80012, "DRIL_03.bma", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle80013);
	puzzleAddBackgroundImage(kPuzzle80013, "DRIL_05.bma", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle80014);
	puzzleAddBackgroundImage(kPuzzle80014, "DRIL_06.bma", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle80002);
	puzzleAddBackgroundImage(kPuzzle80002, "ASV01.bma", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle80003);
	puzzleAddBackgroundImage(kPuzzle80003, "ASV02.bma", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle80004);
	puzzleAddBackgroundImage(kPuzzle80004, "ASV03.bma", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle80005);
	puzzleAddBackgroundImage(kPuzzle80005, "ASV04.bma", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle80008);
	puzzleAddBackgroundImage(kPuzzle80008, "ASP04L01.bma", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle80010);
	puzzleAddBackgroundImage(kPuzzle80010, "ASP05L01.bma", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle80009);
	puzzleAddBackgroundImage(kPuzzle80009, "ASP03L01.bma", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle80006);
	puzzleAddBackgroundImage(kPuzzle80006, "ASP01L01.bma", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle80007);
	puzzleAddBackgroundImage(kPuzzle80007, "ASP02L01.bma", Common::Point(0, 16), true);

	_loadFrom = kLoadFromCd;
	if (_configuration.artAS)
		_archiveType = kArchiveArt;

	rotationAdd(80001, "ASS00N01", 0, 1);
	rotationAdd(80002, "ASS00N02", 0, 1);
	rotationAdd(80003, "ASS00N03", 0, 1);
	rotationAdd(80004, "ASS00N04", 0, 1);
	rotationAdd(80005, "ASS00N05", 0, 1);
	rotationAdd(80006, "ASS00N06", 0, 1);
	rotationAdd(80007, "ASS00N07", 0, 1);
	rotationAdd(80008, "ASS00N08", 0, 1);
	rotationAdd(80009, "ASS00N09", 0, 1);
	rotationAdd(80010, "ASS00N10", 0, 1);
	rotationAdd(80011, "ASS00N11", 0, 1);
	rotationAdd(80013, "ASS00N13", 0, 1);
	rotationAdd(80014, "ASS00N14", 0, 1);
	rotationAdd(80015, "ASS00N15", 0, 1);

	_loadFrom = kLoadFromDisk;
	rotationAdd(80101, "ASS01N01", 0, 6);
	_loadFrom = kLoadFromCd;

	rotationSetComBufferLength(80101, 1800000);
	rotationAddMovabilityToRotation(80001, 80002, "1001", Common::Rect(757, -71, 928, 85), true, 53, 0);
	rotationSetMovabilityToRotation(80001, 0, 52.0f, 0, 85.3f, 0, 0, 52.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80001, 80003, "1002", Common::Rect(1251, -64, 1444, 95), true, 53, 0);
	rotationSetMovabilityToRotation(80001, 1, 90.0f, 0, 85.3f, 0, 0, 90.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80001, 80004, "1003", Common::Rect(1784, -62, 1982, 90), true, 53, 0);
	rotationSetMovabilityToRotation(80001, 2, 128.0f, 0, 85.3f, 0, 0, 128.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80002, 80001, "1004", Common::Rect(2696, -167, 2956, 81), true, 53, 0);
	rotationSetMovabilityToRotation(80002, 0, 232.0f, 0, 85.3f, 0, 0, 232.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80002, 80003, "1005", Common::Rect(2130, -78, 2366, 130), true, 53, 0);
	rotationSetMovabilityToRotation(80002, 1, 182.0f, 0, 85.3f, 0, 0, 182.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80002, 80005, "1006", Common::Rect(1634, -64, 1910, 87), true, 53, 0);
	rotationSetMovabilityToRotation(80002, 2, 131.0f, 0, 85.3f, 0, 0, 131.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80003, 80002, "1007", Common::Rect(321, -69, 548, 108), true, 53, 0);
	rotationSetMovabilityToRotation(80003, 0, 2.0f, 0, 85.3f, 0, 0, 2.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80003, 80005, "1008", Common::Rect(1168, -69, 1538, 106), true, 53, 0);
	rotationSetMovabilityToRotation(80003, 1, 90.0f, 0, 85.3f, 0, 0, 90.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80003, 80004, "1009", Common::Rect(2137, -66, 2394, 99), true, 53, 0);
	rotationSetMovabilityToRotation(80003, 2, 178.0f, 0, 85.3f, 0, 0, 178.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80003, 80013, "1010", Common::Rect(2721, -317, 2882, -198), true, 53, 0);
	rotationSetMovabilityToRotation(80003, 3, 231.0f, -26.0f, 85.3f, 0, 0, 270.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80003, 80014, "1011", Common::Rect(3406, -319, 3578, -204), true, 53, 0);
	rotationSetMovabilityToRotation(80003, 4, 309.0f, -26.0f, 85.3f, 0, 0, 270.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80003, 80001, "1012", Common::Rect(3053, -97, 3246, 57), true, 53, 0);
	rotationSetMovabilityToRotation(80003, 5, 270.0f, 0, 85.3f, 0, 0, 270.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80003, 80101, "1013", Common::Rect(952, 326, 1798, 590), true, 53, 0);
	rotationSetMovabilityToRotation(80003, 6, 90.0f, 26.0f, 85.3f, 0, 0, 90.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80003, 80015, "1014", Common::Rect(1306, -270, 1397, -162), true, 53, 0);
	rotationSetMovabilityToRotation(80003, 7, 90.0f, -24.0f, 85.3f, 0, 0, 90.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80004, 80001, "1015", Common::Rect(3387, -95, 3596, 69), true, 53, 0);
	rotationSetMovabilityToRotation(80004, 0, 308.0f, 0, 85.3f, 0, 0, 308.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80004, 80003, "1016", Common::Rect(374, -22, 541, 104), true, 53, 0);
	rotationSetMovabilityToRotation(80004, 1, 358.0f, 0, 85.3f, 0, 0, 358.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80004, 80005, "1017", Common::Rect(810, -27, 1089, 73), true, 53, 0);
	rotationSetMovabilityToRotation(80004, 2, 49.0f, 0, 85.3f, 0, 0, 49.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80005, 80003, "1018", Common::Rect(3053, -153, 3244, -6), true, 53, 0);
	rotationSetMovabilityToRotation(80005, 0, 270.0f, 0, 85.3f, 0, 0, 270.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80005, 80003, "1018", Common::Rect(2984, -24, 3090, 172), true, 53, 0);
	rotationSetMovabilityToRotation(80005, 1, 270.0f, 0, 85.3f, 0, 0, 270.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80005, 80003, "1018", Common::Rect(3230, -24, 3336, 172), true, 53, 0);
	rotationSetMovabilityToRotation(80005, 2, 270.0f, 0, 85.3f, 0, 0, 270.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80005, 80002, "1019", Common::Rect(3408, -115, 3600, 102), true, 53, 0);
	rotationSetMovabilityToRotation(80005, 3, 311.0f, 0, 85.3f, 0, 0, 311.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80005, 80002, "1019", Common::Rect(0, -115, 135, 102), true, 53, 0);
	rotationSetMovabilityToRotation(80005, 4, 311.0f, 0, 85.3f, 0, 0, 311.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80005, 80004, "1020", Common::Rect(2617, -71, 2870, 83), true, 53, 0);
	rotationSetMovabilityToRotation(80005, 5, 229.0f, 0, 85.3f, 0, 0, 229.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80005, 80008, "1021", Common::Rect(1209, -109, 1313, 181), true, 53, 0);
	rotationSetMovabilityToRotation(80005, 6, 90.0f, 0, 85.3f, 0, 0, 90.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80005, 80008, "1021", Common::Rect(1397, -130, 1509, 181), true, 53, 0);
	rotationSetMovabilityToRotation(80005, 7, 90.0f, 0, 85.3f, 0, 0, 90.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80006, 80007, "1022", Common::Rect(2026, 413, 2387, 586), true, 53, 0);
	rotationSetMovabilityToRotation(80006, 0, 180.0f, 26.0f, 85.3f, 0, 0, 180.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80006, 80011, "1023", Common::Rect(1740, -33, 1875, 102), true, 53, 0);
	rotationSetMovabilityToRotation(80006, 1, 137.0f, 0, 85.3f, 0, 0, 90.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80007, 80006, "1024", Common::Rect(29, -540, 639, -388), true, 53, 0);
	rotationSetMovabilityToRotation(80007, 0, 0, -26.0f, 85.3f, 0, 0, 0, 0, 85.3f);
	rotationAddMovabilityToRotation(80007, 80008, "1025", Common::Rect(2047, -34, 2427, 163), true, 53, 0);
	rotationSetMovabilityToRotation(80007, 1, 180.0f, 0, 85.3f, 0, 0, 180.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80008, 80007, "1026", Common::Rect(305, -48, 622, 122), true, 53, 0);
	rotationSetMovabilityToRotation(80008, 0, 0, 0, 85.3f, 0, 0, 0, 0, 85.3f);
	rotationAddMovabilityToRotation(80008, 80011, "1027", Common::Rect(1274, -272, 1429, 50), true, 53, 0);
	rotationSetMovabilityToRotation(80008, 1, 90.0f, -26.0f, 85.3f, 0, 0, 90.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80008, 80009, "1028", Common::Rect(2102, -71, 2431, 136), true, 53, 0);
	rotationSetMovabilityToRotation(80008, 2, 180.0f, 0, 85.3f, 0, 0, 180.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80008, 80005, "1029", Common::Rect(3000, -73, 3100, 218), true, 53, 0);
	rotationSetMovabilityToRotation(80008, 3, 270.0f, 0, 85.3f, 0, 0, 270.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80008, 80005, "1029", Common::Rect(3200, -73, 3300, 218), true, 53, 0);
	rotationSetMovabilityToRotation(80008, 4, 270.0f, 0, 85.3f, 0, 0, 270.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80008, 80005, "1029", Common::Rect(3100, -73, 3200, -34), true, 53, 0);
	rotationSetMovabilityToRotation(80008, 5, 270.0f, 0, 85.3f, 0, 0, 270.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80009, 80008, "1031", Common::Rect(333, -33, 678, 176), true, 53, 0);
	rotationSetMovabilityToRotation(80009, 0, 0, 0, 85.3f, 0, 0, 0, 0, 85.3f);
	rotationAddMovabilityToRotation(80009, 80010, "1032", Common::Rect(1745, -495, 2478, -369), true, 53, 0);
	rotationSetMovabilityToRotation(80009, 1, 180.0f, -26.0f, 85.3f, 0, 0, 180.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80010, 80009, "1033", Common::Rect(182, 422, 745, 552), true, 53, 0);
	rotationSetMovabilityToRotation(80010, 0, 0, 26.0f, 85.3f, 0, 0, 0, 0, 85.3f);
	rotationAddMovabilityToRotation(80010, 80011, "1034", Common::Rect(849, -83, 1091, 104), true, 53, 0);
	rotationSetMovabilityToRotation(80010, 1, 43.0f, 0, 85.3f, 0, 0, 90.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80011, 80006, "1035", Common::Rect(3531, -47, 3600, 95), true, 53, 0);
	rotationSetMovabilityToRotation(80011, 0, 317.0f, 0, 85.3f, 0, 0, 317.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80011, 80006, "1035", Common::Rect(0, -47, 249, 95), true, 53, 0);
	rotationSetMovabilityToRotation(80011, 1, 317.0f, 0, 85.3f, 0, 0, 317.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80011, 80008, "1036", Common::Rect(3009, 280, 3320, 469), true, 53, 0);
	rotationSetMovabilityToRotation(80011, 2, 270.0f, 26.0f, 85.3f, 0, 0, 270.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80011, 80010, "1037", Common::Rect(2571, -29, 2784, 80), true, 53, 0);
	rotationSetMovabilityToRotation(80011, 3, 223.0f, 0, 85.3f, 0, 0, 223.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80013, 80014, "1038", Common::Rect(342, -55, 444, 47), true, 53, 0);
	rotationSetMovabilityToRotation(80013, 0, 0, 0, 85.3f, 0, 0, 0, 0, 85.3f);
	rotationAddMovabilityToRotation(80013, 80015, "1039", Common::Rect(1149, -31, 1225, 27), true, 53, 0);
	rotationSetMovabilityToRotation(80013, 1, 73.0f, 0, 85.3f, 0, 0, 73.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80013, 80003, "1040", Common::Rect(892, 244, 1045, 348), true, 53, 0);
	rotationSetMovabilityToRotation(80013, 2, 51.0f, 26.0f, 85.3f, 0, 0, 51.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80014, 80013, "1041", Common::Rect(2253, -57, 2366, 52), true, 53, 0);
	rotationSetMovabilityToRotation(80014, 0, 180.0f, 0, 85.3f, 0, 0, 180.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80014, 80015, "1042", Common::Rect(1478, -41, 1550, 34), true, 53, 0);
	rotationSetMovabilityToRotation(80014, 1, 107.0f, 0, 85.3f, 0, 0, 107.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80014, 80003, "1043", Common::Rect(1668, 238, 1814, 359), true, 53, 0);
	rotationSetMovabilityToRotation(80014, 2, 129.0f, 26.0f, 85.3f, 0, 0, 129.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80015, 80013, "1044", Common::Rect(2951, -36, 3019, 34), true, 53, 0);
	rotationSetMovabilityToRotation(80015, 0, 253.0f, 0, 85.3f, 0, 0, 253.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80015, 80014, "1045", Common::Rect(3276, -29, 3355, 29), true, 53, 0);
	rotationSetMovabilityToRotation(80015, 1, 287.0f, 0, 85.3f, 0, 0, 287.0f, 0, 85.3f);
	rotationAddMovabilityToRotation(80015, 80003, "1046", Common::Rect(3079, 212, 3236, 345), true, 53, 0);
	rotationSetMovabilityToRotation(80015, 2, 270.0f, 26.0f, 85.3f, 0, 0, 270.0f, 0, 85.3f);
	puzzleAddMovabilityToRotation(kPuzzle80002, 80101, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle80002, 0, 51.0f, 0, 85.3f);
	puzzleAddMovabilityToRotation(kPuzzle80003, 80101, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle80003, 0, 75.0f, 0, 85.3f);
	puzzleAddMovabilityToRotation(kPuzzle80004, 80101, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle80004, 0, 102.0f, 0, 85.3f);
	puzzleAddMovabilityToRotation(kPuzzle80005, 80101, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle80005, 0, 130.0f, 0, 85.3f);
	puzzleAddMovabilityToRotation(kPuzzle80006, 80101, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle80006, 0, 90.0f, 17.0f, 85.3f);
	puzzleAddMovabilityToRotation(kPuzzle80007, 80101, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle80007, 0, 90.0f, 17.0f, 85.3f);
	puzzleAddMovabilityToRotation(kPuzzle80008, 80101, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle80008, 0, 90.0f, 17.0f, 85.3f);
	puzzleAddMovabilityToRotation(kPuzzle80009, 80101, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle80009, 0, 90.0f, 17.0f, 85.3f);
	puzzleAddMovabilityToRotation(kPuzzle80010, 80101, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle80010, 0, 90.0f, 17.0f, 85.3f);
	objectAdd(kObject80017, "", "", 1);
	objectAdd(kObject80007, "", "", 1);
	objectAddRotationAccessibility(kObject80007, 80101, Common::Rect(1314, 183, 1397, 226), true, 52, 0);
	objectAdd(kObjectDeath, "Death", "AS_Nor_d", 1);
	objectAddBagAnimation(kObjectDeath, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectDeath, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectDeath, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObject80012, "", "", 1);
	objectAddRotationAccessibility(kObject80012, 80005, Common::Rect(3097, -1, 3197, 176), true, 52, 0);
	objectAddRotationAccessibility(kObject80012, 80005, Common::Rect(1318, -66, 1381, 191), true, 52, 1);
	objectAddRotationAccessibility(kObject80012, 80008, Common::Rect(3095, -27, 3215, 73), true, 52, 2);
	objectAddRotationAccessibility(kObject80012, 80005, Common::Rect(384, -61, 478, 197), true, 52, 3);
	objectAddRotationAccessibility(kObject80012, 80005, Common::Rect(2225, -59, 2308, 190), true, 52, 4);
	objectAddRotationAccessibility(kObject80012, 80005, Common::Rect(467, -195, 606, 3), true, 52, 5);
	objectAddRotationAccessibility(kObject80012, 80005, Common::Rect(2088, -167, 2235, 8), true, 52, 6);
	objectAddRotationAccessibility(kObject80012, 80011, Common::Rect(1202, -156, 1552, 355), true, 52, 7);
	objectAdd(kObject80016, "", "", 1);
	objectAddPresentation(kObject80016);
	objectPresentationAddImageToRotation(kObject80016, 0, 80001, 0);
	objectAddPresentation(kObject80016);
	objectPresentationAddImageToRotation(kObject80016, 1, 80002, 0);
	objectAddPresentation(kObject80016);
	objectPresentationAddImageToRotation(kObject80016, 2, 80003, 0);
	objectAddPresentation(kObject80016);
	objectPresentationAddImageToRotation(kObject80016, 3, 80004, 0);
	objectAddPresentation(kObject80016);
	objectPresentationAddImageToRotation(kObject80016, 4, 80005, 0);
	objectAddPresentation(kObject80016);
	objectPresentationAddImageToRotation(kObject80016, 5, 80006, 0);
	objectAddPresentation(kObject80016);
	objectPresentationAddImageToRotation(kObject80016, 6, 80007, 0);
	objectAddPresentation(kObject80016);
	objectPresentationAddImageToRotation(kObject80016, 7, 80008, 0);
	objectAddPresentation(kObject80016);
	objectPresentationAddImageToRotation(kObject80016, 8, 80009, 0);
	objectAddPresentation(kObject80016);
	objectPresentationAddImageToRotation(kObject80016, 9, 80010, 0);
	objectAddPresentation(kObject80016);
	objectPresentationAddImageToRotation(kObject80016, 10, 80011, 0);
	objectAddPresentation(kObject80016);
	objectPresentationAddImageToRotation(kObject80016, 11, 80013, 0);
	objectAddPresentation(kObject80016);
	objectPresentationAddImageToRotation(kObject80016, 12, 80014, 0);
	objectAddPresentation(kObject80016);
	objectPresentationAddImageToRotation(kObject80016, 13, 80015, 0);
	objectAdd(kObject80018, "", "", 1);
	objectAddRotationAccessibility(kObject80018, 80101, Common::Rect(843, -66, 1008, 188), false, 52, 0);
	objectAddRotationAccessibility(kObject80018, 80101, Common::Rect(1147, -104, 1277, 136), false, 52, 1);
	objectAddRotationAccessibility(kObject80018, 80101, Common::Rect(1418, -97, 1562, 130), false, 52, 2);
	objectAddRotationAccessibility(kObject80018, 80101, Common::Rect(1698, -99, 1849, 186), false, 52, 3);
	objectAddPresentation(kObject80018);
	objectPresentationAddImageToRotation(kObject80018, 0, 80101, 0);
	objectAddPresentation(kObject80018);
	objectPresentationAddAnimationToRotation(kObject80018, 1, 80101, 1, 49, 12.5f, 4);
	objectPresentationSetAnimationOnRotation(kObject80018, 1, 0, kObject80001);
	objectPresentationShow(kObject80018, 1);
	objectPresentationPauseAnimation(kObject80018, 1);
	objectAddPresentation(kObject80018);
	objectPresentationAddImageToRotation(kObject80018, 2, 80101, 2);
	objectAddPresentation(kObject80018);
	objectPresentationAddImageToRotation(kObject80018, 3, 80101, 3);
	objectAddPresentation(kObject80018);
	objectPresentationAddImageToRotation(kObject80018, 4, 80101, 4);
	objectAddPresentation(kObject80018);
	objectPresentationAddImageToRotation(kObject80018, 5, 80101, 5);
	objectAdd(kObject80019, "", "", 1);
	objectAddPuzzleAccessibility(kObject80019, kPuzzle80006, Common::Rect(159, 161, 477, 393), true,  52, 0);
	objectAddPuzzleAccessibility(kObject80019, kPuzzle80006, Common::Rect(159, 161, 477, 393), false, 52, 1);
	objectAddPuzzleAccessibility(kObject80019, kPuzzle80007, Common::Rect(159, 161, 477, 393), true,  52, 2);
	objectAddPuzzleAccessibility(kObject80019, kPuzzle80008, Common::Rect(159, 161, 477, 393), true,  52, 3);
	objectAddPuzzleAccessibility(kObject80019, kPuzzle80009, Common::Rect(159, 161, 477, 393), true,  52, 4);
	objectAddPuzzleAccessibility(kObject80019, kPuzzle80010, Common::Rect(159, 161, 477, 393), true,  52, 5);
	objectSetAccessibilityOff(kObject80019, 1, 1);
	objectAdd(kObject80021, "", "", 1);
	objectAddRotationAccessibility(kObject80021, 80101, Common::Rect(1248, 235, 1466, 348), true, 52, 0);
	objectAddRotationAccessibility(kObject80021, 80101, Common::Rect(1089, 170, 1232, 268), true, 52, 1);
	objectAddRotationAccessibility(kObject80021, 80101, Common::Rect(1181, 125, 1341, 190), true, 52, 2);
	objectAddRotationAccessibility(kObject80021, 80101, Common::Rect(1353, 130, 1552, 183), true, 52, 3);
	objectAddRotationAccessibility(kObject80021, 80101, Common::Rect(1444, 183, 1620, 249), true, 52, 4);
	objectAddPresentation(kObject80019);

	_loadFrom = kLoadFromDisk;
	_archiveType = kArchiveFile;

	objectPresentationAddImageToPuzzle(kObject80019, 0, kPuzzle80006, "ASP01L02.bma", Common::Point(0, 16), true, 1, 1000);

	_loadFrom = kLoadFromCd;
	if (_configuration.artAS)
		_archiveType = kArchiveArt;

	objectAdd(kObject80022, "", "", 1);
	objectAddRotationAccessibility(kObject80022, 80101, Common::Rect(0, -580, 3600, -279), true, 53, 0);
	soundAdd(80200, kSoundTypeAmbientMusic, "1048.was", _configuration.ambientMusic.loadFrom, 2, _configuration.ambientMusic.soundChunck);
	soundAdd(80201, kSoundTypeAmbientMusic, "1049.was", _configuration.ambientMusic.loadFrom, 2, _configuration.ambientMusic.soundChunck);
	soundAdd(80203, kSoundTypeAmbientMusic, "1050.was", _configuration.ambientMusic.loadFrom, 2, _configuration.ambientMusic.soundChunck);
	soundAdd(80204, kSoundTypeAmbientMusic, "1051.was", _configuration.ambientMusic.loadFrom, 2, _configuration.ambientMusic.soundChunck);
	soundAdd(80205, kSoundTypeAmbientMusic, "1052.was", _configuration.ambientMusic.loadFrom, 2, _configuration.ambientMusic.soundChunck);
	soundAdd(80207, kSoundTypeAmbientMusic, "1053.wav", _configuration.ambientMusic.loadFrom, 2, _configuration.ambientMusic.soundChunck);
	rotationAddAmbientSound(80001, 80200, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(80002, 80200, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(80003, 80200, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(80004, 80200, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(80005, 80200, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(80006, 80200, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(80007, 80200, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(80008, 80200, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(80009, 80200, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(80010, 80200, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(80011, 80200, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(80013, 80200, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(80014, 80200, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(80015, 80200, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(80101, 80200, 100, 0, 1, 1, 10);
	rotationAddAmbientSound(80101, 80207, 100, 0, 1, 1, 20);
	puzzleAddAmbientSound(kPuzzle80002, 80201, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle80003, 80203, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle80004, 80204, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle80005, 80205, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle80006, 80207, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle80007, 80207, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle80008, 80207, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle80010, 80207, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle80009, 80207, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle80006, 80200, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle80007, 80200, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle80008, 80200, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle80010, 80200, 100, 0, 1, 1, 10);
	puzzleAddAmbientSound(kPuzzle80009, 80200, 100, 0, 1, 1, 10);
	soundAdd(80206, kSoundTypeAmbientEffect, "1053.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(80018, kSoundTypeAmbientEffect, "1054.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(80019, kSoundTypeAmbientEffect, "1055.was", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(80020, kSoundTypeAmbientEffect, "1056.was", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(80208, kSoundTypeAmbientEffect, "1057.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(80209, kSoundTypeAmbientEffect, "1058.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(80210, kSoundTypeAmbientEffect, "1059.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundSetVolume(80210, 90);
	soundAdd(80004, kSoundTypeDialog, "1060.was", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80005, kSoundTypeDialog, "1061.was", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80006, kSoundTypeDialog, "1062.was", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80007, kSoundTypeDialog, "1063.was", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80008, kSoundTypeDialog, "1064.was", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80009, kSoundTypeDialog, "1065.was", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80010, kSoundTypeDialog, "1066.was", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80011, kSoundTypeDialog, "1067.was", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80012, kSoundTypeDialog, "1068.was", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80013, kSoundTypeDialog, "1069.was", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80014, kSoundTypeDialog, "1070.was", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80015, kSoundTypeDialog, "1071.was", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80040, kSoundTypeDialog, "1072.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80041, kSoundTypeDialog, "1073.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80042, kSoundTypeDialog, "1074.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80043, kSoundTypeDialog, "1075.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80044, kSoundTypeDialog, "1076.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80045, kSoundTypeDialog, "1077.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80046, kSoundTypeDialog, "1078.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80047, kSoundTypeDialog, "1079.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80048, kSoundTypeDialog, "1080.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80049, kSoundTypeDialog, "1081.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80050, kSoundTypeDialog, "1082.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80051, kSoundTypeDialog, "1083.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80052, kSoundTypeDialog, "1084.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80053, kSoundTypeDialog, "1085.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80054, kSoundTypeDialog, "1086.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80055, kSoundTypeDialog, "1087.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80056, kSoundTypeDialog, "1088.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80057, kSoundTypeDialog, "1089.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80058, kSoundTypeDialog, "1090.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80059, kSoundTypeDialog, "1091.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80060, kSoundTypeDialog, "1092.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80061, kSoundTypeDialog, "1093.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80062, kSoundTypeDialog, "1094.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80063, kSoundTypeDialog, "1095.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80064, kSoundTypeDialog, "1096.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80065, kSoundTypeDialog, "1097.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80066, kSoundTypeDialog, "1098.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80067, kSoundTypeDialog, "1099.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80068, kSoundTypeDialog, "1100.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80069, kSoundTypeDialog, "1101.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80070, kSoundTypeDialog, "1102.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80071, kSoundTypeDialog, "1103.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80072, kSoundTypeDialog, "1104.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80073, kSoundTypeDialog, "1105.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80074, kSoundTypeDialog, "1106.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80075, kSoundTypeDialog, "1107.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80076, kSoundTypeDialog, "1108.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80077, kSoundTypeDialog, "1109.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80078, kSoundTypeDialog, "1110.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80079, kSoundTypeDialog, "1111.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80080, kSoundTypeDialog, "1112.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80021, kSoundTypeDialog, "1125.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80022, kSoundTypeDialog, "1126.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80023, kSoundTypeDialog, "1127.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80024, kSoundTypeDialog, "1128.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80025, kSoundTypeDialog, "1129.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80026, kSoundTypeDialog, "1130.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80027, kSoundTypeDialog, "1131.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80028, kSoundTypeDialog, "1132.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80100, kSoundTypeDialog, "1133.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80101, kSoundTypeDialog, "1134.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80102, kSoundTypeDialog, "1135.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80103, kSoundTypeDialog, "1136.wav", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80104, kSoundTypeDialog, "1137.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80105, kSoundTypeDialog, "1138.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80106, kSoundTypeDialog, "1139.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(80107, kSoundTypeDialog, "1140.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	rotationAdd3DSound(80001, 80206, 1, 1, 10, 90, 270.0f, 20);
	varDefineByte(80000, 0);
	varDefineByte(80001, 0);
	varDefineByte(80002, 1);
	varDefineByte(80003, 0);
	varDefineByte(80004, 1);
	varDefineByte(80005, 0);
	varDefineFloat(80001, 1.0);
	varDefineFloat(80002, 2.0);
	varDefineFloat(80003, 100.0);
}

void ApplicationRing::initZoneN2() {
	debugC(1, kRingDebugLogic, "Initializing Loge zone (N2)...");

	setZoneAndEnableBag(kZoneN2);

	rotationAdd(70000, "N2S00N01", 0, 1);
	rotationAdd(70001, "N2S00N02", 0, 3);
	rotationAdd(70100, "N2S01N01", 0, 1);
	rotationAdd(70101, "N2S01N02", 0, 1);
	rotationAdd(70300, "N2S03N01", 0, 2);
	rotationAdd(70400, "N2S04N01", 0, 0);
	rotationAdd(70500, "N2S05N01", 0, 2);
	rotationAdd(70600, "N2S06N01", 0, 0);
	rotationAdd(70200, "N2S07N01", 0, 1);
	puzzleAdd(kPuzzle72001);
	puzzleAddBackgroundImage(kPuzzle72001, "TR_AS_RO_BS.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle72002);
	puzzleAddBackgroundImage(kPuzzle72002, "TR_AS_RO_CS.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle72003);
	puzzleAddBackgroundImage(kPuzzle72003, "TR_AS_RO_DS.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle72004);
	puzzleAddBackgroundImage(kPuzzle72004, "TR_AS_RO_ES.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle72005);
	puzzleAddBackgroundImage(kPuzzle72005, "TR_AS_RO_FS.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle72006);
	puzzleAddBackgroundImage(kPuzzle72006, "TR_AS_RO_HS.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle72007);
	puzzleAddBackgroundImage(kPuzzle72007, "TR_AS_RO_IS.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle72008);
	puzzleAddBackgroundImage(kPuzzle72008, "TR_AS_RO_JS.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle72009);
	puzzleAddBackgroundImage(kPuzzle72009, "TR_AS_RO_KS.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle72010);
	puzzleAddBackgroundImage(kPuzzle72010, "TR_AS_RO_MS.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle70000);
	puzzleAddBackgroundImage(kPuzzle70000, "N2EXTRA_1A.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle70100);
	puzzleAddBackgroundImage(kPuzzle70100, "N2S01N01P02.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle70102);
	puzzleAddBackgroundImage(kPuzzle70102, "N2S01N01P03.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle70300);
	puzzleAddBackgroundImage(kPuzzle70300, "N2S03N01P01S01.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle70301);
	puzzleAddBackgroundImage(kPuzzle70301, "N2S03N01P01S02.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle70302);
	puzzleAddBackgroundImage(kPuzzle70302, "N2S03N01P01S04.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle70303);
	puzzleAddBackgroundImage(kPuzzle70303, "N2S02N01P02S02.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle70304);
	puzzleAddBackgroundImage(kPuzzle70304, "N2S02N01P02S03.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle70305);
	puzzleAddBackgroundImage(kPuzzle70305, "N2S00N01P01S01.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle70306);
	puzzleAddBackgroundImage(kPuzzle70306, "N2S03N01P01S03.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle70410);
	puzzleAddBackgroundImage(kPuzzle70410, "N2S04N03P01.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle70411);
	puzzleAddBackgroundImage(kPuzzle70411, "N2S04N03P03.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle70500);
	puzzleAddBackgroundImage(kPuzzle70500, "N2S05N01P01.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle70501);
	puzzleAddBackgroundImage(kPuzzle70501, "N2S05N01P01L01.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle70510);
	puzzleAddBackgroundImage(kPuzzle70510, "N2S05N01P02.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle70511);
	puzzleAddBackgroundImage(kPuzzle70511, "N2S05N01P02L01.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle70520);
	puzzleAddBackgroundImage(kPuzzle70520, "N2S05N01P03.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle70521);
	puzzleAddBackgroundImage(kPuzzle70521, "N2S05N01P03L01.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle70600);
	puzzleAddBackgroundImage(kPuzzle70600, "N2S07N01P01S01.0001.bmp", Common::Point(0, 16), true);
	puzzleAdd(kPuzzle70601);
	puzzleAddBackgroundImage(kPuzzle70601, "N2S07N01P01S04.0001.bmp", Common::Point(0, 16), true);
	rotationAddMovabilityToRotation(70000, 70001, "1373", Common::Rect(1585, -94, 1815, 111), true, 53, 7);
	rotationSetMovabilityToRotation(70000, 0, 0, 0, 85.7f, 0, 2, 90.0f, 0, 85.7f);
	rotationAddMovabilityToRotation(70000, 70400, "1374", Common::Rect(2515, -165, 2796, 106), true, 53, 3);
	rotationSetMovabilityToRotation(70000, 1, 0, 0, 85.7f, 0, 2, 270.0f, 0, 85.7f);
	rotationAddMovabilityToRotation(70000, 70200, "1375", Common::Rect(314, -261, 817, 167), true, 53, 0);
	rotationSetMovabilityToRotation(70000, 2, 0, 0, 85.7f, 0, 2, 185.0f, 0, 85.7f);
	rotationAddMovabilityToRotation(70001, 70000, "1376", Common::Rect(3397, -81, 3600, 129), true, 53, 7);
	rotationSetMovabilityToRotation(70001, 0, 0, 0, 85.7f, 0, 2, 270.0f, 0, 85.7f);
	rotationAddMovabilityToRotation(70001, 70300, "1377", Common::Rect(2132, -198, 2640, 211), true, 53, 0);
	rotationSetMovabilityToRotation(70001, 1, 0, 0, 85.7f, 0, 2, 150.0f, 0, 85.7f);
	rotationAddMovabilityToRotation(70001, 70100, "1378", Common::Rect(1008, 31, 1218, 230), true, 53, 0);
	rotationSetMovabilityToRotation(70001, 2, 0, 0, 85.7f, 0, 2, 270.0f, 0, 85.7f);
	rotationAddMovabilityToRotation(70001, 70400, "1379", Common::Rect(3227, -76, 3364, 61), true, 53, 0);
	rotationSetMovabilityToRotation(70001, 3, 0, 0, 85.7f, 0, 2, 270.0f, 0, 85.7f);
	rotationAddMovabilityToRotation(70100, 70101, "1380", Common::Rect(1186, -143, 1534, 195), true, 53, 0);
	rotationSetMovabilityToRotation(70100, 0, 0, 0.3f, 85.7f, 0, 2, 90.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(70101, 70100, "1381", Common::Rect(2951, -125, 3350, 193), true, 53, 0);
	rotationSetMovabilityToRotation(70101, 0, 0, 0.3f, 85.7f, 0, 2, 270.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(70101, 70600, "1382", Common::Rect(1323, -57, 1587, 225), true, 53, 16);
	rotationSetMovabilityToRotation(70101, 1, 0, 0.3f, 85.7f, 0, 2, 130.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(70101, 70600, "1383", Common::Rect(1323, -57, 1587, 225), false, 53, 0);
	rotationSetMovabilityToRotation(70101, 2, 0, 0.3f, 85.7f, 0, 2, 224.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(70200, 70000, "1384", Common::Rect(108, -160, 578, 156), true, 53, 7);
	rotationSetMovabilityToRotation(70200, 0, 0, 0.3f, 85.7f, 0, 2, 180.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(70300, 70001, "1385", Common::Rect(318, -202, 789, 226), true, 53, 7);
	rotationSetMovabilityToRotation(70300, 0, 0, 0.3f, 85.7f, 0, 2, 320.0f, 0.3f, 85.7f);
	rotationAddMovabilityToRotation(70400, 70000, "1386", Common::Rect(1200, -127, 1508, 143), true, 53, 7);
	rotationSetMovabilityToRotation(70400, 0, 0, 0, 85.7f, 0, 2, 90.0f, 0, 85.7f);
	rotationAddMovabilityToPuzzle(70400, kPuzzle70410, "", Common::Rect(3011, -211, 3280, 29), true, 53, 0);
	rotationSetMovabilityToPuzzle(70400, 1, 0, 0, 85.7f, 0, 2);
	rotationAddMovabilityToRotation(70500, 70600, "1387", Common::Rect(546, -345, 980, 120), true, 53, 0);
	rotationSetMovabilityToRotation(70500, 0, 0, 0.3f, 85.7f, 0, 2, 44.0f, 0.3f, 85.7f);
	rotationAddMovabilityToPuzzle(70500, kPuzzle70500, "", Common::Rect(1139, -204, 1520, 156), true, 52, 0);
	rotationSetMovabilityToPuzzle(70500, 1, 0, 0.3f, 85.7f, 0, 2);
	rotationAddMovabilityToPuzzle(70500, kPuzzle70510, "", Common::Rect(1787, -143, 2033, 108), true, 52, 0);
	rotationSetMovabilityToPuzzle(70500, 2, 0, 0.3f, 85.7f, 0, 2);
	rotationAddMovabilityToPuzzle(70500, kPuzzle70520, "", Common::Rect(2228, -118, 2411, 80), true, 52, 0);
	rotationSetMovabilityToPuzzle(70500, 3, 0, 0.3f, 85.7f, 0, 2);
	rotationAddMovabilityToRotation(70600, 70101, "1388", Common::Rect(863, -211, 1123, 101), true, 53, 0);
	rotationSetMovabilityToRotation(70600, 0, 0, 0.3f, 85.7f, 0, 2, 270.0f, 0.3f, 85.7f);
	puzzleAddMovabilityToPuzzle(kPuzzle70102, kPuzzle70100, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleAddMovabilityToPuzzle(kPuzzle70410, kPuzzle70411, "", Common::Rect(250, 280, 320, 370), true, 52, 0);
	puzzleAddMovabilityToRotation(kPuzzle70410, 70400, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle70410, 1, 270.0f, 0.3f, 85.7f);
	puzzleAddMovabilityToPuzzle(kPuzzle70411, kPuzzle70410, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleAddMovabilityToRotation(kPuzzle70500, 70500, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle70500, 0, 90.0f, 0.3f, 85.7f);
	puzzleAddMovabilityToPuzzle(kPuzzle70500, kPuzzle70501, "", Common::Rect(180, 80, 470, 370), true, 52, 0);
	puzzleAddMovabilityToPuzzle(kPuzzle70501, kPuzzle70500, "", Common::Rect(430, 48, 640, 434), true, 52, 0);
	puzzleAddMovabilityToRotation(kPuzzle70510, 70500, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle70510, 0, 144.0f, 0.3f, 85.7f);
	puzzleAddMovabilityToPuzzle(kPuzzle70510, kPuzzle70511, "", Common::Rect(180, 80, 470, 370), true, 52, 0);
	puzzleAddMovabilityToPuzzle(kPuzzle70511, kPuzzle70510, "", Common::Rect(430, 48, 640, 434), true, 52, 0);
	puzzleAddMovabilityToRotation(kPuzzle70520, 70500, "", Common::Rect(0, 420, 640, 464), true, 55, 0);
	puzzleSetMovabilityToRotation(kPuzzle70520, 0, 184.0f, 0.3f, 85.7f);
	puzzleAddMovabilityToPuzzle(kPuzzle70520, kPuzzle70521, "", Common::Rect(180, 80, 470, 370), true, 52, 0);
	puzzleAddMovabilityToPuzzle(kPuzzle70521, kPuzzle70520, "", Common::Rect(430, 48, 640, 434), true, 52, 0);
	objectAdd(kObject72000, "", "", 1);
	objectAddPresentation(kObject72000);
	objectPresentationAddAnimationToPuzzle(kObject72000, 0, kPuzzle72001, "TR_AS_RO_BS", 0, Common::Point(375, 203), 1, 1000, 20, 12.5f, 4);
	objectAddPresentation(kObject72000);
	objectPresentationAddAnimationToPuzzle(kObject72000, 1, kPuzzle72002, "TR_AS_RO_CS", 0, Common::Point(281, 154), 1, 1000, 20, 12.5f, 4);
	objectAddPresentation(kObject72000);
	objectPresentationAddAnimationToPuzzle(kObject72000, 2, kPuzzle72003, "TR_AS_RO_DS", 0, Common::Point(284, 163), 1, 1000, 20, 12.5f, 4);
	objectAddPresentation(kObject72000);
	objectPresentationAddAnimationToPuzzle(kObject72000, 3, kPuzzle72004, "TR_AS_RO_ES", 0, Common::Point(147, 140), 1, 1000, 23, 12.5f, 4);
	objectAddPresentation(kObject72000);
	objectPresentationAddAnimationToPuzzle(kObject72000, 4, kPuzzle72005, "TR_AS_RO_FS", 0, Common::Point(388, 244), 1, 1000, 20, 12.5f, 4);
	objectAddPresentation(kObject72000);
	objectPresentationAddAnimationToPuzzle(kObject72000, 5, kPuzzle72006, "TR_AS_RO_HS", 0, Common::Point(146, 156), 1, 1000, 20, 12.5f, 4);
	objectAddPresentation(kObject72000);
	objectPresentationAddAnimationToPuzzle(kObject72000, 6, kPuzzle72007, "TR_AS_RO_IS", 0, Common::Point(308, 221), 1, 1000, 20, 12.5f, 4);
	objectAddPresentation(kObject72000);
	objectPresentationAddAnimationToPuzzle(kObject72000, 7, kPuzzle72008, "TR_AS_RO_JS", 0, Common::Point(250, 102), 1, 1000, 20, 12.5f, 4);
	objectAddPresentation(kObject72000);
	objectPresentationAddAnimationToPuzzle(kObject72000, 8, kPuzzle72009, "TR_AS_RO_KS", 0, Common::Point(224, 262), 1, 1000, 20, 12.5f, 4);
	objectAddPresentation(kObject72000);
	objectPresentationAddAnimationToPuzzle(kObject72000, 9, kPuzzle72010, "TR_AS_RO_MS", 0, Common::Point(173, 127), 1, 1000, 20, 12.5f, 4);
	objectAdd(kObject70001, "Alberich", "", 1);
	objectAddPresentation(kObject70001);
	objectPresentationAddAnimationToPuzzle(kObject70001, 0, kPuzzle70000, "N2EXTRA_1B", 0, Common::Point(1, 153), 1, 1000, 53, 10.0f, 4);
	objectAddPresentation(kObject70001);
	objectPresentationAddAnimationToPuzzle(kObject70001, 1, kPuzzle70000, "N2EXTRA_2B", 0, Common::Point(0, 16), 1, 1000, 37, 10.0f, 4);
	objectAddPresentation(kObject70001);
	objectPresentationAddAnimationToPuzzle(kObject70001, 2, kPuzzle70000, "N2EXTRA_3B", 0, Common::Point(49, 38), 1, 1000, 53, 10.0f, 4);
	objectAddPresentation(kObject70001);
	objectPresentationAddAnimationToRotation(kObject70001, 3, 70001, 2, 16, 12.5f, 4);
	objectPresentationSetAnimationOnRotation(kObject70001, 3, 0, kObjectFire);
	objectPresentationShow(kObject70001, 3);
	objectAdd(kObjectFire, "Fire", "N2_Fire", 1);
	objectAddBagAnimation(kObjectFire, 1, 3, 13, 12.5f, 4);
	objectSetActiveCursor(kObjectFire, 22, 22, 13, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectFire, 22, 22, 0, 3, 0, 0, 3);
	objectAddRotationAccessibility(kObjectFire, 70600, Common::Rect(2413, -256, 2970, 256), true, 52, 0);
	objectAddPresentation(kObjectFire);
	objectPresentationAddAnimationToRotation(kObjectFire, 0, 70000, 0, 25, 10.0f, 4);
	objectPresentationAddAnimationToRotation(kObjectFire, 0, 70001, 0, 25, 10.0f, 4);
	objectPresentationAddAnimationToRotation(kObjectFire, 0, 70001, 1, 25, 10.0f, 4);
	objectPresentationShow(kObjectFire, 0);
	objectAdd(kObject70100, "Console", "", 1);
	objectAddPuzzleAccessibility(kObject70100, kPuzzle70100, Common::Rect(  0,  16, 640,  66), true, 53, 0);
	objectAddPuzzleAccessibility(kObject70100, kPuzzle70100, Common::Rect(  0, 414, 640, 464), true, 53, 0);
	objectAddPuzzleAccessibility(kObject70100, kPuzzle70100, Common::Rect(  0,  66,  50, 414), true, 53, 0);
	objectAddPuzzleAccessibility(kObject70100, kPuzzle70100, Common::Rect(610,  66, 640, 414), true, 53, 0);
	objectAddRotationAccessibility(kObject70100, 70100, Common::Rect(3406, 26, 3541, 155), true, 52, 1);
	objectAddPresentation(kObject70100);
	objectPresentationAddImageToPuzzle(kObject70100, 0, kPuzzle70100, "N2S01N01P01L01.0001.bmp", Common::Point(101, 68), true, 1, 1000);
	objectAddPresentation(kObject70100);
	objectPresentationAddImageToPuzzle(kObject70100, 1, kPuzzle70100, "N2S01N01P01L02.0001.bmp", Common::Point(80, 63), true, 1, 1000);
	objectAddPresentation(kObject70100);
	objectPresentationAddImageToPuzzle(kObject70100, 2, kPuzzle70100, "N2S01N01P01L03.0001.bmp", Common::Point(116, 159), true, 1, 1000);
	objectAdd(kObject70101, "CCButton1", "", 2);
	objectAddRotationAccessibility(kObject70101, 70100, Common::Rect(3208, 109, 3322, 184), true, 52, 1);
	objectAddPuzzleAccessibility(kObject70101, kPuzzle70100, Common::Rect(359, 265, 440, 304), false, 52, 0);
	objectAddPuzzleAccessibility(kObject70101, kPuzzle70100, Common::Rect(359, 265, 440, 304), false, 52, 1);
	objectAddPresentation(kObject70101);
	objectPresentationAddImageToPuzzle(kObject70101, 0, kPuzzle70100, "N2S01N01P01S03.0001.bmp", Common::Point(361, 259), true, 1, 1000);
	objectAddPresentation(kObject70101);
	objectPresentationAddAnimationToPuzzle(kObject70101, 1, kPuzzle70100, "N2S01N01P01S01", 0, Common::Point(85, 68), 1, 1000, 36, 12.5f, 6);
	objectPresentationSetAnimationOnPuzzle(kObject70101, 1, 0, kObject70101);
	objectAddPresentation(kObject70101);
	objectPresentationAddAnimationToPuzzle(kObject70101, 2, kPuzzle70100, "N2S01N01P01L02S01", 0, Common::Point(80, 63), 1, 1000, 36, 12.5f, 10);
	objectPresentationSetAnimationOnPuzzle(kObject70101, 2, 0, kObject70103);
	objectAddPresentation(kObject70101);
	objectPresentationAddAnimationToPuzzle(kObject70101, 3, kPuzzle70100, "N2S01N01P01S01", 0, Common::Point(85, 68), 1, 1000, 36, 12.5f, 10);
	objectPresentationSetAnimationOnPuzzle(kObject70101, 3, 0, kObject70100);
	objectAddPresentation(kObject70101);
	objectPresentationAddAnimationToPuzzle(kObject70101, 4, kPuzzle70100, "N2S01N01P01L02S01", 0, Common::Point(80, 63), 1, 1000, 36, 12.5f, 6);
	objectPresentationSetAnimationOnPuzzle(kObject70101, 4, 0, kObject70102);
	objectAddPresentation(kObject70101);
	objectPresentationAddImageToPuzzle(kObject70101, 5, kPuzzle70100, "N2S01N01P01S03.0003.bmp", Common::Point(361, 259), true, 1, 1000);
	objectAddPresentation(kObject70101);
	objectPresentationAddImageToRotation(kObject70101, 6, 70100, 0);
	objectPresentationAddImageToRotation(kObject70101, 6, 70101, 0);
	objectPresentationShow(kObject70101, 6);
	objectAdd(kObject70102, "CCButton2", "", 2);
	objectAddRotationAccessibility(kObject70102, 70100, Common::Rect(3250, 195, 3357, 245), true, 52, 1);
	objectAddPuzzleAccessibility(kObject70102, kPuzzle70100, Common::Rect(382, 308, 472, 352), true, 52, 0);
	objectAddPuzzleAccessibility(kObject70102, kPuzzle70100, Common::Rect(382, 308, 472, 352), true, 52, 1);
	objectAddPresentation(kObject70102);
	objectPresentationAddImageToPuzzle(kObject70102, 0, kPuzzle70100, "N2S01N01P01S03.0004.bmp", Common::Point(385, 299), true, 1, 1000);
	objectAddPresentation(kObject70102);
	objectPresentationAddAnimationToPuzzle(kObject70102, 1, kPuzzle70100, "N2S01N01P01S02", 0, Common::Point(40, 65), 1, 1000, 46, 12.5f, 6);
	objectPresentationSetAnimationOnPuzzle(kObject70102, 1, 0, kObject70105);
	objectAddPresentation(kObject70102);
	objectPresentationAddAnimationToPuzzle(kObject70102, 2, kPuzzle70100, "N2S01N01P01S02", 0, Common::Point(40, 65), 1, 1000, 23, 12.5f, 6);
	objectPresentationSetAnimationOnPuzzle(kObject70102, 2, 0, kObject70104);
	objectAddPresentation(kObject70102);
	objectPresentationAddAnimationToPuzzle(kObject70102, 3, kPuzzle70100, "N2S01N01P01S02", 0, Common::Point(40, 65), 1, 1000, 23, 12.5f, 10);
	objectPresentationSetAnimationOnPuzzle(kObject70102, 3, 0, kObject70106);
	objectAddPresentation(kObject70102);
	objectPresentationAddImageToPuzzle(kObject70102, 4, kPuzzle70100, "N2S01N01P01S03.0002.bmp", Common::Point(385, 299), true, 1, 1000);
	objectAdd(kObject70103, "CCHandle", "NI_HandSel", 4);
	objectAddRotationAccessibility(kObject70103, 70100, Common::Rect(3406, 26, 3541, 155), true, 52, 0);
	objectAddPuzzleAccessibility(kObject70103, kPuzzle70100, Common::Rect(497, 192, 543, 249), true, 10000, 1);
	objectSetActiveDrawCursor(kObject70103, 5, 5, 0, 3, 0, 0, 3);
	objectSetPassiveDrawCursor(kObject70103, 5, 5, 0, 3, 0, 0, 3);

	for (uint32 i = 0; i < 14; i++) {
		objectAddPresentation(kObject70103);
		objectPresentationAddImageToPuzzle(kObject70103, i, kPuzzle70100, Common::String::format("N2S01N01P02S01.%04d.bmp", i + 1), Common::Point(504, 194), true, 1, 1000);
	}

	objectAdd(kObject70105, "CCHolo", "", 1);
	objectAddPuzzleAccessibility(kObject70105, kPuzzle70100, Common::Rect( 77, 132, 340, 298), false, 52, 0);
	objectAddPuzzleAccessibility(kObject70105, kPuzzle70100, Common::Rect(140, 170, 430, 380), false, 52, 0);
	objectAdd(kObject70106, "CCHoloCross", "NI_HandSel", 4);
	objectAddPuzzleAccessibility(kObject70106, kPuzzle70102, Common::Rect(203, 236, 283, 316), true, 10000, 0);
	objectSetActiveDrawCursor(kObject70106, 5, 5, 0, 3, 0, 0, 3);
	objectSetPassiveDrawCursor(kObject70106, 5, 5, 0, 3, 0, 0, 3);

	for (uint32 i = 0; i < 20; i++) {
		objectAddPresentation(kObject70106);
		objectPresentationAddImageToPuzzle(kObject70106, i, kPuzzle70102, Common::String::format("N2S01N01P03L01S01.%04d.bmp", i + 1), Common::Point(213, 254), true, 1, 1000);
	}

	objectAdd(kObject70107, "CCHoloDam1", "NI_HandSel", 4);
	objectAdd(kObject70108, "CCHoloDam2", "NI_HandSel", 4);
	objectAddPuzzleAccessibility(kObject70108, kPuzzle70102, Common::Rect(389, 270, 434, 330), true,  10000, 0);
	objectAddPuzzleAccessibility(kObject70108, kPuzzle70102, Common::Rect(389, 330, 434, 390), false, 10000, 1);
	objectSetActiveDrawCursor(kObject70108, 5, 5, 0, 3, 0, 0, 3);
	objectSetPassiveDrawCursor(kObject70108, 5, 5, 0, 3, 0, 0, 3);

	for (uint32 i = 0; i < 15; i++) {
		objectAddPresentation(kObject70108);
		objectPresentationAddImageToPuzzle(kObject70108, i, kPuzzle70102, Common::String::format("N2S01N01P03L01S03.%04d.bmp", i + 1), Common::Point(390, 271), true, 1, 999);
	}

	objectAddPresentation(kObject70108);
	objectPresentationAddImageToPuzzle(kObject70108, 14, kPuzzle70102, "N2S01N01P03L01S03A.bmp", Common::Point(340, 237), true, 1, 1000);
	objectPresentationAddImageToPuzzle(kObject70108, 14, kPuzzle70102, "N2S01N01P03L01S03B.bmp", Common::Point(311, 179), true, 1, 1000);
	objectAdd(kObjectLogeTear3, "Loge's Tear", "NI_Tear", 1);
	objectAddBagAnimation(kObjectLogeTear3, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectLogeTear3, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectLogeTear3, 22, 22, 0, 3, 0, 0, 3);
	objectAdd(kObject70300, "Mime", "", 1);
	objectAddRotationAccessibility(kObject70300, 70300, Common::Rect(1937, -26, 2079, 88), true, 52, 0);
	objectAddPresentation(kObject70300);
	objectPresentationAddAnimationToRotation(kObject70300, 0, 70300, 0, 25, 10.0f, 4);
	objectPresentationSetAnimationOnRotation(kObject70300, 0, 0, kObject70300);
	objectPresentationShow(kObject70300, 0);
	objectAddPresentation(kObject70300);
	objectPresentationAddAnimationToPuzzle(kObject70300, 1, kPuzzle70300, "N2S03N01P01S01", 0, Common::Point(260, 73), 1, 1000, 20, 12.5f, 4);
	objectAddPresentation(kObject70300);
	objectPresentationAddAnimationToPuzzle(kObject70300, 2, kPuzzle70301, "N2S03N01P01S02", 0, Common::Point(289, 79), 1, 1000, 20, 12.5f, 4);
	objectAddPresentation(kObject70300);
	objectPresentationAddAnimationToPuzzle(kObject70300, 3, kPuzzle70302, "N2S03N01P01S04", 0, Common::Point(305, 117), 1, 1000, 20, 12.5f, 4);
	objectAddPresentation(kObject70300);
	objectPresentationAddAnimationToPuzzle(kObject70300, 4, kPuzzle70303, "N2S02N01P02S02", 0, Common::Point(311, 166), 1, 1000, 20, 12.5f, 4);
	objectAddPresentation(kObject70300);
	objectPresentationAddAnimationToPuzzle(kObject70300, 5, kPuzzle70304, "N2S02N01P02S03", 0, Common::Point(97, 185), 1, 1000, 20, 12.5f, 4);
	objectAddPresentation(kObject70300);
	objectAddPresentation(kObject70300);
	objectPresentationAddAnimationToPuzzle(kObject70300, 7, kPuzzle70306, "N2S03N01P01S03", 0, Common::Point(270, 135), 1, 1000, 20, 12.5f, 4);
	objectAdd(kObjectCage, "Cage", "N2_Cage", 1);
	objectAddBagAnimation(kObjectCage, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectCage, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectCage, 22, 22, 0, 3, 0, 0, 3);
	objectAddPresentation(kObjectCage);
	objectPresentationAddImageToRotation(kObjectCage, 0, 70300, 1);
	objectAdd(kObject70404, "TearCasing", "", 3);
	objectAddPuzzleAccessibility(kObject70404, kPuzzle70411, Common::Rect(189, 189, 362, 377), true,  52, 0);
	objectAddPuzzleAccessibility(kObject70404, kPuzzle70411, Common::Rect(224,  90, 362, 209), false, 52, 1);
	objectAddPuzzleAccessibility(kObject70404, kPuzzle70411, Common::Rect(293, 241, 475, 462), false, 52, 2);
	objectAddPresentation(kObject70404);
	objectPresentationAddImageToPuzzle(kObject70404, 0, kPuzzle70411, "N2S04N03P03L01.0001.bmp", Common::Point(214, 173), true, 1, 1000);
	objectAddPresentation(kObject70404);
	objectPresentationAddImageToPuzzle(kObject70404, 1, kPuzzle70411, "N2S04N03P03L02.0001.bmp", Common::Point(270, 116), true, 1, 1000);
	objectAddPresentation(kObject70404);
	objectPresentationAddAnimationToPuzzle(kObject70404, 2, kPuzzle70411, "N2S04N03P03L01S01", 0, Common::Point(217, 171), 1, 1000, 27, 12.5f, 10);
	objectPresentationSetAnimationOnPuzzle(kObject70404, 2, 0, kObject70422);
	objectAddPresentation(kObject70404);
	objectPresentationAddAnimationToPuzzle(kObject70404, 3, kPuzzle70411, "N2S04N03P03L01S01", 0, Common::Point(217, 171), 1, 1000, 27, 12.5f, 6);
	objectPresentationSetAnimationOnPuzzle(kObject70404, 3, 0, kObject70423);
	objectAddPresentation(kObject70404);
	objectPresentationAddAnimationToPuzzle(kObject70404, 4, kPuzzle70411, "N2S04N03P03L02S01", 0, Common::Point(215, 111), 1, 1000, 26, 12.5f, 10);
	objectPresentationSetAnimationOnPuzzle(kObject70404, 4, 0, kObject70424);
	objectAddPresentation(kObject70404);
	objectPresentationAddAnimationToPuzzle(kObject70404, 5, kPuzzle70411, "N2S04N03P03L02S01", 0, Common::Point(215, 111), 1, 1000, 26, 12.5f, 6);
	objectPresentationSetAnimationOnPuzzle(kObject70404, 5, 0, kObject70425);
	objectAddPresentation(kObject70404);
	objectPresentationAddAnimationToPuzzle(kObject70404, 6, kPuzzle70411, "N2S04N03P03L02S02", 0, Common::Point(275, 126), 1, 1000, 20, 12.5f, 4);
	objectAdd(kObject70406, "", "", 1);
	objectAddPresentation(kObject70406);
	objectPresentationAddAnimationToPuzzle(kObject70406, 0, kPuzzle70410, "N2S04N03P01S02", 0, Common::Point(171, 285), 1, 1000, 13, 10.0f, 32);
	objectPresentationAddAnimationToPuzzle(kObject70406, 0, kPuzzle70410, "N2S04N03P01S03", 0, Common::Point(244, 268), 1, 1000, 12, 10.0f, 32);
	objectPresentationAddAnimationToPuzzle(kObject70406, 0, kPuzzle70410, "N2S04N03P01S04", 0, Common::Point(317, 261), 1, 1000, 7, 10.0f, 32);
	objectPresentationShow(kObject70406, 0);
	objectAdd(kObject70405, "Temperat", "", 1);
	objectAddPresentation(kObject70405);

	for (uint32 i = 0; i < 13; i++) {
		objectAddPresentation(kObject70405);
		objectPresentationAddImageToPuzzle(kObject70405, i, kPuzzle70410, Common::String::format("N2S04N03P01S01.%04d.bmp", i + 1), Common::Point(107, 155), true, 1, 1000);
	}

	objectAdd(kObjectCentaur, "Centaur", "N2_Centaur", 1);
	objectAddBagAnimation(kObjectCentaur, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectCentaur, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectCentaur, 22, 22, 0, 3, 0, 0, 3);
	objectAddPuzzleAccessibility(kObjectCentaur, kPuzzle70501, Common::Rect(270, 250, 364, 363), true, 52, 0);
	objectAddPuzzleAccessibility(kObjectCentaur, kPuzzle70000, Common::Rect(320,  16, 640, 464), true, 52, 1);
	objectAddPresentation(kObjectCentaur);
	objectPresentationAddImageToPuzzle(kObjectCentaur, 0, kPuzzle70501, "N2S05N01P01L02.0001.bmp", Common::Point(278, 258), true, 1, 1000);
	objectPresentationShow(kObjectCentaur);
	objectAdd(kObjectDragon, "Dragon", "N2_Dragon", 1);
	objectAddBagAnimation(kObjectDragon, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectDragon, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectDragon, 22, 22, 0, 3, 0, 0, 3);
	objectAddPuzzleAccessibility(kObjectDragon, kPuzzle70511, Common::Rect(290, 260, 380, 371), true, 52, 0);
	objectAddPuzzleAccessibility(kObjectDragon, kPuzzle70000, Common::Rect(320,  16, 640, 464), true, 52, 1);
	objectAddPresentation(kObjectDragon);
	objectPresentationAddImageToPuzzle(kObjectDragon, 0, kPuzzle70511, "N2S05N01P02L02.0001.bmp", Common::Point(291, 263), true, 1, 1000);
	objectPresentationShow(kObjectDragon);
	objectAdd(kObjectPhoenix1, "Phoenix", "N2_Phoenix", 1);
	objectAddBagAnimation(kObjectPhoenix1, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectPhoenix1, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectPhoenix1, 22, 22, 0, 3, 0, 0, 3);
	objectAddPuzzleAccessibility(kObjectPhoenix1, kPuzzle70521, Common::Rect(265, 255, 356, 358), true, 52, 0);
	objectAddPuzzleAccessibility(kObjectPhoenix1, kPuzzle70000, Common::Rect(320,  16, 640, 464), true, 52, 1);
	objectAddPresentation(kObjectPhoenix1);
	objectPresentationAddImageToPuzzle(kObjectPhoenix1, 0, kPuzzle70521, "N2S05N01P03L02.0001.bmp", Common::Point(268, 255), true, 1, 1000);
	objectPresentationShow(kObjectPhoenix1);
	objectAdd(kObjectChrysoberyl, "Chrysoberyl", "N2_Beryllium", 1);
	objectAddBagAnimation(kObjectChrysoberyl, 1, 3, 20, 12.5f, 4);
	objectSetActiveCursor(kObjectChrysoberyl, 22, 22, 20, 4, 12.5f, 4, 4);
	objectSetPassiveCursor(kObjectChrysoberyl, 22, 22, 0, 3, 0, 0, 3);
	objectAddRotationAccessibility(kObjectChrysoberyl, 70500, Common::Rect(3483, -22, 3600, 155), true, 52, 0);
	objectAddRotationAccessibility(kObjectChrysoberyl, 70500, Common::Rect(0, -22, 103, 155), true, 52, 0);
	objectAddPresentation(kObjectChrysoberyl);
	objectPresentationAddImageToRotation(kObjectChrysoberyl, 0, 70500, 0);
	objectPresentationAddImageToRotation(kObjectChrysoberyl, 0, 70500, 1);
	objectAdd(kObject70700, "Nib", "", 1);
	objectAddRotationAccessibility(kObject70700, 70200, Common::Rect(3188, -62, 3331, 123), true, 52, 0);
	objectAddRotationAccessibility(kObject70700, 70200, Common::Rect(3188, -62, 3331, 123), false, 52, 1);
	objectAddPresentation(kObject70700);
	objectPresentationAddAnimationToRotation(kObject70700, 0, 70200, 0, 30, 12.5f, 4);
	objectAddPresentation(kObject70700);
	objectPresentationAddAnimationToPuzzle(kObject70700, 1, kPuzzle70600, "N2S07N01P01S01", 0, Common::Point(93, 16), 1, 1000, 20, 12.5f, 4);
	objectAddPresentation(kObject70700);
	objectPresentationAddAnimationToPuzzle(kObject70700, 2, kPuzzle70601, "N2S07N01P01S04", 0, Common::Point(286, 160), 1, 1000, 20, 12.5f, 4);
	objectPresentationShow(kObject70700, 0);
	soundAdd(70000, kSoundTypeBackgroundMusic, "1392.was", _configuration.backgroundMusic.loadFrom, 2, _configuration.backgroundMusic.soundChunck);
	soundAdd(70101, kSoundTypeAmbientEffect, "1394.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(70300, kSoundTypeAmbientEffect, "1395.was", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(70301, kSoundTypeAmbientEffect, "1396.wav", _configuration.ambientEffect.loadFrom);
	soundAdd(70412, kSoundTypeAmbientEffect, "1397.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(70501, kSoundTypeAmbientEffect, "1399.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(70502, kSoundTypeAmbientEffect, "1400.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(70102, kSoundTypeAmbientEffect, "1401.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(70103, kSoundTypeAmbientEffect, "1402.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(70104, kSoundTypeAmbientEffect, "1403.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(70105, kSoundTypeAmbientEffect, "1404.wav", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(70107, kSoundTypeAmbientEffect, "1405.was", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(70701, kSoundTypeAmbientEffect, "1406.was", _configuration.ambientEffect.loadFrom, 2, _configuration.ambientEffect.soundChunck);
	soundAdd(70405, kSoundTypeEffect, "1412.wav", _configuration.effect.loadFrom, 2, _configuration.effect.soundChunck);
	soundAdd(70406, kSoundTypeEffect, "1413.wav", _configuration.effect.loadFrom, 2, _configuration.effect.soundChunck);
	soundAdd(70407, kSoundTypeEffect, "1414.wav", _configuration.effect.loadFrom, 2, _configuration.effect.soundChunck);
	soundAdd(70408, kSoundTypeEffect, "1415.wav", _configuration.effect.loadFrom, 2, _configuration.effect.soundChunck);
	soundAdd(72001, kSoundTypeDialog, "1417.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(72002, kSoundTypeDialog, "1418.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(72003, kSoundTypeDialog, "1419.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(72004, kSoundTypeDialog, "1420.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(72005, kSoundTypeDialog, "1421.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(72006, kSoundTypeDialog, "1422.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(72007, kSoundTypeDialog, "1423.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(72008, kSoundTypeDialog, "1424.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(72009, kSoundTypeDialog, "1425.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(72010, kSoundTypeDialog, "1426.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70001, kSoundTypeDialog, "1427.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70002, kSoundTypeDialog, "1428.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70003, kSoundTypeDialog, "1429.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70004, kSoundTypeDialog, "1430.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70005, kSoundTypeDialog, "1431.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70006, kSoundTypeDialog, "1432.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70007, kSoundTypeDialog, "1433.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70008, kSoundTypeDialog, "1434.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70009, kSoundTypeDialog, "1435.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70010, kSoundTypeDialog, "1436.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70011, kSoundTypeDialog, "1437.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70012, kSoundTypeDialog, "1438.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70013, kSoundTypeDialog, "1439.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70014, kSoundTypeDialog, "1440.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70015, kSoundTypeDialog, "1441.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70016, kSoundTypeDialog, "1442.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70017, kSoundTypeDialog, "1443.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70018, kSoundTypeDialog, "1444.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70019, kSoundTypeDialog, "1445.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70020, kSoundTypeDialog, "1446.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70021, kSoundTypeDialog, "1447.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70022, kSoundTypeDialog, "1448.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70023, kSoundTypeDialog, "1449.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70024, kSoundTypeDialog, "1450.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70025, kSoundTypeDialog, "1451.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70026, kSoundTypeDialog, "1452.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70027, kSoundTypeDialog, "1453.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(71001, kSoundTypeDialog, "1454.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(71010, kSoundTypeDialog, "1455.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(71002, kSoundTypeDialog, "1456.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(71011, kSoundTypeDialog, "1457.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(71003, kSoundTypeDialog, "1458.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(71012, kSoundTypeDialog, "1459.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(71100, kSoundTypeDialog, "1460.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(71101, kSoundTypeDialog, "1461.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(71102, kSoundTypeDialog, "1462.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(71111, kSoundTypeDialog, "1463.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(71110, kSoundTypeDialog, "1464.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(71112, kSoundTypeDialog, "1465.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(71122, kSoundTypeDialog, "1466.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(71120, kSoundTypeDialog, "1467.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(71121, kSoundTypeDialog, "1468.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70043, kSoundTypeDialog, "1469.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70044, kSoundTypeDialog, "1470.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70045, kSoundTypeDialog, "1471.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70046, kSoundTypeDialog, "1472.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70047, kSoundTypeDialog, "1473.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70048, kSoundTypeDialog, "1474.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70049, kSoundTypeDialog, "1475.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70050, kSoundTypeDialog, "1476.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70051, kSoundTypeDialog, "1477.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70052, kSoundTypeDialog, "1478.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70053, kSoundTypeDialog, "1479.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70054, kSoundTypeDialog, "1480.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70055, kSoundTypeDialog, "1481.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70056, kSoundTypeDialog, "1482.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70057, kSoundTypeDialog, "1483.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70058, kSoundTypeDialog, "1484.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70059, kSoundTypeDialog, "1485.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70060, kSoundTypeDialog, "1486.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70061, kSoundTypeDialog, "1487.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70062, kSoundTypeDialog, "1488.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70063, kSoundTypeDialog, "1489.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70064, kSoundTypeDialog, "1490.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70065, kSoundTypeDialog, "1491.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	soundAdd(70066, kSoundTypeDialog, "1492.wac", _configuration.dialog.loadFrom, 2, _configuration.dialog.soundChunck);
	rotationAdd3DSound(70300, 70301, 1, 1, 20, 100, 160.0f, 20);
	rotationSet3DSoundOff(70300, 70301);
	rotationAdd3DSound(70300, 70300, 1, 1, 20, 100, 160.0f, 5);
	rotationAdd3DSound(70400, 70412, 1, 1, 20, 90, 270.0f, 10);
	puzzleAdd3DSound(kPuzzle70410, 70412, 1, 1, 2, 93, 300.0f, 5);
	puzzleAdd3DSound(kPuzzle70411, 70412, 1, 1, 2, 95, 300.0f, 5);
	rotationAdd3DSound(70000, 70107, 1, 1, 50, 100, 135.0f, 8);
	rotationAdd3DSound(70001, 70107, 1, 1, 50, 100, 315.0f, 8);
	rotationAdd3DSound(70400, 70107, 1, 1, 10, 93, 90.0f, 20);
	rotationAdd3DSound(70100, 70107, 1, 1, 10, 93, 270.0f, 5);
	rotationAdd3DSound(70101, 70107, 1, 1, 10, 88, 270.0f, 5);
	puzzleAdd3DSound(kPuzzle70100, 70107, 1, 1, 2, 93, 27.0f, 5);
	puzzleAdd3DSound(kPuzzle70102, 70107, 1, 1, 2, 93, 27.0f, 5);
	rotationAdd3DSound(70200, 70701, 1, 1, 20, 100, 285.0f, 5);
	varDefineByte(70001, 1);
	varDefineByte(70005, 1);
	varDefineByte(70010, 0);
	varDefineByte(70011, 0);
	varDefineByte(70012, 0);
	varDefineByte(70013, 0);
	varDefineByte(70014, 1);
	varDefineByte(70015, 97);
	varDefineWord(70016, 0);
	varDefineString(70099, "3231323131 76766     34        020212121212        776000    ");
}

#pragma endregion

} // End of namespace Ring
