
// MFC_LDA005Dlg.cpp : ���� ����
//

#include "stdafx.h"
#include "MFC_LDA005.h"
#include "MFC_LDA005Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


const int mix_size = 16;

vector<CString> m_ImageNameList; // �̸�
vector<Mat> m_result_img;

UINT ���������Լ�1(LPVOID  param);
UINT ���������Լ�2(LPVOID  param);
void matToArray(Mat img, int* result, int rows, int cols);
void imgPreprocessing(Mat img, CString name, int modem, LPVOID param);
Mat ��DIP�������̽�_Histogram_OpenCV(Mat input_img);
int ��DIP�ھ�_HISTOGRAM_EQUALiZATION(unsigned char grayBody[], unsigned char histoGramBody[], int rows, int cols);
CCYMatrix myLDA();
CCYMatrix matToCCYMatrix(int human_index);
CCYMatrix matToCCYMatrix(Mat from);
CCYMatrix meanMatrix(CCYMatrix hu);
CCYMatrix dMatrix(CCYMatrix hu, CCYMatrix mean);
double myLDA(CCYMatrix image, CCYMatrix temp);

vector<Mat> m_image;
map<CString, vector<Mat>> human;

// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFC_LDA005Dlg ��ȭ ����



CMFC_LDA005Dlg::CMFC_LDA005Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMFC_LDA005Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	/*---���������� �ʱ�ȭ ---------------------------------------*/
	m_shareData.thread1_run = 0;
	m_shareData.thread2_run = 0;
	m_shareData.thread3_run = 0;
	m_shareData.camara = FALSE;
	/*------------------------------------------------------------*/
}

void CMFC_LDA005Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFC_LDA005Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CamFrame, &CMFC_LDA005Dlg::OnBnClickedCamframe)
END_MESSAGE_MAP()


// CMFC_LDA005Dlg �޽��� ó����

BOOL CMFC_LDA005Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_ImageNameList.push_back(CString("IU"));
	imgPreprocessing(imread("./������.png"), m_ImageNameList.at(0), 0, NULL);
	imgPreprocessing(imread("./������2.png"), m_ImageNameList.at(0), 0, NULL);
	imgPreprocessing(imread("./������3.png"), m_ImageNameList.at(0), 0, NULL);

	m_image.clear();

	m_ImageNameList.push_back(CString("Barack_Obama"));
	imgPreprocessing(imread("./Barack_Obama.png"), m_ImageNameList.at(1), 0, NULL);
	imgPreprocessing(imread("./Barack_Obama2.png"), m_ImageNameList.at(1), 0, NULL);
	imgPreprocessing(imread("./Barack_Obama3.png"), m_ImageNameList.at(1), 0, NULL);

	m_image.clear();

	m_ImageNameList.push_back(CString("KIM"));
	imgPreprocessing(imread("./kim.jpg"), m_ImageNameList.at(2), 0, NULL);
	imgPreprocessing(imread("./kim2.jpg"), m_ImageNameList.at(2), 0, NULL);
	imgPreprocessing(imread("./kim3.jpg"), m_ImageNameList.at(2), 0, NULL);

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CMFC_LDA005Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CMFC_LDA005Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CMFC_LDA005Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFC_LDA005Dlg::OnBnClickedCamframe()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	AfxBeginThread(���������Լ�1, &m_shareData);
	AfxBeginThread(���������Լ�2, &m_shareData);
}

