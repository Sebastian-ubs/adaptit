/////////////////////////////////////////////////////////////////////////////
/// \project		adaptit
/// \file			LanguageCodesDlg_Single.cpp
/// \author			Bill Martin
/// \date_created	5 May 2015
/// \rcs_id $Id$
/// \copyright		2008 Bruce Waters, Bill Martin, SIL International
/// \license		The Common Public License or The GNU Lesser General Public License (see license directory)
/// \description	This is the implementation file for the CLanguageCodesDlg_Single class.
/// The CLanguageCodesDlg_Single class provides a dialog in which the user can enter
/// the ISO639-3 2- and 3-letter language code for creating the start of a custom language code.
/// The dialog allows the user to search for the code by language name or the code.
/// \derivation		The CLanguageCodesDlg_Single class is derived from AIModalDialog.
/// This is a cut-down class copied from CLanguageCodesDlg and simplified/////////////////////////////////////////////////////////////////////////////

// the following improves GCC compilation performance
#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma implementation "LanguageCodesDlg_Single.h"
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

// other includes

#include "Adapt_It.h"
#include "LanguageCodesDlg_Single.h"
#include "helpers.h"

/// This global is defined in Adapt_It.cpp.
extern CAdapt_ItApp* gpApp; // if we want to access it fast

/// This global is defined in Adapt_It.cpp.
extern LangInfo langsKnownToWX[];

/// Length of the byte-order-mark (BOM) which consists of the three bytes 0xEF, 0xBB and 0xBF
/// in UTF-8 encoding.
#define nBOMLen 3

/// Length of the byte-order-mark (BOM) which consists of the two bytes 0xFF and 0xFE in
/// in UTF-16 encoding.
#define nU16BOMLen 2

// event handler table
BEGIN_EVENT_TABLE(CLanguageCodesDlg_Single, AIModalDialog)
	EVT_INIT_DIALOG(CLanguageCodesDlg_Single::InitDialog)
	EVT_BUTTON(wxID_OK, CLanguageCodesDlg_Single::OnOK)
	EVT_BUTTON(ID_BUTTON_FIND_CODE2, CLanguageCodesDlg_Single::OnFindCode)
	EVT_BUTTON(ID_BUTTON_FIND_LANGUAGE2, CLanguageCodesDlg_Single::OnFindLanguage)
	EVT_BUTTON(ID_BUTTON_USE_SEL_AS_CODE, CLanguageCodesDlg_Single::OnUseSelectedCodeForCode)
	EVT_LISTBOX(ID_LIST_LANGUAGE_CODES_NAMES, CLanguageCodesDlg_Single::OnSelchangeListboxLanguageCodes)
	EVT_TEXT_ENTER(ID_TEXTCTRL_SEARCH_LANG_NAME2, CLanguageCodesDlg_Single::OnEnterInSearchBox)
END_EVENT_TABLE()

CLanguageCodesDlg_Single::CLanguageCodesDlg_Single(wxWindow* parent) // dialog constructor
	: AIModalDialog(parent, -1, _("Choose language code"),
				wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	// This dialog function below is generated in wxDesigner, and defines the controls and sizers
	// for the dialog. The first parameter is the parent which should normally be "this".
	// The second and third parameters should both be TRUE to utilize the sizers and create the right
	// size dialog.
	SingleLanguageCodeDlgFunc(this, TRUE, TRUE);
	// The declaration is: NameFromwxDesignerDlgFunc( wxWindow *parent, bool call_fit, bool set_sizer );

    // whm 5Mar2019 Note: The SingleLanguageCodeDlgFunc() now uses the wxStdDialogButtonSizer,
    // and so we need not call the ReverseOkCancelButtonsForMac() function in this case.

    SetPointers();
	//m_enumLangCodesChoice = all_possibilities;
}
/*
CLanguageCodesDlg_Single::CLanguageCodesDlg_Single(wxWindow* parent, enum LangCodesChoice choice) // dialog constructor
	: AIModalDialog(parent, -1, _("Choose language code"),
				wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	// This dialog function below is generated in wxDesigner, and defines the controls and sizers
	// for the dialog. The first parameter is the parent which should normally be "this".
	// The second and third parameters should both be TRUE to utilize the sizers and create the right
	// size dialog.
	SingleLanguageCodeDlgFunc(this, TRUE, TRUE);
	// The declaration is: NameFromwxDesignerDlgFunc( wxWindow *parent, bool call_fit, bool set_sizer );
	SetPointers();
	//m_enumLangCodesChoice = all_possibilities;
}
*/

