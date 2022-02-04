
// MFC_LDA005Dlg.h : ��� ����
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

} ����������;


// CMFC_LDA005Dlg ��ȭ ����
class CMFC_LDA005Dlg : public CDialogEx
{
// �����Դϴ�.
public:
	CMFC_LDA005Dlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_MFC_LDA005_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	���������� m_shareData;
public:
	afx_msg void OnBnClickedCamframe();
};