UINT ���������Լ�1(LPVOID  param){
	���������� *pShareData = (���������� *)param;


	if (pShareData->thread1_run) {
		AfxMessageBox(_T("�̹� ������1�� �������Դϴ�"));
		return -1;
	}
	else {
		pShareData->thread1_run = 1; // ������1 ������ ǥ�� ����
	}

	// LDA 
	// 0) Mat -> CCYMatrix
	// IU �� CCYMatix
	CCYMatrix IU = matToCCYMatrix(0);

	// Obama �� CCYMatix
	CCYMatrix Obama = matToCCYMatrix(1);

	// Kim �� CCYMatix
	CCYMatrix Kim = matToCCYMatrix(2);
	//IU.Print();

	// 1) ������ ����� ���ϱ�
	CCYMatrix m_IU = meanMatrix(IU);
	CCYMatrix m_Obama = meanMatrix(Obama);
	CCYMatrix m_Kim = meanMatrix(Kim);

	// 2) ��ü�� ���
	CCYMatrix m_Human = (m_IU + m_Obama + m_Kim) / m_ImageNameList.size();

	// 3) ���л� ��� = ������� * �������T
	// 3-1) ����
	CCYMatrix d_IU = dMatrix(IU, m_IU);
	CCYMatrix d_Obama = dMatrix(Obama, m_Obama);
	CCYMatrix d_Kim = dMatrix(Kim, m_Kim);

	// 3-2) ���л�

	CCYMatrix s_IU = d_IU * d_IU.t();
	CCYMatrix s_Obama = d_Obama * d_Obama.t();
	CCYMatrix s_Kim = d_Kim * d_Kim.t();
	/*
	CCYMatrix s_IU = d_IU.t() * d_IU;
	CCYMatrix s_Obama = d_Obama.t() * d_Obama;
	CCYMatrix s_Kim = d_Kim.t() * d_Kim;
	*/
	// 4) Ŭ���� ���� ��ĳ��(���л� ���)
	CCYMatrix Sw = s_IU + s_Obama + s_Kim;
	// 5) Ŭ���� ���� ��ĳ��(���л� ���)
	CCYMatrix Sb = (m_IU - m_Human) * (m_IU - m_Human).t() + (m_Obama - m_Human) * (m_Obama - m_Human).t() + (m_Kim - m_Human) * (m_Kim - m_Human).t();
	for (int i = 0; i < Sb.Row(); i++){
		for (int j = 0; j < Sb.Col(); j++){
			Sb[i][j] = m_ImageNameList.size() * Sb[i][j];
		}
	}

	/*
	MatrixXd   Sw_inv = MatrixXd::Ones(Sw.Row(), Sw.Col());
	for (int i = 0; i < Sw.Row(); i++){
		for (int j = 0; j < Sw.Col(); j++){
			Sw_inv(i, j) = Sw[i][j];
		}
	}

	Sw_inv = Sw_inv.inverse();

	CCYMatrix pre_AA(Sw.Row(), Sw.Col());

	for (int i = 0; i < Sw.Row(); i++){
		for (int j = 0; j < Sw.Col(); j++){
			pre_AA[i][j] = Sw_inv(i, j);
		}
	}
	*/

	//Sw.Print();
	CCYMatrix pre_AA(Sw.Row(), Sw.Col());
	pre_AA = Sw.Inv() * Sb;

	MatrixXd    AA = MatrixXd::Ones(pre_AA.Row(), pre_AA.Col());
	for (int i = 0; i < pre_AA.Row(); i++){
		for (int j = 0; j < pre_AA.Col(); j++){
			AA(i, j) = pre_AA[i][j];
		}
	}

	EigenSolver<MatrixXd>  obj(AA);
	auto   V = obj.eigenvectors();
	auto   D = obj.eigenvalues();
	//cout << "Eigen Vector  \n----------------------\n" << V << "\n";
	//cout << "Eigen Value  \n----------------------\n" << D << "\n";

	//CCYMatrix eigenVector(m_ImageNameList.size(), m_ImageNameList.size());
	CCYMatrix eigenVector(mix_size*mix_size, mix_size*mix_size);
	//CCYMatrix eigenValue(1, m_ImageNameList.size());	
	CCYMatrix eigenValue(1, mix_size*mix_size);
	for (int i = 0; i < pre_AA.Row(); i++){
		for (int j = 0; j < pre_AA.Col(); j++){
			eigenVector[i][j] = V(i, j).real();
		}
		eigenValue[0][i] = D(i, 0).real();
	}

	//eigenVector.Print();
	//eigenValue.Print();

	//CCYMatrix real_eigenVector(m_ImageNameList.size(), m_ImageNameList.size() - 1);
	CCYMatrix real_eigenVector(mix_size*mix_size, m_ImageNameList.size() - 1);
	int j = 0;
	for (int i = 0; i < eigenValue.Col(); i++){
		if (!(-mix_size*mix_size < eigenValue[0][i] && eigenValue[0][i] < mix_size*mix_size)){
			eigenVector.Copy(real_eigenVector, eigenVector, CCYRect(j, 0, j, eigenValue.Col() - 1), i, 0);
			j++;
		}
	}

	real_eigenVector.t().Print();

	// ����ȭ
	CCYMatrix D_IU = dMatrix(IU, m_Human);
	CCYMatrix D_Obama = dMatrix(Obama, m_Human);
	CCYMatrix D_Kim = dMatrix(Kim, m_Human);

	D_IU.Print();

	// ����
	CCYMatrix ����_IU = real_eigenVector.t() * D_IU;
	CCYMatrix ����_Obama = real_eigenVector.t() * D_Obama;
	CCYMatrix ����_Kim = real_eigenVector.t() * D_Kim;

	����_IU.Print();

	while (pShareData->thread1_run){

		if (pShareData->camara){
			imgPreprocessing(pShareData->frame_reco, NULL, 1, pShareData);
			//int *arr_INPUT = new int[mix_size * mix_size];
			//matToArray(pShareData->m_result_img_reco, arr_INPUT, mix_size, mix_size);
			Mat new_Human = pShareData->m_result_img_reco;

			CCYMatrix new_Hu = matToCCYMatrix(new_Human);
			//new_Hu.Print();
			//m_Human.Print();
			//CCYMatrix D_new = dMatrix(new_Hu, m_Human);
			CCYMatrix D_new = new_Hu - m_Human;
			//D_new.Print();
			CCYMatrix �������_New = real_eigenVector.t() * D_new;
			
			double lda_IU = myLDA(����_IU, �������_New);
			double lda_Obama = myLDA(����_Obama, �������_New);
			double lda_Kim = myLDA(����_Kim, �������_New);
			sprintf_s(pShareData->distance, "IU = %10.5f / Obama = %10.5f / KIM = %10.5f", lda_IU, lda_Obama, lda_Kim);
				//sprintf_s(pShareData->distance, "IU = %10.5f / Obama = %10.5f / KIM = %10.5f", ngc_IU, ngc_Obama, ngc_KIM);

			waitKey(1000 * 1);

		}

	}


	pShareData->thread1_run = 0; // ������1 ��������
}

