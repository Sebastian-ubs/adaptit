// *****************************************************************************
/// \project		adaptit
/// \file			Strip.cpp
/// \author			Bill Martin
/// \date_created	26 March 2004
/// \rcs_id $Id$
/// \copyright		2008 Bruce Waters, Bill Martin, SIL International
/// \license		The Common Public License or The GNU Lesser General Public
///  License (see license directory)
/// \description	This is the implementation file for the CStrip class.
/// The CStrip class represents the next smaller divisions of a CBundle.
/// Each CStrip stores an ordered list of CPile instances, which are
/// displayed in LtoR languages from left to right, and in RtoL languages
/// from right to left.
/// \derivation		The CStrip class is derived from wxObject.
// *****************************************************************************

// the following improves GCC compilation performance
#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma implementation "Strip.h"
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

#include "Adapt_It.h"
//#include "SourceBundle.h"
#include "MainFrm.h"
#include "AdaptitConstants.h"
// don't mess with the order of the following includes, Strip must precede View must precede
// Pile must precede Layout and Cell can usefully by last
#include "Strip.h"
#include "Adapt_ItView.h"
#include "Pile.h"
#include "Layout.h"
#include "Cell.h"



// Define type safe pointer lists
#include "wx/listimpl.cpp"

/// This macro together with the macro list declaration in the .h file
/// complete the definition of a new safe pointer list class called StripList.
WX_DEFINE_LIST(StripList);

extern CAdapt_ItApp* gpApp;

extern wxRect	grectViewClient; // used in OnDraw() below
extern bool		gbRTL_Layout;
extern bool	gbShowTargetOnly; // definition in Adapt_ItView.cpp
/// This global is defined in Adapt_It.cpp. (default is FALSE)
extern bool gbDoingInitialSetup;


/// This global is defined in Adapt_It.cpp.
extern struct PageOffsets pgOffsets;

// BEW 22Feb10 no changes to any functions needed for support of doc version 5

// *******************************************************************
// Construction/Destruction
// *******************************************************************

IMPLEMENT_DYNAMIC_CLASS(CStrip, wxObject)

CStrip::CStrip()
{
	m_pLayout = NULL;
	m_nFree = -1;
	m_nStrip = -1;
	m_bValid = FALSE;
}

CStrip::CStrip(CLayout* pLayout)
{
	wxASSERT(pLayout != NULL);
	m_pLayout = pLayout;
	m_nFree = -1;
	m_nStrip = -1;
	m_bValid = FALSE;
}

CStrip::~CStrip()
{

}

wxArrayPtrVoid* CStrip::GetPilesArray()
{
	return &m_arrPiles;
}

int CStrip::GetPileCount()
{
	return m_arrPiles.GetCount();
}

CPile* CStrip::GetPileByIndex(int index)
{
	return (CPile*)m_arrPiles.Item(index);
}

