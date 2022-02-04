
// MFC_LDA005Dlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "MFC_LDA005.h"
#include "MFC_LDA005Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


const int mix_size = 16;

vector<CString> m_ImageNameList; // 이름
vector<Mat> m_result_img;

UINT 내쓰레드함수1(LPVOID  param);
UINT 내쓰레드함수2(LPVOID  param);
void matToArray(Mat img, int* result, int rows, int cols);
void imgPreprocessing(Mat img, CString name, int modem, LPVOID param);
Mat 내DIP인터페이스_Histogram_OpenCV(Mat input_img);
int 내DIP코어_HISTOGRAM_EQUALiZATION(unsigned char grayBody[], unsigned char histoGramBody[], int rows, int cols);
CCYMatrix myLDA();
CCYMatrix matToCCYMatrix(int human_index);
CCYMatrix matToCCYMatrix(Mat from);
CCYMatrix meanMatrix(CCYMatrix hu);
CCYMatrix dMatrix(CCYMatrix hu, CCYMatrix mean);
double myLDA(CCYMatrix image, CCYMatrix temp);

vector<Mat> m_image;
map<CString, vector<Mat>> human;

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CMFC_LDA005Dlg 대화 상자



CMFC_LDA005Dlg::CMFC_LDA005Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMFC_LDA005Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	/*---공유데이터 초기화 ---------------------------------------*/
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


// CMFC_LDA005Dlg 메시지 처리기

BOOL CMFC_LDA005Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	m_ImageNameList.push_back(CString("IU"));
	imgPreprocessing(imread("./아이유.png"), m_ImageNameList.at(0), 0, NULL);
	imgPreprocessing(imread("./아이유2.png"), m_ImageNameList.at(0), 0, NULL);
	imgPreprocessing(imread("./아이유3.png"), m_ImageNameList.at(0), 0, NULL);

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

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMFC_LDA005Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMFC_LDA005Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFC_LDA005Dlg::OnBnClickedCamframe()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	AfxBeginThread(내쓰레드함수1, &m_shareData);
	AfxBeginThread(내쓰레드함수2, &m_shareData);
}

UINT 내쓰레드함수1(LPVOID  param){
	공유데이터 *pShareData = (공유데이터 *)param;


	if (pShareData->thread1_run) {
		AfxMessageBox(_T("이미 쓰레드1이 실행중입니다"));
		return -1;
	}
	else {
		pShareData->thread1_run = 1; // 쓰레드1 실행중 표식 남김
	}

	// LDA 
	// 0) Mat -> CCYMatrix
	// IU 의 CCYMatix
	CCYMatrix IU = matToCCYMatrix(0);

	// Obama 의 CCYMatix
	CCYMatrix Obama = matToCCYMatrix(1);

	// Kim 의 CCYMatix
	CCYMatrix Kim = matToCCYMatrix(2);
	//IU.Print();

	// 1) 각각의 평균을 구하기
	CCYMatrix m_IU = meanMatrix(IU);
	CCYMatrix m_Obama = meanMatrix(Obama);
	CCYMatrix m_Kim = meanMatrix(Kim);

	// 2) 전체의 평균
	CCYMatrix m_Human = (m_IU + m_Obama + m_Kim) / m_ImageNameList.size();

	// 3) 공분산 행렬 = 편차행렬 * 편차행렬T
	// 3-1) 편차
	CCYMatrix d_IU = dMatrix(IU, m_IU);
	CCYMatrix d_Obama = dMatrix(Obama, m_Obama);
	CCYMatrix d_Kim = dMatrix(Kim, m_Kim);

	// 3-2) 공분산

	CCYMatrix s_IU = d_IU * d_IU.t();
	CCYMatrix s_Obama = d_Obama * d_Obama.t();
	CCYMatrix s_Kim = d_Kim * d_Kim.t();
	/*
	CCYMatrix s_IU = d_IU.t() * d_IU;
	CCYMatrix s_Obama = d_Obama.t() * d_Obama;
	CCYMatrix s_Kim = d_Kim.t() * d_Kim;
	*/
	// 4) 클래스 내의 스캐터(공분산 행렬)
	CCYMatrix Sw = s_IU + s_Obama + s_Kim;
	// 5) 클래스 간의 스캐터(공분산 행렬)
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

	// 정규화
	CCYMatrix D_IU = dMatrix(IU, m_Human);
	CCYMatrix D_Obama = dMatrix(Obama, m_Human);
	CCYMatrix D_Kim = dMatrix(Kim, m_Human);

	D_IU.Print();

	// 투영
	CCYMatrix 투영_IU = real_eigenVector.t() * D_IU;
	CCYMatrix 투영_Obama = real_eigenVector.t() * D_Obama;
	CCYMatrix 투영_Kim = real_eigenVector.t() * D_Kim;

	투영_IU.Print();

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
			CCYMatrix 투영결과_New = real_eigenVector.t() * D_new;
			
			double lda_IU = myLDA(투영_IU, 투영결과_New);
			double lda_Obama = myLDA(투영_Obama, 투영결과_New);
			double lda_Kim = myLDA(투영_Kim, 투영결과_New);
			sprintf_s(pShareData->distance, "IU = %10.5f / Obama = %10.5f / KIM = %10.5f", lda_IU, lda_Obama, lda_Kim);
				//sprintf_s(pShareData->distance, "IU = %10.5f / Obama = %10.5f / KIM = %10.5f", ngc_IU, ngc_Obama, ngc_KIM);

			waitKey(1000 * 1);

		}

	}


	pShareData->thread1_run = 0; // 쓰레드1 실행종료
}

