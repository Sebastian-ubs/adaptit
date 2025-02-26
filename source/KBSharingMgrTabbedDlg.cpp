/////////////////////////////////////////////////////////////////////////////
/// \project		adaptit
/// \file			KBSharingMgrTabbedDlg.cpp
/// \author			Bruce Waters
/// \date_created	02 July 2013
/// \rcs_id $Id: KBSharingMgrTabbedDlg.cpp 3310 2013-06-19 07:14:50Z adaptit.bruce@gmail.com $
/// \copyright		2013 Bruce Waters, Bill Martin, Erik Brommers, SIL International
/// \license		The Common Public License or The GNU Lesser General Public License (see license directory)
/// \description	This is the implementation file for the KBSharingMgrTabbedDlg class.
/// The KBSharingMgrTabbedDlg class provides a dialog with tabbed pages in which an
/// appropriately authenticated user/manager of a remote KBserver installation may add,
/// edit or remove users stored in the user table of the mysql server, and/or add or
/// remove knowledge base definitions stored in the kb table of the mysql server.
/// \derivation		The KBSharingMgrTabbedDlg class is derived from AIModalDialog.
/////////////////////////////////////////////////////////////////////////////

// the following improves GCC compilation performance
#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma implementation "KBSharingMgrTabbedDlg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
// Include your minimal set of headers here, or wx.h
#include <wx/wx.h>
#endif

#define _WANT_DEBUGLOG // comment out to suppress wxLogDebug() calls

// other includes
#include <wx/docview.h> // needed for classes that reference wxView or wxDocument
#include <wx/valgen.h> // for wxGenericValidator
#include <wx/filesys.h> // for wxFileName
//#include <wx/dir.h> // for wxDir
//#include <wx/choicdlg.h> // for wxGetSingleChoiceIndex
#include <wx/html/htmlwin.h> // for display of the "Help for Administrators.htm" file from the Administrator menu
#include <wx/textfile.h>

//#if defined(_KBSERVER)

#include "Adapt_It.h"
#include "helpers.h"
#include "KbServer.h"
#include "LanguageCodesDlg.h"
#include "LanguageCodesDlg_Single.h"
#include "Adapt_ItView.h"
#include "KBSharingMgrTabbedDlg.h"
#include "HtmlFileViewer.h"
#include "MainFrm.h"
#include "StatusBar.h"

//#include "C:\Program Files (x86)\MariaDB 10.5\include\mysql\mysql.h"
//#include "C:\Program Files (x86)\MariaDB 10.5\lib\"

extern bool gbIsGlossing;

/// Length of the byte-order-mark (BOM) which consists of the three bytes 0xEF, 0xBB and 0xBF
/// in UTF-8 encoding.
//#define nBOMLen 3

/// Length of the byte-order-mark (BOM) which consists of the two bytes 0xFF and 0xFE in
/// in UTF-16 encoding.
//#define nU16BOMLen 2

//static wxUint8 szBOM[nBOMLen] = {0xEF, 0xBB, 0xBF};
//static wxUint8 szU16BOM[nU16BOMLen] = {0xFF, 0xFE};

// event handler table for the KBSharingMgrTabbedDlg class
BEGIN_EVENT_TABLE(KBSharingMgrTabbedDlg, AIModalDialog)
	EVT_INIT_DIALOG(KBSharingMgrTabbedDlg::InitDialog)
	// Note: value from EVT_NOTEBOOK_PAGE_CHANGING are inconsistent across platforms - so
	// it's better to use EVT_NOTEBOOK_PAGE_CHANGED
	EVT_NOTEBOOK_PAGE_CHANGED(-1, KBSharingMgrTabbedDlg::OnTabPageChanged)
	// For page 1: event handlers for managing Users of the shared databases
	EVT_BUTTON(ID_BUTTON_CLEAR_CONTROLS, KBSharingMgrTabbedDlg::OnButtonUserPageClearControls)
	EVT_LISTBOX(ID_LISTBOX_CUR_USERS, KBSharingMgrTabbedDlg::OnSelchangeUsersList)
	EVT_BUTTON(ID_BUTTON_ADD_USER, KBSharingMgrTabbedDlg::OnButtonUserPageAddUser)
	EVT_BUTTON(ID_BUTTON_SHOW_PASSWORD, KBSharingMgrTabbedDlg::OnButtonShowPassword)
	EVT_CHECKBOX(ID_CHECKBOX_USERADMIN, KBSharingMgrTabbedDlg::OnCheckboxUseradmin)
	EVT_BUTTON(ID_BUTTON_CHANGE_PERMISSION, KBSharingMgrTabbedDlg::OnButtonUserPageChangePermission)
	EVT_BUTTON(ID_BUTTON_CHANGE_FULLNAME, KBSharingMgrTabbedDlg::OnButtonUserPageChangeFullname)
	EVT_BUTTON(ID_BUTTON_CHANGE_PASSWORD, KBSharingMgrTabbedDlg::OnButtonUserPageChangePassword)
	EVT_BUTTON(wxID_OK, KBSharingMgrTabbedDlg::OnOK)
	EVT_BUTTON(wxID_CANCEL, KBSharingMgrTabbedDlg::OnCancel)

	END_EVENT_TABLE()

KBSharingMgrTabbedDlg::KBSharingMgrTabbedDlg(wxWindow* parent) // dialog constructor
	: AIModalDialog(parent, -1, _("Shared Database Server Manager"),
		wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
#if defined  (_DEBUG)
	wxLogDebug(_T("\n\n********  KBSharingMgrTabbedDlg() line %d: ENTERING  **********"),__LINE__);
#endif

	m_pApp = &wxGetApp();
	if (m_pApp->m_bConfigMovedDatFileError == TRUE)
	{
		wxString msg = _("InitDialog() failed due to a problem creating the commandLine for \'lookup_user\'.\nOpening the KB Sharing Manager will not work until this problem is fixed.");
		wxString title = _("Looking up user - warning");
		wxMessageBox(msg, title, wxICON_INFORMATION | wxOK);
		m_pApp->LogUserAction(msg);
		return;
	}

	// wx Note: Since InsertPage also calls SetSelection (which in turn activates our OnTabSelChange
	// handler, we need to initialize some variables before CCTabbedNotebookFunc is called below.
	// Specifically m_nCurPage and pKB needs to be initialized - so no harm in putting all vars
	// here before the dialog controls are created via KBEditorDlgFunc.
	m_nCurPage = 0; // default to first page (i.e. Users page)

	SharedKBManagerNotebookFunc2(this, TRUE, TRUE);
	// The declaration is: SharedKBManagerNotebookFunc2( wxWindow *parent, bool call_fit, bool set_sizer );

    // whm 5Mar2019 Note: The SharedKBManagerNotebookFunc2() tabbed dialog now
    // uses the wxStdDialogButtonSizer, and so we need not call the 
    // ReverseOkCancelButtonsForMac() function below.
	//bool bOK;
	//bOK = m_pApp->ReverseOkCancelButtonsForMac(this);
	//bOK = bOK; // avoid warning
	// pointers to the controls common to each page (most of them) are obtained within
	// the LoadDataForPage() function

	// the following pointer to the KBSharingMgrTabbedDlg control is a single instance;
	// it can only be associated with a pointer after the SharedKBManagerNotebookFunc call above
	m_pKBSharingMgrTabbedDlg = (wxNotebook*)FindWindowById(ID_SHAREDKB_MANAGER_DLG);
	wxASSERT(m_pKBSharingMgrTabbedDlg != NULL);
}

KBSharingMgrTabbedDlg::~KBSharingMgrTabbedDlg()
{
}

