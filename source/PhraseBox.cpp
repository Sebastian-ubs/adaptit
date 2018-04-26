/////////////////////////////////////////////////////////////////////////////
/// \project		adaptit
/// \file			PhraseBox.cpp
/// \author			Bill Martin
/// \date_created	11 February 2004
/// \rcs_id $Id$
/// \copyright		2008 Bruce Waters, Bill Martin, SIL International
/// \license		The Common Public License or The GNU Lesser General Public License (see license directory)
/// \description	This is the implementation file for the CPhraseBox class.
/// The CPhraseBox class governs the behavior of the phrase or
/// target box where the user enters and/or edits translations while adapting text.
/// \derivation		The PhraseBox class derives from the wxTextCtrl class.
/////////////////////////////////////////////////////////////////////////////

// the following improves GCC compilation performance
#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma implementation "PhraseBox.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// defines for debugging purposes
//#define _FIND_DELAY
//#define _AUTO_INS_BUG
//#define LOOKUP_FEEDBACK


#ifndef WX_PRECOMP
// Include your minimal set of headers here, or wx.h
#include <wx/wx.h>
#endif

#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/odcombo.h>

// Other includes uncomment as implemented
#include "Adapt_It.h"
#include "PhraseBox.h"
#include "Adapt_ItView.h"
#include "Adapt_ItCanvas.h"
#include "Cell.h"
#include "Pile.h"
#include "Strip.h"
#include "SourcePhrase.h"
#include "Adapt_ItDoc.h"
#include "Layout.h"
#include "RefString.h"
#include "AdaptitConstants.h"
#include "KB.h"
#include "TargetUnit.h"
#include "ChooseTranslation.h"
#include "MainFrm.h"
#include "Placeholder.h"
#include "helpers.h"
// Other includes uncomment as implemented

// globals

extern bool gbVerticalEditInProgress;
extern EditStep gEditStep;
extern EditRecord gEditRecord;
extern CAdapt_ItApp* gpApp; // to access it fast

// for support of auto-capitalization

/// This global is defined in Adapt_It.cpp.
extern bool	gbAutoCaps;

/// This global is defined in Adapt_It.cpp.
extern bool	gbSourceIsUpperCase;

/// This global is defined in Adapt_It.cpp.
extern bool	gbNonSourceIsUpperCase;

/// This global is defined in Adapt_It.cpp.
extern bool	gbMatchedKB_UCentry;

/// This global is defined in Adapt_It.cpp.
extern wxChar gcharNonSrcUC;

/// This global is defined in Adapt_ItView.cpp.
extern bool	gbIsGlossing; // when TRUE, the phrase box and its line have glossing text

/// This global is defined in Adapt_ItView.cpp.
extern bool gbGlossingUsesNavFont;

/// This global is defined in Adapt_It.cpp.
extern bool	gbRTL_Layout;	// ANSI version is always left to right reading; this flag can only
							// be changed in the NRoman version, using the extra Layout menu

extern int ID_PHRASE_BOX; // Note: int ID_PHRASE_BOX = 22030 is defined in Adapt_ItView.cpp 

//IMPLEMENT_DYNAMIC_CLASS(CPhraseBox, wxTextCtrl)
IMPLEMENT_DYNAMIC_CLASS(CPhraseBox, wxOwnerDrawnComboBox)

BEGIN_EVENT_TABLE(CPhraseBox, wxOwnerDrawnComboBox)
	EVT_MENU(wxID_UNDO, CPhraseBox::OnEditUndo)
	EVT_TEXT(ID_PHRASE_BOX, CPhraseBox::OnPhraseBoxChanged)
	EVT_CHAR(CPhraseBox::OnChar)
	EVT_KEY_DOWN(CPhraseBox::OnKeyDown)
	EVT_KEY_UP(CPhraseBox::OnKeyUp)
	EVT_LEFT_DOWN(CPhraseBox::OnLButtonDown)
	EVT_LEFT_UP(CPhraseBox::OnLButtonUp)
    EVT_COMBOBOX(ID_PHRASE_BOX, CPhraseBox::OnComboItemSelected)
    // Process a wxEVT_COMBOBOX_DROPDOWN event, which is generated when the 
    // list box part of the combo box is shown (drops down). Notice that this 
    // event is only supported by wxMSW, wxGTK with GTK+ 2.10 or later, and wxOSX/Cocoa
#if wxVERSION_NUMBER >= 2900
    EVT_COMBOBOX_DROPDOWN(ID_PHRASE_BOX, CPhraseBox::OnComboProcessDropDownListOpen)

    // Process a wxEVT_COMBOBOX_CLOSEUP event, which is generated when the list 
    // box of the combo box disappears (closes up). This event is only generated 
    // for the same platforms as wxEVT_COMBOBOX_DROPDOWN above. Also note that
    // only wxMSW and wxOSX/Cocoa support adding or deleting items in this event
    EVT_COMBOBOX_CLOSEUP(ID_PHRASE_BOX, CPhraseBox::OnComboProcessDropDownListCloseUp)
#endif
    END_EVENT_TABLE()

CPhraseBox::CPhraseBox(void)
{
    // whm 10Jan2018 Note: This default constructor will never be called in
    // the current codebase. The PhraseBox is only created in one location in
    // the codebase in the CAdapt_ItView::OnCreate() function. 
    /*
	// Problem: The MFC version destroys and recreates the phrasebox every time
	// the box is moved, layout changes, screen is redrawn, etc. In fact, it seems
	// often to be the case that the phrase box contents can remain unchanged, and
	// yet the phrase box itself can go through multiple deletions, and recreations.
	// The MFC design makes it impossible to keep track of a phrase box "dirty" flag
	// from here within the CPhraseBox class. It seems I could either keep a "dirty"
	// flag on the App, or else redesign the TargetBox/PhraseBox in such a way that
	// it doesn't need to be destroyed and recreated all the time, but can exist
	// at least for the life of a view (on the heap), and be hidden, moved, and
	// shown when needed. I've chosen the latter.

    m_textColor = wxColour(0, 0, 0); // default to black
    m_bCurrentCopySrcPunctuationFlag = TRUE; // default
    // whm Note: The above initializations composed all CPhraseBox constructor initializations
    // before the addition of the custom constructor below for the dropdown phrasebox.

    */
}

CPhraseBox::CPhraseBox(
    wxWindow * parent,
    wxWindowID id,
    const wxString & value,
    const wxPoint & pos,
    const wxSize & size,
    const wxArrayString & choices,
    long style)
    : wxOwnerDrawnComboBox(parent,
        id,
        value,
        pos,
        size,
        choices,
        style) // style is wxCB_DROPDOWN | wxTE_PROCESS_ENTER
{
    // whm 10Jan2018 Note: This custom constructor is now the only constructor that will
    // be called in the current codebase. The PhraseBox is only created in one location in
    // the codebase in the CAdapt_ItView::OnCreate() function. 

    // This member repeated here from the default constructor
    m_textColor = wxColour(0,0,0); // default to black
    
    // This member repeated here from the default constructor
    m_bCurrentCopySrcPunctuationFlag = TRUE; // default

    // whm Note: The above members were all repeated here in the custom constructor from the
    // original default constructor. 
    // The following CPhraseBox members were moved here and renamed from global space. 
    // Some subsequently removed or moved (and commented out here) as mentioned in comments.
    // The original comments that appeared with the globals are preserved under my new comments
    // after a blank comment line.

    // whm 24Feb2018 The m_bMergeSucceeded member was originally named gbMergeSucceeded.
    // It was originally declared in PhraseBox.cpp's global space (but not initialized there).
    // Although I initially had it as a member of CPhraseBox, I moved it to become a member of 
    // CAdapt_ItApp and initialized it there where it still functions as intended.
    //
    // [no original documenting comment] 
    // whm Note: m_bMergeSucceeded is used in View's OnReplace() function, and in
    // CPhraseBox::OnPhraseBoxChanged() and CPhraseBox::OnChar() where it functions as intended.
    //m_bMergeSucceeded = FALSE; // whm Note: functions as intended as App member

    // whm 24Feb2018 The m_bSuppressDefaultAdaptation member was originally named bSuppressDefaultAdaptation.
    // It was originally declared in PhraseBox's global space (but not initialised there).
    // Although I initially had it as a member of CPhraseBox, I moved it to become a member of
    // CAdapt_ItApp and initialized it there where it still functions as intended.
    //
    // m_bSuppressDefaultAdaptation normally FALSE, but set TRUE whenever user is
    // wanting a MergeWords done by typing into the phrase box (which also
    // ensures cons.changes won't be done on the typing)- actually more complex than
    // this, see CPhraseBox OnChar()
    //m_bSuppressDefaultAdaptation = FALSE; // whm Note: functions as intended as App member

    // whm 24Feb2018 The pCurTargetUnit member was originally declared in 
    // PhraseBox's global space (initialized to NULL).
    // Although I initially had it as a member of CPhraseBox, I moved it to become a member of
    // CAdapt_ItAPP and initialized it there where it still functions as intended.
    //
    // when non-NULL, pCurTargetUnit is the matched CTargetUnit instance from the Choose Translation dialog
    //pCurTargetUnit = (CTargetUnit*)NULL; // whm Note: functions as intended as App member

    // whm 24Feb2018 moved to constructor and initialized here via .Empty(). It originally was 
    // named gSaveTargetPhrase and initialized to _T("") in PhraseBox.cpp's global space. 
    //
    // m_SaveTargetPhrase for use by the SHIFT+END shortcut for unmerging a phrase
    m_SaveTargetPhrase.Empty(); 

    // whm 24Feb2018 moved to constructor and initialized here to FALSE. It originally was
    // named gbRetainBoxContents and was declared in PhraseBox.cpp's global space.
    //
    // for version 1.4.2; we want deselection of copied source text to set the 
    // m_bRetainBoxContents flag true so that if the user subsequently selects words intending
    // to do a merge, then the deselected word won't get lost when he types something after
    // forming the source words selection (see OnKeyUp( ) for one place the flag is set -
    // (for a left or right arrow keypress), and the other place will be in the view's
    // OnLButtonDown I think - for a click on the phrase box itself)
    m_bRetainBoxContents = FALSE; // whm moved to constructor - originally was initialized in PhraseBox.cpp's global space

    // whm Note: m_bBoxTextByCopyOnly was originally named gbByCopyOnly and was 
    // declared and initialized in PhraseBox.cpp's global space, but I made it a
    // CPhraseBox member and moved its initialization here.
    //
    // will be set TRUE when the target text is the result of a
    // copy operation from the source, and if user types to modify it, it is
    // cleared to FALSE, and similarly, if a lookup succeeds, it is cleared to
    // FALSE. It is used in PlacePhraseBox() to enforce a store to the KB when
    // user clicks elsewhere after an existing location is returned to somehow,
    // and the user did not type anything to the target text retrieved from the
    // KB. In this circumstance the m_bAbandonable flag is TRUE, and the retrieved
    // target text would not be re-stored unless we have this extra flag
    // m_bBoxTextByCopyOnly to check, and when FALSE we enforce the store operation
    m_bBoxTextByCopyOnly = FALSE;

    // whm Note: m_bTunnellingOut was originally named gbTunnellingOut and was
    // declared and initialized in PhraseBox.cpp's global space, but I made it a
    // CPhraseBox member and moved its initialization here.
    //
    // TRUE when control needs to tunnel out of nested procedures when
    // gbVerticalEditInProgress is TRUE and a step-changing custom message
    // has been posted in order to transition to a different edit step;
    // FALSE (default) in all other circumstances
    m_bTunnellingOut = FALSE;

    // whm Note: m_bSavedTargetStringWithPunctInReviewingMode was originally named
    // gbSavedTargetStringWithPunctInReviewingMode and was declared and initialized 
    // in PhraseBox.cpp's global space, but I made it a CPhraseBox member and moved 
    // its initialization here.
    //
    // TRUE if either or both of m_adaption and m_targetStr is empty
    // and Reviewing mode is one (we want to preserve punctuation or
    // lack thereof if the location is a hole)
    m_bSavedTargetStringWithPunctInReviewingMode = FALSE;

    // whm Note: m_StrSavedTargetStringWithPunctInReviewingMode was originally named
    // gStrSavedTargetStringWithPunctInReviewingMode and was declared and initialized 
    // in PhraseBox.cpp's global space, but I made it a CPhraseBox member and moved 
    // its initialization here.
    //
    // works with the above flag, and stores whatever the m_targetStr was
    // when the phrase box, in Reviewing mode, lands on a hole (we want to
    // preserve what we found if the user has not changed it)    
    m_StrSavedTargetStringWithPunctInReviewingMode.Empty();

    // whm Note: m_bNoAdaptationRemovalRequested was originally named
    // gbNoAdaptationRemovalRequested and was declared and initialized 
    // in PhraseBox.cpp's global space, but I made it a CPhraseBox member and moved 
    // its initialization here.
    //
    // TRUE when user hits backspace or DEL key to try remove an earlier
    // assignment of <no adaptation> to the word or phrase at the active
    // location - (affects one of m_bHasKBEntry or m_bHasGlossingKBEntry
    // depending on the current mode, and removes the KB CRefString (if
    // the reference count is 1) or decrements the count, as the case may be)
    m_bNoAdaptationRemovalRequested = FALSE;
    
    // whm Note: m_bCameToEnd was originally named gbCameToEnd and was declared and initialized 
    // in PhraseBox.cpp's global space, but I made it a CPhraseBox member and moved its 
    // initialization here.
    //
    /// Used to delay the message that user has come to the end, until after last
    /// adaptation has been made visible in the main window; in OnePass() only, not JumpForward().
    m_bCameToEnd = FALSE;

    // whm Note: m_bTemporarilySuspendAltBKSP was originally named
    // gTemporarilySuspendAltBKSP and was declared and initialized 
    // in PhraseBox.cpp's global space, but I made it a CPhraseBox member and moved 
    // its initialization here.
    //
    // to enable m_bSuppressStoreForAltBackspaceKeypress flag to be turned
    // back on when <Not In KB> next encountered after being off for one
    // or more ordinary KB entry insertions; CTRL+ENTER also gives same result
    m_bTemporarilySuspendAltBKSP = FALSE;
    
    // whm Note: m_bSuppressStoreForAltBackspaceKeypress was originally named
    // gbSuppressStoreForAltBackspaceKeypress and was declared and initialized 
    // in PhraseBox.cpp's global space, but I made it a CPhraseBox member and moved 
    // its initialization here.
    //
    /// To support the ALT+Backspace key combination for advance to immediate next pile without lookup or
    /// store of the phrase box (copied, and perhaps SILConverters converted) text string in the KB or
    /// Glossing KB. When ALT+Backpace is done, this is temporarily set TRUE and restored to FALSE
    /// immediately after the store is skipped. CTRL+ENTER also can be used for the transliteration.
    m_bSuppressStoreForAltBackspaceKeypress = FALSE;

    // whm 24Feb2018 m_bSuppressMergeInMoveToNextPile was originally named gbSuppressMergeInMoveToNextPile
    // and was declared and initialized in PhraseBox.cpp's global space, but I made it a CPhraseBox
    // member and moved its initialization here. It was accidentally removed from code 22Feb2018, but
    // restored again 24Feb2018.
    // 
    // if a merge is done in LookAhead() so that the
    // phrase box can be shown at the correct location when the Choose Translation
    // dialog has to be put up because of non-unique translations, then on return
    // to MoveToNextPile() with an adaptation chosen in the dialog dialog will
    // come to code for merging (in the case when no dialog was needed), and if
    // not suppressed by this flag, a merge of an extra word or words is wrongly
    // done
    m_bSuppressMergeInMoveToNextPile = FALSE; 

    // whm 24Feb2018 m_bCompletedMergeAndMove was originally named gbCompletedMergeAndMove
    // and was declared and initialized in PhraseBox.cpp's global space, but I made it a
    // CPhraseBox member and moved ints initialization here. 
    //
    // for support of Bill Martin's wish that the phrase box
    // be at the new location when the Choose Translation dialog is shown
    m_bCompletedMergeAndMove = FALSE; 

    // whm 24Feb2018 m_bInhibitMakeTargetStringCall was originally named gbInhibitMakeTargetStringCall
    // and was originally declared and initialized in Adapt_ItView.cpp's global space. I renamed it and
    // moved its declaration and initialization to CAdapt_ItApp where it still functions as intended.
    //
    // Used for inhibiting multiple accesses to MakeTargetStringIncludingPunctuation when only one is needed.
    //m_bInhibitMakeTargetStringCall = FALSE; // whm Note: functions as intended as App member

    // whm 24Feb2018 m_nWordsInPhrase was originally named nWordsInPhrase. It was originally declared and
    // initialized in PhraseBox.cpp's global space to 0. I made it a member of CPhraseBox and initialized here.
    //
    // a matched phrase's number of words (from source phrase)
    m_nWordsInPhrase = 0;

    // whm 24Feb2018 m_CurKey was originally named curKey. It was originally declared and 
    // initialized to _T("") in PhraseBox.cpp's global space. I made it a member of CPhraseBox
    // and initialize it to .Empty() here. To better distinguish this CPhraseBox member from a
    // similarly named vairable in KBEditor.cpp, I renamed the variable in KBEditor.cpp to 
    // m_currentKey.
    // 
    // when non empty, it is the current key string which was matched
    m_CurKey.Empty(); 

    // whm 24Feb2018 m_Translation was originally named translation. It was originally 
    // declared and initialized to _T("") in PhraseBox.cpp's global space. I made it a member
    // of CPhraseBox and initialized it to .Empty() here. It appears that it was originally
    // intended to be a global that just held the result of a selection or new translation in the 
    // Choose Translation dialog - but seems to have been extended later to have a wider use.
    //
    // A wxString containing the translation for a matched source phrase key.
    m_Translation.Empty(); // = _T("") whm added 8Aug04 // translation, for a matched source phrase key

    // whm 24Feb2018 m_bEmptyAdaptationChosen was originally named gbEmptyAdaptationChosen and was
    // declared and initialized to FALSE in Adapt_ItView.cpp's global space. I made it a member of
    // CPhraseBox and initialized it to FALSE here.
    //
    // bool set by ChooseTranslation, when user selects <no adaptation>, then PhraseBox will
    // not use CopySource() but instead use an empty string for the adaptation
    m_bEmptyAdaptationChosen = FALSE;


    // Use custom dropdown control buttons
    //  /* XPM */
    const char * xpm_dropbutton_hover[] = {
        /* columns rows colors chars-per-pixel */
        "14 15 47 1 ",
        "  c black",
        ". c #588EF1",
        "X c #598EF1",
        "o c #588FF1",
        "O c #598FF1",
        "+ c #5C91F1",
        "@ c #6194F2",
        "# c #6597F2",
        "$ c #6A99F2",
        "% c #6C9CF3",
        "& c #719FF4",
        "* c #75A2F4",
        "= c #7AA4F3",
        "- c #7DA7F4",
        "; c #81AAF5",
        ": c #85ADF5",
        "> c #89B0F5",
        ", c #8EB2F5",
        "< c #91B5F6",
        "1 c #92B6F6",
        "2 c #96B8F7",
        "3 c #9ABBF7",
        "4 c #9DBDF6",
        "5 c #9EBEF7",
        "6 c #A2C0F7",
        "7 c #AAC6F7",
        "8 c #A6C3F8",
        "9 c #AFC9F8",
        "0 c #AEC8F9",
        "q c #B2CBF8",
        "w c #B6CEF9",
        "e c #BBD1F9",
        "r c #C3D6FA",
        "t c #C7D9FA",
        "y c #CBDCFA",
        "u c #CFDEFB",
        "i c #D3E1FC",
        "p c #D7E4FC",
        "a c #DAE6FC",
        "s c #DFE9FC",
        "d c #E3ECFD",
        "f c #E7EEFD",
        "g c #E8EFFD",
        "h c #EBF1FD",
        "j c #EFF4FD",
        "k c #F3F7FE",
        "l c None",
        /* pixels */
        "llllllllllllll",
        "llll      llll",
        "llll +XX+ llll",
        "llll %@oX llll",
        "llll -&#X llll",
        "llll ,;*$ llll",
        "llll 4<:= llll",
        "llll 062> llll",
        "l     q83    l",
        "ll faurw74< ll",
        "lll hsiteq lll",
        "llll jdpy llll",
        "lllll kg lllll",
        "llllll  llllll",
        "llllllllllllll"
    };

    //  /* XPM */
    const char * xpm_dropbutton_pressed[] = {
        /* columns rows colors chars-per-pixel */
        "14 15 31 1 ",
        "  c black",
        ". c #000DBC",
        "X c #0713BD",
        "o c #0814BE",
        "O c #1521C1",
        "+ c #1722C2",
        "@ c #232DC5",
        "# c #252EC5",
        "$ c #323AC9",
        "% c #343CC9",
        "& c #3F47CC",
        "* c #4148CC",
        "= c #4F55CF",
        "- c #5056D0",
        "; c #5C60D3",
        ": c #5D61D3",
        "> c #6B6ED6",
        ", c #6C6FD6",
        "< c #6E71D7",
        "1 c #787ADA",
        "2 c #7A7CDA",
        "3 c #7B7DDB",
        "4 c #8788DE",
        "5 c #8889DE",
        "6 c #9595E1",
        "7 c #9797E1",
        "8 c #A3A2E4",
        "9 c #A5A2E4",
        "0 c #AEABE7",
        "q c #AEACE7",
        "w c None",
        /* pixels */
        "wwwwwwwwwwwwww",
        "wwww      wwww",
        "wwww .... wwww",
        "wwww X... wwww",
        "wwww @Oo. wwww",
        "wwww &$#+ wwww",
        "wwww :=*% wwww",
        "wwww 1>:- wwww",
        "w     42,    w",
        "ww 0q08652< ww",
        "www 000097 www",
        "wwww 0q0q wwww",
        "wwwww qq wwwww",
        "wwwwww  wwwwww",
        "wwwwwwwwwwwwww"
    };

    //  /* XPM */
    const char * xpm_dropbutton_normal[] = {
        /* columns rows colors chars-per-pixel */
        "14 15 3 1 ",
        "  c black",
        ". c gray100",
        "X c None",
        /* pixels */
        "XXXXXXXXXXXXXX",
        "XXXX      XXXX",
        "XXXX .... XXXX",
        "XXXX .... XXXX",
        "XXXX .... XXXX",
        "XXXX .... XXXX",
        "XXXX .... XXXX",
        "XXXX .... XXXX",
        "X     ...    X",
        "XX ........ XX",
        "XXX ...... XXX",
        "XXXX .... XXXX",
        "XXXXX .. XXXXX",
        "XXXXXX  XXXXXX",
        "XXXXXXXXXXXXXX"
    };

    //  /* XPM */
    const char * xpm_dropbutton_disabled[] = {
        /* columns rows colors chars-per-pixel */
        "1 15 3 1 ",  // TODO: whm - Test this. It is a one-pixel wide image of transparent pixels
        "  c black",
        ". c gray100",
        "X c None",
        /* pixels */
        "X",
        "X",
        "X",
        "X",
        "X",
        "X",
        "X",
        "X",
        "X",
        "X",
        "X",
        "X",
        "X",
        "X",
        "X"
    };

    // Custom dropdown control button for blank button (no visible dropdown arror)
    //  /* XPM */
    const char * xpm_dropbutton_blank[] = {
        /* columns rows colors chars-per-pixel */
        "15 18 4 1 ",  // TODO: whm - Test this. It is a one-pixel wide image of transparent pixels
        "  c black",
        ". c gray100",
        "r c #FFE4E1", // misty rose
        "X c None",
        /* pixels */
        //"XXXXXXXXXXXXXX",
        //"XXXXXXXXXXX   ",
        //"XXXXX   XX   X",
        //"XXX          X",
        //"XX   XXX    XX",
        //"XX  XXX     XX",
        //"X  XXX   XX  X",
        //"X  XX   XXX  X",
        //"X  X   XXXX  X",
        //"X     XXXX  XX",
        //"XX   XXX    XX",
        //"X          XXX",
        //"   X     XXXXX",
        //"  XXXXXXXXXXXX",
        //"XXXXXXXXXXXXXX"

        "               ",
        " rrrrrrrrrrrrr ",
        " rrrrrrrrrrrrr ",
        "  rrrrrrrrrrr  ",
        " r rrrrrrrrr r ",
        " rr rrrrrrr rr ",
        " rrr rrrrr rrr ",
        " rrrr rrr rrrr ",
        " rrrrr r rrrrr ",
        " rrrrr r rrrrr ",
        " rrrr rrr rrrr ",
        " rrr rrrrr rrr ",
        " rr rrrrrrr rr ",
        " r rrrrrrrrr r ",
        "  rrrrrrrrrrr  ",
        " rrrrrrrrrrrrr ",
        " rrrrrrrrrrrrr ",
        "               "
    };

    dropbutton_hover = wxBitmap(xpm_dropbutton_hover);
    dropbutton_pressed = wxBitmap(xpm_dropbutton_pressed);
    dropbutton_normal = wxBitmap(xpm_dropbutton_normal);
    dropbutton_disabled = wxBitmap(xpm_dropbutton_disabled);
    dropbutton_blank = wxBitmap(xpm_dropbutton_blank);
    this->SetButtonBitmaps(dropbutton_normal, false, dropbutton_pressed, dropbutton_hover, dropbutton_disabled);
    //this->SetButtonBitmaps(xpm_dropbutton_blank, false, xpm_dropbutton_blank, xpm_dropbutton_blank, xpm_dropbutton_blank);
}


CPhraseBox::~CPhraseBox(void)
{
}

// returns number of phrases built; a return value of zero means that we have a halt condition
// and so none could be built (eg. source phrase is a merged one would halt operation)
// When glossing is ON, the build is constrained to phrases[0] only, and return value would then
// be 1 always.
// BEW 13Apr10, no changes needed for support of doc version 5
// BEW 23Apr15, changed to support / as a word-breaking whitespace char, if m_bFwdSlashDelimiter is TRUE
// because Lookups will, for mergers, want to test with ZWSP in strings of two or more words, since we
// store in the kb with / replaced by ZWSP for mergers, and show strings with ZWSP in the interlinear
// layout when the source and or target strings have 2 or more words
int CPhraseBox::BuildPhrases(wxString phrases[10], int nNewSequNum, SPList* pSourcePhrases)
{
	// refactored 25Mar09, -- nothing needs to be done
	// clear the phrases array
	phrases[0] = phrases[1] = phrases[2] = phrases[3] = phrases[4] = phrases[5] = phrases[6]
		= phrases[7] = phrases[8] = phrases[9] = _T("");

	// check we are within bounds
	int nMaxIndex = pSourcePhrases->GetCount() - 1;
	if (nNewSequNum > nMaxIndex)
	{
		// this is unlikely to ever happen, but play safe just in case
		wxMessageBox(_T("Index bounds error in BuildPhrases call\n"), _T(""), wxICON_EXCLAMATION | wxOK);
		wxExit();
	}

	// find position of the active pile's source phrase in the list
	SPList::Node *pos;

	pos = pSourcePhrases->Item(nNewSequNum);

	wxASSERT(pos != NULL);
	int index = 0;
	int counter = 0;
	CSourcePhrase* pSrcPhrase;

	// build the phrases array, breaking if a build halt condition is encounted
	// (These are: if we reach a boundary, or a source phrase with an adaption already, or
	// a change of TextType, or a null source phrase or a retranslation, or EOF, or max
	// number of words is reached, or we reached a source phrase which has been previously
	// merged.) MAX_WORDS is defined in AdaptitConstants.h (TextType changes can be ignored
	// here because they coincide with m_bBoundary == TRUE on the previous source phrase.)
	// When glossing is ON, there are no conditions for halting, because a pile will already
	// be active, and the src word at that location will be glossable no matter what.
	if (gbIsGlossing)
	{
		// BEW 6Aug13, I previously missed altering this block when I refactored in order
		// to have glossing KB use all ten tabs. So instead of putting the key into
		// phrases[0] as before, it now must be put into whichever one of the array
		// pertains to how many words, less one, are indicated by m_nSourceWords, and
		// counter needs to be set to the latter's value rather than to 1 as used to be
		// the case
		pSrcPhrase = (CSourcePhrase*)pos->GetData();
		int theIndex = pSrcPhrase->m_nSrcWords - 1;
		phrases[theIndex] = pSrcPhrase->m_key;
		return counter = pSrcPhrase->m_nSrcWords;
	}
	while (pos != NULL && index < MAX_WORDS)
	{
		// NOTE: MFC's GetNext(pos) retrieves the current pos data into
		// pScrPhrase, then increments pos
		pSrcPhrase = (CSourcePhrase*)pos->GetData();
		pos = pos->GetNext();
		if (pSrcPhrase->m_nSrcWords > 1 || !pSrcPhrase->m_adaption.IsEmpty() ||
			pSrcPhrase->m_bNullSourcePhrase || pSrcPhrase->m_bRetranslation)
			return counter; // don't build with this src phrase, it's a merged one, etc.

		if (index == 0)
		{
			phrases[0] = pSrcPhrase->m_key;
			counter++;
			if (pSrcPhrase->m_bBoundary || nNewSequNum + counter > nMaxIndex)
				break;
		}
		else
		{
			phrases[index] = phrases[index - 1] + PutSrcWordBreak(pSrcPhrase) + pSrcPhrase->m_key;
//#if defined(FWD_SLASH_DELIM)
			CAdapt_ItApp* pApp = (CAdapt_ItApp*)&wxGetApp();
			if (pApp->m_bFwdSlashDelimiter)
			{
				// BEW 23Apr15 if in a merger, we want / converted to ZWSP for the source text
				// to support lookups because we will have ZWSP rather than / in the KB
				// No changes are made if app->m_bFwdSlashDelimiter is FALSE
				phrases[index] = FwdSlashtoZWSP(phrases[index]);
			}
//#endif
			counter++;
			if (pSrcPhrase->m_bBoundary || nNewSequNum + counter > nMaxIndex)
				break;
		}
		index++;
	}
	return counter;
}

CLayout* CPhraseBox::GetLayout()
{
	CAdapt_ItApp* pApp = (CAdapt_ItApp*)&wxGetApp();
	return pApp->m_pLayout;
}

// returns TRUE if the phrase box, when placed at pNextEmptyPile, would not be within a
// retranslation, or FALSE if it is within a retranslation
// Side effects:
// (1) checks for vertical edit being current, and whether or not a vertical edit step
// transitioning event has just been posted (that would be the case if the phrase box at
// the new location would be in grayed-out text), and if so, returns FALSE after setting
// the global bool m_bTunnellingOut to TRUE - so that MoveToNextPile() can be exited early
// and the vertical edit step's transition take place instead
// (2) for non-vertical edit mode, if the new location would be within a retranslation, it
// shows an informative message to the user, enables the button for copying punctuation,
// and returns FALSE
// BEW 13Apr10, no changes needed for support of doc version 5
// BEW 9Apr12, changed to support discontinuous hightlight spans for auto-inserts
bool CPhraseBox::CheckPhraseBoxDoesNotLandWithinRetranslation(CAdapt_ItView* pView,
												CPile* pNextEmptyPile, CPile* pCurPile)
{
	// created for refactored view layout, 24Mar09
	wxASSERT(pNextEmptyPile);
	if (gbIsGlossing)
		return TRUE; // allow phrase box to land in a retranslation when glossing mode is ON

	// the code below will only be entered when glossing mode is OFF, that is, in adapting mode
	// BEW 9Apr12,deprecated the check and clearing of the highlighting, discontinuity in the
	// highlighted spans of auto-inserted material is now supported
	pCurPile = pCurPile; // avoid compiler warning

	if (pNextEmptyPile->GetSrcPhrase()->m_bRetranslation)
	{
		// if the lookup and jump loop comes to an empty pile which is in a retranslation,
		// we halt the loop there. If vertical editing is in progress, this halt location
		// could be either within or beyond the edit span, in which case the former means
		// we don't do any step transition yet, the latter means a step transition is
		// called for. Test for these situations and act accordingly. If we transition
		// the step, there is no point in showing the user the message below because we
		// just want transition and where the jump-landing location might be is of no interest
		if (gbVerticalEditInProgress)
		{
			// bForceTransition is FALSE in the next call
			m_bTunnellingOut = FALSE; // ensure default value set
			int nSequNum = pNextEmptyPile->GetSrcPhrase()->m_nSequNumber;
			bool bCommandPosted = pView->VerticalEdit_CheckForEndRequiringTransition(nSequNum,nextStep);
			if (bCommandPosted)
			{
				// don't proceed further because the current vertical edit step has ended
				m_bTunnellingOut = TRUE; // caller needs to use it
				return FALSE; // use FALSE to help caller recognise need to tunnel out of the lookup loop
			}
		}
		// IDS_NO_ACCESS_TO_RETRANS
		wxMessageBox(_(
"Sorry, to edit or remove a retranslation you must use the toolbar buttons for those operations."),
						_T(""), wxICON_INFORMATION | wxOK);
		GetLayout()->m_pApp->m_pTargetBox->SetFocus();
		// if necessary restore default button image, and m_bCopySourcePunctuation to TRUE
		wxCommandEvent event;
		if (!GetLayout()->m_pApp->m_bCopySourcePunctuation)
		{
			pView->OnToggleEnablePunctuationCopy(event);
		}
		return FALSE;
	}
	if (gbVerticalEditInProgress)
	{
		// BEW 19Oct15 No transition of vert edit modes, and not landing in a retranslation,
		// so we can store this location on the app
		gpApp->m_vertEdit_LastActiveSequNum = pNextEmptyPile->GetSrcPhrase()->m_nSequNumber;
#if defined(_DEBUG)
		wxLogDebug(_T("VertEdit PhrBox, CheckPhraseBoxDoesNotLandWithinRetranslation() storing loc'n: %d "),
			pNextEmptyPile->GetSrcPhrase()->m_nSequNumber);
#endif
	}
	return TRUE;
}

// returns nothing
// this is a helper function to do some housecleaning tasks prior to the caller (which is
// a pile movement function such as MoveToNextPile(), returning FALSE to its caller
// BEW 13Apr10, no changes needed for support of doc version 5
// BEW 21Jun10, no changes needed for support of kbVersion 2
void CPhraseBox::DealWithUnsuccessfulStore(CAdapt_ItApp* pApp, CAdapt_ItView* pView,
										   CPile* pNextEmptyPile)
{
	if (!pApp->m_bSingleStep)
	{
		pApp->m_bAutoInsert = FALSE; // cause halt, if auto lookup & inserting is ON
	}

	// if necessary restore default button image, and m_bCopySourcePunctuation to TRUE
	wxCommandEvent event;
	if (!pApp->m_bCopySourcePunctuation)
	{
		pView->OnToggleEnablePunctuationCopy(event);
	}
	if (m_bSuppressStoreForAltBackspaceKeypress)
		m_SaveTargetPhrase.Empty();
	m_bTemporarilySuspendAltBKSP = FALSE;
	m_bSuppressStoreForAltBackspaceKeypress = FALSE; // make sure it's off before returning

    // if vertical editing is in progress, the store failure may occur with the active
    // location within the editable span, (in which case we don't want a step transition),
    // or having determined the jump location's pile is either NULL (a bundle boundary was
    // reached before an empty pile could be located - in which case a step transition
    // should be forced), or a pile located which is beyond the editable span, in the gray
    // area, in which case transition is wanted; so handle these options using the value
    // for pNextEmptyPile obtained above Note: doing a transition in this circumstance
    // means the KB does not get the phrase box contents added, but the document still has
    // the adaptation or gloss, so the impact of the failure to store is minimal (ie. if
    // the box contents were unique, the adaptation or gloss will need to occur later
    // somewhere for it to make its way into the KB)
	if (gbVerticalEditInProgress)
	{
		// bForceTransition is TRUE in the next call
		m_bTunnellingOut = FALSE; // ensure default value set
		bool bCommandPosted = FALSE;
		if (pNextEmptyPile == NULL)
		{
			bCommandPosted = pView->VerticalEdit_CheckForEndRequiringTransition(-1,nextStep,TRUE);
		}
		else
		{
			// bForceTransition is FALSE in the next call
			int nSequNum = pNextEmptyPile->GetSrcPhrase()->m_nSequNumber;
			bCommandPosted = pView->VerticalEdit_CheckForEndRequiringTransition(nSequNum,nextStep);
		}
		if (bCommandPosted)
		{
			// don't proceed further because the current vertical edit step has ended
			m_bTunnellingOut = TRUE; // caller needs to use it
			// caller unilaterally returns FALSE  when this function returns,
			// this, together with m_bTunnellingOut,  enables the caller of the caller to
			// recognise the need to tunnel out of the lookup loop
		}
		else
		{
			// BEW 19Oct15 No transition of vert edit modes,
			// so we can store this location on the app
			gpApp->m_vertEdit_LastActiveSequNum = pNextEmptyPile->GetSrcPhrase()->m_nSequNumber;
#if defined(_DEBUG)
		wxLogDebug(_T("VertEdit PhrBox, DealWithUnsuccessfulStore() storing loc'n: %d "),
			pNextEmptyPile->GetSrcPhrase()->m_nSequNumber);
#endif
		}
	}
}

// return TRUE if there were no problems encountered with the store, FALSE if there were
// (this function calls DealWithUnsuccessfulStore() if there was a problem with the store)
// BEW 13Apr10, no changes needed for support of doc version 5
// BEW 21Jun10, no changes needed for support of kbVersion 2
bool CPhraseBox::DoStore_NormalOrTransliterateModes(CAdapt_ItApp* pApp, CAdapt_ItDoc* pDoc,
		 CAdapt_ItView* pView, CPile* pCurPile, bool bIsTransliterateMode)
{
	bool bOK = TRUE;
	CSourcePhrase* pOldActiveSrcPhrase = pCurPile->GetSrcPhrase();

	// m_bSuppressStoreForAltBackspaceKeypress is FALSE, so either we are in normal adapting
	// or glossing mode; or we could be in transliteration mode but the global boolean
	// happened to be FALSE because the user has just done a normal save in transliteration
	// mode because the transliterator did not produce a correct transliteration

	// we are about to leave the current phrase box location, so we must try to store what is
	// now in the box, if the relevant flags allow it. Test to determine which KB to store to.
	// StoreText( ) has been ammended for auto-capitalization support (July 2003)
	if (!gbIsGlossing)
	{
		pView->MakeTargetStringIncludingPunctuation(pOldActiveSrcPhrase, pApp->m_targetPhrase);
		pView->RemovePunctuation(pDoc,&pApp->m_targetPhrase,from_target_text);
	}
	if (gbIsGlossing)
	{
		// BEW added next line 27Jan09
		bOK = pApp->m_pGlossingKB->StoreText(pOldActiveSrcPhrase, pApp->m_targetPhrase);
	}
	else
	{
		// BEW added next line 27Jan09
		bOK = pApp->m_pKB->StoreText(pOldActiveSrcPhrase, pApp->m_targetPhrase);
	}

    // if in Transliteration Mode we want to cause m_bSuppressStoreForAltBackspaceKeypress
    // be immediately turned back on, in case a <Not In KB> entry is at the next lookup
    // location and we will then want the special Transliteration Mode KB storage process
    // to be done rather than a normal empty phrasebox for such an entry
	if (bIsTransliterateMode)
	{
		m_bSuppressStoreForAltBackspaceKeypress = TRUE;
	}
	return bOK;
}

// BEW 27Mar10 a change needed for support of doc version 5
void CPhraseBox::MakeCopyOrSetNothing(CAdapt_ItApp* pApp, CAdapt_ItView* pView,
									  CPile* pNewPile, bool& bWantSelect)
{
    // BEW 14Apr10, pass back bWantSelect as TRUE, because this will be used in caller to
    // pass the TRUE value to MoveToNextPile() which uses it to set the app members
    // m_nStartChar, m_nEndChar, to -1 and -1, and then when CLayout::PlaceBox()is called,
    // it internally calls ResizeBox() and the latter is what uses the m_nStartChar and
    // m_nEndChar values to set the phrase box content's selection. BEW 14Apr10
	bWantSelect = TRUE;

	if (pApp->m_bCopySource)
	{
		if (!pNewPile->GetSrcPhrase()->m_bNullSourcePhrase)
		{
			pApp->m_targetPhrase = pView->CopySourceKey(pNewPile->GetSrcPhrase(),
									pApp->m_bUseConsistentChanges);
		}
		else
		{
            // its a null source phrase, so we can't copy anything; and if we are glossing,
            // we just leave these empty whenever we meet them
			pApp->m_targetPhrase.Empty(); // this will cause pile's m_nMinWidth to be
										   // used for box width
		}
	}
	else
	{
		// no copy of source wanted, so just make it an empty string
		pApp->m_targetPhrase.Empty();
	}
}

// BEW 13Apr10, changes needed for support of doc version 5
// whm modified 22Feb2018 to adjust for fact that the Cancel and Select button
// no longer exists in the Choose Translation dialog with the CPhraseBox being
// derived from wxOwnerDrawnComboBox. The m_bCancelAndSelect flag was removed 
// from this function's signature and the overall app. 
// TODO: This and the following function (for AutoAdaptMode) need their logic 
// changed to account for the removals of the legacy flags that were originally 
// called m_bCancelAndSelect and gbUserCancelledChooseTranslationDlg.
// As a temporary measure, I've made two local flags m_bCancelAndSelect_temp and
// gbUserCancelledChooseTranslationDlg_temp that are set to FALSE at the beginning 
// of the function. While the function should be revised at some point, setting the 
// flag to remain always FALSE, is just a temporary measure to ensure that I don't
// mess up the logic by a quick refactor that removes the effects of the original
// bool flags.
// BEW 16Mar18 removed Bill's two temporary flags. If I understand Bill's dropdown combo
// correctly, the removal of Choose Translation dialog automatically coming up means
// that at the location where that would happen, it would be appropriate just to halt 
// and try find appropriate KB target text to insert in the combobox there - but actually
// there may not be any reason to halt so as to invoke this legacy hack function - so it 
// may never be called. Anyway, Bill can test and determine if my foggy brain is thinking right
//
// whm 16Mar2018 restored BEW changes made 16Mar18 in attempt to find why app is now
// failing to fill text within the phrasebox and have it pre-selected.
//
//void CPhraseBox::HandleUnsuccessfulLookup_InSingleStepMode_AsBestWeCan(CAdapt_ItApp* pApp,
//    CAdapt_ItView* pView, CPile* pNewPile, bool m_bCancelAndSelect, bool& bWantSelect)
void CPhraseBox::HandleUnsuccessfulLookup_InSingleStepMode_AsBestWeCan(CAdapt_ItApp* pApp,
	CAdapt_ItView* pView, CPile* pNewPile, bool& bWantSelect)
{
    bool m_bCancelAndSelect_temp = FALSE; // whm 22Feb2018 added for temp fix
    bool gbUserCancelledChooseTranslationDlg_temp = FALSE; // whm 22Feb2018 added for temp fix

    pApp->m_pTargetBox->m_bAbandonable = TRUE;

    // it is single step mode & no adaptation available, so see if we can find a
    // translation, or gloss, for the single src word at the active location, if not,
    // depending on the m_bCopySource flag, either initialize the targetPhrase to
    // an empty string, or to a copy of the sourcePhrase's key string
    bool bGotTranslation = FALSE;

	//  BEW removed 16Mar18, whm restored 16Mar2018
    if (!gbIsGlossing && m_bCancelAndSelect_temp)
    {
        // in ChooseTranslation dialog the user wants the 'cancel and select'
        // option, and since no adaptation is therefore to be retrieved, it
        // remains just to either copy the source word or nothing...
        MakeCopyOrSetNothing(pApp, pView, pNewPile, bWantSelect);

        // BEW added 1Jul09, the flag should be TRUE if nothing was found
        pApp->m_pTargetBox->m_bAbandonable = TRUE;

    }
    else
    {
        // user didn't press the Cancel and Select button in the Choose Translation
        // dialog, but he may have pressed Cancel button, or OK button - so try to
        // find a translation given these possibilities (note, nWordsInPhrase equal
        // to 1 does not distinguish between adapting or glossing modes, so handle that in
        // the next block too)
        // whm 21Feb2018 note: the legacy gbUserCancelledChooseTranslationDlg is now 
        // removed from the app, so its logical value would be FALSE (never 
        // set to TRUE); whether a dialog was Cancelled is no longer a factor in the
        // logic of the following if ... else block. I've set a local _temp bool above
        // to FALSE as a temporary fix.
        if (!gbUserCancelledChooseTranslationDlg_temp || m_nWordsInPhrase == 1)
        {
            // try find a translation for the single word (from July 2003 this supports
            // auto capitalization) LookUpSrcWord() calls RecalcLayout()
            bGotTranslation = LookUpSrcWord(pNewPile);
        }
        else
        {
            // the user cancelled the ChooseTranslation dialog
            gbUserCancelledChooseTranslationDlg_temp = FALSE; // restore default value

            // if the user cancelled the Choose Translation dialog when a phrase was
            // merged, then he will probably want a lookup done for the first word of
            // the now unmerged phrase; nWordsInPhrase will still contain the word count
            // for the formerly merged phrase, so use it; but when glossing is current,
            // the LookUpSrcWord call is done only in the first map, so nWordsInPhrase
            // will not be greater than 1 when doing glossing
            if (m_nWordsInPhrase > 1) // nWordsInPhrase is a global, set in LookAhead()
                                    // or in LookUpSrcWord()
            {
                // nWordsInPhrase can only be > 1 in adapting mode, so handle that
                bGotTranslation = LookUpSrcWord(pNewPile);
            }
        }
		//bGotTranslation = LookUpSrcWord(pNewPile); // BEW 16Mar18 added this line, because the above logic was commented out, whm removed 16Mar2018
        pNewPile = pApp->m_pActivePile; // update the pointer, since LookUpSrcWord()
                                        // calls RecalcLayout() & resets m_pActivePile (in refactored code
                                        // this call is still needed because we replace the old pile with the
                                        // altered one (it has new width since its now active location)
        if (bGotTranslation)
        {
            // if it is a <Not In KB> entry we show any m_targetStr that the
            // sourcephrase instance may have, by putting it in the global
            // translation variable; when glossing is ON, we ignore
            // "not in kb" since that pertains to adapting only
            if (!gbIsGlossing && m_Translation == _T("<Not In KB>"))
            {
                // make sure asterisk gets shown, and the adaptation is taken
                // from the sourcephrase itself - but it will be empty
                // if the sourcephrase has not been accessed before
                m_Translation = pNewPile->GetSrcPhrase()->m_targetStr;
                pNewPile->GetSrcPhrase()->m_bHasKBEntry = FALSE;
                pNewPile->GetSrcPhrase()->m_bNotInKB = TRUE;
            }

            pApp->m_targetPhrase = m_Translation; // set using the global var, set in
                                                // LookUpSrcWord() call
            bWantSelect = TRUE;
        }
        else // did not get a translation, or gloss
        {
            // do a copy of the source (this never needs change of capitalization)
            MakeCopyOrSetNothing(pApp, pView, pNewPile, bWantSelect);

            // BEW added 1Jul09, the flag should be TRUE if nothing was found
            pApp->m_pTargetBox->m_bAbandonable = TRUE;
        }
    } // BEW removed 16Mar18, whm restored 16Mar2018
 }

// BEW 13Apr10, changes needed for support of doc version 5
// whm modified 22Feb2018 to adjust for fact that the Cancel and Select button
// no longer exists in the Choose Translation dialog with the CPhraseBox being
// derived from wxOwnerDrawnComboBox. The m_bCancelAndSelect flag was removed 
// from this function's signature and the overall app. 
// TODO: This and the preceding function (for SingleStepMode) need their logic 
// changed to account for the removals of the legacy flags that were originally 
// called m_bCancelAndSelect and gbUserCancelledChooseTranslationDlg.
// As a temporary measure, I've made two local flags m_bCancelAndSelect_temp and
// gbUserCancelledChooseTranslationDlg_temp that are set to FALSE at the beginning 
// of the function. While the function should be revised at some point, setting the 
// flag to remain always FALSE, is just a temporary measure to ensure that I don't
// mess up the logic by a quick refactor that removes the effects of the original
// bool flags.
 // BEW 16Mar18 refactored to simplify - combobox implementation does not require so much logic
 //
 // whm 16Mar2018 restored BEW changes made 16Mar18 in attempt to find why app is now
 // failing to fill text within the phrasebox and have it pre-selected.
 //
 //void CPhraseBox::HandleUnsuccessfulLookup_InAutoAdaptMode_AsBestWeCan(CAdapt_ItApp* pApp,
//     CAdapt_ItView* pView, CPile* pNewPile, bool m_bCancelAndSelect, bool& bWantSelect)
void CPhraseBox::HandleUnsuccessfulLookup_InAutoAdaptMode_AsBestWeCan(CAdapt_ItApp* pApp,
	CAdapt_ItView* pView, CPile* pNewPile, bool& bWantSelect)
{
    bool m_bCancelAndSelect_temp = FALSE; // whm 22Feb2018 added for temp fix
    bool gbUserCancelledChooseTranslationDlg_temp = FALSE; // whm 22Feb2018 added for temp fix

    pApp->m_bAutoInsert = FALSE; // cause halt

	// BEW 16Mar18, refactored a bit in response to Bill's request to check logic out in the combo dropdown implementation
	// With the combobox approach, Lookup() doesn't get a chance to put up Choose Translation dialog, instead, the user
	// eyeballs the dropped down (or not) list to decide what to do. Halting the box with m_bAutoInsert set FALSE should suffice,
	// so the test below and it's true block should be removed.

    if (!gbIsGlossing && m_bCancelAndSelect_temp)
    {
        // user cancelled CChooseTranslation dialog because he wants instead to
        // select for a merger of two or more source words
        pApp->m_pTargetBox->m_bAbandonable = TRUE;

        // no adaptation available, so depending on the m_bCopySource flag, either
        // initialize the targetPhrase to an empty string, or to a copy of the
        // sourcePhrase's key string; then select the first two words ready for a
        // merger or extension of the selection
        MakeCopyOrSetNothing(pApp, pView, pNewPile, bWantSelect);

        // the DoCancelAndSelect() call is below after the RecalcLayout calls
    }
    else // user does not want a "Cancel and Select" selection; or is glossing
    {

		// BEW 16Mar18, With the combodropdown implementation, it ought not be possible at this 
		// point to know what the user will want, I've set bWantSelect to FALSE able

        // try find a translation for the single source word, use it if we find one;
        // else do the usual copy of source word, with possible cc processing, etc.
        // LookUpSrcWord( ) has been ammended (July 2003) for auto capitalization
        // support; it does any needed case change before returning, leaving the
        // resulting string in the app member: m_Translation
        // whm 21Feb2018 note: the legacy gbUserCancelledChooseTranslationDlg is now 
        // removed from the app, so its logical value would be FALSE (never 
        // set to TRUE); whether a dialog was Cancelled is no longer a factor in the
        // logic of the following if ... else block. I've set a local _temp bool above
        // to FALSE as a temporary fix.
		// BEW 16Mar18 - the code below simplifies for the combodropdown box implementation
		// because glossing uses word counts greater than 1 now, and all we want here is
		// to get something for the box, if possible
        bool bGotTranslation = FALSE;
        if (!gbUserCancelledChooseTranslationDlg_temp || m_nWordsInPhrase == 1)
        {
            bGotTranslation = LookUpSrcWord(pNewPile);
        }
        else
        {
            gbUserCancelledChooseTranslationDlg_temp = FALSE; // restore default value

            // if the user cancelled the Choose Translation dialog when a phrase was
            // merged, then he will probably want a lookup done for the first word
            // of the now unmerged phrase; nWordsInPhrase will still contain the
            // word count for the formerly merged phrase, so use it; but when glossing
            // nWordsInPhrase should never be anything except 1, so this block should
            // not get entered when glossing
            if (m_nWordsInPhrase > 1) // nWordsInPhrase is set in LookAhead()
                                    // or in LookUpSrcWord()
            {
                // must be adapting mode
                bGotTranslation = LookUpSrcWord(pNewPile);
            }
        }
		//bGotTranslation = LookUpSrcWord(pNewPile); // BEW 16Mar18, whm removed 16Mar2018
        pNewPile = pApp->m_pActivePile; // update the pointer (needed, because
                                        // RecalcLayout() was done by LookUpSrcWord(), and in its refactored
                                        // code we called ResetPartnerPileWidth() to get width updated and
                                        // a new copy of the pile replacing the old one at same location
                                        // in the list m_pileList

        if (bGotTranslation)
        {
            // if it is a <Not In KB> entry we show any m_targetStr that the
            // sourcephrase instance may have, by putting it in the global
            // translation variable; when glossing is ON, we ignore
            // "not in kb" since that pertains to adapting only
            if (!gbIsGlossing && m_Translation == _T("<Not In KB>"))
            {
                // make sure asterisk gets shown, and the adaptation is taken
                // from the sourcephrase itself - but it will be empty
                // if the sourcephrase has not been accessed before
                m_Translation = pNewPile->GetSrcPhrase()->m_targetStr;
                pNewPile->GetSrcPhrase()->m_bHasKBEntry = FALSE;
                pNewPile->GetSrcPhrase()->m_bNotInKB = TRUE;
				//bWantSelect = TRUE; // whm removed BEW's addition 16Mar2018
            }

            pApp->m_targetPhrase = m_Translation; // set using the global var,
                                                // set in LookUpSrcWord call
            bWantSelect = TRUE;
        }
        else // did not get a translation, or a gloss when glossing is current
        {
#if defined(_DEBUG) & defined(LOOKUP_FEEDBACK)
            wxLogDebug(_T("HandleUnsuccessfulLookup_InAutoAdaptMode_AsBestWeCan() Before MakeCopy...: sn = %d , key = %s , m_targetPhrase = %s"),
                pNewPile->GetSrcPhrase()->m_nSequNumber, pNewPile->GetSrcPhrase()->m_key.c_str(), pApp->m_targetPhrase.c_str());
#endif
            MakeCopyOrSetNothing(pApp, pView, pNewPile, bWantSelect);

#if defined(_DEBUG) & defined(LOOKUP_FEEDBACK)
            wxLogDebug(_T("HandleUnsuccessfulLookup_InAutoAdaptMode_AsBestWeCan() After MakeCopy...: sn = %d , key = %s , m_targetPhrase = %s"),
                pNewPile->GetSrcPhrase()->m_nSequNumber, pNewPile->GetSrcPhrase()->m_key.c_str(), pApp->m_targetPhrase.c_str());
#endif

            // BEW added 1Jul09, the flag should be TRUE if nothing was found
            pApp->m_pTargetBox->m_bAbandonable = TRUE;
        }

        // is "Accept Defaults" turned on? If so, make processing continue
        if (pApp->m_bAcceptDefaults)
        {
            pApp->m_bAutoInsert = TRUE; // revoke the halt
        }
    } // BEW 16Mar18 removed true block, whm restored 16Mar2018
}

// returns TRUE if the move was successful, FALSE if not successful
// In refactored version, transliteration mode is handled by a separate function, so
// MoveToNextPile() is called only when CAdapt_ItApp::m_bTransliterationMode is FALSE, so
// this value can be assumed. The global boolean gbIsGlossing, however, may be either FALSE
// (adapting mode) or TRUE (glossing mode)
// Ammended July 2003 for auto-capitalization support
// BEW 13Apr10, changes needed for support of doc version 5
// BEW 21Jun10, no changes for support of kbVersion 2, & removed pView from signature
bool CPhraseBox::MoveToNextPile(CPile* pCurPile)
{
	CAdapt_ItApp* pApp = (CAdapt_ItApp*)&wxGetApp();

    // whm added 22Mar2018 for detecting callers of PlaceBox()
    pApp->m_bMovingToDifferentPile = TRUE;

	pApp->m_preGuesserStr.Empty(); // BEW 27Nov14, in case a src string, or modified string
		// is stored ready for user's Esc keypress to restore the pre-guesser
		// form, clear it, because the box is gunna move and we want it
		// restored to default empty ready for next box landing location
	// whm modified 29Mar12. Left mouse clicks now beep only when certain parts of
	// the canvas are clicked on, and allows other clicks to act normally (such as
	// the opening/closing of the ViewFilteredMaterial dialog and the Notes dialog).

	//bool bNoError = TRUE;
	bool bWantSelect = FALSE; // set TRUE if any initial text in the new location is to be
							  // shown selected
	// store the translation in the knowledge base
	//pApp->limiter = 0; // BEW added Aug13, to support OnIdle() hack for m_targetStr non-stick bug // bug fixed 24Sept13 BEW
	CAdapt_ItView* pView = pApp->GetView();
	CAdapt_ItDoc* pDoc = pApp->GetDocument();
	bool bOK;
	m_bBoxTextByCopyOnly = FALSE; // restore default setting
	CSourcePhrase* pOldActiveSrcPhrase = pCurPile->GetSrcPhrase();
	CLayout* pLayout = GetLayout();

/* #if defined(_DEBUG)
	CPile* myPilePtr = pApp->GetView()->GetPile(pApp->m_nActiveSequNum);
	CSourcePhrase* mySrcPhrasePtr = myPilePtr->GetSrcPhrase();
	wxLogDebug(_T("MoveToNextPile() at start: sn = %d , src key = %s , m_adaption = %s , m_targetStr = %s , m_targetPhrase = %s"),
		mySrcPhrasePtr->m_nSequNumber, mySrcPhrasePtr->m_key.c_str(), mySrcPhrasePtr->m_adaption.c_str(), 
		mySrcPhrasePtr->m_targetStr.c_str(), pApp->m_targetPhrase.c_str());
#endif */

	// make sure pApp->m_targetPhrase doesn't have any final spaces
	RemoveFinalSpaces(pApp->m_pTargetBox,&pApp->m_targetPhrase);

	CPile* pNextEmptyPile = pView->GetNextEmptyPile(pCurPile);
	if (pNextEmptyPile == NULL)
	{
		// no more empty piles in the current document. We can just continue at this point
		// since we do this call again below
		;
	}
	else
	{
		// don't move forward if it means moving to an empty retranslation pile, but only for
		// when we are adapting. When glossing, the box is allowed to be within retranslations
		bool bNotInRetranslation =
				CheckPhraseBoxDoesNotLandWithinRetranslation(pView, pNextEmptyPile, pCurPile);
		if (!bNotInRetranslation)
		{
            // whm added 22Mar2018 for detecting callers of PlaceBox()
            pApp->m_bMovingToDifferentPile = FALSE;

            // the phrase box landed in a retranslation, so halt the lookup and insert loop
			// so the user can do something manually to achieve what he wants in the
			// viscinity of the retranslation
			return FALSE;
		}
		// continue processing below if the phrase box did not land in a retranslation
	}

	// if the location we are leaving is a <Not In KB> one, we want to skip the store & fourth
	// line creation --- as of Dec 18, version 1.4.0, according to Susanna Imrie's
	// recommendation, I've changed this so it will allow a non-null adaptation to remain at
	// this location in the document, but just to suppress the KB store; if glossing is ON, then
	// being a <Not In KB> location is irrelevant, and we will want the store done normally - but
	// to the glossing KB of course
	bOK = TRUE;
	if (!gbIsGlossing && !pOldActiveSrcPhrase->m_bHasKBEntry && pOldActiveSrcPhrase->m_bNotInKB)
	{
		// if the user edited out the <Not In KB> entry from the KB editor, we need to put
		// it back so that the setting is preserved (the "right" way to change the setting is to
		// use the toolbar checkbox - this applies when adapting, not glossing)
		pApp->m_pKB->Fix_NotInKB_WronglyEditedOut(pCurPile);
	}
	else
	{
		// make the punctuated target string, but only if adapting; note, for auto capitalization
		// ON, the function will change initial lower to upper as required, whatever punctuation
		// regime is in place for this particular sourcephrase instance
		// in the next call, the final bool flag, bIsTransliterateMode, is default FALSE
		
/* #if defined(_DEBUG)
	wxLogDebug(_T("MoveToNextPile() before DoStore_Normal...(): sn = %d , key = %s , m_targetPhrase = %s , m_targetStr = %s"),
		pCurPile->GetSrcPhrase()->m_nSequNumber, pCurPile->GetSrcPhrase()->m_key.c_str(), pApp->m_targetPhrase.c_str(), 
		pCurPile->GetSrcPhrase()->m_targetStr.c_str());
#endif */

		bOK = DoStore_NormalOrTransliterateModes(pApp, pDoc, pView, pCurPile);
		if (!bOK)
		{
            // whm added 22Mar2018 for detecting callers of PlaceBox()
            pApp->m_bMovingToDifferentPile = FALSE;

            DealWithUnsuccessfulStore(pApp, pView, pNextEmptyPile);
			return FALSE; // can't move until a valid adaption (which could be null) is supplied
		}
	}
/* #if defined(_DEBUG)
	wxLogDebug(_T("MoveToNextPile() after DoStore_Normal...: sn = %d , key = %s , m_targetPhrase = %s , m_targetStr = %s"),
		pCurPile->GetSrcPhrase()->m_nSequNumber, pCurPile->GetSrcPhrase()->m_key.c_str(), pApp->m_targetPhrase.c_str(),
		pCurPile->GetSrcPhrase()->m_targetStr.c_str());
#endif */

	// since we are moving, make sure the default m_bSaveToKB value is set
	pApp->m_bSaveToKB = TRUE;

	// move to next pile's cell which has no adaptation yet
	pApp->m_bUserTypedSomething = FALSE; // user has not typed at the new location yet
	bool bAdaptationAvailable = FALSE;
	CPile* pNewPile = pView->GetNextEmptyPile(pCurPile); // this call does not update
														 // the active sequ number

	// if necessary restore default button image, and m_bCopySourcePunctuation to TRUE
	wxCommandEvent event;
	if (!pApp->m_bCopySourcePunctuation)
	{
		pView->OnToggleEnablePunctuationCopy(event);
	}
	if (pNewPile == NULL)
	{
		// we deem vertical editing current step to have ended if control gets into this
		// block, so user has to be asked what to do next if vertical editing is currently
		// in progress; and we tunnel out before m_nActiveSequNum can be set to -1 (otherwise
		// vertical edit will crash when recalc layout is tried with a bad sequ num value)
		if (gbVerticalEditInProgress)
		{
			m_bTunnellingOut = FALSE; // ensure default value set
			bool bCommandPosted = pView->VerticalEdit_CheckForEndRequiringTransition(-1,
							nextStep, TRUE); // bForceTransition is TRUE
			if (bCommandPosted)
			{
                // whm added 22Mar2018 for detecting callers of PlaceBox()
                pApp->m_bMovingToDifferentPile = FALSE;

                // don't proceed further because the current vertical edit step has ended
				m_bTunnellingOut = TRUE; // so caller can use it
				return FALSE;
			}
		}

		if (!pApp->m_bSingleStep)
		{
			pApp->m_bAutoInsert = FALSE; // cause halt, if auto lookup & inserting is ON
		}

		// ensure the view knows the pile pointer is no longer valid
		pApp->m_pActivePile = (CPile*)NULL;
		pApp->m_nActiveSequNum = -1;
		if (m_bSuppressStoreForAltBackspaceKeypress)
            m_SaveTargetPhrase.Empty();
		m_bSuppressStoreForAltBackspaceKeypress = FALSE; // make sure it's off before returning
		m_bTemporarilySuspendAltBKSP = FALSE;

        // whm added 22Mar2018 for detecting callers of PlaceBox()
        pApp->m_bMovingToDifferentPile = FALSE;

        return FALSE; // we are at the end of the document
	}
	else
	{
		// the pNewPile is valid, so proceed

        // don't commit to the new pile if we are in vertical edit mode, until we've
        // checked the pile is not in the gray text area...
        // if vertical editing is currently in progress we must check if the lookup target
        // is within the editable span, if not then control has moved the box into the gray
        // area beyond the editable span and that means a step transition is warranted &
        // the user should be asked what step is next.
		// BUT!! (BEW 7Feb18) If the pNewPile has no translation (ie. target) text yet,
		// we don't want OnIdle() to keep on calling OnePass(), as then the user gets 
		// no chance to type in an adaptation at the new location. So check, and clear
		//  m_bAutoInsert to FALSE so as to halt the forward advances of the phrasebox. 
		if (gbVerticalEditInProgress)
		{
			int m_nCurrentSequNum = pNewPile->GetSrcPhrase()->m_nSequNumber;
			m_bTunnellingOut = FALSE; // ensure default value set
			bool bCommandPosted = pView->VerticalEdit_CheckForEndRequiringTransition(
									m_nCurrentSequNum,nextStep); // bForceTransition is FALSE
			if (bCommandPosted)
			{
                // whm added 22Mar2018 for detecting callers of PlaceBox()
                pApp->m_bMovingToDifferentPile = FALSE;

                // don't proceed further because the current vertical edit step has ended
				m_bTunnellingOut = TRUE; // so caller can use it
				return FALSE; // try returning FALSE
			}
			// BEW 19Oct15 No transition of vert edit modes,
			// so we can store this location on the app
			gpApp->m_vertEdit_LastActiveSequNum = m_nCurrentSequNum;
#if defined(_DEBUG)
			wxLogDebug(_T("VertEdit PhrBox, MoveToNextPile() storing loc'n: %d "), m_nCurrentSequNum);
#endif
			
		}

        // set active pile, and same var on the phrase box, and active sequ number - but
        // note that only the active sequence number will remain valid if a merge is
        // required; in the latter case, we will have to recalc the layout after the merge
        // and set the first two variables again
		pApp->m_pActivePile = pNewPile;
		pApp->m_nActiveSequNum = pNewPile->GetSrcPhrase()->m_nSequNumber;
		m_nCurrentSequNum = pApp->m_nActiveSequNum; // global, for use by auto-saving

		// refactored design: we want the old pile's strip to be marked as invalid and the
		// strip index added to the CLayout::m_invalidStripArray
		pDoc->ResetPartnerPileWidth(pOldActiveSrcPhrase);

        // look ahead for a match with KB phrase content at this new active location
        // LookAhead (July 2003) has been ammended for auto-capitalization support; and
        // since it does a KB lookup, it will set gbMatchedKB_UCentry TRUE or FALSE; and if
        // an entry is found, any needed case change will have been done prior to it
        // returning (the result is in the global variable: translation)
		bAdaptationAvailable = LookAhead(pNewPile);
		pView->RemoveSelection();

		// check if we found a match and have an available adaptation string ready
		if (bAdaptationAvailable)
		{
			pApp->m_pTargetBox->m_bAbandonable = FALSE;

            // adaptation is available, so use it if the source phrase has only a single
            // word, but if it's multi-worded, we must first do a merge and recalc of the
            // layout
            if (!gbIsGlossing && !m_bSuppressMergeInMoveToNextPile)
			{
                // this merge is done here only if an auto-insert can be done
				if (m_nWordsInPhrase > 1) // m_nWordsInPhrase is a global, set in LookAhead()
										// or in LookUpSrcWord()
				{
					// do the needed merge, etc.
					pApp->bLookAheadMerge = TRUE; // set static flag to ON
					bool bSaveFlag = m_bAbandonable; // the box is "this"
					pView->MergeWords();
					m_bAbandonable = bSaveFlag; // preserved the flag across the merge
					pApp->bLookAheadMerge = FALSE; // restore static flag to OFF
				}
			}

			// BEW changed 9Apr12 to support discontinuous highlighting spans
            if (m_nWordsInPhrase == 1 || !m_bSuppressMergeInMoveToNextPile)
			{
                // When nWordsInPhrase > 1, since the call of LookAhead() didn't require
                // user choice of the adaptation or gloss for the merger, it wasn't done in
                // the latter function, and so is done here automatically (because there is
                // a unique adaptation available) and so it is appropriate to make this
                // location have background highlighting, since the adaptation is now to be
                // auto-inserted after the merger was done above. Note: the
                // m_bSuppressMergeInMoveToNextPile flag will be FALSE if the merger was not
                // done in the prior LookAhead() call (with ChooseTranslation() being
                // required for the user to manually choose which adaptation is wanted); we
                // use that fact in the test above.
                // When m_nWordsInPhrase is 1, there is no merger involved and the
				// auto-insert to be done now requires background highlighting (Note, we
				// set the flag when appropriate, but only suppress doing the background
				// colour change in the CCell's Draw() function, if the user has requested
				// that no background highlighting be used - that uses the
				// m_bSuppressTargetHighlighting flag with TRUE value to accomplish the
				// suppression)
                pLayout->SetAutoInsertionHighlightFlag(pNewPile);
			}
            // assign the translation text - but check it's not "<Not In KB>", if it is,
            // phrase box can have m_targetStr, turn OFF the m_bSaveToKB flag, DON'T halt
            // auto-inserting if it is on, (in the very earliest versions I made it halt)
            // -- for version 1.4.0 and onwards, this does not change because when auto
            // inserting, we must have a default translation for a 'not in kb' one - and
            // the only acceptable default is a null string. The above applies when
            // gbIsGlossing is OFF
			wxString str = m_Translation; // m_Translation set within LookAhead()

			if (!gbIsGlossing && (m_Translation == _T("<Not In KB>")))
			{
				pApp->m_bSaveToKB = FALSE;
                m_Translation = pNewPile->GetSrcPhrase()->m_targetStr; // probably empty
				pApp->m_targetPhrase = m_Translation;
				bWantSelect = FALSE;
				pApp->m_pTargetBox->m_bAbandonable = TRUE;
				pNewPile->GetSrcPhrase()->m_bHasKBEntry = FALSE;
				pNewPile->GetSrcPhrase()->m_bNotInKB = TRUE; // ensures * shows above
															 // this srcPhrase
			}
			else
			{
				pApp->m_targetPhrase = m_Translation;
				bWantSelect = FALSE;
			}
#ifdef Highlighting_Bug
			// BEW changed 9Apr12 for support of discontinuous highlighting spans
			wxLogDebug(_T("PhraseBox::MoveToNextPile(), hilighting: sequnum = %d  where the user chose:  %s  for source:  %s"),
				m_nCurrentSequNum, translation, pNewPile->GetSrcPhrase()->m_srcPhrase);
#endif
            // treat auto insertion as if user typed it, so that if there is a
            // user-generated extension done later, the inserted translation will not be
            // removed and copied source text used instead; since user probably is going to
            // just make a minor modification
			pApp->m_bUserTypedSomething = TRUE;

            // get a widened pile pointer for the new active location, and we want the
            // pile's strip to be marked as invalid and the strip index added to the
            // CLayout::m_invalidStripArray
			if (pNewPile != NULL)
			{
				pDoc->ResetPartnerPileWidth(pNewPile->GetSrcPhrase());
			}
		}
		else // the lookup determined that no adaptation (or gloss when glossing), or
			 // <Not In KB> entry, is available
		{
			pNewPile = pApp->m_pActivePile;

			// clear all storage of the earlier location's target text
			m_Translation.Empty();
			pApp->m_targetPhrase.Empty();
			// initialize the phrase box too, so it doesn't carry the old string
			// to the next pile's cell
			ChangeValue(pApp->m_targetPhrase);

            // RecalcLayout call when there is no adaptation available from the LookAhead,
            // (or user cancelled when shown the Choose Translation dialog from within the
            // LookAhead() function, having matched) we must cause auto lookup and
            // inserting to be turned off, so that the user can do a manual adaptation; but
            // if the m_bAcceptDefaults flag is on, then the copied source (having been
            // through c.changes) is accepted without user input, the m_bAutoInsert flag is
            // turned back on, so processing will continue
            pApp->m_nActiveSequNum = pNewPile->GetSrcPhrase()->m_nSequNumber;

            if (!pApp->m_bSingleStep)
            {
                // This call internally sets m_bAutoInsert to FALSE at its first line, but
                // if in cc mode and m_bAcceptDefaults is true, then cc keeps the box moving
                // forward by resetting m_bAutoInsert to TRUE before it returns
                HandleUnsuccessfulLookup_InAutoAdaptMode_AsBestWeCan(
                    pApp, pView, pNewPile, bWantSelect);
            }
            else // it's single step mode
            {
                HandleUnsuccessfulLookup_InSingleStepMode_AsBestWeCan(
                    pApp, pView, pNewPile, bWantSelect);
            }

            // get a widened pile pointer for the new active location, and we want the
            // pile's strip to be marked as invalid and the strip index added to the
            // CLayout::m_invalidStripArray
			if (pNewPile != NULL)
			{
				pDoc->ResetPartnerPileWidth(pNewPile->GetSrcPhrase());
			}
		}

        // initialize the phrase box too, so it doesn't carry the old string to the next
        // pile's cell
        ChangeValue(pApp->m_targetPhrase); //SetWindowText(pApp->m_targetPhrase);

        // if we merged and moved, we have to update pNewPile, because we have done a
		// RecalcLayout in the LookAhead() function; it's possible to return from
		// LookAhead() without having done a recalc of the layout, so the else block
		// should cover that situation
		if (m_bCompletedMergeAndMove)
		{
			pApp->m_pActivePile = pView->GetPile(pApp->m_nActiveSequNum);
		}
		else
		{
		    // do we need this one?? I think so, but should step it to make sure
#ifdef _NEW_LAYOUT
		    pLayout->RecalcLayout(pApp->m_pSourcePhrases, keep_strips_keep_piles);
#else
		    pLayout->RecalcLayout(pApp->m_pSourcePhrases, create_strips_keep_piles);
#endif
		    // get the new active pile
		    pApp->m_pActivePile = pView->GetPile(pApp->m_nActiveSequNum);
		    wxASSERT(pApp->m_pActivePile != NULL);
		}

        // if the user has turned on the sending of synchronized scrolling messages send
        // the relevant message once auto-inserting halts, because we don't want to make
        // other applications sync scroll during auto-insertions, as it could happen very
        // often and the user can't make any visual use of what would be happening anyway;
        // even if a Cancel and Select is about to be done, a sync scroll is appropriate
        // now, provided auto-inserting has halted
		if (!pApp->m_bIgnoreScriptureReference_Send && !pApp->m_bAutoInsert)
		{
			pView->SendScriptureReferenceFocusMessage(pApp->m_pSourcePhrases,
													pApp->m_pActivePile->GetSrcPhrase());
		}

		// update status bar with project name
		pApp->RefreshStatusBarInfo();

		// recreate the phraseBox using the stored information
		if (bWantSelect)
		{
			//pApp->m_nStartChar = 0;
			pApp->m_nStartChar = -1; // wx uses -1, not 0 as in MFC
			pApp->m_nEndChar = -1;
		}
		else
		{
			int len = GetLineLength(0); // 0 = first line, only line
			pApp->m_nStartChar = len;
			pApp->m_nEndChar = len;
		}

        // fix the m_bSaveToKB flag, depending on whether or not srcPhrase is in kb; but
        // this applies only when adapting, not glossing
		if (!gbIsGlossing && !pApp->m_pActivePile->GetSrcPhrase()->m_bHasKBEntry &&
			pApp->m_pActivePile->GetSrcPhrase()->m_bNotInKB)
		{
			pApp->m_bSaveToKB = FALSE;
			pApp->m_targetPhrase.Empty();
		}
		else
		{
			pApp->m_bSaveToKB = TRUE;
		}

		m_bCompletedMergeAndMove = FALSE; // make sure it's cleared

        // BEW note 24Mar09: later we may use clipping (the comment below may not apply in
        // the new design anyway)
		pView->Invalidate(); // do the whole client area, because if target font is larger
            // than the source font then changes along the line throw words off screen and
            // they get missed and eventually app crashes because active pile pointer will
            // get set to NULL
		pLayout->PlaceBox();

		if (bWantSelect)
			SetModify(TRUE); // our own SetModify(); calls MarkDirty()
		else
			SetModify(FALSE); // our own SetModify(); calls DiscardEdits()

        // make sure gbSuppressMergeInMoveToNextPile is reset to the default value
        m_bSuppressMergeInMoveToNextPile = FALSE;

        // whm added 22Mar2018 for detecting callers of PlaceBox()
        pApp->m_bMovingToDifferentPile = FALSE;

        return TRUE;
	}
}

// returns TRUE if all was well, FALSE if something prevented the move
// Note: this is based on MoveToNextPile(), but with added code for transliterating - and
// recall that when transliterating, the extra code may be used, or the normal KB lookup
// code may be used, depending on the user's assessment of whether the transliterating
// converter did its job correctly, or not correctly, respectively. When control is in this
// function CAdapt_ItApp::m_bTransliterationMode will be TRUE, and can therefore be
// assumed; likewise the global boolean gbIsGlossing will be FALSE (because
// transliteration mode is not available when glossing mode is turned ON), and so that too
// can be assumed
// BEW 13Apr10, changes needed for support of doc version 5
// BEW 21Jun10, no changes needed for support of kbVersion 2, & removed pView from signature
bool CPhraseBox::MoveToNextPile_InTransliterationMode(CPile* pCurPile)
{
	CAdapt_ItApp* pApp = (CAdapt_ItApp*)&wxGetApp();
	pApp->m_preGuesserStr.Empty(); // BEW 27Nov14, in case a src string, or modified string
		// is stored ready for user's Esc keypress to restore the pre-guesser
		// form, clear it, because the box is gunna move and we want it
		// restored to default empty ready for next box landing location
	// whm modified 29Mar12. Left mouse clicks now beep only when certain parts of
	// the canvas are clicked on, and allows other clicks to act normally (such as
	// the opening/closing of the ViewFilteredMaterial dialog and the Notes dialog).
	//bool bNoError = TRUE;
	bool bWantSelect = FALSE; // set TRUE if any initial text in the new location is to be
							  // shown selected
	// store the translation in the knowledge base
	//pApp->limiter = 0; // BEW added Aug13, to support OnIdle() hack for m_targetStr non-stick bug // bug fixed 24Sept13 BEW
	CAdapt_ItView* pView = pApp->GetView();
	CAdapt_ItDoc* pDoc = pApp->GetDocument();
	bool bOK;
	m_bBoxTextByCopyOnly = FALSE; // restore default setting
	CSourcePhrase* pOldActiveSrcPhrase = pCurPile->GetSrcPhrase();
	CLayout* pLayout = GetLayout();

	// make sure pApp->m_targetPhrase doesn't have any final spaces
	RemoveFinalSpaces(pApp->m_pTargetBox,&pApp->m_targetPhrase);

	CPile* pNextEmptyPile = pView->GetNextEmptyPile(pCurPile);
	if (pNextEmptyPile == NULL)
	{
		// no more empty piles in the current document. We can just continue at this point
		// since we do this call again below
		;
	}
	else
	{
		// don't move forward if it means moving to an empty retranslation pile, but only for
		// when we are adapting. When glossing, the box is allowed to be within retranslations
		bool bNotInRetranslation = CheckPhraseBoxDoesNotLandWithinRetranslation(pView,
															pNextEmptyPile, pCurPile);
		if (!bNotInRetranslation)
		{
			// the phrase box landed in a retranslation, so halt the lookup and insert loop
			// so the user can do something manually to achieve what he wants in the
			// viscinity of the retranslation
			return FALSE;
		}
		// continue processing below if the phrase box did not land in a retranslation
	}

	// if the location we are leaving is a <Not In KB> one, we want to skip the store & fourth
	// line creation --- as of Dec 18, version 1.4.0, according to Susanna Imrie's
	// recommendation, I've changed this so it will allow a non-null adaptation to remain at
	// this location in the document, but just to suppress the KB store; if glossing is ON, then
	// being a <Not In KB> location is irrelevant, and we will want the store done normally - but
	// to the glossing KB of course
	// BEW addition 21Apr06 to support transliterating better (showing transiterations)
	if (m_bSuppressStoreForAltBackspaceKeypress)
	{
		pApp->m_targetPhrase = m_SaveTargetPhrase; // set it up in advance, from last LookAhead() call
		goto c;
	}
	if (!gbIsGlossing && !pOldActiveSrcPhrase->m_bHasKBEntry && pOldActiveSrcPhrase->m_bNotInKB)
	{
		// if the user edited out the <Not In KB> entry from the KB editor, we need to put
		// it back so that the setting is preserved (the "right" way to change the setting is to
		// use the toolbar checkbox - this applies when adapting, not glossing)
		pApp->m_pKB->Fix_NotInKB_WronglyEditedOut(pCurPile);
		goto b;
	}

	// make the punctuated target string, but only if adapting; note, for auto capitalization
	// ON, the function will change initial lower to upper as required, whatever punctuation
	// regime is in place for this particular sourcephrase instance

    // BEW added 19Apr06 for support of Alt + Backspace keypress suppressing store on the
    // phrase box move which is a feature for power users requested by Bob Eaton; the code
    // in the first block is for this new (undocumented) feature - power uses will have
    // knowledge of it from an article to be placed in Word&Deed by Bob. Only needed for
    // adapting mode, so the glossing mode case is commented out
c:	bOK = TRUE;
	if (m_bSuppressStoreForAltBackspaceKeypress)
	{
		// when we don't want to store in the KB, we still have some things to do
		// to get appropriate m_adaption and m_targetStr members set up for the doc...
		// when adapting, fill out the m_targetStr member of the CSourcePhrase instance,
		// and do any needed case conversion and get punctuation in place if required
		pView->MakeTargetStringIncludingPunctuation(pOldActiveSrcPhrase, pApp->m_targetPhrase);

		// the m_targetStr member may now have punctuation, so get rid of it
		// before assigning whatever is left to the m_adaption member
		wxString strKeyOnly = pOldActiveSrcPhrase->m_targetStr;
		pView->RemovePunctuation(pDoc,&strKeyOnly,from_target_text);

		// set the m_adaption member too
		pOldActiveSrcPhrase->m_adaption = strKeyOnly;

		// let the user see the unpunctuated string in the phrase box as visual feedback
		pApp->m_targetPhrase = strKeyOnly;

        // now do a store, but only of <Not In KB>, (StoreText uses
        // m_bSuppressStoreForAltBackspaceKeypress == TRUE to get this job done rather than
        // a normal store) & sets flags appropriately (Note, while we pass in
        // pApp->m_targetPhrase, the phrase box contents string, we StoreText doesn't use
        // it when m_bSuppressStoreForAltBackspaceKeypress is TRUE, but internally sets a
        // local string to "<Not In KB>" and stores that instead) BEW 27Jan09, nothing more
        // needed here
        if (gbIsGlossing)
		{
			bOK = pApp->m_pGlossingKB->StoreText(pOldActiveSrcPhrase, pApp->m_targetPhrase);
		}
		else
		{
			bOK = pApp->m_pKB->StoreText(pOldActiveSrcPhrase, pApp->m_targetPhrase);
		}
	}
	else
	{
		bOK = DoStore_NormalOrTransliterateModes(pApp, pDoc, pView, pCurPile,
												pApp->m_bTransliterationMode);
	}
	if (!bOK)
	{
		DealWithUnsuccessfulStore(pApp, pView, pNextEmptyPile);
		return FALSE; // can't move until a valid adaption (which could be null) is supplied
	}

	// since we are moving, make sure the default m_bSaveToKB value is set
b:	pApp->m_bSaveToKB = TRUE;

	// move to next pile's cell which has no adaptation yet
	pApp->m_bUserTypedSomething = FALSE; // user has not typed at the new location yet
	bool bAdaptationAvailable = FALSE;
	CPile* pNewPile = pView->GetNextEmptyPile(pCurPile); // this call does not update
														 // the active sequ number
	// if necessary restore default button image, and m_bCopySourcePunctuation to TRUE
	wxCommandEvent event;
	if (!pApp->m_bCopySourcePunctuation)
	{
		pApp->GetView()->OnToggleEnablePunctuationCopy(event);
	}
	if (pNewPile == NULL)
	{
		// we deem vertical editing current step to have ended if control gets into this
		// block, so user has to be asked what to do next if vertical editing is currently
		// in progress; and we tunnel out before m_nActiveSequNum can be set to -1 (otherwise
		// vertical edit will crash when recalc layout is tried with a bad sequ num value)
		if (gbVerticalEditInProgress)
		{
			m_bTunnellingOut = FALSE; // ensure default value set
			bool bCommandPosted = pView->VerticalEdit_CheckForEndRequiringTransition(-1,
							nextStep, TRUE); // bForceTransition is TRUE
			if (bCommandPosted)
			{
				// don't proceed further because the current vertical edit step has ended
				m_bTunnellingOut = TRUE; // so caller can use it
				return FALSE;
			}
			else
			{
				// BEW 19Oct15 No transition of vert edit modes,
				// so we can store this location on the app
				gpApp->m_vertEdit_LastActiveSequNum = pNewPile->GetSrcPhrase()->m_nSequNumber;
#if defined(_DEBUG)
				wxLogDebug(_T("VertEdit PhrBox, MoveToNextPile_InTransliterationMode() storing loc'n: %d "), 
					pNewPile->GetSrcPhrase()->m_nSequNumber);
#endif
			}
		}

		if (!pApp->m_bSingleStep)
		{
			pApp->m_bAutoInsert = FALSE; // cause halt, if auto lookup & inserting is ON
		}

		// ensure the view knows the pile pointer is no longer valid
		pApp->m_pActivePile = (CPile*)NULL;
		pApp->m_nActiveSequNum = -1;
		if (m_bSuppressStoreForAltBackspaceKeypress)
            m_SaveTargetPhrase.Empty();
		m_bSuppressStoreForAltBackspaceKeypress = FALSE; // make sure it's off before returning
		m_bTemporarilySuspendAltBKSP = FALSE;

		return FALSE; // we are at the end of the document
	}
	else
	{
		// the pNewPile is valid, so proceed

        // don't commit to the new pile if we are in vertical edit mode, until we've
        // checked the pile is not in the gray text area...
        // if vertical editing is currently in progress we must check if the lookup target
        // is within the editable span, if not then control has moved the box into the gray
        // area beyond the editable span and that means a step transition is warranted &
        // the user should be asked what step is next
		if (gbVerticalEditInProgress)
		{
			int m_nCurrentSequNum = pNewPile->GetSrcPhrase()->m_nSequNumber;
			m_bTunnellingOut = FALSE; // ensure default value set
			bool bCommandPosted = pView->VerticalEdit_CheckForEndRequiringTransition(
									m_nCurrentSequNum,nextStep); // bForceTransition is FALSE
			if (bCommandPosted)
			{
				// don't proceed further because the current vertical edit step has ended
				m_bTunnellingOut = TRUE; // so caller can use it
				return FALSE; // try returning FALSE
			}
			else
			{
				// BEW 19Oct15 No transition of vert edit modes,
				// so we can store this location on the app
				gpApp->m_vertEdit_LastActiveSequNum = pNewPile->GetSrcPhrase()->m_nSequNumber;
#if defined(_DEBUG)
				wxLogDebug(_T("VertEdit PhrBox, MoveToNextPile_InTransliterationMode() storing loc'n: %d "), 
					pNewPile->GetSrcPhrase()->m_nSequNumber);
#endif
			}
		}

        // set active pile, and same var on the phrase box, and active sequ number - but
        // note that only the active sequence number will remain valid if a merge is
        // required; in the latter case, we will have to recalc the layout after the merge
        // and set the first two variables again
		pApp->m_pActivePile = pNewPile;
		pApp->m_nActiveSequNum = pNewPile->GetSrcPhrase()->m_nSequNumber;
		m_nCurrentSequNum = pApp->m_nActiveSequNum; // global, for use by auto-saving

        // adjust width pf the pile pointer for the new active location, and we want the
        // pile's strip to be marked as invalid and the strip index added to the
        // CLayout::m_invalidStripArray
		pDoc->ResetPartnerPileWidth(pOldActiveSrcPhrase);

        // look ahead for a match with KB phrase content at this new active location
        // LookAhead (July 2003) has been ammended for auto-capitalization support; and
        // since it does a KB lookup, it will set gbMatchedKB_UCentry TRUE or FALSE; and if
        // an entry is found, any needed case change will have been done prior to it
        // returning (the result is in the global variable: translation)
		bAdaptationAvailable = LookAhead(pNewPile);
		pView->RemoveSelection();

		// check if we found a match and have an available adaptation string ready
		if (bAdaptationAvailable)
		{
			pApp->m_pTargetBox->m_bAbandonable = FALSE;

             // adaptation is available, so use it if the source phrase has only a single
            // word, but if it's multi-worded, we must first do a merge and recalc of the
            // layout
			if (!gbIsGlossing && !m_bSuppressMergeInMoveToNextPile)
			{
                // this merge is done here only if an auto-insert can be done
				if (m_nWordsInPhrase > 1) // m_nWordsInPhrase is a global, set in LookAhead()
										// or in LookUpSrcWord()
				{
					// do the needed merge, etc.
					pApp->bLookAheadMerge = TRUE; // set static flag to ON
					bool bSaveFlag = m_bAbandonable; // the box is "this"
					pView->MergeWords();
					m_bAbandonable = bSaveFlag; // preserved the flag across the merge
					pApp->bLookAheadMerge = FALSE; // restore static flag to OFF
				}
			}

			// BEW changed 9Apr12 to support discontinuous highlighting spans
			if (m_nWordsInPhrase == 1 || !m_bSuppressMergeInMoveToNextPile)
			{
                // When nWordsInPhrase > 1, since the call of LookAhead() didn't require
                // user choice of the adaptation or gloss for the merger, it wasn't done in
                // the latter function, and so is done here automatically (because there is
                // a unique adaptation available) and so it is appropriate to make this
                // location have background highlighting, since the adaptation is now to be
                // auto-inserted after the merger was done above. Note: the
                // m_bSuppressMergeInMoveToNextPile flag will be FALSE if the merger was not
                // done in the prior LookAhead() call (with ChooseTranslation() being
                // required for the user to manually choose which adaptation is wanted); we
                // use that fact in the test above.
                // When m_nWordsInPhrase is 1, there is no merger involved and the
				// auto-insert to be done now requires background highlighting (Note, we
				// set the flag when appropriate, but only suppress doing the background
				// colour change in the CCell's Draw() function, if the user has requested
				// that no background highlighting be used - that uses the
				// m_bSuppressTargetHighlighting flag with TRUE value to accomplish the
				// suppression)
                pLayout->SetAutoInsertionHighlightFlag(pNewPile);
			}
            // assign the translation text - but check it's not "<Not In KB>", if it is,
            // phrase box can have m_targetStr, turn OFF the m_bSaveToKB flag, DON'T halt
            // auto-inserting if it is on, (in the very earliest versions I made it halt)
            // -- for version 1.4.0 and onwards, this does not change because when auto
            // inserting, we must have a default translation for a 'not in kb' one - and
            // the only acceptable default is a null string. The above applies when
            // gbIsGlossing is OFF
			wxString str = m_Translation; // m_Translation set within LookAhead()

            // BEW added 21Apr06, so that when transliterating the lookup puts a fresh
            // transliteration of the source when it finds a <Not In KB> entry, since the
            // latter signals that the SIL Converters conversion yields a correct result
            // for this source text, so we want the user to get the feedback of seeing it,
            // but still just have <Not In KB> in the KB entry
			if (!pApp->m_bSingleStep && (m_Translation == _T("<Not In KB>"))
											&& m_bTemporarilySuspendAltBKSP)
			{
				m_bSuppressStoreForAltBackspaceKeypress = TRUE;
				m_bTemporarilySuspendAltBKSP = FALSE;
			}

			if (m_bSuppressStoreForAltBackspaceKeypress && (m_Translation == _T("<Not In KB>")))
			{
				pApp->m_bSaveToKB = FALSE;
 				CSourcePhrase* pSrcPhr = pNewPile->GetSrcPhrase();
				wxString str = pView->CopySourceKey(pSrcPhr, pApp->m_bUseConsistentChanges);
				bWantSelect = FALSE;
				pApp->m_pTargetBox->m_bAbandonable = TRUE;
				pSrcPhr->m_bHasKBEntry = FALSE;
				pSrcPhr->m_bNotInKB = TRUE; // ensures * shows above
				pSrcPhr->m_adaption = str;
				pSrcPhr->m_targetStr = pSrcPhr->m_precPunct + str;
				pSrcPhr->m_targetStr += pSrcPhr->m_follPunct;
                m_Translation = pSrcPhr->m_targetStr;
				pApp->m_targetPhrase = m_Translation;
                m_SaveTargetPhrase = m_Translation; // to make it available on
												 // next auto call of OnePass()
			}
			// continue with the legacy code
			else if (m_Translation == _T("<Not In KB>"))
			{
				pApp->m_bSaveToKB = FALSE;
                m_Translation = pNewPile->GetSrcPhrase()->m_targetStr; // probably empty
				pApp->m_targetPhrase = m_Translation;
				bWantSelect = FALSE;
				pApp->m_pTargetBox->m_bAbandonable = TRUE;
				pNewPile->GetSrcPhrase()->m_bHasKBEntry = FALSE;
				pNewPile->GetSrcPhrase()->m_bNotInKB = TRUE; // ensures * shows above this srcPhrase
			}
			else
			{
				pApp->m_targetPhrase = m_Translation;
				bWantSelect = FALSE;

				if (m_bSuppressStoreForAltBackspaceKeypress)
				{
                    // was the normal entry found while the
                    // m_bSuppressStoreForAltBackspaceKeypress flag was TRUE? Then we have
                    // to turn the flag off for a while, but turn it on programmatically
                    // later if we are still in Automatic mode and we come to another <Not
                    // In KB> entry. We can do this with another BOOL defined for this
                    // purpose
					m_bTemporarilySuspendAltBKSP = TRUE;
					m_bSuppressStoreForAltBackspaceKeypress = FALSE;
				}
			}

            // treat auto insertion as if user typed it, so that if there is a
            // user-generated extension done later, the inserted translation will not be
            // removed and copied source text used instead; since user probably is going to
            // just make a minor modification
			pApp->m_bUserTypedSomething = TRUE;

            // get a widened pile pointer for the new active location, and we want the
            // pile's strip to be marked as invalid and the strip index added to the
            // CLayout::m_invalidStripArray
			if (pNewPile != NULL)
			{
				pDoc->ResetPartnerPileWidth(pNewPile->GetSrcPhrase());
			}
		}
		else // the lookup determined that no adaptation (or gloss when glossing), or
			 // <Not In KB> entry, is available
		{
			// we're gunna halt, so this is the time to clear the flag
			if (m_bSuppressStoreForAltBackspaceKeypress)
                m_SaveTargetPhrase.Empty();
			m_bSuppressStoreForAltBackspaceKeypress = FALSE; // make sure it's off before returning

			pNewPile = pApp->m_pActivePile; 
            // whm modified 22Feb2018 to remove logic related to Cancel and Select.
            // [Original comment below]:
            // ensure its valid, we may get here after a
            // RecalcLayout call when there is no adaptation available from the LookAhead,
            // (or user cancelled when shown the Choose Translation dialog from within the
            // LookAhead() function, having matched) we must cause auto lookup and
            // inserting to be turned off, so that the user can do a manual adaptation; but
            // if the m_bAcceptDefaults flag is on, then the copied source (having been
            // through c.changes) is accepted without user input, the m_bAutoInsert flag is
            // turned back on, so processing will continue
            pApp->m_nActiveSequNum = pNewPile->GetSrcPhrase()->m_nSequNumber;
			if (!pApp->m_bSingleStep)
			{
				HandleUnsuccessfulLookup_InAutoAdaptMode_AsBestWeCan(pApp, pView, pNewPile, bWantSelect);
			}
			else // it's single step mode
			{
				HandleUnsuccessfulLookup_InSingleStepMode_AsBestWeCan(pApp, pView, pNewPile, bWantSelect);
			}

            // get a widened pile pointer for the new active location, and we want the
            // pile's strip to be marked as invalid and the strip index added to the
            // CLayout::m_invalidStripArray
			if (pNewPile != NULL)
			{
				pDoc->ResetPartnerPileWidth(pNewPile->GetSrcPhrase());
			}
		}

		// initialize the phrase box too, so it doesn't carry the old string to the next
		// pile's cell
		ChangeValue(pApp->m_targetPhrase); //SetWindowText(pApp->m_targetPhrase);

		// if we merged and moved, we have to update pNewPile, because we have done a
		// RecalcLayout in the LookAhead() function; it's possible to return from
		// LookAhead() without having done a recalc of the layout, so the else block
		// should cover that situation
        if (m_bCompletedMergeAndMove)
		{
			pApp->m_pActivePile = pView->GetPile(pApp->m_nActiveSequNum);
		}
		else
		{
		    // do we need this one?? I think so
    #ifdef _NEW_LAYOUT
		    pLayout->RecalcLayout(pApp->m_pSourcePhrases, keep_strips_keep_piles);
    #else
		    pLayout->RecalcLayout(pApp->m_pSourcePhrases, create_strips_keep_piles);
    #endif
		    // in call above, m_stripArray gets rebuilt, but m_pileList is left untouched

		    // get the new active pile
		    pApp->m_pActivePile = pView->GetPile(pApp->m_nActiveSequNum);
		    wxASSERT(pApp->m_pActivePile != NULL);
		}

        // if the user has turned on the sending of synchronized scrolling messages send
        // the relevant message once auto-inserting halts, because we don't want to make
        // other applications sync scroll during auto-insertions, as it could happen very
        // often and the user can't make any visual use of what would be happening anyway;
        // even if a Cancel and Select is about to be done, a sync scroll is appropriate
        // now, provided auto-inserting has halted
		if (!pApp->m_bIgnoreScriptureReference_Send && !pApp->m_bAutoInsert)
		{
			pView->SendScriptureReferenceFocusMessage(pApp->m_pSourcePhrases,
													pApp->m_pActivePile->GetSrcPhrase());
		}

		// update status bar with project name
		pApp->RefreshStatusBarInfo();

		// recreate the phraseBox using the stored information
		if (bWantSelect)
		{
			pApp->m_nStartChar = -1; // WX uses -1, not 0 as in MFC
			pApp->m_nEndChar = -1;
		}
		else
		{
			int len = GetLineLength(0); // 0 = first line, only line
			pApp->m_nStartChar = len;
			pApp->m_nEndChar = len;
		}

        // fix the m_bSaveToKB flag, depending on whether or not srcPhrase is in kb; but
        // this applies only when adapting, not glossing
		if (!gbIsGlossing && !pApp->m_pActivePile->GetSrcPhrase()->m_bHasKBEntry &&
			pApp->m_pActivePile->GetSrcPhrase()->m_bNotInKB)
		{
			pApp->m_bSaveToKB = FALSE;
			pApp->m_targetPhrase.Empty();
		}
		else
		{
			pApp->m_bSaveToKB = TRUE;
		}

        m_bCompletedMergeAndMove = FALSE; // make sure it's cleared

		// BEW note 24Mar09: later we may use clipping (the comment below may not apply in
		// the new design anyway)
		pView->Invalidate(); // do the whole client area, because if target font is larger
            // than the source font then changes along the line throw words off screen and
            // they get missed and eventually app crashes because active pile pointer will
            // get set to NULL
		pLayout->PlaceBox();

		if (bWantSelect)
			SetModify(TRUE); // our own SetModify(); calls MarkDirty()
		else
			SetModify(FALSE); // our own SetModify(); calls DiscardEdits()

        // make sure gbSuppressMergeInMoveToNextPile is reset to the default value
        m_bSuppressMergeInMoveToNextPile = FALSE;

        return TRUE;
	}
}

// BEW 13Apr10, no changes needed for support of doc version 5
bool CPhraseBox::IsActiveLocWithinSelection(const CAdapt_ItView* WXUNUSED(pView), const CPile* pActivePile)
{
	CAdapt_ItApp* pApp = &wxGetApp();
	wxASSERT(pApp != NULL);
	bool bYes = FALSE;
	const CCellList* pList = &pApp->m_selection;
	if (pList->GetCount() == 0)
		return bYes;
	CCellList::Node* pos = pList->GetFirst();
	while (pos != NULL)
	{
		CCell* pCell = (CCell*)pos->GetData();
		pos = pos->GetNext();
		//if (pCell->m_pPile == pActivePile)
		if (pCell->GetPile() == pActivePile)
		{
			bYes = TRUE;
			break;
		}
	}
	return bYes;
}

void CPhraseBox::CloseDropDown()
{
#if wxVERSION_NUMBER < 2900
    ;
#else
    this->Dismiss();
#endif
}

void CPhraseBox::PopupDropDownList()
{
#if wxVERSION_NUMBER < 2900
    if (!this->IsPopupShown())
    {
        //wxLogDebug(_T("DropDown: call ShowPopup()"));
        this->ShowPopup(); // The Popup() function is ShowPopup() in wx2.8.12, so conditional compile for wxversion
    }
#else
    if (!this->IsPopupShown())
    {
        //wxLogDebug(_T("DropDown: call Popup()"));
        this->Popup();
    }
#endif
}

// return TRUE if we made a match and there is a translation to be inserted (see static var
// below); return FALSE if there was no match. This function assumes that the pNewPile pointer
// passed in is the active pile, and that CAdapt_ItApp::m_nActiveSequNum is the correct
// index within the m_pSourcePhrases list for the CSourcePhrase instance pointed at by the
// m_pSrcPhrase member of pNewPile
// BEW 13Apr10, a small change needed for support of doc version 5
// BEW 21Jun10, updated for support of kbVersion 2
// BEW 13Nov10, changed by Bob Eaton's request, for glossing KB to use all maps
bool CPhraseBox::LookAhead(CPile* pNewPile)
{
	// refactored 25Mar09 (old code is at end of file)
	CAdapt_ItApp* pApp = &wxGetApp();
	wxASSERT(pApp != NULL);
	CAdapt_ItView *pView = pApp->GetView(); // <<-- BEWARE if we later support multiple views/panes
	CLayout* pLayout = GetLayout();
	CSourcePhrase* pSrcPhrase = pNewPile->GetSrcPhrase();
	int		nNewSequNum = pSrcPhrase->m_nSequNumber; // sequ number at the new location
	wxString	phrases[10]; // store built phrases here, for testing against KB stored source phrases
	int		numPhrases;  // how many phrases were built in any one call of this LookAhead function
    m_Translation.Empty(); // clear the static variable, ready for a new translation, or gloss,
						 // if one can be found
	m_nWordsInPhrase = 0;	  // the global, initialize to value for no match
	m_bBoxTextByCopyOnly = FALSE; // restore default setting

	// we should never have an active selection at this point, so ensure there is no selection
	pView->RemoveSelection();


	// build the as many as 10 phrases based on first word at the new pile and the following
	// nine piles, or as many as are valid for phrase building (there are 7 conditions which
	// will stop the builds). When adapting, all 10 can be used; when glossing, and when
	// glossing, it now also supports more than one for KB insertions (but disallows merger)
	// For auto capitalization support, the 10 phrases strings are built from the document's
	// CSourcePhrase instances as before, no case changes made; and any case changes, and secondary
	// lookups if the primary (lower case) lookup fails when the source is upper case, are done
	// within the AutoCapsLookup( ) function which is called within FindMatchInKB( ) as called
	// below; so nothing needs to be done here.
	numPhrases = BuildPhrases(phrases, nNewSequNum, pApp->m_pSourcePhrases);

	// if the returned value is zero, no phrases were built, so this constitutes a non-match
	// BEW changed,9May05, to allow for the case when there is a merger at the last sourcephrase
	// of the document and it's m_bHasKBEntry flag (probably erroneously) is FALSE - the move
	// will detect the merger and BuildPhrases will not build any (so it exits returning zero)
	// and if we were to just detect the zero and return FALSE, a copy of the source would
	// overwrite the merger's translation - to prevent this, we'll detect this state of affairs
	// and cause the box to halt, but with the correct (old) adaptation showing. Then we have
	// unexpected behaviour (ie. the halt), but not an unexpected adaptation string.
	// BEW 6Aug13, added a gbIsGlossing == TRUE block to the next test
	if (numPhrases == 0)
	{
		if (gbIsGlossing)
		{
			if (pSrcPhrase->m_nSrcWords > 1 && !pSrcPhrase->m_gloss.IsEmpty())
			{
                m_Translation = pSrcPhrase->m_gloss;
				m_nWordsInPhrase = 1;
				pApp->m_bAutoInsert = FALSE; // cause a halt
				return TRUE;
			}
			else
			{
                // whm Note: return empty m_Translation string when pSrcPhrase->m_nSrcWords is not > 1 OR pSrcPhrase->m_gloss is EMPTY
				return FALSE; // return empty string in the m_Translation global wxString
			}
		}
		else
		{
			if (pSrcPhrase->m_nSrcWords > 1 && !pSrcPhrase->m_adaption.IsEmpty())
			{
                m_Translation = pSrcPhrase->m_adaption;
				m_nWordsInPhrase = 1; // strictly speaking not correct, but it's the value we want
									// on return to the caller so it doesn't try a merger
				pApp->m_bAutoInsert = FALSE; // cause a halt
				return TRUE;
			}
			else
			{
                // whm Note: return empty m_Translation string when pSrcPhrase->m_nSrcWords is not > 1 OR pSrcPhrase->m_adaption is EMPTY
                return FALSE; // return empty string in the m_Translation global wxString
			}
		}
	}

	// check these phrases, longest first, attempting to find a match in the KB
	// BEW 13Nov10, changed by Bob Eaton's request, for glossing KB to use all maps
	CKB* pKB;
	int nCurLongest; // index of the map which is highest having content, maps at higher index
					 // values currently have no content
	if (gbIsGlossing)
	{
		pKB = pApp->m_pGlossingKB;
		nCurLongest = pKB->m_nMaxWords; // no matches are possible for phrases longer
										// than nCurLongest when adapting
		//nCurLongest = 1; // the caller should ensure this, but doing it explicitly here is
						 // some extra insurance to keep matching within the only map that exists
						 // when the glossing KB is in use
	}
	else
	{
		pKB = pApp->m_pKB;
		nCurLongest = pKB->m_nMaxWords; // no matches are possible for phrases longer
										// than nCurLongest when adapting
	}
	CTargetUnit* pTargetUnit = (CTargetUnit*)NULL;
	int index = nCurLongest - 1;
	bool bFound = FALSE;
	while (index > -1)
	{
		bFound = pKB->FindMatchInKB(index + 1, phrases[index], pTargetUnit);
		if (bFound)
		{
			// matched a source phrase which has identical key as the built phrase
			break;
		}
		else
		{
			index--; // try next-smaller built phrase
		}
	}

	// if no match was found, we return immediately with a return value of FALSE
	if (!bFound)
	{
		pApp->pCurTargetUnit = (CTargetUnit*)NULL; // the global pointer must be cleared
		m_CurKey.Empty(); // global var m_CurKey not needed, so clear it
		return FALSE;
	}
    // whm 10Jan2018 Note: We do not call the ChooseTranslation dialog from LookAhead()
    // now that the choose translation feature is implemented in the CPhraseBox's dropdown
    // list. Hence, we should not set pTargetUnitFromChooseTrans here, since it should only be set
    // from the View's OnButtonChooseTranslation() handler.

    // whm 5Mar2018 Note: Originally on 10Jan2018 I commented out the next line as it was
    // primarily used to store the target unit supplied by the ChooseTranslation dialog,
    // however, we can extend its usefulness for my current refactoring of the dropdown
    // setup code in Layout's PlaceBox() function.
    // I've also renamed it from pTargetUnitFromChooseTrans back to its original name of 
    // pCurTargetUnit.
	pApp->pCurTargetUnit = pTargetUnit; // set global pointer so the dialog can use it if it 
                                        // is called


	m_CurKey = phrases[index]; // set the m_CurKey so the dialog can use it if it is called
							 // m_CurKey is a global variable (the lookup of phrases[index] is
							 // done on a copy, so m_CurKey retains the case of the key as in
							 // the document; but the lookup will have set global flags
							 // such as gbMatchedKB_UCentry to TRUE or FALSE as appropriate)
	m_nWordsInPhrase = index + 1; // static variable, needed for merging source phrases in
								// the caller; when glossing, index will == 0 so no merge will
								// be asked for below while in glossing mode

	// we found a target unit in the list with a matching m_key field, so we must now set
	// the static var translation to the appropriate adaptation, or gloss, text: this
	// will be the target unit's first entry in its list if the list has only one entry, else
	// we must present the user with a dialog to put up all possible adaptations (or glosses) for
	//  the user to choose one, or type a new one, or reject all - in which case we return
	// FALSE for manual typing of an adaptation (or gloss) etc. For autocapitalization support,
	// the dialog will show things as stored in the KB (if auto caps is ON, that could be with
	// all or most entries starting with lower case) and we let any needed changes to initial
	// upper case be done automatically after the user has chosen or typed.
	TranslationsList::Node *node = pTargetUnit->m_pTranslations->GetFirst();
	wxASSERT(node != NULL);
	//BEW 21Jun10, for kbVersion 2 we want a count of all non-deleted CRefString
	//instances, not the total number in the list, because there could be deletions stored
	//int count = pTargetUnit->m_pTranslations->GetCount();
	int count = pTargetUnit->CountNonDeletedRefStringInstances();
	// For kbVersion 2, a CTargetUnit can store only deleted CRefString instances, so that
	// means count can be zero - if this is the case, this is an 'unmatched' situation,
	// and should be handled the same as the if(!bFound) test above
	if (count == 0)
	{
		pApp->pCurTargetUnit = (CTargetUnit*)NULL; // the global pointer must be cleared
		m_CurKey.Empty(); // global var m_CurKey not needed, so clear it
		return FALSE;
	}
	wxASSERT(count > 0);
	if (count > 1 || pTargetUnit->m_bAlwaysAsk)
	{
		// move to new location, merge if necessary, so user has visual feedback about which
		// element(s) is involved
		pView->RemoveSelection();

		// set flag for telling us that we will have completed the move when the dialog is shown
		m_bCompletedMergeAndMove = TRUE;

		CPile* pPile = pView->GetPile(pApp->m_nActiveSequNum);
		wxASSERT(pPile);

		// create the appropriate selection there
		CCell* pAnchorCell = pPile->GetCell(0); // always cell [0] in the refactored design

		// we do the merger here in LookAhead() rather than in the caller, such as
		// MoveToNextPile(), so that the merger can be seen by the user at the time it is
		// done (and helpful to see) rather than later
		if (m_nWordsInPhrase > 0)
		{
			pApp->m_pAnchor = pAnchorCell;
			CCellList* pSelection = &pApp->m_selection;
			wxASSERT(pSelection->IsEmpty());
			pApp->m_selectionLine = 1;
			wxClientDC aDC(pApp->GetMainFrame()->canvas); // make a temporary device context

			// then do the new selection, start with the anchor cell
			wxColour oldBkColor = aDC.GetTextBackground();
			aDC.SetBackgroundMode(pApp->m_backgroundMode);
			aDC.SetTextBackground(wxColour(255,255,0)); // yellow
			wxColour textColor = pAnchorCell->GetColor();
			pAnchorCell->DrawCell(&aDC,textColor);
			pApp->m_bSelectByArrowKey = FALSE;
			pAnchorCell->SetSelected(TRUE);

			// preserve record of the selection
			pSelection->Append(pAnchorCell);

			// extend the selection to the right, if more than one pile is involved
			if (m_nWordsInPhrase > 1)
			{
				// extend the selection
				pView->ExtendSelectionForFind(pAnchorCell, m_nWordsInPhrase);
			}
		}

        // whm 5Mar2018 Note about original comment below. It is no longer possible to have
        // a situation in which the "user cancels the dialog" as there is no longer any
        // possibility of the ChooseTranslation dialog appearing during LookAhead(). Also
        // the phrase box should now be shown at it new location and there is no need for
        // "the old state will have to be restored...any merge needs to be unmerged".
        //
		// the following code added to support Bill Martin's wish that the phrase box be shown
		// at its new location when the dialog is open (if the user cancels the dialog, the old
		// state will have to be restored - that is, any merge needs to be unmerged)
		pApp->m_pTargetBox->m_bAbandonable = FALSE;
		// adaptation is available, so use it if the source phrase has only a single word, but
		// if it's multi-worded, we must first do a merge and recalc of the layout

		if (m_nWordsInPhrase > 1) // m_nWordsInPhrase is a global, set in this function
								// or in LookUpSrcWord()
		{
			// do the needed merge, etc.
			pApp->bLookAheadMerge = TRUE; // set static flag to ON
			bool bSaveFlag = m_bAbandonable; // the box is "this"
			pView->MergeWords(); // calls protected OnButtonMerge() in CAdapt_ItView class
			m_bAbandonable = bSaveFlag; // preserved the flag across the merge
			pApp->bLookAheadMerge = FALSE; // restore static flag to OFF
            m_bSuppressMergeInMoveToNextPile = TRUE; // restored 24Feb2018 after being accidentally removed
        }
		else
			pView->RemoveSelection(); // glossing, or adapting a single src word only

        // whm 27Feb2018 Note: The following block that sets this m_pTargetBox to be empty
        // and the App's m_targetPhrase to _T("") needs to be removed for phrasebox contents
        // to appear in the dropdown phrasebox.
        //
		// next code is taken from end of MoveToNextPile()
		// initialize the phrase box to be empty, so as not to confuse the user
		//if (GetHandle() != NULL) // This won't happen (a NULL handle) in wx version since we don't destroy the targetbox window
		//{
		//	// wx version note: we do the following elsewhere when we hide the m_pTargetBox
		//	ChangeValue(_T(""));
		//	pApp->m_targetPhrase = _T("");
		//}

		// recalculate the layout
#ifdef _NEW_LAYOUT
		pLayout->RecalcLayout(pApp->m_pSourcePhrases, keep_strips_keep_piles);
#else
		pLayout->RecalcLayout(pApp->m_pSourcePhrases, create_strips_keep_piles);
#endif

		// get the new active pile
		pApp->m_pActivePile = pView->GetPile(pApp->m_nActiveSequNum);
		wxASSERT(pApp->m_pActivePile != NULL);

		// scroll into view
		pApp->GetMainFrame()->canvas->ScrollIntoView(pApp->m_nActiveSequNum);

        // make what we've done visible
		pView->Invalidate();
		pLayout->PlaceBox();

        // [old comment below]
        // put up a dialog for user to choose translation from a list box, or type new one
		// (note: for auto capitalization; ChooseTranslation (which calls the CChoseTranslation
		// dialog handler, only sets the 'translation' global variable, it does not make any case
		// adjustments - these, if any, must be done in the caller)

        // whm note: The saveAutoInsert assignment below is now unnecessary.
        // since ChooseTranslation() is no longer called from LookAhead().
        // It is sufficient to just to set the App's m_bAutoInsert = FALSE.

        // [old comment below] 
        // wx version addition: In wx the OnIdle handler continues to run even when modal
        // dialogs are being shown, so we'll save the state of m_bAutoInsert before calling
        // ChooseTranslation change m_bAutoInsert to FALSE while the dialog is being shown,
        // then restore m_bAutoInsert's prior state after ChooseTranslation returns.
        // whm [old] update: I fixed the problem by using a derived AIModalDialog that turns off
        // idle processing while modal dialogs are being shown modal, but the following
        // doesn't hurt.

        // whm note: Unilaterally set the m_bAutoInsert flag to FALSE so that the movement of the phrasebox
        // will halt, giving the user opportunity to interact with the just-shown dropdown combobox.
        //bool saveAutoInsert = pApp->m_bAutoInsert;
        pApp->m_bAutoInsert = FALSE;

        // whm 10Jan2018 modified the code below by removing the call of the ChooseTranslation 
        // dialog from within this LookAhead() function.
        // The CPhraseBox is now derived from the wxOwnerDrawnComboBox and now it will 
        // always have the available translations in its dropdown list - ready to popup 
        // from within the new phrasebox at the point a PlaceBox() call is made.
        // LookAhead() will always end at some point with a call to PlaceBox(), which
        // will determine whether the phrasebox's built-in list should be shown to the user.
        // Hence, there is no longer any need to call the CPhraseBox::ChooseTranlsation()
        // function from here.
        // Note: At any time the phrasebox is at a location, the actual modal Choose Translation 
        // dialog can still be called manually via the usual toolbar button, or by using the F8 
        // or Ctrl+L hotkeys. Now that the phrasebox is derived from wxOwnerDrawnComboBox, all 
        // methods of summoning the ChooseTranslation dialog make use of the 
        // Adapt_ItView::ChooseTranslation() function, rather than the old 
        // CPhraseBox::ChooseTranslation() function of the same name (now removed from the codebase).

		// BEW changed 9Apr12 for support of discontinuous highlighting spans...
		// A new protocol is needed here. The legacy code wiped out any auto-insert
		// highlighting already accumulated, and setup for a possible auto-inserted span
		// starting at the NEXT pile following the merger location. The user-choice of the
		// adaptation or gloss rightly ends the previous auto-inserted span, but we want
		// that span to be visible while the ChooseTranslation dialog is open; so here we
		// do nothing, and in the caller (which usually is MoveToNextPile() we clobber the
		// old highlighted span and setup for a potential new one
#ifdef Highlighting_Bug
		// BEW changed 9Apr12 for support of discontinuous highlighting spans
		wxLogDebug(_T("PhraseBox::LookAhead(), hilited span ends at merger at: sequnum = %d  where the user chose:  %s  for source:  %s"),
			pApp->m_nActiveSequNum, translation, pApp->m_pActivePile->GetSrcPhrase()->m_srcPhrase);
#endif
	}
	else // count == 1 case (ie. a unique adaptation, or a unique gloss when glossing)
	{
        // BEW added 08Sep08 to suppress inserting placeholder translations for ... matches
        // when in glossing mode and within the end of a long retranslation
		if (m_CurKey == _T("..."))
		{
			// don't allow an ellipsis (ie. placeholder) to trigger an insertion,
			// leave translation empty
            m_Translation.Empty();
			return TRUE;
		}
		// BEW 21Jun10, we have to loop to find the first non-deleted CRefString instance,
		// because there may be deletions stored in the list
		CRefString* pRefStr = NULL;
		while (node != NULL)
		{
			pRefStr = node->GetData();
			node = node->GetNext();
			wxASSERT(pRefStr);
			if (!pRefStr->GetDeletedFlag())
			{
                m_Translation = pRefStr->m_translation;
				break;
			}
		}
	}

	// adjust for case, if necessary; this algorithm will not make a lower case string start
	// with upper case when the source is uppercase if the user types punctuation at the start
	// of the string. The latter is, however, unlikely - provided the auto punctuation support
	// is utilized by the user
	if (gbAutoCaps && gbSourceIsUpperCase)
	{
		bool bNoError = pApp->GetDocument()->SetCaseParameters(m_Translation,FALSE);
		if (bNoError && !gbNonSourceIsUpperCase && (gcharNonSrcUC != _T('\0')))
		{
			// make it upper case
            m_Translation.SetChar(0,gcharNonSrcUC);
		}
	}
	return TRUE;
}

// BEW 13Apr10, no changes needed for support of doc version 5
// BEW 8July10, no changes needed for support of kbVersion 2
void CPhraseBox::JumpForward(CAdapt_ItView* pView)
{
	#ifdef _FIND_DELAY
		wxLogDebug(_T("9. Start of JumpForward"));
	#endif
	// refactored 25Mar09
	CLayout* pLayout = GetLayout();
	CAdapt_ItApp* pApp = pLayout->m_pApp;
	if (pApp->m_bDrafting)
	{
        // get an adjusted pile pointer for the new active location, and we want the
        // pile's strip to be marked as invalid and the strip index added to the
        // CLayout::m_invalidStripArray
		pLayout->m_pDoc->ResetPartnerPileWidth(pApp->m_pActivePile->GetSrcPhrase());

		// check the mode, whether or not it is single step, and route accordingly
		if (pApp->m_bSingleStep)
		{
			int bSuccessful;
			if (pApp->m_bTransliterationMode && !gbIsGlossing)
			{
				bSuccessful = MoveToNextPile_InTransliterationMode(pApp->m_pActivePile);
			}
			else
			{
				bSuccessful = MoveToNextPile(pApp->m_pActivePile);
			}
			if (!bSuccessful)
			{
				// BEW added 4Sep08 in support of transitioning steps within vertical
				// edit mode
				if (gbVerticalEditInProgress && m_bTunnellingOut)
				{
                    // MoveToNextPile might fail within the editable span while vertical
                    // edit is in progress, so we have to allow such a failure to not cause
                    // tunnelling out; hence we use the m_bTunnellingOut global to assist -
                    // it is set TRUE only when
                    // VerticalEdit_CheckForEndRequiringTransition() in the view class
                    // returns TRUE, which means that a PostMessage(() has been done to
                    // initiate a step transition
					m_bTunnellingOut = FALSE; // caller has no need of it, so clear to
											 // default value
					pLayout->m_docEditOperationType = no_edit_op;
					return;
				}

                // if in vertical edit mode, the end of the doc is always an indication
                // that the edit span has been traversed and so we should force a step
                // transition, otherwise, continue to n:(tunnel out before m_nActiveSequNum
                // can be set to -1, which crashes the app at redraw of the box and recalc
                // of the layout)
				if (gbVerticalEditInProgress)
				{
					bool bCommandPosted = pView->VerticalEdit_CheckForEndRequiringTransition(
										-1, nextStep, TRUE); // bForceTransition is TRUE
					if (bCommandPosted)
					{
						// don't proceed further because the current vertical edit step
						// has ended
						pLayout->m_docEditOperationType = no_edit_op;
						return;
					}
				}

				// BEW changed 9Apr12, to support discontinuous highlighting
				// spans for auto-insertions...
				// When we come to the end, we could get there having done some
				// auto-insertions, and so we'd want them to be highlighted in the normal
				// way, and so we do nothing here -- the user can manually clear them, or
				// position the box elsewhere and initiate the lookup-and-insert loop from
				// there, in which case auto-inserts would kick off with new span(s)

				pLayout->Redraw(); // remove highlight before MessageBox call below
				pLayout->PlaceBox();

				// tell the user EOF has been reached...
				// BEW added 9Jun14, don't show this message when in clipboard adapt mode, because
				// it will come up every time a string of text is finished being adapted, and that
				// soon become a nuisance - having to click it away each time
				if (!pApp->m_bClipboardAdaptMode)
				{
					wxMessageBox(_(
"The end. Provided you have not missed anything earlier, there is nothing more to adapt in this file."),
					_T(""), wxICON_INFORMATION | wxOK);
				}
				wxStatusBar* pStatusBar;
				CMainFrame* pFrame = pApp->GetMainFrame();
				if (pFrame != NULL)
				{
					pStatusBar = pFrame->GetStatusBar();
					wxString str = _("End of the file; nothing more to adapt.");
					pStatusBar->SetStatusText(str,0); // use first field 0
				}
				// we are at EOF, so set up safe end conditions
				pApp->m_targetPhrase.Empty();
				pApp->m_nActiveSequNum = -1;
				pApp->m_pTargetBox->Hide(); // MFC version calls DestroyWindow()
				pApp->m_pTargetBox->ChangeValue(_T("")); // need to set it to null str
													  // since it won't get recreated
				pApp->m_pActivePile = NULL; // can use this as a flag for at-EOF condition too

				pView->Invalidate();
				pLayout->PlaceBox();

                m_Translation.Empty(); // clear the static string storage for the
                    // translation save the phrase box's text, in case user hits SHIFT+END
                    // to unmerge a phrase
                m_SaveTargetPhrase = pApp->m_targetPhrase;
				pLayout->m_docEditOperationType = no_edit_op;

			    RestorePhraseBoxAtDocEndSafely(pApp, pView);  // BEW added 8Sep14
				return; // must be at EOF;
			} // end of block for !bSuccessful

			// for a successful move, a scroll into view is often needed
			pLayout->m_pCanvas->ScrollIntoView(pApp->m_nActiveSequNum);

			// get an adjusted pile pointer for the new active location, and we want the
			// pile's strip to be marked as invalid and the strip index added to the
			// CLayout::m_invalidStripArray
			pApp->m_pActivePile = pView->GetPile(pApp->m_nActiveSequNum);
			CSourcePhrase* pSPhr = pApp->m_pActivePile->GetSrcPhrase();
			pLayout->m_pDoc->ResetPartnerPileWidth(pSPhr);

			// try to keep the phrase box from coming too close to the bottom of
			// the client area if possible
			int yDist = pLayout->GetStripHeight() + pLayout->GetCurLeading();
			wxPoint scrollPos;
			int xPixelsPerUnit,yPixelsPerUnit;
			pLayout->m_pCanvas->GetScrollPixelsPerUnit(&xPixelsPerUnit,&yPixelsPerUnit);

            // MFC's GetScrollPosition() "gets the location in the document to which the
            // upper left corner of the view has been scrolled. It returns values in
            // logical units." wx note: The wx docs only say of GetScrollPos(), that it
            // "Returns the built-in scrollbar position." I assume this means it gets the
            // logical position of the upper left corner, but it is in scroll units which
            // need to be converted to device (pixel) units

			wxSize maxDocSize; // renamed barSizes to maxDocSize for clarity
			pLayout->m_pCanvas->GetVirtualSize(&maxDocSize.x,&maxDocSize.y);
															// gets size in pixels

			pLayout->m_pCanvas->CalcUnscrolledPosition(0,0,&scrollPos.x,&scrollPos.y);
			// the scrollPos point is now in logical pixels from the start of the doc

			wxRect rectClient(0,0,0,0);
			wxSize canvasSize;
			canvasSize = pApp->GetMainFrame()->GetCanvasClientSize(); // with
								// GetClientRect upper left coord is always (0,0)
			rectClient.width = canvasSize.x;
			rectClient.height = canvasSize.y;


			if (rectClient.GetBottom() >= 4 * yDist) // do the adjust only if at least 4
													 // strips are showing
			{
				wxPoint pt = pApp->m_pActivePile->GetCell(1)->GetTopLeft(); // logical coords
																		// of top of phrase box

				// if there are not two full strips below the top of the phrase box, scroll down
				wxASSERT(scrollPos.y + rectClient.GetBottom() >= 2*yDist);
				if (pt.y > scrollPos.y + rectClient.GetBottom() - 2*yDist)
				{
					int logicalViewBottom = scrollPos.y + rectClient.GetBottom();
																	// is always 0
					if (logicalViewBottom < maxDocSize.GetHeight())
					{
						if (logicalViewBottom <= maxDocSize.GetHeight() - yDist)
						{
							// a full strip + leading can be scrolled safely
							pApp->GetMainFrame()->canvas->ScrollDown(1);
						}
						else
						{
							// we are close to the end, but not a full strip +
							// leading can be scrolled, so just scroll enough to
							// reach the end - otherwise position of phrase box will
							// be set wrongly
							wxASSERT(maxDocSize.GetHeight() >= logicalViewBottom);
							yDist = maxDocSize.GetHeight() - logicalViewBottom;
							scrollPos.y += yDist;

							int posn = scrollPos.y;
							posn = posn / yPixelsPerUnit;
                            // Note: MFC's ScrollWindow's 2 params specify the xAmount and
                            // yAmount to scroll in device units (pixels). The equivalent
                            // in wx is Scroll(x,y) in which x and y are in SCROLL UNITS
                            // (pixels divided by pixels per unit). Also MFC's ScrollWindow
                            // takes parameters whose value represents an "amount" to
                            // scroll from the current position, whereas the
                            // wxScrolledWindow::Scroll takes parameters which represent an
                            // absolute "position" in scroll units. To convert the amount
                            // we need to add the amount to (or subtract from if negative)
                            // the logical pixel unit of the upper left point of the client
                            // viewing area; then convert to scroll units in Scroll().
							pLayout->m_pCanvas->Scroll(0,posn);
							//Refresh(); // BEW 25Mar09, this refreshes wxWindow, that is,
							//the phrase box control - but I think we need a layout draw
							//here - so I've called view's Invalidate() at a higher level
							//- see below
						}
					}
				}
			} // end of test for more than or equal to 4 strips showing

			// refresh the view
			pLayout->m_docEditOperationType = relocate_box_op;
			pView->Invalidate(); // BEW added 25Mar09, see comment about Refresh 10 lines above
			pLayout->PlaceBox();

		} // end of block for test for m_bSingleStep == TRUE
		else // auto-inserting -- sets flags and returns, allowing the idle handler to call OnePass()
		{
			// cause auto-inserting using the OnIdle handler to commence
			pApp->m_bAutoInsert = TRUE;

			// User has pressed the Enter key  (OnChar() calls JumpForward())
			// BEW changed 9Apr12, to support discontinuous highlighting
			// spans for auto-insertions...
			// Since OnIdle() will call OnePass() and the latter will call
			// MoveToNextPile(), and it is in MoveToNextPile() that CCell's
			// m_bAutoInserted flag can get set TRUE, we only here need to ensure that the
			// current location is a kick-off one, which we can do by clearing any earlier
			// highlighting currently in effect
			pLayout->ClearAutoInsertionsHighlighting();
#ifdef Highlighting_Bug
			wxLogDebug(_T("\nPhraseBox::JumpForward(), kickoff, from  pile at sequnum = %d   SrcText:  "),
				pSPhr->m_nSequNumber, pSPhr->m_srcPhrase);
#endif

			pLayout->m_docEditOperationType = relocate_box_op;
		}
		// save the phrase box's text, in case user hits SHIFT+End to unmerge a
		// phrase
        m_SaveTargetPhrase = pApp->m_targetPhrase;
	} // end if for m_bDrafting
	else // we are in review mode
	{
		// we are in Review mode, so moves by the RETURN key can only be to
		// immediate next pile
		//int nOldStripIndex = pApp->m_pActivePile->m_pStrip->m_nStripIndex;

        // get an adjusted pile pointer for the new active location, and we want the
        // pile's strip to be marked as invalid and the strip index added to the
        // CLayout::m_invalidStripArray
        pApp->m_nActiveSequNum = pApp->m_pActivePile->GetSrcPhrase()->m_nSequNumber;
		pLayout->m_pDoc->ResetPartnerPileWidth(pApp->m_pActivePile->GetSrcPhrase());

        // if vertical editing is on, don't do the move to the next pile if it lies in the
        // gray area or is at the bundle end; so just check for the sequence number going
        // beyond the edit span bound & transition the step if it has, asking the user what
        // step to do next
		if (gbVerticalEditInProgress)
		{
            //m_bTunnellingOut = FALSE; not needed here as once we are in caller, we've
            //tunnelled out
            //bForceTransition is FALSE in the next call
			int nNextSequNum = pApp->m_pActivePile->GetSrcPhrase()->m_nSequNumber + 1;
			bool bCommandPosted = pView->VerticalEdit_CheckForEndRequiringTransition(
																nNextSequNum,nextStep);
			if (bCommandPosted)
			{
				// don't proceed further because the current vertical edit step has ended

                // NOTE: since step transitions call mode changing handlers, and because
                // those handlers typically do a store of the phrase box contents to the kb
                // if appropriate, we'll rely on it here and not do a store
				//m_bTunnellingOut = TRUE;
				pLayout->m_docEditOperationType = no_edit_op;
				return;
			}
			else
			{
				// BEW 19Oct15 No transition of vert edit modes,
				// so we can store this location on the app
				gpApp->m_vertEdit_LastActiveSequNum = pApp->m_nActiveSequNum;
#if defined(_DEBUG)
				wxLogDebug(_T("VertEdit PhrBox, JumpForward() storing loc'n: %d "), pApp->m_nActiveSequNum);
#endif
			}
		}
		if (pApp->m_bTransliterationMode)
		{
			::wxBell();
			wxMessageBox(_(
"Sorry, transliteration mode is not supported in review mode. Turn review mode off."),
			_T(""), wxICON_INFORMATION | wxOK);
			pLayout->m_docEditOperationType = no_edit_op;
			return;
		}

		// Note: transliterate mode is not supported in Review mode, so there is no
		// function such as MoveToImmedNextPile_InTransliterationMode()
		int bSuccessful = MoveToImmedNextPile(pApp->m_pActivePile);
		if (!bSuccessful)
		{
			CPile* pFwdPile = pView->GetNextPile(pApp->m_pActivePile); // old
                                // active pile pointer (should still be valid)
			//if ((!gbIsGlossing && pFwdPile->GetSrcPhrase()->m_bRetranslation) || pFwdPile == NULL)
			if (pFwdPile == NULL)
			{
				// tell the user EOF has been reached...
				// BEW added 9Jun14, don't show this message when in clipboard adapt mode, because
				// it will come up every time a string of text is finished being adapted, and that
				// soon become a nuisance - having to click it away each time
				if (!pApp->m_bClipboardAdaptMode)
				{
					wxMessageBox(_(
"The end. Provided you have not missed anything earlier, there is nothing more to adapt in this file."),
					_T(""), wxICON_INFORMATION | wxOK);
				}
				wxStatusBar* pStatusBar;
				CMainFrame* pFrame = pApp->GetMainFrame();
				if (pFrame != NULL)
				{
					pStatusBar = pFrame->GetStatusBar();
					wxString str = _("End of the file; nothing more to adapt.");
					pStatusBar->SetStatusText(str,0); // use first field 0
				}
				// we are at EOF, so set up safe end conditions
				pApp->m_pTargetBox->Hide(); // whm added 12Sep04
				pApp->m_pTargetBox->ChangeValue(_T("")); // need to set it to null
											// str since it won't get recreated
				pApp->m_pTargetBox->Enable(FALSE); // whm added 12Sep04
				pApp->m_targetPhrase.Empty();
				pApp->m_nActiveSequNum = -1;
				pApp->m_pActivePile = NULL; // can use this as a flag for
											// at-EOF condition too
				// recalc the layout without any gap for the phrase box, as it's hidden
#ifdef _NEW_LAYOUT
				pLayout->RecalcLayout(pApp->m_pSourcePhrases, keep_strips_keep_piles);
#else
				pLayout->RecalcLayout(pApp->m_pSourcePhrases, create_strips_keep_piles);
#endif
			    RestorePhraseBoxAtDocEndSafely(pApp, pView); // BEW added 8Sep14
			}
			else // pFwdPile is valid, so must have bumped against a retranslation
			{
				wxMessageBox(_(
"Sorry, the next pile cannot be a valid active location, so no move forward was done."),
				_T(""), wxICON_INFORMATION | wxOK);
#ifdef _NEW_LAYOUT
				pLayout->RecalcLayout(pApp->m_pSourcePhrases, keep_strips_keep_piles);
#else
				pLayout->RecalcLayout(pApp->m_pSourcePhrases, create_strips_keep_piles);
#endif
				pApp->m_pActivePile = pView->GetPile(pApp->m_nActiveSequNum);
				pApp->m_pTargetBox->SetFocus();

			}
            m_Translation.Empty(); // clear the static string storage for the translation
			// save the phrase box's text, in case user hits SHIFT+END to unmerge a phrase
            m_SaveTargetPhrase = pApp->m_targetPhrase;

			// get the view redrawn
			pLayout->m_docEditOperationType = no_edit_op;
			pView->Invalidate();
			pLayout->PlaceBox();
			return;
		} // end of block for test !bSuccessful
		else
		{
			// it was successful
			CCell* pCell = pApp->m_pActivePile->GetCell(1); // the cell
											// where the phraseBox is to be

            //pView->ReDoPhraseBox(pCell); // like PlacePhraseBox, but calculations based
            //on m_targetPhrase BEW commented out above call 19Dec07, in favour of
            //RemakePhraseBox() call below. Also added test for whether document at new
            //active location has a hole there or not; if it has, we won't permit a copy of
            //the source text to fill the hole, as that would be inappropriate in Reviewing
            //mode; since m_targetPhrase already has the box text or the copied source
            //text, we must instead check the CSourcePhrase instance explicitly to see if
            //m_adaption is empty, and if so, then we force the phrase box to remain empty
            //by clearing m_targetPhrase (later, when the box is moved to the next
            //location, we must check again in MakeTargetStringIncludingPunctuation() and
            //restore the earlier state when the phrase box is moved on)

			//CSourcePhrase* pSPhr = pCell->m_pPile->m_pSrcPhrase;
			CSourcePhrase* pSPhr = pCell->GetPile()->GetSrcPhrase();
			wxASSERT(pSPhr != NULL);

			// get an adjusted pile pointer for the new active location, and we want the
			// pile's strip to be marked as invalid and the strip index added to the
			// CLayout::m_invalidStripArray
			pApp->m_nActiveSequNum = pApp->m_pActivePile->GetSrcPhrase()->m_nSequNumber;
			pLayout->m_pDoc->ResetPartnerPileWidth(pSPhr);
			
			// BEW 19Oct15 No transition of vert edit modes,
			// so we can store this location on the app, provided
			// we are in bounds
			if (gbVerticalEditInProgress)
			{
				if (gEditRecord.nAdaptationStep_StartingSequNum <= pApp->m_nActiveSequNum &&
					gEditRecord.nAdaptationStep_EndingSequNum >= pApp->m_nActiveSequNum)
				{
					// BEW 19Oct15, store new active loc'n on app
					gpApp->m_vertEdit_LastActiveSequNum = pApp->m_nActiveSequNum;
#if defined(_DEBUG)
				wxLogDebug(_T("VertEdit PhrBox, JumpForward() storing loc'n: %d "), pApp->m_nActiveSequNum);
#endif
				}
			}
			
			if (pSPhr->m_targetStr.IsEmpty() || pSPhr->m_adaption.IsEmpty())
			{
				// no text or punctuation, or no text and punctuation not yet placed,
				// or no text and punctuation was earlier placed -- whichever is the case
				// we need to preserve that state
				pApp->m_targetPhrase.Empty();
				m_bSavedTargetStringWithPunctInReviewingMode = TRUE; // it gets cleared again
						// within MakeTargetStringIncludingPunctuation() at the end the block
						// it is being used in there
				m_StrSavedTargetStringWithPunctInReviewingMode = pSPhr->m_targetStr; // cleared
						// again within MakeTargetStringIncludingPunctuation() at the end of
						// the block it is being used in there
			}
			// if neither test succeeds, then let
			// m_targetPhrase contents stand unchanged

			pLayout->m_docEditOperationType = relocate_box_op;
		} // end of block for bSuccessful == TRUE

		// save the phrase box's text, in case user hits SHIFT+END to unmerge a phrase
        m_SaveTargetPhrase = pApp->m_targetPhrase;

#ifdef _NEW_LAYOUT
		#ifdef _FIND_DELAY
			wxLogDebug(_T("10. Start of RecalcLayout in JumpForward"));
		#endif
		pLayout->RecalcLayout(pApp->m_pSourcePhrases, keep_strips_keep_piles);
		#ifdef _FIND_DELAY
			wxLogDebug(_T("11. End of RecalcLayout in JumpForward"));
		#endif
#else
		pLayout->RecalcLayout(pApp->m_pSourcePhrases, create_strips_keep_piles);
#endif
		pApp->m_pActivePile = pView->GetPile(pApp->m_nActiveSequNum);
		wxASSERT(pApp->m_pActivePile != NULL);

		pLayout->m_pCanvas->ScrollIntoView(pApp->m_nActiveSequNum);

		pView->Invalidate();
		pLayout->PlaceBox();
	} // end Review mode (single src phrase move) block
	#ifdef _FIND_DELAY
		wxLogDebug(_T("12. End of JumpForward"));
	#endif
}

// internally calls CPhraseBox::FixBox() to see if a resizing is needed; this function
// is called for every character typed in phrase box (via OnChar() function which is
// called for every EVT_TEXT posted to event queue); it is not called for selector = 2
// value because OnChar() has a case statement that handles box contraction there, and for
// selector = 1 this function is only called elsewhere in the app within OnEditUndo()
// BEW 13Apr10, no changes needed for support of doc version 5
void CPhraseBox::OnPhraseBoxChanged(wxCommandEvent& WXUNUSED(event))
{
	// whm Note: This phrasebox handler is necessary in the wxWidgets version because the
	// OnChar() handler does not have access to the changed value of the new string
	// within the control reflecting the keystroke that triggers OnChar(). Because
	// of that difference in behavior, I moved the code dependent on updating
	// pApp->m_targetPhrase from OnChar() to this OnPhraseBoxChanged() handler.
    CAdapt_ItApp* pApp = (CAdapt_ItApp*)&wxGetApp();
    if (this->GetTextCtrl()->IsModified()) // whm 14Feb2018 added GetTextCtrl()-> for IsModified()
	{
		CAdapt_ItView* pView = (CAdapt_ItView*) pApp->GetView();
		// preserve cursor location, in case we merge, so we can restore it afterwards
		long nStartChar;
		long nEndChar;
        GetTextCtrl()->GetSelection(&nStartChar,&nEndChar);

		wxPoint ptNew;
		wxRect rectClient;
		wxSize textExtent;
		wxString thePhrase; // moved to OnPhraseBoxChanged()

		// update status bar with project name
		pApp->RefreshStatusBarInfo();

		// restore the cursor position...
        // BEW added 05Oct06; to support the alternative algorithm for setting up the
        // phrase box text, and positioning the cursor, when the selection was done
        // leftwards from the active location... that is, since the cursor position for
        // leftwards selection merges is determined within OnButtonMerge() then we don't
        // here want to let the values stored at the start of OnChar() clobber what
        // OnButtonMerge() has already done - so we have a test to determine when to
        // suppress the cursor setting call below in this new circumstance
		if (!(pApp->m_bMergeSucceeded && pApp->m_curDirection == toleft))
		{
			SetSelection(nStartChar,nEndChar);
			pApp->m_nStartChar = nStartChar;
			pApp->m_nEndChar = nEndChar;
		}

        // whm Note: Because of differences in the handling of events, in wxWidgets the
        // GetValue() call below retrieves the contents of the phrasebox after the
        // keystroke and so it includes the keyed character. OnChar() is processed before
        // OnPhraseBoxChanged(), and in that handler the key typed is not accessible.
        // Getting it here, therefore, is the only way to get it after the character has
        // been added to the box. This is in contrast to the MFC version where
        // GetWindowText(thePhrase) at the same code location in PhraseBox::OnChar there
        // gets the contents of the phrasebox including the just typed character.
		thePhrase = GetValue(); // current box text (including the character just typed)

		// BEW 6Jul09, try moving the auto-caps code from OnIdle() to here
		if (gbAutoCaps && pApp->m_pActivePile != NULL)
		{
			wxString str;
			CSourcePhrase* pSrcPhrase = pApp->m_pActivePile->GetSrcPhrase();
			wxASSERT(pSrcPhrase != NULL);
			bool bNoError = pApp->GetDocument()->SetCaseParameters(pSrcPhrase->m_key);
			if (bNoError && gbSourceIsUpperCase)
			{
				// a change of case might be called for... first
				// make sure the box exists and is visible before proceeding further
				if (pApp->m_pTargetBox != NULL && (pApp->m_pTargetBox->IsShown()))
				{
					// get the string currently in the phrasebox
					//str = pApp->m_pTargetBox->GetValue();
					str = thePhrase;

					// do the case adjustment only after the first character has been
					// typed, and be sure to replace the cursor afterwards
					int len = str.Length();
					if (len != 1)
					{
						; // don't do anything to first character if string has more than 1 char
					}
					else
					{
						// set cursor offsets
						int nStart = 1; int nEnd = 1;

						// check out its case status
						bNoError = pApp->GetDocument()->SetCaseParameters(str,FALSE); // FALSE is value for bIsSrcText

						// change to upper case if required
						if (bNoError && !gbNonSourceIsUpperCase && (gcharNonSrcUC != _T('\0')))
						{
							str.SetChar(0,gcharNonSrcUC);
							pApp->m_pTargetBox->ChangeValue(str);
							pApp->m_pTargetBox->Refresh();
							//pApp->m_targetPhrase = str;
							thePhrase = str;

							// fix the cursor location
							pApp->m_pTargetBox->SetSelection(nStart,nEnd);
						}
					}
				}
			}
		}

		// update m_targetPhrase to agree with what has been typed so far
		pApp->m_targetPhrase = thePhrase;

		bool bWasMadeDirty = FALSE;

		// whm Note: here we can eliminate the test for Return, BackSpace and Tab
		pApp->m_bUserTypedSomething = TRUE;
		pView->GetDocument()->Modify(TRUE);
		pApp->m_pTargetBox->m_bAbandonable = FALSE; // once we type something, it's not
												// considered abandonable
		m_bBoxTextByCopyOnly = FALSE; // even if copied, typing something makes it different so set
							// this flag FALSE
        this->GetTextCtrl()->MarkDirty(); // whm 14Feb2018 added this->GetTextCtrl()->

		// adjust box size
		FixBox(pView, thePhrase, bWasMadeDirty, textExtent, 0); // selector = 0 for incrementing box extent

		// set the globals for the cursor location, ie. m_nStartChar & m_nEndChar,
		// ready for box display
        GetTextCtrl()->GetSelection(&pApp->m_nStartChar, &pApp->m_nEndChar);

		// save the phrase box's text, in case user hits SHIFT+END to unmerge a phrase
        m_SaveTargetPhrase = pApp->m_targetPhrase;

		// scroll into view if necessary
		GetLayout()->m_pCanvas->ScrollIntoView(pApp->m_nActiveSequNum);

		GetLayout()->m_docEditOperationType = no_edit_op;

		// BEW 02May2016, thePhrase gets leaked, so clear it here
		thePhrase.clear();
	}
}

// FixBox() is the core function for supporting box expansion and contraction in various
// situations, especially when typing into the box; this version detects when adjustment to
// the layout is required, it calls CLayout::RecalcLayout() to tweak the strips at the
// active location - that is with input parameter keep_strips_keep_piles. FixBox() is
// called in the following CPhraseBox functions: OnPhraseBoxChanged() with
// selector 0 passed in; OnChar() for backspace keypress, with selector 2 passed in;
// OnEditUndo() with selector 1 passed in, and (BEW added 14Mar11) OnDraw() of
// CAdapt_ItView.
// refactored 26Mar09
// BEW 13Apr10, no changes needed for support of doc version 5
// BEW 22Jun10, no changes needed for support of kbVersion 2
// Called from the View's OnDraw() 2X, CPhraseBox's OnEditUndo(), OnChar(), and OnPhraseBoxChanged()
void CPhraseBox::FixBox(CAdapt_ItView* pView, wxString& thePhrase, bool bWasMadeDirty,
						wxSize& textExtent,int nSelector)
{
    // destroys the phrase box and recreates it with a different size, depending on the
    // nSelector value.
    // nSelector == 0, increment the box width using its earlier value
    // nSelector == 1, recalculate the box width using the input string extent
    // nSelector == 2, backspace was typed, so box may be contracting
    // This new version tries to be smarter for deleting text from the box, so that we
    // don't recalculate the layout for the whole screen on each press of the backspace key
    // - to reduce the blinking effect
    // version 2.0 takes text extent of m_gloss into consideration, if gbIsGlossing is TRUE

	// Note: the refactored design of this function is greatly simplified by the fact that
	// in the new layout system, the TopLeft location for the resized phrase box is
	// obtained directly from the layout after the layout has been recalculated. To
	// recalculate the layout correctly, all the new FixBox() needs to do is:
	// (1) work out if a RecalcLayout(), or other strip tweaking call, is needed, and
	// (2) prior to making the RecalcLayout() call, or other strip tweaking call, the new
	// phrase box width must be calculated and stored in CLayout::m_curBoxWidth so that
	// RecalcLayout() will have access to it when it is setting the width of the active pile.

	//GDLC Added 2010-02-09
	enum phraseBoxWidthAdjustMode nPhraseBoxWidthAdjustMode = steadyAsSheGoes;

	CAdapt_ItApp* pApp = &wxGetApp();
	wxASSERT(pApp != NULL);
	//bool bWholeScreenUpdate = TRUE; // suppress for now, we'll probably do it
	//differently (such as with a clipping rectangle, and may not calculate that here anyway)
	CLayout* pLayout = GetLayout();

	// ensure the active pile is up to date
	pApp->m_pActivePile = pLayout->m_pView->GetPile(pApp->m_nActiveSequNum);
	wxASSERT(pApp->m_pActivePile);

	//wxSize currBoxSize(pApp->m_curBoxWidth,pApp->m_nTgtHeight);
	wxSize sizePhraseBox = GetClientSize(); // BEW added 25Mar09; it's in pixels
	wxSize currBoxSize(pLayout->m_curBoxWidth, sizePhraseBox.y); // note, this is better, because if
						// glossing mode is on and glossing uses the navText font, the height
						// might be different from the height of the target text font

	wxClientDC dC(this);
	wxFont* pFont;
	if (gbIsGlossing && gbGlossingUsesNavFont)
		pFont = pApp->m_pNavTextFont;
	else
		pFont = pApp->m_pTargetFont;
	wxFont SaveFont = dC.GetFont();

	dC.SetFont(*pFont);
	dC.GetTextExtent(thePhrase, &textExtent.x, &textExtent.y); // measure using the current font
	wxSize textExtent2;
	wxSize textExtent3;
	wxSize textExtent4; // version 2.0 and onwards, for extent of m_gloss
	//CStrip* pPrevStrip;

	// if close to the right boundary, then recreate the box larger (by 3 chars of width 'w')
	wxString aChar = _T('w');
	wxSize charSize;
	dC.GetTextExtent(aChar, &charSize.x, &charSize.y);
	bool bResult;
	if (nSelector < 2)
	{
		// when either incrementing the box width from what it was, or recalculating a
		// width using m_targetPhrase contents, generate TRUE if the horizontal extent of
		// the text in it is greater or equal to the current box width less 3 'w' widths
		// (if so, an expansion is required, if not, current size can stand)
		bResult = textExtent.x >= currBoxSize.x - pApp->m_nNearEndFactor*charSize.x;
	}
	else
	{
        // when potentially about to contract the box, generate TRUE if the horizontal
        // extent of the text in it is less than or equal to the current box width less 4
        // 'w' widths (if so, a contraction is required, if not, current size can stand)
		// BEW changed 25Jun05, the above criterion produced very frequent resizing; let's
		// do it far less often...
		//bResult = textExtent.x <= currBoxSize.x - (4*charSize.x); // the too-often way
		bResult = textExtent.x < currBoxSize.x - (8*charSize.x);
	}
	bool bUpdateOfLayoutNeeded = FALSE;
	if (bResult )
	{
		// a width change is required....therefore set m_curBoxWidth and call RecalcLayout()
		if (nSelector < 2)
			nPhraseBoxWidthAdjustMode = expanding; // this is passed on to the functions that
								// calculate the new width of the phrase box

		// make sure the activeSequNum is set correctly, we need it to be able
		// to restore the pActivePile pointer after the layout is recalculated
		//
		// BEW removed 26Mar09 because the m_nActiveSequNum variable is to be trusted over
		// the m_pActivePile - just in case our refactored code forgets to set the latter
		// at some point; but our code won't work if the former is ever wrong - we'd see
		// the muck up in the display of the layout immediately!
		//pApp->m_nActiveSequNum = pApp->m_pActivePile->GetSrcPhrase()->m_nSequNumber;

		// calculate the new width
		if (nSelector == 0)
		{
			pLayout->m_curBoxWidth += pApp->m_nExpandBox*charSize.x;

            GetTextCtrl()->GetSelection(&pApp->m_nStartChar,&pApp->m_nEndChar); // store current phrase
					// box selection in app's m_nStartChar & m_nEndChar members

			bUpdateOfLayoutNeeded = TRUE;
		}
		else // next block is for nSelector == 1 or 2 cases
		{
			if (nSelector == 2)
			{
				// backspace was typed, box may be about to contract

				pApp->m_targetPhrase = GetValue(); // store current typed string

				//move old code into here & then modify it
                GetTextCtrl()->GetSelection(&pApp->m_nStartChar,&pApp->m_nEndChar); // store current selection

				// we are trying to delete text in the phrase box by pressing backspace key
				// shrink the box by 2 'w' widths if the space at end is >= 4 'w' widths
				// BEW changed 25Jun09, to have the box shrink done less often to reduce blinking,
				// the new criterion will shrink the box by 7 'w' widths -- no make it
				// just 5 w widths (26Jun09)
				//int newWidth = pLayout->m_curBoxWidth - 2 * charSize.x;
				//int newWidth = pLayout->m_curBoxWidth - 7 * charSize.x;
				int newWidth = pLayout->m_curBoxWidth - 5 * charSize.x;
				// we have to compare with a reasonable box width based on source text
				// width to ensure we don't reduce the width below that (otherwise piles
				// to the right will encroach over the end of the active location's source
				// text)
				wxString srcPhrase = pApp->m_pActivePile->GetSrcPhrase()->m_srcPhrase;
				wxFont* pSrcFont = pApp->m_pSourceFont;
				wxSize sourceExtent;
				dC.SetFont(*pSrcFont);
				dC.GetTextExtent(srcPhrase, &sourceExtent.x, &sourceExtent.y);
				int minWidth = sourceExtent.x + pApp->m_nExpandBox*charSize.x;
				if (newWidth <= minWidth)
				{
					// we are contracting too much, so set to minWidth instead
					pLayout->m_curBoxWidth = minWidth;
					//GDLC 2010-02-09
					nPhraseBoxWidthAdjustMode = steadyAsSheGoes;
				}
				else
				{
					// newWidth is larger than minWidth, so we can do the full contraction
					pLayout->m_curBoxWidth = newWidth;
					//GDLC 2010-02-09
					nPhraseBoxWidthAdjustMode = contracting;
				}
				//GDLC I think that the normal SetPhraseBoxGapWidth() should be called with
				// nPhraseBoxWidthAdjustmentModepassed to it as a parameter instead of simply
				// using newWidth.
				pApp->m_pActivePile->SetPhraseBoxGapWidth(newWidth); // sets m_nWidth to newWidth
                // The gbContracting flag used above? RecalcLayout() will override
                // m_curBoxWidth if we leave this flag FALSE; setting it makes the
                // ResetPartnerPileWidth() call within RecalcLayout() not do an active pile
                // gap width calculation that otherwise sets the box width too wide and the
                // backspaces then done contract the width of the phrase box not as much as
                // expected (the RecalcLayout() call clears gbContracting after using it)
				bUpdateOfLayoutNeeded = TRUE;
			} // end block for nSelector == 2 case
			else
			{
				// nSelector == 1 case
				pLayout->m_curBoxWidth = textExtent.x + pApp->m_nExpandBox*charSize.x;

				// move the old code into here
                GetTextCtrl()->GetSelection(&pApp->m_nStartChar,&pApp->m_nEndChar); // store current selection

				bUpdateOfLayoutNeeded = TRUE;
			} // end block for nSelector == 1 case
		} // end nSelector != 0 block

		if (bUpdateOfLayoutNeeded)
		{
#ifdef _NEW_LAYOUT
			pLayout->RecalcLayout(pApp->m_pSourcePhrases, keep_strips_keep_piles, nPhraseBoxWidthAdjustMode);
#else
			pLayout->RecalcLayout(pApp->m_pSourcePhrases, create_strips_keep_piles);
#endif
			pApp->m_pActivePile = pView->GetPile(pApp->m_nActiveSequNum);
			wxASSERT(pApp->m_pActivePile != NULL);
		}

		// resize using the stored information (version 1.2.3 and earlier used to recreate, but
		// this conflicted with Keyman (which sends backspace sequences to delete matched char
		// sequences to be converted, so I changed to a resize for version 1.2.4 and onwards.
		// Everything seems fine.
		// wx version: In the MFC version there was a CreateBox function as well as a ResizeBox
		// function used here. I have simplified the code to use ResizeBox everywhere, since
		// the legacy CreateBox now no longer recreates the phrasebox each time it's called.

		wxPoint ptCurBoxLocation;
		CCell* pActiveCell = pApp->m_pActivePile->GetCell(1);
		pActiveCell->TopLeft(ptCurBoxLocation); // returns the .x and .y values in the signature's ref variable
		// BEW 25Dec14, cells are 2 pixels larger vertically as of today, so move TopLeft of box
		// back up by 2 pixels, so text baseline keeps aligned
		ptCurBoxLocation.y -= 2;

        // whm 10Jan2018 Note: Since CPhraseBox is now derived from the wxOwnerDrawnComboBox, 
        // its size and resizing properties are inherent in what already exists within the 
        // CPhraseBox class, and nothing need be done from here in FixBox().

		if (gbIsGlossing && gbGlossingUsesNavFont)
		{
			pView->ResizeBox(&ptCurBoxLocation, pLayout->m_curBoxWidth, pLayout->GetNavTextHeight(),
				pApp->m_targetPhrase, pApp->m_nStartChar, pApp->m_nEndChar, pApp->m_pActivePile);
		}
		else
		{
			pView->ResizeBox(&ptCurBoxLocation, pLayout->m_curBoxWidth, pLayout->GetTgtTextHeight(),
				pApp->m_targetPhrase, pApp->m_nStartChar, pApp->m_nEndChar, pApp->m_pActivePile);
		}
		if (bWasMadeDirty)
			pApp->m_pTargetBox->GetTextCtrl()->MarkDirty(); // TRUE (restore modified status) // whm 14Feb2018 added GetTextCtrl()->

//#ifdef Do_Clipping
//		// support clipping
//		if (!bUpdateOfLayoutNeeded)
//			pLayout->SetAllowClippingFlag(TRUE); // flag is turned off again at end of Draw()
//#endif
	} // end bResult == TRUE block
	else
	{
//#ifdef Do_Clipping
//		// no reason to change box size, so we should be able to support clipping
//		// (provided no scroll is happening - but that is deal with elsewhere, search for
//		// SetScrollingFlag() to find where)
//		pLayout->SetAllowClippingFlag(TRUE); // flag is turned off again at end of Draw()
//#endif
	}
    if (nSelector < 2)
    {
        pApp->m_targetPhrase = thePhrase; // update the string storage on the view
            // (do it here rather than before the resizing code else selection bounds are wrong)
    }

	dC.SetFont(SaveFont); // restore old font (ie "System")
    m_SaveTargetPhrase = pApp->m_targetPhrase;

	// whm Note re BEW's note below: the non-visible phrasebox was not a problem in the wx version.
	// BEW added 20Dec07: in Reviewing mode, the box does not always get drawn (eg. if click on a
	// strip which is all holes and then advance the box by using Enter key, the box remains invisible,
	// and stays so for subsequent Enter presses in later holes in the same and following strips:
	// same addition is at the end of the ResizeBox() function, for the same reason
	//pView->m_targetBox.Invalidate(); // hopefully this will fix it - it doesn't unfortunately
	 // perhaps the box paint occurs too early and the view point wipes it. How then do we delay
	 // the box paint? Maybe put the invalidate call into the View's OnDraw() at the end of its handler?
	//pView->RemakePhraseBox(pView->m_pActivePile, pView->m_targetPhrase); // also doesn't work.
}

// MFC docs say about CWnd::OnChar "The framework calls this member function when
// a keystroke translates to a nonsystem character. This function is called before
// the OnKeyUp member function and after the OnKeyDown member function are called.
// OnChar contains the value of the keyboard key being pressed or released. This
// member function is called by the framework to allow your application to handle
// a Windows message. The parameters passed to your function reflect the parameters
// received by the framework when the message was received. If you call the
// base-class implementation of this function, that implementation will use the
// parameters originally passed with the message and not the parameters you supply
// to the function."
// Hence the calling order in MFC is OnKeyDown, OnChar, OnKeyUp.
// The wxWidgets docs say about wxKeyEvent "Notice that there are three different
// kinds of keyboard events in wxWidgets: key down and up events and char events.
// The difference between the first two is clear - the first corresponds to a key
// press and the second to a key release - otherwise they are identical. Just note
// that if the key is maintained in a pressed state you will typically get a lot
// of (automatically generated) down events but only one up so it is wrong to
// assume that there is one up event corresponding to each down one. Both key
// events provide untranslated key codes while the char event carries the
// translated one. The untranslated code for alphanumeric keys is always an upper
// case value. For the other keys it is one of WXK_XXX values from the keycodes
// table. The translated key is, in general, the character the user expects to
// appear as the result of the key combination when typing the text into a text
// entry zone, for example.

// OnChar is called via EVT_CHAR(OnChar) in our CPhraseBox Event Table.
// BEW 13Apr10, no changes needed for support of doc version 5
// BEW 22Jun10, no changes needed for support of kbVersion 2
void CPhraseBox::OnChar(wxKeyEvent& event)
{
	// whm Note: OnChar() is called before OnPhraseBoxChanged()
	CAdapt_ItApp* pApp = &wxGetApp();
	wxASSERT(pApp != NULL);
	CLayout* pLayout = GetLayout();
	CAdapt_ItView* pView = pLayout->m_pView;
	wxASSERT(pView->IsKindOf(CLASSINFO(CAdapt_ItView)));

	// wx version note: In MFC this OnChar() function is NOT called for system key events,
	// however in the wx version it IS called for combination system key events such as
	// ALT+Arrow-key. We will immediately return if the CTRL or ALT key is down; Under
	// wx's wxKeyEvent we can test CTRL or ALT down with HasModifiers(), and we must also
	// prevent any arrow keys.
	if (event.HasModifiers() || event.GetKeyCode() == WXK_DOWN || event.GetKeyCode() == WXK_UP
		|| event.GetKeyCode() == WXK_LEFT || event.GetKeyCode() == WXK_RIGHT)
	{
		event.Skip(); // to allow OnKeyUp() to handle the event
		return;
	}

	// BEW added 25Sep13 to kill the "Non-sticking / Truncation bug" - where a Save and
	// then typing more in the phrase box, when user hit Enter to advance the box, the
	// string typed was not shown in the layout, but rather whatever was there in the box
	// at the Save (or Auto-save). The app variable, m_nPlacePunctDlgCallNumber, which
	// starts as 0, gets augmented each time MakeTargetStringIncludingPunction() is called
	// and if the value goes over 1, none of the code for setting m_targetStr within that
	// function gets called. The Save got the variable set to 1, and the advance of the
	// box gives another call of the function - to 2, and hence the Non-stick bug happens.
	// The solution is to clear m_nPlacePunctDlgCallNumber to 0 on every keystroke in the
	// phrasebox - so that's what we do here, and the non-stick bug is history
	pApp->m_nPlacePunctDlgCallNumber = 0; // initialize on every keystroke

#ifdef Do_Clipping
	//wxLogDebug(_T("In OnChar), ** KEY TYPED **"));
#endif
	// whm Note: The following code for handling the WXK_BACK key is ok to leave here in
	// the OnChar() handler, because it is placed before the Skip() call (the OnChar() base
	// class call in MFC)

    GetTextCtrl()->GetSelection(&m_nSaveStart,&m_nSaveEnd);

    // MFC Note: CEdit's Undo() function does not undo a backspace deletion of a selection
    // or single char, so implement that here & in an override for OnEditUndo();
	if (event.GetKeyCode() == WXK_BACK)
	{
		m_backspaceUndoStr.Empty();
		wxString s;
		s = GetValue();
		if (m_nSaveStart == m_nSaveEnd)
		{
			// deleting previous character
			if (m_nSaveStart > 0)
			{
				int index = m_nSaveStart;
				index--;
				m_nSaveStart = index;
				m_nSaveEnd = index;
				wxChar ch = s.GetChar(index);
				m_backspaceUndoStr += ch;
			}
			else // m_nSaveStart must be zero, as when the box is empty
			{
                // BEW added 20June06, because if the CSourcePhrase has m_bHasKBEntry TRUE,
                // but an empty m_adaption (because the user specified <no adaptation>
                // there earlier) then there was no way in earlier versions to "remove" the
                // <no adaptation> - so we want to allow a backspace keypress, with the box
                // empty, to effect the clearing of m_bHasKBEntry to FALSE (or if glossing,
                // m_bHasGlossingKBEntry to FALSE), and to decement the <no adaptation> ref
				// count or remove the entry entirely if the count is 1 from the KB entry
                // (BEW 22Jun10, for kbVersion 2, "remove" now means, "set m_bDeleted to
                // TRUE" for the CRefString instance storing the <Not In KB> entry)
				if (pApp->m_pActivePile->GetSrcPhrase()->m_adaption.IsEmpty() &&
					((pApp->m_pActivePile->GetSrcPhrase()->m_bHasKBEntry && !gbIsGlossing) ||
					(pApp->m_pActivePile->GetSrcPhrase()->m_bHasGlossingKBEntry && gbIsGlossing)))
				{
					m_bNoAdaptationRemovalRequested = TRUE;
					CSourcePhrase* pSrcPhrase = pApp->m_pActivePile->GetSrcPhrase();
					wxString emptyStr = _T("");
					pApp->m_pKB->GetAndRemoveRefString(pSrcPhrase,emptyStr,useGlossOrAdaptationForLookup);
				}
			}
		}
		else
		{
			// deleting a selection
			int count = m_nSaveEnd - m_nSaveStart;
			if (count > 0)
			{
				m_backspaceUndoStr = s.Mid(m_nSaveStart,count);
				m_nSaveEnd = m_nSaveStart;
			}
		}
	}

	// wxWidgets Note: The wxTextCtrl does not have a virtual OnChar() method,
	// so we'll just call .Skip() for any special handling of the WXK_RETURN and WXK_TAB
	// key events. In wxWidgets, calling event.Skip() is analagous to calling
	// the base class version of a virtual function. Note: wxTextCtrl has
	// a non-virtual OnChar() method. See "wxTextCtrl OnChar event handling.txt"
	// for a newsgroup sample describing how to use OnChar() to do "auto-
	// completion" while a user types text into a text ctrl.
	if (!(event.GetKeyCode() == WXK_RETURN || event.GetKeyCode() == WXK_TAB)) // I don't
											// want a bell when RET key or HT key typed
	{
        // whm note: Instead of explicitly calling the OnChar() function in the base class
        // (as would normally be done for C++ virtual functions), in wxWidgets, we call
        // Skip() instead, for the event to be processed either in the base wxWidgets class
        // or the native control. This difference in event handling also means that part of
        // the code in OnChar() that can be handled correctly in the MFC version, must be
        // moved to our wx-specific OnPhraseBoxChanged() handler. This is necessitated
        // because GetValue() does not get the new value of the control's contents when
        // called from within OnChar, but received the previous value of the string as it
        // existed before the keystroke that triggers OnChar.
		event.Skip();
	}

	// preserve cursor location, in case we merge, so we can restore it afterwards
	long nStartChar;
	long nEndChar;
    GetTextCtrl()->GetSelection(&nStartChar,&nEndChar);

    // whm Note: See note below about meeding to move some code from OnChar() to the
    // OnPhraseBoxChanged() handler in the wx version, because the OnChar() handler does
    // not have access to the changed value of the new string within the control reflecting
    // the keystroke that triggers OnChar().
	//
	wxSize textExtent;
	pApp->RefreshStatusBarInfo();

	// if there is a selection, and user forgets to make the phrase before typing, then do it
	// for him on the first character typed. But if glossing, no merges are allowed.

	int theCount = pApp->m_selection.GetCount();
	if (!gbIsGlossing && theCount > 1 && (pApp->m_pActivePile == pApp->m_pAnchor->GetPile()
		|| IsActiveLocWithinSelection(pView, pApp->m_pActivePile)))
	{
		if (pView->GetSelectionWordCount() > MAX_WORDS)
		{
			pApp->GetRetranslation()->DoRetranslation();
		}
		else
		{
			if (!pApp->m_bUserTypedSomething &&
				!pApp->m_pActivePile->GetSrcPhrase()->m_targetStr.IsEmpty())
			{
				pApp->m_bSuppressDefaultAdaptation = FALSE; // we want what is already there
			}
			else
			{
				// for version 1.4.2 and onwards, we will want to check m_bRetainBoxContents
				// and two other flags, for a click or arrow key press is meant to allow
				// the deselected source word already in the phrasebox to be retained; so we
				// here want the m_bSuppressDefaultAdaptation flag set TRUE only when the
				// m_bRetainBoxContents is FALSE (- though we use two other flags too to
				// ensure we get this behaviour only when we want it)
				if (m_bRetainBoxContents && !m_bAbandonable && pApp->m_bUserTypedSomething)
				{
                    pApp->m_bSuppressDefaultAdaptation = FALSE;
				}
				else
				{
                    pApp->m_bSuppressDefaultAdaptation = TRUE; // the global BOOLEAN used for temporary
													   // suppression only
				}
			}
			pView->MergeWords(); // simply calls OnButtonMerge
            // 
            //
			pLayout->m_docEditOperationType = merge_op;
            pApp->m_bSuppressDefaultAdaptation = FALSE;

			// we can assume what the user typed, provided it is a letter, replaces what was
			// merged together, but if tab or return was typed, we allow the merged text to
			// remain intact & pass the character on to the switch below; but since v1.4.2 we
			// can only assume this when m_bRetainBoxContents is FALSE, if it is TRUE and
			// a merge was done, then there is probably more than what was just typed, so we
			// retain that instead; also, we we have just returned from a MergeWords( ) call in
			// which the phrasebox has been set up with correct text (such as previous target text
			// plus the last character typed for an extra merge when a selection was present, we
			// don't want this wiped out and have only the last character typed inserted instead,
			// so in OnButtonMerge( ) we test the phrasebox's string and if it has more than just
			// the last character typed, we assume we want to keep the other content - and so there
			// we also set m_bRetainBoxContents
            m_bRetainBoxContents = FALSE; // turn it back off (default) until next required
		}
	}
	else
	{
        // if there is a selection, but the anchor is removed from the active location, we
        // don't want to make a phrase elsewhere, so just remove the selection. Or if
        // glossing, just silently remove the selection - that should be sufficient alert
        // to the user that the merge operation is unavailable
		pView->RemoveSelection();
		wxClientDC dC(pLayout->m_pCanvas);
		pView->canvas->DoPrepareDC(dC); // adjust origin
		pApp->GetMainFrame()->PrepareDC(dC); // wxWidgets' drawing.cpp sample also calls
											 // PrepareDC on the owning frame
		pLayout->m_docEditOperationType = no_edit_op;
		pView->Invalidate();
	}

    // whm Note: The following code is moved to the OnPhraseBoxChanged() handler in the wx
    // version, because the OnChar() handler does not have access to the changed value of
    // the new string within the control reflecting the keystroke that triggers OnChar().
    // Because of that difference in behavior, I moved the code dependent on updating
    // pApp->m_targetPhrase from OnChar() to the OnPhraseBoxChanged() handler.

	long keycode = event.GetKeyCode();
	switch(keycode)
	{
	case WXK_RETURN: //13:	// RETURN key
		{
            // whm 26Feb2018 Note: Code from this case WXK_RETURN is also used in the 
            // OnComboItemSelected() handler to advance the phrasebox when an item is 
            // selected from the list. Here however, an ENTER is pressed within the dropdown's
            // edit box - the phrasebox itself. So, pressing Enter in dropdown phrasebox - is 
            // tantamount to having selected that dropdown list item directly.
            // It should in the new app do the same thing that happened in the legacy app when 
            // the Choose Translation dialog had popped up and the desired item was already
            // highlighted in its dialog list, and the user pressed the OK button. That OK button
            // press is essentially the same as the user in the new app pressing the Enter key
            // when something is contained in the dropdown's edit box. 
            // Hence, before calling MoveToPrevPile() or JumpForward() below, we should inform
            // the app to handle the contents as a change.
            wxString boxContent;
            boxContent = this->GetValue();
            gpApp->m_targetPhrase = boxContent;
            if (!this->GetTextCtrl()->IsModified()) // need to call SetModified on m_pTargetBox before calling SetValue // whm 14Feb2018 added GetTextCtrl()->
            {
                this->GetTextCtrl()->SetModified(TRUE); // Set as modified so that CPhraseBox::OnPhraseBoxChanged() will so its work // whm 14Feb2018 added GetTextCtrl()->
            }
            this->m_bAbandonable = FALSE; // this is done in CChooseTranslation::OnOK()

			// save old sequ number in case required for toolbar's Back button
            pApp->m_nOldSequNum = pApp->m_nActiveSequNum;

			// whm Note: Beware! Setting breakpoints in OnChar() before this point can
			// affect wxGetKeyState() results making it appear that WXK_SHIFT is not detected
			// below. Solution: remove the breakpoint(s) for wxGetKeyState(WXK_SHIFT) to
			// register properly.
			if (wxGetKeyState(WXK_SHIFT)) // SHIFT+RETURN key
			{
				// shift key is down, so move back a pile

				int bSuccessful = MoveToPrevPile(pApp->m_pActivePile);
				if (!bSuccessful)
				{
					// we were at the start of the document, so do nothing
					;
				}
				else
				{
					// it was successful
					pLayout->m_docEditOperationType = relocate_box_op;
				}

				pApp->GetMainFrame()->canvas->ScrollIntoView(pApp->m_nActiveSequNum);

				// save the phrase box's text, in case user hits SHIFT+End to unmerge a phrase
                m_SaveTargetPhrase = pApp->m_targetPhrase;
				return;
			}
			else // we are moving forwards rather than backwards
			{
				JumpForward(pView);
			}
		} // end case 13: block
		return;
	case WXK_TAB: //9:		// TAB key
		{
            // whm 26Feb2018 Note: Except for handling of SHIFT+TAB below, the handling of WXK_TAB
            // below should be identical to the handling of WXK_RETURN above. So, pressing Tab 
            // within dropdown phrasebox - when the content of the dropdown's edit box is tantamount 
            // to having selected that dropdown list item directly.
            // It should - in the new app - do the same thing that happened in the legacy app when 
            // the Choose Translation dialog had popped up and the desired item was already
            // highlighted in its dialog list, and the user pressed the OK button. That OK button
            // press is essentially the same as the user in the new app pressing the Enter key
            // when something is contained in the dropdown's edit box. 
            // Hence, before calling MoveToPrevPile() or JumpForward() below, we should inform
            // the app to handle the contents as a change.
            wxString boxContent;
            boxContent = this->GetValue();
            gpApp->m_targetPhrase = boxContent;
            if (!this->GetTextCtrl()->IsModified()) // need to call SetModified on m_pTargetBox before calling SetValue // whm 14Feb2018 added GetTextCtrl()->
            {
                this->GetTextCtrl()->SetModified(TRUE); // Set as modified so that CPhraseBox::OnPhraseBoxChanged() will so its work // whm 14Feb2018 added GetTextCtrl()->
            }
            this->m_bAbandonable = FALSE; // this is done in CChooseTranslation::OnOK()

                                      // save old sequ number in case required for toolbar's Back button
            pApp->m_nOldSequNum = pApp->m_nActiveSequNum;

			// SHIFT+TAB is the 'universal' keyboard way to cause a move back, so implement it
			// whm Note: Beware! Setting breakpoints in OnChar() before this point can
			// affect wxGetKeyState() results making it appear that WXK_SHIFT is not detected
			// below. Solution: remove the breakpoint(s) for wxGetKeyState(WXK_SHIFT) to
			if (wxGetKeyState(WXK_SHIFT)) // SHIFT+TAB
			{
				// shift key is down, so move back a pile

				// Shift+Tab (reverse direction) indicates user is probably
				// backing up to correct something that was perhaps automatically
				// inserted, so we will preserve any highlighting and do nothing
				// here in response to Shift+Tab.

				Freeze();

				int bSuccessful = MoveToPrevPile(pApp->m_pActivePile);
				if (!bSuccessful)
				{
					// we have come to the start of the document, so do nothing
					pLayout->m_docEditOperationType = no_edit_op;
				}
				else
				{
					// it was successful
					pLayout->m_docEditOperationType = relocate_box_op;
				}

				// scroll, if necessary
				pApp->GetMainFrame()->canvas->ScrollIntoView(pApp->m_nActiveSequNum);

				// save the phrase box's text, in case user hits SHIFT+END key to unmerge
				// a phrase
                m_SaveTargetPhrase = pApp->m_targetPhrase;

				Thaw();
				return;
			}
			else
			{
				//BEW changed 01Aug05. Some users are familiar with using TAB key to advance
				// (especially when working with databases), and without thinking do so in Adapt It
				// and expect the Lookup process to take place, etc - and then get quite disturbed
				// when it doesn't happen that way. So for version 3 and onwards, we will interpret
				// a TAB keypress as if it was an ENTER keypress
				JumpForward(pView);
			}
			return;
		}
	case WXK_BACK: //8:		// BackSpace key
		{
			bool bWasMadeDirty = TRUE;
			pLayout->m_docEditOperationType = no_edit_op;
            // whm Note: pApp->m_targetPhrase is updated in OnPhraseBoxChanged, so the wx
            // version uses the global below, rather than a value determined in OnChar(),
            // which would not be current.
 			// BEW added test 1Jul09, because when a selection for a merge is current and
			// the merge is done by user pressing Backspace key, without the suppression
			// here FixBox() would be called and the box would be made shorter, resulting
			// in the width of the box being significantly less than the width of the
			// source text phrase from the merger, and that would potentially allow any
			// following piles with short words to be displayed after the merger -
			// overwriting the end of the source text; so in a merger where it is
			// initiated by a <BS> key press, we suppress the FixBox() call
			if (!pApp->m_bMergeSucceeded)
			{
				FixBox(pView, pApp->m_targetPhrase, bWasMadeDirty, textExtent, 2);
										// selector = 2 for "contracting" the box
			}
            pApp->m_bMergeSucceeded = FALSE; // clear to default FALSE, otherwise backspacing
									// to remove phrase box characters won't get the
									// needed box resizes done
		}
	default:
		;
	}
}

// returns TRUE if the move was successful, FALSE if not successful
// Ammended July 2003 for auto-capitalization support
// BEW 13Apr10, no changes needed for support of doc version 5
// BEW 21Jun10, no changes needed for support of kbVersion 2, & removed pView from signature
// BEW 17Jul11, changed for GetRefString() to return KB_Entry enum, and use all 10 maps
// for glossing KB
bool CPhraseBox::MoveToPrevPile(CPile *pCurPile)
{
	CAdapt_ItApp* pApp = (CAdapt_ItApp*)&wxGetApp();
	pApp->m_preGuesserStr.Empty(); // BEW 27Nov14, in case a src string, or modified string
		// is stored ready for user's Esc keypress to restore the pre-guesser
		// form, clear it, because the box is gunna move and we want it
		// restored to default empty ready for next box landing location
	// whm modified 29Mar12. Left mouse clicks now beep only when certain parts of
	// the canvas are clicked on, and allows other clicks to act normally (such as
	// the opening/closing of the ViewFilteredMaterial dialog and the Notes dialog).
	// store the current translation, if one exists, before retreating, since each retreat
    // unstores the refString's translation from the KB, so they must be put back each time
    // (perhaps in edited form, if user changed the string before moving back again)
	wxASSERT(pApp != NULL);
	//pApp->limiter = 0; // BEW added Aug13, to support OnIdle() hack for m_targetStr non-stick bug // bug fixed 24Sept13 BEW
	CAdapt_ItView *pView = pApp->GetView();
	CAdapt_ItDoc* pDoc = pView->GetDocument();
	m_bBoxTextByCopyOnly = FALSE; // restore default setting
	CLayout* pLayout = GetLayout();
	CSourcePhrase* pOldActiveSrcPhrase = pCurPile->GetSrcPhrase();

	// make sure m_targetPhrase doesn't have any final spaces either
	RemoveFinalSpaces(pApp->m_pTargetBox,&pApp->m_targetPhrase);

	// if we are at the start, we can't move back any further
	// - but check vertical edit situation first
	int nCurSequNum = pCurPile->GetSrcPhrase()->m_nSequNumber;

	// if vertical editing is in progress, don't permit a move backwards into the preceding
	// gray text area; just beep and return without doing anything
	if (gbVerticalEditInProgress)
	{
		EditRecord* pRec = &gEditRecord;
		if (gEditStep == adaptationsStep || gEditStep == glossesStep)
		{
			if (nCurSequNum <= pRec->nStartingSequNum)
			{
				// we are about to try to move back into the gray text area before the edit span, disallow
				::wxBell();
				pApp->m_pTargetBox->SetFocus();
				pLayout->m_docEditOperationType = no_edit_op;
				return FALSE;
			}
		}
		else if (gEditStep == freeTranslationsStep)
		{
			if (nCurSequNum <= pRec->nFreeTrans_StartingSequNum)
			{
                // we are about to try to move back into the gray text area before the free
                // trans span, disallow (I don't think we can invoke this function from
                // within free translation mode, but no harm to play safe)
				::wxBell();
				pApp->m_pTargetBox->SetFocus();
				pLayout->m_docEditOperationType = no_edit_op;
				return FALSE;
			}
		}
	}
	if (nCurSequNum == 0)
	{
		// IDS_CANNOT_GO_BACK
		wxMessageBox(_(
"You are already at the start of the file, so it is not possible to move back any further."),
		_T(""), wxICON_INFORMATION | wxOK);
		pApp->m_pTargetBox->SetFocus();
		pLayout->m_docEditOperationType = no_edit_op;
		return FALSE;
	}
	bool bOK;

	// don't move back if it means moving to a retranslation pile; but if we are
	// glossing it is okay to move back into a retranslated section
	{
		CPile* pPrev = pView->GetPrevPile(pCurPile);
		wxASSERT(pPrev);
		if (!gbIsGlossing && pPrev->GetSrcPhrase()->m_bRetranslation)
		{
			// IDS_NO_ACCESS_TO_RETRANS
			wxMessageBox(_(
"To edit or remove a retranslation you must use the toolbar buttons for those operations."),
			_T(""), wxICON_INFORMATION | wxOK);
			pApp->m_pTargetBox->SetFocus();
			pLayout->m_docEditOperationType = no_edit_op;
			return FALSE;
		}
	}

    // if the location is a <Not In KB> one, we want to skip the store & fourth line
    // creation --- as of Dec 18, version 1.4.0, according to Susanna Imrie's
    // recommendation, I've changed this so it will allow a non-null adaptation to remain
    // at this location in the document, but just to suppress the KB store; if glossing is
    // ON, then being a <Not In KB> location is irrelevant, and we will want the store done
    // normally - but to the glossing KB of course
	//bool bNoError = TRUE;
	if (!gbIsGlossing && !pCurPile->GetSrcPhrase()->m_bHasKBEntry
												&& pCurPile->GetSrcPhrase()->m_bNotInKB)
	{
        // if the user edited out the <Not In KB> entry from the KB editor, we need to put
        // it back so that the setting is preserved (the "right" way to change the setting
        // is to use the toolbar checkbox - this applies when adapting, not glossing)
		pApp->m_pKB->Fix_NotInKB_WronglyEditedOut(pCurPile);
		goto b;
	}

	// if the box contents is null, then the source phrase must store an empty string
	// as appropriate - either m_adaption when adapting, or m_gloss when glossing
	if (pApp->m_targetPhrase.IsEmpty())
	{
		pApp->m_bForceAsk = FALSE; // make sure it's turned off, & allow function to continue
		if (gbIsGlossing)
			pCurPile->GetSrcPhrase()->m_gloss.Empty();
		else
			pCurPile->GetSrcPhrase()->m_adaption.Empty();
	}

    // make the punctuated target string, but only if adapting; note, for auto
    // capitalization ON, the function will change initial lower to upper as required,
    // whatever punctuation regime is in place for this particular sourcephrase instance...
    // we are about to leave the current phrase box location, so we must try to store what
    // is now in the box, if the relevant flags allow it. Test to determine which KB to
    // store to. StoreText( ) has been ammended for auto-capitalization support (July 2003)
	if (gbIsGlossing)
	{
		// BEW added 27May10, to not save contents if backing back from a halt
		// location, when there is no content on the CSourcePhrase instance already
		if (pCurPile->GetSrcPhrase()->m_gloss.IsEmpty())
		{
			bOK = TRUE;
		}
		else
		{
			bOK = pApp->m_pGlossingKB->StoreTextGoingBack(pCurPile->GetSrcPhrase(), pApp->m_targetPhrase);
		}
	}
	else // adapting
	{
		// BEW added 27May10, to not save contents if backing back from a halt
		// location, when there is no content on the CSourcePhrase instance already
		if (pCurPile->GetSrcPhrase()->m_adaption.IsEmpty())
		{
			bOK = TRUE;
		}
		else
		{
			pView->MakeTargetStringIncludingPunctuation(pCurPile->GetSrcPhrase(), pApp->m_targetPhrase);
			pView->RemovePunctuation(pDoc, &pApp->m_targetPhrase, from_target_text);
			pApp->m_bInhibitMakeTargetStringCall = TRUE;
			bOK = pApp->m_pKB->StoreTextGoingBack(pCurPile->GetSrcPhrase(), pApp->m_targetPhrase);
			pApp->m_bInhibitMakeTargetStringCall = FALSE;
		}
	}
	if (!bOK)
	{
        // here, MoveToNextPile() calls DoStore_NormalOrTransliterateModes(), but for
        // MoveToPrevPile() we will keep it simple and not try to get text for the phrase
        // box
		pLayout->m_docEditOperationType = no_edit_op;
		return FALSE; // can't move if the adaptation or gloss text is not yet completed
	}

	// move to previous pile's cell
b:	CPile* pNewPile = pView->GetPrevPile(pCurPile); // does not update the view's
				// m_nActiveSequNum nor m_pActivePile pointer, so update these here,
				// provided NULL was not returned

	// restore default button image, and m_bCopySourcePunctuation to TRUE
	wxCommandEvent event;
	if (!pApp->m_bCopySourcePunctuation)
	{
		pApp->GetView()->OnToggleEnablePunctuationCopy(event);
	}
	if (pNewPile == NULL)
	{
		// we deem vertical editing current step to have ended if control gets into this
		// block, so user has to be asked what to do next if vertical editing is currently
		// in progress; and we tunnel out before m_nActiveSequNum can be set to -1 (otherwise
		// vertical edit will crash when recalc layout is tried with a bad sequ num value)
		if (gbVerticalEditInProgress)
		{
			m_bTunnellingOut = FALSE; // ensure default value set
			bool bCommandPosted = pView->VerticalEdit_CheckForEndRequiringTransition(-1,
							nextStep, TRUE); // bForceTransition is TRUE
			if (bCommandPosted)
			{
				// don't proceed further because the current vertical edit step has ended
				m_bTunnellingOut = TRUE; // so caller can use it
				pLayout->m_docEditOperationType = no_edit_op;
				return FALSE;
			}
		}
		pLayout->m_docEditOperationType = no_edit_op;
		return FALSE; // we are at the start of the file too, so can't go further back
	}
	else
	{
		// the previous pile ptr is valid, so proceed

        // don't commit to the new pile if we are in vertical edit mode, until we've
        // checked the pile is not in the gray text area...
        // if vertical editing is currently in progress we must check if the lookup target
        // is within the editable span, if not then control has moved the box into the gray
        // area beyond the editable span and that means a step transition is warranted &
        // the user should be asked what step is next
		if (gbVerticalEditInProgress)
		{
			int m_nCurrentSequNum = pNewPile->GetSrcPhrase()->m_nSequNumber;
			m_bTunnellingOut = FALSE; // ensure default value set
			bool bCommandPosted = pView->VerticalEdit_CheckForEndRequiringTransition(
									m_nCurrentSequNum,nextStep); // bForceTransition is FALSE
			if (bCommandPosted)
			{
				// don't proceed further because the current vertical edit step has ended
				m_bTunnellingOut = TRUE; // so caller can use it
				pLayout->m_docEditOperationType = no_edit_op;
				return FALSE; // try returning FALSE
			}
			else
			{
				// BEW 19Oct15 No transition of vert edit modes,
				// so we can store this location on the app
				gpApp->m_vertEdit_LastActiveSequNum = m_nCurrentSequNum;
#if defined(_DEBUG)
				wxLogDebug(_T("VertEdit PhrBox, MoveToPrevPile() storing loc'n: %d "), m_nCurrentSequNum);
#endif
			}
		}

		pApp->m_bUserTypedSomething = FALSE; // user has not typed at the new location yet

		// update the active sequence number, and pile pointer
		pApp->m_nActiveSequNum = pNewPile->GetSrcPhrase()->m_nSequNumber;
		pApp->m_pActivePile = pNewPile;

        // get an adjusted pile pointer for the new active location, and we want the
        // pile's strip to be marked as invalid and the strip index added to the
        // CLayout::m_invalidStripArray
		pDoc->ResetPartnerPileWidth(pOldActiveSrcPhrase);

        // since we are moving back, the prev pile is likely to have a refString
        // translation which is nonempty, so we have to put it into m_targetPhrase so that
        // ResizeBox will use it; but if there is none, the copy the source key if the
        // m_bCopySource flag is set, else just set it to an empty string. (bNeed Modify is
        // a helper flag used for setting/clearing the document's modified flag at the end
        // of this function)
		bool bNeedModify = FALSE; // reset to TRUE if we copy source
								  // because there was no adaptation
		// be careful, the pointer might point to <Not In KB>, rather than a normal entry
		CRefString* pRefString = NULL;
		KB_Entry rsEntry;
		if (gbIsGlossing)
		{
			rsEntry = pApp->m_pGlossingKB->GetRefString(pNewPile->GetSrcPhrase()->m_nSrcWords,
					pNewPile->GetSrcPhrase()->m_key, pNewPile->GetSrcPhrase()->m_gloss, pRefString);
		}
		else
		{
			rsEntry = pApp->m_pKB->GetRefString(pNewPile->GetSrcPhrase()->m_nSrcWords,
					pNewPile->GetSrcPhrase()->m_key, pNewPile->GetSrcPhrase()->m_adaption, pRefString);
		}
		if (pRefString != NULL && rsEntry == really_present)
		{
			pView->RemoveSelection(); // we won't do merges in this situation

			// assign the translation text - but check it's not "<Not In KB>", if it is, we
			// leave the phrase box empty, turn OFF the m_bSaveToKB flag -- this is changed
			// for v1.4.0 and onwards because we will want to leave any adaptation already
			// present unchanged, rather than clear it and so we will not make it abandonable
			// either
			wxString str = pRefString->m_translation; // no case change to be done here since
								// all we want to do is remove the refString or decrease its
								// reference count
			if (!gbIsGlossing && str == _T("<Not In KB>"))
			{
				pApp->m_bSaveToKB = FALSE;
				pApp->m_pTargetBox->m_bAbandonable = FALSE; // used to be TRUE;
				pNewPile->GetSrcPhrase()->m_bHasKBEntry = FALSE; // ensures * shows above this
															   // srcPhrase
				pNewPile->GetSrcPhrase()->m_bNotInKB = TRUE;
			}

            // remove the translation from the KB, in case user wants to edit it before
            // it's stored again (RemoveRefString also clears the m_bHasKBEntry flag on the
            // source phrase, or m_bHasGlossingKBEntry if gbIsGlossing is TRUE)
			if (gbIsGlossing)
			{
				pApp->m_pGlossingKB->RemoveRefString(pRefString, pNewPile->GetSrcPhrase(),
											pNewPile->GetSrcPhrase()->m_nSrcWords);
				pApp->m_targetPhrase = pNewPile->GetSrcPhrase()->m_gloss;
			}
			else
			{
				pApp->m_pKB->RemoveRefString(pRefString, pNewPile->GetSrcPhrase(),
											pNewPile->GetSrcPhrase()->m_nSrcWords);
				// since we have optional punctuation hiding, use the line with
				// the punctuation
				pApp->m_targetPhrase = pNewPile->GetSrcPhrase()->m_targetStr;
			}
		}
		else // the pointer to refString was null (ie. no KB entry) or rsEntry was present_but_deleted
		{
			if (gbIsGlossing)  // ensure the flag below is false when there is no KB entry
				pNewPile->GetSrcPhrase()->m_bHasGlossingKBEntry = FALSE;
			else
				pNewPile->GetSrcPhrase()->m_bHasKBEntry = FALSE;

			// just use an empty string, or copy the sourcePhrase's key if
			// the m_bCopySource flag is set
			if (pApp->m_bCopySource)
			{
				// whether glossing or adapting, we don't want a null source phrase
				// to initiate a copy
				if (!pNewPile->GetSrcPhrase()->m_bNullSourcePhrase)
				{
					pApp->m_targetPhrase = pView->CopySourceKey(pNewPile->GetSrcPhrase(),
											pApp->m_bUseConsistentChanges);
					bNeedModify = TRUE;
				}
				else
					pApp->m_targetPhrase.Empty();
			}
			else
				pApp->m_targetPhrase.Empty(); // this will cause pile's m_nMinWidth to be used
											  // to set the m_curBoxWidth value on the view
		}

        // initialize the phrase box too, so it doesn't carry an old string to the next
        // pile's cell
		ChangeValue(pApp->m_targetPhrase); //SetWindowText(pApp->m_targetPhrase);

        // get an adjusted pile pointer for the new active location, and we want the
        // pile's strip to be marked as invalid and the strip index added to the
        // CLayout::m_invalidStripArray
		if (pNewPile != NULL)
		{
			pDoc->ResetPartnerPileWidth(pNewPile->GetSrcPhrase());
		}

        pApp->m_nActiveSequNum = pNewPile->GetSrcPhrase()->m_nSequNumber;
		pApp->m_pActivePile = pView->GetPile(pApp->m_nActiveSequNum);
#ifdef _NEW_LAYOUT
		pLayout->RecalcLayout(pApp->m_pSourcePhrases, keep_strips_keep_piles);
#else
		pLayout->RecalcLayout(pApp->m_pSourcePhrases, create_strips_keep_piles);
#endif

		// make sure the new active pile's pointer is reset
		pApp->m_pActivePile = pView->GetPile(pApp->m_nActiveSequNum);
		pLayout->m_docEditOperationType = relocate_box_op;

		// recreate the phraseBox using the stored information
		pApp->m_nStartChar = -1; pApp->m_nEndChar = -1;

		// fix the m_bSaveToKB flag, depending on whether or not srcPhrase is in kb
		if (!gbIsGlossing && !pApp->m_pActivePile->GetSrcPhrase()->m_bHasKBEntry &&
										pApp->m_pActivePile->GetSrcPhrase()->m_bNotInKB)
		{
			pApp->m_bSaveToKB = FALSE;
			pApp->m_targetPhrase.Empty();
		}
		else
		{
			pApp->m_bSaveToKB = TRUE;
		}

		// update status bar with project name
		pApp->RefreshStatusBarInfo();

		// BEW note 30Mar09: later we may set clipping.... in the meantime
		// just invalidate the lot
		pView->Invalidate();
		pLayout->PlaceBox();

		if (bNeedModify)
			SetModify(TRUE); // our own SetModify(); calls MarkDirty()
		else
			SetModify(FALSE); // our own SetModify(); calls DiscardEdits();

		return TRUE;
	}
}

// returns TRUE if the move was successful, FALSE if not successful
// Ammended, July 2003, for auto capitalization support
// BEW 13Apr10, no changes needed for support of doc version 5
// BEW 21Jun10, no changes needed for support of kbVersion 2, & removed pView from signature
// BEW 17Jul11, changed for GetRefString() to return KB_Entry enum, and use all 10 maps
// for glossing KB
bool CPhraseBox::MoveToImmedNextPile(CPile *pCurPile)
{
	CAdapt_ItApp* pApp = (CAdapt_ItApp*)&wxGetApp();
	pApp->m_preGuesserStr.Empty(); // BEW 27Nov14, in case a src string, or modified string
		// is stored ready for user's Esc keypress to restore the pre-guesser
		// form, clear it, because the box is gunna move and we want it
		// restored to default empty ready for next box landing location
	// whm modified 29Mar12. Left mouse clicks now beep only when certain parts of
	// the canvas are clicked on, and allows other clicks to act normally (such as
	// the opening/closing of the ViewFilteredMaterial dialog and the Notes dialog).
	// store the current translation, if one exists, before moving to next pile, since each move
	// unstores the refString's translation from the KB, so they must be put back each time
	// (perhaps in edited form, if user changed the string before moving back again)
	wxASSERT(pApp != NULL);
	//pApp->limiter = 0; // BEW added Aug13, to support OnIdle() hack for m_targetStr non-stick bug // bug fixed 24Sept13 BEW
	CAdapt_ItView *pView = pApp->GetView();
	CAdapt_ItDoc* pDoc = pView->GetDocument();
	m_bBoxTextByCopyOnly = FALSE; // restore default setting
	CSourcePhrase* pOldActiveSrcPhrase = pCurPile->GetSrcPhrase();
	bool bOK;

	// make sure m_targetPhrase doesn't have any final spaces
	RemoveFinalSpaces(pApp->m_pTargetBox, &pApp->m_targetPhrase);

	// BEW changed 25Oct09, altered syntax so it no longer exits here if pFwd
	// is NULL, otherwise the document-end typed meaning doesn't 'stick' in
	// the document
	CPile* pFwd = pView->GetNextPile(pCurPile);
	if (pFwd == NULL)
	{
		// no more piles, but continue so we can make the user's typed string
		// 'stick' before we prematurely exit further below
		;
	}
	else
	{
		// when adapting, don't move forward if it means moving to a
		// retranslation pile but we don't care when we are glossing
		bool bNotInRetranslation =
			CheckPhraseBoxDoesNotLandWithinRetranslation(pView,pFwd,pCurPile);
		if (!gbIsGlossing && !bNotInRetranslation)
		{
			// BEW removed this message, because it is already embedded in the prior
			// CheckPhraseBoxDoesNotLandWithinRetranslation(pView,pFwd,pCurPile) call, and
			// will be shown from there if relevant.
			//wxMessageBox(_(
            //"Sorry, to edit or remove a retranslation you must use the toolbar buttons for those operations."),
			//_T(""), wxICON_INFORMATION | wxOK);
			pApp->m_pTargetBox->SetFocus();
			GetLayout()->m_docEditOperationType = no_edit_op;
			return FALSE;
		}
	}

    // if the location is a <Not In KB> one, we want to skip the store & fourth line
    // creation but first check for user incorrectly editing and fix it
	if (!gbIsGlossing && !pCurPile->GetSrcPhrase()->m_bHasKBEntry &&
												pCurPile->GetSrcPhrase()->m_bNotInKB)
	{
        // if the user edited out the <Not In KB> entry from the KB editor, we need to put
        // it back so that the setting is preserved (the "right" way to change the setting
        // is to use the toolbar checkbox - this applies when adapting, not glossing)
		pApp->m_pKB->Fix_NotInKB_WronglyEditedOut(pCurPile);
		goto b;
	}

	if (pApp->m_targetPhrase.IsEmpty())
	{
		pApp->m_bForceAsk = FALSE; // make sure it's turned off, & allow
								   // function to continue
		if (gbIsGlossing)
			pCurPile->GetSrcPhrase()->m_gloss.Empty();
		else
			pCurPile->GetSrcPhrase()->m_adaption.Empty();
	}

    // make the punctuated target string, but only if adapting; note, for auto
    // capitalization ON, the function will change initial lower to upper as required,
    // whatever punctuation regime is in place for this particular sourcephrase instance...
    // we are about to leave the current phrase box location, so we must try to store what
    // is now in the box, if the relevant flags allow it. Test to determine which KB to
    // store to. StoreText( ) has been ammended for auto-capitalization support (July 2003)
	if (!gbIsGlossing)
	{
		pView->MakeTargetStringIncludingPunctuation(pCurPile->GetSrcPhrase(), pApp->m_targetPhrase);
		pView->RemovePunctuation(pDoc, &pApp->m_targetPhrase,from_target_text);
	}
	pApp->m_bInhibitMakeTargetStringCall = TRUE;
	if (gbIsGlossing)
		bOK = pApp->m_pGlossingKB->StoreText(pCurPile->GetSrcPhrase(), pApp->m_targetPhrase);
	else
		bOK = pApp->m_pKB->StoreText(pCurPile->GetSrcPhrase(), pApp->m_targetPhrase);
	pApp->m_bInhibitMakeTargetStringCall = FALSE;
	if (!bOK)
	{
		// restore default button image, and m_bCopySourcePunctuation to TRUE
		wxCommandEvent event;
		if (!pApp->m_bCopySourcePunctuation)
		{
			pApp->GetView()->OnToggleEnablePunctuationCopy(event);
		}
		GetLayout()->m_docEditOperationType = no_edit_op;
		return FALSE; // can't move if the storage failed
	}

b:	pDoc->ResetPartnerPileWidth(pOldActiveSrcPhrase);

	// move to next pile's cell
	CPile* pNewPile = pView->GetNextPile(pCurPile); // does not update the view's
				// m_nActiveSequNum nor m_pActivePile pointer, so update these here,
				// provided NULL was not returned

	// restore default button image, and m_bCopySourcePunctuation to TRUE
	wxCommandEvent event;
	if (!pApp->m_bCopySourcePunctuation)
	{
		pApp->GetView()->OnToggleEnablePunctuationCopy(event);
	}
	if (pNewPile == NULL)
	{
        // we deem vertical editing current step to have ended if control gets into this
        // block, so user has to be asked what to do next if vertical editing is currently
        // in progress; and we tunnel out before m_nActiveSequNum can be set to -1
        // (otherwise vertical edit will crash when recalc layout is tried with a bad sequ
        // num value)
		if (gbVerticalEditInProgress)
		{
			m_bTunnellingOut = FALSE; // ensure default value set
			bool bCommandPosted = pView->VerticalEdit_CheckForEndRequiringTransition(-1,
							nextStep, TRUE); // bForceTransition is TRUE
			if (bCommandPosted)
			{
				// don't proceed further because the current vertical edit step has ended
				m_bTunnellingOut = TRUE; // so caller can use it
				GetLayout()->m_docEditOperationType = no_edit_op;
				return FALSE;
			}
		}

		if (!pApp->m_bSingleStep)
		{
			pApp->m_bAutoInsert = FALSE; // cause halt, if auto lookup & inserting is ON
		}

		// ensure the view knows the pile pointer is no longer valid
		pApp->m_pActivePile = NULL;
		pApp->m_nActiveSequNum = -1;
		GetLayout()->m_docEditOperationType = no_edit_op;
		return FALSE; // we are at the end of the file
	}
	else // we have a pointer to the next pile
	{
		pApp->m_bUserTypedSomething = FALSE; // user has not typed at the new location yet

        // don't commit to the new pile if we are in vertical edit mode, until we've
        // checked the pile is not in the gray text area...
        // if vertical editing is currently in progress we must check if the lookup target
        // is within the editable span, if not then control has moved the box into the gray
        // area beyond the editable span and that means a step transition is warranted &
        // the user should be asked what step is next
		if (gbVerticalEditInProgress)
		{
			int m_nCurrentSequNum = pNewPile->GetSrcPhrase()->m_nSequNumber;
			m_bTunnellingOut = FALSE; // ensure default value set
			bool bCommandPosted = pView->VerticalEdit_CheckForEndRequiringTransition(
									m_nCurrentSequNum,nextStep); // bForceTransition is FALSE
			if (bCommandPosted)
			{
				// don't proceed further because the current vertical edit step has ended
				m_bTunnellingOut = TRUE; // so caller can use it
				GetLayout()->m_docEditOperationType = no_edit_op;
				return FALSE; // try returning FALSE
			}
			else
			{
				// BEW 19Oct15 No transition of vert edit modes,
				// so we can store this location on the app
				gpApp->m_vertEdit_LastActiveSequNum = m_nCurrentSequNum;
#if defined(_DEBUG)
				wxLogDebug(_T("VertEdit PhrBox, MoveToImmedNextPile() storing loc'n: %d "), m_nCurrentSequNum);
#endif
			}
		}

		// update the active sequence number, and pile pointer
		pApp->m_nActiveSequNum = pNewPile->GetSrcPhrase()->m_nSequNumber;
		pApp->m_pActivePile = pNewPile;

        // since we are moving forward from could be anywhere, the next pile is may have a
        // refString translation which is nonempty, so we have to put it into
        // m_targetPhrase so that ResizeBox will use it; but if there is none, the copy the
        // source key if the m_bCopySource flag is set, else just set it to an empty
        // string. (bNeed Modify is a helper flag used for setting/clearing the document's
        // modified flag at the end of this function)
		bool bNeedModify = FALSE; // reset to TRUE if we copy source because there was no
								  // adaptation

        // beware, next call the pRefString pointer may point to <Not In KB>, so take that
        // into account; GetRefString has been modified for auto-capitalization support
		CRefString* pRefString = NULL;
		KB_Entry rsEntry;
		if (gbIsGlossing)
			rsEntry = pApp->m_pGlossingKB->GetRefString(pNewPile->GetSrcPhrase()->m_nSrcWords,
				pNewPile->GetSrcPhrase()->m_key, pNewPile->GetSrcPhrase()->m_gloss, pRefString);
		else
			rsEntry = pApp->m_pKB->GetRefString(pNewPile->GetSrcPhrase()->m_nSrcWords,
				pNewPile->GetSrcPhrase()->m_key, pNewPile->GetSrcPhrase()->m_adaption, pRefString);
		if (pRefString != NULL && rsEntry == really_present)
		{
			pView->RemoveSelection(); // we won't do merges in this situation

			// assign the translation text - but check it's not "<Not In KB>", if it is, we
			// leave the phrase box unchanged (rather than empty as formerly), but turn OFF
			// the m_bSaveToKB flag
			wxString str = pRefString->m_translation;
			if (!gbIsGlossing && str == _T("<Not In KB>"))
			{
				pApp->m_bSaveToKB = FALSE;
				pApp->m_pTargetBox->m_bAbandonable = FALSE;
				pNewPile->GetSrcPhrase()->m_bHasKBEntry = FALSE; // ensures * shows above
															     // this srcPhrase
				pApp->m_targetPhrase = pNewPile->GetSrcPhrase()->m_targetStr;
				pNewPile->GetSrcPhrase()->m_bNotInKB = TRUE;
			}
			else
			{
				pApp->m_pTargetBox->m_bAbandonable = FALSE;
				pApp->m_targetPhrase = str;
			}

            // remove the translation from the KB, in case user wants to edit it before
            // it's stored again (RemoveRefString also clears the m_bHasKBEntry flag on the
            // source phrase)
			if (gbIsGlossing)
			{
				pApp->m_pGlossingKB->RemoveRefString(pRefString, pNewPile->GetSrcPhrase(),
											pNewPile->GetSrcPhrase()->m_nSrcWords);
				pApp->m_targetPhrase = pNewPile->GetSrcPhrase()->m_gloss;
			}
			else
			{
				pApp->m_pKB->RemoveRefString(pRefString, pNewPile->GetSrcPhrase(),
											pNewPile->GetSrcPhrase()->m_nSrcWords);
				pApp->m_targetPhrase = pNewPile->GetSrcPhrase()->m_targetStr;
			}
		}
		else // no kb entry or rsEntry == present_but_deleted
		{
			if (gbIsGlossing)
			{
				pNewPile->GetSrcPhrase()->m_bHasGlossingKBEntry = FALSE; // ensure it's
                                                // false when there is no KB entry
			}
			else
			{
				pNewPile->GetSrcPhrase()->m_bHasKBEntry = FALSE; // ensure it's false
												// when there is no KB entry
			}
            // try to get a suitable string instead from the sourcephrase itself, if that
            // fails then copy the sourcePhrase's key if the m_bCopySource flag is set
			if (gbIsGlossing)
			{
				pApp->m_targetPhrase = pNewPile->GetSrcPhrase()->m_gloss;
			}
			else
			{
				pApp->m_targetPhrase = pNewPile->GetSrcPhrase()->m_adaption;
			}
			if (pApp->m_targetPhrase.IsEmpty() && pApp->m_bCopySource)
			{
				if (!pNewPile->GetSrcPhrase()->m_bNullSourcePhrase)
				{
					pApp->m_targetPhrase = pView->CopySourceKey(
								pNewPile->GetSrcPhrase(),pApp->m_bUseConsistentChanges);
					bNeedModify = TRUE;
				}
				else
				{
					pApp->m_targetPhrase.Empty();
				}
			}
		}
		ChangeValue(pApp->m_targetPhrase); // initialize the phrase box too, so it doesn't
										// carry an old string to the next pile's cell

		// get the new active pile
		pApp->m_pActivePile = pView->GetPile(pApp->m_nActiveSequNum);
		wxASSERT(pApp->m_pActivePile != NULL);

        // get an adjusted pile pointer for the new active location, and we want the
        // pile's strip to be marked as invalid and the strip index added to the
        // CLayout::m_invalidStripArray
		if (pNewPile != NULL)
		{
			pDoc->ResetPartnerPileWidth(pNewPile->GetSrcPhrase());
		}

		// if the user has turned on the sending of synchronized scrolling messages
		// send the relevant message, a sync scroll is appropriate now, provided
		// reviewing mode is ON when the MoveToImmedNextPile() -- which is likely as this
		// latter function is only called when in reviewing mode (in wx and legacy
		// versions, this scripture ref message was not sent here, and should have been)
		if (!pApp->m_bIgnoreScriptureReference_Send && !pApp->m_bDrafting)
		{
			pView->SendScriptureReferenceFocusMessage(pApp->m_pSourcePhrases,
													pApp->m_pActivePile->GetSrcPhrase());
		}

		// recreate the phraseBox using the stored information
		pApp->m_nStartChar = -1; pApp->m_nEndChar = -1;
		GetLayout()->m_docEditOperationType = relocate_box_op;

		// fix the m_bSaveToKB flag, depending on whether or not srcPhrase is in KB
		if (!gbIsGlossing && !pApp->m_pActivePile->GetSrcPhrase()->m_bHasKBEntry &&
										pApp->m_pActivePile->GetSrcPhrase()->m_bNotInKB)
		{
			pApp->m_bSaveToKB = FALSE;
		}
		else
		{
			pApp->m_bSaveToKB = TRUE;
		}

		// BEW note 30Mar09: later we may set clipping here or somewhere
		pView->Invalidate(); // I think this call is needed
		GetLayout()->PlaceBox();

		if (bNeedModify)
			SetModify(TRUE); // our own SetModify(); calls MarkDirty()
		else
			SetModify(FALSE); // our own SetModify(); calls DiscardEdits()

		return TRUE;
	}
}

// This OnSysKeyUp() function is not called by any EVT_ handler in wxWidgets. It is only
// called by the OnKeyUp() handler when it detects the use of ALT, SHIFT, or CTRL used in
// simultaneous combination with other keys.
// whm 10Jan2018 modified key handling so that ALL AltDown(), ShiftDown(), and ControlDown()
// events are now sent to this OnSysKeyUp() function for processing.
// whm 16Feb2018 Notes: OnSysKeyUp() currently does the following key handling:
// 1. Just calls return if m_bReadOnlyAccess is TRUE
// 2. Handles ALT+ENTER to Make a Phrase if not glossing and selection count > 1.
// 3. Handles ALT+BACKSPACE to advance phrasebox without KB lookup if transliteration mode in ON
//    and not glossing.
// 4. Handles ALT+BACKSPACE if transliteration mode in OFF with error message to user.
// 5. Handles ALT+RIGHT to extent selection right if not glossing.
// 6. Handles ALT+LEFT to extent selection left if not glossing.
// 7. Handles ALT+UP to summon the retranslation dialog via DoRetranslationByUpArrow().
// 8. Note: ALT+DOWN no longer handled (to insert placeholder BEFORE) in our key handlers because 
//    ALT+DOWN is the built in hot key to make the dropdown list open/close. See SHIFT+ALT+DOWN below.
// 9. Handle ALT+DELETE to un-merge a current merger into separate words, when not glossing.
// 10. Handle SHIFT+UP to scroll the screen up about 1 strip. A simple WXK_UP cannot be used anymore
//     because it is reserved to move the highlight in the dropdown list.
// 11. Handle SHIFT+DOWN to scroll the screen down about 1 strip. A simple WXK_DOWN cannot be used 
//     anymore because it is reserved to move the highlight in the dropdown list.
// 12. Handle SHIFT+PAGEUP to scroll the screen up about a screen full. A simple WXK_PAGEUP cannot
//     be used anymore because it is reserved to move the highlight in the dropdown list.
// 13. Handle SHIFT+PAGEDOWN to scroll the screen down about a screen full. A simple WXK_PAGEDOWN
//     cannot be used anymore because it is reserved to move the highlight in the dropdown list.
// 14. Handle SHIFT+ALT+DOWN or SHIFT+CTRL+DOWN to add a placeholder BEFORE the phrasebox location.
//     Previously this was just ALT+DOWN, but ALT+DOWN is now reserved to toggle the dropdown list 
//     open/closed.
// 15. Handle SHIFT+CTRL+SPACE to enter a ZWSP (zero width space) into the composebar's editbox; 
//     replacing a selection if there is one defined.
// 16. Handle CTRL+DOWN to insert placeholder AFTER the phrasebox location (not on a Mac)
//     TODO: Add CTRL+ALT+DOWN as alternate???
// 17. Handle CTRL+ENTER to Jump Forward when transliteration, or warning message if not 
//     transliteration is not turned ON.
//
// Note: Put in this OnSysKeyUp() handler custom key handling that involve simultaneous use 
// of ALT, SHIFT, or CTRL keys.
// BEW 13Apr10, no changes needed for support of doc version 5
void CPhraseBox::OnSysKeyUp(wxKeyEvent& event)
{
	// wx Note: This routine handles Adapt It's AltDown key events
	// and CmdDown events (= ControlDown on PCs; Apple Command key events on Macs).
	CAdapt_ItApp* pApp = &wxGetApp();
	wxASSERT(pApp != NULL);
	CAdapt_ItView* pView = (CAdapt_ItView*) pApp->GetView();
	wxASSERT(pView->IsKindOf(CLASSINFO(CAdapt_ItView)));
	long nStart;
	long nEnd;

	bool bTRUE = FALSE;
	if (event.AltDown())// || event.CmdDown()) // CmdDown is same as ControlDown on PCs; Apple Command key on Macs.
	{
		// whm added 26Mar12. Don't allow AltDown + key events when in read-only mode
		if (pApp->m_bReadOnlyAccess)
		{
			return;
		}

		// ALT key or Control/Command key is down
		if (event.AltDown() && event.GetKeyCode() == WXK_RETURN) // ALT+ENTER
		{
			// ALT key is down, and <RET> was nChar typed (ie. 13), so invoke the
			// code to turn selection into a phrase; but if glossing is ON, then
			// merging must not happen - in which case exit early
			if (gbIsGlossing || !(pApp->m_selection.GetCount() > 1))
				return;
			pView->MergeWords();
			GetLayout()->m_docEditOperationType = merge_op;

			// select the lot
			SetSelection(-1,-1);// -1,-1 selects all
			pApp->m_nStartChar = -1;
			pApp->m_nEndChar = -1;

			// set old sequ number in case required for toolbar's Back button - in this case
			// it may have been a location which no longer exists because it was absorbed in
			// the merge, so set it to the merged phrase itself
            pApp->m_nOldSequNum = pApp->m_nActiveSequNum;
			return;
		}

		// BEW added 19Apr06 to allow Bob Eaton to advance phrasebox without having any lookup of the KB done.
		// We want this to be done only for adapting mode, and provided transliteration mode is turned on
		if (!gbIsGlossing && pApp->m_bTransliterationMode && event.GetKeyCode() == WXK_BACK) // ALT+BACKSPACE (transliterating)
		{
			// save old sequ number in case required for toolbar's Back button
            pApp->m_nOldSequNum = pApp->m_nActiveSequNum;

			m_bSuppressStoreForAltBackspaceKeypress = TRUE; // suppress store to KB for this move of box
			// the value is restored to FALSE in MoveToImmediateNextPile()

			// do the move forward to next empty pile, with lookup etc, but no store due to
			// the m_bSuppressStoreForAltBackspaceKeypress global being TRUE until the StoreText()
			// call is jumped over in the MoveToNextPile() call within JumpForward()
			JumpForward(pView);
			return;
		}
		else if (!gbIsGlossing && !pApp->m_bTransliterationMode && event.GetKeyCode() == WXK_BACK) // ALT+BACKSPACE (not transliterating)
		{
			// Alt key down & Backspace key hit, so user wanted to initiate a transliteration
			// advance of the phrase box, with its special KB storage mode, but forgot to turn the
			// transliteration mode on before using this feature, so warn him to turn it on and then
			// do nothing
			//IDS_TRANSLITERATE_OFF
			wxMessageBox(_("Transliteration mode is not yet turned on."),_T(""),wxICON_EXCLAMATION | wxOK);

			// restore focus to the phrase box
			if (pApp->m_pTargetBox != NULL)
				if (pApp->m_pTargetBox->IsShown())
					pApp->m_pTargetBox->SetFocus();
			return;
		}

		// ALT key is down, if an arrow key pressed, extend/retract sel'n left or right
		// or insert a null source phrase, or open the retranslation dialog; but don't
		// allow any of this (except Alt + Backspace) if glossing is ON - in those cases, just return
		if (gbIsGlossing)
			return;
        GetTextCtrl()->GetSelection(&nStart,&nEnd);
		if (event.GetKeyCode() == WXK_RIGHT) // ALT+RIGHT
		{
			if (gbRTL_Layout)
				bTRUE = pView->ExtendSelectionLeft();
			else
				bTRUE = pView->ExtendSelectionRight();
			if(!bTRUE)
			{
				if (gbVerticalEditInProgress)
				{
					::wxBell();
				}
				else
				{
					// did not succeed - do something eg. warn user he's collided with a boundary
					// IDS_RIGHT_EXTEND_FAIL
					wxMessageBox(_("Sorry, you cannot extend the selection that far to the right unless you also use one of the techniques for ignoring boundaries."),_T(""), wxICON_INFORMATION | wxOK);
				}
			}
			SetFocus();
			SetSelection(nStart,nEnd);
			pApp->m_nStartChar = nStart;
			pApp->m_nEndChar = nEnd;
		}
		else if (event.GetKeyCode() == WXK_LEFT) // ALT+LEFT
		{
			if (gbRTL_Layout)
				bTRUE = pView->ExtendSelectionRight();
			else
				bTRUE = pView->ExtendSelectionLeft();
			if(!bTRUE)
			{
				if (gbVerticalEditInProgress)
				{
					::wxBell();
				}
				else
				{
					// did not succeed, so warn user
					// IDS_LEFT_EXTEND_FAIL
					wxMessageBox(_("Sorry, you cannot extend the selection that far to the left unless you also use one of the techniques for ignoring boundaries. "), _T(""), wxICON_INFORMATION | wxOK);
				}
			}
			SetFocus();
			SetSelection(nStart,nEnd);
			pApp->m_nStartChar = nStart;
			pApp->m_nEndChar = nEnd;
		}
		else if (event.GetKeyCode() == WXK_UP) // ALT+UP (also available with CTRL+R or ToolBar Button on current selection)
		{
			// up arrow was pressed, so get the retranslation dialog open
			if (pApp->m_pActivePile == NULL)
			{
				return;
				//goto a;
			}
			if (pApp->m_selectionLine != -1)
			{
				// if there is at least one source phrase with a selection defined,
				// then then use the selection and put up the dialog
				pApp->GetRetranslation()->DoRetranslationByUpArrow();
			}
			else
			{
				// no selection, so make a selection at the phrase box and invoke the
				// retranslation dialog on it
				//CCell* pCell = pApp->m_pActivePile->m_pCell[2];
				CCell* pCell = pApp->m_pActivePile->GetCell(1);
				wxASSERT(pCell);
				pApp->m_selection.Append(pCell);
				//pApp->m_selectionLine = 1;
				pApp->m_selectionLine = 0;// in refactored layout, src text line is always index 0
				wxUpdateUIEvent evt;
				//pView->OnUpdateButtonRetranslation(evt);
				// whm Note: calling OnUpdateButtonRetranslation(evt) here doesn't work because there
				// is not enough idle time for the Do A Retranslation toolbar button to be enabled
				// before the DoRetranslationByUpArrow() call below executes - which has code in it
				// to prevent the Retranslation dialog from poping up unless the toolbar button is
				// actually enabled. So, we explicitly enable the toolbar button here instead of
				// waiting for it to be done in idle time.
				CMainFrame* pFrame = pApp->GetMainFrame();
				wxASSERT(pFrame != NULL);
				wxAuiToolBarItem *tbi;
				tbi = pFrame->m_auiToolbar->FindTool(ID_BUTTON_RETRANSLATION);
				// Return if the toolbar item is hidden
				if (tbi == NULL)
				{
					return;
				}
				// enable the toolbar button
				pFrame->m_auiToolbar->EnableTool(ID_BUTTON_RETRANSLATION, true);
				pApp->GetRetranslation()->DoRetranslationByUpArrow();
			}
		}

		// BEW added 26Sep05, to implement Roland Fumey's request that the shortcut for unmerging
		// not be SHIFT+End as in the legacy app, but something else; so I'll make it ALT+Delete
		// and then SHIFT+End can be used for extending the selection in the phrase box's CEdit
		// to the end of the box contents (which is Windows standard behaviour, & what Roland wants)
		if (event.GetKeyCode() == WXK_DELETE) // ALT+DELETE
		{
			// we have ALT + Delete keys held down, so unmerge the current merger - separating into
			// individual words but only when adapting, if glossing we instead just return
			CSourcePhrase* pSP;
			if (pApp->m_pActivePile == NULL)
			{
				return;
			}
			if (pApp->m_selectionLine != -1 && pApp->m_selection.GetCount() == 1)
			{
				CCellList::Node* cpos = pApp->m_selection.GetFirst();
				CCell* pCell = cpos->GetData();
				pSP = pCell->GetPile()->GetSrcPhrase();
				if (pSP->m_nSrcWords == 1)
					return;
			}
			else if (pApp->m_selectionLine == -1 && pApp->m_pTargetBox->GetHandle() != NULL
									&& pApp->m_pTargetBox->IsShown())
			{
				pSP = pApp->m_pActivePile->GetSrcPhrase();
				if (pSP->m_nSrcWords == 1)
					return;
			}
			else
			{
				return;
			}

			// if we get to here, we can go ahead & remove the merger, if we are adapting
			// but if glossing, the user should be explicitly warned the op is no available
			if (gbIsGlossing)
			{
				// IDS_NOT_WHEN_GLOSSING
				wxMessageBox(_("This particular operation is not available when you are glossing."),
				_T(""),wxICON_INFORMATION | wxOK);
				return;
			}
			pView->UnmergePhrase(); // calls OnButtonRestore() - which will attempt to do a lookup,
				// so don't remake the phrase box with the global (wxString) m_SaveTargetPhrase,
				// otherwise it will override a successful lookkup and make the ALT+Delete give
				// a different result than the Unmerge button on the toolbar. So we in effect
				// are ignoring m_SaveTargetPhrase (the former is used in PlacePhraseBox() only
			GetLayout()->m_docEditOperationType = unmerge_op;

			// save old sequ number in case required for toolbar's Back button - the only safe
			// value is the first pile of the unmerged phrase, which is where the phrase box
			// now is
            pApp->m_nOldSequNum = pApp->m_nActiveSequNum;
		}
	} // end of if (event.AltDown())

    // whm 15Feb2018 added to process new SHIFT down scrolling behaviors (that previously were
    // processed as plain arrow keys.
    // The reason for adding ShiftDown() to the arrow and page scrolling key operations is that
    // the dropdown control needs to use the normal/unmodified arrow and page keys for use within
    // the dropdown control. Hence, scrolling operations using the keyboard now require the 
    // simultaneous holding down of the SHIFT key.
    else if (event.ShiftDown())
    {
        // does user want to unmerge a phrase?

        // user did not want to unmerge, so must want a scroll
        // preserve cursor location (its already been moved by the time this function is entered)
        int nScrollCount = 1;

        // the code below for up arrow or down arrow assumes a one strip + leading scroll. If
        // we change so as to scroll more than one strip at a time (as when pressing page up or
        // page down keys), iterate by the number of strips needing to be scrolled
        CLayout* pLayout = GetLayout();
        int yDist = pLayout->GetStripHeight() + pLayout->GetCurLeading();

        // do the scroll (CEdit also moves cursor one place to left for uparrow, right for
        // downarrow hence the need to restore the cursor afterwards; however, the values of
        // nStart and nEnd depend on whether the app made the selection, or the user; and also
        // on whether up or down was pressed: for down arrow, the values will be either 1,1 or
        // both == length of line, for up arrow, values will be either both = length of line
        // -1; or 0,0 so code accordingly
        bool bScrollFinished = FALSE;
        int nCurrentStrip;
        int nLastStrip;

        if (event.GetKeyCode() == WXK_UP) // SHIFT+UP
        {
        a:	int xPixelsPerUnit, yPixelsPerUnit;
            // whm 14Feb2018 with CPhraseBox based on wxOwnerDrawnComboBox, we need to reserve the
            // WXK_UP key for scrolling up in the combo box list, rather than scrolling the canvas.
#ifdef Do_Clipping
            pLayout->SetScrollingFlag(TRUE); // need full screen drawing, so clipping can't happen
#endif
            pLayout->m_pCanvas->GetScrollPixelsPerUnit(&xPixelsPerUnit, &yPixelsPerUnit);
            wxPoint scrollPos;
            // MFC's GetScrollPosition() "gets the location in the document to which the upper
            // left corner of the view has been scrolled. It returns values in logical units."
            // wx note: The wx docs only say of GetScrollPos(), that it
            // "Returns the built-in scrollbar position."
            // I assume this means it gets the logical position of the upper left corner, but
            // it is in scroll units which need to be converted to device (pixel) units

            pLayout->m_pCanvas->CalcUnscrolledPosition(0, 0, &scrollPos.x, &scrollPos.y);
            // the scrollPos point is now in logical pixels from the start of the doc

            if (scrollPos.y > 0)
            {
                if (scrollPos.y >= yDist)
                {
                    // up uparrow was pressed, so scroll up a strip, provided we are not at the
                    // start of the document; and we are more than one strip + leading from the start,
                    // so it is safe to scroll
                    pLayout->m_pCanvas->ScrollUp(1);
                }
                else
                {
                    // we are close to the start of the document, but not a full strip plus leading,
                    // so do a partial scroll only - otherwise phrase box will be put at the wrong
                    // place
                    yDist = scrollPos.y;
                    scrollPos.y -= yDist;

                    int posn = scrollPos.y;
                    posn = posn / yPixelsPerUnit;
                    // Note: MFC's ScrollWindow's 2 params specify the xAmount and yAmount to
                    // scroll in device units (pixels). The equivalent in wx is Scroll(x,y) in
                    // which x and y are in SCROLL UNITS (pixels divided by pixels per unit).
                    // Also MFC's ScrollWindow takes parameters whose value represents an
                    // "amount" to scroll from the current position, whereas the
                    // wxScrolledWindow::Scroll takes parameters which represent an absolute
                    // "position" in scroll units. To convert the amount we need to add the
                    // amount to (or subtract from if negative) the logical pixel unit of the
                    // upper left point of the client viewing area; then convert to scroll
                    // units in Scroll().
                    pLayout->m_pCanvas->Scroll(0, posn); //pView->ScrollWindow(0,yDist);
                    Refresh();
                    bScrollFinished = TRUE;
                }
            }
            else
            {
                ::wxBell();
                bScrollFinished = TRUE;
            }

            if (bScrollFinished)
                goto c;
            else
            {
                --nScrollCount;
                if (nScrollCount == 0)
                    goto c;
                else
                {
                    goto a;
                }
            }
            return;
            
            // restore cursor location when done
        c:	SetFocus();
            SetSelection(pApp->m_nStartChar, pApp->m_nEndChar);
            return;
        }
        else if (event.GetKeyCode() == WXK_DOWN) // SHIFT+DOWN
        {
            // down arrow was pressed, so scroll down a strip, provided we are not at the end of
            // the bundle
        b:	wxPoint scrollPos;

            // whm 14Feb2018 with CPhraseBox based on wxOwnerDrawnComboBox, we need to reserve the
            // WXK_DOWN key for scrolling down in the combo box list, rather than scrolling the canvas.
#ifdef Do_Clipping
            pLayout->SetScrollingFlag(TRUE); // need full screen drawing, so clipping can't happen
#endif
            int xPixelsPerUnit, yPixelsPerUnit;
            pLayout->m_pCanvas->GetScrollPixelsPerUnit(&xPixelsPerUnit, &yPixelsPerUnit);
            // MFC's GetScrollPosition() "gets the location in the document to which the upper
            // left corner of the view has been scrolled. It returns values in logical units."
            // wx note: The wx docs only say of GetScrollPos(), that it
            // "Returns the built-in scrollbar position."
            // I assume this means it gets the logical position of the upper left corner,
            // but it is in scroll units which need to be converted to device (pixel) units

            wxSize maxDocSize; // renaming barSizes to maxDocSize for clarity
            pLayout->m_pCanvas->GetVirtualSize(&maxDocSize.x, &maxDocSize.y); // gets size in pixels

            pLayout->m_pCanvas->CalcUnscrolledPosition(0, 0, &scrollPos.x, &scrollPos.y);
            // the scrollPos point is now in logical pixels from the start of the doc

            wxRect rectClient(0, 0, 0, 0);
            wxSize canvasSize;
            canvasSize = pApp->GetMainFrame()->GetCanvasClientSize();
            rectClient.width = canvasSize.x;
            rectClient.height = canvasSize.y;

            int logicalViewBottom = scrollPos.y + rectClient.GetBottom();
            if (logicalViewBottom < maxDocSize.GetHeight())
            {
                if (logicalViewBottom <= maxDocSize.GetHeight() - yDist)
                {
                    // a full strip + leading can be scrolled safely
                    pLayout->m_pCanvas->ScrollDown(1);
                }
                else
                {
                    // we are close to the end, but not a full strip + leading can be scrolled, so
                    // just scroll enough to reach the end - otherwise position of phrase box will
                    // be set wrongly
                    wxASSERT(maxDocSize.GetHeight() >= logicalViewBottom);
                    yDist = maxDocSize.GetHeight() - logicalViewBottom; // make yDist be what's
                                                                        // left to scroll
                    scrollPos.y += yDist;

                    int posn = scrollPos.y;
                    posn = posn / yPixelsPerUnit;
                    pLayout->m_pCanvas->Scroll(0, posn);
                    Refresh();
                    bScrollFinished = TRUE;
                }
            }
            else
            {
                bScrollFinished = TRUE;
                ::wxBell();
            }

            if (bScrollFinished)
                goto d;
            else
            {
                --nScrollCount;
                if (nScrollCount == 0)
                    goto d;
                else
                {
                    goto b;
                }
            }
            return;
            
            // restore cursor location when done
        d:	SetFocus();
            SetSelection(pApp->m_nStartChar, pApp->m_nEndChar);
            return;
        }
        else if (event.GetKeyCode() == WXK_PAGEUP) // SHIFT+PAGEUP
        {
            // Note: an overload of CLayout::GetVisibleStripsRange() does the same job, so it
            // could be used instead here and for the other instance in next code block - as
            // these two calls are the only two calls of the view's GetVisibleStrips() function
            // in the whole application ** TODO ** ??
            pView->GetVisibleStrips(nCurrentStrip, nLastStrip);
            nScrollCount = nLastStrip - nCurrentStrip;
            goto a;
        }
        else if (event.GetKeyCode() == WXK_PAGEDOWN) // SHIFT+PAGEDOWN
        {
            // Note: an overload of CLayout::GetVisibleStripsRange() does the same job, so it
            // could be used instead here and for the other instance in above code block - as
            //6 these two calls are the only two calls of the view's GetVisibleStrips() function
            // in the whole application ** TODO ** ??
            pView->GetVisibleStrips(nCurrentStrip, nLastStrip);
            nScrollCount = nLastStrip - nCurrentStrip;
            goto b;
        }
    }

    if ((event.ShiftDown() && event.ControlDown() && event.GetKeyCode() == WXK_DOWN) || // SHIFT+CTRL+DOWN or 
        (event.ShiftDown() && event.AltDown() && event.GetKeyCode() == WXK_DOWN))       // SHIFT+ALT+DOWN - previously was just ALT + DOWN
    {
        // Add Placeholder BEFORE PhraseBox position
        // whm Note 12Feb09: Control passes through here when a simultaneous Ctrl-Alt-Down press is
        // released. This equates to a Command-Option-Down combination, which is acceptable and
        // doesn't conflict with any reserved keys on a Mac. If only Ctrl-Down (Command-Down on a
        // Mac) is pressed, control does NOT pass through here, but through the WXK_DOWN block of
        // OnKeyUp().
        //
        // Insert of null sourcephrase but first save old sequ number in case required for toolbar's
        // Back button (this one is activated when CTRL key is not down, so it does the
        // default "insert before" case; the "insert after" case is done in the
        // OnKeyUp() handler)

        // Bill wanted the behaviour modified, so that if the box's m_bAbandonable flag is TRUE
        // (ie. a copy of source text was done and nothing typed yet) then the current pile
        // would have the box contents abandoned, nothing put in the KB, and then the placeholder
        // inserion - the advantage of this is that if the placeholder is inserted immediately
        // before the phrasebox's location, then after the placeholder text is typed and the user
        // hits ENTER to continue looking ahead, the former box location will get the box and the
        // copy of the source redone, rather than the user usually having to edit out an unwanted
        // copy from the KB, or remember to clear the box manually. A sufficient thing to do here
        // is just to clear the box's contents.
        if (pApp->m_pTargetBox->m_bAbandonable)
        {
            pApp->m_targetPhrase.Empty();
            if (pApp->m_pTargetBox != NULL
                && (pApp->m_pTargetBox->IsShown()))
            {
                pApp->m_pTargetBox->ChangeValue(_T(""));
            }
        }

        // now do the 'insert before'
        pApp->m_nOldSequNum = pApp->m_nActiveSequNum;
        pApp->GetPlaceholder()->InsertNullSrcPhraseBefore();
    }

    if (event.ShiftDown() && event.ControlDown() && event.GetKeyCode() == WXK_SPACE) // SHIFT+CTRL+SPACE
    {
        // BEW 8Jul14 intercept the CTRL+SHIFT+<spacebar> combination to enter a ZWSP
        // (zero width space) into the composebar's editbox; replacing a selection if
        // there is one defined
        OnCtrlShiftSpacebar(this->GetTextCtrl()); // see helpers.h & .cpp // whm 14Feb2018 added ->GetTextCtrl()
        return; // don't call skip - we don't want the end-of-line character entered
                // into the edit box
    }

    if (event.ControlDown() && event.GetKeyCode() == WXK_DOWN) // CTRL+DOWN
    {
        // whm added 26Mar12
        if (pApp->m_bReadOnlyAccess)
        {
            // Disable the Ctrl + down arrow invocation of the insert after of a null srcphrase
            return;
        }
        // CTRL + down arrow was pressed - asking for an "insert after" of a null
        // srcphrase. 
        // whm 12Feb09 Note: Ctrl + Down (equates to Command-Down on a Mac) conflicts
        // with a Mac's system key assignment to "Move focus to another value/cell
        // within a view such as a table", so we'll prevent Ctrl+Down from calling
        // InsertNullSrcPhraseAfter() on the Mac port.
#ifndef __WXMAC__
        // first save old sequ number in case required for toolbar's Back button
        // If glossing is ON, we don't allow the insertion, and just return instead
        pApp->m_nOldSequNum = pApp->m_nActiveSequNum;
        if (!gbIsGlossing)
            pApp->GetPlaceholder()->InsertNullSrcPhraseAfter();
#endif
        return;
    }

    if (event.ControlDown() && event.GetKeyCode() == WXK_RETURN) // CTRL+ENTER
    {
        if (!gbIsGlossing && pApp->m_bTransliterationMode)
        {
            // CTRL + ENTER is a JumpForward() to do transliteration; bleed this possibility
            // out before allowing for any keypress to halt automatic insertion; one side
            // effect is that MFC rings the bell for each such key press and I can't find a way
            // to stop it. So Alt + Backspace can be used instead, for the same effect; or the
            // sound can be turned off at the keyboard if necessary. This behaviour is only
            // available when transliteration mode is turned on.

            // save old sequ number in case required for toolbar's Back button
            pApp->m_nOldSequNum = pApp->m_nActiveSequNum;

            m_bSuppressStoreForAltBackspaceKeypress = TRUE; // suppress store to KB for
                                                           // this move of box, the value is restored to FALSE in MoveToNextPile()

            // do the move forward to next empty pile, with lookup etc, but no store due to
            // the m_bSuppressStoreForAltBackspaceKeypress global being TRUE until the StoreText()
            // call is jumped over in the MoveToNextPile() call within JumpForward()
            JumpForward(pView);
            return;
        }
        else if (!gbIsGlossing && !pApp->m_bTransliterationMode)
        {
            // user wanted to initiate a transliteration advance of the phrase box, with its
            // special KB storage mode, but forgot to turn the transliteration mode on before
            // using this feature, so warn him to turn it on and then do nothing
            // IDS_TRANSLITERATE_OFF
            wxMessageBox(_("Transliteration mode is not yet turned on."), _T(""), wxICON_EXCLAMATION | wxOK);

            // restore focus to the phrase box
            if (pApp->m_pTargetBox != NULL)
                if (pApp->m_pTargetBox->IsShown())
                    pApp->m_pTargetBox->SetFocus();
            return; // whm 16Feb2018 added 
        }
    }
    // whm Note: no event.Skip() from OnSysKeyUP()
}

// return TRUE if we traverse this function without being at the end of the file, or
// failing in the LookAhead function (such as when there was no match); otherwise, return
// FALSE so as to be able to exit from the caller's loop
// BEW 13Apr10, no changes needed for support of doc version 5
// BEW 21May15 added freeze/thaw support

bool CPhraseBox::OnePass(CAdapt_ItView *pView)
{
	#ifdef _FIND_DELAY
		wxLogDebug(_T("1. Start of OnePass"));
	#endif
	CAdapt_ItApp* pApp = &wxGetApp();
	wxASSERT(pApp != NULL);

	CLayout* pLayout = GetLayout();
	//CSourcePhrase* pOldActiveSrcPhrase = NULL; // set but not used
	int nActiveSequNum = pApp->m_nActiveSequNum;
	if (nActiveSequNum < 0)
		return FALSE;
	//else
	//{
	//	pOldActiveSrcPhrase = (pView->GetPile(nActiveSequNum))->GetSrcPhrase();
	//}
	// save old sequ number in case required for toolbar's Back button
    pApp->m_nOldSequNum = nActiveSequNum;
	m_bBoxTextByCopyOnly = FALSE; // restore default setting

	// BEW 21May15 add here support for freezing the canvas for a NUMINSERTS number
	// of consecutive auto-inserts from the KB; the matching thaw calls are done
	// later below, before returning FALSE, or returning TRUE. The actual Freeze()
	// call is within CLayout's Redraw() and Draw() functions, and is managed by
	// the boolean m_bDoFreeze. Recall that OnePass() is called
	// from only one place in the whole app - from OnIdle(), and even then only when
	// several flags have certain values consistent with automatic insertions being
	// currently permitted. So the logic for doing a freeze and doing it's matching
	// thaw, is encapsulated within this one OnePass() function, except for an added
	// else block in OnIdle() where the m_nInsertCount value is defaulted to 0 whenever
	// m_bAutoInsert is FALSE.
	if (pApp->m_bSupportFreeze)
	{
		if (pApp->m_nInsertCount == 0 || (pApp->m_nInsertCount % (int)NUMINSERTS == 0))
		{
			// Make the freeze happen only when the felicity conditions are satisfied
			if (
				!pApp->m_bIsFrozen      // canvas must not currently be frozen
				&& pApp->m_bDrafting    // the GUI is in drafting mode (only then are auto-inserts possible)
				&& (pApp->m_bAutoInsert || !pApp->m_bSingleStep) // one or both of these conditions apply
				)
			{
				// Ask for the freeze
				pApp->m_bDoFreeze = TRUE;
				// Count this call of OnePass()
				pApp->m_nInsertCount = 1;
				// Do a half-second delay, if that was set to 201 ticks
				if (pApp->m_nCurDelay == 31)
				{
					// DoDelay() at the doc end causes DoDelay() to never exit, so
					// protect from calling near the app end
					int maxSN = pApp->m_pSourcePhrases->GetCount() - 1;
					int safeLocSN = maxSN - (int)NUMINSERTS;
					if (safeLocSN < 0)
					{
						safeLocSN = 0;
					}
					if (maxSN > 0 && nActiveSequNum <= safeLocSN)
					{
						DoDelay(); // see helpers.cpp
						pApp->m_nCurDelay = 0; // restore to zero, we want it only the once
						// at the start of each subsection of inserts
					}
				}
			}
			else
			{
				// Ensure it's not asked for
				pApp->m_bDoFreeze = FALSE;
			}
		}
		else if (pApp->m_bIsFrozen && pApp->m_nInsertCount < (int)NUMINSERTS)
		{
			// The canvas is frozen, and we've not yet halted the auto-inserts, nor
			// have we reached the final OnePass() call of this subsequence, so
			// increment the count (the final call of a subsequence should be done
			// with the canvas having just been thawed)
			pApp->m_nInsertCount++;
		}
	}
	int bSuccessful;
	if (pApp->m_bTransliterationMode && !gbIsGlossing)
	{
		bSuccessful = MoveToNextPile_InTransliterationMode(pApp->m_pActivePile);
	}
	else
	{
		#ifdef _FIND_DELAY
			wxLogDebug(_T("2. Before MoveToNextPile"));
		#endif
		bSuccessful = MoveToNextPile(pApp->m_pActivePile);
		#ifdef _FIND_DELAY
			wxLogDebug(_T("3. After MoveToNextPile"));
		#endif
		// If in vertical edit mode, and the phrasebox has come to a hole, then we
		// want to halt OnePass() calls in OnIdle() so that the user gets the chance
		// to adapt at the hole
		if (gbVerticalEditInProgress && bSuccessful)
		{
			// m_pActivePile will be at the hole
			CPile* pNewPile = pApp->m_pActivePile;
			CSourcePhrase* pSrcPhrase = pNewPile->GetSrcPhrase();
			if (pSrcPhrase->m_adaption.IsEmpty())
			{
				// make sure the old location's values are not carried to this location
				m_Translation.Empty();
				pApp->m_targetPhrase.Empty();
				ChangeValue(pApp->m_targetPhrase);
				// make OnIdle() halt auto-inserting
				pApp->m_bAutoInsert = FALSE;
			}
		}
	}
	if (!bSuccessful)
	{
		// BEW added 4Sep08 in support of transitioning steps within vertical edit mode
		if (gbVerticalEditInProgress && m_bTunnellingOut)
		{
			// MoveToNextPile might fail within the editable span while vertical edit is
			// in progress, so we have to allow such a failure to not cause tunnelling out;
			// hence we use the m_bTunnellingOut global to assist - it is set TRUE only when
			// VerticalEdit_CheckForEndRequiringTransition() in the view class returns TRUE,
			// which means that a PostMessage(() has been done to initiate a step transition
			m_bTunnellingOut = FALSE; // caller has no need of it, so clear to default value
			pLayout->m_docEditOperationType = no_edit_op;
			return FALSE; // caller is OnIdle(), OnePass is not used elsewhere
		}


		// it will have failed because we are at eof without finding a "hole" at which to
		// land the phrase box for reaching the document's end, so we must handle eof state
		//if (pApp->m_pActivePile == NULL && pApp->m_endIndex < pApp->m_maxIndex)
		if (pApp->m_pActivePile == NULL || pApp->m_nActiveSequNum == -1)
		{
			// we got to the end of the doc...

			// BEW changed 9Apr12, support discontinuous auto-inserted spans highlighting
			//pLayout->ClearAutoInsertionsHighlighting(); <- I'm trying no call here,
			// hoping that JumpForward() will suffice, so that OnIdle()'s call of OnePass()
			// twice at the end of doc won't clobber the highlighting already established.
            // YES!! That works - the highlighting is now visible when the box has
            // disappeared and the end of doc message shows. Also, normal adapting still
            // works right despite this change, so that's a bug (or undesireable feature -
            // namely, the loss of highlighting when the doc is reached by auto-inserting)
            // now fixed.

			// BEW 21May13, first place for doing a thaw...
			if (pApp->m_bSupportFreeze && pApp->m_bIsFrozen)
			{
				// We want Thaw() done, and the next call of OnPass() will then get the view 
				// redrawn, and the one after that (if the sequence has not halted) will start
				// a new subsequence of calls where the canvas has been re-frozen 
				pApp->m_nInsertCount = 0;
				pView->canvas->Thaw();
				pApp->m_bIsFrozen = FALSE;
				// don't need a delay here
				if (pApp->m_nCurDelay == 31)
				{
					pApp->m_nCurDelay = 0; // set back to zero
				}
			}
			// remove highlight before MessageBox call below
			//pView->Invalidate();
			pLayout->Redraw(); // bFirstClear is default TRUE
			pLayout->PlaceBox();

			// tell the user EOF has been reached
			m_bCameToEnd = TRUE;
			wxStatusBar* pStatusBar;
			CMainFrame* pFrame = (CMainFrame*)pView->GetFrame();
			if (pFrame != NULL)
			{
				pStatusBar = ((CMainFrame*)pFrame)->m_pStatusBar;
				wxASSERT(pStatusBar != NULL);
				wxString str;
				if (gbIsGlossing)
					//IDS_FINISHED_GLOSSING
					str = _("End of the file; nothing more to gloss.");
				else
					//IDS_FINISHED_ADAPTING
					str = _("End of the file; nothing more to adapt.");
				pStatusBar->SetStatusText(str,0);
			}
			// we are at EOF, so set up safe end conditions
			// wxWidgets version Hides the target box rather than destroying it
			pApp->m_pTargetBox->Hide(); // whm added 12Sep04 // MFC version calls DestroyWindow
			pApp->m_pTargetBox->ChangeValue(_T("")); // need to set it to null str since it won't get recreated
			pApp->m_pTargetBox->Enable(FALSE); // whm added 12Sep04
			pApp->m_targetPhrase.Empty();
			pApp->m_nActiveSequNum = -1;

#ifdef _NEW_LAYOUT
			#ifdef _FIND_DELAY
				wxLogDebug(_T("4. Before RecalcLayout"));
			#endif
			pLayout->RecalcLayout(pApp->m_pSourcePhrases, keep_strips_keep_piles);
			#ifdef _FIND_DELAY
				wxLogDebug(_T("5. After RecalcLayout"));
			#endif
#else
			pLayout->RecalcLayout(pApp->m_pSourcePhrases, create_strips_keep_piles);
#endif
			pApp->m_pActivePile = (CPile*)NULL; // can use this as a flag for at-EOF condition too
			pLayout->m_docEditOperationType = no_edit_op;

			RestorePhraseBoxAtDocEndSafely(pApp, pView);  // BEW added 8Sep14

		} // end of TRUE block for test: if (pApp->m_pActivePile == NULL || pApp->m_nActiveSequNum == -1)

		else // we have a non-null active pile defined, and sequence number >= 0
		{
			pApp->m_pTargetBox->SetFocus();
			pLayout->m_docEditOperationType = no_edit_op; // is this correct for here?
		}

		// BEW 21May15, this is the second of three places in OnePass() where a Thaw() call
		// must be initiated when conditions are right; control passes through here when the
		// sequence of auto-inserts is just coming to a halt location - we want a Thaw() done
		// in that circumstance, regardless of the m_nInsertCount value, and the latter
		// reset to default zero
		if (pApp->m_bSupportFreeze && pApp->m_bIsFrozen)
		{
			pApp->m_nInsertCount = 0;
			pView->canvas->Thaw();
			pApp->m_bIsFrozen = FALSE;
			// Don't need a third-of-a-second delay 
			if (pApp->m_nCurDelay == 31)
			{
				pApp->m_nCurDelay = 0; // set back to zero
			}
		}
        // whm 19Feb2018 modified. Don't empty the global m_Translation string before PlaceBox() call
        // below. The PlaceBox may need it for PopulateDropDownList() after which m_Translation.Empty()
        // will be called there.
		//m_Translation.Empty(); // clear the static string storage for the translation (or gloss)
		// save the phrase box's text, in case user hits SHIFT+END to unmerge a phrase
        m_Translation = m_Translation; // for debugging
        m_SaveTargetPhrase = pApp->m_targetPhrase;
		pApp->m_bAutoInsert = FALSE; // ensure we halt for user to type translation
		pView->Invalidate(); // added 1Apr09, since we return at next line
		pLayout->PlaceBox();
		return FALSE; // must have been a null string, or at EOF;
	} // end of TRUE block for test: if (!bSuccessful)

	// if control gets here, all is well so far, so set up the main window
	//pApp->GetMainFrame()->canvas->ScrollIntoView(pApp->m_nActiveSequNum);
	#ifdef _FIND_DELAY
		wxLogDebug(_T("6. Before ScrollIntoView"));
	#endif
	pLayout->m_pCanvas->ScrollIntoView(pApp->m_nActiveSequNum);
	#ifdef _FIND_DELAY
		wxLogDebug(_T("7. After ScrollIntoView"));
	#endif

	// BEW 21May15, this is the final of the three places in OnePass() where a Thaw() call
	// must be initiated when conditions are right; control passes through here when the
	// sequence of auto-inserts has not yet come to a halt location - we want a Thaw() if
	// periodically, breaking up the auto-insert sequence with a single redraw when
	// m_nInsertCount reaches the NUMINSERTS value for each subsequence
	if (pApp->m_bSupportFreeze && pApp->m_bIsFrozen)
	{
		// Are we at the penultimate OnePass() call being completed? If so, we want the
		// Thaw() done, and the next call of OnPass() will then get the view redrawn, and
		// the one after that (if the sequence has not halted) will start a new subsequence
		// of calls where the canvas has been re-frozen 
		if (pApp->m_nInsertCount > 0 && ((pApp->m_nInsertCount + 1) % (int)NUMINSERTS == 0))
		{
			// Ask for the thaw of the canvas window
			pApp->m_nInsertCount = 0;
			pView->canvas->Thaw();
			pApp->m_bIsFrozen = FALSE;
			// Give user a 1-second delay in order to get user visually acquainted with the inserted words
			if (pApp->m_nCurDelay == 0)
			{
				// set delay of 31 ticks, it's unlikely to be accidently set to that value;
				// it's 31/100 of a second
				pApp->m_nCurDelay = 31; 
			}
		}
	}
	pLayout->m_docEditOperationType = relocate_box_op;
	pView->Invalidate(); // added 1Apr09, since we return at next line
	pLayout->PlaceBox();
	#ifdef _FIND_DELAY
		wxLogDebug(_T("8. End of OnePass"));
	#endif
	return TRUE; // all was okay
}

void CPhraseBox::RestorePhraseBoxAtDocEndSafely(CAdapt_ItApp* pApp, CAdapt_ItView *pView)
{
	CLayout* pLayout = pApp->GetLayout();
	int maxSN = pApp->GetMaxIndex();
	CPile* pEndPile = pView->GetPile(maxSN);
	wxASSERT(pEndPile != NULL);
	CSourcePhrase* pTheSrcPhrase = pEndPile->GetSrcPhrase();
	if (!pTheSrcPhrase->m_bRetranslation)
	{
		// BEW 5Oct15, Buka island. Added test for free translation mode here, because
		// coming to the end in free trans mode, putting the phrasebox at last pile
		// results in the current section's last pile getting a spurious new 1-pile
		// section created at the doc end. The fix is to put the phrasebox back at the
		// current pile's anchor, so that no new section is created
		if (pApp->m_bFreeTranslationMode)
		{
			// Find the old anchor pile, closest to the doc end; but just in case
			// something is a bit wonky, test that the last pile has a free translation -
			// if it doesn't, then it is save to make that the active location (and it
			// would become an empty new free translation - which should be free
			// translated probably, and then this function will be reentered to get that
			// location as the final phrasebox location
			CPile* pPile = pView->GetPile(maxSN);
			if (!pPile->GetSrcPhrase()->m_bHasFreeTrans)
			{
				pApp->m_nActiveSequNum = maxSN;
			}
			else
			{
				while (!pPile->GetSrcPhrase()->m_bStartFreeTrans)
				{
					pPile = pView->GetPrevPile(pPile);
					wxASSERT(pPile != NULL);
				}
				pApp->m_nActiveSequNum = pPile->GetSrcPhrase()->m_nSequNumber;
			}
			pApp->m_pActivePile = pPile;
		}
		else
		{
			// Normal mode, use the last pile as active location
			pApp->m_nActiveSequNum = maxSN;
			pApp->m_pActivePile = pEndPile;
		}
	}
	else
	{
		// The end pile is within a retranslation, so find a safe active location
		int aSafeSN = maxSN + 1; // initialize to an illegal value 1 beyond
									// maximum index, this will force the search
									// for a safe location to search backwards
									// from the document's end
		int nFinish = 0; // can be set to num of piles in the retranslation but
							// this will do, as the loop will iterate over them
		// The next call, if successful, sets m_pActivePile
		bool bGotSafeLocation =  pView->SetActivePilePointerSafely(pApp,
							pApp->m_pSourcePhrases,aSafeSN,maxSN,nFinish);
		if (bGotSafeLocation)
		{
			pApp->m_nActiveSequNum = aSafeSN;
			pTheSrcPhrase = pApp->m_pActivePile->GetSrcPhrase();
			wxASSERT(pTheSrcPhrase->m_nSequNumber == aSafeSN);
		}
		else
		{
			// failed to get a safe location, so use start of document
			pApp->m_nActiveSequNum = 0;
			pApp->m_pActivePile = pView->GetPile(0);
		}
	}
	wxString transln = pTheSrcPhrase->m_adaption;
	pApp->m_targetPhrase = transln;
	pApp->m_pTargetBox->ChangeValue(transln);
	int length = transln.Len();
	pApp->m_pTargetBox->SetSelection(length,length);
	pApp->m_bAutoInsert = FALSE; // ensure we halt for user to type translation
#ifdef _NEW_LAYOUT
	pLayout->RecalcLayout(pApp->m_pSourcePhrases, keep_strips_keep_piles);
#else
	pLayout->RecalcLayout(pApp->m_pSourcePhrases, create_strips_keep_piles);
#endif
	pApp->m_pActivePile = pView->GetPile(pApp->m_nActiveSequNum);
	pApp->m_pTargetBox->SetFocus();
	pApp->m_pActivePile = pView->GetPile(pApp->m_nActiveSequNum);
	pApp->m_pTargetBox->SetFocus();
	pLayout->m_docEditOperationType = no_edit_op;

	pView->Invalidate();
	pLayout->PlaceBox();

}

// This OnKeyUp function is called via the EVT_KEY_UP event in our CPhraseBox
// Event Table.
// OnKeyUp() is triggered after OnKeyDown() and OnChar() - if/when OnChar() is triggered.
// 
// BEW 13Apr10, no changes needed for support of doc version 5
// whm Note 15Feb2018 modified key handling so that ALL AltDown(), ShiftDown(), and ControlDown()
// events are now sent to OnSysKeyUp() for processing.
// whm 16Feb2018 Notes: OnKeyUp() currently does the following key handling:
// 1. Detects all AltDown(), all ShiftDown(), and all ControlDown() events (with key combinations) and
//    routes all processing of those events to the separate function OnSysKeyUp(), and returns 
//    without calling Skip() suppressing further handling after execution of OnSysKeyUp().
// 2. Detects SHIFT+CTRL+SPACE, and if detected calls OnCtrlShiftSpacebar(), then return to suppress further handling.
// 3. Detects WXK_RIGHT and WXK_LEFT; if detected sets flags m_bAbandonable = FALSE, pApp->m_bUserTypedSomething = TRUE,
//    and m_bRetainBoxContents = TRUE, for use if auto-merge (OnButtonMerge) is called on a selection.
// 4. Detects WXK_F8 and if detected calls Adapt_ItView::ChooseTranslation() then return to suppress further handling.
// Note: Put in this OnKeyUp() handler custom key handling that does not involve simultaneous use 
// of ALT, SHIFT, or CTRL keys.
void CPhraseBox::OnKeyUp(wxKeyEvent& event)
{
	//wxLogDebug(_T("OnKeyUp() %d called from PhraseBox"),event.GetKeyCode());
	CAdapt_ItApp* pApp = &wxGetApp();
	wxASSERT(pApp != NULL);
	CAdapt_ItView* pView = (CAdapt_ItView*) pApp->GetView();
	wxASSERT(pView->IsKindOf(CLASSINFO(CAdapt_ItView)));

	// whm 16Feb2018 Note: The following test using GetKeyCode will never work for detecting a WXK_ALT key event.
    // Even calling event.AltDown() will NOT detect if ALT is down here in OnKeyUp(). 
    // Removed the m_bALT_KEY_DOWN coding - see my comments in the View's DoSrcPhraseSelCopy()
    // The basic reason is that m_bALT_KEY_DOWN cannot be set FALSE from the OnKeyUp() and the very limited
    // bKeyDown used in DoSrcPhraseSelCopy() is now detected from within DoSrcPhraseSelCopy's caller OnEditCopy().
    //if (event.GetKeyCode() == WXK_ALT)
    //{
	//	pApp->m_bALT_KEY_DOWN = FALSE; // indicate ALT is not down, for use by DoSrcPhraseSelCopy()
	//	// continue on
	//}

    // Note: wxWidgets doesn't have a separate OnSysKeyUp() virtual method (like MFC did)
	// so we'll simply detect if the ALT key or Shift key was down modifying another key stroke
    // and call the OnSysKeyUp() method from here.
    // Notes: 
    // Pressing ALT alone doesn't trigger OnKeyUp() - only ALT + some other key combo.
    // Pressing SHIFT alone doesn't trigger OnKeyUp() - only SHIFT + some other key combo.
    // Pressing CTRL alone doesn't trigger OnKeyUp() - only CTRL + some other key combo.
    // CmdDown() is same as ControlDown on PC, and Apple Command key on Macs.
    // whm 16Feb2018 moved all AltDown() ShiftDown() and ControlDown() processing to OnSysKeyUp()
    if (event.AltDown() || event.ShiftDown() || event.ControlDown()) 
	{
		OnSysKeyUp(event);
		return;
	}

	// version 1.4.2 and onwards, we want a right or left arrow used to remove the
	// phrasebox's selection to be considered a typed character, so that if a subsequent
	// selection and merge is done then the first target word will not get lost; and so
	// we add a block of code also to start of OnChar( ) to check for entry with both
	// m_bAbandonable and m_bUserTypedSomething set TRUE, in which case we don't
	// clear these flags (the older versions cleared the flags on entry to OnChar( ) )

	// we use this flag cocktail to test for these values of the three flags as the
	// condition for telling the application to retain the phrase box's contents
	// when user deselects target word, then makes a phrase and merges by typing.
	// (When glossing, we don't need to do anything here - suppressions handled elsewhere)
    // whm 16Feb2018 verified the following two WXK_RIGHT and WXK_LEFT execute.
    // whm TODO: Although these flags are set, the OnButtonMerge() doesn't seem to 
    // preserve the phrasebox's contents
	if (event.GetKeyCode() == WXK_RIGHT)
	{
		m_bAbandonable = FALSE;
		pApp->m_bUserTypedSomething = TRUE;
        m_bRetainBoxContents = TRUE;
		event.Skip();
	}
	else if (event.GetKeyCode() == WXK_LEFT)
	{
		m_bAbandonable = FALSE;
		pApp->m_bUserTypedSomething = TRUE;
        m_bRetainBoxContents = TRUE;
		event.Skip();
	}

	// does the user want to force the Choose Translation dialog open?
	// whm Note 12Feb09: This F8 action is OK on Mac (no conflict)
	if (event.GetKeyCode() == WXK_F8)
	{
		// whm added 26Mar12
		if (pApp->m_bReadOnlyAccess)
		{
			// Disable the F8 key invocation of the ChooseTranslation dialog
			return;
		}
		pView->ChooseTranslation();
		return;
	}

	event.Skip();
}

// This OnKeyDown function is called via the EVT_KEY_DOWN event in our CPhraseBox
// Event Table. 
// OnKeyDown() is the first key handler generated, and is triggered for ALL key strokes.
// 
// The wx docs say: "If a key down (EVT_KEY_DOWN) event is caught and the event handler 
// does not call event.Skip() then the corresponding char event (EVT_CHAR) will not happen. 
// This is by design and enables the programs that handle both types of events to avoid 
// processing the same key twice. As a consequence, if you do NOT want to suppress the 
// wxEVT_CHAR events for the keys you handle, always call event.Skip() in your 
// wxEVT_KEY_DOWN handler. Not doing so may also prevent accelerators defined using this 
// key from working... For Windows programmers: The key and char events in wxWidgets are 
// similar to but slightly different from Windows WM_KEYDOWN and WM_CHAR events. In particular, 
// Alt-x combination will generate a char event in wxWidgets (unless it is used as an accelerator) 
// and almost all keys, including ones without ASCII equivalents, generate char events too."
//
// whm 16Feb2018 Notes: OnKeyDown() currently does the following key handling:
// 1. Sets m_bALT_KEY_DOWN to TRUE when WXK_ALT is detected
// 2. When m_bReadOnlyAccess is TRUE, beeps for all alphabetic chars, and returns without calling Skip() suppressing further handling.
// 3. Sets App's m_bAutoInsert to FALSE if it was TRUE, and returns without calling Skip() suppressing further handling.
// 4. Calls RefreshStatusBarInfo().
// 5. Handles WXK_DELETE and calls Skip() for DELETE to be handled in further processing.
// 6. Handles WXK_ESCAPE (to restore phrasebox content after guess) and returns without calling Skip() suppressing further handling.
// BEW 13Apr10, no changes needed for support of doc version 5
void CPhraseBox::OnKeyDown(wxKeyEvent& event)
{
	// refactored 2Apr09
	//wxLogDebug(_T("OnKeyDown() %d called from PhraseBox"),event.GetKeyCode());
	CAdapt_ItApp* pApp = &wxGetApp();
	wxASSERT(pApp != NULL);
	CAdapt_ItView* pView = (CAdapt_ItView*) pApp->GetView();
	wxASSERT(pView->IsKindOf(CLASSINFO(CAdapt_ItView)));
	//CLayout* pLayout = GetLayout();

	// BEW 31Jul16 added, to track ALT key down or released
    if (!pApp->m_bReadOnlyAccess)
	{
		int keyCode = event.GetKeyCode();
		if (keyCode == WXK_ALT)
		{
			pApp->m_bALT_KEY_DOWN = TRUE;
		}
		// continue on to do any further processing; for the above variable
        // whm 16Feb2018 The App global m_bALT_KEY_DOWN is now reset FALSE at end of DoSrcPhraseSelCopy().
	}

    // whm added 15Mar12. When in read-only mode don't register any key strokes
	if (pApp->m_bReadOnlyAccess)
	{
		// return without calling Skip(). Beep for read-only feedback
		int keyCode = event.GetKeyCode();
		if (keyCode == WXK_DOWN || keyCode == WXK_UP || keyCode == WXK_LEFT || keyCode == WXK_RIGHT
			|| keyCode == WXK_PAGEUP || keyCode == WXK_PAGEDOWN
			|| keyCode == WXK_CONTROL || keyCode == WXK_ALT || keyCode == WXK_SHIFT
			|| keyCode == WXK_ESCAPE || keyCode == WXK_TAB || keyCode == WXK_BACK
			|| keyCode == WXK_RETURN || keyCode == WXK_DELETE || keyCode == WXK_SPACE
			|| keyCode == WXK_HOME || keyCode == WXK_END || keyCode == WXK_INSERT
			|| keyCode == WXK_F8)
		{
			; // don't beep for the various keys above
		}
		else
		{
			::wxBell();
		}
		// don't pass on the key stroke - don't call Skip()
		return;
	}

	if (!pApp->m_bSingleStep)
	{
		// halt the auto matching and inserting, if a key is typed
		if (pApp->m_bAutoInsert)
		{
			pApp->m_bAutoInsert = FALSE;
			// Skip() should not be called here, because we can ignore the value of
			// any keystroke that was made to stop auto insertions.
			return;
		}
	}

	// update status bar with project name
	pApp->RefreshStatusBarInfo();

	//event.Skip(); //CEdit::OnKeyDown(nChar, nRepCnt, nFlags);

	// [MFC Note] Delete key sends WM_KEYDOWN message only, WM_CHAR not sent
	// so we need to update things for forward deletions here (and it has to be
	// done after the call to the base class, otherwise the last deleted character
	// remains in the final target phrase text)
	// BEW added more on 20June06, so that a DEL keypress in the box when it has a <no adaptation>
	// KB or GKB entry will effect returning the CSourcePhrase instance back to a true "empty" one
	// (ie. m_bHasKBEntry or m_bHasGlossingKBEntry will be cleared, depending on current mode, and
	// the relevant KB's CRefString decremented in the count, or removed entirely if the count is 1.)
	// [wxWidget Note] In contrast to Bruce's note above OnKeyDown() Delete key does trigger
	// OnChar() in wx key handling.
	CPile* pActivePile = pView->GetPile(pApp->m_nActiveSequNum); // doesn't rely on pApp->m_pActivePile
																 // having been set already
	CSourcePhrase* pSrcPhrase = pActivePile->GetSrcPhrase();
	if (event.GetKeyCode() == WXK_DELETE)  // DELETE
	{
		if (pSrcPhrase->m_adaption.IsEmpty() && ((pSrcPhrase->m_bHasKBEntry && !gbIsGlossing) ||
			(pSrcPhrase->m_bHasGlossingKBEntry && gbIsGlossing)))
		{
			m_bNoAdaptationRemovalRequested = TRUE;
			wxString emptyStr = _T("");
			if (gbIsGlossing)
				pApp->m_pGlossingKB->GetAndRemoveRefString(pSrcPhrase,emptyStr,useGlossOrAdaptationForLookup);
			else
				pApp->m_pKB->GetAndRemoveRefString(pSrcPhrase,emptyStr,useGlossOrAdaptationForLookup);
		}
		else
		{
			// legacy behaviour: handle the forward character deletion
			m_backspaceUndoStr.Empty();
			m_nSaveStart = -1;
			m_nSaveEnd = -1;

			wxString s;
			s = GetValue();
			pApp->m_targetPhrase = s; // otherwise, deletions using <DEL> key are not recorded
            if (!this->GetTextCtrl()->IsModified()) // whm 14Feb2018 added GetTextCtrl()->
            {
                // whm added 10Jan2018 The SetModify() call below is needed for the __WXGTK__ port
                // For some unknown reason the delete key deleting the whole phrasebox contents
                // does not set the dirty flag without this.
                this->SetModify(TRUE);
            }
		}
	}
	else if (event.GetKeyCode() == WXK_ESCAPE)
	{
		// user pressed the Esc key. If a Guess is current in the phrasebox we
		// will remove the Guess-highlight background color and the guess, and
		// restore the normal copied source word to the phrasebox. We also reset
		// the App's m_bIsGuess flag to FALSE.
		if (this->GetBackgroundColour() == pApp->m_GuessHighlightColor)
		{
			// get the pSrcPhrase at this active location
			CSourcePhrase* pSrcPhrase;
			pSrcPhrase = pApp->m_pActivePile->GetSrcPhrase();
			//wxString str = pSrcPhrase->m_key; // BEW 27Bov14 deprecated, in favour of using the 
			// stored pre-guesser version of the string, because it may have been modified
			wxString str = pApp->m_preGuesserStr; // use this as the default restore string

			// It was Roland Fumey in 16July08 that wanted strings other than the source text
			// to be used for the restoration, if pApp->m_bLegacySourceTextCopy was not in effect, so
			// keep the following as it is the protocol he requested
			if (!pApp->m_bLegacySourceTextCopy)
			{
				// the user wants smart copying done to the phrase box when the active location
				// landed on does not have any existing adaptation (in adapting mode), or, gloss
				// (in glossing mode). In the former case, it tries to copy a gloss to the box
				// if a gloss is available, otherwise source text used instead; in the latter case
				// it tries to copy an adaptation as the default gloss, if an adaptation is
				// available, otherwise source text is used instead
				if (gbIsGlossing)
				{
					if (!pSrcPhrase->m_adaption.IsEmpty())
					{
						str = pSrcPhrase->m_adaption;
					}
				}
				else
				{
					if (!pSrcPhrase->m_gloss.IsEmpty())
					{
						str = pSrcPhrase->m_gloss;
					}
				}
			}
			this->ChangeValue(str);
			pApp->m_targetPhrase = str; // Required, otherwise the guess persists and gets used in auto-inserts subsequently
			this->SetBackgroundColour(wxColour(255,255,255)); // white;
			this->m_bAbandonable = TRUE;
			pApp->m_bIsGuess = FALSE;
			pApp->m_preGuesserStr.Empty(); // clear this to empty, it's job is done
			this->Refresh();
			return;
		}
	}
	event.Skip(); // allow processing of the keystroke event to continue
}

void CPhraseBox::ClearDropDownList()
{
    this->Clear();
}

void CPhraseBox::SetModify(bool modify)
{
    // Note: whm 14Feb2018 added this->GetTextCtrl->
	if (modify)
		this->GetTextCtrl()->MarkDirty(); // "mark text as modified (dirty)"
	else
        this->GetTextCtrl()->DiscardEdits(); // "resets the internal 'modified' flag
						// as if the current edits had been saved"
}

bool CPhraseBox::GetModify()
{
    // Note: whm 14Feb2018 added this->GetTextCtrl->
    return this->GetTextCtrl()->IsModified();
}

// whm 22Mar2018 Notes: 
// The SetupDropDownPhraseBox() is used once - near the end of the Layout's PlaceBox() 
// function. At that location in PlaceBox() the path of code execution may often pass 
// that point multiple times, especially when auto-inserting target entries. To reduce
// the number of spurious calls of this function, it employs three boolean flags that
// prevent execution of the code within this function unless all three flags are FALSE.
// By allowing the execution path to do the dropdown setup and list population 
// operations below only when the App's bLookAheadMerge, m_bAutoInsert, and
// m_bMovingToDifferentPile flags are all FALSE, we are able to restrict code execution
// here to once (approximately) per final landing location of the phrasebox.
// WARNING: It is possible that the three boolean flags may not prevent all spurious
// calls of this function. Therefore, it is best to not put anything within this
// function that cannot be executed more than once, and still achieve the same desired
// result.
void CPhraseBox::SetupDropDownPhraseBoxForThisLocation()
{
    // whm added 10Jan2018 to support quick selection of a translation equivalent.
    // The Layout's PlaceBox() function is always called just before the phrasebox 'rests' 
    // and can be interacted with by the user. Hence the location in PlaceBox() is the  
    // location in the application where we execute this function to set up the content 
    // and characteristics of the phrasebox's (m_pTargetBox) dropdown control:
    //
    // 1. Test for the App's bLookAheadMerge, m_bAutoInsert, or m_bMovingToDifferentPile flags. 
    //    By allowing the code in this function to execute only when all those flags are FALSE, 
    //    it eliminates extraneous calls of this block of code when PlaceBox() is called during
    //    merging, auto-inserting, and PlaceBox() calls in the midst of box moving operations.
    //
    // 2. Set boolean flags to prepare the location for the dropdown phrasebox
    //    2.a Initialize m_bChooseTransInitializePopup to TRUE so OnIdle() can set the initial
    //        state of the dropdown popup - open or closed.
    //        If in Free Translation mode initialize m_bChooseTransInitializePopup to FALSE.
    //    2.b Initialize m_bAbandonable to TRUE so that an immediate click away to a new
    //        location will not change the current location, unless user changes are made
    //        (which should set m_bAbandonable to FALSE).
    // 3. Determine a pCurTargetUnit for the present location. If a previous ChooseTranslation
    //    dialog session was just dismissed with changes made to this target unit, pCurTargetUnit
    //    will be non-NULL, and we use it to determine the non-deleted ref strings for the dropdown
    //    list. If pCurTargetUnit is NULL at this point, we determine the target unit for the 
    //    current location and use it to determine the non-deleted ref strings, if any.
    //
    // 4. Get the count of non-deleted ref string instances by calling pCurTargetUnit's 
    //    CountNonDeletedRefStringInstances() function, and proceed as follows depending
    //    on the returned count:
    //
    // When the count of non-deleted ref string instances nRefStrCount is > 0:
    //    4a. Load the contents of the phrasebox's drop down list (via calling PopulateDropDownList()).
    //        The PopulateDropDownList() function returns (via reference parameters) information about
    //        a list item that should be selected/highlighted (via selectionIndex), as well as whether
    //        there is a <no adaptation> ref string present (via bNoAdaptationFlagPresent), and if that
    //        flag is to be selected/highlighted (via indexOfNoAdaptation).
    //    4b. Call SetButtonBitmaps() to use the "normal" set of button bitmaps on the dropdown control,
    //        the down arrow button - indicating to user that the dropdown list has one or more items.
    //    4c. Call SetSelection(selectionIndex). The selectionIndex could be -1 which would remove any
    //        selection/highlight; or if >= 0 it highlights the selection. 
    //        Note that selecting/highlighting any list item has the side-effect of copying the item's 
    //        string at selectionIndex from the dropdown list into the combo box's edit box.
    //    4d. If the nRefStrCount == 1 we suppress OnIdle()'s opening of the popup list, by setting the 
    //        App's m_bChooseTransShowPopoup flag to FALSE, since any selection of the single item is 
    //        automatically copied into the edit box/phrasebox. 
    //        Otherwise, when nRefStrCount > 1, we set the App's m_bChooseTransInitializePopup flag to TRUE
    //        to cause the dropdown box to initially appear in an open state.
    //    4e. If the bNoAdaptationFlagPresent == TRUE we ensure the following (see and cf. 2f, 2g, 2h below): 
    //          When nRefStrCount == 1, that ref String must be an empty string, i.e., represented as
    //            "<no adaptation" as the single list item (with indexOfNoAdaptation == 0). 
    //            Here we avoid calling SetSelection() in this case to avoid the unwanted side effect of 
    //            a "<no adaptation" string being copied into the dropdown's edit box. We just ensure that
    //            the m_targetPhrase's (empty) value is assigned in the m_pTargetBox, and that SetFocus()
    //            is on the phrasebox (SetFocus probably not needed but doesn't hurt).
    //          When nRefStrCount > 1, we know that there is more than 1 ref string in the list, and
    //            one of them should be a "<no adaptation>" item. The PopulateDropDownList() call will
    //            have determined the index stored in indesOfNoAdaptation, so we call
    //            SetSelection(indexOfNoAdaptation), and ensure that m_targetPhrase is assigned in the
    //            m_pTargetBox.
    //        If the bNoAdaptationFlagPresent == FALSE we ensure the following:
    //          When nRefStrCount == 1, there is one and only one ref string and it cannot be 
    //          a <no adaptation> type. The PopulateDropDownList() function has put the ref 
    //          string as the first string in the dropdown list. Do the following:
    //              Call GetString(0) and assign that item's string to the App's m_targetPhrase.
    //              Change the value in the m_pTargetBox to become the value now in m_targetPhrase.
    //              Call SetSelection(0) to select/highlight the list item (also copies it to the edit box).
    //          When nRefStrCount > 1, we know that there is more than 1 ref String none of which are 
    //            empty ("<no adaptation>"). Do the following:
    //            If the selectionIndex is -1, set index to 0, otherwise set index to value of selectionIndex.
    //            Call GetString(index) and assign it to the App's m_targetPhrase.
    //            Change the value in the m_pTargetBox to become the value now in m_targetPhrase.
    //            Call SetSelection(index) to select/highlight the list item (also copies it to the edit box).
    //
    // When the count of non-deleted ref string instances nRefStrCount is == 0:
    // In this case - when nRefStrCount == 0, we are at a "hole", and there are no translation equivalents
    // in the dropdown's list.
    //    4a. We call ClearDropDownList() here to ensure that the list is empty. We do NOT call 
    //        PopulateDropDownList(), as we do not need selection indices returned for an empty list.
    //    4b. Call SetButtonBitmaps() to use the [X] button set of button bitmaps on the dropdown control,
    //        the [X] button image - indicating to user that there are no translation equivalents in the
    //        dropdown list to choose from. The list is not truly "disabled" - the user can still click on
    //        the [X] button, but the list will just open and be empty.
    //    4c. We do NOT call SetSelection() here as there is nothing to select/highlight.
    //    4d. With nothing in the dropdown list we set the App's m_bChooseTransInitializePopup flag to FALSE,
    //        which informs OnIdle() not to open/show the dropdown list.
    //    4e. The bNoAdaptationFlagPresent will always be FALSE when nRefStrCount == 0.
    //    4f. Call SetSelection(-1, -1) to select all content of the phrasebox - which in this case
    //        would be a copy of the source phrase if m_bCopySource == TRUE, empty otherwise.
    //    4g. Assign m_pTargetBox to contain the value of m_targetPhrase (a copy of source phrase if 
    //        m_bCopySource == TRUE).
    //    4h. Call SetFocus() to ensure phrasebox is in focus (probably not needed but doesn't hurt)
    //
    // 5. Reset the App's pCurTargetUnit to NULL to prepare for the next location or manual calling 
    //    up of the ChooseTranslation dialog.
    CAdapt_ItApp* pApp = &wxGetApp();
    wxASSERT(pApp != NULL);

    // Here we restrict execution for when all 3 flags below are FALSE. This reduces/eliminates
    // spurios execution when PlaceBox() is called during merging and moving operations.
    if (!pApp->bLookAheadMerge && !pApp->m_bAutoInsert && !pApp->m_bMovingToDifferentPile)
    {
        // If in Free translation mode keep the dropdown list closed by not allowing
        // OnIdle() to initialize the phrasebox's dropdown list to an open state.
        // But, when not in Free translation mode have OnIdle() initialize the phrasebox's
        // to the appropriate state - open for a list with items, closed for an empty list
        if (pApp->m_bFreeTranslationMode)
            pApp->m_bChooseTransInitializePopup = FALSE;
        else
            pApp->m_bChooseTransInitializePopup = TRUE;

        // whm 10Apr2018 added. Set the initial value of m_bAbandonable to TRUE since we are setting up
        // for the dropdown phrasebox, and any content in the phrasebox should initially be considered
        // abandonable at least here when setting up the dropdown phrasebox for display to the user.
        // Certain actions at the current location may change the flag to FALSE before the phrasebox
        // moves - such as any key press that changes the phrasebox contents. 
        this->m_bAbandonable = TRUE;

        // If the caller was LookAhead(), or if the ChooseTranslation dialog was just dismissed before
        // this SetupDropDownPhraseBoxForThisLocation() call, the global pCurTargetUnit will have been 
        // defined at this point, and we should use it. If it is NULL we determine pCurTargetUnit at 
        // this location and use it.
        if (pApp->pCurTargetUnit == NULL)
        {
            // Get a local target unit pointer.
            // No pCurTargetUnit was available in the caller to populate the list,
            // So when pCurTargetUnit is NULL, we get the pCurTargetUnit directly using the 
            // appropriate KB's GetTargetUnit() method to populate the dropdown list.
            CTargetUnit* pTargetUnit = (CTargetUnit*)NULL;

            pApp->m_pTargetBox->m_nWordsInPhrase = 0;

            CKB* pKB;
            if (gbIsGlossing)
            {
                pKB = pApp->m_pGlossingKB;
            }
            else
            {
                pKB = pApp->m_pKB;
            }
            CSourcePhrase* pSrcPhrase = pApp->m_pActivePile->GetSrcPhrase();
            pApp->m_pTargetBox->m_nWordsInPhrase = pSrcPhrase->m_nSrcWords;
            // Get a pointer to the target unit for the current key - could be NULL
            pTargetUnit = pKB->GetTargetUnit(pApp->m_pTargetBox->m_nWordsInPhrase, pSrcPhrase->m_key); 

            // Assign the local target unit to the App's member pCurTargetUnit for use below
            pApp->pCurTargetUnit = pTargetUnit;
        }

        // Get a count of the number of non-deleted ref string instances for the current target unit
        // (which may be adjusted by a prior instance of the ChooseTranslation dialog)
        int nRefStrCount = 0;
        bool bNoAdaptationFlagPresent = FALSE; // the PopulateDropDownList() function may change this value
        int indexOfNoAdaptation = -1; // the PopulateDropDownList() function may change this value
        if (pApp->pCurTargetUnit != NULL)
        {
            nRefStrCount = pApp->pCurTargetUnit->CountNonDeletedRefStringInstances();
        }

        if (nRefStrCount > 0)
        {
            int selectionIndex = -1;
            // whm 5Mar2018 Note: This block executes when nRefStrCount is > 0
            // whm 27Feb2018 Note: If the Choose Translation dialog was called before the execution of the 
            // PopulateDropDownList() function below, any changes, additions, reordering, deletions to the KB 
            // items listed there will pass into PopulateDropDownList() via the pCurTargetUnit 
            // parameter, and any selection of a list item from that dialog will be returned here via the
            // selectionIndex parameter, and the presence and index of <no adaptation> are returned in the
            // bNoAdaptationFlagPresent the SetSelection(selectionIndex) command in the if...else block
            // below.
            if (pApp->pCurTargetUnit != NULL)
            {
                pApp->m_pTargetBox->PopulateDropDownList(pApp->pCurTargetUnit, selectionIndex, bNoAdaptationFlagPresent, indexOfNoAdaptation);
            }

            // whm Note: Setting m_Translation to Empty() can't be done here. 
            // See end of OnButtonChooseTranslation() in the View.
            // Since PlaceBox() gets called multiple times in the course of a user action, resetting
            // variables to null or default values should not be done from this location. Hence, I 
            // commented out the m_Translation.Empty() call below.
            // m_pApp->m_pTargetBox->m_Translation.Empty(); 

            // We're in the nRefStrCount > 0 block so there is at lease one translation equivalent
            // in the dropdown's list, so set the dropdown's button to its normal "enabled" state. 
            // If in Free Translation mode, use the "disable" button on the dropdown control
            if (pApp->m_bFreeTranslationMode)
                pApp->m_pTargetBox->SetButtonBitmaps(pApp->m_pTargetBox->dropbutton_blank, false, pApp->m_pTargetBox->dropbutton_blank, pApp->m_pTargetBox->dropbutton_blank, pApp->m_pTargetBox->dropbutton_blank);
            else
                pApp->m_pTargetBox->SetButtonBitmaps(pApp->m_pTargetBox->dropbutton_normal, false, pApp->m_pTargetBox->dropbutton_pressed, pApp->m_pTargetBox->dropbutton_hover, pApp->m_pTargetBox->dropbutton_disabled);

            // Set the dropdown's list selection to the selectionIndex determined by PopulatDropDownList above.
            // If selectionIndex is -1, it removes any list selection from dropdown list
            // Note: SetSelection() with a single parameter operates to select/highlight the
            // dropdown's list item at the designated selectionIndex parameter. The
            // SetSelection() with two parameters operates to select a substring of text
            // within the dropdown's edit box, delineated by the two parameters.
            pApp->m_pTargetBox->SetSelection(selectionIndex);
            // whm 7Mar2018 addition - SetSelection() highlights the item in the list, and it
            // also has a side effect in that it automatically copies the item string from the 
            // dropdown list (matching the selectionIndexand) into the dropdown's edit box.

            // The dropdown list, however, like the ChooseTranslation dialog's list, may contain
            // items and those items are not-adjusted for case - they are lower case when gbAutoCaps 
            // is TRUE. When we call SetSelection(), it puts the lower case form of the item string 
            // into the m_pApp->m_pTargetBox (the phrasebox). If case is not handled elsewhere we
            // would need to handle case changes here. Note: testing indicates that the case 
            // changes are indeed handled elsewhere.

            // Note: If we need to handle case issues, the code for gbAutoCaps from PlacePhraseBox() is 
            // shown commented out below for reference:
            /*
            // Therefore, we'll process the target box contents here for case depending on the
            // value of gbAutoCaps - using the same block of code that was used in
            // PlacePhraseBox (at the a: label).
            bool bNoError = TRUE;
            if (gbAutoCaps)
            {
                if (gbSourceIsUpperCase && !gbMatchedKB_UCentry)
                {
                    bNoError = pApp->GetDocument()->SetCaseParameters(pApp->m_targetPhrase, FALSE);
                    if (bNoError && !gbNonSourceIsUpperCase && (gcharNonSrcUC != _T('\0')))
                    {
                        // change to upper case initial letter
                        pApp->m_targetPhrase.SetChar(0, gcharNonSrcUC);
                    }
                }
            }
            pApp->m_pTargetBox->m_SaveTargetPhrase = pApp->m_targetPhrase;
            */

            // Within this block we know that nRefStrCount > 0

            // Note: The target unit's ref strings may include a "<no adaptation>" ref string which 
            // is stored internally as an empty ref string. An empty ref string is reconstituted 
            // (for display purposes to the user) as "<no adaptation>" when it resides in a list 
            // (dropdown or ChooseTranslation). The default value of the bNoAdaptationFlagPresent is
            // FALSE, but The PopulateDropDownList() function call above determines when the value
            // of bNoAdaptationFlagPresent is TRUE.
            if (bNoAdaptationFlagPresent)
            {
                // A <no adaptation> ref string is present
                if (nRefStrCount == 1)
                {
                    // There is only one ref string and it is empty, i.e., it is <no adaptation>.
                    // The index of the <no adaptation> list item must be 0. We don't call the
                    // SetSelection() function in this case because we don't want "<no adaptation>"
                    // to be auto-copied into the dropdown's edit box, it should only appear as the
                    // sole item in the list - viewable if the user should open the list to see it.
                    // The App's m_targetPhrase and the phrasebox's edit box should be empty.
                    wxASSERT(indexOfNoAdaptation == 0);
                    // Note: While m_targetPhrase should be empty, the spurious calls of PlaceBox() 
                    // before we get to the "real" one, the m_targetPhrase value may contain a value for
                    // the previous location instead of the current one, so the following wxASSERT
                    // cannot be relied on, and so to be safe I've commented it out. With the addition 
                    // of the test for the 3 flags being FALSE above, the m_targetPhrase value should be
                    // empty, but I'll leave the wxASSERT() test commented out to avoid any unforseen
                    // spurious PlaceBox() calls where it may not be empty.
                    //wxASSERT(pApp->m_targetPhrase.IsEmpty());
                    pApp->m_pTargetBox->ChangeValue(pApp->m_targetPhrase);
                    pApp->m_pTargetBox->SetFocus();

                }
                else if (nRefStrCount > 1)
                {
                    // There is more than 1 ref string, and one of them should be <no adaptation>,
                    // and the PopulateDropDownList() function determined that the <no adaptation> 
                    // item should be selected. In this situation, the phrasebox's edit box will 
                    // also be empty
                    pApp->m_pTargetBox->SetSelection(indexOfNoAdaptation);
                    // The SetSelection call above has the side-effect of puting the list item 
                    // <no adaptation> in the dropdown's edit box, so we have to make the box's
                    // content be empty here.
                    // Note: While m_targetPhrase should be empty, if any spurious calls of PlaceBox() 
                    // happen before we get to the "real" one, the m_targetPhrase value may contain a  
                    // value for the previous location instead of the current one, so the following 
                    // wxASSERT cannot be relied on, and so to be safe, I've commented it out.
                    //wxASSERT(pApp->m_targetPhrase.IsEmpty());
                    pApp->m_pTargetBox->ChangeValue(pApp->m_targetPhrase);
                }
            }
            else // no <no adaptation> present
            {
                // There is no <no adaptation> ref string in play at this location
                wxASSERT(indexOfNoAdaptation == -1);
                if (nRefStrCount == 1)
                {
                    // There is one and only one ref string and it cannot be a <no adaptation> type.
                    //wxASSERT(pApp->m_pTargetBox->GetCount() == 1);
                    int index;
                    if (selectionIndex == -1)
                    {
                        // If we pass a selectionIndex of -1 to the SetSelection() call below it
                        // would just empty the edit box and not select the item. Instead we
                        // set the index to 0 to get the first item
                        index = 0;
                    }
                    else
                    {
                        // The PopulateDropDownList() function determined a selectionIndex to use
                        index = selectionIndex;
                    }
                    pApp->m_targetPhrase = pApp->m_pTargetBox->GetString(index);
                    pApp->m_pTargetBox->ChangeValue(pApp->m_targetPhrase);
                    pApp->m_pTargetBox->SetSelection(index);
                }
                else // nRefStrCount > 1
                {
                    // There is more than 1 ref string, none of which are <no adaptation>.
                    // Here we basically just ensure that any existing phrasebox content
                    // is selected in the dropdown list using the selectIndex info we got
                    // back from PopulateDropDownList().
                    //wxASSERT(pApp->m_pTargetBox->GetCount() > 1);
                    int index;
                    if (selectionIndex == -1)
                    {
                        // If we pass a selectionIndex of -1 to the SetSelection() call below it
                        // would just empty the edit box and not select the item. Instead we
                        // set the index to 0 to get the first item
                        index = 0;
                    }
                    else
                    {
                        // The PopulateDropDownList() function determined a selectionIndex to use
                        index = selectionIndex;
                    }
                    pApp->m_targetPhrase = pApp->m_pTargetBox->GetString(index);
                    pApp->m_pTargetBox->ChangeValue(pApp->m_targetPhrase);
                    pApp->m_pTargetBox->SetSelection(index);
                    pApp->m_pTargetBox->SetSelection(-1, -1); // select all
                }
            }

            /*
            // Below for debugging only!!!
            // Note: The debugging code below can also be copied into the else block (when nRefStrCount == 0) below
            // if desired.
            {
            wxString selStr = pApp->m_pTargetBox->GetTextCtrl()->GetStringSelection();
            wxString tgtBoxValue = pApp->m_pTargetBox->GetValue();
            wxString srcPhraseOfActivePile = pApp->m_pActivePile->m_pSrcPhrase->m_srcPhrase;
            wxString targetStrOfActivePile = pApp->m_pActivePile->m_pSrcPhrase->m_targetStr;
            wxString srcPhraseKey = pApp->m_pActivePile->m_pSrcPhrase->m_key;
            wxString srcPhraseAdaption = pApp->m_pActivePile->m_pSrcPhrase->m_adaption;
            wxString targetPhraseOnApp = pApp->m_targetPhrase;
            wxString translation = pApp->m_pTargetBox->m_Translation;
            CTargetUnit* ptgtUnitFmChooseTrans = pApp->pCurTargetUnit; ptgtUnitFmChooseTrans = ptgtUnitFmChooseTrans;
            bool hasKBEntry = pApp->m_pActivePile->m_pSrcPhrase->m_bHasKBEntry; hasKBEntry = hasKBEntry;
            bool notInKB = pApp->m_pActivePile->m_pSrcPhrase->m_bNotInKB; notInKB = notInKB;
            // [BEW note]: we use this 3-flag cocktail below elsewhere to test for these values of the three flags as
            // the condition for telling the application to retain the phrase box's contents when
            // user deselects target word, then makes a phrase and merges by typing.
            // m_bAbandonable is set FALSE, m_bUserTypedSomething is set TRUE, and m_bRetainContents is set TRUE for:
            // A click in the phrasebox (in OnLButtonDown), or Right/Left arrow key press (in OnKeyUp), or if
            // during a merge the m_targetPhrase > 1 and selections beyond the active location don't have any
            // translation (in OnButtonMerge)
            wxString alwaysAsk;
            if (pApp->pCurTargetUnit != NULL)
            if (pApp->pCurTargetUnit->m_bAlwaysAsk)
            alwaysAsk = _T("1");
            else
            alwaysAsk = _T("0");
            else
            alwaysAsk = _T("-1");
            bool abandonable = pApp->m_pTargetBox->m_bAbandonable; abandonable = abandonable;
            bool userTypedSomething = pApp->m_bUserTypedSomething; userTypedSomething = userTypedSomething;
            bool retainBoxContents = pApp->m_pTargetBox->m_bRetainBoxContents; retainBoxContents = retainBoxContents; // used in OnButtonMerge()
            wxLogDebug(_T("|***********************************************************************************"));
            wxLogDebug(_T("|***|For nRefStrCount > 0 [%d]: m_srcPhrase = %s, m_targetStr = %s, m_key = %s, m_adaption = %s"),nRefStrCount, srcPhraseOfActivePile.c_str(),targetStrOfActivePile.c_str(), srcPhraseAdaption.c_str(), srcPhraseAdaption.c_str());
            wxLogDebug(_T("|***|App's m_targetPhrase = %s, m_Translation = %s, m_pTargetBox->GetValue() = %s"),targetPhraseOnApp.c_str(), translation.c_str(),tgtBoxValue.c_str());
            wxLogDebug(_T("|***|  SrcPhrase Flags: m_bHasKBEntry = %d, m_bNotInKB = %d, targetUnit->m_bAlwaysAsk = %s"), (int)hasKBEntry, (int)notInKB, alwaysAsk.c_str());
            wxLogDebug(_T("|***|  TargetBox Flags: m_bAbandonable = %d, m_bUserTypedSomething = %d, m_bRetainBoxContents = %d"),(int)abandonable, (int)userTypedSomething, (int)retainBoxContents);
            wxLogDebug(_T("|***********************************************************************************"));
            }
            // Above for debugging only!!!
            */

        }
        else // when nRefStrCount == 0
        {
            // whm 5Mar2018 Note: When nRefStrCount is 0, we are at a 'hole', and there are no translation 
            // equivalents in the dropdown's list. In this case we don't call PopulateDropDownList() here
            // (which automatically clears the dropdown list before re-populating it), but here also we must
            // make sure the dropdown list is empty, and set the dropdown's button image to its "disabled" 
            // state. 
            pApp->m_pTargetBox->ClearDropDownList();
            pApp->m_pTargetBox->SetButtonBitmaps(pApp->m_pTargetBox->dropbutton_blank, false, pApp->m_pTargetBox->dropbutton_blank, pApp->m_pTargetBox->dropbutton_blank, pApp->m_pTargetBox->dropbutton_blank);
            // With nothing in the dropdown list, we inform OnIdle() not to show the dropdown list
            // whm 18Apr2018 moved the setting of m_bChooseTransInitializePopup to TRUE/FALSE
            // above the if (nRefStrCount > 0) ... else blocks.
            //pApp->m_bChooseTransInitializePopup = FALSE;
            // A previous call to PlacePhraseBox() would have called DoGetSuitableText_ForPlacePhraseBox() which 
            // stored a suitable string str which was assigned to the App's m_targetPhrase member, and it would have
            // been followed by any AutoCaps processing. Hence, if m_bCopySource was TRUE the m_targetPhrase member
            // string will contain the copied source word/phrase processed by CopySourceKey(), which - if gbAutoCaps 
            // was also TRUE, the case of the copied source word/phrase will be preserved.

            //We should be able to put it in the m_pTargetBox here.
            pApp->m_pTargetBox->ChangeValue(pApp->m_targetPhrase);
            pApp->m_pTargetBox->SetSelection(-1, -1); // select all
            pApp->m_pTargetBox->SetFocus();

        }
        // Clear the current target unit pointer
        pApp->pCurTargetUnit = (CTargetUnit*)NULL; // clear the target unit in this nRefStrCount == 0 block too.
    }
}

int CPhraseBox::GetLineLength(long lineNo) // whm 14Feb2018 added (GetLineLength() is in wxTextCtrl but not wxOwnerDrawnComboBox)
{
    return GetTextCtrl()->GetLineLength(lineNo);
}

#if wxVERSION_NUMBER >= 2900
void CPhraseBox::OnComboProcessDropDownListOpen(wxCommandEvent& event)
{
    // Process a wxEVT_COMBOBOX_DROPDOWN event, which is generated when the 
    // list box part of the combo box is shown (drops down). Notice that this 
    // event is only supported by wxMSW, wxGTK with GTK+ 2.10 or later, and wxOSX/Cocoa
    if (gpApp->m_pTargetBox->GetCount() < 1)
    {
        event.Skip();
        this->CloseDropDown();
    }
    //bDropDownIsPoppedOpen = TRUE; // CAdapt_ItCanvas::OnScroll() needs to know whether the dropdown list if popped down or not
    //wxLogDebug(_T("OnComboProcessDropDownListOpen: Popup Open event"));
}

void CPhraseBox::OnComboProcessDropDownListCloseUp(wxCommandEvent& WXUNUSED(event))
{
    // Process a wxEVT_COMBOBOX_CLOSEUP event, which is generated when the list 
    // box of the combo box disappears (closes up). This event is only generated 
    // for the same platforms as wxEVT_COMBOBOX_DROPDOWN above. Also note that
    // only wxMSW and wxOSX/Cocoa support adding or deleting items in this event

    // CAdapt_ItCanvas::OnScroll() needs to know whether the dropdown list if popped down or not
    //bDropDownIsPoppedOpen = FALSE;
    //wxLogDebug(_T("OnComboProcessDropDownListCloseUp: Popup Close event"));
}
#endif


// This code based on PopulateList() in the CChooseTranslation dialog class.
// The pTU parameter is the pCurTargetUnit passed in from the dropdown setup
// code in PlaceBox(), which determined the value of pCurTargetUnit at the
// PlaceBox() location. If PlaceBox() was called immediately on the dismissal 
// of the Choose Translation dialog. pCurTargetUnit will reflect any changes 
// made to the composition or ordering of the ref strings in that dialog. 
// If pTU is not NULL, we use it herre to populate the dropdown list.
// The selectionIndex is a reference parameter and will return the index
// of the user-selected item, or the index of the user-entered new translation 
// string in the populated dropdown list.
// whm 4Mar2018 moved some of the code that previously determined the value of
// pCurTargetUnit from within this function back to the caller in the Layout's
// PlaceBox() function.
// whm 5Mar2018 refactored the setup and PopulateDropDownList() function to 
// better make use of BEW's original coding as it already handles the bulk of 
// the phrasebox contents (and KB access and storage) by the time code execution 
// calls PopulateDropDownList().
void CPhraseBox::PopulateDropDownList(CTargetUnit* pTU, int& selectionIndex, bool& bNoAdaptationFlagPresent, int& indexOfNoAdaptation)
{
    selectionIndex = -1; // initialize to inform caller if no selection was possible
    this->Clear();
    wxString initialBoxContent;
    // Get the initial phrasebox content so we can match it in dropdown list if it is there
    initialBoxContent = gpApp->m_targetPhrase;
    
    if (pTU == NULL)
    {
        return; // If the incoming pTU is null then just return with empty list
    }
    wxString s = _("<no adaptation>");

    // Populate the combobox's list contents with the translation or gloss strings 
    // stored in the global variable pCurTargetUnit (passed in as pTU).
    // Ignore any CRefString instances for which m_bDeleted is TRUE.
    CRefString* pRefString;
    TranslationsList::Node* pos = pTU->m_pTranslations->GetFirst();
    wxASSERT(pos != NULL);
    int count = 0;
    // initialize the reference parameters
    indexOfNoAdaptation = -1; // initialize to -1 or not found
    bNoAdaptationFlagPresent = FALSE;
    while (pos != NULL)
    {
        pRefString = (CRefString*)pos->GetData();
        pos = pos->GetNext();
        if (!pRefString->GetDeletedFlag())
        {
            // this one is not deleted, so show it to the user
            wxString str = pRefString->m_translation;
            // Note: If user clicked on <no adaptation> gpApp->m_targetPhrase and initialBoxContent 
            // will also be an empty string.
            if (str.IsEmpty())
            {
                str = s;
                bNoAdaptationFlagPresent = TRUE;
            }
            int nLocation = this->Append(str);
            if (bNoAdaptationFlagPresent)
            {
                // Get index of the <no adaptation> string in the dropdown list for use below
                indexOfNoAdaptation = nLocation;
            }
            count++;
            // The combobox's list is NOT sorted; the Append() command above returns the 
            // just-inserted item's index.
            // Note: the selectionIndex may be changed below if a new translation string was
            // appended to the dropdown list.
            wxASSERT(nLocation != -1); // we just added it so it must be there!
            this->SetClientData(nLocation, &pRefString->m_refCount);
        }
    }
    if (count > 0)
    {
        if (!initialBoxContent.IsEmpty())
        {
            // The phrasebox had an entry when we landed there (which could have been a copy of the source text)
            int indx = -1;
            indx = (int)this->FindString(initialBoxContent, FALSE);  // FALSE - not case sensitive
            if (indx != wxNOT_FOUND)
            {
                // Select the list item - if it exists in the list - that matches what was in the 
                // phrasebox when we landed there.
                selectionIndex = indx;
            }
        }
        else
        {
            // If the phrasebox had no content, then we just select first item in the list -
            // unless the phrasebox was empty because user clicked <no adaptation>, in which case 
            // we don't make a selection, but leave selectionIndex as -1 (as initialized at 
            // the beginning of this function). 
            if (indexOfNoAdaptation != -1)
            {
                // User clicked <no adaptation>, leave selectionIndex with value of -1 
                // the indexOfNoAdaptation will have the index of the <no adaptation> item
                // to inform the caller.
                ; 
            }
            else
            {
                selectionIndex = 0; // have caller select first in list (index 0)
            }
        }
    }
    // See notes in CChooseTranslation::OnOK().
    // If the ChooseTranslation dialog was just called up and a new translation string was entered
    // in that dialog and OK'ed, the m_Translation variable (defined in CPhraseBox) will contain
    // that new translation. Although the new translation will appear in the dropdown list, and
    // will appear in the dropdown's edit box (selected), the new translation doesn't get added to 
    // the KB until the phrasebox moves on with Enter or Tab.
    if (!m_Translation.IsEmpty())
    {
        // whm updated 5Mar2018 check for if the string already exists. If not, append it and set its
        // selectionIndex. If it already exists in the list don't append it again, but update its 
        // selectionIndex to its current index in the list.
        int foundItem = -1;
        foundItem = this->FindString(m_Translation);
        if (foundItem == wxNOT_FOUND)
        {
            selectionIndex = this->Append(m_Translation);
        }
        else
        {
            selectionIndex = foundItem;
        }
        m_Translation.Empty(); // We've used it so clear it
    }
}



// BEW 13Apr10, no changes needed for support of doc version 5
void CPhraseBox::OnLButtonDown(wxMouseEvent& event)
{
	// This mouse event is only activated when user clicks mouse L button within
	// the phrase box, not elsewhere on the screen [but see next comment]...
    // whm 10Apr2018 update: With the implementation of the new dropdown phrasebox, 
    // this OnLButtonDown() handler is not activated when the user clicks mouse L button 
    // within the phrasebox. But, surprisingly, it is triggered/executed when the user 
    // clicks on the dropdown control's down-arrow button.
    // Hence, currently neither this handler nor the OnLButtonDown() handler in the 
    // CAdapt_ItCanvas get triggered when the user simply clicks within the phrasebox
    // to remove the selection. This behavior is different than the behavior that was
    // expected for a phrasebox based on wxTextCtrl.
	CAdapt_ItApp* pApp = &wxGetApp();
	wxASSERT(pApp != NULL);

	// whm added 15Mar12. When in read-only mode don't register any key strokes
	if (pApp->m_bReadOnlyAccess)
	{
		// return without calling Skip(). Beep for read-only feedback
		::wxBell();
		return;
	}

    // whm addition: don't allow cursor to be placed in phrasebox when in free trans mode
    // and when it is not editable. Allows us to have a pink background in the phrase box
    // in free trans mode.
    // TODO? we could also make the text grayed out to more closely immulate MFC Windows
    // behavior (we could call Enable(FALSE) but that not only turns the text grayed out,
    // but also the background gray instead of the desired pink. It is better to do this
    // here than in OnLButtonUp since it prevents the cursor from being momemtarily seen in
    // the phrase box if clicked.
	if (pApp->m_bFreeTranslationMode && !this->GetTextCtrl()->IsEditable())
	{
		CMainFrame* pFrame;
		pFrame = pApp->GetMainFrame();
		wxASSERT(pFrame != NULL);
		wxASSERT(pFrame->m_pComposeBar != NULL);
		wxTextCtrl* pEdit = (wxTextCtrl*)
							pFrame->m_pComposeBar->FindWindowById(IDC_EDIT_COMPOSE);
		wxASSERT(pEdit != NULL);
		pEdit->SetFocus();
		int len = pEdit->GetValue().Length();
		if (len > 0)
		{
			pEdit->SetSelection(len,len);
		}
		::wxBell();
		// don't call Skip() as we don't want the mouse click processed elsewhere
		return;
	}
    // version 1.4.2 and onwards, we want a right or left arrow used to remove the
    // phrasebox's selection to be considered a typed character, so that if a subsequent
    // selection and merge is done then the first target word will not get lost; and so we
    // add a block of code also to start of OnChar( ) to check for entry with both
    // m_bAbandonable and m_bUserTypedSomething set TRUE, in which case we don't clear
    // these flags (the older versions cleared the flags on entry to OnChar( ) )

    // we use this flag cocktail elsewhere to test for these values of the three flags as
    // the condition for telling the application to retain the phrase box's contents when
    // user deselects target word, then makes a phrase and merges by typing
	m_bAbandonable = FALSE;
	pApp->m_bUserTypedSomething = TRUE;
    m_bRetainBoxContents = TRUE;
    wxLogDebug(_T("CPhraseBox::OnLButtonDown() triggered"));
	event.Skip();
    GetTextCtrl()->GetSelection(&pApp->m_nStartChar,&pApp->m_nEndChar);
}

// BEW 13Apr10, no changes needed for support of doc version 5
// whm 10Apr2018 update: With the implementation of the new dropdown phrasebox, 
// this OnLButtonUp() handler is not activated when the user clicks mouse L button 
// and releases it within the phrasebox. But, like the OnLButtonDown() handler - see 
// comment in OnLButtonDown() above - this OnLButtonUp() is triggered/executed 
// when the user clicks briefly on the dropdown control's down-arrow button and 
// releases - but OnLButtonUp() doesn't seem to be triggered after a long-press of
// the mouse on the down-arrow button. 
// Hence, currently neither this handler nor the OnLButtonDown() handler in the 
// CAdapt_ItCanvas get triggered when the user simply clicks within the phrasebox
// to remove the selection. This behavior is different than the behavior that was
// expected for a phrasebox based on wxTextCtrl in which the same handlers are
// triggered when the user clicks within the phrasebox to remove a selection.
// Hence, 
void CPhraseBox::OnLButtonUp(wxMouseEvent& event)
{
	CAdapt_ItApp* pApp = &wxGetApp();
	// This mouse event is only activated when user clicks mouse L button within
	// the phrase box, not elsewhere on the screen
    wxLogDebug(_T("CPhraseBox::OnLButtonUp() triggered"));
    event.Skip();
	GetTextCtrl()->GetSelection(&pApp->m_nStartChar,&pApp->m_nEndChar);
}

void CPhraseBox::OnComboItemSelected(wxCommandEvent & WXUNUSED(event))
{
    // This is only called when a list item is selected, not when Enter pressed within the dropdown's edit box
    wxString s;
    // IDS_NO_ADAPTATION
    s = s.Format(_("<no adaptation>")); // get "<no adaptation>" ready in case needed

    wxString selItemStr;
    selItemStr = this->GetValue();
    if (selItemStr == s)
    {
        selItemStr = _T(""); // restore null string
        m_bEmptyAdaptationChosen = TRUE; // set the m_bEmptyAdaptationChosen global used by PlacePhraseBox
    }

    //#if defined(FWD_SLASH_DELIM)
    // BEW added 23Apr15 - in case the user typed a translation manually (with / as word-delimiter)
    // convert any / back to ZWSP, in case KB storage follows. If the string ends up in m_targetBox
    // then the ChangeValue() call within CPhraseBox will convert the ZWSP instances back to forward
    // slashes for display, in case the user does subsequent edits there
    selItemStr = FwdSlashtoZWSP(selItemStr);
    //#endif

    if (gbAutoCaps && gbSourceIsUpperCase)
    {
        bool bNoError = gpApp->GetDocument()->SetCaseParameters(selItemStr, FALSE);
        if (bNoError && !gbNonSourceIsUpperCase && (gcharNonSrcUC != _T('\0')))
        {
            // make it upper case
            selItemStr.SetChar(0, gcharNonSrcUC);
        }
    }

    gpApp->m_targetPhrase = selItemStr;
    this->GetTextCtrl()->ChangeValue(selItemStr); // use WriteText() instead of ChangeValue() or SetValue() since the later two reset the IsModified() to FALSE
    
    if (!this->GetTextCtrl()->IsModified()) // need to call SetModified on m_pTargetBox before calling SetValue // whm 14Feb2018 added GetTextCtrl()->
    {
        this->GetTextCtrl()->SetModified(TRUE); // Set as modified so that CPhraseBox::OnPhraseBoxChanged() will so its work // whm 14Feb2018 added GetTextCtrl()->
    }

    this->m_bAbandonable = FALSE; // this is done in CChooseTranslation::OnOK()

    // whm 26Feb2018 addition to OnComboItemSelected() to cause the phrasebox to move on when 
    // an item is selected from the dropdown list, we can do the same thing here that is done 
    // when the WXK_RETURN key press is detected in the OnChar() handler, namely just 
    // set m_nOldSequNum to the m_nActiveSequNum and call JumpForward(pView).
    // Code below copied from the case WXK_RETURN in OnChar().
    CLayout* pLayout = GetLayout();
    CAdapt_ItView* pView = pLayout->m_pView;
    // save old sequ number in case required for toolbar's Back button
    gpApp->m_nOldSequNum = gpApp->m_nActiveSequNum;
    JumpForward(pView);
}

// This OnMeasureItem() function implements the virtual function in wxOwnerDrawnComboBox
// It returns the height of the list item according to the vertical text extent
wxCoord CPhraseBox::OnMeasureItem(size_t item) const
{
    // Get the text extent height of the combobox item - all should have same height
    // Without OnMeasureItem being defined, the combo box items are vertically too close 
    // together with descenders clipped. This spaces them vertically better.
    wxClientDC dC((wxWindow*)gpApp->GetMainFrame()->canvas);
    wxFont* pFont;
    wxSize textExtent;
    if (gbIsGlossing && gbGlossingUsesNavFont)
        pFont = gpApp->m_pNavTextFont;
    else
        pFont = gpApp->m_pTargetFont;
    wxFont SaveFont = dC.GetFont();

    dC.SetFont(*pFont);
    wxString thePhrase = this->GetString(item);
    dC.GetTextExtent(thePhrase, &textExtent.x, &textExtent.y); // measure using the current font

    return wxCoord(textExtent.GetY()+2);
}

// return TRUE if we made a match and there is a translation to be inserted (see static var
// below); return FALSE if there was no match; based on LookAhead(), but only looks up a
// single src word at pNewPile; assumes that the app member, m_nActiveSequNum is set and
// that the CPile which is at that index is the pNewPile which was passed in
// BEW 26Mar10, no changes needed for support of doc version 5
// BEW 21Jun10: simplified signature
// BEW 21Jun10: changed to support kbVersion 2's m_bDeleted flag
// BEW 6July10, added test for converting a looked-up <Not In KB> string to an empty string
bool CPhraseBox::LookUpSrcWord(CPile* pNewPile)
{
	// refactored 2Apr09
	CAdapt_ItApp* pApp = &wxGetApp();
	CAdapt_ItView *pView = pApp->GetView(); // <<-- BEWARE if we later have multiple views/panes
	CLayout* pLayout = pApp->m_pLayout;
	wxString strNot = _T("<Not In KB>");
	//int	nNewSequNum; // set but not used
	//nNewSequNum = pNewPile->GetSrcPhrase()->m_nSequNumber; // sequ number at the new location
	wxString	phrases[1]; // store built phrases here, for testing
							// against KB stored source phrases
	//int	numPhrases; // set but not used
	//numPhrases = 1;  // how many phrases were built in any one call of this function
    m_Translation.Empty(); // clear the static variable, ready for a new translation
						 // if one can be found
	m_nWordsInPhrase = 0;	  // assume no match
	m_bBoxTextByCopyOnly = FALSE; // restore default setting

	// we should never have an active selection at this point, so ensure it
	pView->RemoveSelection();

	// get the source word
	phrases[0] = pNewPile->GetSrcPhrase()->m_key;
	// BEW added 08Sep08: to prevent spurious words being inserted at the
	// end of a long retranslation when  mode is glossing mode
	if (pNewPile->GetSrcPhrase()->m_key == _T("..."))
	{
		// don't allow an ellipsis (ie. placeholder) to trigger an insertion,
		// leave m_Translation empty
        m_Translation.Empty();
		return TRUE;
	}

    // check this phrase (which is actually a single word), attempting to find a match in
    // the KB (if glossing, it might be a single word or a phrase, depending on what user
    // did earlier at this location before he turned glossing on)
	CKB* pKB;
	if (gbIsGlossing)
		pKB = pApp->m_pGlossingKB;
	else
		pKB = pApp->m_pKB;
	CTargetUnit* pTargetUnit = (CTargetUnit*)NULL;
	int index = 0;
	bool bFound = FALSE;
	// index is always 0 in this function, so lookup is only in the first map
	bFound = pKB->FindMatchInKB(index + 1, phrases[index], pTargetUnit);

	// if no match was found, we return immediately with a return value of FALSE
	if (!bFound)
	{
		pApp->pCurTargetUnit = (CTargetUnit*)NULL; // the global pointer must be cleared
		m_CurKey.Empty(); // global var m_CurKey not needed, so clear it
		return FALSE;
	}
    // whm 10Jan2018 Note: We do not call the ChooseTranslation dialog from LookAhead()
    // now that the choose translation feature is implemented in the CPhraseBox's dropdown
    // list. Hence, we should not set pCurTargetUnit here, since it should only be set
    // from the View's OnButtonChooseTranslation() handler.
    //pCurTargetUnit = pTargetUnit; // set global pointer so the dialog can use it if it is called
	m_CurKey = phrases[index]; // set the global m_CurKey so the dialog can use it if it is called
							 // (phrases[0] is copied for the lookup, so m_CurKey has initial case
							 // setting as in the doc's sourcephrase instance; we don't need
							 // to change it here (if ChooseTranslation( ) is called, it does
							 // any needed case changes internally)
	m_nWordsInPhrase = index + 1; // static variable, needed for merging source phrases in
								// the caller
    // BEW 21Jun10, for kbVersion 2 support, count the number of non-deleted CRefString
    // instances stored on this pTargetUnit
    int count =  pTargetUnit->CountNonDeletedRefStringInstances();
	if (count == 0)
	{
		// nothing in the KB for this key (except, possibly, one or more deleted
		// CRefString instances)
		pApp->pCurTargetUnit = (CTargetUnit*)NULL;
		m_CurKey.Empty();
		return FALSE;
	}

	// we found a target unit in the list with a matching m_key field,
	// so we must now set the static var translation to the appropriate adaptation text: this
	// will be the target unit's first entry in its list if the list has only one entry, else
	// we must present the user with a dialog to put up all possible adaptations for the user to
	// choose one, or type a new one, or reject all - in which case we return FALSE for manual
	// typing of an adaptation etc.
	// BEW 21Jun10, changed to support kbVersion 2's m_bDeleted flag. It is now possible
	// that a CTargetUnit have just a single CRefString instance and the latter has its
	// m_bDeleted flag set TRUE. In this circumstance, matching this is to be regarded as
	// a non-match, and the function then would need to return FALSE for a manual typing
	// of the required adaptation (or gloss)
	TranslationsList::Node* pos = pTargetUnit->m_pTranslations->GetFirst();
	wxASSERT(pos != NULL);

	wxASSERT(count > 0);
	if (count > 1 || pTargetUnit->m_bAlwaysAsk)
	{
		// move view to new location and select, so user has visual feedback)
		// about which element(s) is/are involved
		pView->RemoveSelection();

		// next code is taken from end of MoveToNextPile()
		// initialize the phrase box to be empty, so as not to confuse the user
		ChangeValue(_T(""));
		pApp->m_targetPhrase = _T("");

		// recalculate the layout
#ifdef _NEW_LAYOUT
		pLayout->RecalcLayout(pApp->m_pSourcePhrases, keep_strips_keep_piles);
#else
		pLayout->RecalcLayout(pApp->m_pSourcePhrases, create_strips_keep_piles);
#endif
		// get the new active pile
		pApp->m_pActivePile = pView->GetPile(pApp->m_nActiveSequNum);
		wxASSERT(pApp->m_pActivePile != NULL);

		// scroll into view
		pApp->GetMainFrame()->canvas->ScrollIntoView(pApp->m_nActiveSequNum);

		// make what we've done visible
		pView->Invalidate();
		pLayout->PlaceBox();

        // whm 10Jan2018 removed code here that originally called up the
        // Choose Translation dialog from within LookUpScrWord(). The above 
        // PlaceBox() call will display the dropdown's list of translations that can
        // be chosen from, so that calling CSourcePhrase::ChooseTranslation() was
        // no longer relevant.
	}
	else
	{
		// BEW 21Jun10, count has the value 1, but there could be deleted CRefString
		// intances also, so we must search to find the first non-deleted instance
		CRefString* pRefStr = NULL;
		while (pos != NULL)
		{
			pRefStr = pos->GetData();
			pos = pos->GetNext();
			if (!pRefStr->GetDeletedFlag())
			{
				// the adaptation string returned could be a "<Not In KB>" string, which
				// is something which never must be put into the phrase box, so check for
				// this and change to an empty string if that was what was fetched by the
				// lookup
                m_Translation = pRefStr->m_translation;
				if (m_Translation == strNot)
				{
					// change "<Not In KB>" to an empty string
                    m_Translation.Empty();
				}
				break;
			}
		}
	}

	// adjust for case, if necessary; this algorithm will not make a lower case string start
	// with upper case when the source is uppercase if the user types punctuation at the start
	// of the string. The latter is, however, unlikely - provided the auto punctuation support
	// is utilized by the user
	if (gbAutoCaps && gbSourceIsUpperCase)
	{
		bool bNoError = pApp->GetDocument()->SetCaseParameters(m_Translation, FALSE);
		if (bNoError && !gbNonSourceIsUpperCase && (gcharNonSrcUC != _T('\0')))
		{
			// make it upper case
            m_Translation.SetChar(0, gcharNonSrcUC);
		}
	}
	return TRUE;
}

// BEW 13Apr10, no changes needed for support of doc version 5
void CPhraseBox::OnEditUndo(wxCommandEvent& WXUNUSED(event))
// no changes needed for support of glossing or adapting
{
    // We have to implement undo for a backspace ourselves, but for delete & edit menu
    // commands the CEdit Undo() will suffice; we use a non-empty m_backspaceUndoStr as a
    // flag that the last edit operation was a backspace
	CAdapt_ItApp* pApp = GetLayout()->m_pApp;
	CAdapt_ItView* pView = pApp->GetView();


	if (m_backspaceUndoStr.IsEmpty())
	{
		// last operation was not a <BS> keypress,
		// so Undo() will be done instead
		;
	}
	else
	{
		if (!(m_nSaveStart == -1 || m_nSaveEnd == -1))
		{
			bool bRestoringAll = FALSE;
			wxString thePhrase;
			thePhrase = GetValue();
			int undoLen = m_backspaceUndoStr.Length();
			if (!thePhrase.IsEmpty())
			{
				thePhrase = InsertInString(thePhrase,(int)m_nSaveEnd,m_backspaceUndoStr);
			}
			else
			{
				thePhrase = m_backspaceUndoStr;
				bRestoringAll = TRUE;
			}

			// rebuild the box the correct size
			wxSize textExtent;
			bool bWasMadeDirty = TRUE;
			FixBox(pView,thePhrase,bWasMadeDirty,textExtent,1); // selector = 1 for using
																// thePhrase's extent
			// restore the box contents
			ChangeValue(thePhrase);
			m_backspaceUndoStr.Empty(); // clear, so it can't be mistakenly undone again

			// fix the cursor location
			if (bRestoringAll)
			{
				pApp->m_nStartChar = -1;
				pApp->m_nEndChar = -1;
				SetSelection(pApp->m_nStartChar,pApp->m_nEndChar); // all selected
			}
			else
			{
				pApp->m_nStartChar = pApp->m_nEndChar = (int)(m_nSaveStart + undoLen);
				SetSelection(pApp->m_nStartChar,pApp->m_nEndChar);
			}
		}
	}
}

// DoStore_ForPlacePhraseBox added 3Apr09; it factors out some of the incidental
// complexity in the PlacePhraseBox() function, making the latter's design more
// transparent and the function shorter
// BEW 22Feb10, no changes needed for support of doc version 5
// BEW 21Jun10, no changes needed for support of kbVersion 2
// BEW 17Jul11, changed for GetRefString() to return KB_Entry enum, and use all 10 maps
// for glossing KB
bool CPhraseBox::DoStore_ForPlacePhraseBox(CAdapt_ItApp* pApp, wxString& targetPhrase)
{
	CAdapt_ItDoc* pDoc = pApp->GetDocument();
	bool bOK = TRUE;
	CRefString* pRefStr = NULL;
	KB_Entry rsEntry;
	// Restore user's choice for the command bar button ID_BUTTON_NO_PUNCT_COPY
	pApp->m_bCopySourcePunctuation = pApp->m_pTargetBox->m_bCurrentCopySrcPunctuationFlag;

	if (gbIsGlossing)
	{
		if (targetPhrase.IsEmpty())
			pApp->m_pActivePile->GetSrcPhrase()->m_gloss = targetPhrase;

		// store will fail if the user edited the entry out of the glossing KB, since it
		// cannot know which srcPhrases will be affected, so these will still have their
		// m_bHasKBEntry set true. We have to test for this, ie. a null pRefString but
		// the above flag TRUE is a sufficient test, and if so, set the flag to FALSE
		rsEntry = pApp->m_pGlossingKB->GetRefString(pApp->m_pActivePile->GetSrcPhrase()->m_nSrcWords,
								pApp->m_pActivePile->GetSrcPhrase()->m_key, targetPhrase, pRefStr);
		if ((pRefStr == NULL || rsEntry == present_but_deleted) &&
			pApp->m_pActivePile->GetSrcPhrase()->m_bHasGlossingKBEntry)
		{
			pApp->m_pActivePile->GetSrcPhrase()->m_bHasGlossingKBEntry = FALSE;
		}
		bOK = pApp->m_pGlossingKB->StoreText(pApp->m_pActivePile->GetSrcPhrase(), targetPhrase);
	}
	else // is adapting
	{
		if (targetPhrase.IsEmpty())
		{
			pApp->m_pActivePile->GetSrcPhrase()->m_adaption = targetPhrase; 
			pApp->m_pActivePile->GetSrcPhrase()->m_targetStr = targetPhrase;
			pApp->m_bInhibitMakeTargetStringCall = TRUE;
			bOK = pApp->m_pKB->StoreText(pApp->m_pActivePile->GetSrcPhrase(), targetPhrase);
			pApp->m_bInhibitMakeTargetStringCall = FALSE;
		}
		else
		{
			// re-express the punctuation
			wxString copiedTargetPhrase = targetPhrase; // copy, with any punctuation user typed as well
			// targetPhrase now lacks punctuation - so it can be used for storing in the KB
			pApp->GetView()->RemovePunctuation(pDoc, &targetPhrase, from_target_text);

			// The store would fail if the user edited the entry out of the KB, as the latter
			// cannot know which srcPhrases will be affected, so these would still have their
			// m_bHasKBEntry set true. We have to test for this, and the m_HasKBEntry flag TRUE 
			// is a sufficient test, and if so, set the flag to FALSE, to enable the store op
			rsEntry = pApp->m_pKB->GetRefString(pApp->m_pActivePile->GetSrcPhrase()->m_nSrcWords,
				pApp->m_pActivePile->GetSrcPhrase()->m_key, targetPhrase, pRefStr);
			if ((pRefStr == NULL || rsEntry == present_but_deleted) &&
				pApp->m_pActivePile->GetSrcPhrase()->m_bHasKBEntry)
			{
				pApp->m_pActivePile->GetSrcPhrase()->m_bHasKBEntry = FALSE;
			}

			pApp->m_pActivePile->GetSrcPhrase()->m_targetStr = wxEmptyString; // start off empty

			pApp->m_bInhibitMakeTargetStringCall = TRUE;
			bOK = pApp->m_pKB->StoreText(pApp->m_pActivePile->GetSrcPhrase(), targetPhrase);
			pApp->m_bInhibitMakeTargetStringCall = FALSE;

			pApp->m_bCopySourcePunctuation = pApp->m_pTargetBox->m_bCurrentCopySrcPunctuationFlag;
			// Now use the user's actual phrasebox contents, with any puncts he may have typed
			pApp->GetView()->MakeTargetStringIncludingPunctuation(pApp->m_pActivePile->GetSrcPhrase(), copiedTargetPhrase);

			// Now restore the flag to its default value
			pApp->m_bCopySourcePunctuation = TRUE;
			pApp->m_pTargetBox->m_bCurrentCopySrcPunctuationFlag = TRUE;
		}
	}
	return bOK;
}

// BEW refactored 21Jul14 for support of ZWSP storage and replacement;
// also moved the definition to be in PhraseBox.h & .cpp (was in view class)
void CPhraseBox::RemoveFinalSpaces(CPhraseBox* pBox, wxString* pStr)
{
	// empty strings don't need anything done
	if (pStr->IsEmpty())
		return;

	// remove any phrase final space characters
	bool bChanged = FALSE;
	int len = pStr->Length();
	int nIndexLast = len-1;
	// BEW 21Jul14 refactored for ZWSP support. The legacy code can be left unchanged for
	// handling latin space; but for exotic spaces we'll use the overridden
	// RemoveFinalSpaces() as its for any string - so test here for what is at the end.
	// We'll assume the end doesn't have a mix of latin space with exotic ones
	if (pStr->GetChar(nIndexLast) == _T(' '))
	{
		// Latin space is at the end, so do the legacy code
		do {
			if (pStr->GetChar(nIndexLast) == _T(' '))
			{
				// Note: wsString::Remove must have the second param as 1 here otherwise
				// it will truncate the remainder of the string!
				pStr->Remove(nIndexLast,1);
				// can't trust the Remove's returned value, it exceeds string length by one
				len = pStr->Length();
				nIndexLast = len -1;
				bChanged = TRUE;
			}
			else
			{
				break;
			}
		} while (len > 0 && nIndexLast > -1);
	}
	else
	{
		// There is no latin space at the end, but there might be one or more exotic ones,
		// such as ZWSP. (We'll assume there's no latin spaces mixed in with them)
		wxChar lastChar = pStr->GetChar(nIndexLast);
		CAdapt_ItApp* pApp = &wxGetApp();
		CAdapt_ItDoc* pDoc = pApp->GetDocument();
		if (pDoc->IsWhiteSpace(&lastChar))
		{
			// There must be at least one exotic space at the end, perhaps a ZWSP
			bChanged = TRUE;
			wxString revStr = *pStr; // it's not yet reversed, but will be in the next call
									 // and restored to non-reversed order before its returned
			RemoveFinalSpaces(revStr); // signature is ref to wxString

			*pStr = revStr;
			// pBox will have had its contents changed by at least one wxChar being
			// chopped off the end, so let the bChanged block below do the phrasebox update
		}
		else
		{
			// There is no exotic space at the end either, so pStr needs nothing removed,
			// so just return without changing the phrasebox contents
			return;
		}
	}
	if (bChanged) // need to do this, because for some reason rubbish is getting
            // left in the earlier box when the ChooseTranslation dialog gets put up. That
            // is, a simple call of SetWindowText with parameter pStr cast to (const char
            // *) doesn't work right; but the creation & setting of str below fixes it
	{
		wxString str = *pStr;
		pBox->ChangeValue(str);
	}
}

// BEW added 30Apr08, an overloaded version which deletes final spaces in any CString's
// text, and if there are only spaces in the string, it reduces it to an empty string
// BEW 21Jul14, refactored to also remove ZWSP and other exotic white spaces from end of
// the string as well; and moved to be in PhaseBox.h & .cpp (was in view class)
void CPhraseBox::RemoveFinalSpaces(wxString& rStr)
{
    // whm Note: This could be done with a single line in wx, i.e., rStr.Trim(TRUE), but
    // we'll go with the MFC version for now.
	if (rStr.IsEmpty())
		return;
	rStr = MakeReverse(rStr);
	wxChar chFirst = rStr[0];
	if (chFirst == _T(' '))
	{
		// The legacy code - just remove latin spaces, we'll assume that when this is apt,
		// there are no exotics there as well, such as ZWSP
		while (chFirst == _T(' '))
		{
			rStr = rStr.Mid(1);
			chFirst = rStr[0];
		}
		if (rStr.IsEmpty())
			return;
		else
			rStr = MakeReverse(rStr);
	}
	else
	{
		// BEW 21Jul14 new code, to support ZWSP removals, etc, from end
		// we reversed rStr, so chFirst is the last
		CAdapt_ItApp* pApp = &wxGetApp();
		CAdapt_ItDoc* pDoc = pApp->GetDocument();
		if (pDoc->IsWhiteSpace(&chFirst))
		{
			// There is an exotic space at the end, remove it and do any more
			rStr = rStr.Mid(1);
			if (rStr.IsEmpty())
			{
				return;
			}
			chFirst = rStr[0];
			while (pDoc->IsWhiteSpace(&chFirst))
			{
				rStr = rStr.Mid(1);
				if (rStr.IsEmpty())
				{
					return;
				}
				chFirst = rStr[0];
			}
			rStr = MakeReverse(rStr);
			return;
		}
		else
		{
			// No exotic space at the end, so re-reverse & return string
			rStr = MakeReverse(rStr);
		}
		return;
	}
}
//#if defined(FWD_SLASH_DELIM)
// BEW 23Apr15 functions for support of / as word-breaking whitespace, with
// conversion to ZWSP in strings not accessible to user editing, and replacement
// of ZWSP with / for those strings which are user editable; that is, when
// putting a string into the phrasebox, we restore / delimiters, when getting
// the phrasebox string for some purpose, we replace all / with ZWSP

void CPhraseBox::ChangeValue(const wxString& value)
{
	// uses function from helpers.cpp
	wxString convertedValue = value; // needed due to const qualifier in signature
	convertedValue = ZWSPtoFwdSlash(convertedValue); // no changes done if m_bFwdSlashDelimiter is FALSE
    //wxTextCtrl::ChangeValue(convertedValue);
    this->GetTextCtrl()->ChangeValue(convertedValue); // whm 14Feb2018 added this->GetTextCtrl()->
}

//wxString CPhraseBox::GetValue2()
//{
//	wxString s = GetValue();
//	// uses function from helpers.cpp
//	s = FwdSlashtoZWSP(s); // no changes done if m_bFwdSlashDelimiter is FALSE
//	return s;
//}

//#endif