void CLanguageCodesDlg_Single::SetPointers()
{
	bool bOK;
	bOK = gpApp->ReverseOkCancelButtonsForMac(this);
	bOK = bOK; // avoid warning (retain this line as is)
	pListBox = (wxListBox*)FindWindowById(ID_LIST_LANGUAGE_CODES_NAMES);
	wxASSERT(pListBox != NULL);

	pEditSearchForLangName = (wxTextCtrl*)FindWindowById(ID_TEXTCTRL_SEARCH_LANG_NAME2);
	wxASSERT(pEditSearchForLangName != NULL);

	pEditLangCode = (wxTextCtrl*)FindWindowById(ID_TEXTCTRL_LANG_CODE);
	wxASSERT(pEditLangCode != NULL);

	pBtnFindCode = (wxButton*)FindWindowById(ID_BUTTON_FIND_CODE2);
	wxASSERT(pBtnFindCode != NULL);

	pBtnFindLanguage = (wxButton*)FindWindowById(ID_BUTTON_FIND_LANGUAGE2);
	wxASSERT(pBtnFindLanguage != NULL);

	pBtnUseSelectionAsCode = (wxButton*)FindWindowById(ID_BUTTON_USE_SEL_AS_CODE);
	wxASSERT(pBtnUseSelectionAsCode != NULL);
}

CLanguageCodesDlg_Single::~CLanguageCodesDlg_Single() // destructor
{

}

