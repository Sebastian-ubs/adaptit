/////////////////////////////////////////////////////////////////////////////
/// \project		adaptit
/// \file			Adapt_It_Resources.cpp
/// \author			Bill Martin
/// \date_created	21 January 2019
/// \rcs_id $Id$
/// \copyright		2019 Bruce Waters, Bill Martin, SIL International
/// \license		The Common Public License or The GNU Lesser General Public License (see license directory)
/// \description	This is the header file for Adapt It's resources and resource functions 
/// used by Adapt It. These resources were created initially by wxDesigner. The wxDesigner
/// generated C++ code in Adapt_It_wdr.h was copied to this header file, and is now maintained
/// here for building into the Adapt It application rather than from wxDesigner and the old 
/// Adapt_It_wdr.h file. The Adapt_It_wdr.h and Adapt_It_wdr.cpp files are no longer a part of 
/// the Adapt It project. Changes and additions made within wxDesigner can continue to be 
/// exported to C++ into the Adapt_It_wdr.h and Adapt_It_wdr.cpp files, but those changes and
/// additions must be hand copied from Adapt_It_wdr.h over to this Adapt_It_Resources.h file 
/// where they can be tweaked apart from wxDesigner, and thereby remove problem/obsolete code that 
/// wxDesigner was generating. 
/// With the advent of wxWidgets version 3.1.x and later, the old wxDesigner generated code was
/// producing code that would cause run-time asserts mostly due to improper combinations of wxSizer
/// wxALIGNMENT... style flags. Also wxDesigner's generated code was also increasingly resulting in 
/// deprecated code warnings, such as those related to its use of its outdated wxFont constants, 
/// where it was using wxSWISS, wxNORMAL, wxBOLD, instead of the newer constants wxFONTFAMILY_SWISS, 
/// wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD. 
///
/// NOTE: You might find it helpful to use a file comparison utility like WinMerge (Windows)
/// or Meld (Linux) to see where the differences are, and any new additions between the
/// wxDesigner generated Adapt_It_wdr.* files and the newer Adapt_It_Resources.* files.
/// 
/////////////////////////////////////////////////////////////////////////////
/// IMPORTANT: For any new or revised resource function that you do in wxDesigner
/// you must export it to C++ code which is saved in the old Adapt_It_wdr.h and
/// Adapt_It_wdr.cpp files. Then, make sure you hand copy the extern const int symbol 
/// declarations from the Adapt_It_wdr.h header file to an appropriate place into 
/// this Adapt_It_Resources.h file, and the C++ implementation code for the given
/// new/revised function from the old Adapt_It_wdr.cpp file into the Adapt_It_Resources.cpp
/// file. If you fail to copy the code from the old wxDesigner files into the new
/// resource files (Adapt_It_Resources.*), your coding changes made within 
/// wxDesigner will not be recognized when compiled - as of 21 January 2019, only the 
/// Adapt_It_Resources.h and Adapt_It_Resources.cpp files are now included within
/// the Adapt It project.
/////////////////////////////////////////////////////////////////////////////
/// IMPORTANT: If/When wxDesigner is used in the future, ensure that its
/// "Write C++ source" dialog has the third radio button option:
/// (.) int ID_TEXT; ID_TEXT = wxNewId(); selected
/// so that it doesn't create IDs in succession from a certain starting point,
/// since to do so will make it difficult to copy new wxDesigner function code
/// from wxDesigner's Adapt_It_wrd.h and Adapt_It_wrd.cpp file to the new
/// Adapt_It_Resources.h and Adapt_It_Resources.cpp files.
/// Previously all ID numbers were generated automatically within wxDesigner,
/// whereas they should from this point forward be simply generated on the fly
/// by the use of wxNewId() statements in the resource code.
///
/// [see Adapt_It_Resources.cpp for list of functions that require manual modifications]
///
/////////////////////////////////////////////////////////////////////////////

#ifndef __Adapt_It_Resources_H__
#define __Adapt_It_Resources_H__

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma interface "Adapt_It_Resources.h"
#endif

// Include wxWidgets' headers

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/image.h>
#include <wx/statline.h>
#include <wx/spinbutt.h>
#include <wx/spinctrl.h>
#include <wx/splitter.h>
#include <wx/listctrl.h>
#include <wx/treectrl.h>
#include <wx/notebook.h>
#include <wx/grid.h>
#include <wx/toolbar.h>
#include <wx/tglbtn.h>
#include <wx/collpane.h>
#include <wx/hyperlink.h>
#include <wx/animate.h>
#include <wx/filepicker.h>
#include <wx/datectrl.h>
#include <wx/fontpicker.h>
#include <wx/clrpicker.h>
#include <wx/srchctrl.h>
#if wxCHECK_VERSION(2,9,0)
#include <wx/editlbox.h>
#endif

// Declare window functions

