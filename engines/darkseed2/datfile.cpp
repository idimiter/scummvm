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

#include "common/stream.h"
#include "common/util.h"

#include "engines/darkseed2/datfile.h"
#include "engines/darkseed2/resources.h"

namespace DarkSeed2 {

DATFile::Line::Line() {
}

DATFile::Line::Line(const char *cmd, int cmdLen, const char *args) {
	command   = Common::String(cmd, cmdLen);
	arguments = Common::String(args);
}

DATFile::DATFile(Common::SeekableReadStream &dat) {
	load(dat);
}

DATFile::DATFile(const Resource &dat) {
	load(dat.getStream());
}

DATFile::~DATFile() {
}

bool DATFile::atEnd() const {
	return _pos == _lines.end();
}

void DATFile::load(Common::SeekableReadStream &dat) {
	dat.seek(0);

	while (!dat.err() && !dat.eos()) {
		Common::String line = dat.readLine();
		if (line.empty())
			continue;

		// Remove comments
		const char *semicolon = strchr(line.c_str(), ';');
		if (semicolon)
			line = Common::String(line.c_str(), line.size() - strlen(semicolon));

		// Remove white space
		line.trim();
		if (line.empty())
			continue;

		// Find the command-argument separator
		const char *equals = strchr(line.c_str(), '=');
		if (!equals)
			continue;

		_lines.push_back(Line(line.c_str(), line.size() - strlen(equals), equals + 1));
	}

	_pos = _lines.begin();
}

bool DATFile::nextLine(const Common::String *&command, const Common::String *&arguments) {
	command   = 0;
	arguments = 0;

	// Reached the end?
	if (_pos == _lines.end())
		return false;

	command   = &_pos->command;
	arguments = &_pos->arguments;

	++_pos;

	return true;
}

void DATFile::next() {
	if (_pos != _lines.end())
		++_pos;
}

void DATFile::previous() {
	if (_pos != _lines.begin())
		--_pos;
}

void DATFile::rewind() {
	_pos = _lines.begin();
}

} // End of namespace DarkSeed2
