//------------------------------------------------------------------------------
// Header generated by wxDesigner from file: Adapt_It.wdr
// Do not modify this file, all changes will be lost!
//------------------------------------------------------------------------------

#ifndef __WDR_Adapt_It_H__
#define __WDR_Adapt_It_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "Adapt_It_wdr.h"
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

// Declare window functions

const int ID_STATICBITMAP = -1;
const int ID_TEXT = 0;
const int ID_ABOUT_VERSION_LABEL = 1;
const int ID_ABOUT_VERSION_NUM = 2;
const int ID_ABOUT_VERSION_DATE = 3;
const int ID_STATIC_UNICODE_OR_ANSI = 4;
const int ID_STATIC_WX_VERSION_USED = 5;
const int ID_LINE = 6;
const int ID_STATIC_UI_LANGUAGE = 7;
const int ID_STATIC_HOST_OS = 8;
const int ID_STATIC_SYS_LANGUAGE = 9;
const int ID_STATIC_SYS_LOCALE_NAME = 10;
const int ID_STATIC_CANONICAL_LOCALE_NAME = 11;
const int ID_STATIC_SYS_ENCODING_NAME = 12;
const int ID_STATIC_SYSTEM_LAYOUT_DIR = 13;
wxSizer *AboutDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

extern wxSizer *ID_CONTROLBAR_1_LINE_SIZER;
const int IDC_RADIO_DRAFTING = 14;
const int IDC_RADIO_REVIEWING = 15;
const int IDC_CHECK_SINGLE_STEP = 16;
const int IDC_CHECK_KB_SAVE = 17;
const int IDC_CHECK_FORCE_ASK = 18;
const int IDC_BUTTON_NO_ADAPT = 19;
const int IDC_STATIC = 20;
const int IDC_EDIT_DELAY = 21;
const int IDC_CHECK_ISGLOSSING = 22;
wxSizer *ControlBarFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_EDIT_COMPOSE = 23;
const int IDC_BUTTON_SHORTEN = 24;
const int IDC_BUTTON_PREV = 25;
const int IDC_BUTTON_LENGTHEN = 26;
const int IDC_BUTTON_NEXT = 27;
const int IDC_BUTTON_REMOVE = 28;
const int IDC_BUTTON_APPLY = 29;
const int IDC_STATIC_SECTION_DEF = 30;
const int IDC_RADIO_PUNCT_SECTION = 31;
const int IDC_RADIO_VERSE_SECTION = 32;
const int IDC_BUTTON_CLEAR = 33;
const int IDC_BUTTON_SELECT_ALL = 34;
wxSizer *ComposeBarFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_LISTBOX_ADAPTIONS = 35;
wxSizer *OpenExistingProjectDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_EDIT_FILENAME = 36;
const int ID_TEXT_INVALID_CHARACTERS = 37;
wxSizer *GetOutputFilenameDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_SOURCE_LANGUAGE = 38;
const int ID_EDIT_SOURCE_LANG_CODE = 39;
const int IDC_TARGET_LANGUAGE = 40;
const int ID_EDIT_TARGET_LANG_CODE = 41;
const int ID_BUTTON_LOOKUP_CODES = 42;
wxSizer *LanguagesPageFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_SOURCE_FONTNAME = 43;
const int IDC_CHECK_SRC_RTL = 44;
const int IDC_SOURCE_SIZE = 45;
const int IDC_SOURCE_LANG = 46;
const int IDC_BUTTON_SOURCE_COLOR = 47;
const int ID_BUTTON_CHANGE_SRC_ENCODING = 48;
const int IDC_TARGET_FONTNAME = 49;
const int IDC_CHECK_TGT_RTL = 50;
const int IDC_TARGET_SIZE = 51;
const int IDC_TARGET_LANG = 52;
const int IDC_BUTTON_TARGET_COLOR = 53;
const int ID_BUTTON_CHANGE_TGT_ENCODING = 54;
const int IDC_NAVTEXT_FONTNAME = 55;
const int IDC_CHECK_NAVTEXT_RTL = 56;
const int IDC_NAVTEXT_SIZE = 57;
const int IDC_CHANGE_NAV_TEXT = 58;
const int IDC_BUTTON_NAV_TEXT_COLOR = 59;
const int ID_BUTTON_CHANGE_NAV_ENCODING = 60;
const int ID_TEXTCTRL_AS_STATIC_FONTPAGE = 61;
const int IDC_BUTTON_SPECTEXTCOLOR = 62;
const int IDC_RETRANSLATION_BUTTON = 63;
const int ID_BUTTON_TEXT_DIFFS = 64;
wxSizer *FontsPageFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_PLEASE_WAIT = 65;
wxSizer *WaitDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_LIST_ACCEPTED = 66;
const int IDC_BUTTON_REJECT = 67;
const int IDC_BUTTON_ACCEPT = 68;
const int ID_BUTTON_REJECT_ALL_FILES = 69;
const int ID_BUTTON_ACCEPT_ALL_FILES = 70;
const int IDC_LIST_REJECTED = 71;
wxSizer *WhichFilesDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_TEXTCTRL_TRANSFORM_TO_GLOSSES1 = 72;
const int ID_TEXTCTRL_TRANSFORM_TO_GLOSSES2 = 73;
const int ID_TEXTCTRL_TRANSFORM_TO_GLOSSES3 = 74;
wxSizer *TransformToGlossesDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_CHECK_SOURCE_USES_CAPS = 75;
const int ID_CHECK_USE_AUTO_CAPS = 76;
const int ID_TEXTCTRL_AS_CASE_PAGE_STATIC_TEXT = 77;
const int ID_TEXT_SL = 78;
const int ID_TEXT_TL = 79;
const int ID_TEXT_GL = 80;
const int IDC_EDIT_SRC_CASE_EQUIVALENCES = 81;
const int IDC_EDIT_TGT_CASE_EQUIVALENCES = 82;
const int IDC_EDIT_GLOSS_CASE_EQUIVALENCES = 83;
const int IDC_BUTTON_CLEAR_SRC_LIST = 84;
const int IDC_BUTTON_CLEAR_TGT_LIST = 85;
const int IDC_BUTTON_CLEAR_GLOSS_LIST = 86;
const int IDC_BUTTON_SRC_SET_ENGLISH = 87;
const int IDC_BUTTON_TGT_SET_ENGLISH = 88;
const int IDC_BUTTON_GLOSS_SET_ENGLISH = 89;
const int IDC_BUTTON_SRC_COPY_TO_NEXT = 90;
const int IDC_BUTTON_TGT_COPY_TO_NEXT = 91;
const int IDC_BUTTON_GLOSS_COPY_TO_NEXT = 92;
const int IDC_BUTTON_SRC_COPY_TO_GLOSS = 93;
wxSizer *CaseEquivDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_EDIT_WELCOME_TO = 94;
const int IDC_EDIT_ADAPT_IT = 95;
const int ID_TEXTCTRL_AS_STATIC_WELCOME = 96;
const int IDC_CHECK_NOLONGER_SHOW = 97;
wxSizer *WelcomeDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_TEXTCTRL_AS_STATIC_DOCPAGE = 98;
const int IDC_STATIC_WHICH_MODE = 99;
const int IDC_BUTTON_WHAT_IS_DOC = 100;
const int IDC_STATIC_WHICH_FOLDER = 101;
const int IDC_LIST_NEWDOC_AND_EXISTINGDOC = 102;
const int IDC_CHECK_CHANGE_FIXED_SPACES_TO_REGULAR_SPACES = 103;
const int IDC_CHANGE_FOLDER_BTN = 104;
const int IDC_CHECK_FORCE_UTF8 = 105;
wxSizer *DocPageFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_TEXTCTRL_AS_STATIC_PROJECTPAGE = 106;
const int IDC_BUTTON_WHAT_IS_PROJECT = 107;
const int IDC_LIST_NEW_AND_EXISTING = 108;
wxSizer *ProjectPageFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_EDIT_SRC = 109;
const int IDC_LIST_PUNCTS = 110;
const int ID_TEXTCTRL_AS_STATIC_PLACE_INT_PUNCT = 111;
const int IDC_EDIT_TGT = 112;
const int IDC_BUTTON_PLACE = 113;
wxSizer *PlaceInternalPunctDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_RADIO_INCHES = 114;
const int IDC_RADIO_CM = 115;
wxSizer *UnitsDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_EDIT_MATCHED_SOURCE = 116;
const int IDC_MYLISTBOX_TRANSLATIONS = 117;
const int IDC_EDIT_REFERENCES = 118;
const int IDC_BUTTON_MOVE_UP = 119;
const int IDC_BUTTON_MOVE_DOWN = 120;
const int IDC_BUTTON_CANCEL_ASK = 121;
const int IDC_BUTTON_CANCEL_AND_SELECT = 122;
const int IDC_EDIT_NEW_TRANSLATION = 123;
wxSizer *ChooseTranslationDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_STATIC_SRC = 124;
const int IDC_EDIT_PRECONTEXT = 125;
const int IDC_EDIT_SOURCE_TEXT = 126;
const int IDC_EDIT_RETRANSLATION = 127;
const int IDC_COPY_RETRANSLATION_TO_CLIPBOARD = 128;
const int IDC_BUTTON_TOGGLE = 129;
const int IDC_STATIC_TGT = 130;
const int IDC_EDIT_FOLLCONTEXT = 131;
wxSizer *RetranslationDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_EDIT_SRC_TEXT = 132;
const int IDC_EDIT_TGT_TEXT = 133;
const int IDC_SPIN_CHAPTER = 134;
const int IDC_SPIN_VERSE = 135;
const int IDC_GET_CHVERSE_TEXT = 136;
const int IDC_CLOSE_AND_JUMP = 137;
const int IDC_SHOW_MORE = 138;
const int IDC_SHOW_LESS = 139;
const int IDC_STATIC_BEGIN = 140;
const int IDC_STATIC_END = 141;
wxSizer *EarlierTransDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_CHECK_KB_BACKUP = 142;
const int IDC_CHECK_BAKUP_DOC = 143;
const int ID_TEXTCTRL_AS_STATIC_BACKUPS_AND_KB_PAGE = 144;
const int IDC_EDIT_SRC_NAME = 145;
const int IDC_EDIT_TGT_NAME = 146;
const int IDC_RADIO_ADAPT_BEFORE_GLOSS = 147;
const int IDC_RADIO_GLOSS_BEFORE_ADAPT = 148;
const int IDC_CHECK_LEGACY_SRC_TEXT_COPY = 149;
wxSizer *BackupsAndKBPageFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int STATIC_TEXT_V4 = 150;
const int IDC_EDIT_LEADING = 151;
const int STATIC_TEXT_V5 = 152;
const int IDC_EDIT_GAP_WIDTH = 153;
const int STATIC_TEXT_V6 = 154;
const int IDC_EDIT_LEFTMARGIN = 155;
const int STATIC_TEXT_V7 = 156;
const int IDC_EDIT_MULTIPLIER = 157;
const int IDC_EDIT_DIALOGFONTSIZE = 158;
const int IDC_CHECK_WELCOME_VISIBLE = 159;
const int IDC_CHECK_HIGHLIGHT_AUTO_INSERTED_TRANSLATIONS = 160;
const int ID_PANEL_AUTO_INSERT_COLOR = 161;
const int IDC_BUTTON_CHOOSE_HIGHLIGHT_COLOR = 162;
const int IDC_CHECK_SHOW_ADMIN_MENU = 163;
wxSizer *ViewPageFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

