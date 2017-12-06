#include "mbr.hpp"
#include "mbr.hpp"
#include <DxLib.h>
#include <WinBase.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <iomanip> //���Ԃ��擾���邽��
#include <time.h> //�l���ȒP�ɕ�����ɂ��邽��
#include <string>
#include <direct.h>
#include <sys\stat.h>

Mbr::Mbr()
{

}

void Mbr::init()
{
	//me = this;
	range.size = 4.0;					//�W����1�ӂ̕\���͈�
	range.x = -(range.size / 2);
	range.y = -(range.size / 2);
	nmax = 50;					//��}���f���u���W���Ɣ��f����ő�v�Z��()
	endFrag = false;
	title = "Mandelbrot";
	SetOutApplicationLogValidFlag(FALSE);	//log.txt�𐶐����Ȃ�
	ChangeWindowMode(TRUE);		//�E�B���h�E�\�����[�h
	SetGraphMode(PX, PX + 75, 16);
	SetBackgroundColor(0, 0, 0);
	SetWindowText(title);
	DxLib_Init();
	SetWindowUserCloseEnableFlag(TRUE);
	SetDrawScreen(DX_SCREEN_FRONT);
}

void Mbr::main()
{
	while (ProcessMessage() == 0 && !endFrag) {
		draw();
		loupe();
	}
}

void Mbr::draw() 
{
	mbrImg = MakeScreen(PX, PX, false);
	SetDrawScreen(mbrImg);
	for (int i = 0; PX > i; i++) {
		for (int j = 0; PX > j; j++) {
			double x = i * range.size / PX + range.x;		//�E�B���h�E���W���琔�w�I���W�ɕϊ�
			double y = j * range.size / PX + range.y;
			int m = mandelbrot(x, y);		//�}���e���u���W���ł��邩����
			if (m == 0) {	//���U���Ȃ��ꍇ(�}���e���u���W��)
				DrawPixel(i / PX, j / PX, GetColor(0, 0, 0));
			}else {
				double h = (double)m / (double)nmax;
				DrawPixel(i, j, ColorScaleBCGYR(h));
			}
			if (ProcessMessage() != 0) break;
		}
	}
	SetDrawScreen(DX_SCREEN_FRONT);
	DrawGraph(0, 0, mbrImg, false);
	DrawBox(0, PX, PX, PX + 75, GetColor(0, 0, 0), true);
	DrawFormatString(0, PX + 5, GetColor(255, 255, 255), "x:%f, y:%f, size:%f, ", range.x, range.y, range.size);
}

