#include "DxLib.h"
#include "mbr.hpp"

// �v���O������ WinMain ����n�܂�܂�
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Mbr mbr;
	mbr.init();
	mbr.main();
	mbr.end();

	return 0;				// �\�t�g�̏I�� 
}