void KBSharingMgrTabbedDlg::InitDialog(wxInitDialogEvent& WXUNUSED(event)) // InitDialog is method of wxWindow
{
	wxASSERT(m_pApp->m_bKBSharingEnabled);
	// Entry has been effected, but once in, we need user1 to be different from
	// user2, so here set app's m_bWithinMgr to TRUE.
	m_pApp->m_bWithinMgr = TRUE;
	m_pApp->m_bDoingChangeFullname = FALSE; // initialise -- needed by 
					// LoadDataForPage(), TRUE if change is requested
	m_bLegacyEntry = FALSE;  // re-entry to the Mgr in InitDialog will cause
							 // this to be made TRUE, and it will remain
							 // TRUE until the session ends, except when
							 // temporarily set FALSE when an operating functionality
							 // needs to avoid OnSelChange...() and m_mgr... arrays
							 // being accessed, in the legacy code in LoadDataForPage(0)
#if defined  (_DEBUG)
	wxLogDebug(_T("Init_Dialog(for Sharing Mgr) line %d: entering"), __LINE__);
#endif
	// In the KB Sharing Manager, m_bUserAuthenticating should be FALSE,
	// because it should be TRUE only when using the Authenticate2Dlg.cpp
	// outside of the KB Sharing Manager - and in that circumstance, "Normal"
	// storage variables apply; but different ones for the Manager.

	// Get pointers to the controls created in the two pages, using FindWindowById(),
	// which is acceptable because the controls on each page have different id values
	// Listboxes
	m_pUsersListBox = (wxListBox*)m_pKBSharingMgrTabbedDlg->FindWindowById(ID_LISTBOX_CUR_USERS);
	wxASSERT(m_pUsersListBox != NULL);

	m_pConnectedTo = (wxTextCtrl*)m_pKBSharingMgrTabbedDlg->FindWindowById(ID_TEXT_CONNECTED_TO);
	wxASSERT(m_pConnectedTo != NULL);

	// whm 31Aug2021 modified 2 lines below to use the AutoCorrectTextCtrl class which is now
	// used as a custom control in wxDesigner's RetranslationDlgFunc() dialog.
	//m_pTheUsername = (AutoCorrectTextCtrl*)m_pKBSharingMgrTabbedDlg->FindWindowById(ID_THE_USERNAME); // BEW 17Nov21 temp remove the cast
	m_pTheUsername = (wxTextCtrl*)m_pKBSharingMgrTabbedDlg->FindWindowById(ID_THE_USERNAME);
#if (_DEBUG)
	wxTextCtrl* usernameCtrlPtr = m_pTheUsername; // temp
	wxString aUserNm = usernameCtrlPtr->GetValue();
#endif
	wxASSERT(m_pTheUsername != NULL);
	// m_pEditInformalUsername = (AutoCorrectTextCtrl*)m_pKBSharingMgrTabbedDlg->FindWindowById(ID_TEXTCTRL_INFORMAL_NAME); // BEW 17Nov21 temp remove the cast
	m_pEditInformalUsername = (wxTextCtrl*)m_pKBSharingMgrTabbedDlg->FindWindowById(ID_TEXTCTRL_INFORMAL_NAME);
#if (_DEBUG)
	wxTextCtrl* informal_usernameCtrlPtr = m_pEditInformalUsername; // temp
	wxString anInformalNm = informal_usernameCtrlPtr->GetValue();
#endif

	wxASSERT(m_pEditInformalUsername != NULL);
	m_pEditPersonalPassword = (wxTextCtrl*)m_pKBSharingMgrTabbedDlg->FindWindowById(ID_TEXTCTRL_PASSWORD);
	wxASSERT(m_pEditPersonalPassword != NULL);
	m_pEditPasswordTwo = (wxTextCtrl*)m_pKBSharingMgrTabbedDlg->FindWindowById(ID_TEXTCTRL_PASSWORD_TWO);
	wxASSERT(m_pEditPasswordTwo != NULL);
	m_pEditShowPasswordBox = (wxTextCtrl*)m_pKBSharingMgrTabbedDlg->FindWindowById(ID_TEXTCTRL_SEL_USER_PWD);
	wxASSERT(m_pEditShowPasswordBox != NULL);

	// Checkboxes (only the users page has them)
	m_pCheckUserAdmin = (wxCheckBox*)m_pKBSharingMgrTabbedDlg->FindWindowById(ID_CHECKBOX_USERADMIN);
	wxASSERT(m_pCheckUserAdmin != NULL);

	// Buttons
	m_pBtnUsersClearControls = (wxButton*)m_pKBSharingMgrTabbedDlg->FindWindowById(ID_BUTTON_CLEAR_CONTROLS);
	wxASSERT(m_pBtnUsersClearControls != NULL);
	m_pBtnUsersAddUser = (wxButton*)m_pKBSharingMgrTabbedDlg->FindWindowById(ID_BUTTON_ADD_USER);
	wxASSERT(m_pBtnUsersAddUser != NULL);

	// Set an appropriate 'ForManager' KbServer instance which we'll need for the
	// services it provides regarding access to the KBserver
	// We need to use app's m_pKbServer_ForManager as the pointer, but it should be NULL
	// when the Manager is created, and SetKbServer() should create the needed 'ForManager'
	// instance and assign it to our m_pKbServer in-class pointer. So check for the NULL;
	// if there is an instance currently, we'll delete that and recreate a new one on heap
	// to do that
	// Rather than have a SetKbServer() function which is not self documenting about what
	// m_pKbServer is pointing at, make the app point explicit in this module
	if (m_pApp->m_pKbServer_ForManager == NULL)
	{
		m_pApp->m_pKbServer_ForManager = new KbServer(1, TRUE); // TRUE is bForManager
        // That created and set app's m_pKbServer_ForManager to a 'ForManager' KbServer
        // instance and now we assign that to the Manager's m_pKbServer pointer
        m_pKbServer = m_pApp->m_pKbServer_ForManager;
        // (Re-vectoring m_pKbServer to the app's m_pKbServer_Persistent KbServer*
        // instance should only be possible to do in the handler for deleting a remote
        // kb. Everywhere else in this module, m_pKbServer_ForManager is used exclusively)
        m_pKbServer->SetKB(m_pApp->m_pKB); // set ptr to local target text CKB* for local 
										   // KB access as needed
	}
	else
	{
		// Kill the one that somehow escaped death earlier, and make a new one
		delete m_pApp->m_pKbServer_ForManager; // We must kill it, because the user
				// might have just switched to a different adaptation project and
				// so the old source and non-source language codes may now be incorrect
				// for the project which we are currently in
		m_pApp->m_pKbServer_ForManager = new KbServer(1, TRUE); // TRUE is bForManager
        m_pKbServer = m_pApp->m_pKbServer_ForManager;
        m_pKbServer->SetKB(m_pApp->m_pKB); // set ptr to local target text CKB* for local 
										   // KB access as needed
	}

	// BEW 17Dec20 Need initial call of ListUsers() so that the m_pUsersListBox can get
	// populated before the GUI shows the Mgr to the user.
	// NOTE: InitDialog() is called BEFORE the creator for the Mgr class. So the code
	// for testing if user2 (the user looked up) has useradmin permision == 1 has to
	// be here; and so the rejection of unauthorized users has to be done here

	//First, set these app booleans, so that ConfigureMovedDatFile() accesses the needed
	// block for setting commandLine for this situation
	// set the m_bUser1IsUser2 boolean to TRUE, because we are contraining entry to a
	// lookup_user case ( = 2) in which user1 == user2 == app's m_strUserID (the project's user)
	m_pApp->m_bUser1IsUser2 = TRUE;	// (ConfigureMovedDatFile() asserts if it is FALSE)
	m_pApp->m_bWithinMgr = FALSE; // we aren't in the GUI yet
	m_pApp->m_bKBSharingMgrEntered = TRUE;
	m_pApp->m_bHasUseradminPermission = TRUE; // must be TRUE, otherwise ListUsers() aborts
	m_pApp->m_bConfigMovedDatFileError = FALSE; // initialize to "no ConfigureMovedDatFile() error"

	// BEW 16Dec20 Two accesses of the user table are needed. The first, here, LookupUser() 
	// determines whether or not user2 exists in the user table, and what that user's 
	// useradmin permission level is. This check is not needed again if the user has gained 
	// access to the manager, since within the manager the list of users may be altered
	// and require a new call of ListUsers() be made from there.
	// (Only call LoadDataForPage(0) once. When entering to the manager. Thereafter,
	// keep the kbserver user table in sync with the user page by a function, which reads 
	// the results file of do_list_users.exe and updates the m_pOriginalUsersList and 
	// m_pUsersListForeign 'in place' with an Update....() function

// REPLACE START
// pApp-. : m_strKbServerIpAddr, or m_chosenIpAddr; m_strUserID ; m_curNormalUsername ; m_curAuthPassword & set m_DB_password

/*
#if defined  (_DEBUG)
	wxLogDebug(_T("ConfigureDATfile(lookup_user = %d) line %d: entering, in Mgr InitDialog()"),
			lookup_user, __LINE__);
#endif
	bool bExecutedOK = FALSE;
	bool bReady = m_pApp->ConfigureDATfile(lookup_user); // case = 2
	if (bReady)
	{
		// The input .dat file is now set up ready for do_lookup_user.exe
		wxString execFileName = _T("do_lookup_user.exe");
		wxString execPath = m_pApp->m_appInstallPathOnly + m_pApp->PathSeparator; // whm 22Feb2021 changed execPath to m_appInstallPathOnly + PathSeparator
		wxString resultFile = _T("lookup_user_results.dat");
#if defined  (_DEBUG)
		wxLogDebug(_T("CallExecute(lookup_user = %d) line %d: entering, in Mgr creator"),
			lookup_user, __LINE__);
#endif
		bExecutedOK = m_pApp->CallExecute(lookup_user, execFileName, execPath, resultFile, 32, 33, TRUE);
		// In above call, last param, bReportResult, is default FALSE therefore omitted;
		// wait msg 32 is _("Authentication succeeded."), and msg 33 is _("Authentication failed.")
#if defined (_DEBUG)
		bool bPermission = m_pApp->m_bHasUseradminPermission;
		wxUnusedVar(bPermission);
#endif
		// Disallow entry, if user has insufficient permission level
		if (bExecutedOK && !m_pApp->m_bHasUseradminPermission)
		{
			wxString msg = _("Access to the Knowledge Base Sharing Manager denied. Insufficient permission level, 0. Choose a different user having level 1.");
			wxString title = _("Warning: permission too low");
			wxMessageBox(msg, title, wxICON_WARNING & wxOK);
			m_bAllow = FALSE; // enables caller to cause exit of the Mgr handler
			return;
		}
		else
		{
			if (bExecutedOK)
			{
				m_pApp->m_bHasUseradminPermission = TRUE;
			}
		}
		// m_bHasUseradminPermission has now been set, if LookupUser() succeeded, so
		// get what its access permission level is. TRUE allows access to the manager.
		// (in the entry table, '1' value for useradmin)
		m_pApp->m_bConfigMovedDatFileError = FALSE; // "no error"

	} // end of TRUE block for test: if (bReady)
	else
	{
		// return TRUE for the bool below, caller will warn user, and caller's
		// handler will abort the attempt to create the mgr
		m_pApp->m_bConfigMovedDatFileError = TRUE;
		return;
	}
*/

// First, before dealing with the attempt to list users, which is only relevant to
 // authorizing to enter the KB Sharing Manager, or to it's LoadDataForPage(0) once 
 // logged in; we must find out whether the user trying to log in is credentialed to
 // gain access. This depends on that users  useradmin value, in the user table.
 // A '1' value will allow access; a '0' value will be cause Adapt It code to 
 // disallow access to the Manager. (But such a use can still operate as the owning
 // user for a valid kbserver-supported installation for his/her adapting project.)
 // So here we need to get lookup_user.dat suitably initialized and filled with
 // correct parameters, so do_lookup_user.exe can be called by system(), and return
 // a filled one-line lookup_user_results.dat file, with the useradmin flag value
 // in it so we can stored it, and use that for the subsequent list users code further below.
	m_pApp->RemoveDatFileAndEXE(lookup_user); // BEW 11May22 added, must precede call of ConfigureDATfile()
	bool bUserLookupSucceeded = m_pApp->ConfigureDATfile(lookup_user);
	if (bUserLookupSucceeded)
	{
		// The input .dat file is now set up ready for do_lookup_user.exe
		m_pApp->m_server_username_looked_up = m_pApp->m_strUserID;
		m_pApp->m_server_fullname_looked_up = m_pApp->m_strFullname;

		// open the lookup_user_results.dat file, get its one line contents into a wxString

		wxString resultFile = _T("lookup_user_results.dat");
		wxString datPath = m_pApp->m_appInstallPathOnly + m_pApp->PathSeparator + resultFile;
		bool bExists = ::FileExists(datPath);
		wxTextFile f;
		wxString myResults = wxEmptyString; // initialise
		if (bExists)
		{
			bool bOpened = f.Open(datPath);
			if (bOpened)
			{
				myResults = f.GetFirstLine();
				myResults = MakeReverse(myResults);
				// myResults now should be either (a) ",1,.....") or (b) ",0,...."
				// Remove the initial comma, if present
				wxString commaStr = _T(",");
				// BEW 29Oct22 protect call of Get Char(0)
				wxChar firstChar = (wxChar)0;
				if (!myResults.IsEmpty())
				{
					firstChar = myResults.GetChar(0);

					if ((firstChar != _T('\0')) && firstChar == commaStr)
					{
						myResults = myResults.Mid(1);
					}
					// Okay, '0' or '1' is next - which is it?
					wxChar permissionChar = myResults.GetChar(0);


					// Now store it where it will be available for the list_users case
					f.Close();
					// save the permission value where the list_users case (here) can access it below
					m_pApp->m_server_useradmin_looked_up = permissionChar;  // saves '1' or '0' as wxChar
					// and on the app too
					m_pApp->m_bHasUseradminPermission = permissionChar == _T('1') ? TRUE : FALSE;

					// disallow, if user does not have useradmin == 1 permission
					if (bUserLookupSucceeded && !m_pApp->m_bHasUseradminPermission)
					{
						wxString msg = _("Access to the Knowledge Base Sharing Manager denied. Insufficient permission level, 0. Choose a different user having level 1.");
						wxString title = _("Warning: permission too low");
						wxMessageBox(msg, title, wxICON_WARNING & wxOK);
						m_pApp->LogUserAction(msg);
						m_bAllow = FALSE; // enables caller to cause exit of the Mgr handler
						return;
					}
					else
					{
						m_pApp->m_bHasUseradminPermission = TRUE;
					}
				} // end of TRUE block for test: if (!myResults.IsEmpty())
				else
				{
					// Default to no user permission
					m_pApp->m_bHasUseradminPermission = FALSE;
					wxString msg = _("Looking up user for the Tabbed Manager, line %d, failed - the string myResults was empty for user: %s");
					msg = msg.Format(msg, __LINE__, m_pApp->m_strUserID.c_str());
					m_pApp->LogUserAction(msg);
				}
			} // end of TRUE block for test: if (bOpened)
			else
			{
				// Unable to open file
				m_pApp->m_server_username_looked_up = m_pApp->m_strUserID;
				m_pApp->m_server_fullname_looked_up = m_pApp->m_strFullname;
				m_pApp->m_server_useradmin_looked_up = _T('0');  // '1' or '0' as wxChar
				wxString title = _("Unable to open file");
				wxString msg = _("Looking up user \"%s\" failed when attempting to get useradmin value (0 or 1), in support of ListUsers");
				msg = msg.Format(msg, m_pApp->m_strUserID.c_str());
				m_pApp->LogUserAction(msg);
			}
		}
		else
		{
			// Unlikely to fail, if it does, take the 'play safe' option below
			m_pApp->m_server_username_looked_up = m_pApp->m_strUserID;
			m_pApp->m_server_fullname_looked_up = m_pApp->m_strFullname;
			m_pApp->m_server_useradmin_looked_up = _T('0');  // '1' or '0' as wxChar
			wxString title = _("User not found in the KBserver user table");
			wxString msg = _("Looking up user \"%s\" failed when attempting to get useradmin value (0 or 1), in support of ListUsers");
			msg = msg.Format(msg, m_pApp->m_strUserID.c_str());
			m_pApp->LogUserAction(msg);
		}
	}
	else
	{
		// Play safe. Disallow access to the KB Sharing Manager
		m_pApp->m_server_username_looked_up = m_pApp->m_strUserID;
		m_pApp->m_server_fullname_looked_up = m_pApp->m_strFullname;
		m_pApp->m_server_useradmin_looked_up = _T('0');  // '1' or '0' as wxChar
		wxString title = _("Lookup of username failed");
		wxString msg = _("Looking up user \"%s\" failed when attempting to get useradmin value (0 or 1), in support of ListUsers");
		msg = msg.Format(msg, m_pApp->m_strUserID.c_str());
		m_pApp->LogUserAction(msg);
	}
// REPLACE END

	m_pApp->m_bDoingChangeFullname = FALSE; // initialize
	m_pApp->m_bUseForeignOption = TRUE; // we use this to say "The KB Sharing Mgr is where control is"
	m_pApp->m_bWithinMgr = FALSE; // control is not inside yet, InitDialog() will set it TRUE
								  // if the access is granted to the user page
	// Entry has happened, use the TRUE value in ConfigureMovedDatFile() for case lookup_user
	// to ensure the constraint, user1 == user2 == m_strUserID is satisfied for the lookup_user
	// case just a bit below, as refactoring I've removed the need to have Authenticate2Dlg
	// called in order to open the Mgr. Looking at other usernames than m_strUserID from the
	// currently open shared project, is a job for the internals of the Mgr, once opened
	m_pApp->m_bKBSharingMgrEntered = TRUE;


	// These are reasonable defaults for initialization purposes
	if (m_pApp->m_strKbServerIpAddr != m_pApp->m_chosenIpAddr)
	{
		m_pApp->m_pKbServer_ForManager->SetKBServerIpAddr(m_pApp->m_chosenIpAddr);
	}
	else
	{
		m_pApp->m_pKbServer_ForManager->SetKBServerIpAddr(m_pApp->m_strKbServerIpAddr);
	}
	if (m_pApp->m_curNormalUsername != m_pApp->m_strUserID)
	{
		m_pApp->m_pKbServer_ForManager->SetKBServerUsername(m_pApp->m_strUserID);
		m_pApp->m_DB_username = m_pApp->m_strUserID; // pass this to do_list_users.dat
	}
	else
	{
		m_pApp->m_pKbServer_ForManager->SetKBServerUsername(m_pApp->m_curNormalUsername);
		m_pApp->m_DB_username = m_pApp->m_curNormalUsername; // pass this to do_list_users.dat
	}

	m_pApp->m_pKbServer_ForManager->SetKBServerPassword(m_pApp->m_curAuthPassword);
	if (m_pApp->m_DB_password.IsEmpty())
	{
		m_pApp->m_DB_password = m_pApp->m_curAuthPassword; // pass this to do_list_users.dat
	}


	// Get the list of users into the returned list_users_results.dat file
	int rv = m_pApp->m_pKbServer_ForManager->ListUsers(m_pApp->m_strKbServerIpAddr, m_pApp->m_DB_username, m_pApp->m_DB_password);
	wxUnusedVar(rv);
	wxASSERT(rv == 0);

	// Initialize the User page's checkboxes to OFF
	m_pCheckUserAdmin->SetValue(FALSE);

	// Hook up to the m_usersList member of the KbServer instance
	//m_pUsersListForeign = m_pKbServer->GetUsersListForeign(); // an accessor for m_usersListForeign

	// Add the kbserver's ipAddr to the static text 2nd from top of the tabbed dialog
	wxString myStaticText = m_pConnectedTo->GetLabel();
	myStaticText += _T("  "); // two spaces, for ease in reading the value

	wxString theIpAddr = m_pKbServer->GetKBServerIpAddr();

	myStaticText += theIpAddr; // an accessor for m_kbServerIpAddrBase
	m_pConnectedTo->SetLabel(myStaticText);

	// BEW 13Nov20 changed, 
	m_bKbAdmin = TRUE;  // was m_pApp->m_kbserver_kbadmin; // BEW always TRUE now as of 28Aug20
	m_bUserAdmin = TRUE; // was m_pApp->m_kbserver_useradmin; // has to be TRUE for anyone getting access

	// Start by showing Users page
	m_nCurPage = 0;
#if defined  (_DEBUG)
	wxLogDebug(_T("LoadDataForPage(for Sharing Mgr) line %d: entering"), __LINE__);
#endif
	LoadDataForPage(m_nCurPage); // start off showing the Users page (for now)
#if defined  (_DEBUG)
	wxLogDebug(_T("LoadDataForPage(for Sharing Mgr) line %d: just exited"), __LINE__);
#endif
	m_pApp->GetView()->PositionDlgNearTop(this);

#if defined  (_DEBUG)
	wxLogDebug(_T("Init_Dialog(for Sharing Mgr) line %d: exiting"), __LINE__);
#endif
}