void CStrip::Draw(wxDC* pDC)
{
//	wxLogDebug(_T("%s:%s():line %d, m_bFreeTranslationMode = %s"), __FILE__, __FUNCTION__, __LINE__,
//		(&wxGetApp())->m_bFreeTranslationMode ? _T("TRUE") : _T("FALSE"));

#if !defined(__WXGTK__)
	if (m_pLayout->m_pApp->m_bPagePrintInProgress)
	{
		// whm Note: The pOffsets members nTop and nBottom were negative in the MFC version,
		// but remain positive in the wx version.
		POList* pList = &m_pLayout->m_pApp->m_pagesList;
		POList::Node* pos = pList->Item(m_pLayout->m_pApp->m_nCurPage-1);
		// whm 27Oct11 added test to return if pos == NULL
		// This test is needed to prevent crash on Linux because the Draw()
		// function can get triggered by the Linux system on that platform
		// before App's m_nCurPage is calculated by OnPreparePrinting()'s
		// call of PaginateDoc() in the printing framework (see notes on
		// calling order of print routines starting at line 108 of
		// AIPrintout.cpp).
		// BEW 28Oct11, using m_bPagePrintInProgress in the above test, rather
		// than the earlier m_bIsPrinting should make the next two lines be
		// no longer needed, but they can remain for safety's sake
		if (pos == NULL)
			return;
		PageOffsets* pOffsets = (PageOffsets*)pos->GetData();
		if (m_nStrip < pOffsets->nFirstStrip || m_nStrip > pOffsets->nLastStrip)
			return;
	}
#endif

// declaration of Print_failure is at top of file (there's another in Adapt_It.h near top
// which can be used to turn on logging in many files which use that symbol)
#if defined(_DEBUG) && defined(Print_failure)
	if (m_pLayout->m_pApp->m_bIsPrinting && m_pLayout->m_pApp->m_bPagePrintInProgress)
	{
		wxLogDebug(_T("CStrip::Draw() strip %d, rect (logical coords) x %d  y %d, width %d height %d NOT FROM LINUX"),
			this->m_nStrip, Left(), Top(), Width(), Height());
	}
#endif
//	wxLogDebug(_T("%s:%s():line %d, m_bFreeTranslationMode = %s"), __FILE__, __FUNCTION__, __LINE__,
//		(&wxGetApp())->m_bFreeTranslationMode ? _T("TRUE") : _T("FALSE"));

	int i;
	int nPileCount = m_arrPiles.GetCount();
	CPile* aPilePtr = NULL;
	for (i = 0; i < nPileCount; i++)
	{
		aPilePtr = ((CPile*)m_arrPiles[i]);
		aPilePtr->Draw(pDC);
	}

	//wxLogDebug(_T("%s:%s():line %d, Drawing strip = %d  gbShowTargetOnly = %d @@@@@@@@@@@@@@@@@"), __FILE__, __FUNCTION__, __LINE__,
	//	m_nStrip, gbShowTargetOnly); // <- Yes, the strips get drawn for gbShowTargetOnly set to TRUE, see if piles are same

//	wxLogDebug(_T("%s:%s():line %d, m_bFreeTranslationMode = %s"), __FILE__, __FUNCTION__, __LINE__,
//		(&wxGetApp())->m_bFreeTranslationMode ? _T("TRUE") : _T("FALSE"));
}