wxSizer *UnitsPageFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_TEXTCTRL_AS_STATIC_AUTOSAVE = 164;
const int IDC_CHECK_NO_AUTOSAVE = 165;
const int IDC_RADIO_BY_MINUTES = 166;
const int IDC_EDIT_MINUTES = 167;
const int IDC_SPIN_MINUTES = 168;
const int IDC_RADIO_BY_MOVES = 169;
const int IDC_EDIT_MOVES = 170;
const int IDC_SPIN_MOVES = 171;
const int IDC_EDIT_KB_MINUTES = 172;
const int IDC_SPIN_KB_MINUTES = 173;
wxSizer *AutoSavingPageFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_RADIO_DIV1 = 174;
const int IDC_RADIO_DIV2 = 175;
const int IDC_RADIO_DIV3 = 176;
const int IDC_RADIO_DIV4 = 177;
const int IDC_RADIO_DIV5 = 178;
const int IDC_COMBO_CHOOSE_BOOK = 179;
const int ID_TEXT_AS_STATIC = 180;
wxSizer *WhichBookDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_STATIC_MSG = 181;
const int IDC_EDIT_ERR_XML = 182;
const int IDC_STATIC_LBL = 183;
const int IDC_EDIT_OFFSET = 184;
wxSizer *XMLErrorDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_ED_SILCONVERTER_NAME = 185;
const int IDC_BTN_SELECT_SILCONVERTER = 186;
const int IDC_ED_SILCONVERTER_INFO = 187;
const int IDC_BTN_CLEAR_SILCONVERTER = 188;
wxSizer *SilConvertersDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_STATIC_FOLDER_DOCS = 189;
const int IDC_LIST_FOLDER_DOCS = 190;
const int IDC_BUTTON_NEXT_CHAPTER = 191;
const int IDC_BUTTON_SPLIT_NOW = 192;
const int IDC_STATIC_SPLITTING_WAIT = 193;
const int IDC_RADIO_PHRASEBOX_LOCATION = 194;
const int IDC_RADIO_CHAPTER_SFMARKER = 195;
const int IDC_RADIO_DIVIDE_INTO_CHAPTERS = 196;
const int IDC_STATIC_SPLIT_NAME = 197;
const int IDC_EDIT1 = 198;
const int IDC_STATIC_REMAIN_NAME = 199;
const int IDC_EDIT2 = 200;
wxSizer *SplitDialogFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_LIST_MARKERS = 201;
const int IDC_TEXTCTRL_AS_STATIC_PLACE_INT_MKRS = 202;
wxSizer *PlaceInternalMarkersDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_EDIT_NOTE = 203;
const int IDC_EDIT_FIND_TEXT = 204;
const int IDC_FIND_NEXT_BTN = 205;
const int IDC_LAST_BTN = 206;
const int IDC_NEXT_BTN = 207;
const int IDC_PREV_BTN = 208;
const int IDC_FIRST_BTN = 209;
const int IDC_DELETE_BTN = 210;
wxSizer *NoteDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_TEXTCTRL_AS_STATIC_COLLECT_BT = 211;
const int IDC_RADIO_COLLECT_ADAPTATIONS = 212;
const int IDC_RADIO_COLLECT_GLOSSES = 213;
wxSizer *CollectBackTranslationsDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_SPIN_DELAY_TICKS = 214;
wxSizer *SetDelayDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_RADIO_EXPORT_ALL = 215;
const int IDC_RADIO_EXPORT_SELECTED_MARKERS = 216;
const int ID_TEXTCTRL_AS_STATIC_EXPORT_OPTIONS = 217;
const int IDC_LIST_SFMS = 218;
const int IDC_BUTTON_EXCLUDE_FROM_EXPORT = 219;
const int IDC_BUTTON_INCLUDE_IN_EXPORT = 220;
const int IDC_BUTTON_UNDO = 221;
const int IDC_STATIC_SPECIAL_NOTE = 222;
const int IDC_CHECK_PLACE_FREE_TRANS = 223;
const int IDC_CHECK_INTERLINEAR_PLACE_FREE_TRANS = 224;
const int IDC_CHECK_PLACE_BACK_TRANS = 225;
const int IDC_CHECK_INTERLINEAR_PLACE_BACK_TRANS = 226;
const int IDC_CHECK_PLACE_AI_NOTES = 227;
const int IDC_CHECK_INTERLINEAR_PLACE_AI_NOTES = 228;
wxSizer *ExportOptionsDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_STATIC_TITLE = 229;
const int IDC_RADIO_EXPORT_AS_SFM = 230;
const int ID_TEXTCTRL_AS_STATIC_EXPORT_SAVE_AS_1 = 231;
const int IDC_RADIO_EXPORT_AS_RTF = 232;
const int ID_TEXTCTRL_AS_STATIC_EXPORT_SAVE_AS_2 = 233;
const int ID_TEXTCTRL_AS_STATIC_EXPORT_SAVE_AS_3 = 234;
const int IDC_BUTTON_EXPORT_FILTER_OPTIONS = 235;
wxSizer *ExportSaveAsDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_CHECK_INCLUDE_NAV_TEXT = 236;
const int IDC_CHECK_INCLUDE_SOURCE_TEXT = 237;
const int IDC_CHECK_INCLUDE_TARGET_TEXT = 238;
const int IDC_CHECK_INCLUDE_GLS_TEXT = 239;
const int ID_TEXTCTRL_AS_STATIC_EXP_INTERLINEAR = 240;
const int IDC_RADIO_PORTRAIT = 241;
const int IDC_RADIO_LANDSCAPE = 242;
const int IDC_RADIO_OUTPUT_ALL = 243;
const int IDC_RADIO_OUTPUT_CHAPTER_VERSE_RANGE = 244;
const int IDC_EDIT_FROM_CHAPTER = 245;
const int IDC_EDIT_FROM_VERSE = 246;
const int IDC_EDIT_TO_CHAPTER = 247;
const int IDC_EDIT_TO_VERSE = 248;
const int IDC_RADIO_OUTPUT_PRELIM = 249;
const int IDC_RADIO_OUTPUT_FINAL = 250;
const int ID_CHECK_NEW_TABLES_FOR_NEWLINE_MARKERS = 251;
const int ID_CHECK_CENTER_TABLES_FOR_CENTERED_MARKERS = 252;
const int IDC_BUTTON_RTF_EXPORT_FILTER_OPTIONS = 253;
wxSizer *ExportInterlinearDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_LIST_MARKER = 254;
const int IDC_EDIT_MARKER_TEXT = 255;
const int IDC_REMOVE_BTN = 256;
const int IDC_BUTTON_SWITCH_ENCODING = 257;
const int IDC_LIST_MARKER_END = 258;
const int IDC_STATIC_MARKER_DESCRIPTION = 259;
const int IDC_STATIC_MARKER_STATUS = 260;
wxSizer *ViewFilteredMaterialDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_EDIT_CHAPTER = 261;
const int IDC_EDIT_VERSE = 262;
wxSizer *GoToDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_EDIT_KEY = 263;
const int IDC_EDIT_CH_VERSE = 264;
const int IDC_EDIT_ADAPTATION = 265;
const int IDC_LIST_TRANSLATIONS = 266;
const int IDC_RADIO_LIST_SELECT = 267;
const int IDC_RADIO_ACCEPT_CURRENT = 268;
const int IDC_RADIO_TYPE_NEW = 269;
const int IDC_EDIT_TYPE_NEW = 270;
const int IDC_NOTHING = 271;
const int IDC_BUTTON_IGNORE_IT = 272;
const int IDC_CHECK_DO_SAME = 273;
wxSizer *ConsistencyCheckDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_RADIO_CHECK_OPEN_DOC_ONLY = 274;
const int IDC_RADIO_CHECK_SELECTED_DOCS = 275;
const int ID_TEXTCTRL_AS_STATIC_CHOOSE_CONSISTENCY_CHECK_TYPE = 276;
const int ID_TEXTCTRL_MSG_TWO = 277;
wxSizer *ChooseConsistencyCheckTypeDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_TEXTCTRL_EDIT_SOURCE_AS_STATIC0 = 278;
const int ID_TEXTCTRL = 279;
const int IDC_EDIT_OLD_SOURCE_TEXT = 280;
const int ID_TEXTCTRL_EDIT_SOURCE_AS_STATIC1 = 281;
const int ID_TEXTCTRL_EDIT_SOURCE_AS_STATIC2 = 282;
const int IDC_EDIT_NEW_SOURCE = 283;
const int ID_TEXTCTRL_EDIT_SOURCE_AS_STATIC4 = 284;
wxSizer *EditSourceTextDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_EDIT_UNPACK_WARN = 285;
wxSizer *UnpackWarningDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_STATIC_COUNT = 286;
const int IDC_LIST_SRC_KEYS = 287;
const int IDC_LIST_EXISTING_TRANSLATIONS = 288;
const int IDC_EDIT_EDITORADD = 289;
const int IDC_BUTTON_UPDATE = 290;
const int IDC_BUTTON_ADD = 291;
const int IDC_ADD_NOTHING = 292;
const int IDC_EDIT_SHOW_FLAG = 293;
const int IDC_BUTTON_FLAG_TOGGLE = 294;
const int IDC_EDIT_SRC_KEY = 295;
const int IDC_EDIT_REF_COUNT = 296;
const int ID_TEXTCTRL_SEARCH = 297;
const int ID_BUTTON_GO = 298;
const int ID_BUTTON_ERASE_ALL_LINES = 299;
const int ID_COMBO_OLD_SEARCHES = 300;
wxSizer *KBEditorPanelFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_STATIC_TEXT_SELECT_A_TAB = 301;
const int ID_STATIC_WHICH_KB = 302;
const int ID_KB_EDITOR_NOTEBOOK = 303;
wxSizer *KBEditorDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_STATIC_DOCS_IN_FOLDER = 304;
const int IDC_LIST_SOURCE_FOLDER_DOCS = 305;
const int IDC_RADIO_TO_BOOK_FOLDER = 306;
const int IDC_RADIO_FROM_BOOK_FOLDER = 307;
const int IDC_VIEW_OTHER = 308;
const int IDC_BUTTON_RENAME_DOC = 309;
const int IDC_MOVE_NOW = 310;
wxSizer *MoveDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_TEXTCTRL_AS_STATIC_JOIN1 = 311;
const int IDC_BUTTON_MOVE_ALL_RIGHT = 312;
const int IDC_BUTTON_MOVE_ALL_LEFT = 313;
const int ID_TEXTCTRL_AS_STATIC_JOIN2 = 314;
const int ID_TEXTCTRL_AS_STATIC_JOIN3 = 315;
const int ID_JOIN_NOW = 316;
const int IDC_STATIC_JOINING_WAIT = 317;
const int ID_TEXTCTRL_AS_STATIC_JOIN4 = 318;
const int IDC_EDIT_NEW_FILENAME = 319;
wxSizer *JoinDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

