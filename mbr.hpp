#ifndef MBR_HPP
#define MBR_HPP

//Mbr* Mbr::me;

typedef struct {
	double x, y;	//描画する左上座標
	double size;	//マンデルブロ集合の描画範囲
}graphRange_s;

class Mbr {
private:

public:
	int nmax;			//発散しないと判定するまでの最大計算回数
	int mbrImg;			//描画したマンテルブロ集合の画像を格納しているスクリーンID
	int mouseX, mouseY;	//マウスポインタの座標(GetMousePoint( int *XBuf, int *YBuf )で取得)
	const int PX = 400;	//描画範囲（ピクセル）	//PX...ウィンドウサイズ(正方形なのでw,h共通)
	graphRange_s range;
	char* title;
	//static Mbr *me;

	Mbr(); 
	void init();		//DxLib初期化
	void main();		//メインルーチン
	void end();			//終了処理

	void draw();
	void loupe();
	int ColorScaleBCGYR(double);
	int mandelbrot(double a, double b);
};

#endif // !MBR_HPP