// refactored CreateStrip() returns the iterator set for the CPile instance which is to be
// first when CreateStrip() is again called in order to build the next strip, or NULL if
// the document end has been reached (passing in the iterator avoids having to have a
// PileList::Item() call at the start of the function, so time is saved when setting up the
// strips for a whole document)
// BEW 22Feb10 no changes needed for support of doc version 5
// BEW 19Aug18 refactored for supporting dropdown list phrasebox
// whm 11Nov2022 refactored for new phrasebox sizing methodology and simplifications.
PileList::Node* CStrip::CreateStrip(PileList::Node*& pos, int nStripWidth, int gap)
{
	m_nFree = nStripWidth;

	CPile* pPile = NULL;
	int pileWidth = 0;
	int nCurrentSpan = 0;

    // clear the two arrays - failure to do this leaves garbage in their members (such as
    // m_size a huge number & m_count a huge number) & so get app crash
	m_arrPiles.Clear();
	m_arrPileOffsets.Clear();

	// lay out the piles

    // Note: RTL layout and LRT layout of the piles in the strip uses exactly the same code,
	// they are appended to the strip in logical order, and it is only the rectangle 
	// calculations (none of which are done here) which specify the differing locations 
	// of pile[0], pile[1] etc for RTL versus LTR layout - so that information will be 
	// in the coordinate calculations - specifically, in CPile::Left()
	
	// Unicode version, and Left to Right layout is wanted
	int nHorzOffset_FromLeft = 0;
	int pileIndex_InStrip = 0; // index into CStrip's m_arrPiles array of void*
	int nWidthOfPreviousPile = 0;

    // we must always have at least one pile in the strip in order to prevent an
    // infinite loop of empty strips if the width of the first pile should happen to
    // exceed the strip's width; so we place the first unilaterally, regardless of its
    // width
	pPile = (CPile*)pos->GetData();
	// set the pile's m_pOwningStrip member
	pPile->m_pOwningStrip = this;
	if (m_pLayout->m_pApp->m_nActiveSequNum == -1 || m_pLayout->m_bLayoutWithoutVisiblePhraseBox)
	{
		pileWidth = pPile->m_nMinWidth; // no "wide gap" for phrase box, as it is hidden
	}
	else 
	{ 
		// Is this first pile to be placed in the strip, the doc's current active one?
		if (pPile->m_pSrcPhrase->m_nSequNumber == m_pLayout->m_pApp->m_nActiveSequNum)
		{
			// whm 11Nov2022 simplification. I removed the tests for gbDoingInitialSetup and 
			// m_nNewPhraseBoxGapWidth here in CreateStrip(). They proved redundant and only 
			// added complexity. The m_nMewPhraseBoxGapWidth was removed entirely from the app.
			// 
			// Get the largest width of curBoxWidth or curListWidth, and save them on 
			// the Layout's m_curBoxWidth and m_curListWidth.
			// Note: When there needed to be a phrasebox resize (larger or smaller) the
			// m_pLayout->m_curBoxWidth and m_curListWidth were adjusted to the new width
			// in the CPhraseBox::OnPhraseBoxChanged() function.
			int curBoxWidth = m_pLayout->m_curBoxWidth;
			int curListWidth = m_pLayout->m_curListWidth;
			// Take the larger of the two
			int theMax = wxMax(curBoxWidth, curListWidth);
			// Make both agree
			m_pLayout->m_curBoxWidth = theMax;
			m_pLayout->m_curListWidth = theMax;

			// The pileWidth at the active location is simply the current box width plus
			// the width of the dropdown button.
			pileWidth = m_pLayout->m_curBoxWidth + m_pLayout->GetExtraWidthForButton();
		}
		else
		{
			pileWidth = pPile->m_nMinWidth;
		}
	}
	m_arrPiles.Add(pPile); // store it
	m_arrPileOffsets.Add(nHorzOffset_FromLeft); // store offset to left boundary
	m_nFree -= pileWidth; // reduce the free space accordingly
	pPile->m_nPile = pileIndex_InStrip; // store its index within strip's m_arrPiles array

	// prepare for next iteration
	nWidthOfPreviousPile = pileWidth;
	pileIndex_InStrip++;
	pos = pos->GetNext(); // will be NULL if the pile just created was at doc end
	nHorzOffset_FromLeft = nWidthOfPreviousPile + gap;
	// if m_nFree went negative or zero, we can't fit any more piles, so declare
	// the strip full
	if (m_nFree <= 0)
	{
		m_bValid = TRUE;
		return pos;
	}

	// append second and later piles to the strip
	while (pos != NULL  && m_nFree > 0)
	{
		pPile = (CPile*)pos->GetData();
		wxASSERT(pPile != NULL);

		// break out of the loop without including this pile in the strip if it is a
		// non-initial pile which contains a marker in its pointed at pSrcPhrase which is a
		// wrap marker for text (we want strips to wrap too, provided the view menu item has
		// that feature turned on)
		if (m_pLayout->m_pApp->m_bMarkerWrapsStrip && pileIndex_InStrip > 0)
		{
			bool bCausesWrap = pPile->IsWrapPile();
			// do the wrap only if it is a wrap marker and we have not just deal with
			// a document-initial marker, such as \id
			if (bCausesWrap && !(m_nStrip == 0 && pileIndex_InStrip == 1))
			{
				// if we need to wrap, discontinue assigning piles to this strip (the
				// nPileIndex_InList value is already correct for returning to caller)
				m_bValid = TRUE;
				return pos;
			}
		}

		// if control gets to here, the pile is a potential candidate for inclusion in this
		// strip, so work out if it will fit - and if it does, add it to the m_arrPiles, etc
		if (m_pLayout->m_pApp->m_nActiveSequNum == -1 || m_pLayout->m_bLayoutWithoutVisiblePhraseBox)
		{
			pileWidth = pPile->m_nMinWidth; // no "wide gap" for phrase box, as it is hidden
		}
		else 
		{	
			if (pPile->m_pSrcPhrase->m_nSequNumber == m_pLayout->m_pApp->m_nActiveSequNum)
			{
				// whm 11Nov2022 simplification. I removed the tests for gbDoingInitialSetup and 
				// m_nNewPhraseBoxGapWidth here in CreateStrip(). They proved redundant and only 
				// added complexity. The m_nMewPhraseBoxGapWidth was removed entirely from the app.
				// 
				// Get the largest width of curBoxWidth or curListWidth, and save them on 
				// the Layout's m_curBoxWidth and m_curListWidth.
				// Note: When there needed to be a phrasebox resize (larger or smaller) the
				// m_pLayout->m_curBoxWidth and m_curListWidth were adjusted to the new width
				// in the CPhraseBox::OnPhraseBoxChanged() function.
				int curBoxWidth = m_pLayout->m_curBoxWidth;
				int curListWidth = m_pLayout->m_curListWidth;
				// Take the larger of the two
				int theMax = wxMax(curBoxWidth, curListWidth);
				// Make both agree
				m_pLayout->m_curBoxWidth = theMax;
				m_pLayout->m_curListWidth = theMax;

				// The pileWidth at the active location is simply the current box width plus
				// the width of the dropdown button.
				pileWidth = m_pLayout->m_curBoxWidth + m_pLayout->GetExtraWidthForButton();
			}
			else
			{
				// This pile is NOT the active pile, so we assign the pileWidth to the m_nMinWidth 
				// value and add the gap for nCurrentSpan
				pileWidth = pPile->m_nMinWidth;
			}
		}
		// whm 11Nov2022 note: Addition of + gap below is required for piles to fit within 
		// m_nFree span of strip.
		nCurrentSpan = pileWidth + gap; // this much has to fit in the m_nFree space for this
										// pile to be eligible for inclusion in the strip
		if (nCurrentSpan <= m_nFree)
		{
			// this pile will fit in the strip, so add it
			m_arrPiles.Add(pPile); // store it
			m_arrPileOffsets.Add(nHorzOffset_FromLeft); // store offset to left boundary
			m_nFree -= nCurrentSpan; // reduce the free space accordingly
			pPile->m_nPile = pileIndex_InStrip; // store its index within strip's m_arrPiles array

//#if defined (_DEBUG)
//				{
//					int stripIndex = this->GetStripIndex();
//					wxLogDebug(_T("    %s::%s(), line %d: CREATING STRIP %d , nWidthOfPreviousPile %d , pileIndex_InStrip %d , nHorzOffset_FromLeft (includes + interPile gap) %d\n"),
//						__FILE__, __FUNCTION__, __LINE__, stripIndex, nWidthOfPreviousPile, pileIndex_InStrip, nHorzOffset_FromLeft);
//				}
//#endif

		}
		else
		{
			// this pile won't fit, so the strip is full - declare it full and return
			// the pile list's index for use in the next strip's creation
			m_bValid = TRUE;
			// This is the normal exit point for control, when creating strips
			return pos;
		}

		// set the pile's m_pOwningStrip member
		pPile->m_pOwningStrip = this;

		// update index for next iteration
		pileIndex_InStrip++;
		nWidthOfPreviousPile = pileWidth;

		// advance the iterator for the CLayout's m_pileList list of pile pointers
		pos = pos->GetNext(); // will be NULL if the pile just created was at doc end

		// set the nHorzOffset_FromLeft value ready for the next iteration of the loop
		nHorzOffset_FromLeft += nWidthOfPreviousPile + gap;
	}
	
	// If the loop exits because the while test yields FALSE, then either we are at the 
	// end of the document or the first pile was wider than the whole strip - in either 
	// case we must declare this strip filled and we are done.
	m_bValid = TRUE;

	return pos; // the iterator value where we start when we create the next strip
}

