/////////////////////////////////////////////////////////////////////////////
/// \project		adaptit
/// \file			Adapt_ItCanvas.cpp
/// \author			Bill Martin
/// \date_created	05 January 2004
/// \rcs_id $Id$
/// \copyright		2008 Bruce Waters, Bill Martin, SIL International
/// \license		The Common Public License or The GNU Lesser General Public
///                 License (see license directory)
/// \description	This is the implementation file for the CAdapt_ItCanvas class.
/// The CAdapt_ItCanvas class implements the main Adapt It window based on
/// wxScrolledWindow. This is required because wxWidgets' doc/view framework
/// does not have an equivalent for the CScrolledView in MFC.
/// \derivation		The CAdapt_ItCanvas class is derived from wxScrolledWindow.
/////////////////////////////////////////////////////////////////////////////

//#define _debugLayout

// uncomment out next line in order to turn on wxLogDebug calls in ScrollIntoView()
//#define DEBUG_ScrollIntoView

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma implementation "Adapt_ItCanvas.h"
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
#include <wx/docview.h>	// lost a lot of time trying to track down obscure compillation
						// errors after forgetting to include this! It is needed even
						// for the class to be able to recognize wxView and wxDocument.
#ifdef _USE_SPLITTER_WINDOW
#include <wx/splitter.h> // for wxSplitterWindow
#endif

#include "Adapt_It.h"
#include "Adapt_ItDoc.h"
#include "EarlierTranslationDlg.h"
#include "MainFrm.h"
#include "Adapt_ItView.h"
#include "Adapt_ItCanvas.h"
//#include "SourceBundle.h"
#include "Cell.h"
#include "Pile.h"
#include "Strip.h"
#include "helpers.h"
#include "Layout.h"
#include "NoteDlg.h"
#include "ViewFilteredMaterialDlg.h"
#include "FreeTrans.h"
#include "ChooseTranslation.h"


//#define _FT_ADJUST
//#define CAPTUREMOUSE

/// This global is defined in Adapt_ItView.cpp (for vertical edit functionality)
extern bool gbVerticalEditInProgress;

/// This global is defined in Adapt_ItView.cpp
extern EditStep gEditStep;

/// This global is defined in Adapt_ItView.cpp
extern EditRecord gEditRecord;

/// This global is defined in FindReplace.cpp.
extern bool gbReplaceAllIsCurrent;

extern bool gbHaltedAtBoundary;

/// This global is defined in Adapt_ItView.cpp.
extern bool	gbIsGlossing;

/// This global is defined in Adapt_It.cpp.
extern CPile* gpNotePile;

/// This global is defined in Adapt_It.cpp.
extern wxPoint gptLastClick;

///GDLC Removed 2010-02-12 because it is no longer used anywhere
/// This global is defined in Adapt_It.cpp.
//extern bool gbBundleStartIteratingBack;

/// This global is defined in Adapt_ItView.cpp.
extern bool	gbShowTargetOnly;

extern const wxChar* filterMkr;
extern const wxChar* filterMkrEnd;

/// This global is defined in Adapt_It.cpp.
extern CPile* gpGreenWedgePile;

/// This global is defined in Adapt_It.cpp.
extern bool gbSuppressSetup;

//extern wxString gFreeTranslationStr;

/// The global gpCurFreeTransSectionPileArray was defined in Adapt_It.cpp, but was changed to a member variable
/// of the class CFreeTrans. GDLC 2010-02-16

extern bool	gbFindIsCurrent;
extern bool gbFindOrReplaceCurrent;
extern int gnRetransEndSequNum; // sequ num of last srcPhrase in a matched retranslation
extern bool gbJustReplaced;
extern bool gbSaveSuppressFirst; // save the toggled state of the lines in the strips (across Find or
extern bool gbSaveSuppressLast;  // Find and Replace operations)
extern wxRect grectViewClient;

/// This global is defined in Adapt_It.cpp.
extern bool	gbRTL_Layout;	// ANSI version is always left to right reading; this flag can only
							// be changed in the Unicode version, using the extra Layout menu

/// This global is defined in MainFrm.cpp.
extern bool gbIgnoreScriptureReference_Receive;

/// This global is defined in MainFrm.cpp.
extern bool gbIgnoreScriptureReference_Send;

/// This global is defined in Adapt_It.cpp.
extern int ID_BMTOGGLEBUTTON_PHRASEBOX;

/// This global is defined in Adapt_It.cpp.
extern int ID_DROP_DOWN_LIST;

/// This global is defined in Adapt_It.cpp.
extern CAdapt_ItApp* gpApp; // for rapid access to the app class

// IMPLEMENT_CLASS(CAdapt_ItCanvas, wxScrolledWindow)
IMPLEMENT_DYNAMIC_CLASS(CAdapt_ItCanvas, wxScrolledWindow)

// For drawing lines in a canvas
float xpos = -1;
float ypos = -1;


// event handler table
BEGIN_EVENT_TABLE(CAdapt_ItCanvas, wxScrolledWindow)
    EVT_PAINT(CAdapt_ItCanvas::OnPaint) // whm added 28May07

    // whm 12Jul2018 Note: The the events for the following event handlers 
    // are caught here in CAdapt_ItCanvas, but the handlers below simply
    // call the public handlers of the same name in CPhraseBox:
    EVT_TOGGLEBUTTON(ID_BMTOGGLEBUTTON_PHRASEBOX, CAdapt_ItCanvas::OnTogglePhraseBoxButton) // detected here but handled by calling CPhraseBox::OnTogglePhraseBoxButton()
    EVT_LISTBOX(ID_DROP_DOWN_LIST, CAdapt_ItCanvas::OnListBoxItemSelected) // detected here but handled by calling CPhraseBox::OnTogglePhraseBoxButton()
    EVT_LISTBOX_DCLICK(ID_DROP_DOWN_LIST, CAdapt_ItCanvas::OnListBoxItemSelected) // detected here but handled by calling CPhraseBox::OnTogglePhraseBoxButton()

	// wx Note: wxScrollEvent only appears to intercept scroll events for scroll bars manually
	// placed in wxWindow based windows. In order to handle scroll events for windows like
	// wxScrolledWindow, we must use wxScrollWinEvent in the functions and EVT_SCROLLWIN macro
	// here.
    EVT_SCROLLWIN(CAdapt_ItCanvas::OnScroll)
	EVT_LEFT_DOWN(CAdapt_ItCanvas::OnLButtonDown)
	EVT_LEFT_UP(CAdapt_ItCanvas::OnLButtonUp)
	EVT_MOTION(CAdapt_ItCanvas::OnMouseMove) // whm added to activate OnMouseMove
END_EVENT_TABLE()

CAdapt_ItCanvas::CAdapt_ItCanvas()
{
}

#ifdef _USE_SPLITTER_WINDOW
CAdapt_ItCanvas::CAdapt_ItCanvas(wxSplitterWindow *splitter,
	const wxPoint& pos, const wxSize& size, const long style)
	: wxScrolledWindow(splitter, wxID_ANY, pos, size, style)
#else
CAdapt_ItCanvas::CAdapt_ItCanvas(CMainFrame *frame,
	const wxPoint& pos, const wxSize& size, const long style)
	: wxScrolledWindow(frame, wxID_ANY, pos, size, style)
#endif
{
	pView = NULL; // pView is set in the View's OnCreate() method Make CAdapt_ItCanvas'
				  // view pointer point to incoming view pointer
	pFrame = NULL; // pFrame is set in CMainFrame's creator
}

CAdapt_ItCanvas::~CAdapt_ItCanvas(void)
{
}

// event handling functions

void CAdapt_ItCanvas::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	if (gpApp->m_bReadOnlyAccess)
	{
		// make the background be an insipid red colour
		wxColour backcolor(255,225,232,wxALPHA_OPAQUE);
		this->SetOwnBackgroundColour(backcolor);
	}
	else
	{
		wxColour backcolor(255,255,255,wxALPHA_OPAQUE); // white
		this->SetOwnBackgroundColour(backcolor);
		//this->SetOwnBackgroundColour(wxNullColour);
	}
	wxPaintDC paintDC(this);//wxAutoBufferedPaintDC paintDC(this);
	// whm 9Jun07 Note: use of wxAutoBufferedPaintDC() is supposed to recognize when
	// double buffering is being done by the system's graphics primitives, and avoids
	// adding another buffered layer. Using it here did not affect wxMac's problem
	// of failure to paint properly after scrolling.

	// whm modified conditional test below to include && !__WXGTK__ after finding that
	// a release build on Ubuntu apparently defined wxUSE_GRAPHICS_CONTEXT and got
	// link errors for "undefined reference wxGCDC::...
	// whm 22Jun12 set all ports to use the wxDC &dc = paintDC under wxWidgets 2.9.3
//#if wxUSE_GRAPHICS_CONTEXT && !__WXMSW__ && !__WXGTK__
//     wxGCDC gdc( paintDC ) ;
//   wxDC &dc = m_useContext ? (wxDC&) gdc : (wxDC&) paintDC ;
//#else
    wxDC &dc = paintDC ;
//#endif
	if (!dc.IsOk())// using dc to avoid compiler warning
	{
		wxLogDebug(_T("canvas OnPaint() reports dc is not Ok!"));
	}

//	wxLogDebug(_T("%s:%s():line %d, m_bFreeTranslationMode = %s"), __FILE__, __FUNCTION__, __LINE__,
//		(&wxGetApp())->m_bFreeTranslationMode ? _T("TRUE") : _T("FALSE"));

    DoPrepareDC(paintDC); // PrepareDC() now calls DoPrepareDC()

	if (pView)
	{
#if defined(EditRetransCrash)  && defined(_DEBUG)
		wxLogDebug(_T("OnPaint():  pView->OnDraw() begins now"));
#endif
		pView->OnDraw(& paintDC);
#if defined(EditRetransCrash)  && defined(_DEBUG)
		wxLogDebug(_T("OnPaint():  pView->OnDraw() has ended"));
#endif
//		wxLogDebug(_T("%s:%s():line %d, m_bFreeTranslationMode = %s"), __FILE__, __FUNCTION__, __LINE__,
//			(&wxGetApp())->m_bFreeTranslationMode ? _T("TRUE") : _T("FALSE"));
	}
}


// whm Notes on DoPrepareDC():
// DoPrepareDC() is declared within the class WXDLLEXPORT wxScrollHelper in the scrolwin.h
// header file in the ...\include\wx folder. The signature is:
// virtual void DoPrepareDC(wxDC& dc);
// Since DoPrepareDC() is a virtual function, it would seem necessary to call its base class
// wxScrolledWindow::DoPrepareDC(dc); at the beginning of the CAdapt_ItCanvas::DoPrepareDC(wxDC& dc)
// override.
// The implementation of DoPrepareDC() in the scrlwing.cpp file in the ...\src\generic folder.
// See the implementation code below.
// According to the internal comment, the #ifdef directive should probably be
// removed and the wxLayout_RightToLeft flag taken into consideration for all ports,
// not just wxGTK.
// the wxWidgets docs say, "DoPrepareDC is called automatically within the default
// wxScrolledWindow::OnPaint event handler, so your wxScrolledWindow::OnDraw override
// will be passed a 'pre-scrolled' device context. However, if you wish to draw from
// outside of OnDraw (via OnPaint), or you wish to implement OnPaint yourself, you must
// call this function yourself."
//
// The MFC docs say of CView::OnPrepareDC(), "Called by the framework before the OnDraw
// member function is called for screen display and before the OnPrint member function is
// called for each page during printing or print preview....The default implementation
// of this function does nothing if the function is called for screen display. However,
// this function is overridden in derived classes, such as CScrollView, to adjust
// attributes of the device context; consequently, you should always call the base class
// implementation at the beginning of your override."

//
// The code for the wxWidgets' DoPrepareDC() (from scrlwing.cpp) is simply this:
/*
void wxScrollHelper::DoPrepareDC(wxDC& dc)
{
    wxPoint pt = dc.GetDeviceOrigin();
#ifdef __WXGTK__
    // It may actually be correct to always query
    // the m_sign from the DC here, but I leve the
    // #ifdef GTK for now.
    if (m_win->GetLayoutDirection() == wxLayout_RightToLeft)
        dc.SetDeviceOrigin( pt.x + m_xScrollPosition * m_xScrollPixelsPerLine,
                            pt.y - m_yScrollPosition * m_yScrollPixelsPerLine );
    else
#endif
        dc.SetDeviceOrigin( pt.x - m_xScrollPosition * m_xScrollPixelsPerLine,
                            pt.y - m_yScrollPosition * m_yScrollPixelsPerLine );
    dc.SetUserScale( m_scaleX, m_scaleY );
}

MFC's ScrollView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) method looks like this (from viewscrl.cpp):

void CScrollView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	ASSERT_VALID(pDC);

#ifdef _DEBUG
	if (m_nMapMode == MM_NONE)
	{
		TRACE(traceAppMsg, 0, "Error: must call SetScrollSizes() or SetScaleToFitSize()");
		TRACE(traceAppMsg, 0, "\tbefore painting scroll view.\n");
		ASSERT(FALSE);
		return;
	}
#endif //_DEBUG
	ASSERT(m_totalDev.cx >= 0 && m_totalDev.cy >= 0);
	switch (m_nMapMode)
	{
	case MM_SCALETOFIT:
		pDC->SetMapMode(MM_ANISOTROPIC);
		pDC->SetWindowExt(m_totalLog);  // window is in logical coordinates
		pDC->SetViewportExt(m_totalDev);
		if (m_totalDev.cx == 0 || m_totalDev.cy == 0)
			TRACE(traceAppMsg, 0, "Warning: CScrollView scaled to nothing.\n");
		break;

	default:
		ASSERT(m_nMapMode > 0);
		pDC->SetMapMode(m_nMapMode);
		break;
	}

	CPoint ptVpOrg(0, 0);       // assume no shift for printing
	if (!pDC->IsPrinting())
	{
		ASSERT(pDC->GetWindowOrg() == CPoint(0,0));

		// by default shift viewport origin in negative direction of scroll
		ptVpOrg = -GetDeviceScrollPosition();

		if (m_bCenter)
		{
			CRect rect;
			GetClientRect(&rect);

			// if client area is larger than total device size,
			// override scroll positions to place origin such that
			// output is centered in the window
			if (m_totalDev.cx < rect.Width())
				ptVpOrg.x = (rect.Width() - m_totalDev.cx) / 2;
			if (m_totalDev.cy < rect.Height())
				ptVpOrg.y = (rect.Height() - m_totalDev.cy) / 2;
		}
	}
	pDC->SetViewportOrg(ptVpOrg);

	CView::OnPrepareDC(pDC, pInfo);     // For default Printing behavior
}

In addition, the base class' CView::OnPrepareDC(pDC, pInfo) code (from viewcore.cpp) doesn't really do
anything much but looks like this:

void CView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	ASSERT_VALID(pDC);
	UNUSED(pDC); // unused in release builds

	// Default to one page printing if doc length not known
	if (pInfo != NULL)
		pInfo->m_bContinuePrinting =
			(pInfo->GetMaxPage() != 0xffff || (pInfo->m_nCurPage == 1));
}

*/
// Bill wrote this, but he says it never need be called, and has left it here because of
// its notes... so I guess I won't delete it! (yet)
void CAdapt_ItCanvas::DoPrepareDC(wxDC& dc)
{
    // See notes above comparing MFC's OnPrepareDC and wxWidgets' DoPrepareDC. Here are more notes on
    // MFC's usage of OnPrepareDC. The MFC's OnPrepareDC() is called in the following situations:

    //    OnPrepareDC() is called TWICE at the beginning of each page during printing and print
    //    previewing in the following calling sequence (from MFC's viewprnt.cpp library source file):
    //      OnPrepareDC(&dcPrint, &printInfo);
    //      dcPrint.StartPage()
    //      OnPrepareDC(&dcPrint, &printInfo); [called again because StartPage resets the device attributes]
    //    In contrast, in the wx printing framework, its version of OnPrepareDC called DoPrepareDC() is
    //    not called explicitly at all anywhere that I can find in its print routines.

	//    The View's OnDraw(CDC* pDC) in the following calling sequence:
	//		CClientDC viewDC((CWnd*)m_pBundle->m_pView);
	//		m_pBundle->m_pView->OnPrepareDC(&viewDC);
	//		m_pBundle->m_pView->GetClientRect(&grectViewClient); // set the global rect used for speeding drawing
	//		viewDC.DPtoLP(&grectViewClient); // get the point converted to logical coords
	//		// draw the layout
	//		m_pBundle->Draw(pDC);

	//    The View's CreateBox(...) in the following calling sequence:
	//		// convert to device coords
	//		CClientDC aDC(this);
	//		OnPrepareDC(&aDC); // adjust origin
	//		aDC.LPtoDP(&rectBox);

	//    The View's ResizeBox(...) in the following calling sequence:
	//		// convert to device coords
	//		CClientDC aDC(this);
	//		OnPrepareDC(&aDC); // adjust origin
	//		CPoint ptrLoc = *pLoc;
	//		aDC.LPtoDP(&ptrLoc);

	//    The View's RecalcLayout(...) in the following calling sequence:
	//		// get a device context, and get the origin adjusted (gRectViewClient is ignored when printing)
	//		CClientDC viewDC(this);
	//		OnPrepareDC(&viewDC);
	//		GetClientRect(&grectViewClient); // set the global rect used for speeding drawing
	//		viewDC.DPtoLP(&grectViewClient); // get the point converted to logical coords

	//    The View's OnLButtonDown(...) in the following calling sequence:
	//		// get the point into logical coordinates
	//		CClientDC aDC(this); // make a device context
	//		OnPrepareDC(&aDC); // get origin adjusted
	//		aDC.DPtoLP(&point); // get the point converted to logical coords

	//    The View's OnLButtonUp(...) in the following calling sequence:
	//		CClientDC aDC(this); // make a device context
	//		OnPrepareDC(&aDC); // get origin adjusted
	//		aDC.DPtoLP(&point); // get the point converted to logical coords

	//    The View's OnMouseMove(...) in the following calling sequence:
	//		CClientDC aDC(this); // make a device context
	//		OnPrepareDC(&aDC); // get origin adjusted
	//		aDC.DPtoLP(&point); // get the point converted to logical coords

	//    The View's SelectDragRange(...) in the following calling sequence:
	//		SelectAnchorOnly(); // reduce to just the anchor, before we rebuild the selection
	//		CClientDC aDC(this); // make a device context
	//		OnPrepareDC(&aDC); // get origin adjusted
	//		COLORREF oldBkColor = aDC.SetBkColor(RGB(255,255,0)); // yellow

	//    The View's SelectAnchorOnly(...) in the following calling sequence:
	//		CClientDC aDC(this); // make a device context
	//		OnPrepareDC(&aDC); // get origin adjusted

	//    The View's LayoutStrip(...) in the following calling sequence:
	//		// get a device context, and get the origin adjusted
	//		CClientDC viewDC(this);
	//		OnPrepareDC(&viewDC);

	//    The View's OnEnChangeEditBox(...) in the following calling sequence:
	//		CDC* pDC = this->GetDC();
	//		OnPrepareDC(pDC);
	//		DrawFreeTranslations(pDC, m_pBundle, call_from_edit);
	//		this->ReleaseDC(pDC);

	//    The View's ExtendSelectionRight(...) in the following calling sequence:
	//		OnPrepareDC(&aDC); // adjust the origin
	//		... before various pCell->DrawCell(&aDC); calls

	//    The View's ExtendSelectionLeft(...) in the following calling sequence:
	//		OnPrepareDC(&aDC); // adjust the origin
	//		... before various pCell->DrawCell(&aDC); calls

	//    The View's AdjustDialogPosition(...) in the following calling sequence:
	//		CClientDC dc(pView);
	//		pView->OnPrepareDC(&dc); // adjust origin
	//		// use location where phrase box would be put
	//		dc.LPtoDP(&ptBoxTopLeft); // now it's device coords
	//		pView->ClientToScreen(&ptBoxTopLeft); // now it's screen coords

	//    The View's GetVisibleStrips(...) in the following calling sequence:
	//		CClientDC dc(this);
	//		OnPrepareDC(&dc); // adjust origin
	//		// find the index of the first strip which is visible
	//		...
	//		CRect rectClient;
	//		GetClientRect(&rectClient); // view's client area (device coords)
	//		dc.DPtoLP(&rectClient);

	//	  The ConsistencyCheckDlg.cpp's OnInitDialog() in the following calling sequenct:
	//		CClientDC dc(m_pView);
	//		m_pView->OnPrepareDC(&dc); // adjust origin
	//		dc.LPtoDP(&m_ptBoxTopLeft); // now it's device coords
	//		m_pView->ClientToScreen(&m_ptBoxTopLeft); // now it's screen coords

	//	  The FindReplace.cpp's OnFindNext() in the following calling sequenct:
	//		CClientDC dc(m_pParent);
	//		m_pParent->OnPrepareDC(&dc); // adjust origin

	//	  The PhraseBox.cpp's OnChar() in the following calling sequenct:
	//		pView->RemoveSelection();
	//		CClientDC dC(pView);
	//		pView->OnPrepareDC(&dC); // adjust origin
	//		pView->Invalidate();

	// whm note: The wx version doesn't need to do anything in this override of DoPrepareDC. In fact
	// DoPrepareDC doesn't have to be overridden in the wx version. We'll leave this here for the sake
	// of the above notes and just call the base class method.
	wxScrolledWindow::DoPrepareDC(dc);
}