UINT ���������Լ�2(LPVOID  param){
	���������� *pShareData = (���������� *)param;

	if (pShareData->thread2_run) {
		AfxMessageBox(_T("�̹� ������2�� �������Դϴ�"));
		return -1;
	}
	else {
		pShareData->thread2_run = 1; // ������2 ������ ǥ�ĳ���
	}


	static BOOL cam_connected = FALSE;

	if (cam_connected) {
		AfxMessageBox(_T("�̹� ī�޶� ����Ǿ��ֽ��ϴ�."));
		return 0;
	}
	VideoCapture  cap;
	boolean isOpen = cap.open(0);

	const int FPS = 30; // 1�ʴ� ����� �����Ӽ�	
	const int width = cap.get(CV_CAP_PROP_FRAME_WIDTH);  // ����
	const int height = cap.get(CV_CAP_PROP_FRAME_HEIGHT); // ����
	const char camTitle[] = "CAM:0 - �����Ϸ��� ESCŰ�� ��������";

	if (!isOpen) {
		AfxMessageBox(_T("ī�޶� ���� ����!"));
		return 0;
	}
	else {
		cam_connected = TRUE;
	}

	namedWindow(camTitle, WINDOW_NORMAL);

	double rate = (double)width / (double)height;
	resizeWindow(camTitle, 700, 700 / rate);

	while (pShareData->thread2_run) {
		cap >> pShareData->frame;  // �� ������(��������)�� frame�� ����	

		// ����ð�
		if (pShareData->frame.empty()) break;  // ��������� ����

		// dotted rectangle
		int harf_weight = pShareData->frame.cols / 2;
		int harf_height = pShareData->frame.rows / 2;
		const int size = 120;
		const int length = 10;
		const int interval = 2;
		for (int i = 0; i < size * 2; i += 10){
			if ((i / interval) % interval == 0){
				line(pShareData->frame, Point(harf_weight - size + i, harf_height - size), Point(harf_weight - size + i + length, harf_height - size), Scalar::all(255), 2, 4, 0);
				line(pShareData->frame, Point(harf_weight - size + i, harf_height + size), Point(harf_weight - size + i + length, harf_height + size), Scalar::all(255), 2, 4, 0);
				line(pShareData->frame, Point(harf_weight - size, harf_height - size + i), Point(harf_weight - size, harf_height - size + i + length), Scalar::all(255), 2, 4, 0);
				line(pShareData->frame, Point(harf_weight + size, harf_height - size + i), Point(harf_weight + size, harf_height - size + i + length), Scalar::all(255), 2, 4, 0);
			}
		}

		char mystr[300];
		sprintf_s(mystr, "Linear Discriminant Analysis FD");
		putText(pShareData->frame, mystr, Point(0, 30), 1, 2, Scalar(0, 0, 0), 1, 8);



		if (pShareData->thread1_run){
			putText(pShareData->frame, pShareData->distance, Point(20, pShareData->frame.rows - 30), 1, 1, Scalar(255, 255, 255), 1, 1);
			//putText(pShareData->frame, pShareData->percent, Point(20, pShareData->frame.rows - 15), 1, 1, Scalar(255, 255, 255), 1, 1);
		}



		imshow(camTitle, pShareData->frame); // frame�� ȭ�鿡 ���

		pShareData->frame_reco = pShareData->frame(Range(120, 360), Range(200, 440));
		pShareData->camara = TRUE;

		waitKey(1000 / 30);

		if (waitKey(1000 / FPS) == 27) break; // ESCŰ�� ������ ����
	}



	cam_connected = FALSE;
	destroyWindow(camTitle);
	cap.release();

	pShareData->thread2_run = 0; // ������2 ��������

	return 0;
}