// the following is an overrided version to be used when refilling emptied strips, for a
// range of piles - rather than all the piles to the document's end, and so it has an
// ending index value passed in for the last pile pointer to be placed in a strip.
// The value returned is a count of how many CPile pointers were placed in the current
// strip, there will always be at least one - empty strips are illegal; the function
// should not be called if there are no more piles to be placed.
// BEW 22Feb10 no changes needed for support of doc version 5
// BEW 19Aug18 refactored for supporting dropdown list phrasebox
// whm 11Nov2022 refactored for new phrasebox sizing methodology and simplifications,
// and to make this override function work like its parent CreateStrip() function.
int CStrip::CreateStrip(int nInitialPileIndex, int nEndPileIndex, int nStripWidth, int gap)
{
	m_nFree = nStripWidth;

	CPile* pPile = NULL;
	int pileWidth = 0;
	int nCurrentSpan = 0;

	// establish an iterator for the pPiles list, initialize to the start of the range
	// and a count of how many CPile pointer instances are to be placed; and a counter for
	// how many get placed in the current strip - this value we return when the strip is
	// full or we have run out of piles to be placed
	PileList::Node* pos = m_pLayout->m_pileList.Item(nInitialPileIndex);
	wxASSERT(pos);
	int numPlaced = 0;
	int pileIndex = nInitialPileIndex;

	// clear the two arrays - failure to do this leaves gargage in their members (such as
    // m_size & m_count being left as huge numbers) & could lead to an app crash
	m_arrPiles.Clear();
	m_arrPileOffsets.Clear();

	// lay out the piles...
    // Note: RTL layout and LRT layout of the piles in the strip uses exactly the same code

	int nHorzOffset_FromLeft = 0;
	int pileIndex_InStrip = 0; // index into CStrip's m_arrPiles array of void*
	int nWidthOfPreviousPile = 0;

    // we must always have at least one pile in the strip in order to prevent an
    // infinite loop of empty strips if the width of the first pile should happen to
    // exceed the strip's width; so we place the first unilaterally, regardless of its
    // width
	pPile = (CPile*)pos->GetData();
	wxASSERT(pPile);
	// set the pile's m_pOwningStrip member
	pPile->m_pOwningStrip = this;
	if (m_pLayout->m_pApp->m_nActiveSequNum == -1 || m_pLayout->m_bLayoutWithoutVisiblePhraseBox)
	{
		pileWidth = pPile->m_nMinWidth; // no "wide gap" for phrase box, as it is hidden
	}
	else
	{
		if (pPile->m_pSrcPhrase->m_nSequNumber == m_pLayout->m_pApp->m_nActiveSequNum)
		{
			// whm 11Nov2022 simplification. I removed the tests for gbDoingInitialSetup and 
			// m_nNewPhraseBoxGapWidth here in CreateStrip(). They proved redundant and only 
			// added complexity. The m_nMewPhraseBoxGapWidth was removed entirely from the app.
			// 
			// Get the largest width of curBoxWidth or curListWidth, and save them on 
			// the Layout's m_curBoxWidth and m_curListWidth.
			// Note: When there needed to be a phrasebox resize (larger or smaller) the
			// m_pLayout->m_curBoxWidth and m_curListWidth were adjusted to the new width
			// in the CPhraseBox::OnPhraseBoxChanged() function.
			int curBoxWidth = m_pLayout->m_curBoxWidth;
			int curListWidth = m_pLayout->m_curListWidth;
			// Take the larger of the two
			int theMax = wxMax(curBoxWidth, curListWidth);
			// Make both agree
			m_pLayout->m_curBoxWidth = theMax;
			m_pLayout->m_curListWidth = theMax;

			// The pileWidth at the active location is simply the current box width plus
			// the width of the dropdown button.
			pileWidth = m_pLayout->m_curBoxWidth + m_pLayout->GetExtraWidthForButton();
		}
		else
		{
			pileWidth = pPile->m_nMinWidth;
		}
	}
	m_arrPiles.Add(pPile); // store it
	m_arrPileOffsets.Add(nHorzOffset_FromLeft); // store offset to left boundary
	numPlaced++; // increment counter of how many placed
	pileIndex++; // set tracker index to index of next pile to be placed
	m_nFree -= pileWidth; // reduce the free space accordingly
	pPile->m_nPile = pileIndex_InStrip; // store its index within strip's m_arrPiles array
	if (pileIndex > nEndPileIndex)
	{
		// we've just placed the last pile to be placed, so return numPlaced and set m_bValid
		// to TRUE, and let the caller work out if it should instead by set to FALSE
		m_bValid = TRUE;
		return numPlaced;
	}

	// prepare for next iteration
	nWidthOfPreviousPile = pileWidth;
	pileIndex_InStrip++;
	pos = pos->GetNext(); // will be NULL if the pile just placed was at doc end
	if (pos == NULL)
	{
		// no more piles available for placement, so return
		m_bValid = TRUE;
		return numPlaced;
	}
	nHorzOffset_FromLeft = nWidthOfPreviousPile + gap;

	// if m_nFree went negative or zero, we can't fit any more piles, so declare
	// the strip full
	if (m_nFree <= 0)
	{
		m_bValid = TRUE;
		return numPlaced;
	}

	// append second and later piles to this strip
	while (pos != NULL  && m_nFree > 0 && pileIndex <= nEndPileIndex)
	{
		pPile = (CPile*)pos->GetData();
		wxASSERT(pPile != NULL);

		// break out of the loop without including this pile in the strip if it is a
		// non-initial pile which contains a marker in its pointed at pSrcPhrase which is a
		// wrap marker for text (we want strips to wrap too, provided the view menu item has
		// that feature turned on)
		if (m_pLayout->m_pApp->m_bMarkerWrapsStrip && pileIndex_InStrip > 0)
		{
			bool bCausesWrap = pPile->IsWrapPile();
			// do the wrap only if it is a wrap marker and we have not just deal with
			// a document-initial marker, such as \id
			if (bCausesWrap && !(m_nStrip == 0 && pileIndex_InStrip == 1))
			{
				// if we need to wrap, discontinue assigning piles to this strip (the
				// nPileIndex_InList value is already correct for returning to caller)
				m_bValid = TRUE;
				return numPlaced;
			}
		}

		// if control gets to here, the pile is a potential candidate for inclusion in this
		// strip, so work out if it will fit - and if it does, add it to the m_arrPiles, etc
		if (m_pLayout->m_pApp->m_nActiveSequNum == -1)
		{
			pileWidth = pPile->m_nMinWidth; // no "wide gap" for phrase box, as it is hidden
		}
		else
		{
			if (pPile->m_pSrcPhrase->m_nSequNumber == m_pLayout->m_pApp->m_nActiveSequNum)
			{
				// whm 11Nov2022 modification. This override function of CreateStrip() should have the same
				// coding as its parent function CreateStrip() as far as the mechanism for ensuring that
				// any new gap width is taken into account in the refactored code. Hence, instead of just
				// assigning the value returned from Pile's CalcPhraseBoxGapWidth() here to pileWidth, I'm
				// makine the coding here conform to that in the parent function CreateStrip().
				// pileWidth = pPile->CalcPhraseBoxGapWidth(); 
				// 
				// whm 11Nov2022 simplification. I removed the tests for gbDoingInitialSetup and 
				// m_nNewPhraseBoxGapWidth here in CreateStrip(). They proved redundant and only 
				// added complexity. The m_nMewPhraseBoxGapWidth was removed entirely from the app.
				// 
				// Get the largest width of curBoxWidth or curListWidth, and save them on 
				// the Layout's m_curBoxWidth and m_curListWidth.
				// Note: When there needed to be a phrasebox resize (larger or smaller) the
				// m_pLayout->m_curBoxWidth and m_curListWidth were adjusted to the new width
				// in the CPhraseBox::OnPhraseBoxChanged() function.
				int curBoxWidth = m_pLayout->m_curBoxWidth;
				int curListWidth = m_pLayout->m_curListWidth;
				// Take the larger of the two
				int theMax = wxMax(curBoxWidth, curListWidth);
				// Make both agree
				m_pLayout->m_curBoxWidth = theMax;
				m_pLayout->m_curListWidth = theMax;

				// The pileWidth at the active location is simply the current box width plus
				// the width of the dropdown button.
				pileWidth = m_pLayout->m_curBoxWidth + m_pLayout->GetExtraWidthForButton();
			}
			else
			{
				// This pile is NOT the active pile, so we assign the pileWidth to the m_nMinWidth 
				// value and add the gap for nCurrentSpan
				pileWidth = pPile->m_nMinWidth;
			}
		}
		// whm 11Nov2022 note: Addition of + gap below is required for piles to fit 
		// within m_nFree span of strip.
		nCurrentSpan = pileWidth + gap; // this much has to fit in the m_nFree space
							// for this pile to be eligible for inclusion in the strip	
		if (nCurrentSpan <= m_nFree)
		{
			// this pile will fit in the strip, so add it
			m_arrPiles.Add(pPile); // store it
			m_arrPileOffsets.Add(nHorzOffset_FromLeft); // store offset to left boundary
			numPlaced++; // increment counter of how many placed
			pileIndex++; // set tracker index to index of next pile to be placed
			m_nFree -= nCurrentSpan; // reduce the free space accordingly
			pPile->m_nPile = pileIndex_InStrip; // store its index within strip's
												// m_arrPiles array
			// set the pile's m_pOwningStrip member
			pPile->m_pOwningStrip = this;

		}
		else
		{
			// this pile won't fit, so the strip is full - declare it full and return
			// the pile list's index for use in the next strip's creation
			m_bValid = TRUE;
			return numPlaced;
		}

		// have we just dealt with the last one to be placed? (pileIndex has been
		// augmented to the next pile's index, so if we placed the last one, pileIndex
		// will have a value larger than nEndPileIndex at this point
		if (pileIndex > nEndPileIndex)
		{
			// we've just placed the last pile to be placed, so return numPlaced and set m_bValid
			// to TRUE, and let the caller work out if it should instead by set to FALSE
			m_bValid = TRUE;
			return numPlaced;
		}
		else
		{
			// there is at least one more to be placed, so prepare for next iteration...
			pileIndex_InStrip++;
			nWidthOfPreviousPile = pileWidth;

			// advance the iterator for the CLayout's m_pileList list of pile pointers
			pos = pos->GetNext(); // will be NULL if the pile just created was at doc end
			// set the nHorzOffset_FromLeft value ready for the next iteration of the loop
			nHorzOffset_FromLeft += nWidthOfPreviousPile + gap;
		}
	}

    // if the loop exits because the while test yields FALSE, then 1. either we are at the end
    // of the document, or 2. the first pile was wider than the whole strip, or 3. we have
    // reached and processed the pile at nEndPileIndex - we will declare this strip filled and
    // let the caller work out if m_bValid should instead be set to FALSE and it can then do so
	// (actually, the internal test for pileIndex >= nEndPileIndex should pick up reasons 1. and
	// 2. so we expect to get to the next line only when a pile covers or exceeds the whole
	// width of a strip)
	m_bValid = TRUE;

	return numPlaced;
}