// wx Note: The wxScrollEvent only appears to intercept scroll events for scroll bars
// manually placed in wxWindow based windows. In order to handle scroll events for windows like
// wxScrolledWindow (that have built-in scrollbars like our canvas), we must use wxScrollWinEvent
// in the functions and the EVT_SCROLLWIN macro in the event table.
// We don't actually use this since our base wxScrolledWindow handles scroll events in response
// to clicking on the thumb, arrows, or the paging parts of the canvas' scrollbar.
void CAdapt_ItCanvas::OnScroll(wxScrollWinEvent& event)
{
    // whm added 13Jan2018 - removed the CloseDropDown() call below as it is not
    // needed to correct problems with ghosting of the new dropdown list.
    //CAdapt_ItApp* pApp = &wxGetApp();
    // [NOTE explains why this was needed when phrasebox was derived from wxOwnerDrawnComboBox]
    // Since a scroll of the screen with the list popped up separates the popped up list
    // part of the comboobox from the base edit box part (actually a phantom image), we 
    // here make the popup disappear (if it was open) by calling Dismiss() in wx2.9/3.x or
    // for wx2.8.12 the Frame's SendSizeEvent - when there is an OnScroll event (the 
    // SendSizeEvent causes the popup to close). 
    // When the popup is open it seeks the focus, and prevents the screen from scrolling the 
    // phrasebox-dropdown combination out of the client area - which effectively prevents 
    // the user from scrolling away to check context during adaptation. However, when the 
    // popup isn't open, the user can scroll the phrasebox-dropdown combination out of the 
    // client area.
    //pApp->m_pTargetBox->CloseDropDown();

    event.Skip();	// this is necessary for the built-in scrolling behavior of wxScrolledWindow
					// to be processed
}

bool CAdapt_ItCanvas::IsModified() const
{
	return FALSE;
}


void CAdapt_ItCanvas::DiscardEdits()
{
}

// whm 12Jul2018 Note: This PhraseBox Button handler only forwards event handling
// to the handler of the same name in CPhraseBox.
// whm 12Apr2019 moved code from CPhraseBox::OnTogglePhraseBoxButton() to this 
// CAdapt_ItCanvas::OnTogglePhraseBoxButton() handler to be handled here only.
void CAdapt_ItCanvas::OnTogglePhraseBoxButton(wxCommandEvent & event)
{
    //CAdapt_ItApp* pApp = &wxGetApp();
    CAdapt_ItApp* pApp = &wxGetApp();

    // for debugging the event
    int eventID = event.GetId(); eventID = eventID; //int ID_BMTOGGLEBUTTON_PHRASEBOX = 22040;
                                 
    // whm 13Jul2018 note about focus. In our new 3-part phrasebox framework
    // we get to control focus. When the new phrasebox button is clicked, it
    // grabs the focus, so we need to explicitly put the focus back to the
    // phrasebox's edit box whenever the dropdown button is toggled.
    // The focus is restored and proper selections done in the 
    // SetFocusAndSetSelectionAtLanding() call at the end of this function

    if (pApp->m_pTargetBox->GetDropDownList()->IsShown())
    {
        pApp->m_pTargetBox->GetDropDownList()->Hide();
    }
    else
    {
        if (pApp->m_pTargetBox->GetDropDownList()->GetCount() > 1)
        {
            pApp->m_pTargetBox->GetDropDownList()->Show();
            // whm 21Mar2020 added lines below to set the size of the dropdown list
            // especially the height, when in Reviewing mode would not get properly
            // set - often it would be way too long in height leaving a lot of empty
            // white space below the last list entry.
            int rectWidth = pApp->m_pTargetBox->GetTextCtrl()->GetRect().GetWidth();
			int rectWidthBefore = rectWidth;
            // whm 24Jul2018 added. Extend the list horizontally so it spans the whole
            // area under the edit box and the dropdown button.
            int buttonWidth = pApp->m_pTargetBox->GetPhraseBoxButton()->GetRect().GetWidth();
			wxUnusedVar(rectWidthBefore); // used only for the wxLogDebug call

			// whm 11Nov2022 note: When a document is first opened, and at the Layout's PlaceBox()
			// call the list width is now drawn all the way under the button width. This is 
			// intentional (see comment within the CPhraseBox::PopupDropDownList() function), and 
			// this under-button width of the dropdown list is maintained here in
			// CAdapt_ItCanvas::OnTogglePhraseBoxButton().
            rectWidth += (buttonWidth + 1); // incrememt rectWidth by width of button and 1-pixel space between.
            pApp->m_pTargetBox->SetSizeAndHeightOfDropDownList(rectWidth);
#if defined (_DEBUG) //&& defined (_OVERLAP)
			{ // scope the block for safety
				CSourcePhrase* pSPhr = pApp->m_pActivePile->GetSrcPhrase();
				if (pSPhr->m_bHasKBEntry)
				{
					wxLogDebug(_T("%s::%s() line %d: m_pTextBox recWidth BEFORE %d, AFTER adding btn+1: %d, buttonWidth %d, tgt = %s"),
						__FILE__, __FUNCTION__, __LINE__, rectWidthBefore, rectWidth, buttonWidth, pSPhr->m_adaption.c_str());
				}
			}
#endif
        }
        else
        {
            pApp->m_pTargetBox->GetDropDownList()->Hide();
        }
    }
    // When showing the list, we initially put focus into the phrasebox's 
    // edit box. Within the phrasebox we detect any navigation key presses
    // that are intended to move the list highlight and make actual selections
    // from the list - such as WXK_DOWN and WXK_UP. 
    // In any case, whether the list open or closed, our focus should be kept
    // in the phrasebox's edit box
    pApp->m_pTargetBox->SetFocusAndSetSelectionAtLanding(); // whm 13Aug2018 modified

    //pApp->m_pTargetBox->OnTogglePhraseBoxButton(event);
}

// whm 12Jul2018 Note: This PhraseBox Button handler only forwards event handling
// to the handler of the same name in CPhraseBox.
// whm 12Apr2019 moved code from CPhraseBox::OnListBoxItemSelected() to this 
// CAdapt_ItCanvas::OnListBoxItemSelected() handler to be handled here only.
void CAdapt_ItCanvas::OnListBoxItemSelected(wxCommandEvent & event)
{
    CAdapt_ItApp* pApp = &wxGetApp();
    // This is called when a list item is selected. It is also explicitly called
    // by the CPhraseBox::OnKeyUp() handler when the Enter key _WXT_RETURN is pressed 
    // in the edit box after a up/down arrow selection has been made in the drop down 
    // list. In the later case (up/down arrow + Enter), the event passed in is a
    // dummyevent, i.e., invalid for accessing the current event's methods.

    // whm 15Apr2019 Work-around for sudden scroll causing bad index value.
    // This work-around checks to see if a drop down list box index error has occurred,
    // and if so, sets the list box selection to what the user actually clicked on.
    // The App's m_nDropDownClickedItemIndex value is set in the App's FilterEvent()
    // function which detects the actual item the user clicked on before the class
    // hierarchy messes with the vertical scroll position causing the click to be
    // registered on a different item index. If that "true" index differs from what 
    // has been passed on to OnListBoxItemSelected() handler, we force the index and
    // list selection to be what the user actually clicked on.
    // The index error usually only happens when there are a number of items in the
    // drop down list, and the open list is partly below the client area of the main
    // window. In this particular situation, when the user clicks on an item in the
    // list, the screen content (including the list) suddenly scrolls up (enough to
    // be able to see the entire list, presumably) resulting in the mouse click being 
    // registered on an item farther down in the list than what the user intended.
    // After a thorough investigation, the sudden scroll of the screen happens deep
    // within the Windows-specific handling of its native list box - it is not the
    // result of anything I can track down within AI's own code, nor within the 
    // wxWidgets library routines for wxListBox.
    //
    // whm 30May2019 Correction. If the user moved the up/down arrow to highlight a
    // list item, then presses the Enter key to select it, the App's m_nDropDownClickedItemIndex
    // will still be -1 (no click was made), so to avoid an assert below, we must test for
    // that situation (arrow key and Enter to select an item), and not try to set the
    // list item selection to a -1 value.
    //
    // whm 25Sep2019 Further correction from a bug reported by Stefanie S in which she
    // would get a library debug error (assert "IsValid(n)" failed in wxListBox::GetStgring()) 
    // due to a bad listbox index - even in release mode. The problem was evident when
    // the user moved the list selection with up/down arrow key and presses Enter at a
    // new location (that had not previously been adapted). The _WXK_RETURN handling block 
    // in canvas' OnKeyUp() creates and passes in a dummyevent to this OnListBoxItemSelected() 
    // handler. That means that we cannot use the event object here reliably in that scenario 
    // to determine the listBoxSel, nor the selStr for the initiating event (which was a key
    // press rather than a mouse click on a list item.
    // The solution is to not use the passed in event argument to determine listBoxSel and 
    // setStr, but instead get those current values directly from the list box control.
    //int eventID = event.GetId(); eventID = eventID; //int ID_DROP_DOWN_LIST = 22050; // Note: only valid eventID for mouse click selection
    wxString selStr = pApp->m_pTargetBox->GetDropDownList()->GetStringSelection();
    int listBoxSel = pApp->m_pTargetBox->GetDropDownList()->GetSelection();
    //int listBoxSel = event.GetSelection();
    //wxString selStr = event.GetString(); selStr = selStr;
    if (pApp->m_nDropDownClickedItemIndex != -1 && listBoxSel != pApp->m_nDropDownClickedItemIndex)
    {
        wxLogDebug("***In CAdapt_ItCanvas::OnListBoxItemSelected() BEFORE correction selStr: %s at index %d", selStr.c_str(), listBoxSel);
        // The list box index error occurred, so set the selection back to what the user
        // just clicked on.
        event.SetId(pApp->m_nDropDownClickedItemIndex);
        pApp->m_pTargetBox->GetDropDownList()->SetSelection(pApp->m_nDropDownClickedItemIndex);
        // adjust the listBoxSel and selStr to their corrected values
        listBoxSel = pApp->m_pTargetBox->GetDropDownList()->GetSelection();
        selStr = pApp->m_pTargetBox->GetDropDownList()->GetString(listBoxSel);
        wxLogDebug("***In CAdapt_ItCanvas::OnListBoxItemSelected() AFTER correction selStr: %s at index %d",selStr.c_str(),listBoxSel);
    }
    pApp->m_nDropDownClickedItemIndex = -1; // set the global back to -1

    // whm 21Aug2018 added code to prevent a selection from being entered into the
    // phrasebox during free translation mode
	if (!pApp->m_bFreeTranslationMode)
	{

		wxString selItemStr;
		selItemStr = pApp->m_pTargetBox->GetListItemAdjustedforPhraseBox(TRUE); // whm 17Jul2018 added TRUE sets m_bEmptyAdaptationChosen = TRUE

#if defined(_DEBUG) //&& defined(_NEWDRAW)
		wxLogDebug(_T("CPhraseBox::OnListBoxItemSelected() line %d: at start: selItemStr: %s , for replacing box text: %s , at index: %d , m_bEmptyAdaptationChosen %d"),
			__LINE__, selItemStr.c_str(), pApp->m_targetPhrase.c_str(), pApp->m_pTargetBox->GetDropDownList()->GetSelection(), (int)pApp->m_pTargetBox->m_bEmptyAdaptationChosen);
#endif
		pApp->m_targetPhrase = selItemStr;
		pApp->m_pTargetBox->GetTextCtrl()->ChangeValue(selItemStr); //this->GetTextCtrl()->ChangeValue(selItemStr); // use of ChangeValue() or SetValue() resets the IsModified() to FALSE

		// whm 2Oct2023 addition. If the GetListItemAdjustedforPhraseBox(TRUE) sets the m_pTargetBox->m_bEmptyAdaptationChosen to TRUE,
		// we should not only empty the App's m_targetPhrase and the phrasebox itself, but also set the active location's source phrase members
		// m_adaption and m_targetStr to empty strings in response to the having m_bEmptyAdaptationChosen set to TRUE by the
		// GetListItemAdjustedforPhraseBox(TRUE) call above. 
		// This change is being made to help eliminate the previous phrasebox contents reappearing after the user selects <no adaptation> from
		// the drop down list, and hits Enter to move on or clicks the mouse to another location.
		if (pApp->m_pTargetBox->m_bEmptyAdaptationChosen)
		{
			CSourcePhrase* pSP = pApp->m_pActivePile->GetSrcPhrase();
			if (pSP != NULL)
			{
				pSP->m_adaption.Empty();
				pSP->m_targetStr.Empty();
			}
		}
	
		
		if (!pApp->m_pTargetBox->GetTextCtrl()->IsModified()) // need to call SetModified on m_pTargetBox before calling SetValue
        {
            pApp->m_pTargetBox->GetTextCtrl()->SetModified(TRUE); // Set as modified so that CPhraseBox::OnPhraseBoxChanged() will do its work // whm 12Jul2018 added GetTextCtrl()->
        }

        // whm 15Sep2018 added the Modify(TRUE) statement below, otherwise the document is not made 'dirty' by simply
        // selecting a dropdown list item. When a list box item is selected, it does not automatically trigger the 
        // OnPhraseBoxChanged() function. The code below activates the "Save" button in the toolbar for situations 
        // in which the document was not previously in a 'dirty' state, and the only edit performed after at that 
        // point is to select a different translation from the dropdown list before attempting to save.
        pApp->GetDocument()->Modify(TRUE);

        pApp->m_pTargetBox->m_bAbandonable = FALSE; // this is done in CChooseTranslation::OnOK()
                                      // whm 15Jul2018 added following flag settings to get selected string to stick
        pApp->m_bUserTypedSomething = TRUE;

        // whm 13Jul2018 added. The new phrasebox's list doesn't automatically closed upon
        // making a selection, so we do it here, and ensure focus is in the edit box.
        pApp->m_pTargetBox->CloseDropDown();

        // BEW 27Jul18 we need to force gap recalculations etc here, before focus is put
        // in the phrasebox
        pApp->GetDocument()->ResetPartnerPileWidth(pApp->m_pActivePile->GetSrcPhrase());
		
		// whm 11Nov2022 addition. Selecting a list item from dropdown list amounts to a change 
		// in the phrasebox. So, instead of calling RecalcLayout() here I think it is better to 
		// invoke the OnPhraseBoxChanged() function which does gap recalculations, etc, but also 
		// will call RecalcLayout(..., create_strips_keep_piles) for any box resize that might 
		// be needed. 
		wxCommandEvent dummyEvent;
		pApp->m_pTargetBox->OnPhraseBoxChanged(dummyEvent);

        pApp->m_pTargetBox->SetFocusAndSetSelectionAtLanding(); // whm 13Aug2018 modified
    }
    else
    {
        // free translation mode in effect, just beep and return
        ::wxBell();
        return;
    }

    //pApp->m_pTargetBox->OnListBoxItemSelected(event);
}