void CLanguageCodesDlg_Single::InitDialog(wxInitDialogEvent& WXUNUSED(event)) // InitDialog is method of wxWindow
{
	//InitDialog() is not virtual, no call needed to a base class
	m_bISO639ListFileFound = TRUE;
    m_bFirstCodeSearch = TRUE;
    m_bFirstNameSearch = TRUE;

    // Adapt It uses both the 2-letter iso639-1 codes and the 3-letter iso639-3
    // language codes concatenated together in a single UTF-8 plain text file
    // (without BOM) called iso639-3codes.txt. 
    // An up-to-date copy of iso639-3codes.txt can be generated by running the
    // convertIANA-registry.sh script file (on Linux) found in the scripts dir.
    // The convertIANA-registry.sh script downloads the full IANA registry data
    // and condenses it down to a UTF-8 list of the 2 and 3 letter codes, a tab
    // character, followed by any comments, etc. The script downloads the IANA
    // data from the site: https://www.iana.org/assignments/language-subtag-registry/language-subtag-registry
    // There are about 185 2-letter codes and they are placed first in the 
    // generated iso639-3codes.txt file. The current file contains 8375 codes
    // plus a header line (that is removed before loading into a list by rogram
    // code below).
    //
    // Load the list box from the iso639-3codes.txt file located in the directory
	// where they were installed. This directory is different on each platform but
	// is the same directory where the xml install file AI_USFM.xml and books.xml
	// were placed. That directory is retrieved using the App's
	// GetDefaultPathForXMLControlFiles() function.
	wxString iso639_3CodesFileName = _T("iso639-3codes.txt");
	wxString pathToLangCodesFile;
	pathToLangCodesFile = gpApp->GetDefaultPathForXMLControlFiles();
	pathToLangCodesFile += gpApp->PathSeparator;
	pathToLangCodesFile += iso639_3CodesFileName;
	pListBox->Clear();
	wxLogNull logNo; // avoid system messages about missing file
	wxFFile f(pathToLangCodesFile); // default mode is "r" or read
	if (!(f.IsOpened() && f.Length() > 0))
	{
		// tell user that the iso639-3codes.txt file cannot be found
		wxString msg = msg.Format(_("Cannot open the %s language codes file at the path:\n   %s\nAdapt It cannot show the list of language names and their codes,\nbut you can enter the 3-letter language codes manually."),iso639_3CodesFileName.c_str(),pathToLangCodesFile.c_str());
        // whm 15May2020 added below to supress phrasebox run-on due to handling of ENTER in CPhraseBox::OnKeyUp()
        gpApp->m_bUserDlgOrMessageRequested = TRUE;
        wxMessageBox(msg, _T(""), wxICON_EXCLAMATION | wxOK);
		m_bISO639ListFileFound = FALSE;
	}

	if (!m_bISO639ListFileFound)
	{
		wxString isoFileNotFound;
		isoFileNotFound = isoFileNotFound.Format(_("NO LIST AVAILABLE - %s File Not Found"),iso639_3CodesFileName.c_str());
		pListBox->Append(isoFileNotFound);
		pListBox->Disable(); // disable the list box window
		// also disable other dialog controls that cannot be used when no list is available
		pEditSearchForLangName->Disable();
		pBtnFindCode->Disable();
		pBtnFindLanguage->Disable();
		pBtnUseSelectionAsCode->Disable();
	}

	if (m_bISO639ListFileFound)
	{
		// read the language codes file
		wxString* pTempStr;
		pTempStr = new wxString;
		bool bSuccessfulRead;
		bSuccessfulRead = f.ReadAll(pTempStr); // ReadAll doesn't require the file's Length
		if(!bSuccessfulRead)
		{
			wxASSERT(FALSE);
			// file may have been truncated
			// TODO: issue warning to user
		}
		// language codes file was read into a wxString buffer so parse it line-by-line into the listbox
		const wxChar* pStr = pTempStr->GetData();
		wxChar* ptr = (wxChar*)pStr;
		// whm Note: regardless of the platform, when reading a text file from disk into a
		// wxString, wxWidgets converts the line endings to a single \n char, hence we can't
		// use the fLen value from above to assign pEnd, but we have to get the new Length()
		// of the resulting string.
		int sLen = pTempStr->Length();
		wxChar* pEnd = ptr+sLen;
		wxString tempLine = _T("");
		const wxString tab5sp = _T("     ");
		//int tabCount = 0;
		// GDLC 3SEP13 Assemble the strings for language codes into a wxArrayString first, and then
		// insert the whole lot into the wxListBox in one call of InsertItems to avoid the lengthy
		// scrolling that happens on Mac with WX2.9.5. Also speeds up operation on Win/Lin.
		wxArrayString as;
		unsigned int pos = 0;	// Counter for current position in wxArrayString for its Insert()
		as.Alloc(8380);			// There are 8376 lines in iso639-3codes.txt, so allocate enough
								// space in one go for efficiency.
                                // Skip over any BOM that might be present

        // whm 11Feb2019 added - skip over any BOM that is present in the file's buffer.
        const int bomLen = 3;
        wxUint8 szBOM[bomLen] = { 0xEF, 0xBB, 0xBF };
        if (!memcmp(pStr, szBOM, nBOMLen))
        {
            ptr = ptr + nBOMLen;
        }
        else if (*pStr == 0xFEFF)
        {
            // skip over the UTF16 BOM in the buffer
            ptr = ptr + 1;
        }

        while (ptr < pEnd && *ptr != '\0')
		{
			if (*ptr == _T('\n') || *ptr == _T('\r'))
			{
				if (!tempLine.IsEmpty())
				{
					// GDLC 3SEP13 Use Insert() on the wxArrayString
					as.Insert(tempLine, pos++);
					// pListBox->Append(tempLine);
					tempLine.Empty();
				}
			}
			else if (*ptr == _T('\t'))
			{
                // whm 11Feb2019 modified - iso639-3codes.txt file data now only has 1 tab in each line
                tempLine += tab5sp; // convert tab to 5 spaces in listbox item string
			}
			else
			{
				// add chars other than newlines and tabs to tempLine
				tempLine += *ptr;
			}
			ptr++;
		}
		// catch the last list item if the codes file does not end with a newline char
		if (!tempLine.IsEmpty())
		{
			as.Insert(tempLine, pos);
			// pListBox->Append(tempLine);

		}
		// GDLC 3SEP13 Put the array of strings into the list box
		pListBox->InsertItems(as, 0);
		if (pTempStr != NULL) // whm 11Jun12 added NULL test
			delete pTempStr;
		// remove the first line of the listbox which should contain
		// "Id <tab> Print_Name"
		if (pListBox->GetCount() > 0)
		{
			wxString firstItemStr = pListBox->GetString(0);
			firstItemStr.Trim(FALSE); // trim left end
			firstItemStr.LowerCase();
			if (firstItemStr.Find(_T("id")) == 0)
			{
				pListBox->Delete(0); // remove the Id ... line from the listbox
			}
		}
		m_curSel = 0;
		if (pListBox->GetCount() > 0)
        {
			pListBox->SetSelection(m_curSel,TRUE);
            pListBox->EnsureVisible(m_curSel);
        }
		// if the user had previously designated a language code, enter it into
		// the appropriate edit box as initial/default value
		if (!m_langCode.IsEmpty())
			pEditLangCode->ChangeValue(m_langCode);
	}
}