// return the width of the strip (it's width is the maximum width it can be for the current
// client window size, less the left margin and right hand slop; the document size's .x
// value is already set to that value, so just return that; the width is NOT the width of
// any piles in the strip -- for that, use the Width() value and subtract m_nFree pixels
// from it)
int CStrip::Width()
{
	return (m_pLayout->GetLogicalDocSize()).GetWidth();
}

int CStrip::Height()
{
	return m_pLayout->GetStripHeight();
}

int CStrip::Left()
{
	return m_pLayout->GetStripLeft();
}

int CStrip::Top()
{
	return m_pLayout->GetCurLeading() + m_nStrip * (Height()
				+ m_pLayout->GetCurLeading());
}

int CStrip::GetFree()
{
	return m_nFree;
}

void CStrip::SetFree(int nFree)
{
	m_nFree = nFree;
}

// set the value of the validity flag
void CStrip::SetValidityFlag(bool bValid)
{
	m_bValid = bValid;
}

// get the validity flag's value
bool CStrip::GetValidityFlag()
{
	return m_bValid;
}

// use GetStripRect_CellsOnly() to set a local strip rectangle which bounds the enclosed
// cells only, that is, omitting the free translation rectangle if gbFreeTranslationMode is
// TRUE; because for clicks on the layout, we never do anything for a click on the free
// translation area, but only for clicks on cells (and even then we ignore clicks on cells
// with index == 2); so we set the height value using GetPileHeight() rather than
// GetStripHeight()
// Note: CStrip does not have any wxRect member; use this function to generate a local
// wxRect whenever one is needed for a strip - such as when testing for a mousedown within
// the strip
void CStrip::GetStripRect_CellsOnly(wxRect& rect)
{
	rect.SetTop(Top());
	rect.SetLeft(Left());
	rect.SetWidth(Width());
	rect.SetHeight(m_pLayout->GetPileHeight());
}