// BEW 22Jun10, no changes needed for support of kbVersion 2
// BEW 12Jan11, changed the setting of ptWedgeBotRight.y to be initialized to the
// top of the top cell; before this, it was set to the bottom of the strip, and
// then for a narrow word, a source text click would trick the code into thinking
// a wedge was clicked when in fact, it wasn't
void CAdapt_ItCanvas::OnLButtonDown(wxMouseEvent& event)
{
    wxLogDebug(_T("CAdapt_ItCanvas::OnLButtonDown() triggered"));

    CAdapt_ItApp* pApp = &wxGetApp();
	CLayout* pLayout = pApp->m_pLayout;
	wxASSERT(pApp != NULL);
	CAdapt_ItView* pView = (CAdapt_ItView*) pApp->GetView();
	wxASSERT(pView != NULL);
	wxLogDebug(_T("OnLButtonDown() at start: m_nCacheLeavingLocation = %d"),
		pApp->m_nCacheLeavingLocation);

	pApp->m_preGuesserStr.Empty(); // BEW 27Nov14, in case a src string, or modified string
				// is stored ready for user's Esc keypress to restore the pre-guesser
				// form, clear it, because the box is gunna move and we want it
				// restored to default empty ready for next box landing location
	// whm modified 29Mar12. Left mouse clicks now beep only when certain parts of
	// the canvas are clicked on, and allows other clicks to act normally (such as
	// the opening/closing of the ViewFilteredMaterial dialog and the Notes dialog).

	// Store a copy of the current value of the m_bCopySourcePunctuation flag,
	// so that if the latter is FALSE (due to user's gui choice to suppress the
	// copy in order to override with his own punctuation, or have none, the
	// flag can be restored to its FALSE value within DoStore_ForPlacePhraseBox()
	// BEW added this line at 20May16
	pApp->m_pTargetBox->m_bCurrentCopySrcPunctuationFlag = pApp->m_bCopySourcePunctuation;

#if defined (_DEBUG) && defined (_ABANDONABLE)

	pApp->LogDropdownState(_T("OnLButtonDown()"), _T("Adapt_ItCanvas.cpp"), 546);

#endif

	bool bClickWasProductive = FALSE;

	// GDLC 2010-02-16 Pointer to the CFreeTrans class because it is needed in OnLButtonDown().
	// Ideally, the parts of OnLButtonDown() that relate to free translations would be moved
	// to the CFreeTrans class and this local variable of OnLButtonDown would be removed.
	CFreeTrans* pFreeTrans = pApp->GetFreeTrans();

	wxPoint point;	// MFC passes CPoint point as second parameter to OnLButtonDown; wx gets it from the
					// wxMouseEvent via GetPosition. Both specify x an y coordinates relative to the upper
					// left corner of the window
					// Note: wxMouseEvent also has GetLogicalPosition() that automaticall translates to logical
					// position for the DC.
	point = event.GetPosition();	// GetPosition() gets the point on the view port relative to upper-left
									// corner of the window.
	gptLastClick = point;	// used by AdjustDialogByClick; needs to be device coords (not logical coords) there
							// so we'll calculate logical coords for point below

	// BEW added 6Mar06: check if the point clicked was outside of an open note dialog - we'll
	// assume this is intended as a click to open a different note dialog without losing the note
	// from the current note dialog which is about to become disposed of - we have to restore the
	// text in that one as filtered note text at that source phrase location before the new
	// one is opened. If the click was not intended that way, then too bad - the note dialog
	// will close as if the OK button was pressed, which is safe because no information is lost.
	// whm 29Mar12 Note: Clicks to close a Note dialog should be honored in read-only mode.
	wxRect dlgRect;
	if (pApp->m_pNoteDlg != NULL)
	{
		// whm 01Sep2022 revised comment. 
		// A note dialog is open, so check for a click outside it's rectangle, which would
		// likely indicate a click on a Note icon in the text - either a different note or
		// the same Note that is open. In either case we need to destroy the existing Note
		// dialog, saving any changes, that might have been made. What is needed here is the
		// same action that our CNoteDlg::OnOK() handler does, so we will call that OnOK() 
		// handler from here in OnLButtonDown(). Note that the OnOK() handler calls Destroy()
		// on the dialog, and also calls delete on the dialog's App pointer pApp->m_pNoteDlg
		// in order to prevent the new dialog (created below) from just presenting a "ghost"
		// dialog without any data in it. 
		// 
		dlgRect = pApp->m_pNoteDlg->GetRect(); // gets it as screen coords

		// if the point is not in this rect, then close it as if OK was pressed
		if (!dlgRect.Contains(point))
		{
			bClickWasProductive = TRUE;
			wxCommandEvent cevent(wxID_OK);
			pApp->m_pNoteDlg->OnOK(cevent);
			pApp->m_pNoteDlg = NULL;
			gpNotePile = NULL;
		}
	}
	
	// whm 01Sept2022 added a block that checks whether a ViewFilteredDlg is already open and if so,
	// we'll assme here (as with the NoteDlg above) that this is intended as a click to open a 
	// different green wedge to view filtered information - without losing any text edit 
	// within the view dialog's edit window. As with the NoteDlg (above), the ViewFilteredMaterial
	// dialog will close as if the OK button was pressed, which is safe because no information is lost.
	if (pApp->m_pViewFilteredMaterialDlg != NULL)
	{
		// whm 01Sep2022 revised comment
		// A view filtered material dialog is open, so check for a click outside it's rectangle, which would
		// likely indicate a click on a green wedge icon in the text - either a different green wedge
		// or the same ViewFilteredMaterial dialog that is open. In either case we need to destroy the 
		// existing ViewFilteredMaterial dialog, saving any changes, that might have been made. 
		// What is needed here is the same action that our CViewFilteredMaterialDlg::OnOK() handler does, 
		// so we will call that OnOK() handler from here in OnLButtonDown(). Note that the OnOK() handler 
		// calls Destroy() on the dialog, and also calls delete on the dialog's App pointer 
		// pApp->m_pViewFilteredMaterialDlg in order to prevent the new dialog (created below) from just 
		// presenting a "ghost" dialog without any data in it. 
		// 
		dlgRect = pApp->m_pViewFilteredMaterialDlg->GetRect(); // gets it as screen coords

		// if the point is not in this rect, then close it as if OK was pressed
		if (!dlgRect.Contains(point))
		{
			bClickWasProductive = TRUE;
			wxCommandEvent cevent(wxID_OK);
			pApp->m_pViewFilteredMaterialDlg->OnOK(cevent);
			pApp->m_pViewFilteredMaterialDlg = NULL;
		}
	}
	
	// get the point into logical coordinates
	wxClientDC aDC(this); // make a device context
	DoPrepareDC(aDC); // get origin adjusted (calls wxScrolledWindow::DoPrepareDC)

	// we don't need to call CalcUnscrolledPosition here because GetLogicalPosition already
	// provides logical coordinates for the clicked point; wxPoint in device coords was needed
	// above to set the gptLastClick (used in AdjustDialogByClick), so we'll get the logical
	// coords of the point here.
	wxPoint logicalPoint(event.GetLogicalPosition(aDC));
	point = logicalPoint;

	#ifdef _Trace_Click_FT
	/*
	TRACE2("OnLButtonDown after DPtoLP: point x = %d, y = %d\n", point.x,point.y);
	CSize siz = GetTotalSize();
	TRACE2("Scroll size: cx = %d, cy = %d\n",siz.cx,siz.cy);
	CPoint posn = GetScrollPosition();
	TRACE2("Scroll posn: x = %d, y = %d\n",posn.x,posn.y);
	*/
	#endif

    // a left button click must always halt auto-matching and inserting, so set the flag
    // false to cause this to happen; ditto for Replace All if it is in progress
	pApp->m_bAutoInsert = FALSE;
	gbReplaceAllIsCurrent = FALSE; // turn off Replace All

	CMainFrame *pFrame = pApp->GetMainFrame();
	pFrame = pFrame; // suppresses "local variable is initialized but not referenced" warning
	wxASSERT(pFrame != NULL);
	wxASSERT(pFrame->m_pComposeBar != NULL);
	wxTextCtrl* pEditCompose = (wxTextCtrl*)
								pFrame->m_pComposeBar->FindWindowById(IDC_EDIT_COMPOSE);
	wxString message;
	wxString chVerse;

    // BEW added 19 Apr 05 for support of clicking on the green wedge to open the dialog
    // for viewing filtered material. We allow a click anywhere in the rectangle defined by
    // the location and dimensions of the wedge to be treated as a valid click within the
    // wedge icon itself; updated 02Aug05 to handle RTL rendering in the Unicode version as
    // well (green wedge is at the pile's right for RTL layout)
	// Updated 07Sept05 to handle clicking in the peach coloured Note icon's rectangle to
	// open a note window.
	// whm 29Mar12 Note: Clicks on a green wedge should be honored in read-only mode.
	CStrip* pClickedStrip = pView->GetNearestStrip(&point);

	wxPoint ptWedgeTopLeft;
	wxPoint ptWedgeBotRight;
	wxPoint ptNoteTopLeft;
	wxPoint ptNoteBotRight;
	if (pClickedStrip != NULL)
	{
		ptWedgeTopLeft.x = pClickedStrip->GetStripRect_CellsOnly().GetLeft();
		ptWedgeTopLeft.y = pClickedStrip->GetStripRect_CellsOnly().GetTop();
		ptNoteTopLeft = ptWedgeTopLeft;
		ptWedgeBotRight.x = ptWedgeTopLeft.x;
		// BEW 12Jan11, changed the setting of ptWedgeBotRight.y to be initialized to the
		// top of the top cell; before this, it was set to the bottom of the strip, and
		// then for a narrow word, a source text click would trick the code into thinking
		// a wedge was clicked when in fact, it wasn't
		ptWedgeBotRight.y = ptWedgeTopLeft.y; // initialize bottom of wedge (its point)
											  // to the top of the top cell; we move it
											  // further up in the stuff below
		ptNoteBotRight = ptNoteTopLeft;
		int numPiles;

		// first check for and handle green wedge clicks, then check for and handle
		// note clicks
		ptWedgeBotRight.y -= 2; // this will set y coord for bottom of the wedge's rect
                                // - one pixel lower than the tip of the wedge, but still
                                // not in the cell below it
		ptWedgeTopLeft.y -= 7; // this gets the y coordinate set correctly for the top of
                               // the wedge's rect, the x-coord of topleft of cells will
                               // give us the x coords we need, and we can assume the right
                               // boundaries are 9 pixels to the right of those for
                               // notes...
		ptNoteTopLeft.y -= 9; // sets the y coord for top of the note icon's rectangle,
                              // ptNoteBotRight.y is already correct

		// get the number of piles in the clicked strip
		numPiles = pClickedStrip->GetPileCount();

        // do the check only if we have not suppressed showing source text, and only if
        // there actually are piles in the strip (yeah, I know, there has to be; but safety
        // first isn't a bad idea)
		if (numPiles > 0 && !gbShowTargetOnly)
		{
			CPile* pPile = NULL;
			int indexPile;
			for (indexPile = 0; indexPile < numPiles; indexPile++)
			{
				pPile = pClickedStrip->GetPileByIndex(indexPile);
				wxASSERT(pPile != NULL);

                // is there anything filtered here - if not, check for a note, if that
                // fails too, then just continue the loop
				if (HasFilteredInfo(pPile->GetSrcPhrase()))
				{
					// there is some filtered material stored on this
					// CSourcePhrase instance
					int xLeft;
					int xRight;
					wxRect pileRect = pPile->GetPileRect();
					xLeft = pileRect.GetLeft();

                    // if the Unicode application and we are laying out RTL, the wedge will
                    // be at the other end of the pile - so if this is the case, then set
                    // xLeft to be the x coord of the right boundary
					#ifdef _RTL_FLAGS
					if (gbRTL_Layout)
					{
						xLeft = pileRect.GetRight();
					}
					#endif
                    // BEW changed 16Jul05 to make the clickable rectangle a pixel wider all
                    // round so that less motor control is needed for the wedge click
					// wx note: The following is wxRect ok, since adjustments are made to the
					// upper left and lower right points to form a new wedgeRect
					xLeft -= 4;
					ptWedgeTopLeft.y -= 2; // BEW changed 18Nov05 to accomodate solid down
										   // arrow design, was -= 1
					xRight = xLeft + 11;
					ptWedgeBotRight.y += 1;
					ptWedgeTopLeft.x = xLeft ;
					ptWedgeBotRight.x = xRight;
					wxRect wedgeRect(ptWedgeTopLeft, ptWedgeBotRight);

					// we have the required rectangle where the click would need to occur,
					// now test to see if the click actually occurred within it
					if (wedgeRect.Contains(point))
					{
						bClickWasProductive = TRUE;
						// user clicked in this wedge - so open the dialog

                        // keep track of the sequ num of the src phrase whose m_markers is
                        // being viewed and potentially edited in the
                        // ViewFilteredMarkersDlg. Since the dialog is non-modal, we need a
                        // way to identify the source phrase whose m_markers member is to
                        // be updated after edit (edit update is done in
						pApp->m_nSequNumBeingViewed = pPile->GetSrcPhrase()->m_nSequNumber;

                        // BEW added 11Oct05, to allow clicked wedge's topmost cell in the
                        // layout to be given background highlighting, so user has visual
                        // feedback as to which pile the View Filtered Material dialog
                        // pertains to
						gpGreenWedgePile = pPile;

                        // We always completely destroy each instance of
                        // CViewFilteredMaterialDlg before creating another one
						if (pApp->m_pViewFilteredMaterialDlg != NULL)
						{
                            // whm 01Sep2022 modified and Note.
							// User has clicked on another green wedge with the current
                            // modeless dialog still open. Assume the user did not intend
                            // to save any changes (since he failed to click on OK to save
                            // them before clicking on another green wedge. In such cases
                            // we'll just destroy the window, so a new one can be created
                            // with data for the new location.
							// Note: This block of code is never entered because the
                            // block at the top of OnLButtonDown() detects the click
                            // outside the note dialog boundary and closes the note and
                            // saves its note text, so by the time control gets here,
                            // m_pViewFilteredMaterialDlg will be NULL. I'll
                            // leave this code here because it is defensive, and if the
                            // earlier block was ever removed then we'll still have the
                            // required insurance for inadventent loss of a note
							wxCommandEvent cevent(wxID_OK);
							pApp->m_pViewFilteredMaterialDlg->OnOK(cevent);
							pApp->m_pViewFilteredMaterialDlg = NULL;
						}
						if (pApp->m_pNoteDlg != NULL)
						{
							return; // the Note dialog is open, so prevent a green wedge
									// click from opening the View Filtered Material dialog
						}
						if (pApp->m_pViewFilteredMaterialDlg == NULL)
						{
							pApp->m_pViewFilteredMaterialDlg = new
												CViewFilteredMaterialDlg(pApp->GetMainFrame());
							// wx version: we don't need to call Create() for modeless dialog below:
							pView->AdjustDialogPositionByClick(pApp->m_pViewFilteredMaterialDlg,
														gptLastClick); // avoid click location
							pApp->m_pViewFilteredMaterialDlg->Show(TRUE);
						}
						// after user has finished with the dialog, we've nothing more to
						// do in OnLButtonDown() and so we return to the caller
						Refresh(); // get a refresh done
						return;
					}
					else
					{
						// no click in this wedge, so continue check for a note icon click
						goto u;
					}
				} // end block for a pile with filtered material
				else
				{
                    // control will enter this block always via the above goto u; statement
                    // because the note text is always filtered; but this syntax below is
                    // useful because we can test for note clicks even when there is no
                    // note info stored yet
u:					if (pPile->GetSrcPhrase()->m_bHasNote)
					{
						// this pile has a note, so check if the click was in the note
						// icon's rectangle
						int xLeft;
						int xRight;
						wxRect pileRect = pPile->GetPileRect();
						xLeft = pileRect.GetLeft();
						#ifdef _RTL_FLAGS
						if (gbRTL_Layout)
						{
							xLeft = pileRect.GetRight() + 6;
						}
						else
						{
							xLeft -= 13;
						}
						#else
							xLeft -= 13;
						#endif
						xRight = xLeft + 10; // at least 9,  but an extra one to make it
											 // easier to hit
						ptNoteTopLeft.x = xLeft ;
						ptNoteBotRight.x = xRight;
                        // wx note: The following is wxRect ok, since adjustments are made
                        // to the upper left and lower right points to form a new wedgeRect
						wxRect noteRect(ptNoteTopLeft, ptNoteBotRight);

						// check if the click was in noteRect
						if (noteRect.Contains(point)) //if (noteRect.PtInRect(point))
						{
							bClickWasProductive = TRUE;
							// user clicked in the note icon - so open the note window
							if (pApp->m_pViewFilteredMaterialDlg != NULL)
								return; // if the green wedge dialog is open, prevent a
										// Note icon click from opening the Note dialog

                            // BEW added 6Mar06 to cause return without any change if the
                            // user clicked on the note icon for a note already opened.
                            // Without this change the click has the effect of reopening
                            // the dialog empty and the note text has irretreivably been
                            // lost -- this block of code is never entered because the
                            // block at the very top of OnLButtonDown() detects the click
                            // outside the note dialog boundary and closes the note and
                            // saves its note text, so by the time control gets here,
                            // m_pNoteDlg will be NULL and gpNotePile will be NULL. I'll
                            // leave this code here because it is defensive, and if the
                            // earlier block was ever removed then we'll still have the
                            // required insurance for inadventent loss of a note
							if (pApp->m_pNoteDlg != NULL && pPile == gpNotePile)
							{
                                // if the note dialog is already open and the clicked
                                // pile's pointer is the same pointer as for the currently
                                // opened note dialog's pile, then return without doing
                                // anything other than a beep and restoring the focus to
                                // the note dialog
								::wxBell();
								wxTextCtrl* pEdit = (wxTextCtrl*)
									pApp->m_pNoteDlg->FindWindowById(IDC_EDIT_NOTE);
								pEdit->SetFocus();
								return;
							}

                            // keep track of the sequ num of the src phrase whose
                            // m_markers' filtered note information is being viewed and
                            // potentially edited in the note window. Since the dialog is
                            // non-modal, we need a way to identify the source phrase whose
                            // m_markers member is to be updated after edit (edit update is
                            // done in ...).
							pApp->m_nSequNumBeingViewed = pPile->GetSrcPhrase()->m_nSequNumber;

                            // BEW added 11Oct05, to allow clicked wedge's topmost cell in
                            // the layout to be given background highlighting, so user has
                            // visual feedback as to which pile the View Filtered Material
                            // dialog pertains to
							gpNotePile = pPile;

							// We always completely destroy each instance of CNoteDlg
							// before creating another one
							if (pApp->m_pNoteDlg != NULL)
							{
                                // user has clicked on a note icon with the current
                                // modeless note dialog still open. BEW changed 5Mar06: a
                                // click on a different note icon, with a note dialog open,
                                // should save the currently open dialog's note text as if
                                // he has clicked the OK button. Why? Because that does not
                                // destroy any info, and it is far more likely he'd want to
                                // keep the note, and is just wanting to move quickly to
                                // the next one. So, to get rid of the note, force him to
                                // do the action to explicitly remove it; but a click
                                // elsewhere should never be such an action. -- Tried
                                // implementing it here but failed. So I'll try do it by
                                // intercepting a click outside the note dialog itself and
                                // invoking the OnBnCLickedOk() function if that has
                                // happened.
                                // (Legacy behaviour: Assume the user did not intend to
                                // save any changes (since he failed to click on OK to save
                                // them before clicking).In such cases we'll just destroy
                                // the window, so a new one can be created with data for
                                // the new location.)
								pApp->m_pNoteDlg->Destroy();
								pApp->m_pNoteDlg = NULL;
							}
							if (pApp->m_pNoteDlg == NULL)
							{
								pApp->m_pNoteDlg = new CNoteDlg(this);
								// As with the ViewFilteredMaterialDlg, the modeless NoteDlg
								// doesn't need a Create() call.
								pView->AdjustDialogPositionByClick(pApp->m_pNoteDlg,
													gptLastClick); // avoid click location
								pApp->m_pNoteDlg->Show(TRUE);
							}
                            // after user has finished with the dialog, we've nothing more
                            // to do in OnLButtonDown() and so we return to the caller
							Refresh(); // get a refresh done
							return;
						}
					}
					else
					{
						continue;
					}
				}
			} // end of loop for checking each pile of the strip
		}
	}

    // a left click in the view when the "View Earlier Translation" modeless dialog is the
    // active window, must make the view window the active one
	// whm 29Mar12 Note: Clicks to make the view window be active should be honored in read-only mode.
	bool bMadeViewActive = FALSE;
	if (pApp->m_pEarlierTransDlg != NULL)
	{
		// Need to conditional compile HWND for different platforms here and below:
		if (!(pApp->m_pEarlierTransDlg->GetHandle() == NULL))
		{
            // WX Note: ::IsWindow() is not available in wxWidgets but it should be
            // sufficient here to just check if the Dlg window is being shown
			if (pApp->m_pEarlierTransDlg->IsShown())
			{
				pApp->m_pEarlierTransDlg->Show(FALSE); // hide the dialog window
				bMadeViewActive = TRUE;

				bClickWasProductive = TRUE;

				if (pApp->m_pTargetBox == NULL)
					goto y; // check, just in case, and do the longer cleanup if the box
							// is not there
				// restore focus to the targetBox
				if (pApp->m_pTargetBox != NULL)
				{
					if (pApp->m_pTargetBox->IsShown())
					{
                        // whm 3Aug2018 Note: The SetSelection here restores any existing selection
                        // so it should be allowed to stand as is.
                        // whm 13Aug2018 modified to call SetFocus() before the SetSelection() call below.
                        // Otherwise, if SetFocus() is called last, the Linux/Mac versions do a select all.
						pApp->m_pTargetBox->GetTextCtrl()->SetFocus();
						pApp->m_pTargetBox->GetTextCtrl()->SetSelection(pApp->m_nStartChar,pApp->m_nEndChar);
					}
				}
y:				; // I may put some code here later
			}
		}
	}

    // a left click in the view when a find or replace modeless dialog is the active
    // window, must hide the dialog window and cause the phrase box to be set up at
    // whatever location was the last active one - to do this, copy & adjust the code in
    // OnCancel for CFindReplace whm adjusted the following to accommodate different
    // modeless dialogs for "Find" (pApp->m_pFindDlg) and "Find and Replace"
    // (pApp->m_pReplaceDlg).
	// whm 29Mar12 Note: A Replace modeless dialog won't ever be open in read-only mode
	// but a Find dialog can be, so clicks in this instance should be honored in read-only
	// mode. Note: The phrase Box will always be hidden in read-only mode so the code to
	// set focus on the phrase box below will not have any effect.
	if (pApp->m_pFindDlg != NULL || pApp->m_pReplaceDlg != NULL || bMadeViewActive)
	{
		if (pApp->m_pFindDlg == NULL && pApp->m_pReplaceDlg == NULL && bMadeViewActive)
			return;
		if (pApp->m_pFindDlg != NULL || pApp->m_pReplaceDlg != NULL)
		{
			if ((pApp->m_pFindDlg != NULL && pApp->m_pFindDlg->IsShown()) ||
				(pApp->m_pReplaceDlg != NULL && pApp->m_pReplaceDlg->IsShown()))
			{
				bClickWasProductive = TRUE;
				if (pApp->m_pFindDlg != NULL && pApp->m_pFindDlg->IsShown())
					pApp->m_pFindDlg->Show(FALSE); // hide the dialog window
				if (pApp->m_pReplaceDlg != NULL && pApp->m_pReplaceDlg->IsShown())
					pApp->m_pReplaceDlg->Show(FALSE); // hide the dialog window
				gbFindIsCurrent = FALSE;
				gbFindOrReplaceCurrent = FALSE;

				// clear the globals
				pApp->m_bMatchedRetranslation = FALSE;
				gnRetransEndSequNum = -1;

				if (gbJustReplaced)
				{
                    // we have cancelled just after a replacement, so we expect the phrase
                    // box to exist and be visible, so we only have to do a little tidying
                    // up before we return
					if (pApp->m_pTargetBox == NULL)
						goto x; // check, just in case, and do the longer cleanup if the
                                // box is not there
					// restore focus to the targetBox
					if (pApp->m_pTargetBox != NULL)
					{
						if (pApp->m_pTargetBox->IsShown())
						{
                            // whm 3Aug2018 Note: The SetSelection here restores any existing selection
                            // so it should be allowed to stand as is.
                            // whm 13Aug2018 modified to call SetFocus() before the SetSelection() call below.
                            // Otherwise, if SetFocus() is called last, the Linux/Mac versions do a select all.
							pApp->m_pTargetBox->GetTextCtrl()->SetFocus();
                            pApp->m_pTargetBox->GetTextCtrl()->SetSelection(pApp->m_nStartChar,pApp->m_nEndChar);
						}
					}
					gbJustReplaced = FALSE; // clear to default value
				}
				else
				{
                    // we have tried a FindNext since the previous replacement, so we
                    // expect the phrase box to have been destroyed by the time we enter
                    // this code block; so place the phrase box, if it has been destroyed
                    // whm note 12Aug08. Since the MFC version expects the phrase box to be
                    // NULL here, but in the wx version it never is null, we will remove
                    // the == NULL test here.
x:					CCell* pCell = 0;
					CPile* pPile = 0;
					// This is a first entrance of the code pointer to this handler, so
					// at this point, the phrasebox will not yet have moved from where
					// it is to some other location, or even to the same location, 
					// depending on where the user clicked
					if (!pApp->m_selection.IsEmpty())
					{
						CCellList::Node* cpos = pApp->m_selection.GetFirst();
						pCell = (CCell*)cpos->GetData(); // could be on any line
						wxASSERT(pCell);
						pPile = pCell->GetPile();
					}
					else
					{
						// no selection, so find another way to define the "Leaving" location
						int nCurSequNum = pApp->m_nActiveSequNum;
						if (nCurSequNum == -1)
						{
							nCurSequNum = pApp->GetMaxIndex(); // make active loc the last
															// src phrase in the doc
							pApp->m_nActiveSequNum = nCurSequNum;
						}
						else if (nCurSequNum >= 0 && nCurSequNum <= pApp->GetMaxIndex())
						{
							pApp->m_nActiveSequNum = nCurSequNum;
						}
						else
						{
							// if all else fails, go to the start
							pApp->m_nActiveSequNum = 0;
						}
						pPile = pView->GetPile(pApp->m_nActiveSequNum);
						pCell = pPile->GetCell(1); // whether adapting or glossing mode

						// BEW 6Jul18, ensure we've got a valid cached sequ number for "Leaving" location
						if (pApp->m_nCacheLeavingLocation == wxNOT_FOUND)
						{
							pApp->m_nCacheLeavingLocation = pPile->GetSrcPhrase()->m_nSequNumber;
						}
					}

					CSourcePhrase* pSrcPhrase = NULL;
					pSrcPhrase = pPile->GetSrcPhrase();

					// pPile is what we will use for the active pile, so set everything
					// up there, provided it is not in a retranslation - if it is,
					// place the box preceding it, if possible, else after it; but if
					// we are glossing, then ignore the fact of the retranslation,
					// since we can have a phrasebox within a retranslation when
					// glossing
					CPile* pSavePile = pPile;
					while (!gbIsGlossing && pSrcPhrase->m_bRetranslation)
					{
						pPile = pView->GetPrevPile(pPile);
						if (pPile == NULL)
						{
							// if we get to the start, try again, going the other way
							pPile = pSavePile;
							while (pSrcPhrase->m_bRetranslation)
							{
								pPile = pView->GetNextPile(pPile);
								wxASSERT(pPile); // we'll assume this will never fail
								pSrcPhrase = pPile->GetSrcPhrase();
							}
							break;
						}
						pSrcPhrase = pPile->GetSrcPhrase();
					}
					pSrcPhrase = pPile->GetSrcPhrase();
					pApp->m_nActiveSequNum = pSrcPhrase->m_nSequNumber;
					pApp->m_pActivePile = pPile;
					pCell = pPile->GetCell(1); // we want the 2nd line, for phrase box
					// save old sequ number in case required for toolbar's Back button
                    pApp->m_nOldSequNum = pApp->m_nActiveSequNum;

					// BEW 28Jun18 cache this value for using within PlacePhraseBox() to enable 
					// recalculating the correct pOldActivePile pointer 
					pApp->m_nCacheLeavingLocation = pApp->m_nOldSequNum;
					wxLogDebug(_T(" OnLButtonDown() line %d, setting m_nCacheLeavingLocation, cached sequ num = %d"),__LINE__,
						pApp->m_nCacheLeavingLocation);

					pApp->m_bLandingBox = TRUE;

					// place the phrase box
					pView->PlacePhraseBox(pCell,2);

                    // get a new active pile pointer, the PlacePhraseBox call did a
                    // recalc of the layout
					pApp->m_pActivePile = pView->GetPile(pApp->m_nActiveSequNum);
					wxASSERT(pApp->m_pActivePile);

					// scroll into view, just in case (but shouldn't be needed)
					ScrollIntoView(pApp->m_nActiveSequNum);
					pView->Invalidate(); // get window redrawn
					pLayout->PlaceBox();

					// restore focus to the targetBox
					if (pApp->m_pTargetBox != NULL)
					{
						if (pApp->m_pTargetBox->IsShown())
						{
                            // whm 3Aug2018 Note: The SetSelection here restores any existing selection
                            // so it should be allowed to stand as is.
                            // whm 13Aug2018 modified to call SetFocus() before the SetSelection() call below.
                            // Otherwise, if SetFocus() is called last, the Linux/Mac versions do a select all.
                            pApp->m_pTargetBox->GetTextCtrl()->SetFocus();
                            pApp->m_pTargetBox->GetTextCtrl()->SetSelection(pApp->m_nStartChar,pApp->m_nEndChar);
						}
					}
					gbHaltedAtBoundary = FALSE;

					return; // otherwise, we would go on to process the click, which we don't
							// want to do
				} // end block for a click after a FindNext (which means phrase box will have
				  // been destroyed)
			} // end block for FindReplace visible
		} // end block for FindReplace dialog not null when user clicked
	} // end block for find or replace active, or view has just been reactivated

	// I want to make the background of a clicked cell be yellow
	if (pApp->m_pLayout == NULL) return;
	if (pApp->m_pLayout->GetStripCount() == 0) return;

	// whm 29Mar12 Note: Clicks to make the background of a clicked cell become yellow
	// should NOT be honored in read-only mode.
	if (!pApp->m_bReadOnlyAccess)
	{
		CCell* pAnchor = pApp->m_pAnchor; // anchor cell, for use when extending
										  // selection (null if no selection current)
		int    nAnchorSequNum = -1; // if no anchor defined, set value -1; we can test
									//for this value later
		if (pAnchor != NULL)
		{
			nAnchorSequNum = pAnchor->GetPile()->GetSrcPhrase()->m_nSequNumber; // there
				// is a pre-existing selection or at least the anchor click to make one
			wxASSERT(nAnchorSequNum >= 0);
		}

		CSourcePhrase* pCurSrcPhrase; // used in loop when extending selection
		int sequNum;

		// find which cell the click was in
		CCell* pCell = pView->GetClickedCell(&point); // returns NULL if click was
													  // not in a cell
		// BEW 22Dec14 support Seth Minkoff request (for Show Tgt Only allowing only selected cells to be undrawn)
		bool bSelectionWasActiveAtClick = pApp->m_selectionLine == 0; // a selection in src  line is active

		// BEW added 03Aug08: disallow a click in the gray text area (preceding or following
		// context) during vertical editing mode; I'll code this block as if I was supporting
		// adaptations or glosses or free translations as entry points too, but for MFC it will
		// only be source text editing as the entry point, but the code will work even with the
		// extra stuff in it
		if (gbVerticalEditInProgress && pCell != NULL)
		{
			int nClickedSequNum = pCell->GetPile()->GetSrcPhrase()->m_nSequNumber;
			if (gEditStep == adaptationsStep && gEditRecord.bAdaptationStepEntered)
			{
				// use the bounds for the editable span to test if the click was in the gray
				// text before the left bound, or in the gray text following the right bound;
				// if either is true, warn the user and disallow the click
				if (nClickedSequNum < gEditRecord.nAdaptationStep_StartingSequNum ||
					nClickedSequNum > gEditRecord.nAdaptationStep_EndingSequNum)
				{
					// IDS_CLICK_IN_GRAY_ILLEGAL
                    // whm 15May2020 added below to supress phrasebox run-on due to handling of ENTER in CPhraseBox::OnKeyUp()
                    pApp->m_bUserDlgOrMessageRequested = TRUE;
					// BEW 15Aug23 disallow the message if user was adding text to contentless USFM markup
					if (pCell->GetPile()->GetSrcPhrase()->m_key.IsEmpty())
					{
						pApp->m_pTargetBox->SetFocusAndSetSelectionAtLanding();
					}
					else
					{
						if (!pApp->GetDocument()->m_bWithinEmptyMkrsLoop)
						{
							wxMessageBox(_(
								"Attempting to put the active location within the gray text area while updating information in Vertical Edit mode is illegal. The attempt has been ignored."),
								_T(""), wxICON_EXCLAMATION | wxOK);
						}
						pApp->m_pTargetBox->SetFocusAndSetSelectionAtLanding();// whm 13Aug2018 modified
					}
                    return;
				}
				else
				{
					// Click was within the span, so the phrasebox location
					// needs to be stored on the app for later use in restoring
					// the original mode's state
					pApp->m_vertEdit_LastActiveSequNum = nClickedSequNum;
#if defined(_DEBUG)
					wxLogDebug(_T("VertEdit OnLButtonDown() storing click at sequ num: %d  adaptations step"), nClickedSequNum);
#endif
				}
			}
			else if (gEditStep == glossesStep && gEditRecord.bGlossStepEntered)
			{
				if (nClickedSequNum < gEditRecord.nGlossStep_StartingSequNum ||
					nClickedSequNum > gEditRecord.nGlossStep_EndingSequNum)
				{
					// IDS_CLICK_IN_GRAY_ILLEGAL
                    // whm 15May2020 added below to supress phrasebox run-on due to handling of ENTER in CPhraseBox::OnKeyUp()
                    pApp->m_bUserDlgOrMessageRequested = TRUE;
					// BEW 15Aug23 disallow the message if user was adding text to contentless USFM markup
					if (pCell->GetPile()->GetSrcPhrase()->m_gloss.IsEmpty())
					{
						pApp->m_pTargetBox->SetFocusAndSetSelectionAtLanding();
					}
					else
					{
						wxMessageBox(_(
							"Attempting to put the active location within the gray text area while updating information in Vertical Edit mode is illegal. The attempt has been ignored."),
							_T(""), wxICON_EXCLAMATION | wxOK);
						pApp->m_pTargetBox->SetFocusAndSetSelectionAtLanding();// whm 13Aug2018 modified
					}
                    return;
				}
				else
				{
					// Click was within the span, so the phrasebox location
					// needs to be stored on the app for later use in restoring
					// the original mode's state
					pApp->m_vertEdit_LastActiveSequNum = nClickedSequNum;
#if defined(_DEBUG)
					wxLogDebug(_T("VertEdit OnLButtonDown() storing click at sequ num: %d  glosses step"), nClickedSequNum);
#endif
				}
			}
			else if (gEditStep == freeTranslationsStep && gEditRecord.bFreeTranslationStepEntered)
			{
				if (nClickedSequNum < gEditRecord.nFreeTranslationStep_StartingSequNum ||
					nClickedSequNum > gEditRecord.nFreeTranslationStep_EndingSequNum)
				{
					// IDS_CLICK_IN_GRAY_ILLEGAL
                    // whm 15May2020 added below to supress phrasebox run-on due to handling of ENTER in CPhraseBox::OnKeyUp()
                    pApp->m_bUserDlgOrMessageRequested = TRUE;
					if (pCell->GetPile()->GetSrcPhrase()->m_gloss.IsEmpty())
					{
						pApp->m_pTargetBox->SetFocusAndSetSelectionAtLanding();
					}
					else
					{
						wxMessageBox(_(
							"Attempting to put the active location within the gray text area while updating information in Vertical Edit mode is illegal. The attempt has been ignored."),
							_T(""), wxICON_EXCLAMATION | wxOK);
						pApp->m_pTargetBox->SetFocusAndSetSelectionAtLanding();// whm 13Aug2018 modified
					}
                    return;
				}
				else
				{
					// Click was within the span, so the phrasebox location
					// needs to be stored on the app for later use in restoring
					// the original mode's state - but in this case, phrasebox
					// will get moved to the anchor, so set it to that location
					pApp->m_vertEdit_LastActiveSequNum = gEditRecord.nFreeTranslationStep_StartingSequNum;
#if defined(_DEBUG)
					wxLogDebug(_T("VertEdit OnLButtonDown() storing click at anchor sequ num: %d  freeTrans step"),
						gEditRecord.nFreeTranslationStep_StartingSequNum);
#endif
				}
			}
		}

		// we may be going to drag-select, so prepare for drag
		gbHaltedAtBoundary = FALSE;
		if (pCell != NULL && pCell->GetCellIndex() == 0)
		{
			// if we have a selection and shift key is being held down, we assume user is not
			// dragging, but rather is extending an existing selection, so check for this and
			// exit block if so
			if (event.ShiftDown() && pApp->m_selection.GetCount() > 0)
				goto t;
			// remove any old selection & update window immediately
			pView->RemoveSelection();
			Refresh();

			// prepare for drag
			pApp->m_mouse = point;
			CaptureMouse(); //on Win32, SetCapture() is called via CaptureMouse()
							// and DoCaptureMouse() in wxWidget sources wincmn.cpp
							// and window.cpp
#if defined(_DEBUG) && defined(CAPTUREMOUSE)
			wxLogDebug(_T("CaptureMouse."));
#endif
		}

	t:	if (pCell == NULL)
		{
			// click was not in a cell, so just remove any existing selection
			pView->RemoveSelection();

			// Click was not in a cell, so remove the highlighting of automatically
			// inserted target (or glosses) text
			// BEW changed 9Apr12
			pApp->m_pLayout->ClearAutoInsertionsHighlighting();

			pApp->m_bSelectByArrowKey = FALSE;
			Refresh(); // must force a redraw, or else the selection
					   // stays on the screen (UpdateWindow() doesn't work here)

			// can't initiate a drag selection unless we click on a cell, so clear drag support
			// variables too
			pApp->m_mouse.x = pApp->m_mouse.y = -1;
		}
		else // pCell is not NULL, that is, a cell was clicked on
		{
			if (event.ShiftDown())
			{
				// shift key is down, so extend the selection if there is an existing one on
				// the matching line;
					pApp->m_bSelectByArrowKey = FALSE;
				if (pApp->m_selectionLine == -1)
				{
					// no current selection, so treat the SHIFT+click as an ordinary
					// unshifted click
					goto a;
				}
				else
				{
					// there is a current selection, so extend it but only provided the click
					// was on the same line as the current selection is on
					wxASSERT(pApp->m_selection.GetCount() != 0);

					// local variables to use in the loops below
					CPile*	pEndPile;
					CPile*	pCurPile; // the one we use in the loop, starting from pOldSel's pile
					CStrip* pCurStrip; // the strip the starting pile is in
					CCell*	pCurCell; // the current cell in the current pile (used in loop)
					int nCurPileCount; // how many piles in current strip
					int nCurPile; // index of current pile (in loop)
					int nCurStrip; // index of current strip in which is the current pile

					// set all the above local variables from pCell and pAnchor (anchor is the
					// cell first clicked, pCell is the one at the end of the extend or drag)
					pEndPile = pCell->GetPile();
					pCurPile = pAnchor->GetPile();
					pCurStrip = pCurPile->GetStrip();
					nCurPileCount = pCurStrip->GetPileCount();
					nCurPile = pCurPile->GetPileIndex(); // value of m_nPile
					nCurStrip = pCurStrip->GetStripIndex(); // value of m_nStrip

					if (pApp->m_selectionLine != pCell->GetCellIndex())
						goto b; // delete old selection then do new one, because lines not the same
					else
					{
						// its on the same line, so we can extend it
						sequNum = pCell->GetPile()->GetSrcPhrase()->m_nSequNumber;
						if (sequNum >= nAnchorSequNum)
						{
							// we are extending forwards (to the "right" in logical order, but
							// in the view it is rightwards for LTR layout, but leftwards for
							// RTL layout)
							if (pApp->m_selectionLine == 0)
							{
								// source text line, but respect boundaries, unless
								// m_bRespectBoundaries is FALSE; first determine if the
								// anchor cell is at a boundary, if it is then we cannot
								// extend the selection in which case just do nothing
								// except warn the user
								if (pApp->m_bRespectBoundaries)
								{
									if (pView->IsBoundaryCell(pAnchor) && pCell != pAnchor)
									{
										// warn user
										// IDS_CANNOT_EXTEND_FWD
                                        // whm 15May2020 added below to supress phrasebox run-on due to handling of ENTER in CPhraseBox::OnKeyUp()
                                        pApp->m_bUserDlgOrMessageRequested = TRUE;
                                        wxMessageBox(_(
	"Sorry, but the application will not allow you to extend a selection forwards across any punctuation unless you use a technique for ignoring a boundary as well."),
										_T(""), wxICON_INFORMATION | wxOK);
										event.Skip();
										return;
									}
								}

								// first remove any selection preceding the anchor, since
								// click was forward of the anchor
								pView->RemovePrecedingAnchor(&aDC, pAnchor);

								// extend the selection
								int sequ = nAnchorSequNum;
								while (sequ < sequNum)
								{
									sequ++; // next one

									// get the next pile
									if (nCurPile < nCurPileCount-1)
									{
										// there is at least one more pile in this strip,
										// so access it
										nCurPile++; // index of next pile
										pCurPile = pCurStrip->GetPileByIndex(nCurPile);
										pCurSrcPhrase = pCurPile->GetSrcPhrase();
										pCurSrcPhrase = pCurSrcPhrase; // avoid warning TODO: need test below?
										wxASSERT(pCurSrcPhrase->m_nSequNumber == sequ); // must match
										pCurCell = pCurPile->GetCell(pApp->m_selectionLine); // get the cell

										// if it is already selected, continue to next one,
										// else select it
										if (!pCurCell->IsSelected())
										{
											aDC.SetBackgroundMode(pApp->m_backgroundMode);
											aDC.SetTextBackground(wxColour(255,255,0)); // yellow
											pCurCell->DrawCell(&aDC, pLayout->GetSrcColor());
											pCurCell->SetSelected(TRUE);

											// keep a record of it
											pApp->m_selection.Append(pCurCell);
										}

										// if we have reached a boundary, then break out,
										// otherwise continue
										if (pView->IsBoundaryCell(pCurCell)&& pApp->m_bRespectBoundaries)
										{
											break;
										}
									} // end of block for test "cell isn't the strip's last"
									else
									{
										// we have reached the end of the strip, so go to
										// start of next
										nCurPile = 0; // first in next strip
										nCurStrip++; // the next strip's index
										// get the pointer to next strip
										pCurStrip = pLayout->GetStripByIndex(nCurStrip);
										// get the pointer to its first pile
										pCurPile = pCurStrip->GetPileByIndex(nCurPile);
										nCurPileCount = pCurStrip->GetPileCount(); // update
												// so test above remains correct for the strip
										pCurSrcPhrase = pCurPile->GetSrcPhrase();
										wxASSERT(pCurSrcPhrase->m_nSequNumber == sequ);
										// get the required cell if it is already selected,
										// & continue to next one, else select it
										pCurCell = pCurPile->GetCell(pApp->m_selectionLine);
										if (!pCurCell->IsSelected())
										{
											aDC.SetBackgroundMode(pApp->m_backgroundMode);
											aDC.SetTextBackground(wxColour(255,255,0)); // yellow
											pCurCell->DrawCell(&aDC, pLayout->GetSrcColor());
											pCurCell->SetSelected(TRUE);

											// keep a record of it
											pApp->m_selection.Append(pCurCell);
										}

										// if we have reached a boundary, then break out,
										// otherwise continue
										if (pView->IsBoundaryCell(pCurCell)&& pApp->m_bRespectBoundaries)
										{
											break;
										}
									}
								}
								// user may have shortened an existing selection, so check
								// for any selected cells beyond the last one clicked, and
								// if they exist then remove them from the list and
								// deselect them.
								CCell* pEndCell = pEndPile->GetCell(pApp->m_selectionLine);
								pView->RemoveLaterSelForShortening(&aDC, pEndCell);
							} // end of block for test that selectionLine is 0
							else
							{
								// second or third line, a shift click here does nothing as yet.
								;
							}
						} // end of block for extending to higher sequence numbers
						  // (ie. visibly right for LTR layout, but visibly left for RTL layout)

						// else extend to lower sequence numbers...
						else
						{
							// we are extending backwards (ie. to the "left" for logical order,
							// but in the view it is left for LTR layout, but right for RTL
							// layout); ie. moving to lower sequ numbers
							if (pApp->m_selectionLine == 0)
							{
								// block for source text selection extending the selection
								// backwards; take boundaries into account, provided
								// m_RespectBoundaries is TRUE; first determine if the anchor
								// cell follows a boundary, if it is then we cannot extend the
								// selection backwards, in which case just do nothing except
								// warn the user
								CCell* pPrevCell;
								pPrevCell = pView->GetPrevCell(pAnchor, pApp->m_selectionLine);
								if (pApp->m_bRespectBoundaries)
								{
									if (pView->IsBoundaryCell(pPrevCell))
									{
										// warn user
										// IDS_CANNOT_EXTEND_BACK
                                        // whm 15May2020 added below to supress phrasebox run-on due to handling of ENTER in CPhraseBox::OnKeyUp()
                                        pApp->m_bUserDlgOrMessageRequested = TRUE;
                                        wxMessageBox(_(
	"Sorry, it is not possible to extend the selection backwards at this location unless you use one of the methods for ignoring a boundary."),
										_T(""), wxICON_INFORMATION | wxOK);
										event.Skip();
										return;
									}
								}

								// first if there are any cells selected beyond
								// the anchor cell, then get rid of them
								pView->RemoveFollowingAnchor(&aDC, pAnchor);

								int sequ = nAnchorSequNum; // starting point
								while (sequ > sequNum)
								{
									sequ--; // next one to the left

									// get the previous pile
									if (nCurPile > 0)
									{
										// there is at least one previous pile in this strip,
										// so access it
										nCurPile--; // index of previous pile
										pCurPile = pCurStrip->GetPileByIndex(nCurPile);
										pCurSrcPhrase = pCurPile->GetSrcPhrase();
										wxASSERT(pCurSrcPhrase->m_nSequNumber == sequ); // must match
										pCurCell = pCurPile->GetCell(pApp->m_selectionLine); // get the cell

										// if it is a boundary then we must break out
										// of the loop
										if (pApp->m_bRespectBoundaries && pApp->m_bRespectBoundaries)
										{
											if (pView->IsBoundaryCell(pCurCell))
												break;
										}

										// if it is already selected, continue to next prev one,
										// else select it
										if (!pCurCell->IsSelected())
										{
											aDC.SetBackgroundMode(pApp->m_backgroundMode);
											aDC.SetTextBackground(wxColour(255,255,0)); // yellow
											pCurCell->DrawCell(&aDC, pLayout->GetSrcColor());
											pCurCell->SetSelected(TRUE);

											// keep a record of it, retaining order of words/phrases
											pApp->m_selection.Insert(pCurCell);
										}
									}
									else
									{
										// we have reached the start of the strip, so go to end
										// of previous strip
										nCurStrip--; // the previous strip's index
										pCurStrip = pLayout->GetStripByIndex(nCurStrip); // prev strip
										nCurPileCount = pCurStrip->GetPileCount(); // update this so
														//test above remains correct for the strip
										nCurPile = nCurPileCount-1; // last in this strip
										pCurPile = pCurStrip->GetPileByIndex(nCurPile);  // pointer
																				 // to its last pile
										pCurSrcPhrase = pCurPile->GetSrcPhrase();
										wxASSERT(pCurSrcPhrase->m_nSequNumber == sequ);
										// get the required cell
										pCurCell = pCurPile->GetCell(pApp->m_selectionLine);

										// if it is a boundary then we must break out of the loop
										if (pApp->m_bRespectBoundaries && pApp->m_bRespectBoundaries)
										{
											if (pView->IsBoundaryCell(pCurCell))
												break;
										}

										// if it is already selected, continue to next prev one,
										// else select it
										if (!pCurCell->IsSelected())
										{
											aDC.SetBackgroundMode(pApp->m_backgroundMode);
											aDC.SetTextBackground(wxColour(255,255,0)); // yellow
											pCurCell->DrawCell(&aDC, pLayout->GetSrcColor());
											pCurCell->SetSelected(TRUE);

											// keep a record of it, preserving order of words/phrases
											pApp->m_selection.Insert(pCurCell);
										}
									}
								} // end sequ > sequNum  test block

								// user may have shortened an existing selection, so check for
								// any selected cells previous to the last one clicked, and if
								// they exist then remove them from the list and deselect them.
								CCell* pEndCell = pEndPile->GetCell(pApp->m_selectionLine);
								pView->RemoveEarlierSelForShortening(&aDC,pEndCell);
							}  // end of block for test selectionLine == 0
							else
							{
								// one of the target language lines, -- behaviour yet to be
								// determined probably just ignore the click
								;
							}
						} // end of block for extending to lower sequence numbers
						  // (ie. visibly left for LTR layout but visibly right for RTL layout)
					} // end block for a "same line" click which means extension of selection can
					  // be done
				} // end block for extending a selection
	#ifdef __WXMAC__
				pApp->GetMainFrame()->SendSizeEvent(); // this is needed for wxMAC to paint the highlighted source correctly
	#endif
			} // end of block for a click with SHIFT key down - for extending selection
			else
			{
				// SHIFT key is not down

				// found the cell, and the shift key is not down, so remove the old selection
				// (or shift key was down, but clicked cell was not on same line of a strip)
				if (pCell->GetCellIndex() == 1)
				{
					// second line - the phrase box's line (always): a click here places the
					// phraseBox in that cell clicked, unless the cell is part of a
					// retranslation
					CPile* pClickedPile = pCell->GetPile();
					CSourcePhrase* pSP = pClickedPile->GetSrcPhrase();
					wxASSERT(pClickedPile);

					if (!gbIsGlossing && pSP->m_bRetranslation)
					{
						// BEW 19Jul21 refactoring normal layout and no-visible-src layout
						// so that a click on any target text word in a retranslation no
						// longer shows a message, but instead, just opens the retranslation
						// for editing, and returns when the user is done, whether cancelling
						// the edit or having done editing, to have the active pile be the
						// nearest previous location not in the retranslation span. (If there
						// no safe previous location, it will be placed at safest location after
						// the span, and if THAT fails, the in the retranslation itself - this
						// is the legacy protocol for GetSafePhraseBoxLocatioanUsingList(), a
						// function defined in AI.cpp. Since the user may do editing, and the
						// old active location may change, the return musts do actions like
						// view's Invalidate() followed by ScrollIntoView() followed by PlaceBox()
						// and setting the focus and targetBox and its selection - there are plenty
						// places in our code where these actions can be copied to here.
						
						pApp->m_bUserClickedTgtWordInRetranslation = TRUE;
						pApp->GetRetranslation()->EditRetranslationByTgtClick(pSP);
						// Now update to get the GUI consistent with what was done						
//#if defined (_DEBUG)
//						{
//							CPile* pmyPile = pApp->GetView()->GetPile(2322);
//							wxString mytgt = pmyPile->GetSrcPhrase()->m_adaption;
//							wxLogDebug(_T("%s::%s() line %d, pile for walala, tgt = %s"), __FILE__, __FUNCTION__, __LINE__, mytgt.c_str());
//						}
//#endif
						pApp->m_bUserClickedTgtWordInRetranslation = FALSE; // restore default
						return;
						/* BEW 20Jul21 deprecated, now a tgt word clicked opens the retrans editor
						// make any single pile within a retranslation (other than a click in
						// line 0 which causes a selection) inaccessible - user should
						// treat a retranslation as a whole, & access it via toolbar buttons
						if (!pApp->m_bFreeTranslationMode) // BEW added 8Jul05 to allow making
												// a retranslation pile the anchor location for
												// a free translation by a click
						{
							// IDS_NO_ACCESS_TO_RETRANS
							//::wxBell(); // a ding here might help too
							
                            // whm 15May2020 added below to supress phrasebox run-on due to handling of ENTER in CPhraseBox::OnKeyUp()
                            pApp->m_bUserDlgOrMessageRequested = TRUE;

                            wxMessageBox(_(
	"Sorry, to edit or remove a retranslation you must use the toolbar buttons for those operations."),_T(""),
							wxICON_INFORMATION | wxOK);
							// put the focus back in the former place
                            if (pApp->m_pTargetBox != NULL)
                            {
                                if (pApp->m_pTargetBox->IsShown())
                                {
                                    pApp->m_pTargetBox->SetFocusAndSetSelectionAtLanding();// whm 13Aug2018 modified
                                }
                            }
							return;
						}
						*/
					}

					// BEW 27May14, the following actions, in free translation mode,
					// produced a crash:
					// 1) finished entering text in a free translation early in document
					// (actually at a word in 1:1 of James, but the actual location is irrelevant)
					// 2) did nothing to make the active free translation be stored, but
					// just navigated to much later on (most of the doc is unadapted)  to
					// a little bit of adapted stuff in a \s field immediately prior to ch
					// 2:1, although exactly where is also irrelevant, the bug happens if
					// the click is at any place other than immediately after the old
					// current section of free translation,
					// 3) at the clicked location, only the word clicked because a new
					// section, and attempts to Lengthen did nothing (the pile array was
					// still at the old location, as was m_pFollowingAnchorPile), so I
					// clicked the Adjust button to try join to the two CSourcePhrase
					// instances ahead to make the section bigger, and got the crash.
					// Analysis revealed that at no point was the old current section
					// properly closed off and its free translation saved, and the
					// relevant arrays and pointers for the old section cleared. So I'm
					// going to try a StoreFreeTranslationOnLeaving() call here now, which
					// should do all the proper closings off. It calls
					// StoreFreeTranslation(), and also clears the pointer array for the
					// old section's piles.
					if (!gbIsGlossing && pApp->m_bFreeTranslationMode)
					{
						pFreeTrans->CloseOffCurFreeTransSection();
					}

                    // We should clear target text highlighting if user clicks in a cell
                    // within a stretch of text that is not already highlighted. The
                    // highlighting should be retained if user clicks in a cell within a
                    // stretch of highlighted text since the user is probably correcting
                    // one or more cells that were not good translations

					// BEW changed 9Apr12, to support discontinuous auto-insertions
					int sequNumAtClick = pClickedPile->GetSrcPhrase()->m_nSequNumber;
					if (!pApp->m_pLayout->IsLocationWithinAutoInsertionsHighlightedSpan(sequNumAtClick))
					{
						pApp->m_pLayout->ClearAutoInsertionsHighlighting();
					}

					// save old sequ number in case required for toolbar's Back button
                    pApp->m_nOldSequNum = pApp->m_nActiveSequNum;

                    // BEW 8Aug13, For support of "The HACK" (see end of OnIdle()) --
                    // trying to debug the failure of an edited form to "stick" when the
                    // phrase box moves on. RossJones, Bill, JerryPfaff, and me (once) have
                    // observed it, and also nation translators on Linux netbooks
					//pApp->limiter = 0; // bug fixed 24Sept13 BEW

					// set up a temporary pile pointer
					CPile* pile = NULL;

					// BEW added block 26Jun05 for free translation support
					if (pApp->m_bFreeTranslationMode)
					{
						// get the phrase box to the start of the free translation section if
						// in one, otherwise where clicked becomes the start of a free
						// translation section
						pile = pCell->GetPile();
						CSourcePhrase* pSP = pile->GetSrcPhrase();
						wxASSERT(pSP != NULL);
						while (pSP->m_bHasFreeTrans && !pSP->m_bStartFreeTrans)
						{
							// there must be an earlier one which starts the free translation
							// section, so iterate back until it is found; in the refactored
							// design (April 2009) iterating back will not find a bundle start
							// (there are no bundles anymore), the only possibility is to find
							// the free translation section's start - there has to be one
							// before the start of the doc is found, though it could be the
							// first pile of the doc; but we'll test for a malformed doc too
							// and make a fix as best we can
							CPile* pOldPile = pile; // keep the one we are leaving in case the
											// next line gives NULL and so the fix is needed
							pile = pView->GetPrevPile(pile);
							if (pile == NULL)
							{
								// went past start of doc! we should find the section start
								// before coming to the doc start, (check Split Document
								// command - can it split within a free translation section
								// without alerting user or helping, and so generate a
								// doc-initial partial free trans section with no beginning?
								// Assume it can for now...) So, fix the srcPhrase & leave
								// phrase box here & return
								gbSuppressSetup = TRUE; // don't permit reentry at
														// RecalcLayout() call
								pApp->m_nActiveSequNum = 0;
								CSourcePhrase* pOldSrcPhrase = pOldPile->GetSrcPhrase();
								pOldSrcPhrase->m_bStartFreeTrans = TRUE; // it didn't have it
																		 // set, so do it
	#ifdef _NEW_LAYOUT
								pLayout->RecalcLayout(pApp->m_pSourcePhrases, keep_strips_keep_piles);
	#else
								pLayout->RecalcLayout(pApp->m_pSourcePhrases, create_strips_keep_piles);
	#endif
								pApp->m_pActivePile = pView->GetPile(0);
								wxASSERT(pApp->m_pActivePile != NULL);

								// now we've located the phrase box, permit setup again
								gbSuppressSetup = FALSE; // allow reentry again
								return;
							}
							pSP = pile->GetSrcPhrase(); // get the sourcephrase on the previous pile
						} // end of while loop for test: pSP->m_bHasFreeTrans && !pSP->m_bStartFreeTrans

						pCell = pile->GetCell(1); // the correct pCell which is the free
						// trans anchor for this F.Tr section; or if no free translation
						// is at the place clicked, this one will become the anchor

						// if about to place the phrase box elsewhere due to a click, and free
						// translation mode is turned on, we don't want to retain the Compose
						// Bar's edit box contents, since it will be different at the new
						// location - it is sufficient to clear that edit box's contents and
						// then the SetupCurFreeTransSection() call will, if appropriate, put
						// the free translation text in the box, or none, or compose a default
						// text, depending on what options are currently on and whether or not
						// the place where the box was clicked has free translation text
						// already - if the latter is true, then the phrase box will be
						// automatically moved if necessary so that it is placed at the start
						// of the clicked free translation section
						wxString tempStr;
						tempStr.Empty();
						pEditCompose->ChangeValue(tempStr); // clear the box

						pApp->m_nActiveSequNum = pile->GetSrcPhrase()->m_nSequNumber;
						pApp->m_pActivePile = pView->GetPile(pApp->m_nActiveSequNum);

						// make m_bIsCurrentFreeTransSection FALSE on every pile
						pApp->m_pLayout->MakeAllPilesNonCurrent();

						// BEW added 8Apr10, the wxString m_Translation has to be given
						// the value for the phrase box at the new location, otherwise the
						// last location's string will wrongly be put there; we get the value
						// from the m_adaption member of that CSourcePhrase instance in
						// adapting mode, but from m_gloss when in glossing mode
						// BEW 7Aug13, added the test for gbIsGlossing, hitherto, we
						// always got m_adaption but that's wrong if free translating is
						// being done while in glossing mode (which it is legal to do)
						if (gbIsGlossing)
						{
                            pApp->m_pTargetBox->m_Translation = pCell->GetPile()->GetSrcPhrase()->m_gloss;
						}
						else
						{
                            pApp->m_pTargetBox->m_Translation = pCell->GetPile()->GetSrcPhrase()->m_adaption;
						}

						// the PlacePhraseBox() call calls CLayout::RecalcLayout()
						pView->PlacePhraseBox(pCell,1); // suppress both KB-related code blocks
						pApp->m_pActivePile = pView->GetPile(pApp->m_nActiveSequNum);

						// Is this location an anchor for a free translation section - if it
						// is, we must make the free translation GUI's radio boxes agree with
						// the anchor's value for m_bSectionByVerse flag; if it's not an
						// anchor, then we are making it one, and we must set
						// m_bSectionByVerse to whatever value is correct according to the
						// app's value currently for the flag m_bDefineFreeTransByPunctuation
						// flag
						if (pApp->m_pActivePile->GetSrcPhrase()->m_bStartFreeTrans)
						{
							// we are in an existing free translation section

							// BEW 27Feb12, since we've come to the anchor of a pre-defined
							// section, we must set up the radio buttons to be what was in
							// effect when this section was originally created
							bool bTemporaryByPunctuationFlag =
								!pApp->m_pActivePile->GetSrcPhrase()->m_bSectionByVerse;

							// now set the radio buttons temporarily to possibly different values
							pApp->GetFreeTrans()->SetupFreeTransRadioButtons(bTemporaryByPunctuationFlag);
						}
						else
						{
							// it's not a free translation section yet, so set up to have the radio
							// buttons be in sync with the m_bDefineFreeTransByPunctuation flag value;
							// and we can't assume that the location just left was one which was
							// already in sync with the current radio button settings, so we must do
							// them both here too
							// now set the radio buttons temporarily to possibly different values
							pApp->GetFreeTrans()->SetupFreeTransRadioButtons(pApp->m_bDefineFreeTransByPunctuation);

							// and the anchor must have m_bSectionByVerse set correctly as well
							pApp->m_pActivePile->GetSrcPhrase()->m_bSectionByVerse =
														!pApp->m_bDefineFreeTransByPunctuation;
						}
						ScrollIntoView(pApp->m_nActiveSequNum);
                        pApp->m_pTargetBox->m_Translation.Empty();

					} // end of block for test: pApp->m_bFreeTranslationMode == TRUE
					else
					{
						// not in free translation mode
                        pApp->m_pTargetBox->m_Translation.Empty();

						//#ifdef _Trace_Click_FT
						//TRACE1("PlacePhraseBox() next, normal mode; key: %s\n", pApp->m_targetPhrase);
						//#endif

						// if the user has turned on the sending of synchronized scrolling
						// messages, send the relevant message
						if (!pApp->m_bIgnoreScriptureReference_Send)
						{
							pView->SendScriptureReferenceFocusMessage(pApp->m_pSourcePhrases,pCell->GetPile()->GetSrcPhrase());
						}

						// BEW changed 1Jul09, the use of the CPhraseBox::m_bAbandonable flag
						// got lost in the port to wxWidgets, so it needs to be restored. We
						// don't retain the box contents if m_bAbandonable is TRUE when the
						// user clicks at some other location, nor do we store something in
						// the KB based on whatever source text may have been copied to the
						// location that is now being moved from. This lets the user click
						// around the document in locations without any adaptation, examine
						// what is there, and click to move the box elsewhere, without any
						// copies of source text being stored to the KB (unless of course he
						// clicks in the box or edits, etc). The PlacePhraseBox() call, for
						// either situation, calls RecalcLayout()
						//
						// BEW 2Mar15 added a new test here, for when the 'no punctuation button'
						// has been pressed (that means m_bCopySourcePunctuation will be temporarily
						// FALSE), and the CSourcePhrase instance here stores only [ or ] as
						// punctuation text (and no src or tgt 'word' text), and control is
						// leaving this location (m_bAbandonable is TRUE if the user caused
						// this by clicking at a new location). In these circumstances we
						// want the [ or ] to not be retained as the m_targetStr value, and
						// m_adaption should be empty. Effect this in the block immediately
						// below, but generically enough to apply to more than just [ and ]
						// when they are punctuation and there is no word-building chars.
						// The old location's sequ number is preserved in m_nOldSequNum
						// BEW 20May16 added 3rd subtest to next line, otherwise it bleeds out
						// user's choice of manual typing of punctuation in the block further
						// down
						wxLogDebug(_T("OnLButtonDown() line %d; after x: label,  m_nCacheLeavingLocation = %d"),__LINE__,
							pApp->m_nCacheLeavingLocation);

						if (pApp->m_nOldSequNum != -1 && !pApp->m_bCopySourcePunctuation &&
							(pView->GetPile(pApp->m_nOldSequNum)->GetSrcPhrase()->m_precPunct == _T("[") ||
							pView->GetPile(pApp->m_nOldSequNum)->GetSrcPhrase()->m_follPunct == _T("]")) )
						{
							CSourcePhrase* sp = pView->GetPile(pApp->m_nOldSequNum)->GetSrcPhrase();
							// In this circumstance, m_targetStr should be set to m_adaption,
							// and no punctuation copies done
                            pApp->m_pTargetBox->m_Translation = sp->m_adaption;
							sp->m_targetStr = sp->m_adaption;

							pView->PlacePhraseBox(pCell, 2); // selector = 2, meaning no store
							// is done at the leaving location, but a removal from the KB
							// will be done at the landing location
						}
						else if (pApp->m_pTargetBox->m_bAbandonable)
						{
                            pApp->m_pTargetBox->m_Translation.Empty();
							pApp->m_targetPhrase.Empty();
							pApp->m_pTargetBox->GetTextCtrl()->ChangeValue(_T(""));

#if defined (_DEBUG) && defined (_ABANDONABLE)
							pApp->LogDropdownState(_T("OnLButtonDown() m_bAbandonable TRUE block, before calling PlacePhraseBox() with selector == 2, no store leaving but KB item removal on landing"), _T("Adapt_ItCanvas.cpp"), 1882);
#endif

							pView->PlacePhraseBox(pCell, 2); // selector = 2, meaning no store
								// is done at the leaving location, but a removal from the KB
								// will be done at the landing location
#if defined (_DEBUG) && defined (_ABANDONABLE)
							pApp->LogDropdownState(_T("OnLButtonDown() end of m_bAbandonable TRUE block, after calling PlacePhraseBox() with selector == 2, no store leaving but KB item removal on landing"), _T("Adapt_ItCanvas.cpp"), 1888);
#endif
						}
						else
						{
							// BEW changed 14Mar13. If selector = default 0, then
							// attempting to access the KB when the previous location did
							// not exist (i.e. sn = -1 for that location) will give a
							// crash. Therefore test for this and do a different
							// PlacePhraseBox() call if that is the case
							if (pApp->m_nOldSequNum == -1)
							{
								pView->PlacePhraseBox(pCell, 2); // selector = 2 (meaning
									// KB access is not done at the leaving location, but
									// is done at the landing location
							}
							else
							{
#if defined (_DEBUG) && defined (_ABANDONABLE)
								pApp->LogDropdownState(_T("OnLButtonDown() before calling PlacePhraseBox() in normal situation, selector == 0"), _T("Adapt_ItCanvas.cpp"), 1896);
#endif
								wxASSERT(pApp->m_nOldSequNum != -1);

								// save old sequ number in case required for toolbar's Back button
								//pApp->m_nOldSequNum = pApp->m_nActiveSequNum;

								// BEW 28Jun18 also cache this value for using within PlacePhraseBox to define 
								// pOldActivePile pointer
								pApp->m_nCacheLeavingLocation = pApp->m_nOldSequNum;
								wxLogDebug(_T(" OnLButtonDown() line %d, setting m_nCacheLeavingLocation, cached sequ num = %d"),__LINE__,
									pApp->m_nCacheLeavingLocation);

								pApp->m_bLandingBox = TRUE;

								// Now get the phrasebox placed
								pView->PlacePhraseBox(pCell); // selector = default 0 (meaning
									// KB access is done at both leaving and landing locations)
#if defined (_DEBUG) && defined (_ABANDONABLE)
								pApp->LogDropdownState(_T("OnLButtonDown() after the usual selector = 0 PlacePhraseBox() call has returned"), _T("Adapt_ItCanvas.cpp"), 1915);
#endif
							}
						}
						// BEW added 30Jun18 - to support AuSIL request for cursor at end
                        // whm 3Aug2018 modified for latest protocol of only selecting all when
                        // user has set App's m_bSelectCopiedSource var to TRUE by ticking the
                        // View menu's 'Select Copied Source' toggle menu item. 
                        if (pApp->m_pTargetBox != NULL)
                        {

                            pApp->m_pTargetBox->SetFocusAndSetSelectionAtLanding();// whm 13Aug2018 modified
                        }

                        ScrollIntoView(pApp->m_nActiveSequNum);
					}

					// restore default button image, and m_bCopySourcePunctuation to TRUE
					wxCommandEvent event;
					if (!pApp->m_bCopySourcePunctuation)
					{
						pApp->GetView()->OnToggleEnablePunctuationCopy(event);
					}

					CPile* pPile;
					pPile = pView->GetPile(pApp->m_nActiveSequNum);
					pPile = pPile; // avoid warning in release build
					wxASSERT(pApp->m_nActiveSequNum == pPile->GetSrcPhrase()->m_nSequNumber);

					// refresh status info at the bottom of the main window
					pApp->RefreshStatusBarInfo();

					// BEW addition 22Dec14, if Show Target Text Only is in effect with a selection
					// in operation (as requested by Seth Minkoff), a click to a new location
					// should cause normal view (ie. src text shown everywhere) be restored
					if (bSelectionWasActiveAtClick // there was a selection in existence at user click
						&& gbShowTargetOnly)       // the app is in Show Target Text Only mode
					{
						// Moved somewhere, so toggle back to normal mode
						wxCommandEvent dummyevent;
						pApp->GetView()->OnToggleShowSourceText(dummyevent); // normal view, showing source & target lines	
					}

					// if we are in free translation mode, there is a bit more to do...
					if (pApp->m_bFreeTranslationMode)
					{
						// put the focus in the compose bar's edit box, select any text there,
						// but only when it has been composed from the target text or gloss
						// text; if it is preexisting then put the cursor at the end of it
						pEditCompose->SetFocus();
						if (pApp->m_bTargetIsDefaultFreeTrans || pApp->m_bGlossIsDefaultFreeTrans)
						{
							if (pApp->m_pActivePile->GetSrcPhrase()->m_bHasFreeTrans)
							{
								// whm modified 24Aug06
								wxString tempStr;
								tempStr = pEditCompose->GetValue();
								int len = tempStr.Length();
								pEditCompose->SetSelection(len,len);
							}
							else
							{
                                // whm 3Aug2018 Note: This select all has to do with the compose bar
                                // behavior. TODO: Determine if it should be suppressed if 'Select Copied Source'
                                // toggle menu item is NOT ticked???
								pEditCompose->SetSelection(-1,-1);// -1,-1 selects all
							}
						}

						// mark the current section
						pFreeTrans->MarkFreeTranslationPilesForColoring(pFreeTrans->m_pCurFreeTransSectionPileArray);
#ifdef _DEBUG
//			wxString amsg = _T("Line 1983, OnLButtonDown(), in Adapt_ItCanvas.cpp");
//			pFreeTrans->DebugPileArray(amsg, pFreeTrans->m_pCurFreeTransSectionPileArray);
#endif
						if (pApp->m_nActiveSequNum >= 0 && pApp->m_nActiveSequNum <= pApp->GetMaxIndex())
						{
							ScrollIntoView(pApp->m_nActiveSequNum);
						}
					} // end of block for test: pApp->m_bFreeTranslationMode
                    // whm 10Apr2018 experimenting by adding event.Skip() here before
                    // the return statement. It makes no difference; a click within the
                    // phrasebox to remove the highlight doesn't pass through here.
                    //event.Skip();
					return;

				} // end block for test: pCell->GetCellIndex() == 1
				if (pCell->GetCellIndex() == 2)
				{
					return; // prevent clicks in bottom line of piles selecting or doing anything
				}

				// if it's none of the above things, then just a plain old click for making a
				// selection... so clear the old selection, then make a new one
				if (pCell->GetCellIndex() == 0)
				{
	b:				if (pApp->m_selection.GetCount() != 0)
					{
						CCellList::Node* pos = pApp->m_selection.GetFirst();
						CCell* pOldSel;
						while (pos != NULL)
						{
							pOldSel = (CCell*)pos->GetData();
							pos = pos->GetNext();
							aDC.SetBackgroundMode(pApp->m_backgroundMode);
							aDC.SetTextBackground(wxColour(255,255,255)); // white
							pOldSel->DrawCell(&aDC, pLayout->GetSrcColor());
							pOldSel->SetSelected(FALSE);
						}
						pApp->m_selection.Clear();
						pApp->m_selectionLine = -1; // no selection
						pApp->m_pAnchor = NULL;
					}

					// then do the new selection
	a:				pApp->m_bSelectByArrowKey = FALSE;
					aDC.SetBackgroundMode(pApp->m_backgroundMode);
					aDC.SetTextBackground(wxColour(255,255,0)); // yellow
					pCell->DrawCell(&aDC, pLayout->GetSrcColor());
					pCell->SetSelected(TRUE);

					// preserve record of the selection
					pApp->m_selection.Append(pCell);
					pApp->m_selectionLine = pCell->GetCellIndex();
					pApp->m_pAnchor = pCell;
				} //end of block for test: pCell->GetCellIndex() == 0

			} // end of else block for test: event.ShiftDown() == TRUE
		} // end of else block for test: pCell == NULL, i.e. pCell not null
	} // end of if (!pApp->m_bReadOnlyAccess)
	else
	{
		// Read-only access is active, so if the click was not productive beep
		if (!bClickWasProductive)
		{
			// The click was not productive so beep
			::wxBell();
		}
	}
	event.Skip();
}

