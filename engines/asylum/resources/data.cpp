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
 */

#include "engines/asylum/resources/data.h"

#include "common/rect.h"
#include "common/serializer.h"
#include "common/textconsole.h"

namespace Asylum {

SharedData::SharedData() {
	// Public data
	cdNumber                           = 0;
	movieIndex                         = 0;
	sceneCounter                       = 0;
	actorEnableForStatus7              = false;
	globalDirection                    = kDirectionN;


	memset(&_ambientFlags, 0, sizeof(_ambientFlags));
	memset(&_ambientTicks, 0, sizeof(_ambientTicks));
	_globalPoint.x                     = -1;
	_globalPoint.y                     = -1;
	// _flagSkipScriptProcessing
	// _flagEncounterRunning
	// player ActorIndex
	_sceneOffset                       = 0;
	_sceneOffsetAdd                    = 0;
	memset(&_cursorResources, kResourceNone, sizeof(_cursorResources));
	memset(&_sceneFonts, kResourceNone, sizeof(_sceneFonts));
	memset(&_chapter2Data1, 0, sizeof(_chapter2Data1));
	_smallCurUp                        = 0;
	_smallCurDown                      = 0;
	_encounterFrameBg                  = 0;
	_flagSkipDrawScene                 = false;
	_matteVar1                         = 0;
	_flagActorUpdateEnabledCheck       = false;
	_matteInitialized                  = false;
	_mattePlaySound                    = false;
	_currentScreenUpdatesCount         = 0;
	memset(&_chapter2Data2, 0, sizeof(_chapter2Data2));
	memset(&_chapter2Counters, 0, sizeof(_chapter2Counters));
	memset(&_chapter2Data3, 0, sizeof(_chapter2Data3));
	_chapter2FrameIndexOffset          = 0;
	_chapter2ActorIndex                = 0;
	_eventUpdate                       = 0;
	memset(&_chapter2Data5, 0, sizeof(_chapter2Data4));
	_actorUpdateStatusEnabledCounter   = 0;
	memset(&_chapter2Data5, 0, sizeof(_chapter2Data5));
	// _flagEncounterDisablePlayerOnExit
	_flag1                             = false;
	_nextScreenUpdate                  = 0;
	// _moviesViewed;
	_flagActorUpdateStatus15Check      = false;

	// Non-saved data
	_flag2                             = false;
	_flag3                             = false;
	_flagScene1                        = false;
	_flagRedraw                        = false;

	_matteBarHeight                    = 0;
	_matteVar2                         = 0;
}

//////////////////////////////////////////////////////////////////////////
// Saved scene data
//////////////////////////////////////////////////////////////////////////
void SharedData::saveCursorResources(ResourceId *resources, uint32 size) {
	memcpy((ResourceId *)&_cursorResources, resources, size);
}

void SharedData::loadCursorResources(ResourceId *resources, uint32 size) {
	memcpy(resources, (ResourceId *)&_cursorResources, size);
}

void SharedData::saveSceneFonts(ResourceId font1, ResourceId font2, ResourceId font3) {
	_sceneFonts[0] = font1;
	_sceneFonts[1] = font2;
	_sceneFonts[2] = font3;
}

void SharedData::loadSceneFonts(ResourceId *font1, ResourceId *font2, ResourceId *font3) {
	*font1 = _sceneFonts[0];
	*font2 = _sceneFonts[1];
	*font3 = _sceneFonts[2];
}

void SharedData::saveSmallCursor(int32 smallCurUp, int32 smallCurDown) {
	_smallCurUp = smallCurUp;
	_smallCurDown = smallCurDown;
}

void SharedData::loadSmallCursor(int32 *smallCurUp, int32 *smallCurDown) {
	*smallCurUp = _smallCurUp;
	*smallCurDown = _smallCurDown;
}

//////////////////////////////////////////////////////////////////////////
// Chapter 2 data
//////////////////////////////////////////////////////////////////////////
void SharedData::resetChapter2Data() {
	for (uint32 i = 5; i < 14; i++)
		_chapter2Data3[i] = 0;

	for (uint32 i = 0; i < ARRAYSIZE(_chapter2Data2); i++)
		_chapter2Data2[i] = 160;

	for (uint32 i = 0; i < ARRAYSIZE(_chapter2Data5); i++)
		_chapter2Data5[i] = 0;
}

void SharedData::reset() {
	for (uint32 i = 0; i < ARRAYSIZE(_chapter2Data2); i++)
		_chapter2Data2[i] = 160;

	_chapter2FrameIndexOffset = 1;
}

void SharedData::setChapter2Data(uint32 index, int32 offset, int32 val) {
	error("[SharedData::setChapter2Data] Not implemented");
}

int32 SharedData::getChapter2Data(uint32 index, int32 offset) {
	error("[SharedData::getChapter2Data] Not implemented");
}

void SharedData::setChapter2Counter(uint32 index, int32 val) {
	if (index == 0 || index > 8)
		error("[SharedData::setChapter2Counter] Invalid index (was: %d, valid: [1;8])", index);

	_chapter2Counters[index - 1] = val;
}

int32 SharedData::getChapter2Counter(uint32 index) {
	if (index == 0 || index > 8)
		error("[SharedData::setChapter2Counter] Invalid index (was: %d, valid: [1;8])", index);

	return _chapter2Counters[index - 1];
}

//////////////////////////////////////////////////////////////////////////
// Ambient sounds
//////////////////////////////////////////////////////////////////////////
uint32 SharedData::getAmbientTick(uint32 index) {
	if (index >= ARRAYSIZE(_ambientTicks))
		error("[SharedData::getAmbientTick] index is outside valid values (was: %d, valid: [0:%d]", index, ARRAYSIZE(_ambientTicks));

	return _ambientTicks[index];
}

void SharedData::setAmbientTick(uint32 index, uint32 val) {
	if (index >= ARRAYSIZE(_ambientTicks))
		error("[SharedData::setAmbientTick] index is outside valid values (was: %d, valid: [0:%d]", index, ARRAYSIZE(_ambientTicks));

	_ambientTicks[index] = val;
}

uint32 SharedData::getAmbientFlag(uint32 index) {
	if (index >= ARRAYSIZE(_ambientFlags))
		error("[SharedData::getAmbientFlag] index is outside valid values (was: %d, valid: [0:%d]", index, ARRAYSIZE(_ambientFlags));

	return _ambientFlags[index];
}

void SharedData::setAmbientFlag(uint32 index, uint32 val) {
	if (index >= ARRAYSIZE(_ambientFlags))
		error("[SharedData::setAmbientFlag] index is outside valid values (was: %d, valid: [0:%d]", index, ARRAYSIZE(_ambientFlags));

	_ambientFlags[index] = val;
}

void SharedData::resetAmbientFlags() {
	memset(&_ambientFlags, 0, sizeof(_ambientFlags));
}

//////////////////////////////////////////////////////////////////////////
// Flags
//////////////////////////////////////////////////////////////////////////
bool SharedData::getFlag(GlobalFlag flag) const {
	switch (flag) {
	default:
		error("[SharedData::getFlag] Invalid flag type (%d)!", flag);

	case kFlag1:
		return _flag1;

	case kFlag2:
		return _flag2;

	case kFlag3:
		return _flag3;

	case kFlagRedraw:
		return _flagRedraw;
		break;

	case kFlagSkipDrawScene:
		return _flagSkipDrawScene;

	case kFlagSceneRectChanged:
		error("[SharedData::getFlag] Invalid flag type (kFlagSceneRectChanged)!");

	case kFlagScene1:
		return _flagScene1;

	//case kFlagSkipScriptProcessing:
	//	return _flagSkipScriptProcessing;

	//case kFlagEncounterRunning:
	//	return _flagEncounterRunning;

	case kFlagActorUpdateEnabledCheck:
		return _flagActorUpdateEnabledCheck;

	case kFlagActorUpdateStatus15Check:
		return _flagActorUpdateStatus15Check;
	}
}

void SharedData::setFlag(GlobalFlag flag, bool state) {
	switch (flag) {
	default:
		error("[SharedData::getFlag] Invalid flag type (%d)!", flag);

	case kFlag1:
		_flag1 = state;
		break;

	case kFlag2:
		_flag2 = state;
		break;

	case kFlag3:
		_flag3 = state;
		break;

	case kFlagRedraw:
		_flagRedraw = state;
		break;

	case kFlagSkipDrawScene:
		_flagSkipDrawScene = state;
		break;

	case kFlagSceneRectChanged:
		error("[SharedData::getFlag] Invalid flag type (kFlagSceneRectChanged)!");
		break;

	case kFlagScene1:
		_flagScene1 = state;
		break;

	//case kFlagSkipScriptProcessing:
	//	_flagSkipScriptProcessing = state;
	//	break;

	//case kFlagEncounterRunning:
	//	_flagEncounterRunning = state;
	//	break;

	case kFlagActorUpdateEnabledCheck:
		_flagActorUpdateEnabledCheck = state;
		break;

	case kFlagActorUpdateStatus15Check:
		_flagActorUpdateStatus15Check = state;
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
// Serializer
//////////////////////////////////////////////////////////////////////////
void SharedData::saveLoadAmbientSoundData(Common::Serializer &s) {
	// Ambient sound flags/ticks
	for (uint32 i = 0; i < ARRAYSIZE(_ambientFlags); i++)
		s.syncAsUint32LE(_ambientFlags[i]);

	for (uint32 i = 0; i < ARRAYSIZE(_ambientTicks); i++)
		s.syncAsUint32LE(_ambientTicks[i]);
}

void SharedData::saveLoadWithSerializer(Common::Serializer &s) {
	// Original skips two elements (original has one unused, one used for debugging screen update counts)
	s.skip(8);

	// Script queue (part of ScriptManager)

	// Global coordinates (original uses int32 for coordinates)
	s.syncAsSint32LE(_globalPoint.x);
	s.syncAsSint32LE(_globalPoint.y);

	// Processing skipped (part of ScriptManager)
	// Encounter running (part of Encounter)

	// Player index
	//s.syncAsUint32LE(playerIndex);

	// Scene coordinates
	s.syncAsSint32LE(_sceneCoords.x);
	s.syncAsSint32LE(_sceneCoords.y);
	s.syncAsSint32LE(_sceneOffset);
	s.syncAsSint32LE(_sceneOffsetAdd);

	// Original skips 4 bytes
	s.skip(4);

	// Cursor resources
	for (uint32 i = 0; i < ARRAYSIZE(_cursorResources); i++)
		s.syncAsUint32LE(_cursorResources[i]);

	// Fonts
	for (uint32 i = 0; i < ARRAYSIZE(_sceneFonts); i++)
		s.syncAsUint32LE(_sceneFonts[i]);

	// Chapter 2 actor data (Part 1)
	for (uint32 i = 0; i < ARRAYSIZE(_chapter2Data1); i++)
		s.syncAsUint32LE(_chapter2Data1[i]);

	// Scene information
	s.syncAsSint32LE(_smallCurUp);
	s.syncAsSint32LE(_smallCurDown);
	s.syncAsSint32LE(_encounterFrameBg);
	s.syncAsUint32LE(_flagSkipDrawScene);
	s.syncAsSint32LE(_matteVar1);
	s.syncAsUint32LE(_flagActorUpdateEnabledCheck);
	s.syncAsUint32LE(_matteInitialized);
	s.syncAsUint32LE(_mattePlaySound);
	s.syncAsSint32LE(_currentScreenUpdatesCount);

	// Chapter 2 actor data (Part 2)
	for (uint32 i = 0; i < ARRAYSIZE(_chapter2Data2); i++)
		s.syncAsUint32LE(_chapter2Data2[i]);

	// Chapter 2 counters (1-4)
	s.syncAsSint32LE(_chapter2Counters[0]);
	s.syncAsSint32LE(_chapter2Counters[1]);
	s.syncAsSint32LE(_chapter2Counters[2]);
	s.syncAsSint32LE(_chapter2Counters[3]);

	// Chapter 2 actor data (Part 3)
	for (uint32 i = 0; i < ARRAYSIZE(_chapter2Data3); i++)
		s.syncAsUint32LE(_chapter2Data3[i]);

	// Chapter 2 counter (5) and other data
	s.syncAsSint32LE(_chapter2Counters[4]);
	s.syncAsSint32LE(_chapter2FrameIndexOffset);
	s.syncAsSint32LE(_chapter2ActorIndex);

	//s.syncAsSint32LE(_eventUpdateFlag);

	// Chapter 2 counters (6-8)
	s.syncAsSint32LE(_chapter2Counters[5]);
	s.syncAsSint32LE(_chapter2Counters[6]);
	s.syncAsSint32LE(_chapter2Counters[7]);

	// Chapter 2 actor data (Part 4)
	for (uint32 i = 0; i < ARRAYSIZE(_chapter2Data4); i++)
		s.syncAsUint32LE(_chapter2Data4[i]);

	// Actor UpdateStatusEnabled Counter
	s.syncAsSint32LE(_actorUpdateStatusEnabledCounter);

	// Chapter2 actor data (part 5)
	for (uint32 i = 0; i < ARRAYSIZE(_chapter2Data5); i++)
		s.syncAsUint32LE(_chapter2Data5[i]);

	// Encounter disable player on exit
	//s.syncAsSint32LE(encounterDisablePlayerOnExit);

	// Scene flag 1
	s.syncAsUint32LE(_flagScene1);

	// Next screen update
	s.syncAsUint32LE(_nextScreenUpdate);

	// Viewed movies (we also load it from an external file...)
	//s.syncBytes(&_moviesViewed, sizeof(_moviesViewed));

	// Actor update status 15 check
	s.syncAsUint32LE(_flagActorUpdateStatus15Check);

	// TODO More?
}

} // End of namespace Asylum
