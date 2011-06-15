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
 */

#ifndef RING_FAUST_INIT_H
#define RING_FAUST_INIT_H

namespace Ring {

class ApplicationFaust;

class EventInitFaust {
public:
	EventInitFaust(ApplicationFaust *application);
	~EventInitFaust();

	//////////////////////////////////////////////////////////////////////////
	// Zone initialization
	//////////////////////////////////////////////////////////////////////////
	void initZoneSY();
	void initZone2();
	void initZone3();
	void initZone4();
	void initZone5();
	void initZone6();
	void initZone7();
	void initZone8();
	void initZone9();
	void initZone10();
	void initZone11();
	void initZone12();
	void initZone13();
	void initZone14();
	void initZone15();
	void initZone16();
	void initZone17();

private:
	ApplicationFaust *_app;
};

} // End of namespace Ring

#endif // RING_FAUST_INIT_H