// event handling functions

// whm revised 5Dec11 changed name of handler and modified to only search within the
// code part of the list strings (up to the 5 spaces). This routine does a brute
// force linear search through the list. On a fast machine if the search
// string is near the end of the list it can take 7 or 8 seconds, longer on a slower
// machine, but this function is likely to be used only rarely when the code for
// a new language is being determined.
void CLanguageCodesDlg_Single::OnFindCode(wxCommandEvent& WXUNUSED(event))
{
	unsigned int count = pListBox->GetCount();
	// get the text in the edit control
	m_searchString = pEditSearchForLangName->GetValue();
	if (m_searchString.IsEmpty() || count < 2)
	{
		::wxBell();
		return;
	}

	m_searchString.LowerCase();

    // set the initial position to search from
	int nCurSel;
    if (m_bFirstCodeSearch == TRUE)
    {
        // the first time through, search from the beginning of the list
        nCurSel = 0;
        m_bFirstCodeSearch = FALSE;
    }
    else {
        // subsequent times search from the current selection on (like a "find next instance")
        nCurSel= m_curSel;
    }

	nCurSel++; // start search with following item
	unsigned int index;
	bool bFound = FALSE;
	wxString strLabel = _T("");
	// do a little benchmark test of search times in _DEBUG
#ifdef _DEBUG
	wxDateTime dt1 = wxDateTime::Now(),
			   dt2 = wxDateTime::UNow();
#endif
	for (index = nCurSel; index < count; index++)
	{
		//wxCursor(wxCURSOR_WAIT);
		// get the list's label string at index & check for a match; return with the
		// matched item selected; but if not matched, continue to iterate thru the list
		strLabel = pListBox->GetString(index);
		strLabel.LowerCase();
		// whm modified 5Dec11 to only search for codes in the first column (before the 5 spaces)
		const wxString tab5sp = _T("     "); // the tab was replaced by 5 spaces in InitDialog()
		int offset = strLabel.Find(tab5sp);
		wxASSERT(offset != wxNOT_FOUND); // there should be 5 spaces in the string
		strLabel = strLabel.Mid(0,offset); // get substring up to but not including the 5 spaces
		offset = strLabel.Find(m_searchString);
		if (offset == wxNOT_FOUND)
			continue;
		else
		{
			pListBox->SetSelection(index,TRUE);
            pListBox->EnsureVisible(index);
			m_curSel = index;
			bFound = TRUE;
			break;
		}
	} // end of search loop
	//wxCursor(wxNullCursor);
	if (!bFound)
		::wxBell();

#ifdef _DEBUG
		dt1 = dt2;
		dt2 = wxDateTime::UNow();
		wxLogDebug(_T("Find Code executed in %s ms"),
			(dt2 - dt1).Format(_T("%l")).c_str());
#endif
}

// whm revised 5Dec11 changed name of handler and modified to only search within the
// language part of the list strings (following the 5 spaces). This routine does
// a brute force linear search through the list. On a fast machine if the search
// string is near the end of the list it can take 7 or 8 seconds, longer on a slower
// machine, but this function is likely to be used only rarely when the code for
// a new language is being determined.
void CLanguageCodesDlg_Single::OnFindLanguage(wxCommandEvent& WXUNUSED(event))
{
	unsigned int count = pListBox->GetCount();
	// get the text in the edit control
	m_searchString = pEditSearchForLangName->GetValue();
	if (m_searchString.IsEmpty() || count < 2)
	{
		::wxBell();
		return;
	}

	m_searchString.LowerCase();

    // set the initial position to search from
	int nCurSel;
    if (m_bFirstNameSearch == TRUE)
    {
        // the first time through, search from the beginning of the list
        nCurSel = 0;
        m_bFirstNameSearch = FALSE;
    }
    else {
        // subsequent times search from the current selection on (like a "find next instance")
        nCurSel = m_curSel;
    }

	nCurSel++; // start search with following item
	unsigned int index;
	bool bFound = FALSE;
	wxString strLabel = _T("");
	// do a little benchmark test of search times in _DEBUG
#ifdef _DEBUG
	wxDateTime dt1 = wxDateTime::Now(),
			   dt2 = wxDateTime::UNow();
#endif
	for (index = nCurSel; index < count; index++)
	{
		//wxCursor(wxCURSOR_WAIT);
		// get the list's label string at index & check for a match; return with the
		// matched item selected; but if not matched, continue to iterate thru the list
		strLabel = pListBox->GetString(index);
		strLabel.LowerCase();
		// whm modified 5Dec11 to only search for codes in the first column (before the 5 spaces)
		const wxString tab5sp = _T("     "); // the tab was replaced by 5 spaces in InitDialog()
		int offset = strLabel.Find(tab5sp);
		wxASSERT(offset != wxNOT_FOUND); // there should be 5 spaces in the string
		strLabel = strLabel.Mid(offset+tab5sp.Length()); // get substring up to but not including the tab
		offset = strLabel.Find(m_searchString);
		if (offset == wxNOT_FOUND)
			continue;
		else
		{
			pListBox->SetSelection(index,TRUE);
            pListBox->EnsureVisible(index);
			m_curSel = index;
			bFound = TRUE;
			break;
		}
	} // end of search loop
	//wxCursor(wxNullCursor);
	if (!bFound)
		::wxBell();

#ifdef _DEBUG
		dt1 = dt2;
		dt2 = wxDateTime::UNow();
		wxLogDebug(_T("Find Language executed in %s ms"),
			(dt2 - dt1).Format(_T("%l")).c_str());
#endif
}