KbServer*KBSharingMgrTabbedDlg::GetKbServer()
{
#if defined(_DEBUG) && defined(_WANT_DEBUGLOG)
	//wxLogDebug(_T("KBSharingMgrTabbedDlg::GetKbServer(): m_bForManager = %d"),m_pKbServer->m_bForManager ? 1 : 0);
#endif
   return m_pKbServer;
}

// This is called each time the page to be viewed is switched to
void KBSharingMgrTabbedDlg::LoadDataForPage(int pageNumSelected)
{
	if (pageNumSelected == 0) // Users page
	{
		// Setting m_pApp->m_bChangePermission_DifferentUser to TRUE is
		// incompatible with an operation different than changing some
		// other parameter, so check and hack it to FALSE before making
		// the test following
		if (m_pApp->m_bDoingChangeFullname)
		{
			// The following assures that LoadDataForPage will not 
			// unnecessarily reset the useradmin checkbox further below
			m_pApp->m_bChangingPermission = FALSE;
			m_pApp->m_bChangePermission_DifferentUser = FALSE;
		}
#if defined (_DEBUG)
		wxLogDebug(_T("/n%s::%s(), line %d : PreTest: m_bDoingChangeFullname %d , m_bChangingPermission %d , m_bChangePermission_DifferentUser %d"),
			__FILE__, __FUNCTION__, __LINE__, (int)m_pApp->m_bDoingChangeFullname, 
			(int)m_pApp->m_bChangingPermission, (int)m_pApp->m_bChangePermission_DifferentUser);
#endif
		// BEW 8Jan21 NOTE the test carefully, if user is re-entering the KB Sharing Mgr
		// more than once, each time m_pApp->m_nMgrSel must have previously been set to
		// -1 (wxNOT_FOUND), so that control enters the TRUE block of the compound test
		// below. Failure to reset to -1 will send control to the else block where the
		// there is no attempt to access the arrays for username, fullname and password,
		// with the result that the manager would open with an EMPTY LIST - and not be
		// able to do a thing. So, OnOK() and OnCancel() reinitialize m_nMgrSel to -1
#if defined (_DEBUG)
		wxLogDebug(_T("%s::%s(), line %d : PreTest: m_pApp->m_nMgrSel = %d  <<-- IT MUST BE -1 HERE"),
			__FILE__, __FUNCTION__, __LINE__, m_pApp->m_nMgrSel);
#endif
		if (m_pApp->m_nMgrSel == wxNOT_FOUND)
		{
			// The manager is being re-entered
			m_bLegacyEntry = TRUE;
		}
		else
		{
			// m_nMgrSel is not -1, so...
			// LoadDataForPage is being called during a Manager session, this can
			// happen multiple times, and each time m_bLegacyEntry should be TRUE
			// except when "Change Permission" is the functionality currently
			// in effect. Implement this protocol in this else block. The end of
			// LoadDataForPage() will reinstate m_bLegacyEntry reset to TRUE

			// Which one(s) require no legacy code access? Bleed those out in the
			// TRUE block here; the else block then sets every other functionality
			// to have legacy access to the full works
			if (m_pApp->m_bChangingPermission == TRUE)
			{
				// So far, this is the only one requiring no access to the legacy code
				m_bLegacyEntry = FALSE;
			}
			else
			{
				// The other functionalities will want legacy code access
				m_bLegacyEntry = TRUE;
			}
		}
		// The TRUE block for this test has control do the full works, setting the
		// commandLine for wxExecute() call, getting the returned .dat file's data
		// extracted into app's m_mgr.... set of arrays, calling OnSelchangeUsersList()
		// writing the various string fields to their GUI wxTextCtrl members, and then
		// calling LoadDataForPage() to get the Mgr user page updated	
		if ( m_bLegacyEntry == TRUE)
		{
			// Clear out anything from a previous button press done on this page
			// ==================================================================
			m_pUsersListBox->Clear();
			wxCommandEvent dummy;
			//OnButtonUserPageClearControls(dummy);

			m_pApp->EmptyMgrArraySet(TRUE); // bNormalSet is TRUE  for, m_mgrUsernameArr etc
			m_pApp->EmptyMgrArraySet(FALSE); // bNormalSet is FALSE  for, m_mgrSavedUsernameArr etc

			// Adding a new user, need to add the user line here (manually)
			if (!m_pApp->m_strNewUserLine.IsEmpty())
			{
				m_pApp->m_arrLines.Add(m_pApp->m_strNewUserLine);
			}
#if defined (_DEBUG)
			wxLogDebug(_T("%s::%s() line %d: m_arrLines line count = %d"), 
				__FILE__,__FUNCTION__,__LINE__,m_pApp->m_arrLines.GetCount());
#endif
			m_pApp->ConvertLinesToMgrArrays(m_pApp->m_arrLines); // data may require tweaking before logging

			if (m_pApp->m_arrLines.IsEmpty())
			{
				m_pApp->m_nMgrSel = wxNOT_FOUND; // no selection index could be valid
												 // and don't try to FillUserList()
			}
			else
			{
				// There are lines to get and use for filling the list,
				// and m_nMgrSel will have a non negative value
				FillUserList(m_pApp);
			}

			// Each LoadDataForPage() has to update the useradmin checkbox value, 
			// and fullname value, and username value
			if (m_pApp->m_nMgrSel != wxNOT_FOUND)
			{
				// Get the username
				wxString username;
				wxString fullname;
				int theUseradmin = -1;
				if (!m_pApp->m_mgrUsernameArr.IsEmpty())
				{
					username = m_pApp->m_mgrUsernameArr.Item(m_pApp->m_nMgrSel);
					// select the username in the list
					int seln = m_pUsersListBox->FindString(username);
					// Make sure app's member agrees
					wxASSERT(m_pApp->m_nMgrSel == seln);
					m_pApp->m_nMgrSel = seln;
					// select the list item
					m_pUsersListBox->SetSelection(seln);

					// Put the selected username into the m_pTheUsername wxTextCrl box
					m_pTheUsername->ChangeValue(username);
				}

				// Put the associated fullname value into it's box, or if change of
				// fullname was requested, it will already by reset - in which case
				// check it's correct
				if (m_pApp->m_bDoingChangeFullname)
				{
					// Get the wxTextCtrl's updated value
					wxString strFullname = m_pEditInformalUsername->GetValue();
					wxASSERT(m_pApp->m_nMgrSel != wxNOT_FOUND);
					m_pApp->m_mgrPasswordArr.RemoveAt(m_pApp->m_nMgrSel);
					m_pApp->m_mgrFullnameArr.Insert(strFullname, m_pApp->m_nMgrSel);
				}
				else
				{
					fullname = m_pApp->m_mgrFullnameArr.Item(m_pApp->m_nMgrSel);
					m_pEditInformalUsername->ChangeValue(fullname);
				}

				if (m_pApp->m_bDoingChangePassword && (m_pApp->m_nMgrSel != -1))
				{
					m_pEditShowPasswordBox->Clear();
					wxString strPwd = m_pApp->m_ChangePassword_NewPassword; // stored here 
																// after wxExecute() done
					// We'll not show it changed, but require a "Show Password" click
					// to get the new value displayed. But to display it, it first
					// has to be lodged in the relevant m_mgr....Array of AI.h
					wxASSERT(m_pApp->m_nMgrSel != wxNOT_FOUND);
					m_pApp->m_mgrPasswordArr.RemoveAt(m_pApp->m_nMgrSel);
					m_pApp->m_mgrPasswordArr.Insert(strPwd, m_pApp->m_nMgrSel);
				}

				// Set the useradmin checkbox's value to what it should be, if
				// it needs changing
				if (m_pApp->m_bChangingPermission && m_pApp->m_bChangePermission_DifferentUser)
				{
					theUseradmin = m_pApp->m_mgrUseradminArr.Item(m_pApp->m_nMgrSel);
					bool bUsrAdmin = theUseradmin == 1 ? FALSE : TRUE; // flip the bool value
					m_pCheckUserAdmin->SetValue(bUsrAdmin);
				}

#if defined (_DEBUG)
				wxString dumpedUsers = m_pApp->DumpManagerArray(m_pApp->m_mgrUsernameArr);
				wxString dumpedFullnames = m_pApp->DumpManagerArray(m_pApp->m_mgrFullnameArr);
				wxString dumpedPasswords = m_pApp->DumpManagerArray(m_pApp->m_mgrPasswordArr);
				wxString dumpedUseradmins = m_pApp->DumpManagerUseradmins(m_pApp->m_mgrUseradminArr);

				wxLogDebug(_T("%s::%s(), line %d  usernames: %s"), __FILE__, __FUNCTION__, __LINE__, dumpedUsers.c_str());
				wxLogDebug(_T("%s::%s(), line %d  fullnames: %s"), __FILE__, __FUNCTION__, __LINE__, dumpedFullnames.c_str());
				wxLogDebug(_T("%s::%s(), line %d  passwords: %s"), __FILE__, __FUNCTION__, __LINE__, dumpedPasswords.c_str());
				wxLogDebug(_T("%s::%s(), line %d  useradmins: %s"), __FILE__, __FUNCTION__, __LINE__, dumpedUseradmins.c_str());
#endif

			} // end TRUE block for test: if (m_pApp->m_nMgrSel != wxNOT_FOUND)

			// If a new user was added manually above, we need to empty the
			// line added to m_arrLines, and clear out the two password
			// wxTextCtrl boxes in the dialog's centre column
			if (!m_pApp->m_strNewUserLine.IsEmpty())
			{
				wxString empty = wxEmptyString;
				m_pApp->m_strNewUserLine.Empty();
				m_pEditPersonalPassword->ChangeValue(empty);
				m_pEditPasswordTwo->ChangeValue(empty);
			}
		} // end of TRUE block for test: if ( m_bLegacyEntry == TRUE)
		else
		{
#if defined (_DEBUG)		
			wxLogDebug(_T("/n%s::%s(), line %d : ELSE block, entry here when not doing 'change permission' is a BUG"),
				__FILE__, __FUNCTION__,__LINE__);
#endif
			// When the before and after .py execution's user2 values are the same,
			// nothing much needs to be done, because the checkbox in the GUI has
			// already been flipped to the new value. But that value has not gotten
			// into app's store of the useradmin values, so we do it here
			bool bCurrentValue = m_pCheckUserAdmin->GetValue(); // whether TRUE or FALSE
			// Update the m_mgrUseradminArr to have this value
			int nCurrentValue = bCurrentValue == TRUE ? 1 : 0;
			if (m_pApp->m_nMgrSel != -1)
			{
				// Remove the old value stored
				m_pApp->m_mgrUseradminArr.RemoveAt(m_pApp->m_nMgrSel);
				// Insert nNewValue at the same location
				m_pApp->m_mgrUseradminArr.Insert(nCurrentValue, m_pApp->m_nMgrSel);
#if defined (_DEBUG)
				wxString dumpedUseradmins = m_pApp->DumpManagerUseradmins(m_pApp->m_mgrUseradminArr);
				wxLogDebug(_T("%s::%s(), line %d  useradmins: %s"), __FILE__, __FUNCTION__, 
					__LINE__, dumpedUseradmins.c_str());
#endif
			}
			else
			{
				wxBell();
				// we don't expect this sync error, so just put a message in LogUserAction(), and
				// things should get right eventually, especially if use shuts down and reopens, 
				// and tries again
				wxString msg = _T("LoadDataForPage() line %s: m_nMgrSel value is wrongly -1 in KB Sharing Manager");
				msg = msg.Format(msg, __LINE__);
				m_pApp->LogUserAction(msg);
			}
		} // end of else block for test: if ( m_bLegacyEntry == TRUE)

	} // end of TRUE block for test: if (pageNumSelected == 0)
	m_pApp->m_bDoingChangeFullname = FALSE; // re-initialise pending a new request for fullname change
	m_pApp->m_bDoingChangePassword = FALSE; // restore default
	m_pApp->m_bChangingPermission = FALSE; // restore default
	m_bLegacyEntry = TRUE; // reset default value
}
void KBSharingMgrTabbedDlg::OnButtonUserPageChangePermission(wxCommandEvent& WXUNUSED(event))
{
	m_pApp->m_bChangingPermission = TRUE; // BEW 7Jan21 added
	m_pApp->RemoveDatFileAndEXE(change_permission); // BEW 11May22 added, must precede call of ConfigureDATfile()
	bool bReady = m_pApp->ConfigureDATfile(change_permission); // arg is const int, value 10
	if (bReady)
	{
		// The input .dat file is now set up ready for do_change_permission.exe
		wxString execFileName = _T("do_change_permission.exe");
		wxString execPath = m_pApp->m_appInstallPathOnly + m_pApp->PathSeparator; // whm 22Feb2021 changed execPath to m_appInstallPathOnly + PathSeparator
		wxString resultFile = _T("change_permission_results.dat");
		bool bExecutedOK = m_pApp->CallExecute(list_users, execFileName, execPath, resultFile, 99, 99);
		if (!bExecutedOK)
		{
			// error in the call, inform user, and put entry in LogUserAction() - English will do
			wxString msg = _T("Line %d: CallExecute for enum: change_permission, failed - perhaps input parameters (and/or password) did not match any entry in the user table; Adapt It will continue working ");
			msg = msg.Format(msg, __LINE__);
			wxString title = _T("Probable do_change_permission.exe error");
			wxMessageBox(msg, title, wxICON_WARNING | wxOK);
			m_pApp->LogUserAction(msg);
		}

		// At this point, the user table is altered, so it just remains to
		// use LoadDataForPage() and make the GUI conform to what was done
		LoadDataForPage(0);
	}
}
void KBSharingMgrTabbedDlg::OnButtonUserPageChangeFullname(wxCommandEvent& WXUNUSED(event))
{
	// Get the new fullname value, not the one in the m_pUserStructForeign - the latter is old one
	wxString newFullname = m_pEditInformalUsername->GetValue();
	m_pApp->m_bDoingChangeFullname = TRUE;
	m_pApp->m_strChangeFullname = newFullname; // make sure app knows it
	m_pApp->m_strChangeFullname_User2 = m_pApp->m_Username2; // so LoadDataForPage(0) can grab it
#if defined (_DEBUG)
	wxLogDebug(_T("%s::%s() line %d: before ConfiDATfile(change_fullname): newFullname: %s (old)m_Username2: %s"),
		__FILE__,__FUNCTION__,__LINE__, newFullname.c_str(), m_pApp->m_Username2.c_str());
#endif
	m_pApp->RemoveDatFileAndEXE(change_fullname); // BEW 11May22 added, must precede call of ConfigureDATfile()
	bool bReady = m_pApp->ConfigureDATfile(change_fullname); // arg is const int, value 11
	if (bReady)
	{
		// The input .dat file is now set up ready for do_change_fullname.exe
		wxString execFileName = _T("do_change_fullname.exe");
		wxString execPath = m_pApp->m_appInstallPathOnly + m_pApp->PathSeparator; // whm 22Feb2021 changed execPath to m_appInstallPathOnly + PathSeparator
		wxString resultFile = _T("change_fullname_results.dat");

		bool bExecutedOK = FALSE;
		bExecutedOK = m_pApp->CallExecute(change_fullname, execFileName, execPath, resultFile, 99, 99);
		if (!bExecutedOK)
		{
			// error in the call, inform user, and put entry in LogUserAction() - English will do
			wxString msg = _T("Line %d: CallExecute for enum: change_fullname, failed - perhaps input parameters (and/or password) did not match any entry in the user table; Adapt It will continue working ");
			msg = msg.Format(msg, __LINE__);
			wxString title = _T("Probable do_change_fullname.exe error");
			wxMessageBox(msg, title, wxICON_WARNING | wxOK);
			m_pApp->LogUserAction(msg);
			m_pApp->m_bDoingChangeFullname = FALSE; // restore default value
		}

		// At this point, the user table is altered, so it just remains to
		// Update...() the two lists of structs to achieve synced state
		// and load in the new state to the m_pUsersListBox
		//UpdateUserPage(m_pApp, m_pApp->m_appInstallPathOnly + m_pApp->PathSeparator, resultFile, &m_pApp->m_arrLines); deprecated 22Dec20
		LoadDataForPage(0);

		if (m_pApp->m_bDoingChangeFullname)
		{
			// Turn it off, to default FALSE
			m_pApp->m_bDoingChangeFullname = FALSE;
			// And clear these, until such time as another change of fullname is done
			m_pApp->m_strChangeFullname.Clear();
			m_pApp->m_strChangeFullname_User2.Clear();
		}
	} // end of TRUE block for test: if (bReady)
}

