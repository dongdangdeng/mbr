#ifndef MBR_HPP
#define MBR_HPP

//Mbr* Mbr::me;

typedef struct {
	double x, y;	//�`�悷�鍶����W
	double size;	//�}���f���u���W���̕`��͈�
}graphRange_s;

class Mbr {
private:

public:
	int nmax;			//���U���Ȃ��Ɣ��肷��܂ł̍ő�v�Z��
	int mbrImg;			//�`�悵���}���e���u���W���̉摜���i�[���Ă���X�N���[��ID
	int mouseX, mouseY;	//�}�E�X�|�C���^�̍��W(GetMousePoint( int *XBuf, int *YBuf )�Ŏ擾)
	const int PX = 400;	//�`��͈́i�s�N�Z���j	//PX...�E�B���h�E�T�C�Y(�����`�Ȃ̂�w,h����)
	graphRange_s range;
	char* title;
	//static Mbr *me;

	Mbr(); 
	void init();		//DxLib������
	void main();		//���C�����[�`��
	void end();			//�I������

	void draw();
	void loupe();
	int ColorScaleBCGYR(double);
	int mandelbrot(double a, double b);
};

#endif // !MBR_HPP