void matToArray(Mat img, int* result, int rows, int cols){
	if (img.channels() == 3) {
		for (int k = 0, ndx = 0; ndx < rows*cols; k += 3) {
			result[ndx++] = (img.data[k] << 16) | (img.data[k + 1] << 8) | img.data[k + 2];
		}
	}
	else if (img.channels() == 1) {
		for (int r = 0; r < rows; r++) {
			for (int c = 0; c < cols; c++) {

				int R = img.data[c + r*cols];
				int G = R;
				int B = G;

				result[c + r*cols] = (R << 16) | (G << 8) | B;

			}
		}
	}

}

void imgPreprocessing(Mat img, CString name, int mode, LPVOID param){


	if (img.empty()) return;

	// ��ó�� 
	Mat small_img;
	resize(img, small_img, Size(mix_size, mix_size));

	int rows = small_img.rows;
	int cols = small_img.cols;
	// 1) Mat -> img
	int  * result = new int[rows * cols];
	matToArray(small_img, result, rows, cols);

	unsigned char  * gray_result = new unsigned char[rows * cols];
	// 2) gray
	for (int k = 0; k < rows*cols; k++) {
		int r = (result[k] & 0x00FF0000) >> 16;
		int g = (result[k] & 0x0000FF00) >> 8;
		int b = result[k] & 0x000000FF;

		gray_result[k] = (int)(r*0.299 + g*0.587 + b*0.114);
	}

	// 3) histogram Equalizaion
	//unsigned char * histoGramBody = new unsigned char[rows*cols];
	//��DIP�ھ�_HISTOGRAM_EQUALiZATION(gray_result, histoGramBody, rows, cols);

	// 4) img -> Mat
	Mat result_img(rows, cols, CV_8UC1, gray_result);
	���������� *pShareData = (���������� *)param;

	if (mode == 0){
		m_image.push_back(result_img);
		human[name] = m_image;

		m_result_img.push_back(result_img);
	}
	else{
		pShareData->m_result_img_reco = result_img;
	}
}