// this overloaded version is more useful, it can be placed in line with other accessors
wxRect CStrip::GetStripRect_CellsOnly()
{
	wxRect rect;
	GetStripRect_CellsOnly(rect);
	return rect;
}

// this rectangle gives the same topLeft, width and height as in the legacy (ie.
// pre-refactoring) version -- we need this to avoid having to tweak the scroll calculations
wxRect CStrip::GetStripRect()
{
	wxRect rect;
	GetStripRect_CellsOnly(rect);
	rect.SetHeight(rect.GetHeight() + 3 + m_pLayout->m_nTgtHeight);
	return rect;
}

void CStrip::GetFreeTransRect(wxRect& rect)
{
	GetStripRect_CellsOnly(rect);
	// reset the Top to be 3 pixels below the bottom of the piles of the strip
	rect.SetTop(rect.GetBottom() + 3);
	// reset the Bottom to be the target text's height lower than the top
	rect.SetBottom(rect.GetTop() + m_pLayout->GetTgtTextHeight());
	// increase the width to be 80% of the RH_SLOP (which is 60) value = 48 pixels more
	rect.SetWidth(rect.GetWidth() + (RH_SLOP * 4) / 5);
}

wxRect CStrip::GetFreeTransRect()
{
	wxRect rect;
	GetFreeTransRect(rect);
	return rect;
}

int CStrip::GetStripIndex()
{
	return m_nStrip;
}
