/////////////////////////////////////////////////////////////////////////////
/// \project		adaptit
/// \file			FindReplace.cpp
/// \author			Bill Martin
/// \date_created	24 July 2006
/// \rcs_id $Id$
/// \copyright		2008 Bruce Waters, Bill Martin, SIL International
/// \license		The Common Public License or The GNU Lesser General Public License (see license directory)
/// \description	This is the implementation file for two classes: CFindDlg and CReplaceDlg.
/// These classes provide a find and/or find and replace dialog in which the user can find 
/// the location(s) of specified source and/or target text. The dialog has an "Ignore case" 
/// checkbox, a "Special Search" button, and other options. The replace fdialog allows the 
/// user to specify a replacement string.
/// Both CFindDlg and CReplaceDlg are created as a Modeless dialogs. They are created on 
/// the heap and are displayed with Show(), not ShowModal().
/// \derivation		The CFindDlg and CReplaceDlg classes are derived from wxScrollingDialog 
/// when built with wxWidgets prior to version 2.9.x, but derived from wxDialog for version 
/// 2.9.x and later.
/////////////////////////////////////////////////////////////////////////////

// BEW 26Mar10, these classes are updated for support of doc version 5

// the following improves GCC compilation performance
#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma implementation "FindReplace.h"
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
#include <wx/docview.h> // needed for classes that reference wxView or wxDocument
#include <wx/valgen.h> // for wxGenericValidator

#include "Adapt_It.h"
#include "KB.h"
#include "FindReplace.h"
#include "Adapt_ItView.h"
#include "Cell.h"
#include "Pile.h"
#include "Strip.h"
#include "SourcePhrase.h"
#include "Layout.h"
#include "Adapt_ItDoc.h"
#include "MainFrm.h" // whm added 24Jul06
#include "Adapt_ItCanvas.h" // whm added 24Jul06
#include "helpers.h"
#include "MyListBox.h"

// next two are for version 2.0 which includes the option of a 3rd line for glossing

/// This global is defined in Adapt_ItView.cpp.
extern bool	gbIsGlossing; // when TRUE, the phrase box and its line have glossing text

/// This global is defined in Adapt_ItView.cpp.
extern bool	gbGlossingVisible; // TRUE makes Adapt It revert to Shoebox functionality only

/// This global is defined in Adapt_ItView.cpp.
extern bool gbGlossingUsesNavFont;

/// This global is defined in Adapt_It.cpp.
extern wxChar	gSFescapechar;

extern bool		gbFind;
extern bool		gbFindIsCurrent;
extern bool		gbJustReplaced;
extern int		gnRetransEndSequNum;
extern wxString	gOldConcatStr;
extern wxString	gOldConcatStrNoPunct;
extern bool		gbFindOrReplaceCurrent;
extern bool		gbSaveSuppressFirst; // save the toggled state of the lines in the strips (across Find or
extern bool		gbSaveSuppressLast;  // Find and Replace operations)

/// This global is defined in Adapt_It.cpp.
extern CAdapt_ItApp* gpApp; // if we want to access it fast

// for ReplaceAll support
bool	gbFound; // TRUE if Find Next succeeded in making a match, else FALSE

/// TRUE if a Find and Replace operation is current, otherwise FALSE
bool	gbReplaceAllIsCurrent = FALSE;
bool	gbJustCancelled = FALSE;


// the CFindDlg class //////////////////////////////////////////////////////////////////////

// whm 14Jun12 modified to use wxDialog for wxWidgets 2.9.x and later; wxScrollingDialog for pre-2.9.x
#if wxCHECK_VERSION(2,9,0)
IMPLEMENT_DYNAMIC_CLASS( CFindDlg, wxDialog )
#else
IMPLEMENT_DYNAMIC_CLASS( CFindDlg, wxScrollingDialog )
#endif

// event handler table
// whm 14Jun12 modified to use wxDialog for wxWidgets 2.9.x and later; wxScrollingDialog for pre-2.9.x
#if wxCHECK_VERSION(2,9,0)
BEGIN_EVENT_TABLE(CFindDlg, wxDialog)
#else
BEGIN_EVENT_TABLE(CFindDlg, wxScrollingDialog)
#endif
	EVT_INIT_DIALOG(CFindDlg::InitDialog)
	EVT_BUTTON(wxID_CANCEL, CFindDlg::OnCancel)	
	
	EVT_COMBOBOX(IDC_COMBO_SFM, CFindDlg::OnSelchangeComboSfm)
	EVT_BUTTON(IDC_BUTTON_SPECIAL, CFindDlg::OnButtonSpecial)
    
    // whm 14May2020 added the following EVT_TEXT_ENTER() lines to intercept and prevent
    // an ENTER key press inside the "Source Text" and/or "Translation" edit boxes from
    // propagating to the phrasebox in the main window and causing the phrasebox to move 
    // on from the found location. 
    // The OnSrcEditBoxEnterKeyPress() and OnTgtEditBoxEnterKeyPress() handlers simply 
    // call the OnFindNext() handler with a dummy event, and then return without calling 
    // event.Skip().
    // whm 14May2020 followup note: The following handlers work sometimes and not at other
    // times. They tend to work well when the debugger stops the flow of control at a 
    // breakpoint within the handlers and then allowed program control to resume.
    EVT_TEXT_ENTER(IDC_EDIT_SRC_FIND, CFindDlg::OnSrcEditBoxEnterKeyPress)
    EVT_TEXT_ENTER(IDC_EDIT_TGT_FIND, CFindDlg::OnTgtEditBoxEnterKeyPress)

	EVT_BUTTON(wxID_OK, CFindDlg::OnFindNext)
	EVT_BUTTON(ID_BUTTON_RESTORE_DEFAULTS, CFindDlg::OnRestoreFindDefaults)
	EVT_RADIOBUTTON(IDC_RADIO_SRC_ONLY_FIND, CFindDlg::OnRadioSrcOnly)
	EVT_RADIOBUTTON(IDC_RADIO_TGT_ONLY_FIND, CFindDlg::OnRadioTgtOnly)
	EVT_RADIOBUTTON(IDC_RADIO_SRC_AND_TGT_FIND, CFindDlg::OnRadioSrcAndTgt)
	EVT_RADIOBUTTON(IDC_RADIO_RETRANSLATION, CFindDlg::OnRadioRetranslation)
	EVT_RADIOBUTTON(IDC_RADIO_NULL_SRC_PHRASE, CFindDlg::OnRadioNullSrcPhrase)
	EVT_RADIOBUTTON(IDC_RADIO_SFM, CFindDlg::OnRadioSfm)
END_EVENT_TABLE()
	
// these are unique to CFindDlg

CFindDlg::CFindDlg()
{
}

CFindDlg::~CFindDlg()
{
	gbFind = FALSE;
}