UINT 내쓰레드함수2(LPVOID  param){
	공유데이터 *pShareData = (공유데이터 *)param;

	if (pShareData->thread2_run) {
		AfxMessageBox(_T("이미 쓰레드2가 실행중입니다"));
		return -1;
	}
	else {
		pShareData->thread2_run = 1; // 쓰레드2 실행중 표식남김
	}


	static BOOL cam_connected = FALSE;

	if (cam_connected) {
		AfxMessageBox(_T("이미 카메라가 연결되어있습니다."));
		return 0;
	}
	VideoCapture  cap;
	boolean isOpen = cap.open(0);

	const int FPS = 30; // 1초당 출력할 프레임수	
	const int width = cap.get(CV_CAP_PROP_FRAME_WIDTH);  // 가로
	const int height = cap.get(CV_CAP_PROP_FRAME_HEIGHT); // 세로
	const char camTitle[] = "CAM:0 - 종료하려면 ESC키를 누르세요";

	if (!isOpen) {
		AfxMessageBox(_T("카메라 연결 실패!"));
		return 0;
	}
	else {
		cam_connected = TRUE;
	}

	namedWindow(camTitle, WINDOW_NORMAL);

	double rate = (double)width / (double)height;
	resizeWindow(camTitle, 700, 700 / rate);

	while (pShareData->thread2_run) {
		cap >> pShareData->frame;  // 한 프레임(정지영상)을 frame에 받음	

		// 현재시각
		if (pShareData->frame.empty()) break;  // 비어있으면 중지

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



		imshow(camTitle, pShareData->frame); // frame을 화면에 출력

		pShareData->frame_reco = pShareData->frame(Range(120, 360), Range(200, 440));
		pShareData->camara = TRUE;

		waitKey(1000 / 30);

		if (waitKey(1000 / FPS) == 27) break; // ESC키를 누르면 종료
	}



	cam_connected = FALSE;
	destroyWindow(camTitle);
	cap.release();

	pShareData->thread2_run = 0; // 쓰레드2 실행종료

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

	// 전처리 
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
	//내DIP코어_HISTOGRAM_EQUALiZATION(gray_result, histoGramBody, rows, cols);

	// 4) img -> Mat
	Mat result_img(rows, cols, CV_8UC1, gray_result);
	공유데이터 *pShareData = (공유데이터 *)param;

	if (mode == 0){
		m_image.push_back(result_img);
		human[name] = m_image;

		m_result_img.push_back(result_img);
	}
	else{
		pShareData->m_result_img_reco = result_img;
	}
}


int 내DIP코어_HISTOGRAM_EQUALiZATION(unsigned char grayBody[], unsigned char histoGramBody[], int rows, int cols){


	// 프로세싱
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

	// 히스토그램 저장
	for (int k = 0; k < rows*cols; k++)
	{
		int r = (grayBody[k] & 0x00FF0000) >> 16;
		int g = (grayBody[k] & 0x0000FF00) >> 8;
		int b = grayBody[k] & 0x000000FF;

		histoR[r]++;
		histoG[g]++;
		histoB[b]++;
	}

	// 누적 계산
	for (int i = 0; i < 256; i++)
	{
		totalR += (int)histoR[i];
		totalG += (int)histoG[i];
		totalB += (int)histoB[i];
		sumR[i] = totalR;
		sumG[i] = totalG;
		sumB[i] = totalB;
	}

	// 정규화
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

	// 출력
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

// Mat -> CCYMatrix 변환
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

// 인식할 얼굴 변환
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

// 평균 계산
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

// 편차행렬 계산
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