extern wxSizer *ID_ABOUT_TOP_AREA;
extern const int ID_ABOUT_AI_BMP;
extern const int ID_TEXT;
extern const int ID_ABOUT_VERSION_LABEL;
extern const int ID_ABOUT_VERSION_NUM;
extern const int ID_ABOUT_VERSION_DATE;
extern const int ID_STATIC_UNICODE_OR_ANSI;
extern const int ID_STATIC_WX_VERSION_USED;
extern const int ID_LINE;
extern const int ID_STATIC_UI_LANGUAGE;
extern const int ID_STATIC_HOST_OS;
extern const int ID_STATIC_SYS_LANGUAGE;
extern const int ID_STATIC_SYS_LOCALE_NAME;
extern const int ID_STATIC_CANONICAL_LOCALE_NAME;
extern const int ID_STATIC_SYS_ENCODING_NAME;
extern const int ID_STATIC_SYSTEM_LAYOUT_DIR;
wxSizer *AboutDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern wxSizer *ID_CONTROLBAR_1_LINE_SIZER;
extern const int IDC_RADIO_DRAFTING;
extern const int IDC_RADIO_REVIEWING;
extern const int IDC_CHECK_SINGLE_STEP;
extern const int IDC_CHECK_KB_SAVE;
extern const int IDC_CHECK_FORCE_ASK;
extern const int IDC_BUTTON_NO_ADAPT;
extern const int IDC_STATIC;
extern const int IDC_EDIT_DELAY;
extern const int IDC_CHECK_ISGLOSSING;
wxSizer *ControlBarFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_EDIT_COMPOSE;
extern wxSizer *ID_BSH_COMPOSE_LH_BUTTONS;
extern const int IDC_BUTTON_SHORTEN;
extern const int IDC_BUTTON_PREV;
extern const int IDC_BUTTON_LENGTHEN;
extern const int IDC_BUTTON_NEXT;
extern const int IDC_BUTTON_REMOVE;
extern const int IDC_BUTTON_APPLY;
extern const int ID_BUTTON_ADJUST;
extern const int ID_BUTTON_JOIN_TO_NEXT;
extern wxSizer *ID_BSV_SECTIONS;
extern const int IDC_STATIC_SECTION_DEF;
extern const int IDC_RADIO_PUNCT_SECTION;
extern const int IDC_RADIO_VERSE_SECTION;
extern wxSizer *ID_BSH_COMPOSE_RH_BUTTONS;
extern const int IDC_BUTTON_CLEAR;
extern const int IDC_BUTTON_SELECT_ALL;
wxSizer *ComposeBarFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_LISTBOX_ADAPTIONS;
wxSizer *OpenExistingProjectDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_EDIT_FILENAME;
extern const int ID_TEXT_INVALID_CHARACTERS;
wxSizer *GetOutputFilenameDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_SOURCE_LANGUAGE;
extern const int ID_EDIT_SOURCE_LANG_CODE;
extern const int IDC_TARGET_LANGUAGE;
extern const int ID_EDIT_TARGET_LANG_CODE;
extern const int ID_BUTTON_LOOKUP_CODES;
wxSizer *LanguagesPageFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_SOURCE_FONTNAME;
extern const int IDC_CHECK_SRC_RTL;
extern const int IDC_SOURCE_SIZE;
extern const int IDC_SOURCE_LANG;
extern const int IDC_BUTTON_SOURCE_COLOR;
extern const int ID_BUTTON_CHANGE_SRC_ENCODING;
extern const int IDC_TARGET_FONTNAME;
extern const int IDC_CHECK_TGT_RTL;
extern const int IDC_TARGET_SIZE;
extern const int IDC_TARGET_LANG;
extern const int IDC_BUTTON_TARGET_COLOR;
extern const int ID_BUTTON_CHANGE_TGT_ENCODING;
extern const int IDC_NAVTEXT_FONTNAME;
extern const int IDC_CHECK_NAVTEXT_RTL;
extern const int IDC_NAVTEXT_SIZE;
extern const int IDC_CHANGE_NAV_TEXT;
extern const int IDC_BUTTON_NAV_TEXT_COLOR;
extern const int ID_BUTTON_CHANGE_NAV_ENCODING;
extern const int ID_TEXTCTRL_AS_STATIC_FONTPAGE;
extern const int IDC_BUTTON_SPECTEXTCOLOR;
extern const int IDC_RETRANSLATION_BUTTON;
extern const int ID_BUTTON_TEXT_DIFFS;
wxSizer *FontsPageFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern wxSizer *myboxsizer;
extern const int IDC_PLEASE_WAIT;
wxSizer *WaitDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_LIST_ACCEPTED;
extern const int IDC_BUTTON_REJECT;
extern const int IDC_BUTTON_ACCEPT;
extern const int ID_BUTTON_REJECT_ALL_FILES;
extern const int ID_BUTTON_ACCEPT_ALL_FILES;
extern const int IDC_LIST_REJECTED;
wxSizer *WhichFilesDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXTCTRL_TRANSFORM_TO_GLOSSES1;
extern const int ID_TEXTCTRL_TRANSFORM_TO_GLOSSES2;
extern const int ID_TEXTCTRL_TRANSFORM_TO_GLOSSES3;
wxSizer *TransformToGlossesDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_CHECK_SOURCE_USES_CAPS;
extern const int ID_CHECK_USE_AUTO_CAPS;
extern const int ID_TEXTCTRL_AS_CASE_PAGE_STATIC_TEXT;
extern const int ID_TEXT_SL;
extern const int ID_TEXT_TL;
extern const int ID_TEXT_GL;
extern const int IDC_EDIT_SRC_CASE_EQUIVALENCES;
extern const int IDC_EDIT_TGT_CASE_EQUIVALENCES;
extern const int IDC_EDIT_GLOSS_CASE_EQUIVALENCES;
extern const int IDC_BUTTON_CLEAR_SRC_LIST;
extern const int IDC_BUTTON_CLEAR_TGT_LIST;
extern const int IDC_BUTTON_CLEAR_GLOSS_LIST;
extern const int IDC_BUTTON_SRC_SET_ENGLISH;
extern const int IDC_BUTTON_TGT_SET_ENGLISH;
extern const int IDC_BUTTON_GLOSS_SET_ENGLISH;
extern const int IDC_BUTTON_SRC_COPY_TO_NEXT;
extern const int IDC_BUTTON_TGT_COPY_TO_NEXT;
extern const int IDC_BUTTON_GLOSS_COPY_TO_NEXT;
extern const int IDC_BUTTON_SRC_COPY_TO_GLOSS;
extern const int ID_TEXTCTRL_TRIGGER_MSG;
extern const int ID_TEXTCTRL_CASE_TRIGGERS;
wxSizer *CaseEquivDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_EDIT_WELCOME_TO;
extern const int IDC_EDIT_ADAPT_IT;
extern const int ID_TEXTCTRL_AS_STATIC_WELCOME;
extern const int IDC_CHECK_NOLONGER_SHOW;
wxSizer *WelcomeDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXTCTRL_AS_STATIC_DOCPAGE;
extern const int IDC_STATIC_WHICH_MODE;
extern const int IDC_BUTTON_WHAT_IS_DOC;
extern const int IDC_STATIC_WHICH_FOLDER;
extern const int IDC_LIST_NEWDOC_AND_EXISTINGDOC;
extern const int IDC_CHECK_CHANGE_FIXED_SPACES_TO_REGULAR_SPACES;
extern const int IDC_CHANGE_FOLDER_BTN;
extern const int IDC_CHECK_FORCE_UTF8;
wxSizer *DocPageFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXTCTRL_AS_STATIC_PROJECTPAGE;
extern const int IDC_BUTTON_WHAT_IS_PROJECT;
extern const int IDC_LIST_NEW_AND_EXISTING;
wxSizer *ProjectPageFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_EDIT_SRC;
extern const int IDC_LIST_PUNCTS;
extern const int ID_TEXTCTRL_AS_STATIC_PLACE_INT_PUNCT;
extern const int IDC_EDIT_TGT;
extern const int IDC_BUTTON_PLACE;
wxSizer *PlaceInternalPunctDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_RADIO_INCHES;
extern const int IDC_RADIO_CM;
wxSizer *UnitsDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_STATIC_SRC;
extern const int IDC_EDIT_PRECONTEXT;
extern const int IDC_EDIT_SOURCE_TEXT;
extern const int IDC_EDIT_RETRANSLATION;
extern const int IDC_COPY_RETRANSLATION_TO_CLIPBOARD;
extern const int IDC_BUTTON_TOGGLE;
extern const int IDC_STATIC_TGT;
extern const int IDC_EDIT_FOLLCONTEXT;
wxSizer *RetranslationDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_EDIT_SRC_TEXT;
extern const int IDC_EDIT_TGT_TEXT;
extern const int IDC_SPIN_CHAPTER;
extern const int IDC_SPIN_VERSE;
extern const int IDC_GET_CHVERSE_TEXT;
extern const int IDC_CLOSE_AND_JUMP;
extern const int IDC_SHOW_MORE;
extern const int IDC_SHOW_LESS;
extern const int IDC_STATIC_BEGIN;
extern const int IDC_STATIC_END;
wxSizer *EarlierTransDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_CHECK_KB_BACKUP;
extern const int IDC_CHECK_BAKUP_DOC;
extern const int ID_TEXTCTRL_AS_STATIC_BACKUPS_AND_KB_PAGE;
extern const int IDC_EDIT_SRC_NAME;
extern const int IDC_EDIT_TGT_NAME;
extern const int IDC_EDIT_GLS_NAME;
extern const int ID_EDIT_GLOSS_LANG_CODE;
extern const int ID_EDIT_FRTR_NAME;
extern const int ID_EDIT_FRTR_LANG_CODE;
extern const int IDC_RADIO_ADAPT_BEFORE_GLOSS;
extern const int IDC_RADIO_GLOSS_BEFORE_ADAPT;
extern const int IDC_RADIO_COPY_SRC_WORD_DELIM;
extern const int IDC_RADIO_USE_ONLY_LATIN_SPACE;
extern const int ID_CHECKBOX_NO_FOOTNOTES_IN_COLLAB;
extern const int ID_CHECKBOX_TURN_ON_CONFRES;
wxSizer *BackupsAndKBPageFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int STATIC_TEXT_V4;
extern const int IDC_EDIT_LEADING;
extern const int STATIC_TEXT_V5;
extern const int IDC_EDIT_GAP_WIDTH;
extern const int STATIC_TEXT_V6;
extern const int IDC_EDIT_LEFTMARGIN;
extern const int STATIC_TEXT_V8;
extern const int ID_TEXTCTRL_MIN_PILE_WIDTH;
extern const int STATIC_TEXT_V7;
extern const int IDC_EDIT_MULTIPLIER;
extern const int IDC_EDIT_DIALOGFONTSIZE;
extern const int IDC_CHECK_WELCOME_VISIBLE;
extern const int IDC_CHECK_HIGHLIGHT_AUTO_INSERTED_TRANSLATIONS;
extern const int IDC_BUTTON_CHOOSE_HIGHLIGHT_COLOR;
extern const int ID_PANEL_AUTO_INSERT_COLOR;
extern const int IDC_CHECK_SHOW_ADMIN_MENU;
extern const int ID_RADIOBOX_SCROLL_INTO_VIEW;
extern const int ID_CHECKBOX_ENABLE_INSERT_ZWSP;
extern const int ID_DIACRITICS_SLIDER;
extern const int ID_CHECKBOX_SOLIDUS_WDBREAK;
extern const int IDC_CHECK_LEGACY_SRC_TEXT_COPY;
extern const int ID_CHECKBOX_FREEZE_THAW;
extern const int ID_CHECKBOX_DIAGNOSTIC_LOG;
wxSizer *ViewPageFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