int ��DIP�ھ�_HISTOGRAM_EQUALiZATION(unsigned char grayBody[], unsigned char histoGramBody[], int rows, int cols){


	// ���μ���
	int pixelR = 0;
	int pixelG = 0;
	int pixelB = 0;
	double histoR[256] = { 0, };
	double histoG[256] = { 0, };
	double histoB[256] = { 0, };
	int sumR[256] = { 0, };
	int sumG[256] = { 0, };
	int sumB[256] = { 0, };
	int totalR = 0;
	int totalG = 0;
	int totalB = 0;
	double scale = 0;
	double resultR[256] = { 0, };
	double resultG[256] = { 0, };
	double resultB[256] = { 0, };

	// ������׷� ����
	for (int k = 0; k < rows*cols; k++)
	{
		int r = (grayBody[k] & 0x00FF0000) >> 16;
		int g = (grayBody[k] & 0x0000FF00) >> 8;
		int b = grayBody[k] & 0x000000FF;

		histoR[r]++;
		histoG[g]++;
		histoB[b]++;
	}

	// ���� ���
	for (int i = 0; i < 256; i++)
	{
		totalR += (int)histoR[i];
		totalG += (int)histoG[i];
		totalB += (int)histoB[i];
		sumR[i] = totalR;
		sumG[i] = totalG;
		sumB[i] = totalB;
	}

	// ����ȭ
	scale = 255.0 / (rows * cols);


	int max_R = 0;
	int max_G = 0;
	int max_B = 0;
	int avg_R, avg_G, avg_B;

	for (int i = 0; i < 256; i++)
	{
		resultR[i] = sumR[i] * scale;
		if (resultR[i] > max_R){
			max_R = (int)(round(resultR[i]));
		}

		resultG[i] = sumG[i] * scale;
		if (resultG[i] > max_G){
			max_G = (int)(round(resultG[i]));
		}
		resultB[i] = sumB[i] * scale;
		if (resultB[i] > max_B){
			max_B = (int)(round(resultB[i]));
		}
	}


	avg_R = 255 / max_R;
	avg_G = 255 / max_G;
	avg_B = 255 / max_B;

	// ���
	for (int k = 0; k < rows * cols; k++)
	{

		int r = (grayBody[k] & 0x00FF0000) >> 16;
		int g = (grayBody[k] & 0x0000FF00) >> 8;
		int b = grayBody[k] & 0x000000FF;

		pixelR = r;
		pixelG = g;
		pixelB = b;

		histoGramBody[k] = (unsigned char)(resultR[pixelR] * avg_R) << 16 | (unsigned char)(resultG[pixelG] * avg_G) << 8 | (unsigned char)(resultB[pixelB] * avg_B);

	}


	return 0;
}

// Mat -> CCYMatrix ��ȯ
CCYMatrix matToCCYMatrix(int human_index){
	CCYMatrix tmp(mix_size* mix_size, m_ImageNameList.size());
	//CCYMatrix tmp(m_ImageNameList.size(),mix_size* mix_size);
	for (int k = 0; k < m_ImageNameList.size(); k++){
		Mat from = human[m_ImageNameList.at(human_index)].at(k);
		for (int i = 0; i < mix_size; i++){
			for (int j = 0; j < mix_size; j++){

				int R = from.data[j + i*from.cols];
				int G = R;
				int B = G;
				int RGB = (R << 16) | (G << 8) | B;

				//tmp[k][mix_size * i + j] = RGB;
				tmp[mix_size * i + j][k] = R;
			}
		}
	}
	return tmp;
}

// �ν��� �� ��ȯ
CCYMatrix matToCCYMatrix(Mat from){
	CCYMatrix tmp(mix_size* mix_size, 1);
	//CCYMatrix tmp(1, mix_size* mix_size);
	for (int i = 0; i < mix_size; i++){
		for (int j = 0; j < mix_size; j++){

			int R = from.data[j + i*from.cols];
			int G = R;
			int B = G;
			int RGB = (R << 16) | (G << 8) | B;

			tmp[mix_size * i + j][0] = R;
			//tmp[0][mix_size * i + j] = R;
		}
	}

	return tmp;
}

// ��� ���
CCYMatrix meanMatrix(CCYMatrix hu){
	CCYMatrix tmp(mix_size* mix_size, 1);
	//CCYMatrix tmp(m_ImageNameList.size(), 1);
	for (int i = 0; i < hu.Row(); i++){
		int sum = 0;
		for (int j = 0; j < hu.Col(); j++){
			sum += hu[i][j];
		}
		tmp[i][0] = (double)sum / (double)hu.Col();
	}
	return tmp;

}

// ������� ���
CCYMatrix dMatrix(CCYMatrix hu, CCYMatrix mean){
	CCYMatrix tmp(mix_size* mix_size, m_ImageNameList.size());
	//CCYMatrix tmp(m_ImageNameList.size(), mix_size* mix_size);
	int a = hu.Row();
	for (int i = 0; i < hu.Row(); i++){
		for (int j = 0; j < hu.Col(); j++){
			tmp[i][j] = hu[i][j] - mean[i][0];
		}
	}
	return tmp;

}

double myLDA(CCYMatrix image, CCYMatrix temp){
	long sum = 0;
	double result = 0;
	//image.Print();
	//temp.Print();

	for (int i = 0; i<image.Col(); i++){
		for (int j = 0; j<image.Row(); j++){
			int im = image[j][i];
			int t = temp[0][i];
			long abc = pow(im - t, 2);
			sum += abc;
		}
		result += sqrt(sum);
	}

	return result;
}