void CAdapt_ItCanvas::OnLButtonUp(wxMouseEvent& event)
// do the selection of the current pile, if not already selected; release mouse, and set
// direction
{
//    wxLogDebug(_T("CAdapt_ItCanvas::OnLButtonUp() triggered"));

	gbReplaceAllIsCurrent = FALSE; // need this, otherwise after a Find and Replace (and even
    // though no replaces are done), if user cancels the find and replace dlg, then clicks
    // to remove the selection (this somehow sets gbReplaceAllIsCurrent to TRUE) then on
    // the next Find dialog, instead of doing the FindNext only as asked, the OnIdle()
    // handler for Replace All button is invoked and we get a spurious set of unwanted
    // replace alls. Probably the click to cancel the selection overwrites something but
    // there is nothing wrong with the code that I can spot, so I put an explicit gbRepl...
    // = FALSE here in order to force the flag back FALSE after the prior OnLButtonDown()
    // call removes the selection.

	CAdapt_ItApp* pApp = &wxGetApp();
	CLayout* pLayout = pApp->m_pLayout;
	wxASSERT(pApp != NULL);
	CAdapt_ItView* pView = (CAdapt_ItView*) pApp->GetView();
	wxASSERT(pView != NULL);
	wxClientDC aDC(this); // make a device context
	DoPrepareDC(aDC); // get origin adjusted - this only has significance if gbIsPrinting - needed?

	//wxPoint point = event.GetPosition();
    // we don't need to call CalcUnscrolledPosition() here because GetLogicalPosition
    // already provides the logical position of the clicked point
	wxPoint point(event.GetLogicalPosition(aDC));

	// can do a selection only if we have a non zero anchor pointer
	if (pApp->m_pAnchor != NULL)
	{
		// find which cell the cursor was over when the mouse was released (not a well-named
		// function but it does what we want)
		CPile* pCurPile = NULL;
		CCell* pCell = pView->GetClickedCell(&point); // returns NULL if point was not in a cell

		// BEW added 03Oct08 for support of vertical editing, to prevent dragging
		// a selection into the gray text area either side of the editable span
		if (pCell != NULL)
		{
			pCurPile = pCell->GetPile();
			bool bIsOK = pView->CheckForVerticalEditBoundsError(pCurPile);
			if (!bIsOK)
			{
				// if FALSE was returned, RemoveSelection() has already been called
				return;
			}
		}

		if (pCell == NULL || pApp->m_selectionLine != pCell->GetCellIndex())
		{
            // oops, we missed a cell, or are in wrong line, so we have to clobber any
            // existing selection
			pView->RemoveSelection();
			Refresh();
			goto a;
		}

		if (pView->IsTypeDifferent(pApp->m_pAnchor,pCell))
		{
			if (HasCapture())
			{
				ReleaseMouse();
			}
			// IDS_DIFF_TEXT_TYPE
            // whm 15May2020 added below to supress phrasebox run-on due to handling of ENTER in CPhraseBox::OnKeyUp()
            pApp->m_bUserDlgOrMessageRequested = TRUE;
            wxMessageBox(_(
"Sorry, you are trying to select text of different types, such as a heading and verse text, or some other illegal combination. Combining verse text with poetry is acceptable, other combinations are not."),
			_T(""), wxICON_EXCLAMATION | wxOK);
			pView->RemoveSelection();
			Refresh(); //Invalidate();  but no phrase box redraw here
			goto a;
		}

        // set the direction (this is always "logical direction", ie, to higher sequence
        // numbers is "rightwards" and to lower sequence number is "leftwards"; for an LTR
        // layout, logical order and visible mouse movement coincide, but for RTL layout,
        // the mouse moves in the opposite direction to logical direction)
		if (pApp->m_pAnchor->GetPile()->GetSrcPhrase()->m_nSequNumber <=
											pCell->GetPile()->GetSrcPhrase()->m_nSequNumber)
		{
				pApp->m_curDirection = toright; // BEW 2Oct13 changed from right to toright due to ambiguity
		}
		else
		{
				pApp->m_curDirection = toleft; // BEW 3Oct13 changed to toleft from left due to ambiguity
		}

        // finish drag select, but only if not halted at a boundary (note: if selecting
        // forwards, the boundary sourcePhrase actually can be selected (and should be),
        // but unless the user is some kind of speed king with the mouse, the bounding
        // sourcePhrase will have been selected already by the OnMouseMove's internal
        // SelectDragRange() call; so we can use the gbHaltedAtBoundary value in
        // OnLButtonUp() to suppress the final selection without losing the selection of
        // the final element; for backwards selections, the bounding sourcePhrase must not
        // be selected, and so we need to suppress the code below in that case too.
		if (!gbHaltedAtBoundary)
		{
            // first make sure we don't get here when trying to extend a selection by
            // holding down the SHIFT key, because this is not a drag situation, and
            // control can still get to this point. A sufficient test is to check for the
            // SHIFT key down, if it is, we are done and must exit this block immediately
			if (event.ShiftDown() && pApp->m_selection.GetCount() > 0)
				goto a;

			CCellList::Node* pos = pApp->m_selection.Find(pCell);
			if (pos == NULL)
			{
				// cell is not yet in the selection, so add it
				aDC.SetBackgroundMode(pApp->m_backgroundMode);
				aDC.SetTextBackground(wxColour(255,255,0)); // yellow
				pApp->m_bSelectByArrowKey = FALSE;
				pCell->DrawCell(&aDC, pLayout->GetSrcColor());
				pCell->SetSelected(TRUE);

				// preserve record of the selection
				if (pApp->m_curDirection == toright) // BEW 2Oct13 changed from right to toright due to ambiguity
				{
					pApp->m_selection.Append(pCell);
				}
				else
				{
					// whm Note: wxList::Insert(pCell) inserts the pCell at the
					// front of the list by default
					pApp->m_selection.Insert(pCell);
				}
			}
		}
	}

	// clear the drag variables and release the mouse
a:	pApp->m_mouse.x = pApp->m_mouse.y = -1;
    // In wx, it is an error to call ReleaseMouse() if the canvas did not previously call
    // CaptureMouse() so we'll check first to make sure canvas has captured the mouse
	if (HasCapture()) // whm added if (HasCapture()) because wx asserts if ReleaseMouse
	{				  // is called without capture
#ifdef __WXMAC__
		pApp->GetMainFrame()->SendSizeEvent(); // this is needed for wxMAC to paint the highlighted source correctly
#endif
		ReleaseMouse(); // assume no failure
	}
	gbHaltedAtBoundary = FALSE; // ensure it is cleared

	event.Skip(); //CScrollView::OnLButtonUp(nFlags, point);
}