void Mbr::loupe() {
	SetDrawScreen(DX_SCREEN_BACK);	//�`��E�B���h�E��BACK��

	std::string filename;	//�ۑ������t�@�C���̖��O
	bool isShowSaveMessage = false;	//�ۑ����b�Z�[�W��\�����邩�ǂ���
	bool successMkDir = true;		//�摜�ۑ��t�H���_���쐬�\��
	int successSave;				//�ۑ�������������(0...���� -1...���s)
	int frameCnt;			//���[�v�񐔂��J�E���g����(��莞�ԃ��b�Z�[�W�\������ۓ��Ɏg�p)
	bool isInWindow;	//�}�E�X�|�C���^���E�B���h�E���ɂ��邩�ǂ���
	while (ProcessMessage() == 0) {
		GetMousePoint(&mouseX, &mouseY);	//�}�E�X�|�C���^�̍��W���X�V
		DrawGraph(0, 0, mbrImg, false);		//�o�b�t�@�ς݂̃}���e���u���W����\��
		isInWindow = mouseX > 0 && mouseY > 0 && mouseX < PX && mouseY < PX;	//�}�E�X�|�C���^���E�B���h�E���̏ꍇtrue���A����ȊO����false����������

		//���[�y�̕\���͈͂��E�B���h�E�O�ɂ͂ݏo���Ȃ��悤�ɐ���
		if (mouseX - PX / 4 < 0) mouseX = PX / 4;
		if (mouseY - PX / 4 < 0) mouseY = PX / 4;
		if (mouseX + PX / 4 > PX) mouseX = PX - PX / 4;
		if (mouseY + PX / 4 > PX) mouseY = PX - PX / 4;
		if (isInWindow){	//�}�E�X�J�[�\���̍��W���E�B���h�E���ɑ��݂��鎞
			SetMouseDispFlag(FALSE);		//�}�E�X�J�[�\����\�����Ȃ�
			//���[�y�g
			DrawBox(mouseX - PX / 4 - 1, mouseY - PX / 4 - 1, mouseX + PX / 4 + 1, mouseY + PX / 4 + 1, GetColor(0, 0, 0), false);
			DrawBox(mouseX - PX / 4, mouseY - PX / 4, mouseX + PX / 4, mouseY + PX / 4, GetColor(255, 255, 255), false);
			DrawBox(mouseX - PX / 4 + 1, mouseY - PX / 4 + 1, mouseX + PX / 4 - 1, mouseY + PX / 4 - 1, GetColor(0, 0, 0), false);
			//���[�y���S�J�[�\��
			DrawBox(mouseX - 2, mouseY - 7, mouseX + 2, mouseY + 7, GetColor(0, 0, 0), true);
			DrawBox(mouseX - 7, mouseY - 2, mouseX + 7, mouseY + 2, GetColor(0, 0, 0), true);
			DrawBox(mouseX - 1, mouseY - 6, mouseX + 1, mouseY + 6, GetColor(255,255,255), true);
			DrawBox(mouseX - 6, mouseY - 1, mouseX + 6, mouseY + 1, GetColor(255, 255, 255), true);
			
			DrawBox(0, PX + 5 + 20, PX, PX + 50, GetColor(0, 0, 0), true);
			DrawFormatString(0, PX + 20 + 5, GetColor(255,255,255), "x:%f, y:%f, nmax:%d", 
				range.x + (mouseX - PX / 4) * range.size / PX, 
				range.y + (mouseY - PX / 4) * range.size / PX,nmax);

			if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {	//���N���b�N��
				range.x = range.x + (mouseX - PX / 4) * range.size / PX;	//�V�����E����W����
				range.y = range.y + (mouseY - PX / 4) * range.size / PX;
				range.size /= 2.0;
				nmax = (int)(nmax * 1.1);
				WaitTimer(100);	//�ҋ@����(�����N���b�N����ɂȂ��Ă��܂��̂�h��)
				return;
			}

		}
		else {
			SetMouseDispFlag(TRUE);		//�}�E�X�J�[�\����\��
		}

		GetHitKeyStateAll(KeyBuf);
		DrawBox(0, PX + 50, PX, PX + 75, GetColor(0, 0, 0), TRUE);
		if (KeyBuf[KEY_INPUT_LCONTROL] == 1 || KeyBuf[KEY_INPUT_RCONTROL] == 1) {	//Ctrl�L�[��������Ă��鎞
			DrawString(0, PX + 50, "[S]save img / [I]initialize / [Q]quit", GetColor(255, 255, 255));
			//S�L�[��������Ă��鎞(�摜�ۑ�)
			if (KeyBuf[KEY_INPUT_S] == 1 && !isShowSaveMessage) {	//�ۑ����b�Z�[�W��\�����͕ۑ����s��Ȃ�(�t�@�C�����������ŊǗ�����Ă��邽�߁A�A���ŕۑ�����ƃt�@�C�������_�u��)
				//���ݓ������擾���A�t�@�C�����Ɏg�p����
				time_t t;
				struct tm lt;
				t = time(NULL);
				localtime_s(&lt, &t);
				
				//�摜�ۑ��t�H���_�̊m�F�E������΍쐬
				struct stat statBuf;
				if (stat(saveDir.c_str(), &statBuf) != 0) {					//�t�H���_�����݂��Ȃ��ꍇ�A
					if (_mkdir(saveDir.c_str()) != 0) successMkDir = false;	//�t�H���_��V�K�쐬���A�쐬�̐��ۂ�successMkDir�ɕԂ�
				}

				if (successMkDir) {
					filename = saveDir + "/20" + std::to_string(lt.tm_year - 100) + "-" + std::to_string(lt.tm_mon + 1) + "-" + std::to_string(lt.tm_mday)
						+ "-" + std::to_string(lt.tm_hour) + "-" + std::to_string(lt.tm_min) + "-" + std::to_string(lt.tm_sec) + ".bmp";
					DrawGraph(0, 0, mbrImg, false);		//�o�b�t�@�ς݂̃}���e���u���W����\��(���[�y����������)
					successSave = SaveDrawScreen(0, 0, PX, PX, filename.c_str());
				} else {
					successSave = -1;
				}
				isShowSaveMessage = true;
				frameCnt = 60 * 2;	//2�b�\��(60��1�b)
			}

			//I�L�[��������Ă��鎞(��ʏ�����)
			if (KeyBuf[KEY_INPUT_I] == 1) {
				init();
				return;
			}

			//Q�L�[��������Ă��鎞(�I��)
			if (KeyBuf[KEY_INPUT_Q] == 1) {
				endFrag = true;
				return;
			}
		} else {
			DrawString(0, PX + 50, "[Ctrl]menu", GetColor(255,255,255));
		}

		if(isShowSaveMessage){	//�摜�ۑ������ہA��莞�ԕۑ����b�Z�[�W��\������
			//�usave as �h�t�@�C�����h�v�ƕ\��
			std::string saveMessage;
			if (successSave == 0) {	//�ۑ�����
				saveMessage = "save as \"" + filename + "\"";
			}else if (!successMkDir) {	//�ۑ����s(�t�H���_�쐬���s)
				saveMessage = "faild to make directory";
			} else {	//�ۑ����s(�摜�쐬���s)
				saveMessage = "faild to save img";
			}
			DrawBox(0, PX + 50, PX, PX + 75, GetColor(0, 0, 0), TRUE);
			DrawString(0, PX + 50, saveMessage.c_str(), GetColor(255, 255, 255));
			frameCnt--;
			if (frameCnt <= 0) isShowSaveMessage = false;
		}
		ScreenFlip();
	}
}