void KBSharingMgrTabbedDlg::OnButtonUserPageChangePassword(wxCommandEvent& WXUNUSED(event))
{
	m_pApp->m_bDoingChangePassword = TRUE;
	// Get the new password value
	wxString newPassword = m_pEditShowPasswordBox->GetValue(); // user should have typed the new one

	// If the user tries to change the password for root access, disallow
	wxString rootPwd = _T("pq46lfy#BZ");
	if (rootPwd == newPassword)
	{
		// disallow, return here after informing user
		wxString msg = _("You are not allowed to change that password. You can change any of the others.");
		wxString title = _("Illegal Password Change");
		wxMessageBox(msg, title, wxICON_WARNING | wxOK);
		m_pEditShowPasswordBox->Clear();
		wxBell();
		return;
	}

	m_pApp->m_ChangePassword_NewPassword = newPassword; // make sure app knows it
	m_pApp->m_strChangePassword_User2 = m_pApp->m_Username2; // so LoadDataForPage(0) can grab it

	// Test, it's same as what's in gui text ctrl
	wxString guiUser2 = m_pTheUsername->GetValue(); // use this if the assert trips
	wxASSERT(guiUser2 == m_pApp->m_Username2); 
#if defined (_DEBUG)
	wxLogDebug(_T("%s::%s() line %d: before ConfiDATfile(change_password): newPassword: %s (old)m_Username2: %s , guiUser2: %s"),
		__FILE__, __FUNCTION__, __LINE__, newPassword.c_str(), m_pApp->m_Username2.c_str(), guiUser2.c_str());
#endif
	m_pApp->RemoveDatFileAndEXE(change_password); // BEW 11May22 added, must precede call of ConfigureDATfile()
	bool bReady = m_pApp->ConfigureDATfile(change_password); // arg is const int, value 12
	if (bReady)
	{
		// The input .dat file is now set up ready for do_change_fullname.exe
		wxString execFileName = _T("do_change_password.exe");
		wxString execPath = m_pApp->m_appInstallPathOnly + m_pApp->PathSeparator; // whm 22Feb2021 changed execPath to m_appInstallPathOnly + PathSeparator
		wxString resultFile = _T("change_password_results.dat");

		bool bExecutedOK = FALSE;
		bExecutedOK = m_pApp->CallExecute(change_password, execFileName, execPath, resultFile, 99, 99);
		if (!bExecutedOK)
		{
			// error in the call, inform user, and put entry in LogUserAction() - English will do
			wxString msg = _T("Line %d: CallExecute for enum: change_password, failed - perhaps input parameters (and/or password) did not match any entry in the user table; Adapt It will continue working ");
			msg = msg.Format(msg, __LINE__);
			wxString title = _T("Probable do_change_password.exe error");
			wxMessageBox(msg, title, wxICON_WARNING | wxOK);
			m_pApp->LogUserAction(msg);
		}
		// At this point, the user table is altered, so it just remains to
		// load in the new state to the m_pUsersListBox
		LoadDataForPage(0);

		// Clear these, until such time as another change of fullname is done
		m_pApp->m_ChangePassword_NewPassword.Clear();
		m_pApp->m_strChangePassword_User2.Clear();
		
	} // end of TRUE block for test: if (bReady)
}