// The Adapt It canvas is a child window of CMainFrame, along with other windows belonging
// to CMainFrame - notably the menu bar, tool bar, status bar, mode bar and compose bar.
// The menu bar, tool bar and status bar windows are associated with the wxFrame by the
// SetMenuBar(), SetToolBar(), and SetStatusBar() methods.
// Adapt It's mode bar and compose bar are unique to Adapt It and are managed separately
// from the other standard wxFrame's "bar" windows. These Adapt It specific bars are
// managed in CMainFrame, and its OnSize() handler, rather than here in the canvas class.


void CAdapt_ItCanvas::OnMouseMove(wxMouseEvent& event)
{
	CAdapt_ItApp* pApp = &wxGetApp();
	wxASSERT(pApp != NULL);
	CAdapt_ItView* pView = pApp->GetView();
	if (pApp->m_pAnchor == NULL)
		return; // no anchor, so we can't possibly have a selection

	wxClientDC aDC(this); // make a device context
	DoPrepareDC(aDC); // get origin adjusted

    // whm note: The wx docs seem to indicate that, once DoPrepareDC is called,
    // event.GetPosition() should return a point that is already converted to logical
    // coords, but testing shows that's not the case. GetPosition() always returns
    // device/screen coordinates; only GetLogicalPosition() returns the point converted to
    // logical coords
	wxPoint point(event.GetLogicalPosition(aDC));

	if (event.Dragging()) // whm note: Dragging() works here; LeftDown() doesn't
	{
		// do the following only provided the button is down
		if (pApp->m_mouse.x != point.x || pApp->m_mouse.y != point.y)
		{
			// there has been movement, so check if more selection is
			// required & act accordingly
			CCell* pCell = pView->GetClickedCell(&point); // returns NULL
											// if the point was not in a cell
			if (pCell == NULL)
			{
				pView->SelectAnchorOnly(); // is independent of direction
			}
			else
			{
				pView->SelectDragRange(pApp->m_pAnchor,pCell); // internally sets
											// m_curDirection to left or right
			}
		}
	}
}