wxSizer *ListDocInOtherFolderDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_STATIC_SET_FONT_TITLE = 320;
const int ID_STATIC_CURR_ENCODING_IS = 321;
const int ID_TEXT_CURR_ENCODING = 322;
const int ID_LISTBOX_POSSIBLE_FACENAMES = 323;
const int ID_LISTBOX_POSSIBLE_ENCODINGS = 324;
extern wxSizer *pTestBoxStaticTextBoxSizer;
const int ID_TEXT_TEST_ENCODING_BOX = 325;
const int ID_BUTTON_CHART_FONT_SIZE_DECREASE = 326;
const int ID_BUTTON_CHART_FONT_SIZE_INCREASE = 327;
const int ID_STATIC_CHART_FONT_SIZE = 328;
const int ID_SCROLLED_ENCODING_WINDOW = 329;
wxSizer *SetEncodingDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_EDIT_CCT = 330;
wxSizer *CCTableEditDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_LIST_CCTABLES = 331;
const int IDC_EDIT_SELECTED_TABLE = 332;
const int IDC_BUTTON_BROWSE = 333;
const int IDC_BUTTON_CREATE_CCT = 334;
const int IDC_BUTTON_EDIT_CCT = 335;
const int IDC_BUTTON_SELECT_NONE = 336;
const int IDC_EDIT_FOLDER_PATH = 337;
wxSizer *CCTablePageFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_RADIO_SRC_ONLY_FIND = 338;
const int IDC_RADIO_TGT_ONLY_FIND = 339;
const int IDC_RADIO_SRC_AND_TGT_FIND = 340;
const int IDC_CHECK_IGNORE_CASE_FIND = 341;
const int IDC_STATIC_SRC_FIND = 342;
const int IDC_EDIT_SRC_FIND = 343;
const int IDC_STATIC_TGT_FIND = 344;
const int IDC_EDIT_TGT_FIND = 345;
const int IDC_CHECK_INCLUDE_PUNCT_FIND = 346;
const int IDC_CHECK_SPAN_SRC_PHRASES_FIND = 347;
const int IDC_STATIC_SPECIAL = 348;
extern wxSizer *IDC_STATIC_SIZER_SPECIAL;
const int IDC_RADIO_RETRANSLATION = 349;
const int IDC_RADIO_NULL_SRC_PHRASE = 350;
const int IDC_RADIO_SFM = 351;
const int IDC_STATIC_SELECT_MKR = 352;
const int IDC_COMBO_SFM = 353;
const int IDC_BUTTON_SPECIAL = 354;
wxSizer *FindDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_RADIO_SRC_ONLY_REPLACE = 355;
const int IDC_RADIO_TGT_ONLY_REPLACE = 356;
const int IDC_RADIO_SRC_AND_TGT_REPLACE = 357;
const int IDC_CHECK_IGNORE_CASE_REPLACE = 358;
const int IDC_STATIC_SRC_REPLACE = 359;
const int IDC_EDIT_SRC_REPLACE = 360;
const int IDC_STATIC_TGT_REPLACE = 361;
const int IDC_EDIT_TGT_REPLACE = 362;
const int IDC_CHECK_INCLUDE_PUNCT_REPLACE = 363;
const int IDC_CHECK_SPAN_SRC_PHRASES_REPLACE = 364;
const int IDC_STATIC_REPLACE = 365;
const int IDC_EDIT_REPLACE = 366;
const int IDC_REPLACE = 367;
const int IDC_REPLACE_ALL_BUTTON = 368;
wxSizer *ReplaceDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_TEXTCTRL_AS_STATIC = 369;
const int IDC_EDIT_TBLNAME = 370;
wxSizer *CCTableNameDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_TEXTCTRL_EDIT_AS_STATIC = 371;
const int ID_RADIO_ALL = 372;
const int ID_RADIO_SELECTION = 373;
const int IDC_RADIO_PAGES = 374;
const int IDC_EDIT_PAGES_FROM = 375;
const int IDC_EDIT_PAGES_TO = 376;
const int IDC_RADIO_CHAPTER_VERSE_RANGE = 377;
const int IDC_EDIT3 = 378;
const int IDC_EDIT4 = 379;
const int IDC_CHECK_SUPPRESS_PREC_HEADING = 380;
const int IDC_CHECK_INCLUDE_FOLL_HEADING = 381;
const int IDC_CHECK_SUPPRESS_FOOTER = 382;
wxSizer *PrintOptionsDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_CC_TABBED_NOTEBOOK = 383;
wxSizer *CCTabbedNotebookFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_LIST_UI_LANGUAGES = 384;
const int ID_TEXT_AS_STATIC_SHORT_LANG_NAME = 385;
const int ID_TEXT_AS_STATIC_LONG_LANG_NAME = 386;
const int IDC_LOCALIZATION_PATH = 387;
const int IDC_BTN_BROWSE_PATH = 388;
wxSizer *ChooseLanguageDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_COMBO_REMOVALS = 389;
const int ID_STATIC_TEXT_REMOVALS = 390;
const int IDC_BUTTON_UNDO_LAST_COPY = 391;
wxSizer *RemovalsBarFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_EDIT_MSG_TEXT = 392;
const int IDC_BUTTON_PREV_STEP = 393;
const int IDC_BUTTON_NEXT_STEP = 394;
const int ID_BUTTON_END_NOW = 395;
const int ID_BUTTON_CANCEL_ALL_STEPS = 396;
wxSizer *VertEditBarFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_TEXT_TITLE = 397;
const int ID_TEXTCTRL_AS_STATIC_PUNCT_CORRESP_PAGE = 398;
const int IDC_TOGGLE_UNNNN_BTN = 399;
const int ID_NOTEBOOK = 400;
wxSizer *PunctCorrespPageFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_EDIT_SRC0 = 401;
const int IDC_EDIT_TGT0 = 402;
const int IDC_EDIT_SRC9 = 403;
const int IDC_EDIT_TGT9 = 404;
const int IDC_EDIT_SRC18 = 405;
const int IDC_EDIT_TGT18 = 406;
const int IDC_EDIT_SRC1 = 407;
const int IDC_EDIT_TGT1 = 408;
const int IDC_EDIT_SRC10 = 409;
const int IDC_EDIT_TGT10 = 410;
const int IDC_EDIT_SRC19 = 411;
const int IDC_EDIT_TGT19 = 412;
const int IDC_EDIT_SRC2 = 413;
const int IDC_EDIT_TGT2 = 414;
const int IDC_EDIT_SRC11 = 415;
const int IDC_EDIT_TGT11 = 416;
const int IDC_EDIT_SRC20 = 417;
const int IDC_EDIT_TGT20 = 418;
const int IDC_EDIT_SRC3 = 419;
const int IDC_EDIT_TGT3 = 420;
const int IDC_EDIT_SRC12 = 421;
const int IDC_EDIT_TGT12 = 422;
const int IDC_EDIT_SRC21 = 423;
const int IDC_EDIT_TGT21 = 424;
const int IDC_EDIT_SRC4 = 425;
const int IDC_EDIT_TGT4 = 426;
const int IDC_EDIT_SRC13 = 427;
const int IDC_EDIT_TGT13 = 428;
const int IDC_EDIT_SRC22 = 429;
const int IDC_EDIT_TGT22 = 430;
const int IDC_EDIT_SRC5 = 431;
const int IDC_EDIT_TGT5 = 432;
const int IDC_EDIT_SRC14 = 433;
const int IDC_EDIT_TGT14 = 434;
const int IDC_EDIT_SRC23 = 435;
const int IDC_EDIT_TGT23 = 436;
const int IDC_EDIT_SRC6 = 437;
const int IDC_EDIT_TGT6 = 438;
const int IDC_EDIT_SRC15 = 439;
const int IDC_EDIT_TGT15 = 440;
const int IDC_EDIT_SRC24 = 441;
const int IDC_EDIT_TGT24 = 442;
const int IDC_EDIT_SRC7 = 443;
const int IDC_EDIT_TGT7 = 444;
const int IDC_EDIT_SRC16 = 445;
const int IDC_EDIT_TGT16 = 446;
const int IDC_EDIT_SRC25 = 447;
const int IDC_EDIT_TGT25 = 448;
const int IDC_EDIT_SRC8 = 449;
const int IDC_EDIT_TGT8 = 450;
const int IDC_EDIT_SRC17 = 451;
const int IDC_EDIT_TGT17 = 452;
wxSizer *SinglePunctTabPageFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_EDIT_2SRC0 = 453;
const int IDC_EDIT_2TGT0 = 454;
const int IDC_EDIT_2SRC5 = 455;
const int IDC_EDIT_2TGT5 = 456;
const int IDC_EDIT_2SRC1 = 457;
const int IDC_EDIT_2TGT1 = 458;
const int IDC_EDIT_2SRC6 = 459;
const int IDC_EDIT_2TGT6 = 460;
const int IDC_EDIT_2SRC2 = 461;
const int IDC_EDIT_2TGT2 = 462;
const int IDC_EDIT_2SRC7 = 463;
const int IDC_EDIT_2TGT7 = 464;
const int IDC_EDIT_2SRC3 = 465;
const int IDC_EDIT_2TGT3 = 466;
const int IDC_EDIT_2SRC8 = 467;
const int IDC_EDIT_2TGT8 = 468;
const int IDC_EDIT_2SRC4 = 469;
const int IDC_EDIT_2TGT4 = 470;
const int IDC_EDIT_2SRC9 = 471;
const int IDC_EDIT_2TGT9 = 472;
wxSizer *DoublePunctTabPageFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