// BEW 11Jan21 leave it, but with no support for any page but page zero
void KBSharingMgrTabbedDlg::OnTabPageChanged(wxNotebookEvent& event)
{
	// OnTabPageChanged is called whenever any tab is selected
	int pageNumSelected = event.GetSelection();
	if (pageNumSelected == m_nCurPage)
	{
		// user selected same page, so just return
		return;
	}
}

// OnOK() calls wxWindow::Validate, then wxWindow::TransferDataFromWindow.
// If this returns TRUE, the function either calls EndModal(wxID_OK) if the
// dialog is modal, or sets the return value to wxID_OK and calls Show(FALSE)
// if the dialog is modeless.
void KBSharingMgrTabbedDlg::OnOK(wxCommandEvent& event)
{
	m_pUsersListBox->Clear();
	ClearCurPwdBox(); 
	m_pApp->m_nMgrSel = -1; // reinitialize, in case user reenters the Mngr later
	event.Skip();

	delete m_pKBSharingMgrTabbedDlg;
}

void KBSharingMgrTabbedDlg::OnCancel(wxCommandEvent& event)
{
	m_pUsersListBox->Clear();
	ClearCurPwdBox();
	m_pApp->m_nMgrSel = -1; // reinitialize, in case user reenters the Mngr later
	event.Skip();

	delete m_pKBSharingMgrTabbedDlg;
}