wxSizer *UnitsPageFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXTCTRL_AS_STATIC_AUTOSAVE;
extern const int IDC_CHECK_NO_AUTOSAVE;
extern const int IDC_RADIO_BY_MINUTES;
extern const int IDC_EDIT_MINUTES;
extern const int IDC_SPIN_MINUTES;
extern const int IDC_RADIO_BY_MOVES;
extern const int IDC_EDIT_MOVES;
extern const int IDC_SPIN_MOVES;
extern const int IDC_EDIT_KB_MINUTES;
extern const int IDC_SPIN_KB_MINUTES;
wxSizer *AutoSavingPageFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_RADIO_DIV1;
extern const int IDC_RADIO_DIV2;
extern const int IDC_RADIO_DIV3;
extern const int IDC_RADIO_DIV4;
extern const int IDC_RADIO_DIV5;
extern const int IDC_COMBO_CHOOSE_BOOK;
extern const int ID_TEXT_AS_STATIC;
wxSizer *WhichBookDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_STATIC_MSG;
extern const int IDC_EDIT_ERR_XML;
extern const int IDC_STATIC_LBL;
extern const int IDC_EDIT_OFFSET;
wxSizer *XMLErrorDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_ED_SILCONVERTER_NAME;
extern const int IDC_BTN_SELECT_SILCONVERTER;
extern const int IDC_ED_SILCONVERTER_INFO;
extern const int IDC_BTN_CLEAR_SILCONVERTER;
wxSizer *SilConvertersDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_STATIC_FOLDER_DOCS;
extern const int IDC_LIST_FOLDER_DOCS;
extern const int IDC_BUTTON_NEXT_CHAPTER;
extern const int IDC_BUTTON_SPLIT_NOW;
extern const int IDC_STATIC_SPLITTING_WAIT;
extern const int IDC_RADIO_PHRASEBOX_LOCATION;
extern const int IDC_RADIO_CHAPTER_SFMARKER;
extern const int IDC_RADIO_DIVIDE_INTO_CHAPTERS;
extern const int IDC_STATIC_SPLIT_NAME;
extern const int IDC_EDIT1;
extern const int IDC_STATIC_REMAIN_NAME;
extern const int IDC_EDIT2;
wxSizer *SplitDialogFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_LIST_MARKERS;
extern const int IDC_TEXTCTRL_AS_STATIC_PLACE_INT_MKRS;
wxSizer *PlaceInternalMarkersDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_EDIT_NOTE;
extern const int IDC_EDIT_FIND_TEXT;
extern const int IDC_FIND_NEXT_BTN;
extern const int IDC_LAST_BTN;
extern const int IDC_NEXT_BTN;
extern const int IDC_PREV_BTN;
extern const int IDC_FIRST_BTN;
extern const int IDC_DELETE_BTN;
wxSizer *NoteDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXTCTRL_AS_STATIC_COLLECT_BT;
extern const int IDC_RADIO_COLLECT_ADAPTATIONS;
extern const int IDC_RADIO_COLLECT_GLOSSES;
wxSizer *CollectBackTranslationsDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_SPIN_DELAY_TICKS;
wxSizer *SetDelayDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_RADIO_EXPORT_ALL;
extern const int IDC_RADIO_EXPORT_SELECTED_MARKERS;
extern const int ID_TEXTCTRL_AS_STATIC_EXPORT_OPTIONS;
extern const int IDC_LIST_SFMS;
extern const int IDC_BUTTON_EXCLUDE_FROM_EXPORT;
extern const int IDC_BUTTON_INCLUDE_IN_EXPORT;
extern const int IDC_BUTTON_UNDO;
extern const int IDC_STATIC_SPECIAL_NOTE;
extern const int IDC_CHECK_PLACE_FREE_TRANS;
extern const int IDC_CHECK_INTERLINEAR_PLACE_FREE_TRANS;
extern const int IDC_CHECK_PLACE_BACK_TRANS;
extern const int IDC_CHECK_INTERLINEAR_PLACE_BACK_TRANS;
extern const int IDC_CHECK_PLACE_AI_NOTES;
extern const int IDC_CHECK_INTERLINEAR_PLACE_AI_NOTES;
wxSizer *ExportOptionsDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_PNLEXPORT;
extern const int ID_BUTTON_CHANGE_BOOK_NAME;
extern const int ID_LBLEXPORTTYPEDESCRIPTION;
extern const int ID_RDOFILTEROFF;
extern const int ID_RDOFILTERON;
extern const int ID_BTNFILTEROPTIONS;
extern const int ID_CHKPROJECTNAMEPREFIX;
extern const int ID_CHKTARGETTEXTPREFIX;
extern const int ID_CHKDATETIMESUFFIX;
wxSizer *ExportSaveAsDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_CHECK_INCLUDE_NAV_TEXT;
extern const int IDC_CHECK_INCLUDE_SOURCE_TEXT;
extern const int IDC_CHECK_INCLUDE_TARGET_TEXT;
extern const int IDC_CHECK_INCLUDE_GLS_TEXT;
extern const int ID_TEXTCTRL_AS_STATIC_EXP_INTERLINEAR;
extern const int IDC_RADIO_PORTRAIT;
extern const int IDC_RADIO_LANDSCAPE;
extern const int IDC_RADIO_OUTPUT_ALL;
extern const int IDC_RADIO_OUTPUT_CHAPTER_VERSE_RANGE;
extern const int IDC_EDIT_FROM_CHAPTER;
extern const int IDC_EDIT_FROM_VERSE;
extern const int IDC_EDIT_TO_CHAPTER;
extern const int IDC_EDIT_TO_VERSE;
extern const int IDC_RADIO_OUTPUT_PRELIM;
extern const int IDC_RADIO_OUTPUT_FINAL;
extern const int ID_CHECK_NEW_TABLES_FOR_NEWLINE_MARKERS;
extern const int ID_CHECK_CENTER_TABLES_FOR_CENTERED_MARKERS;
extern const int ID_CHECKBOX_PREFIX_EXPORT_PROJ_NAME;
extern const int ID_CHECKBOX_PREFIX_EXPORT_TYPE;
extern const int ID_CHECKBOX_SUFFIX_EXPORT_DATETIME_STAMP;
extern const int IDC_BUTTON_RTF_EXPORT_FILTER_OPTIONS;
wxSizer *ExportInterlinearDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_LIST_MARKER;
extern const int IDC_EDIT_MARKER_TEXT;
extern const int IDC_REMOVE_BTN;
extern const int IDC_BUTTON_SWITCH_ENCODING;
extern const int IDC_LIST_MARKER_END;
extern const int IDC_STATIC_MARKER_DESCRIPTION;
extern const int IDC_STATIC_MARKER_STATUS;
wxSizer *ViewFilteredMaterialDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_EDIT_CHAPTER;
extern const int IDC_EDIT_VERSE;
wxSizer *GoToDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXT_MAIN_MSG;
extern const int IDC_EDIT_KEY;
extern wxSizer *m_topRightBoxLabel;
extern const int IDC_EDIT_ADAPTATION;
extern const int IDC_RADIO_ACCEPT_HERE;
extern const int ID_TEXT_NO_ADAPT_MSG;
extern const int IDC_RADIO_CHANGE_INSTEAD;
extern const int IDC_EDIT_CH_VERSE;
extern const int ID_TEXT_AVAIL_ADAPT_OR_GLOSS;
extern const int IDC_LIST_TRANSLATIONS;
extern wxSizer *m_pNumberedPointsStaticBoxSizer;
extern const int ID_TEXT_CLICK_LISTED;
extern const int ID_TEXT_CLICK_EDIT_LISTED;
extern const int ID_TEXT_TYPE_NEW;
extern const int IDC_CHECK_DO_SAME;
extern const int ID_BUTTON_IGNORE_IT;
wxSizer *ConsistencyCheckDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_RADIO_CHECK_OPEN_DOC_ONLY;
extern const int IDC_RADIO_CHECK_SELECTED_DOCS;
extern const int ID_TEXTCTRL_MSG;
extern const int ID_CHECKBOXBLIND_FIXES;
extern const int ID_TEXTCTRL_MSG2;
wxSizer *ChooseConsistencyCheckTypeDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXTCTRL;
extern const int IDC_EDIT_OLD_SOURCE_TEXT;
extern const int ID_TEXTCTRL_EDIT_SOURCE_AS_STATIC1;
extern const int ID_TEXTCTRL_EDIT_SOURCE_AS_STATIC2;
extern const int IDC_EDIT_NEW_SOURCE;
wxSizer *EditSourceTextDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_EDIT_UNPACK_WARN;
wxSizer *UnpackWarningDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_STATIC_COUNT;
extern const int IDC_LIST_SRC_KEYS;
extern const int IDC_LIST_EXISTING_TRANSLATIONS;
extern const int IDC_EDIT_EDITORADD;
extern const int IDC_BUTTON_UPDATE;
extern const int IDC_BUTTON_MOVE_UP;
extern const int IDC_BUTTON_ADD;
extern const int IDC_BUTTON_MOVE_DOWN;
extern const int IDC_ADD_NOTHING;
extern const int IDC_EDIT_SHOW_FLAG;
extern const int IDC_BUTTON_FLAG_TOGGLE;
extern const int IDC_EDIT_SRC_KEY;
extern const int ID_BUTTON_SRC_FIND_GO;
extern const int IDC_EDIT_REF_COUNT;
extern const int ID_BUTTON_REMOVE_SOME;
extern const int ID_TEXTCTRL_SEARCH;
extern const int ID_BUTTON_GO;
extern const int ID_BUTTON_ERASE_ALL_LINES;
extern const int ID_COMBO_OLD_SEARCHES;
wxSizer *KBEditorPanelFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_STATIC_TEXT_SELECT_A_TAB;
extern const int ID_STATIC_WHICH_KB;
extern const int ID_KB_EDITOR_NOTEBOOK;
wxSizer *KBEditorDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_STATIC_DOCS_IN_FOLDER;
extern const int IDC_LIST_SOURCE_FOLDER_DOCS;
extern const int IDC_RADIO_TO_BOOK_FOLDER;
extern const int IDC_RADIO_FROM_BOOK_FOLDER;
extern const int IDC_VIEW_OTHER;
extern const int IDC_BUTTON_RENAME_DOC;
extern const int IDC_MOVE_NOW;
wxSizer *MoveDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXTCTRL_AS_STATIC_JOIN1;
extern const int IDC_BUTTON_MOVE_ALL_RIGHT;
extern const int IDC_BUTTON_MOVE_ALL_LEFT;
extern const int ID_TEXTCTRL_AS_STATIC_JOIN2;
extern const int ID_TEXTCTRL_AS_STATIC_JOIN3;
extern const int ID_JOIN_NOW;
extern const int IDC_STATIC_JOINING_WAIT;
extern const int ID_TEXTCTRL_AS_STATIC_JOIN4;
extern const int IDC_EDIT_NEW_FILENAME;
wxSizer *JoinDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

