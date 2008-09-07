#include "innocent/inter.h"

#include "common/endian.h"
#include "common/util.h"

#include "innocent/innocent.h"
#include "innocent/logic.h"
#include "innocent/program.h"

namespace Innocent {

enum Debug {
	kOpcodeDetails = 3
};

enum {
	kOpcodeMax = 0xfd
};

class Uint16Argument : public Argument {
public:
	Uint16Argument(byte *ptr) : Argument(ptr) {}
	operator uint16() const { return READ_LE_UINT16(_ptr); }
	Argument operator=(uint16 value) { WRITE_LE_UINT16(_ptr, value); return *this; }
	operator byte() const { return READ_LE_UINT16(_ptr); }
	Argument operator=(byte value) { WRITE_LE_UINT16(_ptr, value); return *this; }
};

class ByteArgument : public Argument {
public:
	ByteArgument(byte *ptr) : Argument(ptr) {}
	operator byte() const { return *_ptr; }
	Argument operator=(byte b) { *_ptr = b; return *this; }
};

Interpreter::Interpreter(Engine *e) :
		_engine(e),
		_logic(e->logic()),
		_resources(e->resources())
		{
}


/* mode:
0 - initialization,
1 - room handler,
8 - dataset init
*/
Status Interpreter::run(byte *code, OpcodeMode mode) {
	_code = code;
	_mode = mode;
	return run();
}

Status Interpreter::run() {
	_errorCount = 0;

	while (true) {
		byte opcode = *_code;
		if (opcode > kOpcodeMax) {
			return kInvalidOpcode;
		}

		uint8 nargs = _argumentsCounts[opcode];
		debug(4, "opcode %02x with %d args", opcode, nargs);

		_currentCode = opcode;
		OpcodeHandler handler = _handlers[opcode];
		if (!handler)
			handler = &Interpreter::defaultHandler;

		Argument *args[6];

		for (uint i = 0; i < nargs; i++)
			args[i] = getArgument();

		if (nargs == 0)
			_code += 2;

		if (opcode == 0x2c || opcode == 0x2d || opcode == 1 || !_errorCount)
			(*handler)(this, args);
		else if (opcode != 0 && opcode < 0x26)
			_errorCount++;

		for (int i = 0; i < nargs; i++)
			delete args[i];

		if (_return)
			break;
	}

	return kReturned;
}

void Interpreter::returnUp() {
	_return = 1;
	// TODO currently only to the toplevel
}

void Interpreter::forgetLastError() {
	if (_errorCount) _errorCount--;
	debug(kOpcodeDetails, "forgotten last error, count now %d", _errorCount);
}

void Interpreter::defaultHandler(Interpreter *self, Argument /*args*/*[]) {
	error("unhandled opcode %d [=0x%02x]", self->_currentCode, self->_currentCode);
}

enum ArgumentTypes {
	kArgumentImmediate = 1,
	kArgumentMainWord = 2,
	kArgumentMainByte = 3,
	kArgumentLocal = 9
};

Argument *Interpreter::readImmediateArg() {
	debug(kOpcodeDetails, "immediate, value 0x%04x", READ_LE_UINT16(_code));
	byte *ptr = _code;
	_code += 2;
	return new Uint16Argument(ptr);
}

/*Argument *Interpreter::readMainByteArg() {
	uint16 index = READ_LE_UINT16(_code);
	_code += 2;
	Argument *arg = new ByteArgument(_resources->getGlobalByteVar(index));
	debug(kOpcodeDetails, "byte wide variable in main, index 0x%04x, value 0x%02x", index, byte(*arg));
	return arg;
}

Argument *Interpreter::readMainWordArg() {
	uint16 offset = READ_LE_UINT16(_code);
	_code += 2;
	Argument *arg = new Uint16Argument(_resources->getGlobalWordVar(offset/2));
	debug(kOpcodeDetails, "word wide variable in main, index 0x%04x, value 0x%04x", offset/2, uint16(*arg));
	return arg;
}


Argument *Interpreter::readLocalArg() {
	uint16 offset = READ_LE_UINT16(_code);
	_code += 2;
	Argument *arg = new Uint16Argument(_logic->roomScript()->localVariable(offset));
	debug(kOpcodeDetails, "local variable, offset 0x%04x, value 0x%04x", offset, uint16(*arg));
	return arg;
}
*/

Argument *Interpreter::getArgument() {
	uint8 argument_type = _code[1];
	_code += 2;
	debug(kOpcodeDetails, "argument type %02x", argument_type);

	switch (argument_type) {
		case kArgumentImmediate:
			return readImmediateArg();
/*		case kArgumentMainWord:
			return readMainWordArg();
		case kArgumentMainByte:
			return readMainByteArg();
		case kArgumentLocal:
			return readLocalArg();*/
		default:
			error("don't know how to handle argument type 0x%02x", argument_type);
	}
}

const uint8 Interpreter::_argumentsCounts[] = {
	#include "opcodes_nargs.data"
};

} // End of namespace Innocent
