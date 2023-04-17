#pragma once
#include "afxdialogex.h"
#include "resource.h"
#include "afxwin.h"
#include "SceneObject.h"
#include <vector>

// SelectDialogue dialog

class Terraindialogue : public CDialogEx
{
	DECLARE_DYNAMIC(Terraindialogue)

public:
	Terraindialogue(CWnd* pParent, std::vector<SceneObject>* SceneGraph);   // modal // takes in out scenegraph in the constructor
	Terraindialogue(CWnd* pParent = NULL);
	virtual ~Terraindialogue();
	void SetObjectData(std::vector<SceneObject>* SceneGraph, int* Selection);	//passing in pointers to the data the class will operate on.

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void End();		//kill the dialogue
	afx_msg void Select();	//Item has been selected

	std::vector<SceneObject>* m_sceneGraph;
	int* m_currentSelection;


	DECLARE_MESSAGE_MAP()
public:
	// Control variable for more efficient access of the listbox
	CBitmap m_listBox;
	virtual BOOL OnInitDialog() override;
	virtual void PostNcDestroy();
	afx_msg void OnBnClickedOk();
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnLbnSelchangeBitmap1();
	afx_msg void OnStnClickedBitmap1();
	CButton ToggleEditor;
	afx_msg void OnToggleEditor1();
	afx_msg void OnToggleEditor();
};


INT_PTR CALLBACK SelectProc(HWND   hwndDlg, UINT   uMsg, WPARAM wParam, LPARAM lParam);