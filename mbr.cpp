#include "mbr.hpp"
#include "mbr.hpp"
#include <DxLib.h>
#include <WinBase.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <iomanip> //時間を取得するため
#include <time.h> //値を簡単に文字列にするため
#include <string>
#include <direct.h>
#include <sys\stat.h>

Mbr::Mbr()
{

}

void Mbr::init()
{
	//me = this;
	range.size = 4.0;					//集合の1辺の表示範囲
	range.x = -(range.size / 2);
	range.y = -(range.size / 2);
	nmax = 50;					//非マンデルブロ集合と判断する最大計算回数()
	endFrag = false;
	title = "Mandelbrot";
	SetOutApplicationLogValidFlag(FALSE);	//log.txtを生成しない
	ChangeWindowMode(TRUE);		//ウィンドウ表示モード
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
			double x = i * range.size / PX + range.x;		//ウィンドウ座標から数学的座標に変換
			double y = j * range.size / PX + range.y;
			int m = mandelbrot(x, y);		//マンテルブロ集合であるか判定
			if (m == 0) {	//発散しない場合(マンテルブロ集合)
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
	SetDrawScreen(DX_SCREEN_BACK);	//描画ウィンドウをBACKに

	std::string filename;	//保存したファイルの名前
	bool isShowSaveMessage = false;	//保存メッセージを表示するかどうか
	bool successMkDir = true;		//画像保存フォルダが作成可能か
	int successSave;				//保存が成功したか(0...成功 -1...失敗)
	int frameCnt;			//ループ回数をカウントする(一定時間メッセージ表示する際等に使用)
	bool isInWindow;	//マウスポインタがウィンドウ内にあるかどうか
	while (ProcessMessage() == 0) {
		GetMousePoint(&mouseX, &mouseY);	//マウスポインタの座標を更新
		DrawGraph(0, 0, mbrImg, false);		//バッファ済みのマンテルブロ集合を表示
		isInWindow = mouseX > 0 && mouseY > 0 && mouseX < PX && mouseY < PX;	//マウスポインタがウィンドウ内の場合trueが、それ以外だとfalseが代入される

		//ルーペの表示範囲がウィンドウ外にはみ出さないように制限
		if (mouseX - PX / 4 < 0) mouseX = PX / 4;
		if (mouseY - PX / 4 < 0) mouseY = PX / 4;
		if (mouseX + PX / 4 > PX) mouseX = PX - PX / 4;
		if (mouseY + PX / 4 > PX) mouseY = PX - PX / 4;
		if (isInWindow){	//マウスカーソルの座標がウィンドウ内に存在する時
			SetMouseDispFlag(FALSE);		//マウスカーソルを表示しない
			//ルーペ枠
			DrawBox(mouseX - PX / 4 - 1, mouseY - PX / 4 - 1, mouseX + PX / 4 + 1, mouseY + PX / 4 + 1, GetColor(0, 0, 0), false);
			DrawBox(mouseX - PX / 4, mouseY - PX / 4, mouseX + PX / 4, mouseY + PX / 4, GetColor(255, 255, 255), false);
			DrawBox(mouseX - PX / 4 + 1, mouseY - PX / 4 + 1, mouseX + PX / 4 - 1, mouseY + PX / 4 - 1, GetColor(0, 0, 0), false);
			//ルーペ中心カーソル
			DrawBox(mouseX - 2, mouseY - 7, mouseX + 2, mouseY + 7, GetColor(0, 0, 0), true);
			DrawBox(mouseX - 7, mouseY - 2, mouseX + 7, mouseY + 2, GetColor(0, 0, 0), true);
			DrawBox(mouseX - 1, mouseY - 6, mouseX + 1, mouseY + 6, GetColor(255,255,255), true);
			DrawBox(mouseX - 6, mouseY - 1, mouseX + 6, mouseY + 1, GetColor(255, 255, 255), true);
			
			DrawBox(0, PX + 5 + 20, PX, PX + 50, GetColor(0, 0, 0), true);
			DrawFormatString(0, PX + 20 + 5, GetColor(255,255,255), "x:%f, y:%f, nmax:%d", 
				range.x + (mouseX - PX / 4) * range.size / PX, 
				range.y + (mouseY - PX / 4) * range.size / PX,nmax);

			if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {	//左クリック時
				range.x = range.x + (mouseX - PX / 4) * range.size / PX;	//新しい右上座標を代入
				range.y = range.y + (mouseY - PX / 4) * range.size / PX;
				range.size /= 2.0;
				nmax = (int)(nmax * 1.1);
				WaitTimer(100);	//待機時間(複数クリック判定になってしまうのを防ぐ)
				return;
			}

		}
		else {
			SetMouseDispFlag(TRUE);		//マウスカーソルを表示
		}

		GetHitKeyStateAll(KeyBuf);
		DrawBox(0, PX + 50, PX, PX + 75, GetColor(0, 0, 0), TRUE);
		if (KeyBuf[KEY_INPUT_LCONTROL] == 1 || KeyBuf[KEY_INPUT_RCONTROL] == 1) {	//Ctrlキーが押されている時
			DrawString(0, PX + 50, "[S]save img / [I]initialize / [Q]quit", GetColor(255, 255, 255));
			//Sキーが押されている時(画像保存)
			if (KeyBuf[KEY_INPUT_S] == 1 && !isShowSaveMessage) {	//保存メッセージを表示中は保存を行わない(ファイル名が日時で管理されているため、連続で保存するとファイル名がダブる)
				//現在日時を取得し、ファイル名に使用する
				time_t t;
				struct tm lt;
				t = time(NULL);
				localtime_s(&lt, &t);
				
				//画像保存フォルダの確認・無ければ作成
				struct stat statBuf;
				if (stat(saveDir.c_str(), &statBuf) != 0) {					//フォルダが存在しない場合、
					if (_mkdir(saveDir.c_str()) != 0) successMkDir = false;	//フォルダを新規作成し、作成の成否をsuccessMkDirに返す
				}

				if (successMkDir) {
					filename = saveDir + "/20" + std::to_string(lt.tm_year - 100) + "-" + std::to_string(lt.tm_mon + 1) + "-" + std::to_string(lt.tm_mday)
						+ "-" + std::to_string(lt.tm_hour) + "-" + std::to_string(lt.tm_min) + "-" + std::to_string(lt.tm_sec) + ".bmp";
					DrawGraph(0, 0, mbrImg, false);		//バッファ済みのマンテルブロ集合を表示(ルーペを消すため)
					successSave = SaveDrawScreen(0, 0, PX, PX, filename.c_str());
				} else {
					successSave = -1;
				}
				isShowSaveMessage = true;
				frameCnt = 60 * 2;	//2秒表示(60で1秒)
			}

			//Iキーが押されている時(画面初期化)
			if (KeyBuf[KEY_INPUT_I] == 1) {
				init();
				return;
			}

			//Qキーが押されている時(終了)
			if (KeyBuf[KEY_INPUT_Q] == 1) {
				endFrag = true;
				return;
			}
		} else {
			DrawString(0, PX + 50, "[Ctrl]menu", GetColor(255,255,255));
		}

		if(isShowSaveMessage){	//画像保存した際、一定時間保存メッセージを表示する
			//「save as ”ファイル名”」と表示
			std::string saveMessage;
			if (successSave == 0) {	//保存成功
				saveMessage = "save as \"" + filename + "\"";
			}else if (!successMkDir) {	//保存失敗(フォルダ作成失敗)
				saveMessage = "faild to make directory";
			} else {	//保存失敗(画像作成失敗)
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
	// 0.0～1.0 の範囲の値をサーモグラフィみたいな色にする
	// 0.0                    1.0
	// 青    水    緑    黄    赤
	// 最小値以下 = 青
	// 最大値以上 = 赤
	int ret;
	int a = 255;    // alpha値
	int r, g, b;    // RGB値
	double  value = in_value;
	double  tmp_val = cos(4 * M_PI * value);
	int     col_val = (int)((-tmp_val / 2 + 0.5) * 255);
	if (value >= (4.0 / 4.0)) { r = 255;     g = 0;       b = 0; }   // 赤
	else if (value >= (3.0 / 4.0)) { r = 255;     g = col_val; b = 0; }   // 黄～赤
	else if (value >= (2.0 / 4.0)) { r = col_val; g = 255;     b = 0; }   // 緑～黄
	else if (value >= (1.0 / 4.0)) { r = 0;       g = 255;     b = col_val; }   // 水～緑
	else if (value >= (0.0 / 4.0)) { r = 0;       g = col_val; b = 255; }   // 青～水
	else { r = 0;       g = 0;       b = 255; }   // 青
	ret = (a & 0x000000FF) << 24
		| (r & 0x000000FF) << 16
		| (g & 0x000000FF) << 8
		| (b & 0x000000FF);
	return ret;
}

int Mbr::mandelbrot(double x, double y) {	//マンデルブロ集合であるか判定
	double x_0 = 0.0;
	double y_0 = 0.0;
	for (int i = 1; i <= nmax; i++){
		double x_n = x_0 * x_0 - y_0 * y_0 + x;
		double y_n = 2.0 * x_0 * y_0 + y;
		x_0 = x_n;
		y_0 = y_n;

		if (x_0 * x_0 + y_0 * y_0 >= 4.0) return i;    //発散する場合、発散するまでの計算回数を返す(非マンデルブロ集合)
	}
	return 0;	//nmax回計算しても発散条件を満たさない場合、発散しないとみなし、０を返す(マンデルブロ集合)
}

void Mbr::end() 
{
	DxLib_End();
}