// BEW 29Aug20 updated
void KBSharingMgrTabbedDlg::OnButtonUserPageClearControls(wxCommandEvent& WXUNUSED(event))
{
	wxString emptyStr = _T("");
	m_pUsersListBox->SetSelection(wxNOT_FOUND);
	m_pTheUsername->ChangeValue(emptyStr);
	m_pEditInformalUsername->ChangeValue(emptyStr);
	m_pEditPersonalPassword->ChangeValue(emptyStr);
	m_pEditPasswordTwo->ChangeValue(emptyStr);
	m_pCheckUserAdmin->SetValue(FALSE);
	ClearCurPwdBox();
}

// BEW 19Dec20 uses the mgr arrays defined in AI.h at 2217 to 2225 to
// extract values from these arrays for use in the users page of the GUI.
// This function takes (the already filled arrays) and extracts the
// usernames, unsorted, and adds them to the m_pUsersListBox for viewing
void KBSharingMgrTabbedDlg::FillUserList(CAdapt_ItApp* pApp)
{
	// Sanity check - make sure the m_mgrUsernameArr is not empty
	if (pApp->m_mgrUsernameArr.IsEmpty())
	{
#if defined (_DEBUG)
		wxLogDebug(_T("%s::%s(), line %d: pApp->m_mgrUsernameArr is EMPTY"),
			__FILE__, __FUNCTION__, __LINE__);
#endif
		wxBell();
		return;
	}
	int locn = -1;
    locn = locn; // avoid "set but not used" gcc warning in release builds
	int count = pApp->m_mgrUsernameArr.GetCount();
	wxString aUsername;
	int i;
	for (i = 0; i < count; i++)
	{
		aUsername = pApp->m_mgrUsernameArr.Item(i);
		locn = m_pUsersListBox->Append(aUsername);
#if defined (_DEBUG)
		wxLogDebug(_T("%s::%s(), line %d: list location - where appended = %d , aUsername = %s"), 
			__FILE__, __FUNCTION__, __LINE__, locn, aUsername.c_str());
#endif
	}
}

// Return TRUE if the passwords match and are non-empty , FALSE if mismatched
// BEW 11Jan21 This one is needed, for we still check for a match
bool KBSharingMgrTabbedDlg::CheckThatPasswordsMatch(wxString password1, wxString password2)
{
	if (	(password1.IsEmpty() && !password2.IsEmpty()) ||
			(!password1.IsEmpty() && password2.IsEmpty()) ||
			(password1 != password2))
	{
		// The passwords are not matching, tell the user and force him to retype
		wxString msg = _("The passwords in the two boxes are not identical. Try again.");
		wxString title = _("Error: different passwords");
		wxMessageBox(msg, title, wxICON_EXCLAMATION | wxOK);
		// Clear the two password boxes
		wxString emptyStr = _T("");
		m_pEditPersonalPassword->ChangeValue(emptyStr);
		m_pEditPasswordTwo->ChangeValue(emptyStr);
		return FALSE;
	}
	else
		return TRUE;
}