void CLanguageCodesDlg_Single::OnEnterInSearchBox(wxCommandEvent& WXUNUSED(event))
{
	unsigned int count = pListBox->GetCount();
	// get the text in the edit control
	m_searchString = pEditSearchForLangName->GetValue();
	if (m_searchString.IsEmpty() || pListBox->GetCount() < 2)
	{
		::wxBell();
		return;
	}

	m_searchString.LowerCase();

	// for an ordinary search start at the first list position
	int nCurSel = 0;
	unsigned int index;
	bool bFound = FALSE;
	wxString strLabel = _T("");

	// do a little benchmark test of search times in _DEBUG
#ifdef _DEBUG
	wxDateTime dt1 = wxDateTime::Now(),
			   dt2 = wxDateTime::UNow();
#endif
	for (index = nCurSel; index < count; index++)
	{
		//wxCursor(wxCURSOR_WAIT);
		// get the list's label string at index & check for a match; return with the
		// matched item selected; but if not matched, continue to iterate thru the list
		strLabel = pListBox->GetString(index);
		strLabel.LowerCase();
		int offset = strLabel.Find(m_searchString);
		if (offset == wxNOT_FOUND)
			continue;
		else
		{
			pListBox->SetSelection(index,TRUE);
			m_curSel = index;
			bFound = TRUE;
			break;
		}
	} // end of search loop
	//wxCursor(wxNullCursor);
	if (!bFound)
		::wxBell();

#ifdef _DEBUG
		dt1 = dt2;
		dt2 = wxDateTime::UNow();
		wxLogDebug(_T("Search executed in %s ms"),
			(dt2 - dt1).Format(_T("%l")).c_str());
#endif

}

void CLanguageCodesDlg_Single::OnSelchangeListboxLanguageCodes(wxCommandEvent& WXUNUSED(event))
{
	// wx note: Under Linux/GTK ...Selchanged... listbox events can be triggered after a call to Clear()
	// so we must check to see if the listbox contains no items and if so return immediately
	//if (pListBox->GetCount() == 0)
	if (!ListBoxPassesSanityCheck((wxControlWithItems*)pListBox))
		return;

	m_curSel = pListBox->GetSelection();
	if (m_curSel != wxNOT_FOUND)
	{
		// enable appropriate buttons
		pBtnUseSelectionAsCode->Enable();
	}
}

void CLanguageCodesDlg_Single::OnUseSelectedCodeForCode(wxCommandEvent& WXUNUSED(event))
{
	pEditLangCode->ChangeValue(Get3LetterCodeFromLBItem());
}


// OnOK() calls wxWindow::Validate, then wxWindow::TransferDataFromWindow.
// If this returns TRUE, the function either calls EndModal(wxID_OK) if the
// dialog is modal, or sets the return value to wxID_OK and calls Show(FALSE)
// if the dialog is modeless.
void CLanguageCodesDlg_Single::OnOK(wxCommandEvent& event)
{
	m_langCode = pEditLangCode->GetValue();

	event.Skip();
}

wxString CLanguageCodesDlg_Single::Get3LetterCodeFromLBItem()
{
	m_curSel = pListBox->GetSelection();
	wxASSERT(m_curSel != wxNOT_FOUND);
	wxString tempStr = pListBox->GetString(m_curSel);

	// return the language code
	return tempStr.Mid(0,3);
}

// other class methods