/*
// A code snippit from the wxWidgets doodling program.
// It illustrates how to capture and process stuff while
// dragging the mouse with the left button.
void CAdapt_ItCanvas::OnMouseEvent(wxMouseEvent& event)
{
  if (!view)
    return;

  static DoodleSegment *currentSegment = (DoodleSegment *) NULL;

  wxClientDC dc(this);
  PrepareDC(dc);

  dc.SetPen(*wxBLACK_PEN);

  wxPoint pt(event.GetLogicalPosition(dc));

  if (currentSegment && event.LeftUp())
  {
    if (currentSegment->lines.Number() == 0)
    {
		if (currentSegment != NULL) // whm 11Jun12 added NULL test
	      delete currentSegment;
      currentSegment = (DoodleSegment *) NULL;
    }
    else
    {
      // We've got a valid segment on mouse left up, so store it.
	  CAdapt_ItDoc* pDoc = (CAdapt_ItDoc *)view->GetDocument();
	  wxASSERT(pDoc != NULL);

      pDoc->GetCommandProcessor()->Submit(new DrawingCommand(_T("Add Segment"),
											DOODLE_ADD, pDoc, currentSegment));

      view->GetDocument()->Modify(TRUE);
      currentSegment = (DoodleSegment *) NULL;
    }
  }

  if (xpos > -1 && ypos > -1 && event.Dragging())
  {
    if (!currentSegment)
      currentSegment = new DoodleSegment;

    DoodleLine *newLine = new DoodleLine;
    newLine->x1 = (long)xpos;
    newLine->y1 = (long)ypos;
    newLine->x2 = pt.x;
    newLine->y2 = pt.y;
    currentSegment->lines.Append(newLine);

    dc.DrawLine( (long)xpos, (long)ypos, pt.x, pt.y);
  }
  xpos = pt.x;
  ypos = pt.y;
}
*/