wxSizer *ListDocInOtherFolderDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_STATIC_SET_FONT_TITLE;
extern const int ID_STATIC_CURR_ENCODING_IS;
extern const int ID_TEXT_CURR_ENCODING;
extern const int ID_LISTBOX_POSSIBLE_FACENAMES;
extern const int ID_LISTBOX_POSSIBLE_ENCODINGS;
extern wxSizer *pTestBoxStaticTextBoxSizer;
extern const int ID_TEXT_TEST_ENCODING_BOX;
extern const int ID_BUTTON_CHART_FONT_SIZE_DECREASE;
extern const int ID_BUTTON_CHART_FONT_SIZE_INCREASE;
extern const int ID_STATIC_CHART_FONT_SIZE;
extern const int ID_SCROLLED_ENCODING_WINDOW;
wxSizer *SetEncodingDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_EDIT_CCT;
wxSizer *CCTableEditDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_LIST_CCTABLES;
extern const int IDC_EDIT_SELECTED_TABLE;
extern const int IDC_BUTTON_BROWSE;
extern const int IDC_BUTTON_CREATE_CCT;
extern const int IDC_BUTTON_EDIT_CCT;
extern const int IDC_BUTTON_SELECT_NONE;
extern const int IDC_EDIT_FOLDER_PATH;
wxSizer *CCTablePageFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_RADIO_SRC_ONLY_FIND;
extern const int IDC_RADIO_TGT_ONLY_FIND;
extern const int IDC_RADIO_SRC_AND_TGT_FIND;
extern const int IDC_CHECK_IGNORE_CASE_FIND;
extern const int IDC_STATIC_SRC_FIND;
extern const int IDC_EDIT_SRC_FIND;
extern const int IDC_STATIC_TGT_FIND;
extern const int IDC_EDIT_TGT_FIND;
extern const int IDC_CHECK_INCLUDE_PUNCT_FIND;
extern const int IDC_CHECK_SPAN_SRC_PHRASES_FIND;
extern const int IDC_STATIC_SPECIAL;
extern wxSizer *IDC_STATIC_SIZER_SPECIAL;
extern const int IDC_RADIO_RETRANSLATION;
extern const int IDC_RADIO_NULL_SRC_PHRASE;
extern const int IDC_RADIO_SFM;
extern const int IDC_STATIC_SELECT_MKR;
extern const int IDC_COMBO_SFM;
extern const int IDC_BUTTON_SPECIAL;
wxSizer *FindDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_RADIO_SRC_ONLY_REPLACE;
extern const int IDC_RADIO_TGT_ONLY_REPLACE;
extern const int IDC_RADIO_SRC_AND_TGT_REPLACE;
extern const int IDC_CHECK_IGNORE_CASE_REPLACE;
extern const int IDC_STATIC_SRC_REPLACE;
extern const int IDC_EDIT_SRC_REPLACE;
extern const int IDC_STATIC_TGT_REPLACE;
extern const int IDC_EDIT_TGT_REPLACE;
extern const int IDC_CHECK_INCLUDE_PUNCT_REPLACE;
extern const int IDC_CHECK_SPAN_SRC_PHRASES_REPLACE;
extern const int IDC_STATIC_REPLACE;
extern const int IDC_EDIT_REPLACE;
extern const int IDC_REPLACE;
extern const int IDC_REPLACE_ALL_BUTTON;
wxSizer *ReplaceDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXTCTRL_AS_STATIC;
extern const int IDC_EDIT_TBLNAME;
wxSizer *CCTableNameDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXTCTRL_EDIT_AS_STATIC;
extern const int ID_CHECKBOX_INCLUDE_FREE_TRANS_TEXT;
extern const int ID_CHECKBOX_INCLUDE_GLOSSES_TEXT;
extern const int ID_RADIO_ALL;
extern const int ID_RADIO_SELECTION;
extern const int IDC_RADIO_PAGES;
extern const int IDC_EDIT_PAGES_FROM;
extern const int IDC_EDIT_PAGES_TO;
extern const int IDC_RADIO_CHAPTER_VERSE_RANGE;
extern const int IDC_EDIT3;
extern const int IDC_EDIT4;
extern const int IDC_CHECK_SUPPRESS_PREC_HEADING;
extern const int IDC_CHECK_INCLUDE_FOLL_HEADING;
extern const int IDC_CHECK_SUPPRESS_FOOTER;
extern const int ID_CHECKBOX_FOOTER_ONLY_HAS_PAGE_NUM;
wxSizer *PrintOptionsDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_CC_TABBED_NOTEBOOK;
wxSizer *CCTabbedNotebookFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_LIST_UI_LANGUAGES;
extern const int ID_TEXT_AS_STATIC_SHORT_LANG_NAME;
extern const int ID_TEXT_AS_STATIC_LONG_LANG_NAME;
extern const int IDC_LOCALIZATION_PATH;
wxSizer *ChooseLanguageDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_COMBO_REMOVALS;
extern const int ID_STATIC_TEXT_REMOVALS;
extern const int IDC_BUTTON_UNDO_LAST_COPY;
wxSizer *RemovalsBarFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_EDIT_MSG_TEXT;
extern const int IDC_BUTTON_PREV_STEP;
extern const int IDC_BUTTON_NEXT_STEP;
extern const int ID_BUTTON_END_NOW;
extern const int ID_BUTTON_CANCEL_ALL_STEPS;
wxSizer *VertEditBarFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXT_TITLE;
extern const int IDC_TOGGLE_UNNNN_BTN;
extern const int ID_TEXTCTRL_AS_STATIC_PUNCT_CORRESP_PAGE;
extern const int IDC_EDIT_SRC0;
extern const int IDC_EDIT_TGT0;
extern const int IDC_EDIT_SRC1;
extern const int IDC_EDIT_TGT1;
extern const int IDC_EDIT_SRC2;
extern const int IDC_EDIT_TGT2;
extern const int IDC_EDIT_SRC3;
extern const int IDC_EDIT_TGT3;
extern const int IDC_EDIT_SRC4;
extern const int IDC_EDIT_TGT4;
extern const int IDC_EDIT_SRC5;
extern const int IDC_EDIT_TGT5;
extern const int IDC_EDIT_SRC6;
extern const int IDC_EDIT_TGT6;
extern const int IDC_EDIT_SRC7;
extern const int IDC_EDIT_TGT7;
extern const int IDC_EDIT_SRC8;
extern const int IDC_EDIT_TGT8;
extern const int IDC_EDIT_SRC9;
extern const int IDC_EDIT_TGT9;
extern const int IDC_EDIT_SRC10;
extern const int IDC_EDIT_TGT10;
extern const int IDC_EDIT_SRC11;
extern const int IDC_EDIT_TGT11;
extern const int IDC_EDIT_SRC12;
extern const int IDC_EDIT_TGT12;
extern const int IDC_EDIT_SRC13;
extern const int IDC_EDIT_TGT13;
extern const int IDC_EDIT_SRC14;
extern const int IDC_EDIT_TGT14;
extern const int IDC_EDIT_SRC15;
extern const int IDC_EDIT_TGT15;
extern const int IDC_EDIT_SRC16;
extern const int IDC_EDIT_TGT16;
extern const int IDC_EDIT_SRC17;
extern const int IDC_EDIT_TGT17;
extern const int IDC_EDIT_SRC18;
extern const int IDC_EDIT_TGT18;
extern const int IDC_EDIT_SRC19;
extern const int IDC_EDIT_TGT19;
extern const int IDC_EDIT_SRC20;
extern const int IDC_EDIT_TGT20;
extern const int IDC_EDIT_SRC21;
extern const int IDC_EDIT_TGT21;
extern const int IDC_EDIT_SRC22;
extern const int IDC_EDIT_TGT22;
extern const int IDC_EDIT_SRC23;
extern const int IDC_EDIT_TGT23;
extern const int IDC_EDIT_SRC24;
extern const int IDC_EDIT_TGT24;
extern const int IDC_EDIT_SRC25;
extern const int IDC_EDIT_TGT25;
extern const int IDC_EDIT_2SRC0;
extern const int IDC_EDIT_2TGT0;
extern const int IDC_EDIT_2SRC1;
extern const int IDC_EDIT_2TGT1;
extern const int IDC_EDIT_2SRC2;
extern const int IDC_EDIT_2TGT2;
extern const int IDC_EDIT_2SRC3;
extern const int IDC_EDIT_2TGT3;
extern const int IDC_EDIT_2SRC4;
extern const int IDC_EDIT_2TGT4;
extern const int IDC_EDIT_2SRC5;
extern const int IDC_EDIT_2TGT5;
extern const int IDC_EDIT_2SRC6;
extern const int IDC_EDIT_2TGT6;
extern const int IDC_EDIT_2SRC7;
extern const int IDC_EDIT_2TGT7;
extern const int IDC_EDIT_2SRC8;
extern const int IDC_EDIT_2TGT8;
extern const int IDC_EDIT_2SRC9;
extern const int IDC_EDIT_2TGT9;
wxSizer *PunctCorrespPageFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern wxSizer *ID_CONTROLBAR_2_LINE_SIZER_TOP;
extern wxSizer *ID_CONTROLBAR_2_LINE_SIZER_BOTTOM;
wxSizer *ControlBar2LineFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXTCTRL_MSG1;
extern const int ID_BUTTON_LOCATE_SOURCE_FOLDER;
extern const int ID_BITMAPBUTTON_SRC_OPEN_FOLDER_UP;
extern const int ID_TEXT_SOURCE_FOLDER_PATH;
extern const int ID_TEXTCTRL_SOURCE_PATH;
extern const int ID_LISTCTRL_SOURCE_CONTENTS;
extern const int ID_BUTTON_LOCATE_DESTINATION_FOLDER;
extern const int ID_BITMAPBUTTON_DEST_OPEN_FOLDER_UP;
extern const int ID_TEXTCTRL_DESTINATION_PATH;
extern const int ID_LISTCTRL_DESTINATION_CONTENTS;
extern const int ID_BUTTON_MOVE;
extern const int ID_BUTTON_COPY;
extern const int ID_BUTTON_PEEK;
extern const int ID_BUTTON_RENAME;
extern const int ID_BUTTON_DELETE;
wxSizer *MoveOrCopyFilesOrFoldersFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXT_MSG1;
extern const int ID_STATICBITMAP;
extern const int ID_TEXTCTRL_SOURCE_FILE_DETAILS;
extern const int ID_TEXTCTRL_DESTINATION_FILE_DETAILS;
extern const int ID_RADIOBUTTON_REPLACE;
extern const int ID_RADIOBUTTON_NO_COPY;
extern const int ID_RADIOBUTTON_COPY_AND_RENAME;
extern const int ID_TEXT_MODIFY_NAME;
extern const int ID_CHECKBOX_HANDLE_SAME;
wxSizer *FilenameConflictFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_LISTBOX_UPDATED;
extern const int ID_TEXTCTRL_INFO_SOURCE;
extern const int ID_TEXTCTRL_INFO_REFS;
extern const int ID_LISTBOX_MATCHED;
extern const int ID_BUTTON_UPDATE;
extern const int ID_TEXTCTRL_LOCAL_SEARCH;
extern const int ID_BUTTON_FIND_NEXT;
extern const int ID_TEXTCTRL_EDITBOX;
extern const int ID_BUTTON_RESTORE;
extern const int ID_BUTTON_REMOVE_UPDATE;
wxSizer *KBEditSearchFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_STATICTEXT_SCROLL_LIST;
extern const int ID_LIST_LANGUAGE_CODES_NAMES;
extern const int ID_STATICTEXT_SEARCH_FOR_LANG_NAME;
extern const int ID_TEXTCTRL_SEARCH_LANG_NAME;
extern const int ID_BUTTON_FIND_CODE;
extern const int ID_BUTTON_FIND_LANGUAGE;
extern const int ID_BUTTON_USE_SEL_AS_SRC;
extern const int ID_BUTTON_USE_SEL_AS_TGT;
extern const int ID_BUTTON_USE_SEL_AS_GLS;
extern const int ID_BUTTON_USE_SEL_AS_FRTR;
extern const int ID_TEXT_CODES_MSG1;
extern const int ID_TEXT_CODES_MSG2;
extern const int ID_SRC_LANGUAGE_CODE;
extern const int ID_TGT_LANGUAGE_CODE;
extern const int ID_GLS_LANGUAGE_CODE;
extern const int ID_TEXT_FRTR;
extern const int ID_TEXTCTRL_SRC_LANG_CODE;
extern const int ID_TEXTCTRL_TGT_LANG_CODE;
extern const int ID_TEXTCTRL_GLS_LANG_CODE;
extern const int ID_TEXTCTRL_FRTR_LANG_CODE;
wxSizer *LanguageCodesDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXTCTRL_PEEKMSG;
extern wxSizer *m_pHorizBox_for_textctrl;
extern const int ID_TEXTCTRL_LINES100;
extern const int ID_BUTTON_TOGGLE_TEXT_DIRECTION;
wxSizer *PeekAtFileFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXTCTRL_INSTRUCTIONS;
extern const int ID_LISTBOX_LOADABLES_FILENAMES;
wxSizer *NewDocFromSourceDataFolderFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_STATIC_SELECT_A_TAB;
extern const int ID_MENU_EDITOR_NOTEBOOK;
extern const int ID_RADIOBUTTON_NONE;
extern const int ID_RADIOBUTTON_USE_PROFILE;
extern const int ID_COMBO_PROFILE_ITEMS;
extern const int ID_BUTTON_RESET_TO_FACTORY;
extern const int ID_TEXT_STATIC_DESCRIPTION;
extern const int ID_TEXTCTRL_PROFILE_DESCRIPTION;
wxSizer *MenuEditorDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_CHECKLISTBOX_MENU_ITEMS;
wxSizer *MenuEditorPanelFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_RADIO_USE_UBS_SET_ONLY;
extern const int IDC_RADIO_USE_SILPNG_SET_ONLY;
extern const int IDC_RADIO_USE_BOTH_SETS;
extern const int IDC_RADIO_USE_UBS_SET_ONLY_PROJ;
extern const int IDC_RADIO_USE_SILPNG_SET_ONLY_PROJ;
extern const int IDC_RADIO_USE_BOTH_SETS_PROJ;
extern const int IDC_CHECK_CHANGE_FIXED_SPACES_TO_REGULAR_SPACES_USFM;
extern const int ID_TEXTCTRL_AS_STATIC_FILTERPAGE;
extern const int IDC_LIST_SFMS_PROJ;
wxSizer *UsfmFilterPageFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_CHECK_USE_GUESSER;
extern const int ID_PANEL_GUESS_COLOR_DISPLAY;
extern const int ID_BUTTON_GUESS_HIGHLIGHT_COLOR;
extern const int ID_SLIDER_GUESSER;
extern const int ID_CHECK_ALLOW_GUESSER_ON_UNCHANGED_CC_OUTPUT;
extern const int ID_TEXT_STATIC_NUM_CORRESP_ADAPTATIONS_GUESSER;
extern const int ID_TEXT_STATIC_NUM_CORRESP_GLOSSING_GUESSER;
extern const int ID_BTN_GUESSER_HOW_TO_USE_DLG;
extern const int ID_TEXT_USE_ESC_KEY;
wxSizer *GuesserSettingsDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXTCTRL_DEVS_EMAIL_ADDR;
extern const int ID_TEXTCTRL_MY_EMAIL_ADDR;
extern const int ID_TEXTCTRL_SUMMARY_SUBJECT;
extern wxSizer *STATIC_TEXT_DESCRIPTION;
extern const int ID_TEXTCTRL_DESCRIPTION_BODY;
extern const int ID_TEXTCTRL_SENDERS_NAME;
extern const int ID_CHECKBOX_LET_DEVS_KNOW_AI_USAGE;
extern const int ID_BUTTON_VIEW_USAGE_LOG;
extern const int ID_TEXT_AI_VERSION;
extern const int ID_TEXT_RELEASE_DATE;
extern const int ID_TEXT_DATA_TYPE;
extern const int ID_TEXT_FREE_MEMORY;
extern const int ID_TEXT_SYS_LOCALE;
extern const int ID_TEXT_INTERFACE_LANGUAGE;
extern const int ID_TEXT_SYS_ENCODING;
extern const int ID_TEXT_SYS_LAYOUT_DIR;
extern const int ID_TEXT_WXWIDGETS_VERSION;
extern const int ID_TEXT_OS_VERSION;
extern const int ID_BUTTON_SAVE_REPORT_AS_TEXT_FILE;
extern const int ID_BUTTON_LOAD_SAVED_REPORT;
extern const int ID_RADIOBUTTON_SEND_DIRECTLY_FROM_AI;
extern const int ID_RADIOBUTTON_SEND_TO_MY_EMAIL;
extern const int ID_BUTTON_ATTACH_PACKED_DOC;
extern const int ID_BUTTON_SEND_NOW;
wxSizer *EmailReportDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXT_LOG_FILE_PATH_AND_NAME;
extern const int ID_TEXTCTRL_LOGGED_TEXT;
wxSizer *LogViewerFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern wxSizer *STATIC_TEXT_PTorBE_PROJECTS;
extern const int ID_TEXT_SRC_PROJ;
extern const int ID_STATIC_TEXT_SRC_PROJ;
extern const int ID_TEXT_TGT_PROJ;
extern const int ID_STATIC_TEXT_TGT_PROJ;
extern const int ID_TEXT_FREETRANS_PROJ;
extern const int ID_STATIC_TEXT_FREETRANS_PROJ;
extern const int ID_TEXT_AI_PROJ;
extern const int ID_TEXT_SELECT_A_CHAPTER;
extern const int ID_LISTBOX_BOOK_NAMES;
extern const int ID_LISTCTRL_CHAPTER_NUMBER_AND_STATUS;
extern const int ID_TEXTCTRL_AS_STATIC_NOTE;
extern const int ID_LINE_1;
wxSizer *GetSourceTextFromEditorDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXTCTRL_AS_STATIC_TOP_INFO;
extern const int ID_CHECKBOX_PROTECT_SOURCE_INPUTS;
extern const int ID_BUTTON_PRE_LOAD_SOURCE_TEXTS;
extern const int ID_CHECKBOX_PROTECT_FREETRANS_OUTPUTS;
extern const int ID_CHECKBOX_PROTECT_FREETRANS_RTF_OUTPUTS;
extern const int ID_CHECKBOX_PROTECT_GLOSS_OUTPUTS;
extern const int ID_CHECKBOX_PROTECT_GLOSS_RTF_OUTPUTS;
extern const int ID_CHECKBOX_PROTECT_SOURCE_OUTPUTS;
extern const int ID_CHECKBOX_PROTECT_SOURCE_RTF_OUTPUTS;
extern const int ID_CHECKBOX_PROTECT_TARGET_OUTPUTS;
extern const int ID_CHECKBOX_PROTECT_TARGET_RTF_OUTPUTS;
extern const int ID_CHECKBOX_PROTECT_REPORTS_OUTPUTS;
extern const int ID_CHECKBOX_PROTECT_INTERLINEAR_RTF_OUTPUTS;
extern const int ID_CHECKBOX_PROTECT_XHTML_OUTPUTS;
extern const int ID_CHECKBOX_PROTECT_PATHWAY_OUTPUTS;
extern const int ID_CHECKBOX_PROTECT_KB_INPUTS_AND_OUTPUTS;
extern const int ID_CHECKBOX_PROTECT_LIFT_INPUTS_AND_OUTPUTS;
extern const int ID_CHECKBOX_PROTECT_PACKED_INPUTS_AND_OUTPUTS;
extern const int ID_CHECKBOX_PROTECT_CCTABLE_INPUTS_AND_OUTPUTS;
extern const int ID_BUTTON_UNSELECT_ALL_CHECKBOXES;
extern const int ID_BUTTON_SELECT_ALL_CHECKBOXES;
wxSizer *AssignLocationsForInputsOutputsFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXTCTRL_SOURCE_PHRASE_1;
extern const int ID_TEXT_EMPTY_STR;
extern const int ID_RADIO_NO_ADAPTATION;
extern const int ID_RADIO_LEAVE_HOLE;
extern const int ID_RADIO_NOT_IN_KB;
extern const int ID_RADIOBUTTON_TYPE_AORG;
extern const int ID_TEXTCTRL_TYPED_AORG;
extern const int ID_CHECK_DO_SAME;
wxSizer *ConsistencyCheck_EmptyNoTU_DlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXTCTRL_SOURCE_PHRASE_2;
extern const int ID_TEXTCTRL_TARGET_PHRASE_2;
extern const int ID_TEXT_EXISTS_STR;
extern const int ID_RADIO_STORE_NORMALLY;
extern const int ID_RADIO_NOT_IN_KB_LEAVEINDOC;
extern const int ID_CHECK_DO_SAME2;
wxSizer *ConsistencyCheck_ExistsNoTU_DlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_BITMAPBUTTON_MOVE_BACK;
extern const int ID_BITMAPBUTTON_MOVE_FORWARD;
extern const int ID_BUTTON_OPEN_HTML_FILE;
extern const int ID_TEXTCTRL_HTML_FILE_PATH;
extern const int ID_HTML_WINDOW;
wxSizer *HtmlFileViewerDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXT_SRC;
extern const int ID_TEXTCTRL_SRC_LANG;
extern const int ID_TEXT_NOTCHANGED;
extern const int ID_TEXT_TGT_CODE;
extern const int ID_LISTBOX_TGT;
extern const int ID_TEXT_GLOSS_CODE;
extern const int ID_LISTBOX_GLOSS;
extern const int ID_TEXTCTRL_TELL;
wxSizer *LiftLangFilterFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_RADIOBOX_KB_EXPORT_IMPORT_OPTIONS;
extern wxSizer *pKBExpImpCheckBoxesSizer;
wxSizer *KBExportImportOptionsFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXT_SELECTED_AI_PROJECT;
extern const int ID_BUTTON_TELL_ME_MORE;
extern const int ID_RADIOBUTTON_TURN_COLLAB_ON;
extern const int ID_TEXTCTRL_NOT_INSTALLED_ERROR_MSG;
extern const int ID_RADIOBUTTON_TURN_COLLAB_OFF;
extern const int ID_RADIOBUTTON_READ_ONLY_MODE;
wxSizer *ChooseCollabOptionsDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXT_USE_THIS_DIALOG;
extern const int ID_COMBO_AI_PROJECTS;
extern const int ID_BUTTON_CREATE_NEW_AI_PROJECT;
extern const int ID_TEXT_STATIC_LIST_OF_PROJECTS;
extern const int ID_RADIOBOX_EXTERNAL_SCRIPTURE_EDITOR;
extern const int IDC_LIST_OF_COLLAB_PROJECTS;
extern const int ID_TEXTCTRL_AS_STATIC_TOP_NOTE;
extern const int ID_TEXT_STATIC_SELECT_WHICH_PROJECTS;
extern const int ID_TEXT_STATIC_SRC_FROM_THIS_PROJECT;
extern const int ID_TEXTCTRL_AS_STATIC_SELECTED_SRC_PROJ;
extern const int ID_BUTTON_SELECT_FROM_LIST_SOURCE_PROJ;
extern const int ID_RADIO_BY_CHAPTER_ONLY;
extern const int ID_TEXT_STATIC_TARGET_TO_THIS_PROJECT;
extern const int ID_TEXTCTRL_AS_STATIC_SELECTED_TGT_PROJ;
extern const int ID_BUTTON_SELECT_FROM_LIST_TARGET_PROJ;
extern const int ID_RADIO_BY_WHOLE_BOOK;
extern const int ID_TEXT_STATIC_TO_THIS_FT_PROJECT;
extern const int ID_TEXTCTRL_AS_STATIC_SELECTED_FREE_TRANS_PROJ;
extern const int ID_BUTTON_SELECT_FROM_LIST_FREE_TRANS_PROJ;
extern const int ID_BUTTON_NO_FREE_TRANS;
extern const int ID_LINE_2;
extern const int ID_BUTTON_SAVE_SETUP_FOR_THIS_PROJ_NOW;
extern const int ID_BUTTON_REMOVE_THIS_PROJ_FROM_COLLAB;
wxSizer *SetupCollaborationBetweenAIandEditorFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXT_TOP_INFO_1;
extern const int ID_TEXT_TOP_INFO_2;
extern const int ID_TEXTCTRL_SRC_LANG_NAME;
extern const int ID_TEXTCTRL_TGT_LANG_NAME;
extern const int ID_TEXT_AS_STATIC_NEW_AI_PROJ_NAME;
extern const int ID_TEXTCTRL_NEW_AI_PROJ_NAME;
wxSizer *CreateNewAIProjForCollabFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXT_STATIC_LAST_BOOKNAME;
extern const int ID_TEXTCTRL_OLD_BOOKNAME;
extern const int ID_TEXT_STATIC_TOP_MSG;
extern const int ID_RADIO_USE_LAST_BOOKNAME;
extern const int ID_RADIO_BOOKNAME_IS_INAPPROPRIATE;
extern const int ID_RADIO_SUGGESTED_BOOKNAME_ACCEPTABLE;
extern const int ID_RADIO_DIFFERENT_BOOKNAME;
extern const int ID_TEXTCTRL_BOOKNAME;
wxSizer *BookNameDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_SPINCTRL_RECEIVE;
extern const int ID_RADIO_SHARING_OFF;
extern const int ID_GET_RECENT;
extern const int ID_GET_ALL;
extern const int ID_SEND_ALL;
wxSizer *kb_sharing_dlg_func( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_LST_TOOLBAR_BUTTONS;
extern const int ID_BMP_TOOLBAR_SMALL;
extern const int ID_BMP_TOOLBAR_MEDIUM;
extern const int ID_BMP_TOOLBAR_LARGE;
extern const int ID_RDO_TOOLBAR_SMALL;
extern const int ID_RDO_TOOLBAR_MEDIUM;
extern const int ID_RDO_TOOLBAR_LARGE;
extern const int ID_CBO_TOOLBAR_ICON;
extern const int ID_BTN_TOOLBAR_MINIMAL;
extern const int ID_TOOLBAR_RESET;
wxSizer *ToolbarPageFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_MYLISTBOX_TRANSLATIONS;
extern const int IDC_EDIT_MATCHED_SOURCE;
extern const int IDC_EDIT_REFERENCES;
extern const int ID_BUTTON_CANCEL_ASK;
extern const int IDC_EDIT_NEW_TRANSLATION;
wxSizer *ChooseTranslationDlgFunc2( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int IDC_COMMIT_COMMENT;
extern const int IDC_COMMIT_BLURB;
wxSizer *DVCSDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_COMMITTER;
extern const int ID_VERSION_DATE;
extern const int ID_VERSION_COMMENT;
extern const int ID_BTN_PREV;
extern const int ID_BTN_NEXT;
extern const int ID_ACCEPT;
extern const int ID_LATEST;
wxSizer *DVCSNavDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_LST_VERSIONS;
wxSizer *DVCSLogDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXTCTRL_USERNAME_MSG;
extern const int ID_TEXTCTRL_USERNAME;
extern const int ID_TEXTCTRL_USERNAME_INFORMAL_MSG;
extern const int ID_TEXTCTRL_USERNAME_INFORMAL;
wxSizer *UsernameInputFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_LISTBOX_CUR_USERS;
extern const int ID_THE_USERNAME;
extern const int ID_TEXTCTRL_INFORMAL_NAME;
extern const int ID_TEXTCTRL_PASSWORD;
extern const int ID_TEXTCTRL_PASSWORD_TWO;
extern const int ID_CHECKBOX_USERADMIN;
extern const int ID_CHECKBOX_KBADMIN;
extern const int ID_BUTTON_CLEAR_CONTROLS;
extern const int ID_BUTTON_ADD_USER;
extern const int ID_BUTTON_EDIT_USER;
extern const int ID_BUTTON_REMOVE_USER;
wxSizer *SharedKBManagerUsersPageFunc2( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXT_CONNECTED_TO;
extern const int ID_TEXT_SERVER_URL;
extern const int ID_SHAREDKB_MANAGER_DLG;
wxSizer *SharedKBManagerNotebookFunc2( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_RADIOBUTTON_TYPE1_KB;
extern const int ID_RADIOBUTTON_TYPE2_KB;
extern const int ID_TEXT_EXISTING_KB_CODE_PAIRS;
extern const int ID_LISTBOX_KB_CODE_PAIRS;
extern const int ID_TEXTCTRL_CREATED_BY;
extern const int ID_BUTTON_LOOKUP_THE_CODES;
extern const int ID_BUTTON_RFC5646;
extern const int ID_BUTTON_CLEAR_LIST_SELECTION;
extern const int ID_TEXTCTRL_SRC;
extern const int ID_TEXT_TGT_LANG_CODE;
extern const int ID_TEXTCTRL_NONSRC;
extern const int ID_BUTTON_ADD_DEFINITION;
extern const int ID_BUTTON_CLEAR_BOXES;
extern const int ID_BUTTON_REMOVE_SELECTED_DEFINITION;
wxSizer *SharedKBManager_CreateKbsPageFunc2( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern wxSizer *m_pTopSizer;
extern const int ID_TEXTCTRL_URLMSG;
extern const int ID_SERVER_URL_LABEL_STATELESS;
extern const int ID_TEXTCTRL_SERVER_URL_STATELESS;
extern const int ID_TEXT_USERNAME_LABEL_STATELESS;
extern const int ID_TEXT_USERNAME_MSG_LABEL_STATELESS;
extern const int ID_TEXTCTRL_USERNAME_STATELESS;
extern const int ID_TEXT_ASKED_STATELESS;
extern const int ID_TEXTCTRL_KBSERVER_PWD;
wxSizer *kb_sharing_stateless_setup_func( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXT_INDICATE_SHARE;
extern const int ID_TEXT_LABEL2;
extern const int ID_TEXT_LABEL4;
extern const int ID_CHECKBOX_SHARE_MY_TGT_KB;
extern const int ID_CHECKBOX_SHARE_MY_GLOSS_KB;
wxSizer *kb_share_setup_or_remove_func( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern wxSizer *m_LIST_CTRL_SIZER;
extern const int ID_LISTCTRL_BULK_DEL;
extern const int ID_RADIO_ORGANISE_BY_KEYS;
extern const int ID_RADIO_SIMPLY_TARGET_ALPHABETICAL;
extern const int ID_BUTTON_SAVE_ENTRYLIST_TO_FILE;
wxSizer *Bulk_Delete_Pairs_Func( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXTCTRL_TELL_USER;
extern const int ID_RADIO_JOIN_TO_NEXT;
extern const int ID_RADIO_JOIN_TO_PREVIOUS;
extern const int ID_RADIO_SPLIT_OFF;
wxSizer *FTAdjustFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXTCTRL_SRC_TRANS;
extern const int ID_TEXTCTRL_FREE_TRANS;
extern const int ID_TEXTCTRL_CURRENT_SECTION;
extern const int ID_TEXTCTRL_NEXT_SECTION;
extern const int ID_BUTTON_SPLIT_HERE;
wxSizer *SplitterDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_BUTTON_GUESSER_DLG_EXPLAIN;
extern const int ID_TEXT_PREFIX_LIMIT;
extern const int ID_CHOICE_PREFIXES_LIMIT;
extern const int ID_TEXT_SUFFIX_LIMIT;
extern const int ID_CHOICE_SUFFIXES_LIMIT;
extern const int ID_RADIO_PREFIXES_LIST;
extern const int ID_RADIO_SUFFIXES_LIST;
extern const int ID_LISTCTRL_SRC_TGT_AFFIX_PAIR;
extern const int ID_LABEL_SRC_AFFIX;
extern const int ID_LABEL_TGT_AFFIX;
extern const int ID_STATICTEXT_SRC_SUFFIX;
extern const int ID_TEXT_SRC_AFFIX;
extern const int ID_STATICTEXT_SRC_PREFIX;
extern const int ID_STATICTEXT_TGT_SUFFIX;
extern const int ID_TEXT_TGT_AFFIX;
extern const int ID_STATICTEXT_TGT_PREFIX;
extern const int ID_BUTTON_ADD;
extern const int ID_BUTTON_INSERT;
wxSizer *GuesserAffixListDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_BUTTON_GET_FROM_CLIPBOARD;
extern const int ID_BUTTON_COPY_TO_CLIPBOARD;
extern const int ID_BUTTON_COPY_FREETRANS_TO_CLIPBOARD;
extern const int ID_BUTTON_CLIPBOARD_ADAPT_CLOSE;
wxSizer *ClipboardAdaptBarFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_LISTBOX_CUSTOM_CODES;
extern const int ID_TEXTCTRL_CREATOR;
extern const int ID_BUTTON_LOOKUP_THE_CODE;
extern const int ID_BUTTON_ISO639;
extern const int ID_BUTTON_CLEAR_LIST_SELECTION2;
extern const int ID_BUTTON_CLEAR_BOXES2;
extern const int ID_TEXTCTRL_CUSTOM_CODE;
extern const int ID_TEXTCTRL_DESCRIPTION;
extern const int ID_BUTTON_CREATE_CUSTOM_CODE;
extern const int ID_BUTTON_DELETE_CUSTOM_CODE;
wxSizer *SharedKBManager_CreateCodesPageFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXTCTRL_SEARCH_LANG_NAME2;
extern const int ID_BUTTON_FIND_CODE2;
extern const int ID_BUTTON_FIND_LANGUAGE2;
extern const int ID_BUTTON_USE_SEL_AS_CODE;
extern const int ID_TEXTCTRL_LANG_CODE;
wxSizer *SingleLanguageCodeDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern wxSizer *pConflictDlgTopSizer;
extern const int ID_TEXTCTRL_READONLY_TOP;
extern const int ID_TEXT_INFO_1;
extern const int ID_TEXT_INFO_2;
extern const int ID_TEXT_INFO_3;
extern const int ID_TEXT_INFO_4;
extern const int ID_CHECKLISTBOX_VERSE_REFS;
extern const int ID_TEXTCTRL_READONLY_SOURCE_TEXT;
extern const int ID_TEXTCTRL_READONLY_AI_VERSION;
extern const int ID_RADIOBUTTON_USE_AI_VERSION;
extern wxSizer *pPT_BoxSizer;
extern const int ID_TEXT_STATIC_PT_VS_TITLE;
extern const int ID_TEXTCTRL_EDITABLE_PT_VERSION;
extern const int ID_RADIOBUTTON_RETAIN_PT_VERSION;
extern const int ID_BUTTON_SELECT_ALL_VS;
extern const int ID_BUTTON_UNSELECT_ALL_VS;
extern wxSizer *pTransferBtnBoxSizerH;
extern const int ID_CHECKBOX_MAKE_SOLIDUS_VISIBLE;
extern const int ID_CHECKBOX_TURN_OFF_CONFRES2;
wxSizer *AI_PT_ConflictingVersesFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern wxSizer *CollabActionDlgSizer;
extern wxSizer *TopStaticSizer;
extern const int ID_RADIOBUTTON_PTorBE_RETAIN;
extern const int ID_TEXTCTRL_TOP;
extern wxSizer *MiddleStaticSizer;
extern const int ID_RADIOBUTTON_FORCE_AI_VERSE_TRANSFER;
extern const int ID_TEXTCTRL_MIDDLE;
extern wxSizer *BottomStaticSizer;
extern const int ID_RADIOBUTTON_USER_CHOICE_FOR_CONFLICT_RESOLUTION;
extern const int ID_TEXTCTRL_BOTTOM;
extern const int ID_CHECKBOX_TURN_OFF_CONFRES;
wxSizer *ConflictResolutionActionFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXTCTRL_TOPMSG;
extern const int ID_BUTTON_MORE_INFORMATION;
extern const int ID_TEXTCTRL_MAIN;
extern const int ID_LISTCTRL_URLS;
extern const int ID_TEXTCTRL_MSG_BOTTOM;
extern const int ID_BUTTON_REMOVE_KBSERVER_SELECTION;
extern const int ID_BUTTON_REMOVE_SEL_ENTRY;
wxSizer *ServDisc_KBserversDlg( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXT_1;
extern const int ID_TEXT_2;
extern const int ID_RADIOBOX_HOW;
wxSizer *kb_ask_how_get_url_func( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXTCTRL_URL_CHOSEN;
extern const int ID_TEXTCTRL_HOSTNAME_CHOSEN;
wxSizer *PasswordDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXT_PREAMBLE;
extern const int ID_RADIOBUTTON_DO_NOT_INSTALL_GIT;
extern const int ID_TEXT_TOP_BTN_DESC;
extern const int ID_RADIOBUTTON_INSTALL_GIT_FROM_INTERNET;
extern const int ID_RADIOBUTTON_BROWSE_FOR_GIT_INSTALLER;
wxSizer *GitInstallOptionsDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXT_TOP;
extern const int ID_TEXT_AIPROJECT;
extern const int ID_TEXT_FREE_TRANS_PROJ;
extern const int ID_RADIOBUTTON_PT8;
extern const int ID_RADIOBUTTON_PT7;
extern const int ID_CHECKBOX_DONT_SHOW_AGAIN;
wxSizer *CollabProjectMigrationDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_TEXT_DIALOG_TOPIC_LINE;
extern const int ID_TEXT_COLLAB_EDITOR;
extern const int ID_TEXT_VERSIFICATION_NAME;
extern const int ID_TEXT_EDITOR_PROJ_SOURCE_NAME;
extern const int ID_TEXT_EDITOR_PROJ_TARGET_NAME;
extern const int ID_TEXT_BY_WHOLE_BOOK_OR_CHAPTER;
extern const int ID_TEXT_STEP2;
extern const int ID_TEXT_STEP3;
extern const int ID_LISTBOX_BOOKS;
extern const int ID_CHECKLISTBOX_BOOKS;
extern const int ID_BUTTON_BOOKS_SELECT_ALL;
extern const int ID_BUTTON_BOOKS_SELECT_OT;
extern const int ID_BUTTON_BOOKS_SELECT_NT;
extern const int ID_BUTTON_BOOKS_SELECT_DC;
extern const int ID_BUTTON_BOOKS_DE_SELECT_ALL;
extern const int ID_CHECKLISTBOX_CHAPTERS;
extern const int ID_BUTTON_CHAPTERS_SELECT_ALL;
extern const int ID_BUTTON_CHAPTERS_DE_SELECT_ALL;
wxSizer *CollabProtectEditorBulkSettingsDlgFunc( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

extern const int ID_PHRASE_BOX_DESIGNER;
extern const int ID_BMTOGGLEBUTTON_PHRASEBOX_DESIGNER;
extern const int ID_DROP_DOWN_LIST_DESIGNER;
wxSizer *PhraseBoxDropDownFunc_UNUSED( wxWindow *parent, bool call_fit = true, bool set_sizer = true );

// Declare menubar functions

extern const int ID_FILE_MENU;
extern const int ID_SAVE_AS;
extern const int ID_FILE_SAVE_COMMIT;
extern const int ID_FILE_PACK_DOC;
extern const int ID_FILE_UNPACK_DOC;
extern const int ID_MENU;
extern const int ID_FILE_STARTUP_WIZARD;
extern const int ID_FILE_CLOSEKB;
extern const int ID_FILE_CHANGEFOLDER;
extern const int ID_FILE_SAVEKB;
extern const int ID_FILE_BACKUP_KB;
extern const int ID_FILE_RESTORE_KB;
extern const int ID_FILE_REVERT_FILE;
extern const int ID_FILE_LIST_VERSIONS;
extern const int ID_FILE_TAKE_OWNERSHIP;
extern const int ID_EDIT_MENU;
extern const int ID_EDIT_CUT;
extern const int ID_EDIT_COPY;
extern const int ID_EDIT_PASTE;
extern const int ID_GO_TO;
extern const int ID_EDIT_SOURCE_TEXT;
extern const int ID_EDIT_CONSISTENCY_CHECK;
extern const int ID_MENU_CHANGE_USERNAME;
extern const int ID_EDITMENU_CHANGE_PUNCTS_MKRS_PLACE;
extern const int ID_EDIT_MOVE_NOTE_FORWARD;
extern const int ID_EDIT_MOVE_NOTE_BACKWARD;
extern const int ID_VIEW_MENU;
extern const int ID_VIEW_TOOLBAR;
extern const int ID_VIEW_STATUS_BAR;
extern const int ID_VIEW_COMPOSE_BAR;
extern const int ID_VIEW_MODE_BAR;
extern const int ID_COPY_SOURCE;
extern const int ID_SELECT_COPIED_SOURCE;
extern const int ID_MARKER_WRAPS_STRIP;
extern const int ID_UNITS;
extern const int ID_CHANGE_INTERFACE_LANGUAGE;
extern const int ID_VIEW_SHOW_ADMIN_MENU;
extern const int ID_TOOLS_MENU;
extern const int ID_TOOLS_CLIPBOARD_ADAPT;
extern const int ID_TOOLS_DEFINE_CC;
extern const int ID_UNLOAD_CC_TABLES;
extern const int ID_USE_CC;
extern const int ID_ACCEPT_CHANGES;
extern const int ID_TOOLS_DEFINE_SILCONVERTER;
extern const int ID_USE_SILCONVERTER;
extern const int ID_TOOLS_KB_EDITOR;
extern const int ID_TOOLS_AUTO_CAPITALIZATION;
extern const int ID_MENU_UPPER_AVAIL;
extern const int ID_RETRANS_REPORT;
extern const int ID_TOOLS_SPLIT_DOC;
extern const int ID_TOOLS_JOIN_DOCS;
extern const int ID_TOOLS_MOVE_DOC;
extern const int ID_TOOLS_INSTALL_GIT;
extern const int ID_EXPORT_IMPORT_MENU;
extern const int ID_FILE_EXPORT_SOURCE;
extern const int ID_FILE_EXPORT;
extern const int ID_FILE_EXPORT_TO_RTF;
extern const int ID_EXPORT_GLOSSES;
extern const int ID_EXPORT_FREE_TRANS;
extern const int ID_FILE_EXPORT_KB;
extern const int ID_IMPORT_TO_KB;
extern const int ID_MENU_IMPORT_EDITED_SOURCE_TEXT;
extern const int ID_ADVANCED_MENU;
extern const int ID_ADVANCED_SEE_GLOSSES;
extern const int ID_ADVANCED_GLOSSING_USES_NAV_FONT;
extern const int ID_ADVANCED_TRANSFORM_ADAPTATIONS_INTO_GLOSSES;
extern const int ID_ADVANCED_DELAY;
extern const int ID_ADVANCED_BOOKMODE;
extern const int ID_ADVANCED_FREE_TRANSLATION_MODE;
extern const int ID_ADVANCED_TARGET_TEXT_IS_DEFAULT;
extern const int ID_ADVANCED_GLOSS_TEXT_IS_DEFAULT;
extern const int ID_ADVANCED_REMOVE_FILTERED_FREE_TRANSLATIONS;
extern const int ID_ADVANCED_COLLECT_BACKTRANSLATIONS;
extern const int ID_ADVANCED_REMOVE_FILTERED_BACKTRANSLATIONS;
extern const int ID_ADVANCED_USETRANSLITERATIONMODE;
extern const int ID_ADVANCED_SENDSYNCHRONIZEDSCROLLINGMESSAGES;
extern const int ID_ADVANCED_RECEIVESYNCHRONIZEDSCROLLINGMESSAGES;
extern const int ID_ADVANCED_PROTECT_EDITOR_FM__GETTING_CHANGES_FOR_THIS_DOC;
extern const int ID_ADVANCED_ALLOW_EDITOR_TO_GET_CHANGES_FOR_THIS_DOC;
extern const int ID_MENU_SHOW_KBSERVER_SETUP_DLG;
extern const int ID_MENU_SHOW_KBSERVER_DLG;
extern const int ID_MENU_DISCOVER_KBSERVERS;
extern const int ID_LAYOUT_MENU;
extern const int ID_ALIGNMENT;
extern const int ID_HELP_MENU;
extern const int ID_MENU_AI_QUICK_START;
extern const int ID_REPORT_A_PROBLEM;
extern const int ID_GIVE_FEEDBACK;
extern const int ID_HELP_USE_TOOLTIPS;
extern const int ID_ADMINISTRATOR_MENU;
extern const int ID_MENU_HELP_FOR_ADMINISTRATORS;
extern const int ID_SETUP_EDITOR_COLLABORATION;
extern const int ID_MENU_MANAGE_DATA_TRANSFER_PROTECTIONS_TO_EDITOR;
extern const int ID_FORCE_VERSE_SECTIONING;
extern const int ID_EDIT_USER_MENU_SETTINGS_PROFILE;
extern const int ID_MENU_TEMP_TURN_OFF_CURRENT_PROFILE;
extern const int ID_ASSIGN_LOCATIONS_FOR_INPUTS_OUTPUTS;
extern const int ID_MOVE_OR_COPY_FOLDERS_OR_FILES;
extern const int ID_SET_PASSWORD_MENU;
extern const int ID_CUSTOM_WORK_FOLDER_LOCATION;
extern const int ID_LOCK_CUSTOM_LOCATION;
extern const int ID_UNLOCK_CUSTOM_LOCATION;
extern const int ID_LOCAL_WORK_FOLDER_MENU;
extern const int ID_DVCS_VERSION;
extern const int ID_MENU_KBSHARINGMGR;
wxMenuBar *AIMenuBarFunc();

// Declare toolbar functions

extern const int ID_BUTTON_GUESSER;
extern const int ID_BUTTON_CREATE_NOTE;
extern const int ID_BUTTON_PREV_NOTE;
extern const int ID_BUTTON_NEXT_NOTE;
extern const int ID_BUTTON_DELETE_ALL_NOTES;
extern const int ID_BUTTON_RESPECTING_BDRY;
extern const int ID_BUTTON_SHOWING_PUNCT;
extern const int ID_BUTTON_TO_END;
extern const int ID_BUTTON_TO_START;
extern const int ID_BUTTON_STEP_DOWN;
extern const int ID_BUTTON_STEP_UP;
extern const int ID_BUTTON_BACK;
extern const int ID_BUTTON_MERGE;
extern const int ID_BUTTON_RETRANSLATION;
extern const int ID_BUTTON_EDIT_RETRANSLATION;
extern const int ID_REMOVE_RETRANSLATION;
extern const int ID_BUTTON_NULL_SRC;
extern const int ID_BUTTON_REMOVE_NULL_SRCPHRASE;
extern const int ID_BUTTON_CHOOSE_TRANSLATION;
extern const int ID_SHOWING_ALL;
extern const int ID_BUTTON_EARLIER_TRANSLATION;
extern const int ID_BUTTON_NO_PUNCT_COPY;
void AIToolBarFunc( wxToolBar *parent );

void AIToolBar32x30Func( wxToolBar *parent );

// Declare bitmap functions

wxBitmap AIToolBarBitmapsUnToggledFunc( size_t index );

extern const int ID_BITMAP_FOLDERAI;
extern const int ID_BITMAP_FILEAI;
extern const int ID_BITMAP_EMPTY_FOLDER;
wxBitmap AIMainFrameIcons( size_t index );

extern const int ID_BUTTON_IGNORING_BDRY;
extern const int ID_BUTTON_HIDING_PUNCT;
extern const int ID_SHOWING_TGT;
extern const int ID_BUTTON_ENABLE_PUNCT_COPY;
wxBitmap AIToolBarBitmapsToggledFunc( size_t index );

wxBitmap WhichFilesBitmapsFunc( size_t index );

wxBitmap AIToolBarBitmapsToggled32x30Func( size_t index );

wxBitmap AIToolBarBitmapsUnToggled32x30Func( size_t index );

wxBitmap HtmlWindowBitmapsFunc( size_t index );

extern const int ID_BITMAP_CHBOX_OFF;
extern const int ID_BITMAP_CHBOX_ON;
wxBitmap BulkKbDeleteFunc( size_t index );

#endif /* __Adapt_It_Resources_H__ */

// End of file
