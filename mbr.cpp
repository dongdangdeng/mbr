#include "mbr.hpp"
#include "mbr.hpp"
#include <DxLib.h>
#include <WinBase.h>
#define _USE_MATH_DEFINES
#include <math.h>

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
	title = "Mandelbrot";
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
	while (ProcessMessage() == 0) {
		draw();
		//drawTest();
		loupe();
	}
}

void Mbr::draw() 
{
	mbrImg = MakeScreen(PX, PX, false);
	SetDrawScreen(mbrImg);
	double currentMax = 0;		//最大計算回数
	//int mMap[PX][PX];
	for (int i = 0; PX > i; i++) {
		for (int j = 0; PX > j; j++) {
			double x = i * range.size / PX + range.x;		//ウィンドウ座標から数学的座標に変換
			double y = j * range.size / PX + range.y;
			int m = mandelbrot(x, y);		//マンテルブロ集合であるか判定
			if (m == 0) {	//発散しない場合(マンテルブロ集合)
				DrawPixel(i / PX, j / PX, GetColor(0, 0, 0));
			}else {
				double h = (double)m / (double)nmax;
				if (currentMax < h) currentMax = h;	//最大計算回数の更新
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
		if (isInWindow){
			SetMouseDispFlag(FALSE);		//マウスカーソルを表示しない
			//ルーペ枠
			DrawBox(mouseX - PX / 4 - 1, mouseY - PX / 4 - 1, mouseX + PX / 4 + 1, mouseY + PX / 4 + 1, GetColor(0, 0, 0), false);
			DrawBox(mouseX - PX / 4, mouseY - PX / 4, mouseX + PX / 4, mouseY + PX / 4, GetColor(255, 255, 255), false);
			//ルーペ中心カーソル
			DrawBox(mouseX - 2, mouseY - 7, mouseX + 2, mouseY + 7, GetColor(0, 0, 0), true);
			DrawBox(mouseX - 7, mouseY - 2, mouseX + 7, mouseY + 2, GetColor(0, 0, 0), true);
			DrawBox(mouseX - 1, mouseY - 6, mouseX + 1, mouseY + 6, GetColor(255,255,255), true);
			DrawBox(mouseX - 6, mouseY - 1, mouseX + 6, mouseY + 1, GetColor(255, 255, 255), true);
			
			DrawBox(0, PX + 5 + 20, PX, PX + 50, GetColor(0, 0, 0), true);
			DrawFormatString(0, PX + 20 + 5, GetColor(255,255,255), "x:%f, y:%f, nmax:%d", 
				range.x + (mouseX - PX / 4) * range.size / PX, 
				range.y + (mouseY - PX / 4) * range.size / PX,nmax);

			if (GetMouseInput() & MOUSE_INPUT_LEFT != 0) {	//左クリック時
				range.x = range.x + (mouseX - PX / 4) * range.size / PX;	//新しい右上座標を代入
				range.y = range.y + (mouseY - PX / 4) * range.size / PX;
				range.size /= 2.0;
				nmax *= 1.1;
				WaitTimer(100);	//待機時間(複数クリック判定になってしまうのを防ぐ)
				return;
			}
		}
		else {
			SetMouseDispFlag(TRUE);		//マウスカーソルを表示
		}
		DrawString(0, PX + 50, "[Ctrl]:menu", GetColor(255,255,255));
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