void KBSharingMgrTabbedDlg::OnButtonShowPassword(wxCommandEvent& WXUNUSED(event))
{
	ClearCurPwdBox();
	// Sanity checks
	if (m_pUsersListBox == NULL)
	{
		m_pApp->m_nMgrSel = wxNOT_FOUND; // -1
		return;
	}
	if (m_pUsersListBox->IsEmpty())
	{
		m_pApp->m_nMgrSel = wxNOT_FOUND; // -1
		return;
	}
	// The GetSelection() call returns -1 if there is no selection current, so check for
	// this and return (with a beep) if that's what got returned
	m_pApp->m_nMgrSel = m_pUsersListBox->GetSelection();
	if (m_pApp->m_nMgrSel == wxNOT_FOUND)
	{
		wxBell();
		return;
	}
	// Get the password from the matrix
	wxString the_password = m_pApp->GetFieldAtIndex(m_pApp->m_mgrPasswordArr, m_pApp->m_nMgrSel);
	// show it in the box
	m_pEditShowPasswordBox->ChangeValue(the_password);
}
void KBSharingMgrTabbedDlg::ClearCurPwdBox()
{
	m_pEditShowPasswordBox->Clear();
}

// BEW 29Aug20 updated -- TODO, legacy code commented out
void KBSharingMgrTabbedDlg::OnButtonUserPageAddUser(wxCommandEvent& WXUNUSED(event))
{
	m_pApp->m_bUseForeignOption = TRUE;

	// mediating AI.cpp wxString variables to be set: m_temp_username, m_temp_fullname,
	// m_temp_password, m_temp_useradmin_flag. Set these from within this handler.
	// Then our ConfigureMovedDatFile() can grab it's needed data from these.

	// Legacy comments - still relevant...
	// Username box with a value, informal username box with a value, password box with a
	// value, and the checkbox for useradmin flag, with or without a tick, are mandatory. 
	// Test for these and if one is not set, abort the button press
	// and inform the user why. Also, a selection in the list is irrelevant, and it may
	// mean that the logged in person is about to try to add the selected user a second
	// time - which is illegal, so test for this and if so, warn, and remove the
	// selection, and clear the controls & return. In fact, ensure no duplication of username
	wxString strUsername = m_pTheUsername->GetValue();
	wxString strFullname = m_pEditInformalUsername->GetValue();
	// BEW 24Mar23 couple of problems here. First, strPassword ad strPasswordTwo are going
	// to be empty, even if the chosen user's password shown by "Show Password" is visible.
	// So out test must not ask for these two to be non-empty.
	// Second, our selected user must have a password anyway, otherwise he'd not be in the list.
	// So there's no point in checking it exists. The only test value relevant are the first two,
	// and last - the strUseradmin value - it has to be TRUE for the chosen user to be able
	// to add any other users - regardless of what permission level those others are to receive
	wxString strPassword = m_pEditPersonalPassword->GetValue(); // variable needs to be defined, even if empty
	wxString strPasswordTwo = m_pEditPasswordTwo->GetValue();  //  ditto
	bool bUseradmin = m_pCheckUserAdmin->GetValue();
	wxString strUseradmin = bUseradmin ? _T("1") : _T("0");

	// First, test the relevant parameters have enabling values in them
	if (strUsername.IsEmpty() || strFullname.IsEmpty() || (bUseradmin == FALSE) )
	{
		wxString msg = _("First click a user with full permission level from the user table.\nThe Add User button only works when you have selected a username\ntogether with the fullname, and that username's password.\nYou can see this password by clicking the Show Password button.\nTo give your new user the privilege to add other users\ntick the User Administrator checkbox also.");
		wxString title = _("Warning: No user selected");

		//wxString msg = _("One or more of the text boxes: Username, Informal username, or one or both password boxes, are empty.\nEach of these must have appropriate text typed into them before an Add User request will be honoured. Do so now.\nIf you want this new user to have the privilege to add other users, tick the checkbox also.");
		//wxString title = _("Warning: Incomplete user definition");
		wxMessageBox(msg, title, wxICON_WARNING | wxOK);
		return;
	}
	else
	{
        // BEW 27Jan16 refactored, to test strUsername against *all* currently listed
        // usernames, to ensure this is not an attempt to create a duplicate - that's a
        // fatal error
		int count = m_pUsersListBox->GetCount();
		int index;
		for (index = 0; index < count; index++)
		{
			m_pUsersListBox->SetSelection(index);
			wxString selectedUser = m_pUsersListBox->GetString(index);
			if (selectedUser == strUsername)
			{
				// Oops, he's trying to add someone who is already there. Warn, and probably
				// best to clear the controls to force him to start over
				wxBell();
				wxString title = _("Warning: this user already exists");
				wxString msg = _(
				"You are trying to add a username which already exists in the user table. This is illegal.");
				wxMessageBox(msg, title, wxICON_WARNING | wxOK);
				wxCommandEvent dummy;
				OnButtonUserPageClearControls(dummy);
				return;
			}
		}

		// Test the passwords match in the two boxes for them,(if they don't match, 
		// or a box is empty, then the call will show an exclamation message from 
		// within it, and clear both password text boxes)
		bool bMatchedPasswords = CheckThatPasswordsMatch(strPassword, strPasswordTwo);
		if (!bMatchedPasswords)
		{
			// Unmatched passwords, the user has been told to try again
			return;
		}

		// Create the new entry in the KBserver's user table
		int result = -1; // initialize
		// First, copy the strings needed to the temp variables above
		m_pApp->m_temp_username = strUsername;
		m_pApp->m_temp_fullname = strFullname;
		m_pApp->m_temp_password = strPassword;
		m_pApp->m_temp_useradmin_flag = strUseradmin;
		wxString ipAddr = m_pApp->m_chosenIpAddr;
		wxString datFilename = _T("add_foreign_users.dat");
		wxString resultFile = _T("add_foreign_KBUsers_results.dat");

		bool bCredsOK = Credentials_For_User(&ipAddr, &strUsername, &strFullname,
			&strPassword, bUseradmin, datFilename);
		if (bCredsOK)
		{
			m_pApp->RemoveDatFileAndEXE(credentials_for_user); // BEW 11May22 added, must precede call of ConfigureDATfile() - call does nothing here, yet
			bool bOK = m_pApp->ConfigureDATfile(credentials_for_user);
			if (bOK)
			{
				wxString execFileName = _T("do_add_KBUsers.exe");				
				result = m_pApp->CallExecute(credentials_for_user, execFileName,
						m_pApp->m_curExecPath, resultFile, 99, 99);
			}
		}
		// Update the page if we had success, if no success, just clear the controls
		if (result == 0)
		{
			LoadDataForPage(0);
		}
		else
		{
			// The creation did not succeed -- an error message will have been shown
			// from within the above CreateUser() call
			wxString msg = _T("KB Sharing Manager, line %d: OnButtonUserPageAddUser() failed.");
			msg = msg.Format(msg, __LINE__);
			m_pApp->LogUserAction(msg);
			wxCommandEvent dummy;
			OnButtonUserPageClearControls(dummy);
		}
	}
}

// The box state will have already been changed by the time control enters the handler's body
void KBSharingMgrTabbedDlg::OnCheckboxUseradmin(wxCommandEvent& WXUNUSED(event))
{
	// The value in the checkbox has toggled by the time control
	// enters this handler, so code accordingly
	bool bCurrentUseradminValue = m_pCheckUserAdmin->GetValue();
    // kbadmin value is auto-set to 1 (TRUE) always, because users have to be 
	// able to create entries in kb table

	wxString title = _("Permission level: Set or Change");
	wxString msg = _("Tick the box, but after that do not click 'Change Permission',\nif you are giving the permission 'Can add other users'\n to a new user you are creating.\nTick the box and then click the 'Change Permission' button,\nif you want to change the permission level for an existing user.\nUn-tick the box to give a new user no permission to add other users,\nor to remove an existing user's permission to add new users.");
	wxMessageBox(msg, title, wxICON_INFORMATION | wxOK);

	wxString strAuthenticatedUser;
	wxString strSelectedUsername;

	if (bCurrentUseradminValue)
	{
		// This click was to promote the user administrator privilege level, so that
		// the user can add other users. The box is now ticked, but the kbserver
		// knows nothing of this change. So after this handler exits, the button
		// "Change Permission" needs to be clicked.
		// But if adding a new user is being done, tick the box but do not click
		// the Change Permission button below it
		;
	}
	else
	{
		// The click was to demote from user administrator privilege level...
		//
		// Test, to make sure this constrait is not violated for the authenticated username
		// (that username is that of app's m_strUserID, for whatever project is current)		
		strAuthenticatedUser = m_pApp->m_strUserID;
		// Get the selected username from the list in the manager's GUI: for this we need
		// to use app's m_mgrSavedUsernameArr array, which has just been filled with the
		// list's username strings; and we also need the selection index for which
		// username got clicked - that value is in app's m_nMgrSel public member
		strSelectedUsername = m_pApp->m_mgrSavedUsernameArr.Item(m_pApp->m_nMgrSel);

		// Check these are different usernames, if not, return control to the GUI and
		// reset the checkbox back to ticked (ie. useradmin is reset t0 '1')
		if (strSelectedUsername != strAuthenticatedUser)
		{
			// This guy (on the left) can be safely demoted, because he's
			// not the currently authenticated username for the project
			m_pCheckUserAdmin->SetValue(FALSE);
		}
		else
		{
			// Both usenames are matched, so this selected guy has to retain his 
			// useradmin == TRUE permission level
			wxString title = _("Illegal permission change");
			wxString msg = _("Warning: you are not permitted to change the permission value for the authenticated user: %s");
			msg = msg.Format(msg, strAuthenticatedUser.c_str());
			wxMessageBox(msg, title, wxICON_WARNING | wxOK);

			m_pCheckUserAdmin->SetValue(TRUE); // restore the ticked state of the useradmin checkbox
		}
	}
}

