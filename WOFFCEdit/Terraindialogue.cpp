#include "Terraindialogue.h"
// SelectDialogue.cpp : implementation file
//
IMPLEMENT_DYNAMIC(Terraindialogue, CDialogEx)

//Message map.  Just like MFCMAIN.cpp.  This is where we catch button presses etc and point them to a handy dandy method.
BEGIN_MESSAGE_MAP(Terraindialogue, CDialogEx)
	ON_COMMAND(IDOK, &Terraindialogue::End)					//ok button
	ON_BN_CLICKED(IDOK, &Terraindialogue::OnBnClickedOk)
	ON_LBN_SELCHANGE(IDB_BITMAP1, &Terraindialogue::OnLbnSelchangeBitmap1)
	ON_STN_CLICKED(IDB_BITMAP1, &Terraindialogue::OnStnClickedBitmap1)
	//ON_BN_CLICKED(IDOK2, &Terraindialogue::OnToggleEditor1)
	//ON_COMMAND(ID_BUTTON40001, &Terraindialogue::OnToggleEditor)
END_MESSAGE_MAP()


Terraindialogue::Terraindialogue(CWnd* pParent, std::vector<SceneObject>* SceneGraph)		//constructor used in modal
	: CDialogEx(IDD_DIALOG2, pParent)
{
	Select();
	m_sceneGraph = SceneGraph;
}

Terraindialogue::Terraindialogue(CWnd* pParent)			//constructor used in modeless
	: CDialogEx(IDD_DIALOG2, pParent)
{
}

Terraindialogue::~Terraindialogue()
{
}

///pass through pointers to the data in the tool we want to manipulate
void Terraindialogue::SetObjectData(std::vector<SceneObject>* SceneGraph, int* selection)
{
	//m_sceneGraph = SceneGraph;
	//m_currentSelection = selection;

	////roll through all the objects in the scene graph and put an entry for each in the listbox
	//int numSceneObjects = m_sceneGraph->size();
	//for (int i = 0; i < numSceneObjects; i++)
	//{
	//	//easily possible to make the data string presented more complex. showing other columns.
	//	std::wstring listBoxEntry = std::to_wstring(m_sceneGraph->at(i).ID);
	//	m_listBox.AddString(listBoxEntry.c_str());
	//}
}


void Terraindialogue::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDB_BITMAP1, m_listBox);
	DDX_Control(pDX, IDOK, ToggleEditor);
}

void Terraindialogue::End()
{
	DestroyWindow();	//destory the window properly.  INcluding the links and pointers created.  THis is so the dialogue can start again. 
}

void Terraindialogue::Select()
{
	int index = m_listBox.LoadBitmapW(IDB_BITMAP1);
	CString currentSelectionValue;

	//m_listBox.(index, currentSelectionValue);

	//*m_currentSelection = _ttoi(currentSelectionValue);

}

BOOL Terraindialogue::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//uncomment for modal only
/*	//roll through all the objects in the scene graph and put an entry for each in the listbox
	int numSceneObjects = m_sceneGraph->size();
	for (size_t i = 0; i < numSceneObjects; i++)
	{
		//easily possible to make the data string presented more complex. showing other columns.
		std::wstring listBoxEntry = std::to_wstring(m_sceneGraph->at(i).ID);
		m_listBox.AddString(listBoxEntry.c_str());
	}*/

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void Terraindialogue::PostNcDestroy()
{
}




// SelectDialogue message handlers callback   - We only need this if the dailogue is being setup-with createDialogue().  We are doing
//it manually so its better to use the messagemap
/*INT_PTR CALLBACK SelectProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		//	EndDialog(hwndDlg, wParam);
			DestroyWindow(hwndDlg);
			return TRUE;


		case IDCANCEL:
			EndDialog(hwndDlg, wParam);
			return TRUE;
			break;
		}
	}

	return INT_PTR();
}*/


void Terraindialogue::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}



void Terraindialogue::OnLbnSelchangeBitmap1()
{
	// TODO: Add your control notification handler code here
}


void Terraindialogue::OnStnClickedBitmap1()
{
	// TODO: Add your control notification handler code here
}


void Terraindialogue::OnToggleEditor1() {
	///m_sceneGraph.
}