// BEW 26Mar10, no changes needed for support of doc version 5
CFindDlg::CFindDlg(wxWindow* parent) // dialog constructor
// whm 14Jun12 modified to use wxDialog for wxWidgets 2.9.x and later; wxScrollingDialog for pre-2.9.x
#if wxCHECK_VERSION(2,9,0)
	: wxDialog(parent, -1, _("Find"),
		wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
#else
	: wxScrollingDialog(parent, -1, _("Find"),
		wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
#endif
{
	// This dialog function below is generated in wxDesigner, and defines the controls and sizers
	// for the dialog. The first parameter is the parent which should normally be "this".
	// The second and third parameters should both be TRUE to utilize the sizers and create the right
	// size dialog.
	pFindDlgSizer = FindDlgFunc(this, TRUE, TRUE);
	// The declaration is: FindDlgFunc( wxWindow *parent, bool call_fit, bool set_sizer );

    // whm 5Mar2019 Since wxStdDialogButtonsSizer is not used in the FindDlgFunc(),
    // we need to call App's ReverseOkCancelButtonsForMac() function to reverse the "Find Next" (wxID_OK) 
    // and "Cancel" buttons.
	bool bOK;
	bOK = gpApp->ReverseOkCancelButtonsForMac(this); 
	bOK = bOK; // avoid warning (BEW 2Jan12, retain this)
	m_marker = -1;
	m_srcStr = _T("");
	m_replaceStr = _T("");
	m_tgtStr = _T("");
	m_bIncludePunct = FALSE;
	m_bSpanSrcPhrases = FALSE;
	m_bIgnoreCase = FALSE;

	// Get pointers to dialog controls created in FindDlgFunc() [by wxDesigner]
	
	m_pRadioSrcTextOnly = (wxRadioButton*)FindWindowById(IDC_RADIO_SRC_ONLY_FIND);
	wxASSERT(m_pRadioSrcTextOnly != NULL);

	m_pRadioTransTextOnly = (wxRadioButton*)FindWindowById(IDC_RADIO_TGT_ONLY_FIND);
	wxASSERT(m_pRadioTransTextOnly != NULL);

	m_pRadioBothSrcAndTransText = (wxRadioButton*)FindWindowById(IDC_RADIO_SRC_AND_TGT_FIND);
	wxASSERT(m_pRadioBothSrcAndTransText != NULL);

	m_pCheckIgnoreCase = (wxCheckBox*)FindWindowById(IDC_CHECK_IGNORE_CASE_FIND);
	wxASSERT(m_pCheckIgnoreCase != NULL);
	//m_pCheckIgnoreCase->SetValidator(wxGenericValidator(&m_bIgnoreCase)); // use validator

	m_pFindNext = (wxButton*)FindWindow(wxID_OK); // just use FindWindow here to limit search to CFindDlg
	wxASSERT(m_pFindNext != NULL);

	m_pStaticSrcBoxLabel = (wxStaticText*)FindWindowById(IDC_STATIC_SRC_FIND);
	wxASSERT(m_pStaticSrcBoxLabel != NULL);

	m_pEditSrc = (wxTextCtrl*)FindWindowById(IDC_EDIT_SRC_FIND);
	wxASSERT(m_pEditSrc != NULL);
	//m_pEditSrc->SetValidator(wxGenericValidator(&m_srcStr)); // use validator

	m_pStaticTgtBoxLabel = (wxStaticText*)FindWindowById(IDC_STATIC_TGT_FIND);
	wxASSERT(m_pStaticTgtBoxLabel != NULL);

	// whm 31Aug2021 modified line below to use the AutoCorrectTextCtrl class which is now
	// used as a custom control in wxDesigner's FindDlgFunc() dialog.
	m_pEditTgt = (AutoCorrectTextCtrl*)FindWindowById(IDC_EDIT_TGT_FIND);
	wxASSERT(m_pEditTgt != NULL);
	//m_pEditTgt->SetValidator(wxGenericValidator(&m_tgtStr)); // use validator

	m_pButtonSpecialNormal = (wxButton*)FindWindowById(IDC_BUTTON_SPECIAL);
	wxASSERT(m_pButtonSpecialNormal != NULL);

	m_pCheckIncludePunct = (wxCheckBox*)FindWindowById(IDC_CHECK_INCLUDE_PUNCT_FIND);
	wxASSERT(m_pCheckIncludePunct != NULL);
	//m_pCheckIncludePunct->SetValidator(wxGenericValidator(&m_bIncludePunct)); // use validator

	m_pCheckSpanSrcPhrases = (wxCheckBox*)FindWindowById(IDC_CHECK_SPAN_SRC_PHRASES_FIND);
	wxASSERT(m_pCheckSpanSrcPhrases != NULL);
	//m_pCheckSpanSrcPhrases->SetValidator(wxGenericValidator(&m_bSpanSrcPhrases)); // use validator

	m_pSpecialSearches = (wxStaticText*)FindWindowById(IDC_STATIC_SPECIAL);
	wxASSERT(m_pSpecialSearches != NULL);

	m_pSelectAnSfm = (wxStaticText*)FindWindowById(IDC_STATIC_SELECT_MKR);
	wxASSERT(m_pSelectAnSfm != NULL);

	m_pFindRetranslation = (wxRadioButton*)FindWindowById(IDC_RADIO_RETRANSLATION);
	wxASSERT(m_pFindRetranslation != NULL);
	//m_pFindRetranslation->SetValidator(wxGenericValidator(&m_bFindRetranslation)); // use validator

	m_pFindPlaceholder = (wxRadioButton*)FindWindowById(IDC_RADIO_NULL_SRC_PHRASE);
	wxASSERT(m_pFindPlaceholder != NULL);
	//m_pFindPlaceholder->SetValidator(wxGenericValidator(&m_bFindNullSrcPhrase)); // use validator

	m_pFindSFM = (wxRadioButton*)FindWindowById(IDC_RADIO_SFM);
	wxASSERT(m_pFindSFM != NULL);
	//m_pFindSFM->SetValidator(wxGenericValidator(&m_bFindSFM)); // use validator

	m_pComboSFM = (wxComboBox*)FindWindowById(IDC_COMBO_SFM); 
	wxASSERT(m_pComboSFM != NULL);
	//m_pComboSFM->SetValidator(wxGenericValidator(&m_marker)); // use validator
}

// BEW 26Mar10, no changes needed for support of doc version 5
void CFindDlg::InitDialog(wxInitDialogEvent& WXUNUSED(event)) // InitDialog is method of wxWindow
{
	gbFound = FALSE;
	gbReplaceAllIsCurrent = FALSE;
	gbJustCancelled = FALSE;

    // whm 15May2020 added
    m_bSearchFromDocBeginning = FALSE;

	// make the font show user-chosen point size in the dialog
	#ifdef _RTL_FLAGS
	gpApp->SetFontAndDirectionalityForDialogControl(gpApp->m_pSourceFont, m_pEditSrc, NULL,
								NULL, NULL, gpApp->m_pDlgSrcFont, gpApp->m_bSrcRTL);
	#else // Regular version, only LTR scripts supported, so use default FALSE for last parameter
	gpApp->SetFontAndDirectionalityForDialogControl(gpApp->m_pSourceFont, m_pEditSrc, NULL, 
								NULL, NULL, gpApp->m_pDlgSrcFont);
	#endif


	// for glossing or adapting
	if (gbIsGlossing && gbGlossingUsesNavFont)
	{
		#ifdef _RTL_FLAGS
		gpApp->SetFontAndDirectionalityForDialogControl(gpApp->m_pNavTextFont, m_pEditTgt, NULL,
									NULL, NULL, gpApp->m_pDlgTgtFont, gpApp->m_bNavTextRTL);
		#else // Regular version, only LTR scripts supported, so use default FALSE for last parameter
		gpApp->SetFontAndDirectionalityForDialogControl(gpApp->m_pNavTextFont, m_pEditTgt, NULL, 
									NULL, NULL, gpApp->m_pDlgTgtFont);
		#endif
	}
	else
	{
		#ifdef _RTL_FLAGS
		gpApp->SetFontAndDirectionalityForDialogControl(gpApp->m_pTargetFont, m_pEditTgt, NULL,
									NULL, NULL, gpApp->m_pDlgTgtFont, gpApp->m_bTgtRTL);
		#else // Regular version, only LTR scripts supported, so use default FALSE for last parameter
		gpApp->SetFontAndDirectionalityForDialogControl(gpApp->m_pTargetFont, m_pEditTgt, NULL, 
									NULL, NULL, gpApp->m_pDlgTgtFont);
		#endif
	}

	// set default checkbox values
	m_bIncludePunct = FALSE;
	m_bSpanSrcPhrases = FALSE;

	// hide the special stuff
	m_bSpecialSearch = FALSE;

	wxASSERT(m_pButtonSpecialNormal != NULL);
	wxString str;
	str = _("Special Search"); //str.Format(IDS_SPECIAL_SEARCH);
	m_pButtonSpecialNormal->SetLabel(str);
	
	wxASSERT(m_pFindRetranslation != NULL);
	m_pFindRetranslation->Hide();
	// pButton->Enable(FALSE);
	
	wxASSERT(m_pFindPlaceholder != NULL);
	m_pFindPlaceholder->Hide();
	// pButton->Enable(FALSE);
	
	wxASSERT(m_pFindSFM != NULL);
	m_pFindSFM->Hide();
	// pButton->Enable(FALSE);
	
	wxASSERT(m_pComboSFM != NULL);
	m_pComboSFM->Hide();
	
	wxASSERT(m_pSelectAnSfm != NULL);
	m_pSelectAnSfm->Hide();
	
	wxASSERT(m_pSpecialSearches != NULL);
	m_pSpecialSearches->Hide();

	// if glossing is ON, the "Search, retaining target text's punctuation" and
	// "Allow the search to occur in the text spanning multiple piles" have to be
	// hidden; since merging is not permitted and only the gloss line can be accessed
	// as the 'target'
	if (gbIsGlossing)
	{
		wxASSERT(m_pCheckIncludePunct != NULL);
		m_pCheckIncludePunct->Hide();
		
		wxASSERT(m_pCheckSpanSrcPhrases != NULL);
		m_pCheckSpanSrcPhrases->Hide();
	}

	// get the normal defaults set up
	wxASSERT(m_pRadioSrcTextOnly != NULL);
	m_pRadioSrcTextOnly->SetValue(TRUE);
	
	wxASSERT(m_pStaticTgtBoxLabel != NULL);
	m_pStaticTgtBoxLabel->Hide();

    // whm 14May2020 added set focus in the m_pEditSrc edit box
    m_pEditSrc->SetFocus();

	wxASSERT(m_pEditTgt != NULL);
	m_pEditTgt->Hide();

	// whm added 17Feb05 in support of USFM and SFM Filtering
	// Hard coded sfm/descriptions strings were deleted from the Data
	// Properties attribute of the IDC_COMBO_SFM control in the
	// IDD_FIND_REPLACE dialog. The combox Sort attribute was also set.
	// Here we populate m_comboSFM combo box with the appropriate sfms 
	// depending on gCurrentSfmSet.
	// wx revision of behavior: We should list only those sfms which
	// are actually used in the currently open document (including any
	// unknown markers).
	MapSfmToUSFMAnalysisStruct::iterator iter;
	USFMAnalysis* pSfm;
	wxString key;
	wxString cbStr;
	m_pComboSFM->Clear(); // whm added 26Oct08
    // whm modified 21Nov11. On Linux, the combobox is not sorted even though the m_pComboSFM
    // has the wxCB_SORT style flag set. So, I'll put the markers in a wxArrayString first,
    // sort it, and then load the wxArrayString items into the combobox.
    wxArrayString comboItemsArray;
	comboItemsArray.Clear();
	// wx version: I've modified the m_pComboSFM->Append() routines to only put sensible
    // sfms in the combo box, i.e., none of the _basestyle markers or others that are
    // merely used internally by Adapt It are now included.
	switch(gpApp->gCurrentSfmSet)
	{
	case UsfmOnly:
		{
			for (iter = gpApp->m_pUsfmStylesMap->begin(); 
				iter != gpApp->m_pUsfmStylesMap->end(); ++iter)
			{
				// Retrieve each USFMAnalysis struct from the map
				key = iter->first;
				pSfm = iter->second;
				if (pSfm->marker.Find(_T('_')) == 0) // skip it if it is a marker beginning with _
					continue;
				cbStr = gSFescapechar;
				cbStr += pSfm->marker;
				cbStr += _T("  ");
				cbStr += pSfm->description;
				comboItemsArray.Add(cbStr); //m_pComboSFM->Append(cbStr);
			}
			break;
		}
	case PngOnly:
		{
			for (iter = gpApp->m_pPngStylesMap->begin(); 
				iter != gpApp->m_pPngStylesMap->end(); ++iter)
			{
				// Retrieve each USFMAnalysis struct from the map
				key = iter->first;
				pSfm = iter->second;
				if (pSfm->marker.Find(_T('_')) == 0) // skip it if it is a marker beginning with _
					continue;
				cbStr = gSFescapechar;
				cbStr += pSfm->marker;
				cbStr += _T("  ");
				cbStr += pSfm->description;
				comboItemsArray.Add(cbStr); //m_pComboSFM->Append(cbStr);
			}
			break;
		}
	case UsfmAndPng:
		{
			for (iter = gpApp->m_pUsfmAndPngStylesMap->begin(); 
				iter != gpApp->m_pUsfmAndPngStylesMap->end(); ++iter)
			{
				// Retrieve each USFMAnalysis struct from the map
				key = iter->first;
				pSfm = iter->second;
				if (pSfm->marker.Find(_T('_')) == 0) // skip it if it is a marker beginning with _
					continue;
				cbStr = gSFescapechar;
				cbStr += pSfm->marker;
				cbStr += _T("  ");
				cbStr += pSfm->description;
				comboItemsArray.Add(cbStr); //m_pComboSFM->Append(cbStr);
			}
			break;
		}
	default:
		{
			// this should never happen
			for (iter = gpApp->m_pUsfmStylesMap->begin(); 
				iter != gpApp->m_pUsfmStylesMap->end(); ++iter)
			{
				// Retrieve each USFMAnalysis struct from the map
				key = iter->first;
				pSfm = iter->second;
				if (pSfm->marker.Find(_T('_')) == 0) // skip it if it is a marker beginning with _
					continue;
				cbStr = gSFescapechar;
				cbStr += pSfm->marker;
				cbStr += _T("  ");
				cbStr += pSfm->description;
				comboItemsArray.Add(cbStr); //m_pComboSFM->Append(cbStr);
			}
		}
	}
	// now sort the array and add it to the m_pComboSFM combobox
	comboItemsArray.Sort();
	m_pComboSFM->Append(comboItemsArray);

	// set the default button to Find Next button explicitly (otherwise, an MFC bug makes it
	// the Replace All button)
	wxASSERT(m_pFindNext != NULL);
	m_pFindNext->SetDefault(); // ID for Find Next button

	m_nCount = 0; // nothing matched yet

    // whm 15May2020 added below to supress phrasebox run-on due to handling of ENTER in CPhraseBox::OnKeyUp()
    gpApp->m_bUserDlgOrMessageRequested = TRUE;

	pFindDlgSizer->Layout(); // force the sizers to resize the dialog
}
// initdialog here above

// BEW 26Mar10, no changes needed for support of doc version 5
// BEW 17Jul11, changed for GetRefString() to return KB_Entry enum, and use all 10 maps
// for glossing KB
// BEW 23Apr15 added support for using / as a word-breaking whitespace character
// whm 14May2020 revised to support searching from beginning of document when a search
// reaches the end of the document and doesn't find the search string. Also revised to
// use m_bUserDlgOrMessageRequested to help prevent run on phrasebox while interacting
// with the dialog.
void CFindDlg::DoFindNext() 
{
	// this handles the wxID_OK special identifier assigned to the "Find Next" button
	CAdapt_ItView* pView = gpApp->GetView();
	pView->RemoveSelection();
	CAdapt_ItDoc* pDoc = gpApp->GetDocument();


    int nKickOffSequNum = gpApp->m_pActivePile->GetSrcPhrase()->m_nSequNumber; // could be 0 if active pile is at 0

repeatfind:

	// BEW 27Mar21 write the cached values into the config dialog, if the struct
	// CacheFindReplaceConfig has it's bool bFindDlg set TRUE (it's TRUE after a
	// Find or Find Replace has been initiated, and stays TRUE until user cancels, 
	// whereupon its FALSE). Reading the cache in order to restore the cached values
	// to the next OnFind(), etc, call so that the config dialog reappears with values
	// unchanged is done at the start of the OnFind() call in the View class after
	// line 19,138 (near start of the function)
	bool bWriteStructCache = gpApp->WriteFindCache();
	wxUnusedVar(bWriteStructCache);

	// whm 15May2020 Note: gpApp->m_bUserDlgOrMessageRequested is set to TRUE in this->InitDialog()
    // but it becomes FALSE again (by code elsewhere) after the first Find Next is done. 
    // For modeless dialogs - which can execute ENTER-generated commands multiple times
    // setting gpApp->m_bUserDlgOrMessageRequested to TRUE in the dialog's InitDialog() only
    // is not sufficient. Here in CFind, it needs to be set to TRUE in each DoFindNext() call 
    // in order to supress all phrasebox run-on instances due to ENTER key getting 
    // propagated on to CPhraseBox::OnKeyUp().
    gpApp->m_bUserDlgOrMessageRequested = TRUE;

	// do nothing if past the end of the last srcPhrase, ie. at the EOF
	if (gpApp->m_nActiveSequNum == -1)
	{	
		wxASSERT(gpApp->m_pActivePile == 0);
		::wxBell();
		return;
	}

    if (gbJustReplaced)
    {
        // do all the housekeeping tasks associated with a move, after a replace was done
		// if the targetBox is visible, store the contents in KB since we will advance the
		// active location potentially elsewhere
		// whm Note 12Aug08. The following should be OK even though in the wx version the
		// m_pTargetBox is never NULL (because of the IsShown() test which constrains the
		// logic to be the same as in the MFC version.
		if (gpApp->m_pTargetBox != NULL)
		{
			if (gpApp->m_pTargetBox->IsShown())
			{
				if (!gbIsGlossing)
				{
					pView->MakeTargetStringIncludingPunctuation(gpApp->m_pActivePile->GetSrcPhrase(),
											gpApp->m_targetPhrase);
					pView->RemovePunctuation(pDoc,&gpApp->m_targetPhrase,from_target_text);
				}
                // the store will fail if the user edited the entry out of the KB, as the
                // latter cannot know which srcPhrases will be affected, so these will
                // still have their m_bHasKBEntry set true. We have to test for this, ie. a
                // null pRefString or rsEntry value of present_but_deleted, but the above
                // flag TRUE, is a sufficient test, and if so, set the flag to FALSE
				CRefString* pRefStr = NULL;
				KB_Entry rsEntry;
				bool bOK;
				if (gbIsGlossing)
				{
					rsEntry = gpApp->m_pGlossingKB->GetRefString(gpApp->m_pActivePile->GetSrcPhrase()->m_nSrcWords,
						gpApp->m_pActivePile->GetSrcPhrase()->m_key, gpApp->m_targetPhrase, pRefStr);
					if ((pRefStr == NULL || rsEntry == present_but_deleted) && 
						gpApp->m_pActivePile->GetSrcPhrase()->m_bHasGlossingKBEntry)
					{
						gpApp->m_pActivePile->GetSrcPhrase()->m_bHasGlossingKBEntry = FALSE;
					}
					bOK = gpApp->m_pGlossingKB->StoreText(gpApp->m_pActivePile->GetSrcPhrase(), gpApp->m_targetPhrase);
					bOK = bOK; // avoid warning
				}
				else
				{
					rsEntry = gpApp->m_pKB->GetRefString(gpApp->m_pActivePile->GetSrcPhrase()->m_nSrcWords,
						gpApp->m_pActivePile->GetSrcPhrase()->m_key, gpApp->m_targetPhrase, pRefStr);
					if ((pRefStr == NULL || rsEntry == present_but_deleted) && 
						gpApp->m_pActivePile->GetSrcPhrase()->m_bHasKBEntry)
					{
						gpApp->m_pActivePile->GetSrcPhrase()->m_bHasKBEntry = FALSE;
					}
					//gpApp->m_bInhibitMakeTargetStringCall = TRUE;
					bOK = gpApp->m_pKB->StoreText(gpApp->m_pActivePile->GetSrcPhrase(), gpApp->m_targetPhrase);
					bOK = bOK; // avoid warning
					gpApp->m_bInhibitMakeTargetStringCall = TRUE;
				}
			}
		}

		// now we can get rid of the phrase box till wanted again
		// wx version just hides the phrase box
        gpApp->m_pTargetBox->HidePhraseBox(); // hides all three parts of the new phrasebox

		gpApp->m_pTargetBox->GetTextCtrl()->ChangeValue(_T("")); // need to set it to null str since it 
											   // won't get recreated
		gpApp->m_targetPhrase = _T("");

        // did we just replace in a retranslation, if so, reduce the kick off sequ num by
        // one otherwise anything matchable in the first srcPhrase after the retranslation
        // will not get matched (ie. check if the end of a retranslation precedes the
        // current location and if so, we'll want to make that the place we move forward
        // from so that we actually try to match in the first pile after the retranslation)
        // We don't need the adjustment if glossing is ON however.
		int nEarlierSN = nKickOffSequNum -1;
		CPile* pPile = pView->GetPile(nEarlierSN);
		bool bRetrans = pPile->GetSrcPhrase()->m_bRetranslation;
		if ( !gbIsGlossing && bRetrans)
			nKickOffSequNum = nEarlierSN;

		gbJustReplaced = FALSE;
		m_nCount = 0; // nothing currently matched
	}

    // in some situations (eg. after a merge in a replacement) a LayoutStrip call is
    // needed, otherwise the destruction of the targetBox window will leave an empty white
    // space at the active loc.
#ifdef _NEW_LAYOUT
		gpApp->m_pLayout->RecalcLayout(gpApp->m_pSourcePhrases, keep_strips_keep_piles);
#else
		gpApp->m_pLayout->RecalcLayout(gpApp->m_pSourcePhrases, create_strips_keep_piles);
#endif

	// restore the pointers which were clobbered
	CPile* pPile = pView->GetPile(gpApp->m_nActiveSequNum);
	gpApp->m_pActivePile = pPile;

	// BEW 30Mar21, I have split m_bFindRetranslation (for a Special Search)
	// from a new boolean, m_bFindRetransln (for Normal searching),
	// so ensure the former is FALSE, till a Special Search is
	// invoked for searching for a retranslation
	m_bFindRetranslation = FALSE;
	m_pFindRetranslation->SetValue(m_bFindRetranslation);

	//TransferDataFromWindow(); // whm removed 21Nov11
	// whm added below 21Nov11 replacing TransferDataFromWindow()
	m_bIgnoreCase = m_pCheckIgnoreCase->GetValue();
	m_srcStr = m_pEditSrc->GetValue();
	m_tgtStr = m_pEditTgt->GetValue();
//#if defined(FWD_SLASH_DELIM)
	// BEW added 23Apr15
	m_srcStr = FwdSlashtoZWSP(m_srcStr);
	m_tgtStr = FwdSlashtoZWSP(m_tgtStr);
//#endif
	m_bIncludePunct = m_pCheckIncludePunct->GetValue();
	m_bSpanSrcPhrases = m_pCheckSpanSrcPhrases->GetValue();
	m_bFindRetranslation = m_pFindRetranslation->GetValue();
	m_bFindNullSrcPhrase = m_pFindPlaceholder->GetValue();
	m_bFindSFM = m_pFindSFM->GetValue();
	m_marker = m_pComboSFM->GetSelection();

	int nAtSequNum = gpApp->m_nActiveSequNum;
	int nCount = 1;

	gbFindIsCurrent = TRUE; // turn it back off after the active location cell is drawn

	// do the Find Next operation
	bool bFound;
	bool bWhichFindRetrans = m_bFindRetransln; // default, for 'Normal" scenario
	if (m_bSpecialSearch) // IF TRUE, user has asked for a search for retranslations, or placeholders, or USfMs
	{
		//CFindDlg* pDlg = gpApp->m_pFindDlg;
		// Find what the Retranslation radio button is, on or off. This is
		// tricky because the first radio button will be ticked, but
		// m_bFindRetranslation is still false. So we do it by process of
		// elimination. Set it true if the other two radio buttons are false
		if ((m_bFindNullSrcPhrase == FALSE) && (m_bFindSFM == FALSE))
		{
			m_bFindRetranslation = TRUE;
		}
		// The other two, will get values set beforehand, so don't need
		// special check here on
		if (m_bFindRetranslation)
		{
			bWhichFindRetrans = m_bFindRetranslation; // use this RHSide
			bWhichFindRetrans = TRUE; // we want finding of a retranslation
		}
		else if (m_bFindNullSrcPhrase)
		{
			bWhichFindRetrans = FALSE;
			m_bFindSFM = FALSE; // redundant, but no harm
		}
		else if (m_bFindSFM)
		{
			bWhichFindRetrans = FALSE;
			m_bFindNullSrcPhrase = FALSE;
		}
	}
	gpApp->LogUserAction(_T("pView->DoFindNext() executed in CFindDlg"));
	bFound = pView->DoFindNext(nKickOffSequNum,
									m_bIncludePunct,
									m_bSpanSrcPhrases,
									m_bSpecialSearch,
									m_bSrcOnly,
									m_bTgtOnly,
									m_bSrcAndTgt,
									bWhichFindRetrans,
									m_bFindNullSrcPhrase,
									m_bFindSFM,
									m_srcStr,
									m_tgtStr,
									m_sfm,
									m_bIgnoreCase,
									nAtSequNum,
									nCount);

	gbJustReplaced = FALSE;

	if (bFound)
	{
		gbFound = TRUE; // set the global
		gpApp->LogUserAction(_T("   Found!"));
		// inform the dialog about how many were matched
		m_nCount = nCount;

		// place the dialog window so as not to obscure things
		// work out where to place the dialog window
		m_nTwoLineDepth = 2 * gpApp->m_nTgtHeight;
		if (gbGlossingVisible)
		{
			if (gbGlossingUsesNavFont)
				m_nTwoLineDepth += gpApp->m_nNavTextHeight;
			else
				m_nTwoLineDepth += gpApp->m_nTgtHeight;
		}
		m_ptBoxTopLeft = gpApp->m_pActivePile->GetCell(1)->GetTopLeft();
		wxRect rectScreen;
		rectScreen = wxGetClientDisplayRect(); // a wx global call

		wxClientDC dc(gpApp->GetMainFrame()->canvas);
		pView->canvas->DoPrepareDC(dc); // adjust origin
		gpApp->GetMainFrame()->PrepareDC(dc); // wxWidgets' drawing.cpp sample 
									// also calls PrepareDC on the owning frame

		if (!gbIsGlossing && gpApp->m_bMatchedRetranslation)
		{
			// use end of retranslation
			CCellList::Node* cpos = gpApp->m_selection.GetLast();
			CCell* pCell = (CCell*)cpos->GetData();
			wxASSERT(pCell != NULL);
			CPile* pPile = pCell->GetPile();
			pCell = pPile->GetCell(1); // last line
			m_ptBoxTopLeft = pCell->GetTopLeft();

			int newXPos,newYPos;
			// CalcScrolledPosition translates logical coordinates to device ones. 
			gpApp->GetMainFrame()->canvas->CalcScrolledPosition(m_ptBoxTopLeft.x,
												m_ptBoxTopLeft.y,&newXPos,&newYPos);
			m_ptBoxTopLeft.x = newXPos;
			m_ptBoxTopLeft.y = newYPos;
		}
		else
		{
			// use location where phrase box would be put
			int newXPos,newYPos;
			// CalcScrolledPosition translates logical coordinates to device ones. 
			gpApp->GetMainFrame()->canvas->CalcScrolledPosition(m_ptBoxTopLeft.x,
												m_ptBoxTopLeft.y,&newXPos,&newYPos);
			m_ptBoxTopLeft.x = newXPos;
			m_ptBoxTopLeft.y = newYPos;
		}
		gpApp->GetMainFrame()->canvas->ClientToScreen(&m_ptBoxTopLeft.x,
									&m_ptBoxTopLeft.y); // now it's screen coords
		// BEW 16Sep11, replaced code below with these calls from helpers.cpp
		wxRect rectDlg;
		GetSize(&rectDlg.width, &rectDlg.height); // dialog frame's window
		int myTopCoord;
		int myLeftCoord;
		RepositionDialogToUncoverPhraseBox(gpApp, 0, 0, rectDlg.width, rectDlg.height,
						m_ptBoxTopLeft.x, m_ptBoxTopLeft.y, myTopCoord, myLeftCoord);
		SetSize(myLeftCoord, myTopCoord, wxDefaultCoord, wxDefaultCoord, wxSIZE_USE_EXISTING);
		// end of new code on 16Sep11
		Update();
		// In wx version we seem to need to scroll to the found location
		gpApp->GetMainFrame()->canvas->ScrollIntoView(nAtSequNum); // whm added 7Jun07
	}
	else // bFound is FALSE
	{
        // whm 14May2020 added below for giving option to search earlier in document.
        // Notify user that end of document was reached and search item was not found.
        // If search didn't start at doc beginning, ask if the search should be made of the earlier part of
        // the document.
        wxString searchStr;
        if (m_bSrcOnly)
            searchStr = m_pEditSrc->GetValue();
        else if (m_bTgtOnly)
            searchStr = m_pEditTgt->GetValue();
        else if (m_bSrcAndTgt)
            searchStr = m_pEditSrc->GetValue(); // when searching both src and translated text the m_srcStr is used

        wxString msg;
        if (nKickOffSequNum == 0) // nKickOffSequNum was 0 at beginning of document
        {
            // nKickOffSequNum was at start of doc, and nothing was found
            msg = _("All of document was searched. No \"%s\" was found.");
            msg = msg.Format(msg, searchStr.c_str());
            // whm 15May2020 added below to supress phrasebox run-on due to handling of ENTER in CPhraseBox::OnKeyUp()
            gpApp->m_bUserDlgOrMessageRequested = TRUE;
            wxMessageBox(msg, _T(""), wxICON_INFORMATION);
            // pass throgh to pView->FindNextHasLanded() below...
        }
        else if (nKickOffSequNum > 0)
        {
            // nKickOffSequNum was NOT at start of doc, and nothing was found
            if (m_bSearchFromDocBeginning)
            {
                // We searched already from the beginning and nothing was found
                m_bSearchFromDocBeginning = FALSE;
                // starting point was reached and nothing found
                msg = _("Remainder of document was searched. No \"%s\" was found.");
                msg = msg.Format(msg, searchStr.c_str());
                // whm 15May2020 added below to supress phrasebox run-on due to handling of ENTER in CPhraseBox::OnKeyUp()
                gpApp->m_bUserDlgOrMessageRequested = TRUE;
                wxMessageBox(msg, _T(""), wxICON_INFORMATION);
            }
            else
            {
                // nKickOffSequNum was not at start of doc and nothing found, but earlier part of 
                // doc was not searched, so offer to search it.
                gpApp->LogUserAction(_T("   Not Found!"));
                m_nCount = 0; // none matched
                // Search was initiated in middle of document
                msg = _("End of document was reached. No \"%s\" was found. Search for %s in the earlier part of document?");
                msg = msg.Format(msg, searchStr.c_str(), searchStr.c_str());
                int result;
                // whm 15May2020 added below to supress phrasebox run-on due to handling of ENTER in CPhraseBox::OnKeyUp()
                gpApp->m_bUserDlgOrMessageRequested = TRUE;
                result = wxMessageBox(msg, _T(""), wxICON_QUESTION | wxYES_NO | wxYES_DEFAULT);
                if (result == wxYES)
                {
                    m_bSearchFromDocBeginning = TRUE;
                    nKickOffSequNum = 0;
                    // Go to near beginning of DoFindNext() above to setup and do another Find operation
                    goto repeatfind;
                }
            }
        }


		gpApp->LogUserAction(_T("   Not Found!"));
		m_nCount = 0; // none matched
		gbFound = FALSE;
		pView->FindNextHasLanded(gpApp->m_nActiveSequNum,FALSE); // show old active location

		Update();
		::wxBell();
	}
}

void CFindDlg::OnRestoreFindDefaults(wxCommandEvent& WXUNUSED(event))
{
	// CFindDlg does not support a 'replace' wxTextBox
	gpApp->WriteCacheDefaults(); // sets the default config struct to have default values
	CacheFindReplaceConfig* pStruct = &gpApp->defaultFindConfig; // point to it
	CFindDlg* pDlg = gpApp->m_pFindDlg;
	// Now set the relevant member parameters from the struct
	pDlg->m_srcStr = pStruct->srcStr;
	pDlg->m_tgtStr = pStruct->tgtStr;
	pDlg->m_replaceStr = pStruct->replaceStr;
	pDlg->m_sfm = pStruct->sfm;
	pDlg->m_markerStr = pStruct->markerStr;
	pDlg->m_marker = pStruct->marker; // int
	pDlg->m_bFindRetransln = pStruct->bFindRetranslation;
	pDlg->m_bFindNullSrcPhrase = pStruct->bFindNullSrcPhrase; // to find a Placeholder
	pDlg->m_bFindSFM = pStruct->bFindSFM;
	pDlg->m_bSrcOnly = pStruct->bSrcOnly;
	pDlg->m_bTgtOnly = pStruct->bTgtOnly;
	pDlg->m_bSrcAndTgt = pStruct->bSrcAndTgt;
	pDlg->m_bSpecialSearch = pStruct->bSpecialSearch;
	pDlg->m_bFindDlg = pStruct->bFindDlg;
	pDlg->m_bSpanSrcPhrases = pStruct->bSpanSrcPhrases;
	pDlg->m_bIncludePunct = pStruct->bIncludePunct;
	pDlg->m_bIgnoreCase = pStruct->bIgnoreCase;
	pDlg->m_nCount = pStruct->nCount;
	// Now make the GUI conform to the new (default) values
	wxString src = pDlg->m_pEditSrc->GetValue();
	if (src != pDlg->m_srcStr)
	{
		pDlg->m_pEditSrc->ChangeValue(pDlg->m_srcStr);
		pDlg->m_pEditSrc->SetFocus();
	}
	wxString tgt = pDlg->m_pEditTgt->GetValue();
	if (tgt != pDlg->m_tgtStr)
	{
		// Change to show the src text box
		pDlg->m_pEditTgt->ChangeValue(pDlg->m_tgtStr);
	}
	//pDlg->DoRadioSrcOnly();
		
	// Fix the radio buttons, if needed
	wxRadioButton* pRadioSrc = pDlg->m_pRadioSrcTextOnly;
	bool bRadioSrc = pRadioSrc->GetValue();
	if (bRadioSrc == FALSE)
	{
		pRadioSrc->SetValue(TRUE);
	}
	wxRadioButton* pRadioTgt = pDlg->m_pRadioTransTextOnly;
	bool bRadioTgt = pRadioTgt->GetValue();
	if (bRadioTgt == TRUE)
	{
		pRadioTgt->SetValue(FALSE);
	}
	wxRadioButton* pRadioSrcTgt = pDlg->m_pRadioBothSrcAndTransText;
	bool bRadioSrcTgt = pRadioSrcTgt->GetValue();			
	if (bRadioSrcTgt == TRUE)
	{
		pRadioSrcTgt->SetValue(FALSE);
	}

	wxCheckBox* pIgnore = pDlg->m_pCheckIgnoreCase;
	bool bIgnoreCase = pIgnore->GetValue();
	if (bIgnoreCase == TRUE)
	{
		pIgnore->SetValue(FALSE);
	}
	wxCheckBox* pIncludePunct = pDlg->m_pCheckIncludePunct;
	bool bIncludePunct = pIncludePunct->GetValue();
	if (bIncludePunct == TRUE)
	{
		pIncludePunct->SetValue(FALSE);
	}
	wxCheckBox* pSpan = pDlg->m_pCheckSpanSrcPhrases;
	bool bSpan = pSpan->GetValue();
	if (bSpan == TRUE)
	{
		pSpan->SetValue(FALSE);
	}

	pDlg->Refresh();
}

void CFindDlg::DoRadioSrcOnly() 
{
	wxASSERT(m_pStaticSrcBoxLabel != NULL);
	m_pStaticSrcBoxLabel->Show(TRUE);
	
	wxASSERT(m_pEditSrc != NULL);
	m_pEditSrc->SetFocus();
    // whm 3Aug2018 Note: TODO: I assume the select all below is appropriate
    // for the find dialog, and shouldn't be suppressed if 'Select Copied Source'
    // menu item is NOT ticked.
	m_pEditSrc->SetSelection(-1,-1); // -1,-1 selects all
	m_pEditSrc->Show(TRUE);
	
	wxASSERT(m_pStaticTgtBoxLabel != NULL);
	m_pStaticTgtBoxLabel->Hide();
	
	wxASSERT(m_pEditTgt != NULL);
	m_pEditTgt->Hide();
	m_bSrcOnly = TRUE;
	m_bTgtOnly = FALSE;
	m_bSrcAndTgt = FALSE;
}

void CFindDlg::DoRadioTgtOnly() 
{
	wxASSERT(m_pStaticSrcBoxLabel != NULL);
	m_pStaticSrcBoxLabel->Hide();
	
	wxASSERT(m_pEditSrc != NULL);
	m_pEditSrc->Hide();

    // whm 14May2020 added set focus in the m_pEditTgt box
    m_pEditTgt->SetFocus();
	
	wxASSERT(m_pStaticTgtBoxLabel != NULL);
	wxString str;
	str = _("        Translation:"); // str.Format(IDS_TRANS);
	m_pStaticTgtBoxLabel->SetLabel(str);
	m_pStaticTgtBoxLabel->Show(TRUE);
	
	wxASSERT(m_pEditTgt != NULL);
	m_pEditTgt->SetFocus();
    // whm 3Aug2018 Note: TODO: I assume the select all below is appropriate
    // for the find dialog, and shouldn't be suppressed if 'Select Copied Source'
    // menu item is NOT ticked.
    m_pEditTgt->SetSelection(-1,-1); // -1,-1 selects all
	m_pEditTgt->Show(TRUE);
	m_bSrcOnly = FALSE;
	m_bTgtOnly = TRUE;
	m_bSrcAndTgt = FALSE;
}

void CFindDlg::DoRadioSrcAndTgt() 
{
	wxASSERT(m_pStaticSrcBoxLabel != NULL);
	m_pStaticSrcBoxLabel->Show(TRUE);
	
	wxASSERT(m_pEditSrc != NULL);
	m_pEditSrc->Show(TRUE);
	
	wxASSERT(m_pStaticTgtBoxLabel != NULL);
	wxString str;
	str = _("With Translation:"); //IDS_WITH_TRANS
	m_pStaticTgtBoxLabel->SetLabel(str);
	m_pStaticTgtBoxLabel->Show(TRUE);
	
	wxASSERT(m_pEditTgt != NULL);
	m_pEditTgt->SetFocus();
    // whm 3Aug2018 Note: TODO: I assume the select all below is appropriate
    // for the find dialog, and shouldn't be suppressed if 'Select Copied Source'
    // menu item is NOT ticked.
    m_pEditTgt->SetSelection(-1,-1); // -1,-1 selects all
	m_pEditTgt->Show(TRUE);
	m_bSrcOnly = FALSE;
	m_bTgtOnly = FALSE;
	m_bSrcAndTgt = TRUE;	
}


void CFindDlg::OnSelchangeComboSfm(wxCommandEvent& WXUNUSED(event))
{
    // wx note: Under Linux/GTK ...Selchanged... listbox events can be triggered after a
    // call to Clear() so we must check to see if the listbox contains no items and if so
    // return immediately
	if (m_pComboSFM->GetCount() == 0)
		return;

	//TransferDataFromWindow(); // whm removed 21Nov11
	// whm added below 21Nov11 replacing TransferDataFromWindow()
	m_bIgnoreCase = m_pCheckIgnoreCase->GetValue();
	m_srcStr = m_pEditSrc->GetValue();
	m_tgtStr = m_pEditTgt->GetValue();
	m_bIncludePunct = m_pCheckIncludePunct->GetValue();
	m_bSpanSrcPhrases = m_pCheckSpanSrcPhrases->GetValue();
	m_bFindRetranslation = m_pFindRetranslation->GetValue();
	m_bFindNullSrcPhrase = m_pFindPlaceholder->GetValue();
	m_bFindSFM = m_pFindSFM->GetValue();
	m_marker = m_pComboSFM->GetSelection();


	// get the new value of the combobox & convert to an sfm
	m_marker = m_pComboSFM->GetSelection();
	wxASSERT(m_marker != -1);
	m_markerStr = m_pComboSFM->GetStringSelection();
	wxASSERT(!m_markerStr.IsEmpty());
	m_sfm = SpanExcluding(m_markerStr, _T(" ")); // up to the first space character
	wxASSERT(m_sfm.Left(1) == gSFescapechar); // it must start with a backslash 
						// to be a valid sfm, or with sfm escape char set by user
}

void CFindDlg::OnButtonSpecial(wxCommandEvent& WXUNUSED(event)) 
{
	if (m_bSpecialSearch)
	{
		m_bSpecialSearch = FALSE; // toggle flag value, we are 
								  // returning to 'normal' search mode
		
		wxASSERT(m_pButtonSpecialNormal != NULL);
		wxString str;
		str = _("Special Search"); //IDS_SPECIAL_SEARCH
		m_pButtonSpecialNormal->SetLabel(str);
		
		wxASSERT(m_pFindRetranslation != NULL);
		m_pFindRetranslation->SetValue(TRUE);
		m_bFindRetranslation = FALSE;
		m_pFindRetranslation->Hide();
		
		wxASSERT(m_pFindPlaceholder != NULL);
		m_pFindPlaceholder->SetValue(FALSE);
		m_bFindNullSrcPhrase = FALSE;
		m_pFindPlaceholder->Hide();
		
		wxASSERT(m_pFindSFM != NULL);
		m_pFindSFM->SetValue(FALSE);
		m_bFindSFM = FALSE;
		m_pFindSFM->Hide();
		
		wxASSERT(m_pComboSFM != NULL);
		m_pComboSFM->Hide();
		
		wxASSERT(m_pSelectAnSfm != NULL);
		m_pSelectAnSfm->Hide();

		wxASSERT(m_pSpecialSearches != NULL);
		m_pSpecialSearches->Hide();

		// enable everything which is part of the normal search stuff
		wxASSERT(m_pCheckIgnoreCase != NULL);
		m_pCheckIgnoreCase->Enable(TRUE);
		
		wxASSERT(m_pRadioSrcTextOnly != NULL);
		m_pRadioSrcTextOnly->SetValue(TRUE);
		m_pRadioSrcTextOnly->Enable(TRUE);
		
		wxASSERT(m_pRadioTransTextOnly != NULL);
		m_pRadioTransTextOnly->SetValue(FALSE);
		m_pRadioTransTextOnly->Enable(TRUE);
		
		wxASSERT(m_pRadioBothSrcAndTransText != NULL);
		m_pRadioBothSrcAndTransText->SetValue(FALSE);
		m_pRadioBothSrcAndTransText->Enable(TRUE);
		
		wxASSERT(m_pStaticTgtBoxLabel != NULL);
		m_pStaticTgtBoxLabel->Hide();
		
		wxASSERT(m_pStaticSrcBoxLabel != NULL);
		m_pStaticSrcBoxLabel->Show(TRUE);

		wxASSERT(m_pEditTgt != NULL);
		m_pEditTgt->Hide();
		
		wxASSERT(m_pEditSrc != NULL);
		m_pEditSrc->Show(TRUE);
		
		wxASSERT(m_pCheckIncludePunct != NULL);
		m_pCheckIncludePunct->Show(TRUE);
		
		wxASSERT(m_pCheckSpanSrcPhrases != NULL);
		m_pCheckSpanSrcPhrases->Show(TRUE);
	}
	else
	{
		m_bSpecialSearch = TRUE; // toggle flag value, we are 
								 // entering 'special' mode
		
		wxASSERT(m_pButtonSpecialNormal != NULL);
		wxString str;
		str = _("Normal Search"); //IDS_NORMAL_SEARCH
		m_pButtonSpecialNormal->SetLabel(str);
		
		wxASSERT(m_pFindRetranslation != NULL);
		m_pFindRetranslation->SetValue(TRUE);
		m_bFindRetranslation = TRUE;
		m_pFindRetranslation->Enable(TRUE);
		m_pFindRetranslation->Show(TRUE);
		
		wxASSERT(m_pFindPlaceholder != NULL);
		m_pFindPlaceholder->SetValue(FALSE);
		m_bFindNullSrcPhrase = 0;
		m_pFindPlaceholder->Enable(TRUE);
		m_pFindPlaceholder->Show(TRUE);
		
		wxASSERT(m_pFindSFM != NULL);
		m_pFindSFM->SetValue(FALSE);
		m_bFindSFM = FALSE;
		m_pFindSFM->Enable(TRUE);
		m_pFindSFM->Show(TRUE);
		
		wxASSERT(m_pSpecialSearches != NULL);
		m_pSpecialSearches->Show(TRUE);
		
		wxASSERT(m_pComboSFM != NULL);
		m_pComboSFM->Show(TRUE);
		
		wxASSERT(m_pSelectAnSfm != NULL);
		m_pSelectAnSfm->Show(TRUE);

		// get the initial value of the combobox, as the default, then disable it
		wxASSERT(m_pComboSFM->GetCount() > 0);
		m_pComboSFM->SetSelection(0);
		m_markerStr = m_pComboSFM->GetStringSelection();
		wxASSERT(!m_markerStr.IsEmpty());
		m_sfm = SpanExcluding(m_markerStr,_T(" ")); // up to the first space character
		wxASSERT(m_sfm.Left(1) == gSFescapechar); // it must start with a backslash 
						// to be a valid sfm, or with the user set escape character
		// now disable the box until it's explicitly wanted
		m_pComboSFM->Enable(FALSE); // it should start off disabled

		// disable everything which is not part of the special search stuff
		wxASSERT(m_pCheckIgnoreCase != NULL);
		m_pCheckIgnoreCase->Enable(FALSE);
		
		wxASSERT(m_pRadioSrcTextOnly != NULL);
		m_pRadioSrcTextOnly->SetValue(TRUE);
		m_pRadioSrcTextOnly->Enable(FALSE);
		
		wxASSERT(m_pRadioTransTextOnly != NULL);
		m_pRadioTransTextOnly->SetValue(FALSE);
		m_pRadioTransTextOnly->Enable(FALSE);
		
		wxASSERT(m_pRadioBothSrcAndTransText != NULL);
		m_pRadioBothSrcAndTransText->SetValue(FALSE);
		m_pRadioBothSrcAndTransText->Enable(FALSE);
		
		wxASSERT(m_pStaticTgtBoxLabel != NULL);
		m_pStaticTgtBoxLabel->Hide();
		
		wxASSERT(m_pStaticSrcBoxLabel != NULL);
		m_pStaticSrcBoxLabel->Hide();

		wxASSERT(m_pEditTgt != NULL);
		m_pEditTgt->Hide();
		
		wxASSERT(m_pEditSrc != NULL);
		m_pEditSrc->Hide();
		
		wxASSERT(m_pCheckIncludePunct != NULL);
		m_pCheckIncludePunct->Hide();
		
		wxASSERT(m_pCheckSpanSrcPhrases != NULL);
		m_pCheckSpanSrcPhrases->Hide();
	}
	pFindDlgSizer->Layout(); // force the top dialog sizer to re-layout the dialog
}

// whm 14May2020 addition. The following OnSrcEditBoxEnterKeyPress() function is triggered from
// the EVENT_TABLE macro line: EVT_TEXT_ENTER(IDC_EDIT_SRC_FIND, CFindDlg::OnSrcditBoxEnterKeyPress)
// when an ENTER key is pressed from within the Source Text wxTextCtrl, which has its
// wxTE_PROCESS_ENTER style flag set. Here we set up a default action we call the DoFindNext() handler 
// function to make it be the default action triggered by the ENTER key press.
void CFindDlg::OnSrcEditBoxEnterKeyPress(wxCommandEvent & WXUNUSED(event))
{
    // whm 15May2020 Note: gpApp->m_bUserDlgOrMessageRequested is set to TRUE in this->InitDialog()
    // but it becomes FALSE again (by code elsewhere) after the first Find Next is done. 
    // For modeless dialogs - which can execute ENTER-generated commands multiple times
    // setting gpApp->m_bUserDlgOrMessageRequested to TRUE in the dialog's InitDialos() 
    // only is not sufficient. Here in CFind, it needs to be set to TRUE in each DoFindNext() call 
    // in order to supress all phrasebox run-on instances due to ENTER key getting 
    // propagated on to CPhraseBox::OnKeyUp().
    wxLogDebug(_T("CFindDlg::OnSrcEditBoxEnterKeyPress() handling WXK_RETURN key"));
    this->DoFindNext();
    // don't call event.Skip() here, just return.
    return; // event.Skip();
}

// whm 14May2020 addition. The following OnTgtEditBoxEnterKeyPress() function is triggered from
// the EVENT_TABLE macro line: EVT_TEXT_ENTER(IDC_EDIT_TGT_FIND, CFindDlg::OnTgtditBoxEnterKeyPress)
// when an ENTER key is pressed from within the Translation wxTextCtrl, which has its
// wxTE_PROCESS_ENTER style flag set. Here we set up a default action we call the DoFindNext() handler 
// function to make it be the default action triggered by the ENTER key press.
void CFindDlg::OnTgtEditBoxEnterKeyPress(wxCommandEvent & WXUNUSED(event))
{
    // whm 15May2020 Note: gpApp->m_bUserDlgOrMessageRequested is set to TRUE in this->InitDialog()
    // but it becomes FALSE again (by code elsewhere) after the first Find Next is done. 
    // For modeless dialogs - which can execute ENTER-generated commands multiple times
    // setting gpApp->m_bUserDlgOrMessageRequested to TRUE in the dialog's InitDialog() 
    // only is not sufficient. Here in CFind, it needs to be set to TRUE in each DoFindNext() call 
    // in order to supress all phrasebox run-on instances due to ENTER key getting 
    // propagated on to CPhraseBox::OnKeyUp().
    wxLogDebug(_T("CFindDlg::OnTgtEditBoxEnterKeyPress() handling WXK_RETURN key"));
    this->DoFindNext();
    // don't call event.Skip() here, just return.
    return; // event.Skip();
}

void CFindDlg::OnRadioSfm(wxCommandEvent& event) 
{
	wxASSERT(m_pComboSFM != NULL);
	m_pComboSFM->Enable(TRUE);
	
	wxASSERT(m_pFindRetranslation != NULL);
	m_pFindRetranslation->SetValue(FALSE);
	m_bFindRetranslation = FALSE;
	
	wxASSERT(m_pFindPlaceholder != NULL);
	m_pFindPlaceholder->SetValue(FALSE);
	m_bFindNullSrcPhrase = FALSE;
	
	wxASSERT(m_pFindSFM != NULL);
	m_pFindSFM->SetValue(TRUE);
	m_bFindSFM = TRUE;

	// get initial value
	OnSelchangeComboSfm(event);
	//TransferDataToWindow(); // whm removed 21Nov11
	// whm added below 21Nov11 replacing TransferDataToWindow()
	m_pCheckIgnoreCase->SetValue(m_bIgnoreCase);
	m_pEditSrc->ChangeValue(m_srcStr);
	m_pEditTgt->ChangeValue(m_tgtStr);
	m_pCheckIncludePunct->SetValue(m_bIncludePunct);
	m_pCheckSpanSrcPhrases->SetValue(m_bSpanSrcPhrases);
	m_pFindRetranslation->SetValue(m_bFindRetranslation);
	m_pFindPlaceholder->SetValue(m_bFindNullSrcPhrase);
	m_pFindSFM->SetValue(m_bFindSFM);
	m_pComboSFM->SetSelection(m_marker);

}
	
void CFindDlg::OnRadioRetranslation(wxCommandEvent& WXUNUSED(event)) 
{
	wxASSERT(m_pComboSFM != NULL);
	m_pComboSFM->Enable(FALSE);
	
	wxASSERT(m_pFindRetranslation != NULL);
	m_pFindRetranslation->SetValue(TRUE);
	m_bFindRetranslation = TRUE;
	
	wxASSERT(m_pFindPlaceholder != NULL);
	m_pFindPlaceholder->SetValue(FALSE);
	m_bFindNullSrcPhrase = FALSE;
	
	wxASSERT(m_pFindSFM != NULL);
	m_pFindSFM->SetValue(FALSE);
	m_bFindSFM = FALSE;
	//TransferDataToWindow(); // whm removed 21Nov11
	// whm added below 21Nov11 replacing TransferDataToWindow()
	m_pCheckIgnoreCase->SetValue(m_bIgnoreCase);
	m_pEditSrc->ChangeValue(m_srcStr);
	m_pEditTgt->ChangeValue(m_tgtStr);
	m_pCheckIncludePunct->SetValue(m_bIncludePunct);
	m_pCheckSpanSrcPhrases->SetValue(m_bSpanSrcPhrases);
	m_pFindRetranslation->SetValue(m_bFindRetranslation);
	m_pFindPlaceholder->SetValue(m_bFindNullSrcPhrase);
	m_pFindSFM->SetValue(m_bFindSFM);
	m_pComboSFM->SetSelection(m_marker);
}

void CFindDlg::OnRadioNullSrcPhrase(wxCommandEvent& WXUNUSED(event)) 
{
	wxASSERT(m_pComboSFM != NULL);
	m_pComboSFM->Enable(FALSE);
	
	wxASSERT(m_pFindRetranslation != NULL);
	m_pFindRetranslation->SetValue(FALSE);
	m_bFindRetranslation = FALSE;
	
	wxASSERT(m_pFindPlaceholder != NULL);
	m_pFindPlaceholder->SetValue(TRUE);
	m_bFindNullSrcPhrase = TRUE;
	
	wxASSERT(m_pFindSFM != NULL);
	m_pFindSFM->SetValue(FALSE);
	m_bFindSFM = FALSE;
	//TransferDataToWindow(); // whm removed 21Nov11
	// whm added below 21Nov11 replacing TransferDataToWindow()
	m_pCheckIgnoreCase->SetValue(m_bIgnoreCase);
//#if defined(FWD_SLASH_DELIM)
	// BEW added 23Apr15 -- need to restore / from any ZWSP instances
	m_srcStr = ZWSPtoFwdSlash(m_srcStr);
	m_tgtStr = ZWSPtoFwdSlash(m_tgtStr);
//#endif
	m_pEditSrc->ChangeValue(m_srcStr);
	m_pEditTgt->ChangeValue(m_tgtStr);
	m_pCheckIncludePunct->SetValue(m_bIncludePunct);
	m_pCheckSpanSrcPhrases->SetValue(m_bSpanSrcPhrases);
	m_pFindRetranslation->SetValue(m_bFindRetranslation);
	m_pFindPlaceholder->SetValue(m_bFindNullSrcPhrase);
	m_pFindSFM->SetValue(m_bFindSFM);
	m_pComboSFM->SetSelection(m_marker);

	// BEW 31Mar21 turn off the bool in app which gets
	// the Retranslation radio button reset TRUE
	gpApp->m_bMatchedRetranslation = FALSE;
}

// BEW 3Aug09 removed unneeded FindNextHasLanded() call in OnCancel()
void CFindDlg::OnCancel(wxCommandEvent& WXUNUSED(event)) 
{
	wxASSERT(gpApp->m_pFindDlg != NULL);

	// destroying the window, but first clear the variables to defaults
	m_srcStr = _T("");
	m_tgtStr = _T("");
	m_replaceStr = _T("");
	m_marker = 0;
	m_markerStr = _T("");
	m_sfm = _T("");
	m_bFindRetranslation = FALSE;
	m_bFindNullSrcPhrase = FALSE;
	m_bFindSFM = FALSE;
	m_bSrcOnly = TRUE;
	m_bTgtOnly = FALSE;
	m_bSrcAndTgt = FALSE;
	m_bSpecialSearch = FALSE;
	m_bFindDlg = TRUE;

	//TransferDataToWindow(); // whm removed 21Nov11
	// whm added below 21Nov11 replacing TransferDataToWindow()
	m_pCheckIgnoreCase->SetValue(m_bIgnoreCase);
	m_pEditSrc->ChangeValue(m_srcStr);
	m_pEditTgt->ChangeValue(m_tgtStr);
	m_pCheckIncludePunct->SetValue(m_bIncludePunct);
	m_pCheckSpanSrcPhrases->SetValue(m_bSpanSrcPhrases);
	m_pFindRetranslation->SetValue(m_bFindRetranslation);
	m_pFindPlaceholder->SetValue(m_bFindNullSrcPhrase);
	m_pFindSFM->SetValue(m_bFindSFM);
	m_pComboSFM->SetSelection(m_marker);

    // whm 17May2020 set the flag just before Destroy() call in this OnCancel() handler
    gpApp->m_bUserDlgOrMessageRequested = TRUE;
    Destroy();

	gbFindIsCurrent = FALSE;
	gbJustReplaced = FALSE; // clear to default value 

	// no selection, so find another way to define active location 
	// & place the phrase box
	int nCurSequNum = gpApp->m_nActiveSequNum;
	if (nCurSequNum == -1)
	{
		nCurSequNum = gpApp->GetMaxIndex(); // make active loc the last 
											// src phrase in the doc
		gpApp->m_nActiveSequNum = nCurSequNum;
	}
	else if (nCurSequNum >= 0 && nCurSequNum <= gpApp->GetMaxIndex())
	{
		gpApp->m_nActiveSequNum = nCurSequNum;
	}
	else
	{
		// if all else fails, go to the start
		gpApp->m_nActiveSequNum = 0;
	}
	gbJustCancelled = TRUE;
	gbFindOrReplaceCurrent = FALSE; // turn it back off

	gpApp->m_pFindDlg = (CFindDlg*)NULL;

	// clear the globals
	gpApp->m_bMatchedRetranslation = FALSE;
	gnRetransEndSequNum = -1;

	//	wxDialog::OnCancel(); // don't call base class because we are modeless
							  // - use this only if its modal
}

void CFindDlg::OnFindNext(wxCommandEvent& WXUNUSED(event))
{
	DoFindNext();
}

void CFindDlg::OnRadioSrcOnly(wxCommandEvent& WXUNUSED(event))
{
	DoRadioSrcOnly();
	pFindDlgSizer->Layout(); // force the top dialog sizer to re-layout the dialog
}

void CFindDlg::OnRadioTgtOnly(wxCommandEvent& WXUNUSED(event))
{
	DoRadioTgtOnly();
	pFindDlgSizer->Layout(); // force the top dialog sizer to re-layout the dialog
}

void CFindDlg::OnRadioSrcAndTgt(wxCommandEvent& WXUNUSED(event))
{
	DoRadioSrcAndTgt();
	pFindDlgSizer->Layout(); // force the top dialog sizer to re-layout the dialog
}



// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CReplaceDlg !!!!!!!!!!!!!!!!!!!!!!!!!!!

// whm 14Jun12 modified to use wxDialog for wxWidgets 2.9.x and later; wxScrollingDialog for pre-2.9.x
#if wxCHECK_VERSION(2,9,0)
IMPLEMENT_DYNAMIC_CLASS( CReplaceDlg, wxDialog )
#else
IMPLEMENT_DYNAMIC_CLASS( CReplaceDlg, wxScrollingDialog )
#endif

// event handler table
// whm 14Jun12 modified to use wxDialog for wxWidgets 2.9.x and later; wxScrollingDialog for pre-2.9.x
#if wxCHECK_VERSION(2,9,0)
BEGIN_EVENT_TABLE(CReplaceDlg, wxDialog)
#else
BEGIN_EVENT_TABLE(CReplaceDlg, wxScrollingDialog)
#endif
	EVT_INIT_DIALOG(CReplaceDlg::InitDialog)
	EVT_BUTTON(wxID_CANCEL, CReplaceDlg::OnCancel)	
	

    // whm 14May2020 added the following EVT_TEXT_ENTER() lines to intercept and prevent
    // an ENTER key press inside the "Source Text" and/or "Translation" edit boxes from
    // propagating to the phrasebox in the main window and causing the phrasebox to move 
    // on from the found location. 
    // The OnSrcEditBoxEnterKeyPress() and OnTgtEditBoxEnterKeyPress() handlers simply 
    // call the OnFindNext() handler with a dummy event, and then return without calling 
    // event.Skip().
    // whm 14May2020 followup note: The following handlers work sometimes and not at other
    // times. They tend to work well when the debugger stops the flow of control at a 
    // breakpoint within the handlers and then allowed program control to resume.
    EVT_TEXT_ENTER(IDC_EDIT_SRC_FIND, CReplaceDlg::OnSrcEditBoxEnterKeyPress)
    EVT_TEXT_ENTER(IDC_EDIT_TGT_FIND, CReplaceDlg::OnTgtEditBoxEnterKeyPress)
    EVT_TEXT_ENTER(IDC_EDIT_REPLACE, CReplaceDlg::OnReplaceEditBoxEnterKeyPress)

    EVT_BUTTON(IDC_REPLACE, CReplaceDlg::OnReplaceButton)
	EVT_BUTTON(IDC_REPLACE_ALL_BUTTON, CReplaceDlg::OnReplaceAllButton)
	EVT_BUTTON(wxID_OK, CReplaceDlg::OnFindNext) // Find Next button has ID of wxID_OK
	EVT_RADIOBUTTON(IDC_RADIO_SRC_ONLY_REPLACE, CReplaceDlg::OnRadioSrcOnly)
	EVT_RADIOBUTTON(IDC_RADIO_TGT_ONLY_REPLACE, CReplaceDlg::OnRadioTgtOnly)
	EVT_RADIOBUTTON(IDC_RADIO_SRC_AND_TGT_REPLACE, CReplaceDlg::OnRadioSrcAndTgt)
	EVT_CHECKBOX(IDC_CHECK_SPAN_SRC_PHRASES_REPLACE, CReplaceDlg::OnSpanCheckBoxChanged)
    EVT_UPDATE_UI(IDC_REPLACE_ALL_BUTTON, CReplaceDlg::OnUpdateReplaceAllButton)
    EVT_UPDATE_UI(IDC_REPLACE, CReplaceDlg::OnUpdateReplace)
    EVT_UPDATE_UI(wxID_OK, CReplaceDlg::OnUpdateFindNext)  // Find Next button has ID of wxID_OK
    END_EVENT_TABLE()

CReplaceDlg::CReplaceDlg()
{
}

CReplaceDlg::~CReplaceDlg()
{
}

CReplaceDlg::CReplaceDlg(wxWindow* parent) // dialog constructor
// whm 14Jun12 modified to use wxDialog for wxWidgets 2.9.x and later; wxScrollingDialog for pre-2.9.x
#if wxCHECK_VERSION(2,9,0)
	: wxDialog(parent, -1, _("Find and Replace"),
		wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
#else
	: wxScrollingDialog(parent, -1, _("Find and Replace"),
		wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
#endif
{
    // This dialog function below is generated in wxDesigner, and defines the controls and
    // sizers for the dialog. The first parameter is the parent which should normally be
    // "this". The second and third parameters should both be TRUE to utilize the sizers
    // and create the right size dialog.
	pReplaceDlgSizer = ReplaceDlgFunc(this, TRUE, TRUE);
	// The declaration is: ReplaceDlgFunc( wxWindow *parent, bool call_fit, bool set_sizer );

    // whm 5Mar2019 Since wxStdDialogButtonsSizer is not used in the ReplaceDlgFunc(),
    // we need to call App's ReverseOkCancelButtonsForMac() function to reverse the "Find Next" (wxID_OK) 
    // and "Cancel" buttons.
    bool bOK;
	bOK = gpApp->ReverseOkCancelButtonsForMac(this);
	bOK = bOK; // avoid warning
	//m_marker = -1;
	m_srcStr = _T("");
	m_replaceStr = _T("");
	m_tgtStr = _T("");
	m_bIncludePunct = FALSE;
	m_bSpanSrcPhrases = FALSE;
	m_bIgnoreCase = FALSE;

	m_pRadioSrcTextOnly = (wxRadioButton*)FindWindowById(IDC_RADIO_SRC_ONLY_REPLACE);
	wxASSERT(m_pRadioSrcTextOnly != NULL);

	m_pRadioTransTextOnly = (wxRadioButton*)FindWindowById(IDC_RADIO_TGT_ONLY_REPLACE);
	wxASSERT(m_pRadioTransTextOnly != NULL);

	m_pRadioBothSrcAndTransText = (wxRadioButton*)FindWindowById(IDC_RADIO_SRC_AND_TGT_REPLACE);
	wxASSERT(m_pRadioBothSrcAndTransText != NULL);

	m_pCheckIgnoreCase = (wxCheckBox*)FindWindowById(IDC_CHECK_IGNORE_CASE_REPLACE);
	wxASSERT(m_pCheckIgnoreCase != NULL);
	//m_pCheckIgnoreCase->SetValidator(wxGenericValidator(&m_bIgnoreCase)); // use validator

	m_pFindNext = (wxButton*)FindWindow(wxID_OK); // just use FindWindow here to 
												  // limit search to CReplaceDlg
	wxASSERT(m_pFindNext != NULL);

	m_pStaticSrcBoxLabel = (wxStaticText*)FindWindowById(IDC_STATIC_SRC_REPLACE);
	wxASSERT(m_pStaticSrcBoxLabel != NULL);

	m_pEditSrc_Rep = (wxTextCtrl*)FindWindowById(IDC_EDIT_SRC_REPLACE); // BEW 5Apr21 name change, see reason in next comment
	wxASSERT(m_pEditSrc_Rep != NULL);
	//m_pEditSrc->SetValidator(wxGenericValidator(&m_srcStr)); // use validator

	m_pStaticTgtBoxLabel = (wxStaticText*)FindWindowById(IDC_STATIC_TGT_REPLACE);
	wxASSERT(m_pStaticTgtBoxLabel != NULL);

	// whm 31Aug2021 modified line below to use the AutoCorrectTextCtrl class which is now
	// used as a custom control in wxDesigner's ReplaceDlgFnc() dialog.
	m_pEditTgt_Rep = (AutoCorrectTextCtrl*)FindWindowById(IDC_EDIT_TGT_REPLACE); // BEW 5Apr21 added _Rep, as control was going to Find's line 66
	wxASSERT(m_pEditTgt_Rep != NULL);
	//m_pEditTgt->SetValidator(wxGenericValidator(&m_tgtStr)); // use validator

	m_pButtonReplace = (wxButton*)FindWindowById(IDC_REPLACE);
	wxASSERT(m_pButtonReplace != NULL);
	
	m_pButtonReplaceAll = (wxButton*)FindWindowById(IDC_REPLACE_ALL_BUTTON);
	wxASSERT(m_pButtonReplaceAll != NULL);
	
	m_pCheckIncludePunct = (wxCheckBox*)FindWindowById(IDC_CHECK_INCLUDE_PUNCT_REPLACE);
	wxASSERT(m_pCheckIncludePunct != NULL);
	//m_pCheckIncludePunct->SetValidator(wxGenericValidator(&m_bIncludePunct)); // use validator

	m_pCheckSpanSrcPhrases = (wxCheckBox*)FindWindowById(IDC_CHECK_SPAN_SRC_PHRASES_REPLACE);
	wxASSERT(m_pCheckSpanSrcPhrases != NULL);
	//m_pCheckSpanSrcPhrases->SetValidator(wxGenericValidator(&m_bSpanSrcPhrases)); // use validator

	m_pEditReplace = (AutoCorrectTextCtrl*)FindWindowById(IDC_EDIT_REPLACE);
	wxASSERT(m_pEditReplace != NULL);
	//m_pEditReplace->SetValidator(wxGenericValidator(&m_replaceStr)); // use validator
	
}

void CReplaceDlg::InitDialog(wxInitDialogEvent& WXUNUSED(event)) // InitDialog is method of wxWindow
{
	gbFound = FALSE;
	gbReplaceAllIsCurrent = FALSE;
	gbJustCancelled = FALSE;

    // whm 15May2020 added
    m_bSearchFromDocBeginning = FALSE;

    // make the font show user-chosen point size in the dialog
	#ifdef _RTL_FLAGS
	gpApp->SetFontAndDirectionalityForDialogControl(gpApp->m_pSourceFont, m_pEditSrc_Rep, NULL,
								NULL, NULL, gpApp->m_pDlgSrcFont, gpApp->m_bSrcRTL);
	#else // Regular version, only LTR scripts supported, so use default FALSE for last parameter
	gpApp->SetFontAndDirectionalityForDialogControl(gpApp->m_pSourceFont, m_pEditSrc, NULL, 
								NULL, NULL, gpApp->m_pDlgSrcFont);
	#endif


	// for glossing or adapting
	if (gbIsGlossing && gbGlossingUsesNavFont)
	{
		#ifdef _RTL_FLAGS
		gpApp->SetFontAndDirectionalityForDialogControl(gpApp->m_pNavTextFont, 
				m_pEditTgt_Rep, m_pEditReplace, NULL, NULL, gpApp->m_pDlgTgtFont, 
				gpApp->m_bNavTextRTL);
		#else // Regular version, only LTR scripts supported, 
			  // so use default FALSE for last parameter
		gpApp->SetFontAndDirectionalityForDialogControl(gpApp->m_pNavTextFont, 
				m_pEditTgt, m_pEditReplace, NULL, NULL, gpApp->m_pDlgTgtFont);
		#endif
	}
	else
	{
		#ifdef _RTL_FLAGS
		gpApp->SetFontAndDirectionalityForDialogControl(gpApp->m_pTargetFont, 
				m_pEditTgt_Rep, m_pEditReplace, NULL, NULL, gpApp->m_pDlgTgtFont, 
				gpApp->m_bTgtRTL);
		#else // Regular version, only LTR scripts supported, 
			  // so use default FALSE for last parameter
		gpApp->SetFontAndDirectionalityForDialogControl(gpApp->m_pTargetFont, 
				m_pEditTgt, m_pEditReplace, NULL, NULL, gpApp->m_pDlgTgtFont);
		#endif
	}

	// set default checkbox values
	m_bIncludePunct = FALSE;
	m_bSpanSrcPhrases = FALSE;

	// disable the search in src only radio button, it makes to sense to
	// replace if we only search source language text
	wxASSERT(m_pRadioSrcTextOnly != NULL);
	m_pRadioSrcTextOnly->SetValue(FALSE);
	m_pRadioSrcTextOnly->Enable(FALSE);


	// if glossing is ON, the "Search, retaining target text punctuation" and
	// "Allow the search to occur in the text spanning multiple piles" have to be
	// hidden; since merging is not permitted and only the gloss line can be accessed
	// as the 'target'
	if (gbIsGlossing)
	{
		wxASSERT(m_pCheckIncludePunct != NULL);
		m_pCheckIncludePunct->Hide();
		
		wxASSERT(m_pCheckSpanSrcPhrases != NULL);
		m_pCheckSpanSrcPhrases->Hide();
	}

	// get the normal defaults set up
	wxASSERT(m_pRadioTransTextOnly != NULL);
	m_pRadioTransTextOnly->SetValue(TRUE);
	
	wxASSERT(m_pEditSrc_Rep != NULL);
	m_pEditSrc_Rep->Hide();
	
	wxASSERT(m_pStaticSrcBoxLabel != NULL);
	m_pStaticSrcBoxLabel->Hide();
	
	wxASSERT(m_pStaticTgtBoxLabel != NULL);
	wxString str;
	str = _("        Translation:"); //IDS_TRANS
	m_pStaticTgtBoxLabel->SetLabel(str);
	m_pStaticTgtBoxLabel->Show(TRUE);
	
	wxASSERT(m_pEditTgt_Rep != NULL);
	m_pEditTgt_Rep->Show(TRUE);

    // whm 16May2020 added - initially disable the Replace and Replace All buttons
    wxASSERT(m_pButtonReplace != NULL);
    m_pButtonReplace->Disable();

    wxASSERT(m_pButtonReplaceAll != NULL);
    m_pButtonReplaceAll->Disable();


    // whm 14May2020 in CReplaceDlg we set initial focus in the m_pEditTgt edit box
    m_pEditTgt_Rep->SetFocus();

    // set the default button to Find Next button explicitly (otherwise, an MFC bug makes
    // it the Replace All button)
	// whm note: The "Find Next" button is assigned the wxID_OK identifier
	wxASSERT(m_pFindNext != NULL);
	m_pFindNext->SetDefault(); // ID for Find Next button

	m_nCount = 0; // nothing matched yet

    // whm 15May2020 added below to supress phrasebox run-on due to handling of ENTER in CPhraseBox::OnKeyUp()
    gpApp->m_bUserDlgOrMessageRequested = TRUE;

    pReplaceDlgSizer->Layout(); // force the sizers to resize the dialog
}

// BEW 26Mar10, no changes needed for support of doc version 5
// BEW 23Apr15 added support for / used as a word-breaking whitespace character
// whm 14May2020 revised to support searching from beginning of document when a search
// reaches the end of the document and doesn't find the search string. Also revised to
// use m_bUserDlgOrMessageRequested to help prevent run on phrasebox while interacting
// with the dialog.
void CReplaceDlg::DoFindNext()
{
	// this handles the wxID_OK special identifier assigned to the "Find Next" button
	CAdapt_ItView* pView = gpApp->GetView();
	pView->RemoveSelection();
	CAdapt_ItDoc* pDoc = gpApp->GetDocument();

    int nKickOffSequNum = gpApp->m_pActivePile->GetSrcPhrase()->m_nSequNumber; // could be 0 if active pile is at 0

repeatfind:

	// BEW 3Apr21 write the cached values into the config dialog, if the struct
	// CacheReplaceConfig has it's bool bReplaceDlg set TRUE (it's TRUE after a
	// Find or Find Replace has been initiated, and stays TRUE until user cancels, 
	// whereupon its FALSE). Reading the cache in order to restore the cached values
	// to the next OnFind(), etc, call so that the config dialog reappears with values
	// unchanged is done at the start of the OnFind() call in the View class after
	// line 19,138 (near start of the function). For m_pReplalceDlg, use WriteFindCache()
	bool bWriteStructCache = gpApp->WriteReplaceCache();
	wxUnusedVar(bWriteStructCache);

    // whm 15May2020 Note: gpApp->m_bUserDlgOrMessageRequested is set to TRUE in this->InitDialog()
    // but it becomes FALSE again (by code elsewhere) after the first Find Next is done. 
    // For modeless dialogs - which can execute ENTER-generated commands multiple times
    // setting gpApp->m_bUserDlgOrMessageRequested to TRUE in the dialog's InitDialos() 
    // only is not sufficient. Here in CFind, it needs to be set to TRUE in each DoFindNext() call 
    // in order to supress all phrasebox run-on instances due to ENTER key getting 
    // propagated on to CPhraseBox::OnKeyUp().
    gpApp->m_bUserDlgOrMessageRequested = TRUE;

    // do nothing if past the end of the last srcPhrase, ie. at the EOF
	if (gpApp->m_nActiveSequNum == -1)
	{	
		wxASSERT(gpApp->m_pActivePile == 0);
		::wxBell();
		return;
	}

	if (gbJustReplaced)
	{
		// do all the housekeeping tasks associated with a move, after a replace was done
		// if the targetBox is visible, store the contents in KB since we will advance the
		// active location potentially elsewhere
		if (gpApp->m_pTargetBox != NULL)
		{
			if (gpApp->m_pTargetBox->IsShown())
			{
				if (!gbIsGlossing)
				{
					pView->MakeTargetStringIncludingPunctuation(gpApp->m_pActivePile->GetSrcPhrase(),
												gpApp->m_targetPhrase);
					pView->RemovePunctuation(pDoc,&gpApp->m_targetPhrase,from_target_text);
				}
                // the store will fail if the user edited the entry out of the KB, as the
                // latter cannot know which srcPhrases will be affected, so these will
                // still have their m_bHasKBEntry set true, or in the case of glossing KB,
                // their m_bHasGlossingKBEntry set true. We have to test for this, ie. a
                // null pRefString or rsEntry returned as present_but_deleted, but the
                // above flag TRUE is a sufficient test, and if so, set the flag to FALSE
				CRefString* pRefStr = NULL;
				KB_Entry rsEntry;
				bool bOK;
				if (gbIsGlossing)
				{
					rsEntry = gpApp->m_pGlossingKB->GetRefString(gpApp->m_pActivePile->GetSrcPhrase()->m_nSrcWords,
						gpApp->m_pActivePile->GetSrcPhrase()->m_key, gpApp->m_targetPhrase, pRefStr);
					if ((pRefStr == NULL || rsEntry == present_but_deleted) && 
						gpApp->m_pActivePile->GetSrcPhrase()->m_bHasGlossingKBEntry)
					{
						gpApp->m_pActivePile->GetSrcPhrase()->m_bHasGlossingKBEntry = FALSE;
					}
					bOK = gpApp->m_pGlossingKB->StoreText(gpApp->m_pActivePile->GetSrcPhrase(), gpApp->m_targetPhrase);
					bOK = bOK; // avoid warning
				}
				else
				{
					rsEntry = gpApp->m_pKB->GetRefString(gpApp->m_pActivePile->GetSrcPhrase()->m_nSrcWords,
						gpApp->m_pActivePile->GetSrcPhrase()->m_key, gpApp->m_targetPhrase, pRefStr);
					if ((pRefStr == NULL || rsEntry == present_but_deleted) && 
						gpApp->m_pActivePile->GetSrcPhrase()->m_bHasKBEntry)
					{
						gpApp->m_pActivePile->GetSrcPhrase()->m_bHasKBEntry = FALSE;
					}
					//gpApp->m_bInhibitMakeTargetStringCall = TRUE; // BEW 2Mar20 added the inhibit call, MakeTgtstr...() is above
					bOK = gpApp->m_pKB->StoreText(gpApp->m_pActivePile->GetSrcPhrase(), gpApp->m_targetPhrase);
					bOK = bOK; // avoid warning
					gpApp->m_bInhibitMakeTargetStringCall = FALSE;
				}
			}
		}

		// now we can get rid of the phrase box till wanted again
		// wx version just hides the phrase box
        gpApp->m_pTargetBox->HidePhraseBox(); // hides all three parts of the new phrasebox
        
        gpApp->m_pTargetBox->GetTextCtrl()->ChangeValue(_T("")); // need to set it to null str since it 
											   // won't get recreated
		gpApp->m_targetPhrase = _T("");

        // did we just replace in a retranslation, if so, reduce the kick off sequ num by
        // one otherwise anything matchable in the first srcPhrase after the retranslation
        // will not get matched (ie. check if the end of a retranslation precedes the
        // current location and if so, we'll want to make that the place we move forward
        // from so that we actually try to match in the first pile after the retranslation)
        // We don't need the adjustment if glossing is ON however.
		int nEarlierSN = nKickOffSequNum -1;
		CPile* pPile = pView->GetPile(nEarlierSN);
		bool bRetrans = pPile->GetSrcPhrase()->m_bRetranslation;
		if ( !gbIsGlossing && bRetrans)
			nKickOffSequNum = nEarlierSN;

		gbJustReplaced = FALSE;
		m_nCount = 0; // nothing currently matched
	}

    // in some situations (eg. after a merge in a replacement) a LayoutStrip call is
    // needed, otherwise the destruction of the targetBox window will leave an empty white
    // space at the active loc.
#ifdef _NEW_LAYOUT
		gpApp->m_pLayout->RecalcLayout(gpApp->m_pSourcePhrases, keep_strips_keep_piles);
#else
		gpApp->m_pLayout->RecalcLayout(gpApp->m_pSourcePhrases, create_strips_keep_piles);
#endif

	// restore the pointers which were clobbered
	CPile* pPile = pView->GetPile(gpApp->m_nActiveSequNum);
	gpApp->m_pActivePile = pPile;

	//TransferDataFromWindow(); // whm removed 21Nov11
	// whm added below 21Nov11 replacing TransferDataFromWindow()
	m_bIgnoreCase = m_pCheckIgnoreCase->GetValue();
//#if defined(FWD_SLASH_DELIM)
	// BEW added 23Apr15 -- need to change any / into ZWSP instances
	m_srcStr = FwdSlashtoZWSP(m_srcStr);
	m_tgtStr = FwdSlashtoZWSP(m_tgtStr);
//#endif
	m_srcStr = m_pEditSrc_Rep->GetValue();
	m_tgtStr = m_pEditTgt_Rep->GetValue();
	m_bIncludePunct = m_pCheckIncludePunct->GetValue();
	m_bSpanSrcPhrases = m_pCheckSpanSrcPhrases->GetValue();
	m_replaceStr = m_pEditReplace->GetValue();



	int nAtSequNum = gpApp->m_nActiveSequNum;
	int nCount = 1;

	gbFindIsCurrent = TRUE; // turn it back off after the active location cell is drawn

	// do the Find Next operation
	bool bFound;
	gpApp->LogUserAction(_T("pView->DoFindNext() executed in CReplaceDlg"));

	wxLogDebug(_T("%s:%s() line %d: sequnum %d , m_tgtStr %s , bool m_bTgtOnly %d , nAtSequNum %d"),
		__FILE__,__FUNCTION__,__LINE__,nKickOffSequNum,m_tgtStr.c_str(),(int)m_bTgtOnly,nAtSequNum);

	bFound = pView->DoFindNext(nKickOffSequNum,
									m_bIncludePunct,
									m_bSpanSrcPhrases,
									FALSE, //m_bSpecialSearch, // not used in this case
									m_bSrcOnly,
									m_bTgtOnly,
									m_bSrcAndTgt,
									FALSE, //m_bFindRetranslation, // not used in this case
									FALSE, //m_bFindNullSrcPhrase, // not used in this case
									FALSE, //m_bFindSFM,// not used in this case
									m_srcStr,
									m_tgtStr,
									m_sfm,
									m_bIgnoreCase,
									nAtSequNum,
									nCount);

	gbJustReplaced = FALSE;

	if (bFound)
	{
		gbFound = TRUE; // set the global
		gpApp->LogUserAction(_T("   Found!"));

		// enable the Replace and Replace All buttons
		wxASSERT(m_pButtonReplace != NULL);
		m_pButtonReplace->Enable(TRUE);
		
		wxASSERT(m_pButtonReplaceAll != NULL);
		m_pButtonReplaceAll->Enable(TRUE);

		// inform the dialog about how many were matched
		m_nCount = nCount;

		// place the dialog window so as not to obscure things
		// work out where to place the dialog window
		m_nTwoLineDepth = 2 * gpApp->m_nTgtHeight;
		if (gbGlossingVisible)
		{
			if (gbGlossingUsesNavFont)
				m_nTwoLineDepth += gpApp->m_nNavTextHeight;
			else
				m_nTwoLineDepth += gpApp->m_nTgtHeight;
		}
		m_ptBoxTopLeft = gpApp->m_pActivePile->GetCell(1)->GetTopLeft();
		wxRect rectScreen;
		rectScreen = wxGetClientDisplayRect();

		wxClientDC dc(gpApp->GetMainFrame()->canvas);
		pView->canvas->DoPrepareDC(dc); // adjust origin
		gpApp->GetMainFrame()->PrepareDC(dc); // wxWidgets' drawing.cpp sample also calls 
											  // PrepareDC on the owning frame
		if (!gbIsGlossing && gpApp->m_bMatchedRetranslation)
		{
			// use end of retranslation
			CCellList::Node* cpos = gpApp->m_selection.GetLast();
			CCell* pCell = (CCell*)cpos->GetData();
			wxASSERT(pCell != NULL); 
			CPile* pPile = pCell->GetPile();
			pCell = pPile->GetCell(1); // last line
			m_ptBoxTopLeft = pCell->GetTopLeft();

			int newXPos,newYPos;
			// CalcScrolledPosition translates logical coordinates to device ones. 
			gpApp->GetMainFrame()->canvas->CalcScrolledPosition(m_ptBoxTopLeft.x,
											m_ptBoxTopLeft.y,&newXPos,&newYPos);
			m_ptBoxTopLeft.x = newXPos;
			m_ptBoxTopLeft.y = newYPos;
		}
		else
		{
			// use location where phrase box would be put
			int newXPos,newYPos;
			// CalcScrolledPosition translates logical coordinates to device ones. 
			gpApp->GetMainFrame()->canvas->CalcScrolledPosition(m_ptBoxTopLeft.x,
											m_ptBoxTopLeft.y,&newXPos,&newYPos);
			m_ptBoxTopLeft.x = newXPos;
			m_ptBoxTopLeft.y = newYPos;
		}
		gpApp->GetMainFrame()->canvas->ClientToScreen(&m_ptBoxTopLeft.x,&m_ptBoxTopLeft.y);
		wxRect rectDlg;
		GetSize(&rectDlg.width, &rectDlg.height);
		// BEW 16Sep11, new dialog positioning code - it puts the dialog to left or right
		// and mostly up rather than down; to keep the landed location in view without the
		// user needing to scroll
		int myTopCoord;
		int myLeftCoord;
		RepositionDialogToUncoverPhraseBox(gpApp, 0, 0, rectDlg.width, rectDlg.height,
						m_ptBoxTopLeft.x, m_ptBoxTopLeft.y, myTopCoord, myLeftCoord);
		SetSize(myLeftCoord, myTopCoord, wxDefaultCoord, wxDefaultCoord, wxSIZE_USE_EXISTING);
		// end of new code on 16Sep11
		Update();
		// In wx version we seem to need to scroll to the found location
		gpApp->GetMainFrame()->canvas->ScrollIntoView(nAtSequNum); // whm added 7Jun07
	}
	else // bFound is FALSE
	{
        // whm 14May2020 added below for giving option to search earlier in document.
        // Notify user that end of document was reached and search item was not found.
        // If search didn't start at doc beginning, ask if the search should be made of the earlier part of
        // the document.
        wxString searchStr;
        if (m_bSrcOnly)
            searchStr = m_pEditSrc_Rep->GetValue();
        else if (m_bTgtOnly)
            searchStr = m_pEditTgt_Rep->GetValue();
        else if (m_bSrcAndTgt)
            searchStr = m_pEditSrc_Rep->GetValue(); // when searching both src and translated text the m_srcStr is used

        wxString msg;
        if (nKickOffSequNum == 0) // nKickOffSequNum was 0 at beginning of document
        {
            // nKickOffSequNum was at start of doc, and nothing was found
            msg = _("All of document was searched. No \"%s\" was found.");
            msg = msg.Format(msg, searchStr.c_str());
            // whm 15May2020 added below to supress phrasebox run-on due to handling of ENTER in CPhraseBox::OnKeyUp()
            gpApp->m_bUserDlgOrMessageRequested = TRUE;
            wxMessageBox(msg, _T(""), wxICON_INFORMATION);
            // pass throgh to pView->FindNextHasLanded() below...
        }
        else if (nKickOffSequNum > 0)
        {
            // nKickOffSequNum was NOT at start of doc, and nothing was found
            if (m_bSearchFromDocBeginning)
            {
                // We searched already from the beginning and nothing was found
                m_bSearchFromDocBeginning = FALSE;
                // starting point was reached and nothing found
                msg = _("Remainder of document was searched. No \"%s\" was found.");
                msg = msg.Format(msg, searchStr.c_str());
                // whm 15May2020 added below to supress phrasebox run-on due to handling of ENTER in CPhraseBox::OnKeyUp()
                gpApp->m_bUserDlgOrMessageRequested = TRUE;
                wxMessageBox(msg, _T(""), wxICON_INFORMATION);
            }
            else
            {
                // nKickOffSequNum was not at start of doc and nothing found, but earlier part of 
                // doc was not searched, so offer to search it.
                gpApp->LogUserAction(_T("   Not Found!"));
                m_nCount = 0; // none matched
                              // Search was initiated in middle of document
                msg = _("End of document was reached. No \"%s\" was found. Search for %s in the earlier part of document?");
                msg = msg.Format(msg, searchStr.c_str(), searchStr.c_str());
                int result;
                // whm 15May2020 added below to supress phrasebox run-on due to handling of ENTER in CPhraseBox::OnKeyUp()
                gpApp->m_bUserDlgOrMessageRequested = TRUE;
                result = wxMessageBox(msg, _T(""), wxICON_QUESTION | wxYES_NO | wxYES_DEFAULT);
                if (result == wxYES)
                {
                    m_bSearchFromDocBeginning = TRUE;
                    nKickOffSequNum = 0;
                    // Go to near beginning of DoFindNext() above to setup and do another Find operation
                    goto repeatfind;
                }
            }
        }


        gpApp->LogUserAction(_T("   Not Found!"));
		m_nCount = 0; // none matched
		gbFound = FALSE;

		// disable the Replace and Replace All buttons
		wxASSERT(m_pButtonReplace != NULL);
		m_pButtonReplace->Disable();
		
		wxASSERT(m_pButtonReplaceAll != NULL);
		m_pButtonReplaceAll->Disable();
		pView->FindNextHasLanded(gpApp->m_nActiveSequNum,FALSE); // show old active location
		
		Update();
		::wxBell();
	}
}

void CReplaceDlg::OnSpanCheckBoxChanged(wxCommandEvent& WXUNUSED(event))
{
	bool bCheckboxValue = m_pCheckSpanSrcPhrases->GetValue();
	if (bCheckboxValue)
	{
		// the user has requested matching be attempted across multiple piles, so in order
		// to ensure the user can see and adjust what is done before the next Find Next is
		// done, we suppress the Replace All option by disabling the button for it,
		// because Replace All, in order to work, must do an OnFindNext() after each
		// OnReplace() has been done. Note: we don't place with the gbReplaceAllIsCurrent
		// flag, it is unnecessary to do so because it is set only in the Replace All
		// button's handler, so disabling the button suffices.
		m_bSpanSrcPhrases = TRUE;
		m_pCheckSpanSrcPhrases->SetValue(TRUE);
		m_pButtonReplaceAll->Disable();
	}
	else
	{
		// the user has just unchecked the box for matching across piles, so re-enable the
		// Replace All button
		m_bSpanSrcPhrases = FALSE;
		m_pCheckSpanSrcPhrases->SetValue(FALSE);
		m_pButtonReplaceAll->Enable();
	}
}

void CReplaceDlg::OnUpdateReplaceAllButton(wxUpdateUIEvent& event)
{
    // whm 16May2020 added. The Replace All button should be disabled if the Replacement Text edit box is empty,
    // or if the Translation text box is empty. Both edit boxes need to have some text value before the Replace
    // All button is enabled. These conditions can be combined with m_bSpanSrcPhrases
    
	if (m_bSpanSrcPhrases || m_pEditTgt_Rep->GetValue().IsEmpty() || m_pEditReplace->GetValue().IsEmpty())
	{
		// keep the Replace All button disabled
		event.Enable(FALSE);
	}
	else
	{
        // When m_bSpanSrcPhrases is FALSE, and both the Translation edit box and the Replacement edit box have text
		// keep the Replace All button enabled
		event.Enable(TRUE);
	}
}

// whm 16May2020 added the following OnUpdateReplace() handler
// to keep the Replace button disabled until there is text in both
// the Translation edit box as well as the Replacement Text edit box.
// In addition, the Replace button should not be enabled unless a Find Next
// has been executed - resulting in a source text selection being active.
void CReplaceDlg::OnUpdateReplace(wxUpdateUIEvent & event)
{
    if (gpApp->m_selection.GetCount() == 0)
    {
        event.Enable(FALSE);
    }
    else if (m_pEditTgt_Rep->GetValue().IsEmpty() || m_pEditReplace->GetValue().IsEmpty())
    {
        // keep the Replace All button disabled
        event.Enable(FALSE);
    }
    else
    {
        // When m_bSpanSrcPhrases is FALSE, and both the Translation edit box and the Replacement edit box have text
        // keep the Replace All button enabled
        event.Enable(TRUE);
    }
}

// whm 16May2020 added the following OnUpdateFindNext() handler
// to keep the Find Next button disabled until there is text in both
// the Translation edit box as well as the Replacement Text edit box.
void CReplaceDlg::OnUpdateFindNext(wxUpdateUIEvent & event)
{
    if (m_pEditTgt_Rep->GetValue().IsEmpty() || m_pEditReplace->GetValue().IsEmpty())
    {
        // keep the Replace All button disabled
        event.Enable(FALSE);
    }
    else
    {
        // When m_bSpanSrcPhrases is FALSE, and both the Translation edit box and the Replacement edit box have text
        // keep the Replace All button enabled
        event.Enable(TRUE);
    }
}

void CReplaceDlg::DoRadioSrcOnly() 
{
	wxASSERT(m_pStaticSrcBoxLabel != NULL);
	m_pStaticSrcBoxLabel->Show(TRUE);
	
	wxASSERT(m_pEditSrc_Rep != NULL);
	m_pEditSrc_Rep->SetFocus();
    // whm 3Aug2018 Note: TODO: I assume the select all below is appropriate
    // for the replace dialog, and shouldn't be suppressed if 'Select Copied Source'
    // menu item is NOT ticked.
    m_pEditSrc_Rep->SetSelection(-1,-1); // -1,-1 selects all
	m_pEditSrc_Rep->Show(TRUE);
	
	wxASSERT(m_pStaticTgtBoxLabel != NULL);
	m_pStaticTgtBoxLabel->Hide();
	
	wxASSERT(m_pEditTgt_Rep != NULL);
	m_pEditTgt_Rep->Hide();
	m_bSrcOnly = TRUE;
	m_bTgtOnly = FALSE;
	m_bSrcAndTgt = FALSE;
}

void CReplaceDlg::DoRadioTgtOnly() 
{
	wxASSERT(m_pStaticSrcBoxLabel != NULL);
	m_pStaticSrcBoxLabel->Hide();
	
	wxASSERT(m_pEditSrc_Rep != NULL);
	m_pEditSrc_Rep->Hide();

    // whm 14May2020 added set focus in the m_pEditTgt box
    m_pEditTgt_Rep->SetFocus();

    wxASSERT(m_pStaticTgtBoxLabel != NULL);
	wxString str;
	str = _("        Translation:"); //IDS_TRANS
	m_pStaticTgtBoxLabel->SetLabel(str);
	m_pStaticTgtBoxLabel->Show(TRUE);
	
	wxASSERT(m_pEditTgt_Rep != NULL);
	m_pEditTgt_Rep->SetFocus();
    // whm 3Aug2018 Note: TODO: I assume the select all below is appropriate
    // for the replace dialog, and shouldn't be suppressed if 'Select Copied Source'
    // menu item is NOT ticked.
    m_pEditTgt_Rep->SetSelection(-1,-1); // -1,-1 selects all
	m_pEditTgt_Rep->Show(TRUE);
	m_bSrcOnly = FALSE;
	m_bTgtOnly = TRUE;
	m_bSrcAndTgt = FALSE;
}

void CReplaceDlg::DoRadioSrcAndTgt() 
{
	wxASSERT(m_pStaticSrcBoxLabel != NULL);
	m_pStaticSrcBoxLabel->Show(TRUE);
	
	wxASSERT(m_pEditSrc_Rep != NULL);
	m_pEditSrc_Rep->Show(TRUE);
	
	wxASSERT(m_pStaticTgtBoxLabel != NULL);
	wxString str;
	str = _("With Translation:"); //IDS_WITH_TRANS
	m_pStaticTgtBoxLabel->SetLabel(str);
	m_pStaticTgtBoxLabel->Show(TRUE);
	
	wxASSERT(m_pEditTgt_Rep != NULL);
	m_pEditTgt_Rep->SetFocus();
    // whm 3Aug2018 Note: TODO: I assume the select all below is appropriate
    // for the replace dialog, and shouldn't be suppressed if 'Select Copied Source'
    // menu item is NOT ticked.
    m_pEditTgt_Rep->SetSelection(-1,-1); // -1,-1 selects all
	m_pEditTgt_Rep->Show(TRUE);
	m_bSrcOnly = FALSE;
	m_bTgtOnly = FALSE;
	m_bSrcAndTgt = TRUE;	
}

// whm 14May2020 addition. The following OnSrcEditBoxEnterKeyPress() function is triggered from
// the EVENT_TABLE macro line: EVT_TEXT_ENTER(IDC_EDIT_SRC_FIND, CFindDlg::OnSrcditBoxEnterKeyPress)
// when an ENTER key is pressed from within the Source Text wxTextCtrl, which has its
// wxTE_PROCESS_ENTER style flag set. Here we set up a default action we call the DoFindNext() handler 
// function to make it be the default action triggered by the ENTER key press.
void CReplaceDlg::OnSrcEditBoxEnterKeyPress(wxCommandEvent & WXUNUSED(event))
{
    // whm 15May2020 Note: gpApp->m_bUserDlgOrMessageRequested is set to TRUE in this->InitDialog()
    // but it becomes FALSE again (by code elsewhere) after the first Find Next is done. 
    // For modeless dialogs - which can execute ENTER-generated commands multiple times
    // setting gpApp->m_bUserDlgOrMessageRequested to TRUE in the dialog's InitDialos() 
    // only is not sufficient. Here in CFind, it needs to be set to TRUE in each DoFindNext() call 
    // in order to supress all phrasebox run-on instances due to ENTER key getting 
    // propagated on to CPhraseBox::OnKeyUp().
    wxLogDebug(_T("CReplaceDlg::OnSrcEditBoxEnterKeyPress() handling WXK_RETURN key"));
    this->DoFindNext();
    // don't call event.Skip() here, just return.
    return; // event.Skip();
}

// whm 14May2020 addition. The following OnTgtEditBoxEnterKeyPress() function is triggered from
// the EVENT_TABLE macro line: EVT_TEXT_ENTER(IDC_EDIT_TGT_FIND, CFindDlg::OnTgtditBoxEnterKeyPress)
// when an ENTER key is pressed from within the Translation wxTextCtrl, which has its
// wxTE_PROCESS_ENTER style flag set. Here we set up a default action we call the DoFindNext() handler 
// function to make it be the default action triggered by the ENTER key press.
void CReplaceDlg::OnTgtEditBoxEnterKeyPress(wxCommandEvent & WXUNUSED(event))
{
    // whm 15May2020 Note: gpApp->m_bUserDlgOrMessageRequested is set to TRUE in this->InitDialog()
    // but it becomes FALSE again (by code elsewhere) after the first Find Next is done. 
    // For modeless dialogs - which can execute ENTER-generated commands multiple times
    // setting gpApp->m_bUserDlgOrMessageRequested to TRUE in the dialog's InitDialos() 
    // only is not sufficient. Here in CReplaceDlg, it needs to be set to TRUE in each DoFindNext() call 
    // in order to supress all phrasebox run-on instances due to ENTER key getting 
    // propagated on to CPhraseBox::OnKeyUp().
    wxLogDebug(_T("CReplaceDlg::OnTgtEditBoxEnterKeyPress() handling WXK_RETURN key"));
    this->DoFindNext();
    // don't call event.Skip() here, just return.
    return; // event.Skip();
}

void CReplaceDlg::OnReplaceEditBoxEnterKeyPress(wxCommandEvent & WXUNUSED(event))
{
    // whm 15May2020 Note: gpApp->m_bUserDlgOrMessageRequested is set to TRUE in this->InitDialog()
    // but it becomes FALSE again (by code elsewhere) after the first Find Next is done. 
    // For modeless dialogs - which can execute ENTER-generated commands multiple times
    // setting gpApp->m_bUserDlgOrMessageRequested to TRUE in the dialog's InitDialos() 
    // only is not sufficient. Here in CReplaceDlg, it needs to be set to TRUE in each DoFindNext() call 
    // in order to supress all phrasebox run-on instances due to ENTER key getting 
    // propagated on to CPhraseBox::OnKeyUp().
    // Here we need to check is a source text selection is active or not. If so we can call
    // the OnReplaceButton() handler, but if no source text selection is active it means that
    // we haven't yet done a FindNext operation, and we should do that operation first in
    // response to the Enter key proes in this Replacement Text box.
    if (gpApp->m_selection.GetCount() == 0)
    {
        // No selection is active so execute the DoFindNext handler first to locate the target string
        // and get tha source location selected.
        wxLogDebug(_T("CReplaceDlg::OnTgtEditBoxEnterKeyPress() handling WXK_RETURN key - executing DoFindNext()"));
        this->DoFindNext();
    }
    else
    {
        // There is a source text selection so execute the OnReplaceButton() handler
        wxLogDebug(_T("CReplaceDlg::OnReplaceEditBoxEnterKeyPress() handling WXK_RETURN key"));
        wxCommandEvent dummyevent;
        this->OnReplaceButton(dummyevent);
    }

    // don't call event.Skip() here, just return.
    return; // event.Skip();
}

void CReplaceDlg::OnReplaceButton(wxCommandEvent& event) 
{
    // whm 15May2020 Note: gpApp->m_bUserDlgOrMessageRequested is set to TRUE in this->InitDialog()
    // but it becomes FALSE again (by code elsewhere) after the first Replace is done. 
    // For modeless dialogs - which can execute ENTER-generated commands multiple times
    // setting gpApp->m_bUserDlgOrMessageRequested to TRUE in the dialog's InitDialos() 
    // only is not sufficient. Here in CReplaceDlg, it needs to be set to TRUE in each 
    // OnReplaceButton() call in order to supress all phrasebox run-on instances due to
    // ENTER key getting propagated on to CPhraseBox::OnKeyUp().
    gpApp->m_bUserDlgOrMessageRequested = TRUE;

    // whm added 15Mar12 for read-only mode
	if (gpApp->m_bReadOnlyAccess)
	{
		::wxBell();
		return;
	}
	
	//TransferDataFromWindow(); // whm removed 21Nov11
	// whm added below 21Nov11 replacing TransferDataFromWindow()
	m_bIgnoreCase = m_pCheckIgnoreCase->GetValue();
	m_srcStr = m_pEditSrc_Rep->GetValue();
	m_tgtStr = m_pEditTgt_Rep->GetValue();
	m_bIncludePunct = m_pCheckIncludePunct->GetValue();
	m_bSpanSrcPhrases = m_pCheckSpanSrcPhrases->GetValue();
	m_replaceStr = m_pEditReplace->GetValue();
//#if defined(FWD_SLASH_DELIM)
	// BEW added 23Apr15 -- need to change any / into ZWSP instances
	m_srcStr = FwdSlashtoZWSP(m_srcStr);
	m_tgtStr = FwdSlashtoZWSP(m_tgtStr);
	m_replaceStr = FwdSlashtoZWSP(m_replaceStr);
//#endif

	// do nothing if past the end of the last srcPhrase, ie. at the EOF
	if (gpApp->m_nActiveSequNum == -1)
	{	
		wxASSERT(gpApp->m_pActivePile == 0);
		::wxBell();
		return;
	}

	// check there is a selection
	m_bSelectionExists = TRUE;
	if (gpApp->m_selection.GetCount() == 0)
	{
		m_bSelectionExists = FALSE;
		// IDS_NO_SELECTION_YET
        // whm 15May2020 added below to supress phrasebox run-on due to handling of ENTER in CPhraseBox::OnKeyUp()
        gpApp->m_bUserDlgOrMessageRequested = TRUE;
        wxMessageBox(_(
"Sorry, you cannot do a Replace operation because there is no selection yet."),
		_T(""), wxICON_INFORMATION | wxOK);
		gbJustReplaced = FALSE;
		return;
	}

	// check that m_nCount value is same as number of cells selected in the view
	wxASSERT(m_nCount == (int)gpApp->m_selection.GetCount());
	
	bool bOK = gpApp->GetView()->DoReplace(gpApp->m_nActiveSequNum,m_bIncludePunct,
											m_tgtStr,m_replaceStr,m_nCount);

    // clear the global strings, in case they were set by OnButtonMerge() (we can safely
    // not clear them, since DoReplace clears them when first entered, but its best to play
    // safe
	gOldConcatStr = _T("");
	gOldConcatStrNoPunct = _T("");

	if (!bOK)
	{
        // whm 15May2020 added below to supress phrasebox run-on due to handling of ENTER in CPhraseBox::OnKeyUp()
        gpApp->m_bUserDlgOrMessageRequested = TRUE;
        wxMessageBox(_(
		"Failed to do the replacement without error. The next Find Next will be skipped."),
		_T(""), wxICON_INFORMATION | wxOK);
		return;
	}
	
    // let everyone know that a replace was just done (this will have put phrase box at the
    // active location in order to make the replacement, so the view state will not be the
    // same if OnCancel is called next, cf. if the latter was called after a Find which
    // matched something & user did not replace - in the latter case the phrase box will
    // have been destroyed and the view will only show a selection at the active loc.
	gbJustReplaced = TRUE;

    // BEW changed 17Jun09, because having an automatic OnFindNext() call after a
    // replacment means that the user gets no chance to see and verify that what has
    // happened is actually what he wanted to happen. We aren't dealing with connected
    // text, so the protocols for that scenario don't apply here; the discrete
    // CSourcePhrase break up of the original text, and the need to ensure that
    // associations between source & target going into the KB are valid associations,
    // requires that the user be able to make a visual check and press Find Next button
    // again only after he's satisfied and/or fixed it up to be as he wants after the
    // replacement has been made; however, a Replace All button press (we allow it only if
    // not matching across multiple piles) must be allowed to work - so we allow it in that
    // circumstance
    // 
	// a replace must be followed by an attempt to find next match, so do it (see comment
	// above)
	if (gbReplaceAllIsCurrent)
	{
		OnFindNext(event);
	}
}

void CReplaceDlg::OnReplaceAllButton(wxCommandEvent& event) 
{
	// whm added 15Mar12 for read-only mode
	if (gpApp->m_bReadOnlyAccess)
	{
		::wxBell();
		return;
	}
	
	//TransferDataFromWindow(); // whm removed 21Nov11
	// whm added below 21Nov11 replacing TransferDataFromWindow()
	m_bIgnoreCase = m_pCheckIgnoreCase->GetValue();
	m_srcStr = m_pEditSrc_Rep->GetValue();
	m_tgtStr = m_pEditTgt_Rep->GetValue();
	m_bIncludePunct = m_pCheckIncludePunct->GetValue();
	m_bSpanSrcPhrases = m_pCheckSpanSrcPhrases->GetValue();
	m_replaceStr = m_pEditReplace->GetValue();
//#if defined(FWD_SLASH_DELIM)
	// BEW added 23Apr15 -- need to change any / into ZWSP instances
	m_srcStr = FwdSlashtoZWSP(m_srcStr);
	m_tgtStr = FwdSlashtoZWSP(m_tgtStr);
	m_replaceStr = FwdSlashtoZWSP(m_replaceStr);
//#endif

	// do nothing if past the end of the last srcPhrase, ie. at the EOF
	if (gpApp->m_nActiveSequNum == -1)
	{	
		wxASSERT(gpApp->m_pActivePile == 0);
		::wxBell();
		return;
	}

	if (gpApp->m_selection.GetCount() == 0)
	{
		// no selection yet, so do a Find Next
		OnFindNext(event);
		if (!gbFound)
		{
			// return, nothing was matched
			::wxBell();
			return;
		}
	}

    // if we get here then we have a selection, and the OnIdle() handler can now start
    // doing the replacements following by finds, till no match happens or eof reached
	gpApp->LogUserAction(_T("Replace All executed"));
	Hide();
	gbReplaceAllIsCurrent = TRUE;
	
	pReplaceDlgSizer->Layout(); // force the top dialog sizer to re-layout the dialog
}

bool CReplaceDlg::OnePassReplace()
{
	wxCommandEvent event;
	OnReplaceButton(event); // includes an OnFindNext() call after the DoReplace()

	if (gbFound)
		return TRUE;
	else
		return FALSE;
}

// BEW 26Mar10, no changes needed for support of doc version 5
void CReplaceDlg::OnCancel(wxCommandEvent& WXUNUSED(event)) 
{
	CAdapt_ItView* pView = gpApp->GetView();
	wxASSERT(gpApp->m_pReplaceDlg != NULL);

	// clear the globals
	gpApp->m_bMatchedRetranslation = FALSE;
	gnRetransEndSequNum = -1;

	// destroying the window, but first clear the variables to defaults
	m_srcStr = _T("");
	m_tgtStr = _T("");
	m_replaceStr = _T("");

	m_markerStr = _T("");
	m_sfm = _T("");

	m_bSrcOnly = TRUE;
	m_bTgtOnly = FALSE;
	m_bSrcAndTgt = FALSE;

	m_bFindDlg = FALSE; // it is TRUE in the CFindDlg
	m_bReplaceDlg = FALSE; // BEW added 3Apr21

	//TransferDataToWindow(); // whm removed 21Nov11
	// whm added below 21Nov11 replacing TransferDataToWindow()
	m_pCheckIgnoreCase->SetValue(m_bIgnoreCase);
	m_pEditSrc_Rep->ChangeValue(m_srcStr);
	m_pEditTgt_Rep->ChangeValue(m_tgtStr);
	m_pCheckIncludePunct->SetValue(m_bIncludePunct);
	m_pCheckSpanSrcPhrases->SetValue(m_bSpanSrcPhrases);
	m_pEditReplace->ChangeValue(m_replaceStr);

    // whm 17May2020 set the flag just before Destroy() call in this OnCancel() handler
    gpApp->m_bUserDlgOrMessageRequested = TRUE;
	Destroy();

	gbFindIsCurrent = FALSE;

	if (gbJustReplaced)
	{
        // we have cancelled just after a replacement, so we expect the phrase box to exist
        // and be visible, so we only have to do a little tidying up before we return whm
        // 12Aug08 Note: In the wx version m_pTargetBox is never NULL. Therefore, I think
        // we should remove the == NULL check below and always jump to the "longer" cleanup
        // in the else block below.
		//if (gpApp->m_pTargetBox == NULL)
		// whm Note: moved the gbJustReplaced = FALSE statement here 12Aug08 because in
		// its placement below, it would never be reset to FALSE. The logic is not real
		// clear, however, because if in the MFC version the target box were NULL, the
		// goto a jump would be made without resetting the boolean (which seems to me
		// to be faulty logic somewhere).
		gbJustReplaced = FALSE; // clear to default value 
		goto a; // check, just in case, and do the longer cleanup if the box is not there
	}
	else
	{
        // we have tried a FindNext since the previous replacement, so we expect the phrase
        // box to have been destroyed by the time we enter this code block; so place the
        // phrase box, if it has been destroyed
		// wx version the phrase box always exists
        // whm 11Aug08 - here we need to remove the m_pTargetBox == NULL test, because in
        // the MFC version we "expect the phrase box to have been destroyed by the time we
        // enter this code block". But we don't destroy the phrase box in the wx version so
        // we should always execute the following block.
		//if (gpApp->m_pTargetBox == NULL)
		//{
			// in wx version this block should never execute
a:			CCell* pCell = 0;
			CPile* pPile = 0;
			if (!gpApp->m_selection.IsEmpty())
			{
				CCellList::Node* cpos = gpApp->m_selection.GetFirst();
				pCell = cpos->GetData(); // could be on any line
				wxASSERT(pCell != NULL);
				pPile = pCell->GetPile();
			}
			else
			{
				// no selection, so find another way to define active location 
				// & place the phrase box
				int nCurSequNum = gpApp->m_nActiveSequNum;
				if (nCurSequNum == -1)
				{
					nCurSequNum = gpApp->GetMaxIndex(); // make active loc the 
														// last src phrase in the doc
					gpApp->m_nActiveSequNum = nCurSequNum;
				}
				else if (nCurSequNum >= 0 && nCurSequNum <= gpApp->GetMaxIndex())
				{
					gpApp->m_nActiveSequNum = nCurSequNum;
				}
				else
				{
					// if all else fails, go to the start
					gpApp->m_nActiveSequNum = 0;
				}
				pPile = pView->GetPile(gpApp->m_nActiveSequNum);
			}

			// preserve enough information to be able to recreate the appropriate selection
			// since placing the phrase box will clobber the current selection
			CSourcePhrase* pSrcPhrase = pPile->GetSrcPhrase();
			int nCount = 0;
			if (!gpApp->m_selection.IsEmpty())
			{
				nCount = gpApp->m_selection.GetCount();
				wxASSERT(nCount >0);
			}
			int nSaveSelSequNum = pSrcPhrase->m_nSequNumber; // if in a retrans, 
                                // selection will not be where phrase box ends up

            // pPile is what we will use for the active pile, so set everything up there,
            // provided it is not in a retranslation - if it is, place the box preceding
            // it, if possible; but if glossing is ON, we can have the box within a
            // retranslation in which case ignore the block of code
			CPile* pSavePile = pPile;
			if (!gbIsGlossing)
			{
				while (pSrcPhrase->m_bRetranslation)
				{
					pPile = pView->GetPrevPile(pPile);
					if (pPile == NULL)
					{
						// if we get to the start, try again, going the other way
						pPile = pSavePile;
						while (pSrcPhrase->m_bRetranslation)
						{
							pPile = pView->GetNextPile(pPile);
							wxASSERT(pPile != NULL); // we'll assume this will never fail
							pSrcPhrase = pPile->GetSrcPhrase();
						}
						break;
					}
					pSrcPhrase = pPile->GetSrcPhrase();
				}
			}
			pSrcPhrase = pPile->GetSrcPhrase();
			gpApp->m_nActiveSequNum = pSrcPhrase->m_nSequNumber;
			gpApp->m_pActivePile = pPile;
			pCell = pPile->GetCell(1); // we want the 2nd line, for phrase box

			// scroll into view, just in case (but shouldn't be needed)
			gpApp->GetMainFrame()->canvas->ScrollIntoView(gpApp->m_nActiveSequNum);

			// place the phrase box
			gbJustCancelled = TRUE;
			pView->PlacePhraseBox(pCell,2);

			// get a new active pile pointer, the PlacePhraseBox call did a recal of the layout
			gpApp->m_pActivePile = pView->GetPile(gpApp->m_nActiveSequNum);
			wxASSERT(gpApp->m_pActivePile != NULL);

			// get a new pointer to the pile at the start of the selection, since the recalc
			// also clobbered the old one
			CPile* pSelPile = pView->GetPile(nSaveSelSequNum);
			wxASSERT(pSelPile != NULL);

			pView->Invalidate(); // get window redrawn
			gpApp->m_pLayout->PlaceBox();

			// restore focus to the targetBox
			if (gpApp->m_pTargetBox != NULL)
			{
				if (gpApp->m_pTargetBox->IsShown())
				{
                    gpApp->m_pTargetBox->SetFocusAndSetSelectionAtLanding();// whm 13Aug2018 modified
                }
			}

			// recreate the selection to be in line 1, hence ignoring boundary flags
			CCell* pAnchorCell = pSelPile->GetCell(0); // first line, index 0 cell
			if (nCount > 0)
			{
				gpApp->m_pAnchor = pAnchorCell;
				CCellList* pSelection = &gpApp->m_selection;
				wxASSERT(pSelection->IsEmpty());
				gpApp->m_selectionLine = 0;
				wxClientDC aDC(gpApp->GetMainFrame()->canvas); // make a device context

				// then do the new selection, start with the anchor cell

				aDC.SetBackgroundMode(gpApp->m_backgroundMode);
				aDC.SetTextBackground(wxColour(255,255,0)); // yellow
				pAnchorCell->DrawCell(&aDC, gpApp->m_pLayout->GetSrcColor());
				gpApp->m_bSelectByArrowKey = FALSE;
				pAnchorCell->SetSelected(TRUE);

				// preserve record of the selection
				pSelection->Append(pAnchorCell);

				// extend the selection to the right, if more than one pile is involved
				if (nCount > 1)
				{
					// extend the selection (shouldn't be called when glossing is ON
					// because we inhibit matching across piles in that circumstance)
					pView->ExtendSelectionForFind(pAnchorCell,nCount);
				}
			}
	}

	gbFindOrReplaceCurrent = FALSE; // turn it back off

	gpApp->m_pReplaceDlg = (CReplaceDlg*)NULL;

	//	wxDialog::OnCancel(); //don't call base class because we 
	//	                      // are modeless - use this only if its modal
}

void CReplaceDlg::OnFindNext(wxCommandEvent& WXUNUSED(event))
{
	DoFindNext();
}

void CReplaceDlg::OnRadioSrcOnly(wxCommandEvent& WXUNUSED(event))
{
	DoRadioSrcOnly();
	pReplaceDlgSizer->Layout(); // force the top dialog sizer to re-layout the dialog
}

void CReplaceDlg::OnRadioTgtOnly(wxCommandEvent& WXUNUSED(event))
{
	DoRadioTgtOnly();
	pReplaceDlgSizer->Layout(); // force the top dialog sizer to re-layout the dialog
}

void CReplaceDlg::OnRadioSrcAndTgt(wxCommandEvent& WXUNUSED(event))
{
	DoRadioSrcAndTgt();
	pReplaceDlgSizer->Layout(); // force the top dialog sizer to re-layout the dialog
}