int Mbr::ColorScaleBCGYR(double in_value)
{
	// 0.0�`1.0 �͈̔͂̒l���T�[���O���t�B�݂����ȐF�ɂ���
	// 0.0                    1.0
	// ��    ��    ��    ��    ��
	// �ŏ��l�ȉ� = ��
	// �ő�l�ȏ� = ��
	int ret;
	int a = 255;    // alpha�l
	int r, g, b;    // RGB�l
	double  value = in_value;
	double  tmp_val = cos(4 * M_PI * value);
	int     col_val = (int)((-tmp_val / 2 + 0.5) * 255);
	if (value >= (4.0 / 4.0)) { r = 255;     g = 0;       b = 0; }   // ��
	else if (value >= (3.0 / 4.0)) { r = 255;     g = col_val; b = 0; }   // ���`��
	else if (value >= (2.0 / 4.0)) { r = col_val; g = 255;     b = 0; }   // �΁`��
	else if (value >= (1.0 / 4.0)) { r = 0;       g = 255;     b = col_val; }   // ���`��
	else if (value >= (0.0 / 4.0)) { r = 0;       g = col_val; b = 255; }   // �`��
	else { r = 0;       g = 0;       b = 255; }   // ��
	ret = (a & 0x000000FF) << 24
		| (r & 0x000000FF) << 16
		| (g & 0x000000FF) << 8
		| (b & 0x000000FF);
	return ret;
}

int Mbr::mandelbrot(double x, double y) {	//�}���f���u���W���ł��邩����
	double x_0 = 0.0;
	double y_0 = 0.0;
	for (int i = 1; i <= nmax; i++){
		double x_n = x_0 * x_0 - y_0 * y_0 + x;
		double y_n = 2.0 * x_0 * y_0 + y;
		x_0 = x_n;
		y_0 = y_n;

		if (x_0 * x_0 + y_0 * y_0 >= 4.0) return i;    //���U����ꍇ�A���U����܂ł̌v�Z�񐔂�Ԃ�(��}���f���u���W��)
	}
	return 0;	//nmax��v�Z���Ă����U�����𖞂����Ȃ��ꍇ�A���U���Ȃ��Ƃ݂Ȃ��A�O��Ԃ�(�}���f���u���W��)
}

void Mbr::end() 
{
	DxLib_End();
}