extern wxSizer *ID_CONTROLBAR_2_LINE_SIZER_TOP;
extern wxSizer *ID_CONTROLBAR_2_LINE_SIZER_BOTTOM;
wxSizer *ControlBar2LineFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_TEXTCTRL_MSG1 = 473;
const int ID_BUTTON_LOCATE_SOURCE_FOLDER = 474;
const int ID_BITMAPBUTTON_SRC_OPEN_FOLDER_UP = 475;
const int ID_TEXT_SOURCE_FOLDER_PATH = 476;
const int ID_TEXTCTRL_SOURCE_PATH = 477;
const int ID_LISTCTRL_SOURCE_CONTENTS = 478;
const int ID_BUTTON_LOCATE_DESTINATION_FOLDER = 479;
const int ID_BITMAPBUTTON_DEST_OPEN_FOLDER_UP = 480;
const int ID_TEXTCTRL_DESTINATION_PATH = 481;
const int ID_LISTCTRL_DESTINATION_CONTENTS = 482;
const int ID_BUTTON_MOVE = 483;
const int ID_BUTTON_COPY = 484;
const int ID_BUTTON_PEEK = 485;
const int ID_BUTTON_RENAME = 486;
const int ID_BUTTON_DELETE = 487;
wxSizer *MoveOrCopyFilesOrFoldersFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_TEXT_MSG1 = 488;
const int ID_TEXTCTRL_SOURCE_FILE_DETAILS = 489;
const int ID_TEXTCTRL_DESTINATION_FILE_DETAILS = 490;
const int ID_RADIOBUTTON_REPLACE = 491;
const int ID_RADIOBUTTON_NO_COPY = 492;
const int ID_RADIOBUTTON_COPY_AND_RENAME = 493;
const int ID_TEXT_MODIFY_NAME = 494;
const int ID_CHECKBOX_HANDLE_SAME = 495;
wxSizer *FilenameConflictFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_LISTBOX_UPDATED = 496;
const int ID_TEXTCTRL_INFO_SOURCE = 497;
const int ID_TEXTCTRL_INFO_REFS = 498;
const int ID_LISTBOX_MATCHED = 499;
const int ID_BUTTON_UPDATE = 500;
const int ID_TEXTCTRL_LOCAL_SEARCH = 501;
const int ID_BUTTON_FIND_NEXT = 502;
const int ID_TEXTCTRL_EDITBOX = 503;
const int ID_BUTTON_RESTORE = 504;
const int ID_BUTTON_REMOVE_UPDATE = 505;
wxSizer *KBEditSearchFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_STATICTEXT_SCROLL_LIST = 506;
const int ID_LIST_LANGUAGE_CODES_NAMES = 507;
const int ID_STATICTEXT_SEARCH_FOR_LANG_NAME = 508;
const int ID_TEXTCTRL_SEARCH_LANG_NAME = 509;
const int ID_BUTTON_USE_SEL_AS_SRC = 510;
const int ID_BUTTON_USE_SEL_AS_TGT = 511;
const int ID_SRC_LANGUAGE_CODE = 512;
const int ID_TEXTCTRL_SRC_LANG_CODE = 513;
const int ID_TGT_LANGUAGE_CODE = 514;
const int ID_TEXTCTRL_TGT_LANG_CODE = 515;
wxSizer *LanguageCodesDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_TEXTCTRL_PEEKMSG = 516;
extern wxSizer *m_pHorizBox_for_textctrl;
const int ID_TEXTCTRL_LINES100 = 517;
const int ID_BUTTON_TOGGLE_TEXT_DIRECTION = 518;
wxSizer *PeekAtFileFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_TEXTCTRL_INSTRUCTIONS = 519;
const int ID_LISTBOX_LOADABLES_FILENAMES = 520;
wxSizer *NewDocFromSourceDataFolderFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_STATIC_SELECT_A_TAB = 521;
const int ID_MENU_EDITOR_NOTEBOOK = 522;
const int ID_RADIOBUTTON_NONE = 523;
const int ID_RADIOBUTTON_USE_PROFILE = 524;
const int ID_COMBO_PROFILE_ITEMS = 525;
const int ID_BUTTON_RESET_TO_FACTORY = 526;
const int ID_TEXT_STATIC_DESCRIPTION = 527;
const int ID_TEXTCTRL_PROFILE_DESCRIPTION = 528;
wxSizer *MenuEditorDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_CHECKLISTBOX_MENU_ITEMS = 529;
wxSizer *MenuEditorPanelFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IDC_RADIO_USE_UBS_SET_ONLY = 530;
const int IDC_RADIO_USE_SILPNG_SET_ONLY = 531;
const int IDC_RADIO_USE_BOTH_SETS = 532;
const int IDC_RADIO_USE_UBS_SET_ONLY_PROJ = 533;
const int IDC_RADIO_USE_SILPNG_SET_ONLY_PROJ = 534;
const int IDC_RADIO_USE_BOTH_SETS_PROJ = 535;
const int IDC_CHECK_CHANGE_FIXED_SPACES_TO_REGULAR_SPACES_USFM = 536;
const int ID_TEXTCTRL_AS_STATIC_FILTERPAGE = 537;
const int IDC_LIST_SFMS_PROJ = 538;
wxSizer *UsfmFilterPageFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_CHECK_USE_GUESSER = 539;
const int ID_PANEL_GUESS_COLOR_DISPLAY = 540;
const int ID_BUTTON_GUESS_HIGHLIGHT_COLOR = 541;
const int ID_SLIDER_GUESSER = 542;
const int ID_CHECK_ALLOW_GUESSER_ON_UNCHANGED_CC_OUTPUT = 543;
const int ID_TEXT_STATIC_NUM_CORRESP_ADAPTATIONS_GUESSER = 544;
const int ID_TEXT_STATIC_NUM_CORRESP_GLOSSING_GUESSER = 545;
wxSizer *GuesserSettingsDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_TEXTCTRL_DEVS_EMAIL_ADDR = 546;
const int ID_TEXTCTRL_MY_EMAIL_ADDR = 547;
const int ID_TEXTCTRL_SUMMARY_SUBJECT = 548;
extern wxSizer *STATIC_TEXT_DESCRIPTION;
const int ID_TEXTCTRL_DESCRIPTION_BODY = 549;
const int ID_TEXTCTRL_SENDERS_NAME = 550;
const int ID_CHECKBOX_LET_DEVS_KNOW_AI_USAGE = 551;
const int ID_BUTTON_VIEW_USAGE_LOG = 552;
const int ID_TEXT_AI_VERSION = 553;
const int ID_TEXT_RELEASE_DATE = 554;
const int ID_TEXT_DATA_TYPE = 555;
const int ID_TEXT_FREE_MEMORY = 556;
const int ID_TEXT_SYS_LOCALE = 557;
const int ID_TEXT_INTERFACE_LANGUAGE = 558;
const int ID_TEXT_SYS_ENCODING = 559;
const int ID_TEXT_SYS_LAYOUT_DIR = 560;
const int ID_TEXT_WXWIDGETS_VERSION = 561;
const int ID_TEXT_OS_VERSION = 562;
const int ID_BUTTON_SAVE_REPORT_AS_TEXT_FILE = 563;
const int ID_BUTTON_LOAD_SAVED_REPORT = 564;
const int ID_RADIOBUTTON_SEND_DIRECTLY_FROM_AI = 565;
const int ID_RADIOBUTTON_SEND_TO_MY_EMAIL = 566;
const int ID_BUTTON_ATTACH_PACKED_DOC = 567;
const int ID_BUTTON_SEND_NOW = 568;
wxSizer *EmailReportDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_TEXT_LOG_FILE_PATH_AND_NAME = 569;
const int ID_TEXTCTRL_LOGGED_TEXT = 570;
wxSizer *LogViewerFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_TEXTCTRL_AS_STATIC_TOP_NOTE = 571;
const int ID_TEXT_STATIC_LIST_OF_PROJECTS = 572;
const int IDC_LIST_OF_COLLAB_PROJECTS = 573;
const int ID_TEXTCTRL_AS_STATIC_IMPORTANT_BOTTOM_NOTE = 574;
const int ID_RADIOBOX_COLLABORATION_ON_OFF = 575;
const int ID_TEXTCTRL_AS_STATIC_SELECTED_SRC_PROJ = 576;
const int ID_BUTTON_SELECT_FROM_LIST_SOURCE_PROJ = 577;
const int ID_TEXTCTRL_AS_STATIC_SELECTED_TGT_PROJ = 578;
const int ID_BUTTON_SELECT_FROM_LIST_TARGET_PROJ = 579;
const int ID_TEXTCTRL_AS_STATIC_SELECTED_FREE_TRANS_PROJ = 580;
const int ID_BUTTON_SELECT_FROM_LIST_FREE_TRANS_PROJ = 581;
wxSizer *SetupEditorCollaborationFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_COMBO_SOURCE_PROJECT_NAME = 582;
const int ID_COMBO_DESTINATION_PROJECT_NAME = 583;
const int ID_COMBO_FREE_TRANS_PROJECT_NAME = 584;
const int ID_BUTTON_NO_FREE_TRANS = 585;
const int ID_RADIOBOX_WHOLE_BOOK_OR_CHAPTER = 586;
const int ID_LISTBOX_BOOK_NAMES = 587;
const int ID_TEXT_SELECT_A_CHAPTER = 588;
const int ID_LISTCTRL_CHAPTER_NUMBER_AND_STATUS = 589;
const int ID_TEXTCTRL_AS_STATIC_NOTE = 590;
wxSizer *GetSourceTextFromEditorDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_TEXTCTRL_SOURCE_TEXT_VERSE = 591;
const int ID_TEXTCTRL_AS_STATIC_INSTRUCTIONS = 592;
const int ID_TEXTCTRL_AS_STATIC_EARLIER_FROM = 593;
const int ID_TEXTCTRL_AS_STATIC_RECENT_FROM = 594;
const int ID_TEXTCTRL_EARLIER_VERSION = 595;
const int ID_TEXTCTRL_RECENT_VERSION = 596;
const int ID_BUTTON_USE_EARLIER = 597;
const int ID_BUTTON_USE_RECENT = 598;
wxSizer *ConflictResolutionDlgFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_TEXTCTRL_AS_STATIC_TOP_INFO = 599;
const int ID_CHECKBOX_PROTECT_SOURCE_INPUTS = 600;
const int ID_BUTTON_PRE_LOAD_SOURCE_TEXTS = 601;
const int ID_CHECKBOX_PROTECT_FREETRANS_OUTPUTS = 602;
const int ID_CHECKBOX_PROTECT_FREETRANS_RTF_OUTPUTS = 603;
const int ID_CHECKBOX_PROTECT_GLOSS_OUTPUTS = 604;
const int ID_CHECKBOX_PROTECT_GLOSS_RTF_OUTPUTS = 605;
const int ID_CHECKBOX_PROTECT_INTERLINEAR_RTF_OUTPUTS = 606;
const int ID_CHECKBOX_PROTECT_SOURCE_OUTPUTS = 607;
const int ID_CHECKBOX_PROTECT_SOURCE_RTF_OUTPUTS = 608;
const int ID_CHECKBOX_PROTECT_TARGET_OUTPUTS = 609;
const int ID_CHECKBOX_PROTECT_TARGET_RTF_OUTPUTS = 610;
const int ID_CHECKBOX_PROTECT_KB_INPUTS_AND_OUTPUTS = 611;
const int ID_CHECKBOX_PROTECT_LIFT_INPUTS_AND_OUTPUTS = 612;
const int ID_CHECKBOX_PROTECT_PACKED_INPUTS_AND_OUTPUTS = 613;
const int ID_BUTTON_SELECT_ALL_CHECKBOXES = 614;
wxSizer *AssignLocationsForInputsOutputsFunc( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

// Declare menubar functions

const int ID_FILE_MENU = 615;
const int ID_SAVE_AS = 616;
const int ID_FILE_PACK_DOC = 617;
const int ID_FILE_UNPACK_DOC = 618;
const int ID_MENU = 619;
const int ID_FILE_STARTUP_WIZARD = 620;
const int ID_FILE_CLOSEKB = 621;
const int ID_FILE_CHANGEFOLDER = 622;
const int ID_FILE_SAVEKB = 623;
const int ID_FILE_BACKUP_KB = 624;
const int ID_FILE_RESTORE_KB = 625;
const int ID_EDIT_MENU = 626;
const int ID_EDIT_CUT = 627;
const int ID_EDIT_COPY = 628;
const int ID_EDIT_PASTE = 629;
const int ID_GO_TO = 630;
const int ID_EDIT_SOURCE_TEXT = 631;
const int ID_EDIT_CONSISTENCY_CHECK = 632;
const int ID_EDIT_MOVE_NOTE_FORWARD = 633;
const int ID_EDIT_MOVE_NOTE_BACKWARD = 634;
const int ID_VIEW_MENU = 635;
const int ID_VIEW_TOOLBAR = 636;
const int ID_VIEW_STATUS_BAR = 637;
const int ID_VIEW_COMPOSE_BAR = 638;
const int ID_COPY_SOURCE = 639;
const int ID_MARKER_WRAPS_STRIP = 640;
const int ID_UNITS = 641;
const int ID_CHANGE_INTERFACE_LANGUAGE = 642;
const int ID_TOOLS_MENU = 643;
const int ID_TOOLS_DEFINE_CC = 644;
const int ID_UNLOAD_CC_TABLES = 645;
const int ID_USE_CC = 646;
const int ID_ACCEPT_CHANGES = 647;
const int ID_TOOLS_DEFINE_SILCONVERTER = 648;
const int ID_USE_SILCONVERTER = 649;
const int ID_TOOLS_KB_EDITOR = 650;
const int ID_TOOLS_AUTO_CAPITALIZATION = 651;
const int ID_RETRANS_REPORT = 652;
const int ID_TOOLS_SPLIT_DOC = 653;
const int ID_TOOLS_JOIN_DOCS = 654;
const int ID_TOOLS_MOVE_DOC = 655;
const int ID_EXPORT_IMPORT_MENU = 656;
const int ID_FILE_EXPORT_SOURCE = 657;
const int ID_FILE_EXPORT = 658;
const int ID_FILE_EXPORT_TO_RTF = 659;
const int ID_EXPORT_GLOSSES = 660;
const int ID_EXPORT_FREE_TRANS = 661;
const int ID_FILE_EXPORT_KB = 662;
const int ID_IMPORT_TO_KB = 663;
const int ID_MENU_IMPORT_EDITED_SOURCE_TEXT = 664;
const int ID_ADVANCED_MENU = 665;
const int ID_ADVANCED_ENABLEGLOSSING = 666;
const int ID_ADVANCED_GLOSSING_USES_NAV_FONT = 667;
const int ID_ADVANCED_TRANSFORM_ADAPTATIONS_INTO_GLOSSES = 668;
const int ID_ADVANCED_DELAY = 669;
const int ID_ADVANCED_BOOKMODE = 670;
const int ID_ADVANCED_FREE_TRANSLATION_MODE = 671;
const int ID_ADVANCED_TARGET_TEXT_IS_DEFAULT = 672;
const int ID_ADVANCED_GLOSS_TEXT_IS_DEFAULT = 673;
const int ID_ADVANCED_REMOVE_FILTERED_FREE_TRANSLATIONS = 674;
const int ID_ADVANCED_COLLECT_BACKTRANSLATIONS = 675;
const int ID_ADVANCED_REMOVE_FILTERED_BACKTRANSLATIONS = 676;
const int ID_ADVANCED_USETRANSLITERATIONMODE = 677;
const int ID_ADVANCED_SENDSYNCHRONIZEDSCROLLINGMESSAGES = 678;
const int ID_ADVANCED_RECEIVESYNCHRONIZEDSCROLLINGMESSAGES = 679;
const int ID_LAYOUT_MENU = 680;
const int ID_ALIGNMENT = 681;
const int ID_HELP_MENU = 682;
const int ID_ONLINE_HELP = 683;
const int ID_REPORT_A_PROBLEM = 684;
const int ID_GIVE_FEEDBACK = 685;
const int ID_HELP_USE_TOOLTIPS = 686;
const int ID_ADMINISTRATOR_MENU = 687;
const int ID_CUSTOM_WORK_FOLDER_LOCATION = 688;
const int ID_LOCK_CUSTOM_LOCATION = 689;
const int ID_UNLOCK_CUSTOM_LOCATION = 690;
const int ID_LOCAL_WORK_FOLDER_MENU = 691;
const int ID_SET_PASSWORD_MENU = 692;
const int ID_SETUP_EDITOR_COLLABORATION = 693;
const int ID_MOVE_OR_COPY_FOLDERS_OR_FILES = 694;
const int ID_ASSIGN_LOCATIONS_FOR_INPUTS_OUTPUTS = 695;
const int ID_EDIT_USER_MENU_SETTINGS_PROFILE = 696;
wxMenuBar *AIMenuBarFunc();

// Declare toolbar functions

const int ID_BUTTON_GUESSER = 697;
const int ID_BUTTON_CREATE_NOTE = 698;
const int ID_BUTTON_PREV_NOTE = 699;
const int ID_BUTTON_NEXT_NOTE = 700;
const int ID_BUTTON_DELETE_ALL_NOTES = 701;
const int ID_BUTTON_RESPECTING_BDRY = 702;
const int ID_BUTTON_SHOWING_PUNCT = 703;
const int ID_BUTTON_TO_END = 704;
const int ID_BUTTON_TO_START = 705;
const int ID_BUTTON_STEP_DOWN = 706;
const int ID_BUTTON_STEP_UP = 707;
const int ID_BUTTON_BACK = 708;
const int ID_BUTTON_MERGE = 709;
const int ID_BUTTON_RETRANSLATION = 710;
const int ID_BUTTON_EDIT_RETRANSLATION = 711;
const int ID_REMOVE_RETRANSLATION = 712;
const int ID_BUTTON_NULL_SRC = 713;
const int ID_BUTTON_REMOVE_NULL_SRCPHRASE = 714;
const int ID_BUTTON_CHOOSE_TRANSLATION = 715;
const int ID_SHOWING_ALL = 716;
const int ID_BUTTON_EARLIER_TRANSLATION = 717;
const int ID_BUTTON_NO_PUNCT_COPY = 718;
void AIToolBarFunc( wxToolBar *parent );

void AIToolBar32x30Func( wxToolBar *parent );

// Declare bitmap functions

wxBitmap AIToolBarBitmapsUnToggledFunc( size_t index );

const int ID_BITMAP_FOLDERAI = 719;
const int ID_BITMAP_FILEAI = 720;
const int ID_BITMAP_EMPTY_FOLDER = 721;
wxBitmap AIMainFrameIcons( size_t index );

const int ID_BUTTON_IGNORING_BDRY = 722;
const int ID_BUTTON_HIDING_PUNCT = 723;
const int ID_SHOWING_TGT = 724;
const int ID_BUTTON_ENABLE_PUNCT_COPY = 725;
wxBitmap AIToolBarBitmapsToggledFunc( size_t index );

wxBitmap WhichFilesBitmapsFunc( size_t index );

wxBitmap AIToolBarBitmapsToggled32x30Func( size_t index );

wxBitmap AIToolBarBitmapsUnToggled32x30Func( size_t index );

#endif

// End of generated file
