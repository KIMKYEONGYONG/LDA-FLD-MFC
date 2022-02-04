
// MFC_LDA005Dlg.h : 헤더 파일
//

#pragma once

#include <Eigen/Core>
#include <Eigen/Eigenvalues>

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "CYPCA.h"
#include "CYMatrix.h"

using namespace cv;
using namespace Eigen;
using namespace std;

typedef struct MyStruct
{

	char distance[1000];
	char percent[1000];
	int thread1_run;
	int thread2_run;
	int thread3_run;
	Mat frame;
	Mat frame_reco;
	bool camara;
	Mat m_result_img_reco;

} 공유데이터;


// CMFC_LDA005Dlg 대화 상자
class CMFC_LDA005Dlg : public CDialogEx
{
// 생성입니다.
public:
	CMFC_LDA005Dlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_MFC_LDA005_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	공유데이터 m_shareData;
public:
	afx_msg void OnBnClickedCamframe();
};