// The user clicked on a different line of the listbox for usernames
void KBSharingMgrTabbedDlg::OnSelchangeUsersList(wxCommandEvent& WXUNUSED(event))
{
	// some minimal sanity checks - can't use Bill's helpers.cpp function
	// ListBoxSanityCheck() because it clobbers the user's selection just made
	if (m_pUsersListBox == NULL)
	{
		m_pApp->m_nMgrSel = wxNOT_FOUND; // -1
		return;
	}
	if (m_pUsersListBox->IsEmpty())
	{
		m_pApp->m_nMgrSel = wxNOT_FOUND; // -1
		return;
	}
	// The GetSelection() call returns -1 if there is no selection current, so check for
	// this and return (with a beep) if that's what got returned
	m_pApp->m_nMgrSel = m_pUsersListBox->GetSelection();
	if (m_pApp->m_nMgrSel == wxNOT_FOUND)
	{
		wxBell();
		return;
	}

#if defined(_DEBUG) //&& defined(_WANT_DEBUGLOG)
	wxLogDebug(_T("OnSelchangeUsersList(): selection index m_nMgrSel = %d"), m_pApp->m_nMgrSel);
#endif

	// Get the username - the one clicked in the list - to compare with whatever
	// was the existing username before the Change Permission request. Why?
	// We need a different processing path if the user clicks the checkbox and asks 
	// for "Change Permission" without altering the selected user in the list. Both
	// processing paths, however, need to call do_change_permission.exe, which
	// has the python code for toggling the useradmin permission to the opposite value
	wxString theUsername = m_pUsersListBox->GetString(m_pApp->m_nMgrSel);
#if defined(_DEBUG) && defined(_WANT_DEBUGLOG)
	wxLogDebug(_T("OnSelchangeUsersList(): from list: username = %s"), theUsername.c_str());
#endif
	wxString userReturned = m_pApp->GetFieldAtIndex(m_pApp->m_mgrUsernameArr, m_pApp->m_nMgrSel);

	// BEW 20Nov20 every time a user is clicked on in the list, we need that user
	// to be stored on the app in the member variable m_strChangePermission_User2
	// because if a change of the user permission is wanted, the ConfigureMovedDatFile()
	// app function will need that user as it's user2 value (user1 is for authenticating
	// to mariaDB/kbserver so is not appropriate). The aforementioned function needs to
	// create the correct comandLine for the ::wxExecute() call in app function CallExecute()
	// and it can't do so without getting the user name selected in the Manager user list
	m_pApp->m_strChangePermission_User2 = userReturned;

#if defined(_DEBUG) && defined(_WANT_DEBUGLOG)
	wxLogDebug(_T("OnSelchangeUsersList(): Old username = %s"),
		m_pApp->m_ChangePermission_OldUser.c_str());
#endif
	if (theUsername != m_pApp->m_ChangePermission_OldUser)
	{
		m_pApp->m_bChangePermission_DifferentUser = TRUE;
	}
	else
	{
		m_pApp->m_bChangePermission_DifferentUser = FALSE; // user didn't select 
				// a different user than the one currently selected in the list
	}

	// BEW 9Dec20 fullname changing needs it's own app storage wxStrings for commandLine building
	m_pApp->m_strChangeFullname_User2 = userReturned;
	wxString fullnameReturned = m_pApp->GetFieldAtIndex(m_pApp->m_mgrFullnameArr, m_pApp->m_nMgrSel);
	m_pApp->m_strChangeFullname = fullnameReturned;
	wxASSERT(!fullnameReturned.IsEmpty()); // make sure it's not empty

	// BEW 20Nov20 a click to select some other user must clear the curr password box
	// so that it does not display the pwd for any user other than the selected user
	ClearCurPwdBox();

	// Copy the whole matrix of mgr arrays to the 'saved set' -- only useradmin is unset so get it
	int useradminReturned = m_pApp->GetIntAtIndex(m_pApp->m_mgrUseradminArr, m_pApp->m_nMgrSel);
	wxUnusedVar(useradminReturned);
	bool bNormalToSaved = TRUE; // empty 'saved set' and refill with 'normal set', 
								// as 'normal set' may change below
	m_pApp->MgrCopyFromSet2DestSet(bNormalToSaved); // facilitates checking for differences below

#if defined(_DEBUG) //&& defined(_WANT_DEBUGLOG)
	wxLogDebug(_T("OnSelchangeUsersList(): username = %s , fullname = %s,  useradmin = %d"),
		userReturned.c_str(), fullnameReturned.c_str(), useradminReturned);
#endif

	// Fill the User page's (central vertical list of wxTextCtrl) controls with their 
	// required data; the logged in user can then edit the these parameters (m_pTheUsername is 
	// the ptr to the username wxTextCtrl)
#if defined(_DEBUG) && defined(_WANT_DEBUGLOG)
	wxLogDebug(_T("OnSelchangeUsersList(): username box BEFORE contains: %s"), m_pTheUsername->GetValue().c_str());
#endif
	// BEW 22Dec20  Changing the value of the username (but keeping rest of row unchanged)
	// is not permitted in our refactored design. It makes no sense. If a different username
	// is wanted, it has to be handled as a CreateUser() scenario - with different fullname and
	// password and it's own useradmin value. The menu item on the Administrator menu
	// can do this job, but we can support it in the Mngr too.	
	m_pTheUsername->ChangeValue(theUsername);

	// BEW 16Dec20 also put the value into app's m_Username2 string using
	// it's Update function UpdateUsername2(wxString str), so that
	// user page buttons which work with whatever is the selected user,
	// can grab its value and use for building commandLine for
	// ConfigureMovedDatFile() - so the correct sql change can be effected
	m_pApp->UpdateUsername2(theUsername);

#if defined(_DEBUG) && defined(_WANT_DEBUGLOG)
	wxLogDebug(_T("OnSelchangeUsersList(): username box AFTER contains: %s"), m_pTheUsername->GetValue().c_str());
	// it should not have changed at this point
#endif

	// Show the fullname associated with theUsername
	wxString aFullname = m_pApp->GetFieldAtIndex(m_pApp->m_mgrFullnameArr, m_pApp->m_nMgrSel);
	m_pEditInformalUsername->ChangeValue(aFullname);

	// Show the checkbox value for the useradmin flag
	int flag = m_pApp->GetIntAtIndex(m_pApp->m_mgrUseradminArr, m_pApp->m_nMgrSel);
	bool bValue = flag == 1 ? TRUE : FALSE;
	m_pCheckUserAdmin->SetValue(bValue);

	// Show the password in central boxes? No, but there is a "Show Password" button for
	// for the box with ptr m_pEditShowPasswordBox - which is empty till "Show Password"
	// button is pressed. So....
	m_pEditShowPasswordBox->ChangeValue(_T("")); // don't display pwd, until asked to
			// When OnSelchangeUsersList() is invoked, the two central password boxes
			// should stay empty. So..
	m_pEditPersonalPassword->ChangeValue(_T(""));
	m_pEditPasswordTwo->ChangeValue(_T(""));

	// Note 1: selecting in the list does NOT in itself cause changes in the mysql
	// database, nor does editing any of the control values, or clearing them. The 
	// the mysql user table is only affected when the user clicks one of the buttons
	// Add User, or Change Permission, or Change Fullname is invoked - because only
	// those three are implemented with .c wrapped to be .exe functions, one for each.

	// Note 2: Remove User is not permitted - it would make an unacceptable mess of
	// the mysql entry table, so we have no support for such a button. Do you want
	// to change a username - the way to do it is to make a whole new user.
}

//#endif