// BEW new version 3June09 - logic identical to Bill's except: (1) where desiredViewBottom
// is calculated -- the new calculations define it as desiredViewTop + view window depth,
// and (2) we adjust that value (Bill's "sanity check" if it goes beyond the logical doc
// bound), as done before, and (3) we don't leave the box unscrolled if top and bottom
// satisfy separate conditions but rather force it to be mid-window, except for
// adjustments when auto-highlighting is to be made visible
// BEW changed 3Jun309 to make smarter when auto-inserts are done
// BEW 13Jan12, added a separate block to be used when free translation mode is current.
// It supports narrow or small screens better. If 5 or less strips are showable in the
// window, it shows only about one strip's worth of context above the phrase box's strip,
// otherwise it shows two strip's worth
// BEW refactored 7Feb13. Following Ross Jones' request for a scrolling logic which keeps
// strips unmoved while the box is visible and within context boundaries, so that the
// phrase box moves down rather than strips scrolling up continually, is implemented. I've
// done it so that the legacy AI 6.x.y scroll is retained as the default, but Preferences
// > View page allows the user to choose this older legacy type of scroll behaviour (the
// controlling variable is app's m_bKeepBoxMidscreen, default TRUE), and the project
// config file stores this boolean's value. The legacy code, tweaked for a scrolling
// protocol matching Ross's wish, is in an else block at the bottom part of
// ScrollIntoView().
void CAdapt_ItCanvas::ScrollIntoView(int nSequNum)
{
	CAdapt_ItApp* pApp = &wxGetApp();
	wxASSERT(pApp != NULL);
	CLayout* pLayout = pApp->m_pLayout;

//	wxLogDebug(_T("%s:%s():line %d, m_bFreeTranslationMode = %s"), __FILE__, __FUNCTION__, __LINE__,
//		(&wxGetApp())->m_bFreeTranslationMode ? _T("TRUE") : _T("FALSE"));

	if (pApp->m_bKeepBoxMidscreen) {
	// COMMENCE code for scrolling regime "keep phrase box midscreen"


	if (pApp->m_nActiveSequNum == -1)
	{
		return; // do nothing if the phrase box is hidden because we are at doc end
	}
#ifdef Do_Clipping
	// disable clipping, but note below - if we determine that no scroll is needed we will
	// turn the flag off so that clipping becomes possible (provided the CLayout bool
	// m_bAllowScrolling is also TRUE)
	pLayout->SetScrollingFlag(TRUE);  // turned off at the end of Draw()

#endif
	//#ifdef _debugLayout
	//ShowSPandPile(393, 50);
	//ShowSPandPile(394, 50);
	//ShowInvalidStripRange();
	//#endif

// ------------------------------------------------------------------------------------------

//#if defined(_FT_ADJUST) && defined(_DEBUG)
	if (!pApp->m_bFreeTranslationMode)
	{
//#endif
	// the legacy scroll into view code - now used when not in free translation mode

	bool debugDisableScrollIntoView = FALSE; // set TRUE to disable ScrollIntoView
	if (!debugDisableScrollIntoView)
	{
		CAdapt_ItView* pView = pApp->GetView();
		CPile* pPile = pView->GetPile(nSequNum);
		//CStrip* pStrip = pPile->GetStrip(); // unused
		//wxRect rectStrip = pStrip->GetStripRect(); // unused

		// get the visible rectangle's coordinates
		wxRect visRect; // wxRect rectClient;
        // wx note: calling GetClientSize on the canvas produced different results in wxGTK
        // and wxMSW, so I'll use my own GetCanvasClientSize() which calculates it from the
        // main frame's client size after subtracting the controlBar's height and
        // composeBar's height (if visible).
		wxSize canvasSize;
		canvasSize = pApp->GetMainFrame()->GetCanvasClientSize();
		visRect.width = canvasSize.GetWidth();
		visRect.height = canvasSize.GetHeight();

        // calculate the window depth, and then how many strips are fully visible in it; we
        // will use the latter in order to change the behaviour so that instead of
        // scrolling so that the active strip is at the top (which hides preceding context
        // and so is a nuisance), we will scroll to somewhere a little past the window
        // center (so as to show more, rather than less, of any automatic inserted material
        // which may have background highlighting turned on)
		// BEW 26Apr09: legacy app included 3 pixels plus height of free trans line (when
		// in free translation mode) in the now removed m_curPileHeight value;
		// the refactored design doesn't so I'll have to add them here
		int nWindowDepth = visRect.GetHeight();
		int nStripHeight = pLayout->GetPileHeight() + pLayout->GetCurLeading();
		if (pApp->m_bFreeTranslationMode)
		{
			nStripHeight += 3 + pLayout->GetTgtTextHeight();
		}
		int nVisStrips = nWindowDepth / nStripHeight;
		if (nWindowDepth % nStripHeight > 0) // modulo
			nVisStrips++; // add 1 if a part strip fits as well

		// get the current horizontal and vertical pixels currently scrolled
		int xPixelsPerUnit,yPixelsPerUnit; // needed farther below
		GetScrollPixelsPerUnit(&xPixelsPerUnit,&yPixelsPerUnit);

        // determine how much preceding context (ie. how many strips) we want to try make
        // visible above the phrase box when auto inserting (so as to show as much
        // highlighted material as possible); we make this amount equal to nVisStrips less
        // two (one for the phrase box strip itself, and another for one strip of following
        // context below it) when auto inserting, but a much bigger value (see below) when
        // auto inserting stops (so that more of any auto inserted & hilighted adapatations
        // will be visible to the user without scrolling)
        int numTopHalfStrips = nVisStrips / 2;

		// for debugging purposes
#ifdef DEBUG_ScrollIntoView
		int midstripPixelDist = numTopHalfStrips * nStripHeight;
			CSourcePhrase* pSrcPhrase = pApp->m_pActivePile->GetSrcPhrase();
			wxLogDebug(_T("\nScollIntoView: m_srcPhrase %s  , numTopHalfStrips %d ,  midstripPixelDist  %d , nVisStrips %d"),
						pSrcPhrase->m_srcPhrase, numTopHalfStrips, midstripPixelDist, nVisStrips);
			if (pApp->m_bAutoInsert)
				wxLogDebug(_T("nStripHeight %d ,  nWindowDepth  %d , pApp->m_bAutoInsert is TRUE"),
						nStripHeight, nWindowDepth);
			else
				wxLogDebug(_T("nStripHeight %d ,  nWindowDepth  %d , pApp->m_bAutoInsert is FALSE"),
						nStripHeight, nWindowDepth);
#endif
		int nBoundForPrecedingContextStrips = nVisStrips - 3;
		int nPrecedingContextDepth = numTopHalfStrips * nStripHeight; // nStripHeight
		// includes the leading; the value calculated here is the default, it may
		// may be changed by the code a little further below where auto-insertions
		// are taken into account when the box has halted

        // Get the required y-coord of the top of the phrase box's strip where the "strip"
        // includes its preceding leading -- that is, the distance from the start of the
        // document to the beginning of the leading for the active strip (the new value was
        // determined by a prior call to RecalcLayout)
		int yDistFromDocStartToStripTopLeading =
										pPile->GetPileRect().GetTop() - pApp->m_curLeading;
#ifdef DEBUG_ScrollIntoView
			wxLogDebug(_T("Pixels to top of active strip %d  , nBoundForPrecedingContextStrips %d strips, Default nPrecedingContextDepth %d pixels"),
						yDistFromDocStartToStripTopLeading, nBoundForPrecedingContextStrips, nPrecedingContextDepth);
#endif
       // wx version design considerations 14Sep06: Most of the MFC ScrollIntoView code is
        // designed to figure out how much to scroll from the current scroll position to
        // get to the new position using ScrollWindow. But, the wx version equivalent of
        // ScrollWindow is Scroll(x,y) which scrolls to a given position in the doc,
        // eliminating the need to determine an "amount" to scroll from a current position.

        // if auto inserting, use an nPrecedingContextDepth value that puts the box about
        // mid way down the view window; similarly for review mode, or when in single step
        // mode; but if in automatic mode and m_bAutoInsert is FALSE (eg. as when the auto
        // insertions have just stopped) then take into account the number of auto-inserted
        // (ie. highlighted) strips and try show them all while keeping the box mid-window,
        // but show the box lower if necessary, but always maintain at least two strips of
        // "following context" at the window bottom
		int nAutoInsertedStripsCount = 0;
		bool bPhraseBoxIsInLastAutoInsertedStrip = FALSE;
		bool bAutoInsertionsExist = pApp->m_pLayout->GetHighlightedStripsRange(
			nAutoInsertedStripsCount, bPhraseBoxIsInLastAutoInsertedStrip);
		int nPrecedingContextDepth_Max = nBoundForPrecedingContextStrips * nStripHeight;

#ifdef DEBUG_ScrollIntoView
		if (bAutoInsertionsExist)
			wxLogDebug(_T("bAutoInsertionsExist is TRUE , nAutoInsertedStripsCount %d strips, bPhraseBoxIsInLastAutoInsertedStrip bool is %d"),
						bAutoInsertionsExist, nAutoInsertedStripsCount, (int)bPhraseBoxIsInLastAutoInsertedStrip);
		else
			wxLogDebug(_T("bAutoInsertionsExist is FALSE , nAutoInsertedStripsCount %d strips, bPhraseBoxIsInLastAutoInsertedStrip bool is %d"),
						bAutoInsertionsExist, nAutoInsertedStripsCount, (int)bPhraseBoxIsInLastAutoInsertedStrip);
		wxLogDebug(_T("nPrecedingContextDepth_Max (the bound) is  %d  pixels"), nPrecedingContextDepth_Max);
#endif
		int numExtrasToShow = 0; // how many extra strips need to be shown if not all will
								 // fit in the top half of the window
		int numExtras_Max = 0; // maximum number of extra highlighted strips that can be
					// shown by moving the phrase box's strip lower, without exceeding
					// the calculated bound given by nBoundForPrecedingContextStrips
		if (!pApp->m_bAutoInsert)
		{
			if (pApp->m_bDrafting || !pApp->m_bSingleStep) // whm added
			{
				if (bAutoInsertionsExist)
				{
#ifdef DEBUG_ScrollIntoView
		wxLogDebug(_T("m_bAutoInsert is FALSE &  bAutoInsertionsExist is TRUE"));
#endif
					// try to keep the nPrecedingContextDepth value such that the active
					// strip is just at or below mid window vertically, but adjust the
					// value to accomodate making all the inserted material visible; and
					// when these two conditions conflict because the range of strips with
					// inserted material cannot be all shown without the phrase box being
					// too low in the window, use the bounding value which is already set
					if (bPhraseBoxIsInLastAutoInsertedStrip)
					{
						if (nAutoInsertedStripsCount - 1 <= numTopHalfStrips)
						{
							// nothing to be done, all the highlighted strips are visible
#ifdef DEBUG_ScrollIntoView
		wxLogDebug(_T("bPhraseBoxIsInLastAutoInsertedStrip is TRUE &  Do nothing because ALL strips are visible"));
#endif
							;
						}
						else
						{
							// not all the hightlighted strips are visible, so check out
							// whether or not we can make an adjustment to get them all
							// shown without going beyond the calculated bound, if so,
							// get the phrase box's strip shown the necessary amount lower
#ifdef DEBUG_ScrollIntoView
		wxLogDebug(_T("bPhraseBoxIsInLastAutoInsertedStrip is TRUE  &  NOT ALL strips are visible -- adjust now"));
#endif
							numExtras_Max = nBoundForPrecedingContextStrips - nAutoInsertedStripsCount + 1;
							numExtrasToShow = nAutoInsertedStripsCount - (numTopHalfStrips + 1);
							if (numExtrasToShow > numExtras_Max)
							{
								// use the bounding value instead, user will have to
								// manually scroll if he wants to see all of the
								// highlighted ones
								nPrecedingContextDepth = nPrecedingContextDepth_Max;
#ifdef DEBUG_ScrollIntoView
		wxLogDebug(_T("numExtras_Max = %d strips ,  numExtrasToShow = %d strips , TOO MANY so nPrecedingContextDepth = %d"),
					numExtras_Max,numExtrasToShow,nPrecedingContextDepth);
#endif
							}
							else
							{
								// we can make all the highlighted strips visible by
								// showing the box numExtrasToShow's amount of strips
								// lower in the view window
								nPrecedingContextDepth += numExtrasToShow * nStripHeight;
#ifdef DEBUG_ScrollIntoView
		wxLogDebug(_T("numExtras_Max = %d strips ,  numExtrasToShow = %d strips , NOT TOO MANY so nPrecedingContextDepth = %d"),
					numExtras_Max,numExtrasToShow,nPrecedingContextDepth);
#endif
							}
						}
					}
					else
					{
						if (nAutoInsertedStripsCount <= numTopHalfStrips)
						{
							// nothing to be done, all the highlighted strips are visible
#ifdef DEBUG_ScrollIntoView
		wxLogDebug(_T("bPhraseBoxIsInLastAutoInsertedStrip is FALSE &  Do nothing because ALL strips are visible"));
#endif
							;
						}
						else
						{
							// not all the hightlighted strips are visible, so check out
							// whether or not we can make an adjustment to get them all
							// shown without going beyond the calculated bound, if so,
							// get the phrase box's strip shown the necessary amount lower
#ifdef DEBUG_ScrollIntoView
		wxLogDebug(_T("bPhraseBoxIsInLastAutoInsertedStrip is FALSE  &  NOT ALL strips are visible -- adjust now"));
#endif
							numExtras_Max = nBoundForPrecedingContextStrips - nAutoInsertedStripsCount;
							numExtrasToShow = nAutoInsertedStripsCount - numTopHalfStrips;
							if (numExtrasToShow > numExtras_Max)
							{
								// use the bounding value instead, user will have to
								// manually scroll if he wants to see all of the
								// highlighted ones
								nPrecedingContextDepth = nPrecedingContextDepth_Max;
#ifdef DEBUG_ScrollIntoView
		wxLogDebug(_T("numExtras_Max = %d strips ,  numExtrasToShow = %d strips , TOO MANY so nPrecedingContextDepth = %d"),
					numExtras_Max,numExtrasToShow,nPrecedingContextDepth);
#endif
							}
							else
							{
								// we can make all the highlighted strips visible by
								// showing the box numExtrasToShow's amount of strips
								// lower in the view window
								nPrecedingContextDepth += numExtrasToShow * nStripHeight;
#ifdef DEBUG_ScrollIntoView
		wxLogDebug(_T("numExtras_Max = %d strips ,  numExtrasToShow = %d strips , NOT TOO MANY so nPrecedingContextDepth = %d"),
					numExtras_Max,numExtrasToShow,nPrecedingContextDepth);
#endif
							}
						}
					}
				}
			}
			else // review mode or it's single step
			{
#ifdef DEBUG_ScrollIntoView
		wxLogDebug(_T("Review mode, or it's single step mode, so -- NO ADJUSTMENT required (phrase box should be mid window)"));
#endif
			//	nPrecedingContextDepth = nVisStrips / 2 * nStripHeight + 1; // mid window,
			//													// or a little below that
			}
		}

		// get the desired logical top (ie. y Distance) to the desired scroll position
		// (this calculation will yield a negative number if the target strip is closer
		// to the top of the virtual document than the value of nPrecedingContextDepth)
		int desiredViewTop = yDistFromDocStartToStripTopLeading - nPrecedingContextDepth;
#ifdef DEBUG_ScrollIntoView
		wxLogDebug(_T("Initial desiredViewTop = %d"),desiredViewTop);
#endif
#ifdef Do_Clipping
		int old_desiredTop = desiredViewTop; // for anti-flicker support
#endif
		// make a sanity check on the above value
		if (desiredViewTop < 0)
		{
			desiredViewTop = 0;
#ifdef DEBUG_ScrollIntoView
		wxLogDebug(_T("Final desiredViewTop = 0 (reset because was negative)"));
#endif
		}
		//-------------------- now the desiredViewBottom calculations ---------------

        // Determine the desired bottom position in the document of the content we wish to
        // view. We do this by adding the window depth to the desiredViewTop value. When
        // the active strip gets close to the end of the logical document, this could
        // result in a value which exceeds the logical document's height - so if that is
        // the case, we force the bottom of the view to be at the end of the logical
        // document - except when the document height is so small that all of it fits
        // within the view - in that case we just show it all. (Remember that
        // desiredViewTop is the vertical distance that we want the scroll car to track,
        // therefore everything has to be calculated based on the desiredViewTop value.)
		int desiredViewBottom = desiredViewTop + nWindowDepth;
		bool bForceRepositioningToDocEnd = FALSE;
		wxSize virtDocSize;
		GetVirtualSize(&virtDocSize.x,&virtDocSize.y); // GetVirtualSize gets size in pixels
#ifdef DEBUG_ScrollIntoView
		wxLogDebug(_T("Initial desiredViewBottom = %d"),desiredViewBottom);
#endif
		// sanity check on the above value
		if (desiredViewBottom > virtDocSize.y)
		{
			desiredViewBottom = virtDocSize.y;
			bForceRepositioningToDocEnd = TRUE;
#ifdef DEBUG_ScrollIntoView
		wxLogDebug(_T("Final desiredViewBottom = %d (reset smaller because value was too great"),desiredViewBottom);
#endif
		}

		// now we are ready to scroll - we don't care where the scroll car currently is,
		// because the Scroll() function in wxWidgets is absolute, and resets the car
		// position to whatever location we've calculated as the desiredViewTop
		if (nWindowDepth >= virtDocSize.y)
		{
			// it's a short document that fits in the current view window's vertical
			// dimensions
			Scroll(0,0); // Scroll takes scroll units not pixels
#ifdef DEBUG_ScrollIntoView
		wxLogDebug(_T("Short doc, scroll car must be at 0"));
#endif
		}
		else
		{
			// the logical document is longer than the view window, so the scroll bar will
			// be active (the Scroll call takes scroll units) Note: see the comment about
			// the kluge done in CLayout::SetLogicalDocHeight(), the comment is within
			// that function - it prevents the last strip, when box is in it and mode is
			// vertical editing, from being below the bottom of the view window.
			// Increasing the y value in the Scroll() call below doesn't do what we want.
			if (bForceRepositioningToDocEnd)
			{
				// active location is at or near doc end, so scroll to the end; allow for
				// granularity of yPixelsPerUnit, add 1 more
				Scroll(0, (virtDocSize.y - nWindowDepth) / yPixelsPerUnit + 1);
#ifdef DEBUG_ScrollIntoView
		wxLogDebug(_T("Scroll forced to doc end"));
#endif
			}
			else
			{
                // normal situation: active strip is either near the doc top (and remember
                // that the needed ajustment for the value of desiredViewTop has been made
                // already), or it is somewhere in the document and not near either end
#ifdef Do_Clipping
// 				wxLogDebug(_T("allow clipping flag is %s, old_desiredTop  %d, desiredViewTop  %d  Test is %s , Clipped? %s"),
//				pLayout->GetAllowClippingFlag() ? _T("TRUE") : _T("FALSE"),
//				old_desiredTop,desiredViewTop,
//				old_desiredTop == desiredViewTop ? _T("TRUE") : _T("FALSE"),
//				pLayout->GetAllowClippingFlag() && !pLayout->GetScrollingFlag() != TRUE ? _T("YES") : _T("NO"));
				if (old_desiredTop == desiredViewTop)
				{
					// no scroll is needed, so clipping is potentially possible (provided
					// m_bAllowClipping is also TRUE)
					pLayout->SetScrollingFlag(FALSE); // clear m_bScrolling to FALSE
				}
#endif
				//int scrollRange = GetScrollRange(wxVERTICAL);
				//wxLogDebug(_T("scrollRange = %d  steps"), scrollRange);
				Scroll(0,desiredViewTop / yPixelsPerUnit); // Scroll takes scroll units not pixels

#ifdef DEBUG_ScrollIntoView
		wxLogDebug(_T("Typical scroll, scroll car set to yDist of  %d pixels"),desiredViewTop);
		int newXPos,current_yDistFromDocStartToViewTop;
		CalcUnscrolledPosition(0,0,&newXPos,&current_yDistFromDocStartToViewTop);
		wxLogDebug(_T("Scroll Distance =  %d logical units; +ve value means Scrolled DOWN, -ve means Scrolled UP , old car position = %d logical units"),
			(desiredViewTop/yPixelsPerUnit - current_yDistFromDocStartToViewTop/yPixelsPerUnit), current_yDistFromDocStartToViewTop/yPixelsPerUnit);
#endif
			}
		}
	}
//	wxLogDebug(_T("%s:%s():line %d, m_bFreeTranslationMode = %s"), __FILE__, __FUNCTION__, __LINE__,
//		(&wxGetApp())->m_bFreeTranslationMode ? _T("TRUE") : _T("FALSE"));

//#if defined(_FT_ADJUST) && defined(_DEBUG)
	}
//#endif

// ------------------------------------------------------------------------------------------------------

//#if defined(_FT_ADJUST) && defined(_DEBUG)
	else
	{
//#endif
	// The (hopefully simpler) free-translation supporting scroll into view code

	bool debugDisableScrollIntoView = FALSE; // set TRUE to disable ScrollIntoView
	if (!debugDisableScrollIntoView)
	{
		CAdapt_ItView* pView = pApp->GetView();
		CPile* pPile = pView->GetPile(nSequNum);
		//CStrip* pStrip = pPile->GetStrip(); // unused
		//wxRect rectStrip = pStrip->GetStripRect(); // unused

		// get the visible rectangle's coordinates
		wxRect visRect; // wxRect rectClient;
        // wx note: calling GetClientSize on the canvas produced different results in wxGTK
        // and wxMSW, so I'll use my own GetCanvasClientSize() which calculates it from the
        // main frame's client size after subtracting the controlBar's height and
        // composeBar's height (if visible).
		wxSize canvasSize;
		canvasSize = pApp->GetMainFrame()->GetCanvasClientSize();
		//visRect.width = canvasSize.GetWidth();
		visRect.height = canvasSize.GetHeight();

        // calculate the window depth, and then how many strips are fully visible in it; we
        // will use the latter in order to change the behaviour so that instead of
        // scrolling so that the active strip is at the top (which hides preceding context
        // and so is a nuisance), we will scroll to somewhere a little past the window
        // center (so as to show more, rather than less, of any automatic inserted material
        // which may have background highlighting turned on)
		// BEW 26Apr09: legacy app included 3 pixels plus height of free trans line (when
		// in free translation mode) in the now removed m_curPileHeight value;
		// the refactored design doesn't so I'll have to add them here
		int nWindowDepth = visRect.GetHeight();

		int nStripHeight = pLayout->GetPileHeight() + pLayout->GetCurLeading();
		if (pApp->m_bFreeTranslationMode)
		{
			nStripHeight += 3 + pLayout->GetTgtTextHeight();
		}
		int nVisStrips = nWindowDepth / nStripHeight;
		if (nWindowDepth % nStripHeight > 0) // modulo
			nVisStrips++; // add 1 if a part strip fits as well

		// get the current horizontal and vertical pixel scale factors for scroll units
		int xPixelsPerUnit,yPixelsPerUnit; // needed farther below
		GetScrollPixelsPerUnit(&xPixelsPerUnit,&yPixelsPerUnit);

        // determine how much preceding context (ie. how many strips) we want to try make
        // visible above the phrase box  - for free translation, we'll use a sliding scale
        // depending on how many strips fit. For 5 or less, show one above; otherwise show 2
        int numTopHalfStrips;
		if (nVisStrips <= 5)
			numTopHalfStrips = 1;
		else
			numTopHalfStrips = 2;

		//int nBoundForPrecedingContextStrips = nVisStrips - 3;

		int nPrecedingContextDepth = numTopHalfStrips * nStripHeight;
        // nStripHeight calculated above includes the leading

        // Get the required y-coord of the top of the phrase box's strip where the "strip"
        // includes its preceding leading -- that is, the distance from the start of the
        // document to the beginning of the leading for the active strip (the new value was
        // determined by a prior call to RecalcLayout)
		int yDistFromDocStartToStripTopLeading = pPile->GetPileRect().GetTop() - pApp->m_curLeading;

		// get the desired logical top (ie. y Distance) to the desired scroll position
		// (this calculation will yield a negative number if the target strip is closer
		// to the top of the virtual document than the value of nPrecedingContextDepth)
		int desiredViewTop = yDistFromDocStartToStripTopLeading - nPrecedingContextDepth;
#ifdef Do_Clipping
		int old_desiredTop = desiredViewTop; // for anti-flicker support
#endif
		// make a sanity check on the above value
		if (desiredViewTop < 0)
		{
			desiredViewTop = 0;
		}
		//-------------------- now the desiredViewBottom calculations ---------------

        // Determine the desired bottom position in the document of the content we wish to
        // view. We do this by adding the window depth to the desiredViewTop value. When
        // the active strip gets close to the end of the logical document, this could
        // result in a value which exceeds the logical document's height - so if that is
        // the case, we force the bottom of the view to be at the end of the logical
        // document - except when the document height is so small that all of it fits
        // within the view - in that case we just show it all. (Remember that
        // desiredViewTop is the vertical distance that we want the scroll car to track,
        // therefore everything has to be calculated based on the desiredViewTop value.)
		int desiredViewBottom = desiredViewTop + nWindowDepth;
		bool bForceRepositioningToDocEnd = FALSE;
		wxSize virtDocSize;
		GetVirtualSize(&virtDocSize.x,&virtDocSize.y); // GetVirtualSize gets size in pixels

		// sanity check on the above value
		if (desiredViewBottom > virtDocSize.y)
		{
			desiredViewBottom = virtDocSize.y;
			bForceRepositioningToDocEnd = TRUE;
		}

		// now we are ready to scroll - we don't care where the scroll car currently is,
		// because the Scroll() function in wxWidgets is absolute, and resets the car
		// position to whatever location we've calculated as the desiredViewTop
		if (nWindowDepth >= virtDocSize.y)
		{
			// it's a short document that fits in the current view window's vertical
			// dimensions
			Scroll(0,0); // Scroll takes scroll units not pixels
		}
		else
		{
			// the logical document is longer than the view window, so the scroll bar will
			// be active (the Scroll call takes scroll units) Note: see the comment about
			// the kluge done in CLayout::SetLogicalDocHeight(), the comment is within
			// that function - it prevents the last strip, when box is in it and mode is
			// vertical editing, from being below the bottom of the view window.
			// Increasing the y value in the Scroll() call below doesn't do what we want.
			if (bForceRepositioningToDocEnd)
			{
				// active location is at or near doc end, so scroll to the end; allow for
				// granularity of yPixelsPerUnit, add 1 more
				Scroll(0, (virtDocSize.y - nWindowDepth) / yPixelsPerUnit + 1);
			}
			else
			{
                // normal situation: active strip is either near the doc top (and remember
                // that the needed ajustment for the value of desiredViewTop has been made
                // already), or it is somewhere in the document and not near either end
#ifdef Do_Clipping
				if (old_desiredTop == desiredViewTop)
				{
					// no scroll is needed, so clipping is potentially possible (provided
					// m_bAllowClipping is also TRUE)
					pLayout->SetScrollingFlag(FALSE); // clear m_bScrolling to FALSE
				}
#endif
				Scroll(0,desiredViewTop / yPixelsPerUnit); // Scroll takes scroll units not pixels
			}
		}
	}
#if defined (FREETRMODE)
	wxLogDebug(_T("%s:%s():line %d, m_bFreeTranslationMode = %s"), __FILE__, __FUNCTION__, __LINE__,
		(&wxGetApp())->m_bFreeTranslationMode ? _T("TRUE") : _T("FALSE"));
#endif
//#if defined(_FT_ADJUST) && defined(_DEBUG)
	}
//#endif
	// end of the free translation supporting else block

	// END of code for scrolling regime "keep phrase box midscreen"
	}
	else
	{
    // COMMENCE code for scrolling regime "keep strips stationary (until an autoscroll is
    // forced on us), and move the box down the strips until it reaches the last, then do
    // autoscroll"
    // BEW comment 7Feb13.(This is legacy code, from 9May2009 (svn r500) tweaked for using
    // the refactoring's accessor functions for pile and strip and layout; but with
    // additional logic changes. A logic 'bad feature' is now fixed. A legacy variable
    // called desiredViewBottom (i.e. the place in the view where we don't want the bottom
    // of the phrase box's strip to be below [which would bring it too close to the window
    // bottom] is calculated, so far so good, but the failure of two previous tests
    // resulted in a third using that variable, and the test would always be TRUE,
    // resulting in the phrase box being kept at 3 strips from the bottom of the window.
    // The logic error here is that autoscrolling thereby nearly always ignores the
    // desiredViewTop value, and the box never gets put near the client area's top, and so
    // the user never sees a temporarily 'fixed' vertical location of the strips while just
    // the box moves down vertically as the user adapts. I've reworked the protocol. The
    // desiredViewBottom calculation really is not a place for where to scroll to when all
    // else fails; instead, it's a location at which if the box moves down past it, then it
    // should trigger a scroll down so that the box ends up at the desiredViewTop location.
    // So I'm renaming desiredViewBottom as scrollTriggerLocation (I'll make it be one
    // strip height short of the client area bottom), and when the box moves past that
    // vertical location, an autoscroll is done. The scroll calculations are done at the
    // bottom of this code, I'll comment out the legacy code there, and rewrite the scroll
    // logic according to the new protocol described here. Also, desiredViewTop is set at
	// 3 strips from bottom of client area, which is unfortunate - I want it to be such
	// that there is only a single strip of preceding context visible in the window (when
	// an autoscroll happens, the viewing eyes need some earlier context visible so they
	// can pick up where their attention should be; those cues would be missing if we
	// scrolled so as to put the phrase box at the top strip)

	if (pApp->m_nActiveSequNum == -1)
	{
		return; // do nothing if the phrase box is hidden because we are at doc end
	}
	bool debugDisableScrollIntoView = FALSE; // set TRUE to disable ScrollIntoView
	if (!debugDisableScrollIntoView)
	{
		CAdapt_ItView* pView = pApp->GetView();
		CPile* pPile = pView->GetPile(nSequNum);

		// get the visible rectangle's coordinates
		wxRect visRect; // wxRect rectClient;
		// wx note: calling GetClientSize on the canvas produced different results in wxGTK
		// and wxMSW, so I'll use my own GetCanvasClientSize() which calculates it from the
		// main frame's client size after subtracting the controlBar's height and
		// composeBar's height (if visible).
		wxSize canvasSize;
		canvasSize = pApp->GetMainFrame()->GetCanvasClientSize();
		visRect.width = canvasSize.GetWidth();
		visRect.height = canvasSize.GetHeight();

		// calculate the window depth, and then how many strips are fully visible in it; we
		// will use the latter in order to change the behaviour so that instead of
		// scrolling so that the active strip is at the top (which hides preceding context
		// and so is a nuisance), we will scroll to somewhere a little past the window
		// center (so as to show more, rather than less, of any automatic inserted material
		// which may have background highlighting turned on)...

		int nWindowDepth = visRect.GetHeight();
        // BEW 7Feb13 the following strip height calculation takes into account the height
        // adjustment required if free translation mode is currently on
		int nStripHeight = pLayout->GetStripHeight() + pLayout->GetCurLeading();

		// get the current horizontal and vertical pixels currently scrolled
		int xPixelsPerUnit,yPixelsPerUnit; // needed farther below
		GetScrollPixelsPerUnit(&xPixelsPerUnit,&yPixelsPerUnit);

        // Determine how much preceding context (ie. how many strips) we want to try make
        // visible above the phrase box when auto inserting (so as to show some cues for
        // reorienting where the eye should be looking); we make this amount equal to one
        // strip lower than the visual top. The legacy protocol adjusted the calculated
        // distance depending on mode - I'm not sure yet to what extent I'll comply with
        // that
		int nPrecedingContextDepth = nStripHeight; // nStripHeight includes the leading

        // Get the required y-coord of the top of the phrase box's strip -- that is, the
        // distance from the start of the document to the top of the top cell for the
        // active strip (the new value was determined by a prior call to RecalcLayout)
		int yDistFromDocStartToActiveStripTop = pPile->GetPileRect().GetTop();

		// if auto inserting, use a small nPrecedingContextDepth value; but if not (eg. as
	    // when the auto insertions have just stopped) then put the box near the bottom of
	    // the window to show more of the preceding context
	    /* the legacy adjustments - remove shortly as the code above makes them wrong now
		int nVisStrips = nWindowDepth / nStripHeight;
		if (!pApp->m_bAutoInsert)
		{
			if (pApp->m_bDrafting || !pApp->m_bSingleStep) // whm added
				nPrecedingContextDepth = (nVisStrips - 3) * nStripHeight; // whm changed 2 to 3
			else
				nPrecedingContextDepth = nVisStrips / 2 * nStripHeight;
		}
		*/

		// get the desired logical top (ie. y Distance) to the desired scroll position
		// (this calculation will yield a negative number if the target strip is closer
		// to the top of the virtual document than the value of nPrecedingContextDepth)
		int desiredViewTop = yDistFromDocStartToActiveStripTop - nPrecedingContextDepth;

		// make a sanity check on the above value
		if (desiredViewTop < 0)
			desiredViewTop = 0;

		//-------------------- now the scrollTriggerLocation calculations ---------------

		// Determine the trigger location. If, like in the legacy calculations, we base
		// the value on where the bottom of the active strip would be to be near the
		// window bottom but at least a strip height's short of that, to avoid a scroll,
		// then we'd have the right value.
		wxSize virtDocSize;
		GetVirtualSize(&virtDocSize.x,&virtDocSize.y); // GetVirtualSize gets size in pixels
		int scrollTriggerLocation = pPile->GetPileRect().GetBottom();
		// if free translating, add in the additional vertical space taken by the free
		// translation line
		if (pApp->m_bFreeTranslationMode)
		{
			scrollTriggerLocation += 3 + pApp->m_nTgtHeight;
		}
		// increment scrollTriggerLocation by the height of one strip; note, this value may
		// be greater than virtDocSize.y, so if we get to use it, we'll need to do a
		// sanity check first and make it equal to virtDocSize.y
		//scrollTriggerLocation += nStripHeight;
#if defined (FREETRMODE)
		wxLogDebug(_T("%s:%s():line %d, m_bFreeTranslationMode = %s"), __FILE__, __FUNCTION__, __LINE__,
			(&wxGetApp())->m_bFreeTranslationMode ? _T("TRUE") : _T("FALSE"));
#endif
		// sanity check on the above value
		if (scrollTriggerLocation > virtDocSize.y)
			scrollTriggerLocation = virtDocSize.y;
	    /* legacy calc, where scrollTriggerLocation was called desiredViewBottom
		wxSize virtDocSize;
		GetVirtualSize(&virtDocSize.x,&virtDocSize.y); // GetVirtualSize gets size in pixels
		int scrollTriggerLocation = pPile->GetPileRect().GetBottom();
		if (pApp->m_bFreeTranslationMode)
		{
			scrollTriggerLocation += 3 + pApp->m_nTgtHeight;
		}
		// increment scrollTriggerLocation by the amount of two strips; note, this value may
		// be greater than virtDocSize.y, so if we get to use it, we'll need to do a
		// sanity check first and make it equal to virtDocSize.y
		scrollTriggerLocation += 2*nStripHeight;

		// sanity check on the above value
		if (scrollTriggerLocation > virtDocSize.y)
			scrollTriggerLocation = virtDocSize.y;
		*/

		//-------------------- now the current position data ------------------------

		// get the current y distance to the top of the view, that is, to the top of the
		// client area
		int newXPos,current_yDistFromDocStartToViewTop;
		CalcUnscrolledPosition(0,0,&newXPos,&current_yDistFromDocStartToViewTop);

		//------- now check if no scroll needed, or if needed, do it -----------------

		// first check if no scroll is needed - return if it isn't needed
		if ((desiredViewTop >= current_yDistFromDocStartToViewTop) &&
			(scrollTriggerLocation <= current_yDistFromDocStartToViewTop + nWindowDepth - nStripHeight))
		{
			// no scroll needed, the box is visible and within the preceding and following
			// context depths, so just return
			return;
		}

        // Have we reached the trigger location, at which a scroll must be done? But first
        // check for the box having reached the last visible strip of the document if so,
        // don't scroll, just leave it there
        if (scrollTriggerLocation == virtDocSize.y)
		{
			return;
		}
		else if (scrollTriggerLocation >= current_yDistFromDocStartToViewTop + nWindowDepth - nStripHeight)
		{
			Scroll(0, desiredViewTop / yPixelsPerUnit); // Scroll takes scroll units not pixels
			return;
		}
		// Third test must ensure that the phrase box isn't left above the top of the
		// client area...
		// handle the situation where the desiredViewTop is < (i.e., above)
		// the current logical view top
		if (desiredViewTop < current_yDistFromDocStartToViewTop)
		{
			Scroll(0,desiredViewTop / yPixelsPerUnit); // Scroll takes scroll units not pixels
			return;
		}
#if defined (FREETRMODE)
		wxLogDebug(_T("%s:%s():line %d, m_bFreeTranslationMode = %s"), __FILE__, __FUNCTION__, __LINE__,
			(&wxGetApp())->m_bFreeTranslationMode ? _T("TRUE") : _T("FALSE"));
#endif
		/* legacy scroll code, which unfortunately worked to keep the box near window bottom
		// handle the situation where the desiredViewTop is < (i.e., above)
		// the current logical view top
		if (desiredViewTop < current_yDistFromDocStartToViewTop)
		{
			Scroll(0,desiredViewTop / yPixelsPerUnit); // Scroll takes scroll units not pixels
			return;
		}

		// handle the situation where the scrollTriggerLocation is > (i.e., above)
		// the current logical view bottom
		if (scrollTriggerLocation > current_yDistFromDocStartToViewTop + nWindowDepth)
		{
			Scroll(0, (scrollTriggerLocation - nWindowDepth) / yPixelsPerUnit); //takes scroll units
			return;
		}
		*/
	}
	// END of code for scrolling regime "keep strips as stationary as possible, and the
	// phrase box then moves down the client area until it reaches the last strip, and
	// then autoscroll is done to put it near the view top again"
	}
}

