import immlib
import struct

def main(args):
	modules = ['vuln4.exe', 'func.dll', 'KERNEL32.DLL', 'msvcrt.dll', 'KERNELBASE.dll', 'ntdll.dll']
	for module in modules:
		imm = immlib.Debugger()
		mod = imm.getModule(module)
		if (mod == None):
			return "Cannot find module %s" % (module)
		mzbase = mod.getBaseAddress()
		peoffset = struct.unpack('<L',imm.readMemory(mzbase+0x3c,4))[0]
		pebase = mzbase + peoffset
		flags = struct.unpack('<H',imm.readMemory(pebase+0x5e,2))[0]
		if ((flags&0x0040) == 0):
			imm.log("%s module IS NOT ASLR aware" % (module))
		else:
			imm.log("%s module IS ASLR aware" % (module))
	return("END")