#include "DxLib.h"
#include "mbr.hpp"

// プログラムは WinMain から始まります
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Mbr mbr;
	mbr.init();
	mbr.main();
	mbr.end();

	return 0;				// ソフトの終了 
}