// Returns positive y-distance for the scroll down (whm: return value is never used)
int CAdapt_ItCanvas::ScrollDown(int nStrips)
{
	CAdapt_ItApp* pApp = &wxGetApp();
	wxASSERT(pApp != NULL);
	CLayout* pLayout = pApp->m_pLayout;
	wxPoint scrollPos;
	int xPixelsPerUnit,yPixelsPerUnit;
	GetScrollPixelsPerUnit(&xPixelsPerUnit,&yPixelsPerUnit);
#ifdef Do_Clipping
	pLayout->SetScrollingFlag(TRUE); // need full screen drawing, so clipping can't happen
#endif

	CalcUnscrolledPosition(0,0,&scrollPos.x,&scrollPos.y);

	wxRect visRect; // wxRect rectClient;
    // wx note: calling GetClientSize on the canvas produced different results in wxGTK and
    // wxMSW, so I'll use my own GetCanvasClientSize() which calculates it from the main
    // frame's client size.
	wxSize canvasSize;
	canvasSize = pApp->GetMainFrame()->GetCanvasClientSize();
	visRect.width = canvasSize.x;
	visRect.height = canvasSize.y;

	// make adjustment due to fact that some parameters values are calculated differently
	// in the refactored application
	int nCurrentPileHeight = pLayout->GetPileHeight();
	if (pApp->m_bFreeTranslationMode)
	{
		// the legacy app included the 3 pixels and tgt text height in the
		// calculation
		nCurrentPileHeight += 3 + pLayout->GetTgtTextHeight();
	}

	// calculate the window depth
	int yDist = 0;
	int nLimit = GetScrollRange(wxVERTICAL);
    // whm note: GetscrollRange returns scroll units, and MFC's GetScrollLimit apparently
    // returns device units (pixels), so must multiply the value obtained from
    // GetScrollRange by yPixelsPerUnit GetScrollRange got the range of scroll units for
    // the whole virtual document, not just the value of the upper left y coord for the
    // window when maximally scrolled.
    // To find the y coord for the client window at its maximal scrolled extent we need to
    // do the following:
	// Take the modulus of DocLengthInScrollUnits % ScrollUnitsPerPage
    // The modulus operation will give the number of scroll units that exist beyond the
    // last fully scrolled page. Multiply this value by pixelsPerScrollUnit to get the
    // amount the y coord of the client view should be moved up toward the beginning of the
    // doc to be the same value that MFC's GetScrollLimit obtains.
	int scrollUnitsPerPage = GetScrollThumb(wxVERTICAL);
	int unitsBeyondLastFullScrolledPage = nLimit % scrollUnitsPerPage;
	// reduce the nLimit by the unitsBeyondLastFullScrolledPage.
	nLimit -= unitsBeyondLastFullScrolledPage; // to make wxWindow::GetScrollRange ==
											   // CWnd::GetScrollLimit
	nLimit *= yPixelsPerUnit; // convert scroll units to pixels
	int nMaxDist = nLimit - scrollPos.y;

	// do the vertical scroll asked for
	yDist = nCurrentPileHeight + pLayout->GetCurLeading();
	yDist *= nStrips;

	if (yDist > nMaxDist)
	{
		scrollPos.y += nMaxDist;

		int posn = scrollPos.y;
		posn = posn / yPixelsPerUnit;
        // Note: MFC's ScrollWindow's 2 params specify the xAmount and yAmount to scroll in
        // device units (pixels). The equivalent in wx is Scroll(x,y) in which x and y are
        // in SCROLL UNITS (pixels divided by pixels per unit). Also MFC's ScrollWindow
        // takes parameters whose value represents an "amount" to scroll from the current
        // position, whereas the wxScrolledWindow::Scroll takes parameters which represent
        // an absolute "position" in scroll units. To convert the amount we need to add the
        // amount to (or subtract from if negative) the logical pixel unit of the upper
        // left point of the client viewing area; then convert to scroll units in Scroll().
        // whm note: wxScrolledWindow::Scroll() scrolls the window so the view start is at
        // the given point (expressed in scroll units)
		Scroll(0,posn);
		Refresh();
		return nMaxDist;
	}
	else
	{
		scrollPos.y += yDist;

		int posn = scrollPos.y;
		posn = posn / yPixelsPerUnit;
       // Note: MFC's ScrollWindow's 2 params specify the xAmount and yAmount to scroll in
        // device units (pixels). The equivalent in wx is Scroll(x,y) in which x and y are
        // in SCROLL UNITS (pixels divided by pixels per unit). Also MFC's ScrollWindow
        // takes parameters whose value represents an "amount" to scroll from the current
        // position, whereas the wxScrolledWindow::Scroll takes parameters which represent
        // an absolute "position" in scroll units. To convert the amount we need to add the
        // amount to (or subtract from if negative) the logical pixel unit of the upper
        // left point of the client viewing area; then convert to scroll units in Scroll().
        // whm note: wxScrolledWindow::Scroll() scrolls the window so the view start is at
        // the given point (expressed in scroll units)
		Scroll(0,posn);
		Refresh();
		return yDist;
	}
}

// Returns positive y-distance for the scroll up (whm: return value is never used)
int CAdapt_ItCanvas::ScrollUp(int nStrips)
{
	CAdapt_ItApp* pApp = &wxGetApp();
	wxASSERT(pApp != NULL);
	CLayout* pLayout = pApp->m_pLayout;
	wxPoint scrollPos;
	int xPixelsPerUnit,yPixelsPerUnit;
	GetScrollPixelsPerUnit(&xPixelsPerUnit,&yPixelsPerUnit);
#ifdef Do_Clipping
	pLayout->SetScrollingFlag(TRUE); // need full screen drawing, so clipping can't happen
#endif

	CalcUnscrolledPosition(0,0,&scrollPos.x,&scrollPos.y);

	int yDist;
	int nMaxDist = scrollPos.y;

	// make adjustment due to fact that some parameters values are calculated differently
	// in the refactored application
	int nCurrentPileHeight = pLayout->GetPileHeight();
	if (pApp->m_bFreeTranslationMode)
	{
		// the legacy app included the 3 pixels and tgt text height in the
		// calculation
		nCurrentPileHeight += 3 + pLayout->GetTgtTextHeight();
	}

	// do the vertical scroll asked for
	yDist = nCurrentPileHeight + pLayout->GetCurLeading();
	yDist *= nStrips;

	if (yDist > nMaxDist)
	{
        // the amount of scroll wanted is greater than the amount the window is already
        // scrolled, so only scroll up the exact amount the window is scrolled, bringing it
        // up to an unscrolled state
		scrollPos.y -= nMaxDist;
		int posn = scrollPos.y;
		wxASSERT(posn == 0); // should be zero
		posn = posn / yPixelsPerUnit;
        // Note: MFC's ScrollWindow's 2 params specify the xAmount and yAmount to scroll in
        // device units (pixels). The equivalent in wx is Scroll(x,y) in which x and y are
        // in SCROLL UNITS (pixels divided by pixels per unit). Also MFC's ScrollWindow
        // takes parameters whose value represents an "amount" to scroll from the current
        // position, whereas the wxScrolledWindow::Scroll takes parameters which represent
        // an absolute "position" in scroll units. To convert the amount we need to add the
        // amount to (or subtract from if negative) the logical pixel unit of the upper
        // left point of the client viewing area; then convert to scroll units in Scroll().
        // whm note: wxScrolledWindow::Scroll() scrolls the window so the view start is at
        // the given point (expressed in scroll units)
		Scroll(0,posn);
		Refresh();
		return nMaxDist;
	}
	else
	{
		// scroll up the amount requested
		scrollPos.y -= yDist;

		int posn = scrollPos.y;
		posn = posn / yPixelsPerUnit;
        // Note: MFC's ScrollWindow's 2 params specify the xAmount and yAmount to scroll in
        // device units (pixels). The equivalent in wx is Scroll(x,y) in which x and y are
        // in SCROLL UNITS (pixels divided by pixels per unit). Also MFC's ScrollWindow
        // takes parameters whose value represents an "amount" to scroll from the current
        // position, whereas the wxScrolledWindow::Scroll takes parameters which represent
        // an absolute "position" in scroll units. To convert the amount we need to add the
        // amount to (or subtract from if negative) the logical pixel unit of the upper
        // left point of the client viewing area; then convert to scroll units in Scroll().
        // whm note: wxScrolledWindow::Scroll() scrolls the window so the view start is at
        // the given point (expressed in scroll units)
		Scroll(0,posn);
		Refresh();
		return yDist;
	}
}

