#ifndef DARKMODE_H
#define DARKMODE_H

#include <windows.h>

#if (NTDDI_VERSION >= NTDDI_VISTA)

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct DarkMode_Colors
	{
		COLORREF background;
		COLORREF ctrlBackground;
		COLORREF hotBackground;
		COLORREF dlgBackground;
		COLORREF errorBackground;
		COLORREF text;
		COLORREF darkerText;
		COLORREF disabledText;
		COLORREF linkText;
		COLORREF edge;
		COLORREF hotEdge;
		COLORREF disabledEdge;
	} DarkMode_Colors;

	typedef struct DarkMode_ColorsView
	{
		COLORREF background;
		COLORREF text;
		COLORREF gridlines;
		COLORREF headerBackground;
		COLORREF headerHotBackground;
		COLORREF headerText;
		COLORREF headerEdge;
	} DarkMode_ColorsView;

	typedef enum DarkMode_ToolTipsType
	{
		DarkMode_ToolTipsType_tooltip,
		DarkMode_ToolTipsType_toolbar,
		DarkMode_ToolTipsType_listview,
		DarkMode_ToolTipsType_treeview,
		DarkMode_ToolTipsType_tabbar,
		DarkMode_ToolTipsType_trackbar,
		DarkMode_ToolTipsType_rebar
	} DarkMode_ToolTipsType;

	typedef enum DarkMode_ColorTone
	{
		DarkMode_ColorTone_black = 0,
		DarkMode_ColorTone_red = 1,
		DarkMode_ColorTone_green = 2,
		DarkMode_ColorTone_blue = 3,
		DarkMode_ColorTone_purple = 4,
		DarkMode_ColorTone_cyan = 5,
		DarkMode_ColorTone_olive = 6,
		DarkMode_ColorTone_max = 7
	} DarkMode_ColorTone;

	typedef enum DarkMode_TreeViewStyle
	{
		DarkMode_TreeViewStyle_classic,
		DarkMode_TreeViewStyle_light,
		DarkMode_TreeViewStyle_dark
	} DarkMode_TreeViewStyle;

	typedef enum DarkMode_LibInfo
	{
		DarkMode_LibInfo_featureCheck,
		DarkMode_LibInfo_verMajor,
		DarkMode_LibInfo_verMinor,
		DarkMode_LibInfo_verRevision,
		DarkMode_LibInfo_iathookExternal,
		DarkMode_LibInfo_iniConfigUsed,
		DarkMode_LibInfo_allowOldOS,
		DarkMode_LibInfo_useDlgProcCtl,
		DarkMode_LibInfo_preferTheme,
		DarkMode_LibInfo_useSBFix,
		DarkMode_LibInfo_maxValue
	} DarkMode_LibInfo;

	typedef enum DarkMode_DarkModeType
	{
		DarkMode_DarkModeType_light = 0,
		DarkMode_DarkModeType_dark = 1,
		DarkMode_DarkModeType_classic = 3
	} DarkMode_DarkModeType;

	extern HMODULE g_hDarkModeLib;

	typedef int(WINAPI *PFN_DarkMode_getLibInfo)(DarkMode_LibInfo libInfoType);

	typedef void(WINAPI *PFN_DarkMode_initDarkModeConfig)(UINT dmType);
	typedef void(WINAPI *PFN_DarkMode_setRoundCornerConfig)(UINT roundCornerStyle);
	typedef void(WINAPI *PFN_DarkMode_setBorderColorConfig)(COLORREF clr);
	typedef void(WINAPI *PFN_DarkMode_setMicaConfig)(UINT mica);
	typedef void(WINAPI *PFN_DarkMode_setMicaExtendedConfig)(BOOL extendMica);
	typedef void(WINAPI *PFN_DarkMode_setColorizeTitleBarConfig)(BOOL colorize);
	typedef void(WINAPI *PFN_DarkMode_setDarkModeConfig)(UINT dmType);
	typedef void(WINAPI *PFN_DarkMode_setDarkModeConfig_System)(void);
	typedef void(WINAPI *PFN_DarkMode_initDarkMode)(const wchar_t *iniName);
	typedef void(WINAPI *PFN_DarkMode_initDarkMode_NoIni)(void);

	typedef BOOL(WINAPI *PFN_DarkMode_isEnabled)(void);
	typedef BOOL(WINAPI *PFN_DarkMode_isExperimentalActive)(void);
	typedef BOOL(WINAPI *PFN_DarkMode_isExperimentalSupported)(void);
	typedef BOOL(WINAPI *PFN_DarkMode_isWindowsModeEnabled)(void);
	typedef BOOL(WINAPI *PFN_DarkMode_isAtLeastWindows10)(void);
	typedef BOOL(WINAPI *PFN_DarkMode_isAtLeastWindows11)(void);
	typedef DWORD(WINAPI *PFN_DarkMode_getWindowsBuildNumber)(void);

	typedef BOOL(WINAPI *PFN_DarkMode_handleSettingChange)(LPARAM lParam);
	typedef BOOL(WINAPI *PFN_DarkMode_isDarkModeReg)(void);

	typedef void(WINAPI *PFN_DarkMode_setSysColor)(int nIndex, COLORREF color);

	typedef void(WINAPI *PFN_DarkMode_enableDarkScrollBarForWindowAndChildren)(HWND hWnd);

	typedef void(WINAPI *PFN_DarkMode_setColorTone)(DarkMode_ColorTone colorTone);
	typedef DarkMode_ColorTone(WINAPI *PFN_DarkMode_getColorTone)(void);
	typedef COLORREF(WINAPI *PFN_DarkMode_setBackgroundColor)(COLORREF clrNew);
	typedef COLORREF(WINAPI *PFN_DarkMode_setCtrlBackgroundColor)(COLORREF clrNew);
	typedef COLORREF(WINAPI *PFN_DarkMode_setHotBackgroundColor)(COLORREF clrNew);
	typedef COLORREF(WINAPI *PFN_DarkMode_setDlgBackgroundColor)(COLORREF clrNew);
	typedef COLORREF(WINAPI *PFN_DarkMode_setErrorBackgroundColor)(COLORREF clrNew);
	typedef COLORREF(WINAPI *PFN_DarkMode_setTextColor)(COLORREF clrNew);
	typedef COLORREF(WINAPI *PFN_DarkMode_setDarkerTextColor)(COLORREF clrNew);
	typedef COLORREF(WINAPI *PFN_DarkMode_setDisabledTextColor)(COLORREF clrNew);
	typedef COLORREF(WINAPI *PFN_DarkMode_setLinkTextColor)(COLORREF clrNew);
	typedef COLORREF(WINAPI *PFN_DarkMode_setEdgeColor)(COLORREF clrNew);
	typedef COLORREF(WINAPI *PFN_DarkMode_setHotEdgeColor)(COLORREF clrNew);
	typedef COLORREF(WINAPI *PFN_DarkMode_setDisabledEdgeColor)(COLORREF clrNew);
	typedef void(WINAPI *PFN_DarkMode_setThemeColors)(DarkMode_Colors colors);
	typedef void(WINAPI *PFN_DarkMode_updateThemeBrushesAndPens)(void);
	typedef COLORREF(WINAPI *PFN_DarkMode_getBackgroundColor)(void);
	typedef COLORREF(WINAPI *PFN_DarkMode_getCtrlBackgroundColor)(void);
	typedef COLORREF(WINAPI *PFN_DarkMode_getHotBackgroundColor)(void);
	typedef COLORREF(WINAPI *PFN_DarkMode_getDlgBackgroundColor)(void);
	typedef COLORREF(WINAPI *PFN_DarkMode_getErrorBackgroundColor)(void);
	typedef COLORREF(WINAPI *PFN_DarkMode_getTextColor)(void);
	typedef COLORREF(WINAPI *PFN_DarkMode_getDarkerTextColor)(void);
	typedef COLORREF(WINAPI *PFN_DarkMode_getDisabledTextColor)(void);
	typedef COLORREF(WINAPI *PFN_DarkMode_getLinkTextColor)(void);
	typedef COLORREF(WINAPI *PFN_DarkMode_getEdgeColor)(void);
	typedef COLORREF(WINAPI *PFN_DarkMode_getHotEdgeColor)(void);
	typedef COLORREF(WINAPI *PFN_DarkMode_getDisabledEdgeColor)(void);
	typedef HBRUSH(WINAPI *PFN_DarkMode_getBackgroundBrush)(void);
	typedef HBRUSH(WINAPI *PFN_DarkMode_getDlgBackgroundBrush)(void);
	typedef HBRUSH(WINAPI *PFN_DarkMode_getCtrlBackgroundBrush)(void);
	typedef HBRUSH(WINAPI *PFN_DarkMode_getHotBackgroundBrush)(void);
	typedef HBRUSH(WINAPI *PFN_DarkMode_getErrorBackgroundBrush)(void);
	typedef HBRUSH(WINAPI *PFN_DarkMode_getEdgeBrush)(void);
	typedef HBRUSH(WINAPI *PFN_DarkMode_getHotEdgeBrush)(void);
	typedef HBRUSH(WINAPI *PFN_DarkMode_getDisabledEdgeBrush)(void);
	typedef HPEN(WINAPI *PFN_DarkMode_getDarkerTextPen)(void);
	typedef HPEN(WINAPI *PFN_DarkMode_getEdgePen)(void);
	typedef HPEN(WINAPI *PFN_DarkMode_getHotEdgePen)(void);
	typedef HPEN(WINAPI *PFN_DarkMode_getDisabledEdgePen)(void);
	typedef COLORREF(WINAPI *PFN_DarkMode_setViewBackgroundColor)(COLORREF clrNew);
	typedef COLORREF(WINAPI *PFN_DarkMode_setViewTextColor)(COLORREF clrNew);
	typedef COLORREF(WINAPI *PFN_DarkMode_setViewGridlinesColor)(COLORREF clrNew);
	typedef COLORREF(WINAPI *PFN_DarkMode_setHeaderBackgroundColor)(COLORREF clrNew);
	typedef COLORREF(WINAPI *PFN_DarkMode_setHeaderHotBackgroundColor)(COLORREF clrNew);
	typedef COLORREF(WINAPI *PFN_DarkMode_setHeaderTextColor)(COLORREF clrNew);
	typedef COLORREF(WINAPI *PFN_DarkMode_setHeaderEdgeColor)(COLORREF clrNew);
	typedef void(WINAPI *PFN_DarkMode_setViewColors)(DarkMode_ColorsView colors);
	typedef void(WINAPI *PFN_DarkMode_updateViewBrushesAndPens)(void);
	typedef COLORREF(WINAPI *PFN_DarkMode_getViewBackgroundColor)(void);
	typedef COLORREF(WINAPI *PFN_DarkMode_getViewTextColor)(void);
	typedef COLORREF(WINAPI *PFN_DarkMode_getViewGridlinesColor)(void);
	typedef COLORREF(WINAPI *PFN_DarkMode_getHeaderBackgroundColor)(void);
	typedef COLORREF(WINAPI *PFN_DarkMode_getHeaderHotBackgroundColor)(void);
	typedef COLORREF(WINAPI *PFN_DarkMode_getHeaderTextColor)(void);
	typedef COLORREF(WINAPI *PFN_DarkMode_getHeaderEdgeColor)(void);
	typedef HBRUSH(WINAPI *PFN_DarkMode_getViewBackgroundBrush)(void);
	typedef HBRUSH(WINAPI *PFN_DarkMode_getViewGridlinesBrush)(void);
	typedef HBRUSH(WINAPI *PFN_DarkMode_getHeaderBackgroundBrush)(void);
	typedef HBRUSH(WINAPI *PFN_DarkMode_getHeaderHotBackgroundBrush)(void);
	typedef HPEN(WINAPI *PFN_DarkMode_getHeaderEdgePen)(void);
	typedef void(WINAPI *PFN_DarkMode_setDefaultColors)(BOOL updateBrushesAndOther);

	typedef void(WINAPI *PFN_DarkMode_paintRoundRect)(HDC hdc, const RECT *rect, HPEN hpen, HBRUSH hBrush, int width, int height);
	typedef void(WINAPI *PFN_DarkMode_paintRoundFrameRect)(HDC hdc, const RECT *rect, HPEN hpen, int width, int height);

	typedef void(WINAPI *PFN_DarkMode_setCheckboxOrRadioBtnCtrlSubclass)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_removeCheckboxOrRadioBtnCtrlSubclass)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_setGroupboxCtrlSubclass)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_removeGroupboxCtrlSubclass)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_setUpDownCtrlSubclass)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_removeUpDownCtrlSubclass)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_setTabCtrlUpDownSubclass)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_removeTabCtrlUpDownSubclass)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_setTabCtrlSubclass)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_removeTabCtrlSubclass)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_setCustomBorderForListBoxOrEditCtrlSubclass)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_removeCustomBorderForListBoxOrEditCtrlSubclass)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_setComboBoxCtrlSubclass)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_removeComboBoxCtrlSubclass)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_setComboBoxExCtrlSubclass)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_removeComboBoxExCtrlSubclass)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_setListViewCtrlSubclass)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_removeListViewCtrlSubclass)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_setHeaderCtrlSubclass)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_removeHeaderCtrlSubclass)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_setStatusBarCtrlSubclass)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_removeStatusBarCtrlSubclass)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_setProgressBarCtrlSubclass)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_removeProgressBarCtrlSubclass)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_setStaticTextCtrlSubclass)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_removeStaticTextCtrlSubclass)(HWND hWnd);

	typedef void(WINAPI *PFN_DarkMode_setChildCtrlsSubclassAndTheme)(HWND hParent, BOOL subclass, BOOL theme);
	typedef void(WINAPI *PFN_DarkMode_setChildCtrlsTheme)(HWND hParent);

	typedef void(WINAPI *PFN_DarkMode_setWindowEraseBgSubclass)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_removeWindowEraseBgSubclass)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_setWindowCtlColorSubclass)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_removeWindowCtlColorSubclass)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_setWindowNotifyCustomDrawSubclass)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_removeWindowNotifyCustomDrawSubclass)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_setWindowMenuBarSubclass)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_removeWindowMenuBarSubclass)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_setWindowSettingChangeSubclass)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_removeWindowSettingChangeSubclass)(HWND hWnd);

	typedef void(WINAPI *PFN_DarkMode_enableSysLinkCtrlCtlColor)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_setDarkTitleBarEx)(HWND hWnd, BOOL useWin11Features);
	typedef void(WINAPI *PFN_DarkMode_setDarkTitleBar)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_setDarkThemeExperimental)(HWND hWnd, const wchar_t *themeClassName);
	typedef void(WINAPI *PFN_DarkMode_setDarkExplorerTheme)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_setDarkScrollBar)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_setDarkTooltips)(HWND hWnd, DarkMode_ToolTipsType type);
	typedef void(WINAPI *PFN_DarkMode_setDarkLineAbovePanelToolbar)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_setDarkListView)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_setDarkListViewCheckboxes)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_setDarkRichEdit)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_setDarkWndSafe)(HWND hWnd, BOOL useWin11Features);
	typedef void(WINAPI *PFN_DarkMode_setDarkWndNotifySafeEx)(HWND hWnd, BOOL setSettingChangeSubclass, BOOL useWin11Features);
	typedef void(WINAPI *PFN_DarkMode_setDarkWndNotifySafe)(HWND hWnd, BOOL useWin11Features);
	typedef void(WINAPI *PFN_DarkMode_enableThemeDialogTexture)(HWND hWnd, BOOL theme);
	typedef void(WINAPI *PFN_DarkMode_disableVisualStyle)(HWND hWnd, BOOL doDisable);
	typedef double(WINAPI *PFN_DarkMode_calculatePerceivedLightness)(COLORREF clr);
	typedef DarkMode_TreeViewStyle(WINAPI *PFN_DarkMode_getTreeViewStyle)(void);
	typedef void(WINAPI *PFN_DarkMode_calculateTreeViewStyle)(void);
	typedef void(WINAPI *PFN_DarkMode_setTreeViewWindowTheme)(HWND hWnd, BOOL force);
	typedef DarkMode_TreeViewStyle(WINAPI *PFN_DarkMode_getPrevTreeViewStyle)(void);
	typedef void(WINAPI *PFN_DarkMode_setPrevTreeViewStyle)(void);
	typedef BOOL(WINAPI *PFN_DarkMode_isThemeDark)(void);
	typedef BOOL(WINAPI *PFN_DarkMode_isColorDark)(COLORREF clr);
	typedef void(WINAPI *PFN_DarkMode_redrawWindowFrame)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_setWindowStyle)(HWND hWnd, BOOL setStyle, LONG_PTR styleFlag);
	typedef void(WINAPI *PFN_DarkMode_setWindowExStyle)(HWND hWnd, BOOL setExStyle, LONG_PTR exStyleFlag);
	typedef void(WINAPI *PFN_DarkMode_replaceExEdgeWithBorder)(HWND hWnd, BOOL replace, LONG_PTR exStyleFlag);
	typedef void(WINAPI *PFN_DarkMode_replaceClientEdgeWithBorderSafe)(HWND hWnd);
	typedef void(WINAPI *PFN_DarkMode_setProgressBarClassicTheme)(HWND hWnd);

	typedef LRESULT(WINAPI *PFN_DarkMode_onCtlColor)(HDC hdc);
	typedef LRESULT(WINAPI *PFN_DarkMode_onCtlColorCtrl)(HDC hdc);
	typedef LRESULT(WINAPI *PFN_DarkMode_onCtlColorDlg)(HDC hdc);
	typedef LRESULT(WINAPI *PFN_DarkMode_onCtlColorError)(HDC hdc);
	typedef LRESULT(WINAPI *PFN_DarkMode_onCtlColorDlgStaticText)(HDC hdc, BOOL isTextEnabled);
	typedef LRESULT(WINAPI *PFN_DarkMode_onCtlColorDlgLinkText)(HDC hdc, BOOL isTextEnabled);
	typedef LRESULT(WINAPI *PFN_DarkMode_onCtlColorListbox)(WPARAM wParam, LPARAM lParam);

	typedef UINT_PTR(CALLBACK *PFN_DarkMode_HookDlgProc)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	extern PFN_DarkMode_getLibInfo _DarkMode_getLibInfo;
	extern PFN_DarkMode_initDarkModeConfig _DarkMode_initDarkModeConfig;
	extern PFN_DarkMode_setRoundCornerConfig _DarkMode_setRoundCornerConfig;
	extern PFN_DarkMode_setBorderColorConfig _DarkMode_setBorderColorConfig;
	extern PFN_DarkMode_setMicaConfig _DarkMode_setMicaConfig;
	extern PFN_DarkMode_setMicaExtendedConfig _DarkMode_setMicaExtendedConfig;
	extern PFN_DarkMode_setColorizeTitleBarConfig _DarkMode_setColorizeTitleBarConfig;
	extern PFN_DarkMode_setDarkModeConfig _DarkMode_setDarkModeConfig;
	extern PFN_DarkMode_setDarkModeConfig_System _DarkMode_setDarkModeConfig_System;
	extern PFN_DarkMode_initDarkMode _DarkMode_initDarkMode;
	extern PFN_DarkMode_initDarkMode_NoIni _DarkMode_initDarkMode_NoIni;
	extern PFN_DarkMode_isEnabled _DarkMode_isEnabled;
	extern PFN_DarkMode_isExperimentalActive _DarkMode_isExperimentalActive;
	extern PFN_DarkMode_isExperimentalSupported _DarkMode_isExperimentalSupported;
	extern PFN_DarkMode_isWindowsModeEnabled _DarkMode_isWindowsModeEnabled;
	extern PFN_DarkMode_isAtLeastWindows10 _DarkMode_isAtLeastWindows10;
	extern PFN_DarkMode_isAtLeastWindows11 _DarkMode_isAtLeastWindows11;
	extern PFN_DarkMode_getWindowsBuildNumber _DarkMode_getWindowsBuildNumber;
	extern PFN_DarkMode_handleSettingChange _DarkMode_handleSettingChange;
	extern PFN_DarkMode_isDarkModeReg _DarkMode_isDarkModeReg;
	extern PFN_DarkMode_setSysColor _DarkMode_setSysColor;
	extern PFN_DarkMode_enableDarkScrollBarForWindowAndChildren _DarkMode_enableDarkScrollBarForWindowAndChildren;
	extern PFN_DarkMode_setColorTone _DarkMode_setColorTone;
	extern PFN_DarkMode_getColorTone _DarkMode_getColorTone;
	extern PFN_DarkMode_setBackgroundColor _DarkMode_setBackgroundColor;
	extern PFN_DarkMode_setCtrlBackgroundColor _DarkMode_setCtrlBackgroundColor;
	extern PFN_DarkMode_setHotBackgroundColor _DarkMode_setHotBackgroundColor;
	extern PFN_DarkMode_setDlgBackgroundColor _DarkMode_setDlgBackgroundColor;
	extern PFN_DarkMode_setErrorBackgroundColor _DarkMode_setErrorBackgroundColor;
	extern PFN_DarkMode_setTextColor _DarkMode_setTextColor;
	extern PFN_DarkMode_setDarkerTextColor _DarkMode_setDarkerTextColor;
	extern PFN_DarkMode_setDisabledTextColor _DarkMode_setDisabledTextColor;
	extern PFN_DarkMode_setLinkTextColor _DarkMode_setLinkTextColor;
	extern PFN_DarkMode_setEdgeColor _DarkMode_setEdgeColor;
	extern PFN_DarkMode_setHotEdgeColor _DarkMode_setHotEdgeColor;
	extern PFN_DarkMode_setDisabledEdgeColor _DarkMode_setDisabledEdgeColor;
	extern PFN_DarkMode_setThemeColors _DarkMode_setThemeColors;
	extern PFN_DarkMode_updateThemeBrushesAndPens _DarkMode_updateThemeBrushesAndPens;
	extern PFN_DarkMode_getBackgroundColor _DarkMode_getBackgroundColor;
	extern PFN_DarkMode_getCtrlBackgroundColor _DarkMode_getCtrlBackgroundColor;
	extern PFN_DarkMode_getHotBackgroundColor _DarkMode_getHotBackgroundColor;
	extern PFN_DarkMode_getDlgBackgroundColor _DarkMode_getDlgBackgroundColor;
	extern PFN_DarkMode_getErrorBackgroundColor _DarkMode_getErrorBackgroundColor;
	extern PFN_DarkMode_getTextColor _DarkMode_getTextColor;
	extern PFN_DarkMode_getDarkerTextColor _DarkMode_getDarkerTextColor;
	extern PFN_DarkMode_getDisabledTextColor _DarkMode_getDisabledTextColor;
	extern PFN_DarkMode_getLinkTextColor _DarkMode_getLinkTextColor;
	extern PFN_DarkMode_getEdgeColor _DarkMode_getEdgeColor;
	extern PFN_DarkMode_getHotEdgeColor _DarkMode_getHotEdgeColor;
	extern PFN_DarkMode_getDisabledEdgeColor _DarkMode_getDisabledEdgeColor;
	extern PFN_DarkMode_getBackgroundBrush _DarkMode_getBackgroundBrush;
	extern PFN_DarkMode_getDlgBackgroundBrush _DarkMode_getDlgBackgroundBrush;
	extern PFN_DarkMode_getCtrlBackgroundBrush _DarkMode_getCtrlBackgroundBrush;
	extern PFN_DarkMode_getHotBackgroundBrush _DarkMode_getHotBackgroundBrush;
	extern PFN_DarkMode_getErrorBackgroundBrush _DarkMode_getErrorBackgroundBrush;
	extern PFN_DarkMode_getEdgeBrush _DarkMode_getEdgeBrush;
	extern PFN_DarkMode_getHotEdgeBrush _DarkMode_getHotEdgeBrush;
	extern PFN_DarkMode_getDisabledEdgeBrush _DarkMode_getDisabledEdgeBrush;
	extern PFN_DarkMode_getDarkerTextPen _DarkMode_getDarkerTextPen;
	extern PFN_DarkMode_getEdgePen _DarkMode_getEdgePen;
	extern PFN_DarkMode_getHotEdgePen _DarkMode_getHotEdgePen;
	extern PFN_DarkMode_getDisabledEdgePen _DarkMode_getDisabledEdgePen;
	extern PFN_DarkMode_setViewBackgroundColor _DarkMode_setViewBackgroundColor;
	extern PFN_DarkMode_setViewTextColor _DarkMode_setViewTextColor;
	extern PFN_DarkMode_setViewGridlinesColor _DarkMode_setViewGridlinesColor;
	extern PFN_DarkMode_setHeaderBackgroundColor _DarkMode_setHeaderBackgroundColor;
	extern PFN_DarkMode_setHeaderHotBackgroundColor _DarkMode_setHeaderHotBackgroundColor;
	extern PFN_DarkMode_setHeaderTextColor _DarkMode_setHeaderTextColor;
	extern PFN_DarkMode_setHeaderEdgeColor _DarkMode_setHeaderEdgeColor;
	extern PFN_DarkMode_setViewColors _DarkMode_setViewColors;
	extern PFN_DarkMode_updateViewBrushesAndPens _DarkMode_updateViewBrushesAndPens;
	extern PFN_DarkMode_getViewBackgroundColor _DarkMode_getViewBackgroundColor;
	extern PFN_DarkMode_getViewTextColor _DarkMode_getViewTextColor;
	extern PFN_DarkMode_getViewGridlinesColor _DarkMode_getViewGridlinesColor;
	extern PFN_DarkMode_getHeaderBackgroundColor _DarkMode_getHeaderBackgroundColor;
	extern PFN_DarkMode_getHeaderHotBackgroundColor _DarkMode_getHeaderHotBackgroundColor;
	extern PFN_DarkMode_getHeaderTextColor _DarkMode_getHeaderTextColor;
	extern PFN_DarkMode_getHeaderEdgeColor _DarkMode_getHeaderEdgeColor;
	extern PFN_DarkMode_getViewBackgroundBrush _DarkMode_getViewBackgroundBrush;
	extern PFN_DarkMode_getViewGridlinesBrush _DarkMode_getViewGridlinesBrush;
	extern PFN_DarkMode_getHeaderBackgroundBrush _DarkMode_getHeaderBackgroundBrush;
	extern PFN_DarkMode_getHeaderHotBackgroundBrush _DarkMode_getHeaderHotBackgroundBrush;
	extern PFN_DarkMode_getHeaderEdgePen _DarkMode_getHeaderEdgePen;
	extern PFN_DarkMode_setDefaultColors _DarkMode_setDefaultColors;
	extern PFN_DarkMode_paintRoundRect _DarkMode_paintRoundRect;
	extern PFN_DarkMode_paintRoundFrameRect _DarkMode_paintRoundFrameRect;
	extern PFN_DarkMode_setCheckboxOrRadioBtnCtrlSubclass _DarkMode_setCheckboxOrRadioBtnCtrlSubclass;
	extern PFN_DarkMode_removeCheckboxOrRadioBtnCtrlSubclass _DarkMode_removeCheckboxOrRadioBtnCtrlSubclass;
	extern PFN_DarkMode_setGroupboxCtrlSubclass _DarkMode_setGroupboxCtrlSubclass;
	extern PFN_DarkMode_removeGroupboxCtrlSubclass _DarkMode_removeGroupboxCtrlSubclass;
	extern PFN_DarkMode_setUpDownCtrlSubclass _DarkMode_setUpDownCtrlSubclass;
	extern PFN_DarkMode_removeUpDownCtrlSubclass _DarkMode_removeUpDownCtrlSubclass;
	extern PFN_DarkMode_setTabCtrlUpDownSubclass _DarkMode_setTabCtrlUpDownSubclass;
	extern PFN_DarkMode_removeTabCtrlUpDownSubclass _DarkMode_removeTabCtrlUpDownSubclass;
	extern PFN_DarkMode_setTabCtrlSubclass _DarkMode_setTabCtrlSubclass;
	extern PFN_DarkMode_removeTabCtrlSubclass _DarkMode_removeTabCtrlSubclass;
	extern PFN_DarkMode_setCustomBorderForListBoxOrEditCtrlSubclass _DarkMode_setCustomBorderForListBoxOrEditCtrlSubclass;
	extern PFN_DarkMode_removeCustomBorderForListBoxOrEditCtrlSubclass _DarkMode_removeCustomBorderForListBoxOrEditCtrlSubclass;
	extern PFN_DarkMode_setComboBoxCtrlSubclass _DarkMode_setComboBoxCtrlSubclass;
	extern PFN_DarkMode_removeComboBoxCtrlSubclass _DarkMode_removeComboBoxCtrlSubclass;
	extern PFN_DarkMode_setComboBoxExCtrlSubclass _DarkMode_setComboBoxExCtrlSubclass;
	extern PFN_DarkMode_removeComboBoxExCtrlSubclass _DarkMode_removeComboBoxExCtrlSubclass;
	extern PFN_DarkMode_setListViewCtrlSubclass _DarkMode_setListViewCtrlSubclass;
	extern PFN_DarkMode_removeListViewCtrlSubclass _DarkMode_removeListViewCtrlSubclass;
	extern PFN_DarkMode_setHeaderCtrlSubclass _DarkMode_setHeaderCtrlSubclass;
	extern PFN_DarkMode_removeHeaderCtrlSubclass _DarkMode_removeHeaderCtrlSubclass;
	extern PFN_DarkMode_setStatusBarCtrlSubclass _DarkMode_setStatusBarCtrlSubclass;
	extern PFN_DarkMode_removeStatusBarCtrlSubclass _DarkMode_removeStatusBarCtrlSubclass;
	extern PFN_DarkMode_setProgressBarCtrlSubclass _DarkMode_setProgressBarCtrlSubclass;
	extern PFN_DarkMode_removeProgressBarCtrlSubclass _DarkMode_removeProgressBarCtrlSubclass;
	extern PFN_DarkMode_setStaticTextCtrlSubclass _DarkMode_setStaticTextCtrlSubclass;
	extern PFN_DarkMode_removeStaticTextCtrlSubclass _DarkMode_removeStaticTextCtrlSubclass;
	extern PFN_DarkMode_setChildCtrlsSubclassAndTheme _DarkMode_setChildCtrlsSubclassAndTheme;
	extern PFN_DarkMode_setChildCtrlsTheme _DarkMode_setChildCtrlsTheme;
	extern PFN_DarkMode_setWindowEraseBgSubclass _DarkMode_setWindowEraseBgSubclass;
	extern PFN_DarkMode_removeWindowEraseBgSubclass _DarkMode_removeWindowEraseBgSubclass;
	extern PFN_DarkMode_setWindowCtlColorSubclass _DarkMode_setWindowCtlColorSubclass;
	extern PFN_DarkMode_removeWindowCtlColorSubclass _DarkMode_removeWindowCtlColorSubclass;
	extern PFN_DarkMode_setWindowNotifyCustomDrawSubclass _DarkMode_setWindowNotifyCustomDrawSubclass;
	extern PFN_DarkMode_removeWindowNotifyCustomDrawSubclass _DarkMode_removeWindowNotifyCustomDrawSubclass;
	extern PFN_DarkMode_setWindowMenuBarSubclass _DarkMode_setWindowMenuBarSubclass;
	extern PFN_DarkMode_removeWindowMenuBarSubclass _DarkMode_removeWindowMenuBarSubclass;
	extern PFN_DarkMode_setWindowSettingChangeSubclass _DarkMode_setWindowSettingChangeSubclass;
	extern PFN_DarkMode_removeWindowSettingChangeSubclass _DarkMode_removeWindowSettingChangeSubclass;
	extern PFN_DarkMode_enableSysLinkCtrlCtlColor _DarkMode_enableSysLinkCtrlCtlColor;
	extern PFN_DarkMode_setDarkTitleBarEx _DarkMode_setDarkTitleBarEx;
	extern PFN_DarkMode_setDarkTitleBar _DarkMode_setDarkTitleBar;
	extern PFN_DarkMode_setDarkThemeExperimental _DarkMode_setDarkThemeExperimental;
	extern PFN_DarkMode_setDarkExplorerTheme _DarkMode_setDarkExplorerTheme;
	extern PFN_DarkMode_setDarkScrollBar _DarkMode_setDarkScrollBar;
	extern PFN_DarkMode_setDarkTooltips _DarkMode_setDarkTooltips;
	extern PFN_DarkMode_setDarkLineAbovePanelToolbar _DarkMode_setDarkLineAbovePanelToolbar;
	extern PFN_DarkMode_setDarkListView _DarkMode_setDarkListView;
	extern PFN_DarkMode_setDarkListViewCheckboxes _DarkMode_setDarkListViewCheckboxes;
	extern PFN_DarkMode_setDarkRichEdit _DarkMode_setDarkRichEdit;
	extern PFN_DarkMode_setDarkWndSafe _DarkMode_setDarkWndSafe;
	extern PFN_DarkMode_setDarkWndNotifySafeEx _DarkMode_setDarkWndNotifySafeEx;
	extern PFN_DarkMode_setDarkWndNotifySafe _DarkMode_setDarkWndNotifySafe;
	extern PFN_DarkMode_enableThemeDialogTexture _DarkMode_enableThemeDialogTexture;
	extern PFN_DarkMode_disableVisualStyle _DarkMode_disableVisualStyle;
	extern PFN_DarkMode_calculatePerceivedLightness _DarkMode_calculatePerceivedLightness;
	extern PFN_DarkMode_getTreeViewStyle _DarkMode_getTreeViewStyle;
	extern PFN_DarkMode_calculateTreeViewStyle _DarkMode_calculateTreeViewStyle;
	extern PFN_DarkMode_setTreeViewWindowTheme _DarkMode_setTreeViewWindowTheme;
	extern PFN_DarkMode_getPrevTreeViewStyle _DarkMode_getPrevTreeViewStyle;
	extern PFN_DarkMode_setPrevTreeViewStyle _DarkMode_setPrevTreeViewStyle;
	extern PFN_DarkMode_isThemeDark _DarkMode_isThemeDark;
	extern PFN_DarkMode_isColorDark _DarkMode_isColorDark;
	extern PFN_DarkMode_redrawWindowFrame _DarkMode_redrawWindowFrame;
	extern PFN_DarkMode_setWindowStyle _DarkMode_setWindowStyle;
	extern PFN_DarkMode_setWindowExStyle _DarkMode_setWindowExStyle;
	extern PFN_DarkMode_replaceExEdgeWithBorder _DarkMode_replaceExEdgeWithBorder;
	extern PFN_DarkMode_replaceClientEdgeWithBorderSafe _DarkMode_replaceClientEdgeWithBorderSafe;
	extern PFN_DarkMode_setProgressBarClassicTheme _DarkMode_setProgressBarClassicTheme;
	extern PFN_DarkMode_onCtlColor _DarkMode_onCtlColor;
	extern PFN_DarkMode_onCtlColorCtrl _DarkMode_onCtlColorCtrl;
	extern PFN_DarkMode_onCtlColorDlg _DarkMode_onCtlColorDlg;
	extern PFN_DarkMode_onCtlColorError _DarkMode_onCtlColorError;
	extern PFN_DarkMode_onCtlColorDlgStaticText _DarkMode_onCtlColorDlgStaticText;
	extern PFN_DarkMode_onCtlColorDlgLinkText _DarkMode_onCtlColorDlgLinkText;
	extern PFN_DarkMode_onCtlColorListbox _DarkMode_onCtlColorListbox;
	extern PFN_DarkMode_HookDlgProc _DarkMode_HookDlgProc;

	BOOL DarkMode_LoadLibrary(const wchar_t *dllPath);
	void DarkMode_FreeLibrary();

	int DarkMode_getLibInfo(DarkMode_LibInfo libInfoType);

	void DarkMode_initDarkModeConfig(UINT dmType);
	void DarkMode_setRoundCornerConfig(UINT roundCornerStyle);
	void DarkMode_setBorderColorConfig(COLORREF clr);
	void DarkMode_setMicaConfig(UINT mica);
	void DarkMode_setMicaExtendedConfig(BOOL extendMica);
	void DarkMode_setColorizeTitleBarConfig(BOOL colorize);
	void DarkMode_setDarkModeConfig(UINT dmType);
	void DarkMode_setDarkModeConfig_System();
	void DarkMode_initDarkMode(const wchar_t *iniName);
	void DarkMode_initDarkMode_NoIni();

	BOOL DarkMode_isEnabled();
	BOOL DarkMode_isExperimentalActive();
	BOOL DarkMode_isExperimentalSupported();
	BOOL DarkMode_isWindowsModeEnabled();
	BOOL DarkMode_isAtLeastWindows10();
	BOOL DarkMode_isAtLeastWindows11();
	DWORD DarkMode_getWindowsBuildNumber();

	BOOL DarkMode_handleSettingChange(LPARAM lParam);
	BOOL DarkMode_isDarkModeReg();

	void DarkMode_setSysColor(int nIndex, COLORREF color);

	void DarkMode_enableDarkScrollBarForWindowAndChildren(HWND hWnd);

	void DarkMode_setColorTone(DarkMode_ColorTone colorTone);
	DarkMode_ColorTone DarkMode_getColorTone();
	COLORREF DarkMode_setBackgroundColor(COLORREF clrNew);
	COLORREF DarkMode_setCtrlBackgroundColor(COLORREF clrNew);
	COLORREF DarkMode_setHotBackgroundColor(COLORREF clrNew);
	COLORREF DarkMode_setDlgBackgroundColor(COLORREF clrNew);
	COLORREF DarkMode_setErrorBackgroundColor(COLORREF clrNew);
	COLORREF DarkMode_setTextColor(COLORREF clrNew);
	COLORREF DarkMode_setDarkerTextColor(COLORREF clrNew);
	COLORREF DarkMode_setDisabledTextColor(COLORREF clrNew);
	COLORREF DarkMode_setLinkTextColor(COLORREF clrNew);
	COLORREF DarkMode_setEdgeColor(COLORREF clrNew);
	COLORREF DarkMode_setHotEdgeColor(COLORREF clrNew);
	COLORREF DarkMode_setDisabledEdgeColor(COLORREF clrNew);
	void DarkMode_setThemeColors(DarkMode_Colors colors);
	void DarkMode_updateThemeBrushesAndPens();
	COLORREF DarkMode_getBackgroundColor();
	COLORREF DarkMode_getCtrlBackgroundColor();
	COLORREF DarkMode_getHotBackgroundColor();
	COLORREF DarkMode_getDlgBackgroundColor();
	COLORREF DarkMode_getErrorBackgroundColor();
	COLORREF DarkMode_getTextColor();
	COLORREF DarkMode_getDarkerTextColor();
	COLORREF DarkMode_getDisabledTextColor();
	COLORREF DarkMode_getLinkTextColor();
	COLORREF DarkMode_getEdgeColor();
	COLORREF DarkMode_getHotEdgeColor();
	COLORREF DarkMode_getDisabledEdgeColor();
	HBRUSH DarkMode_getBackgroundBrush();
	HBRUSH DarkMode_getDlgBackgroundBrush();
	HBRUSH DarkMode_getCtrlBackgroundBrush();
	HBRUSH DarkMode_getHotBackgroundBrush();
	HBRUSH DarkMode_getErrorBackgroundBrush();
	HBRUSH DarkMode_getEdgeBrush();
	HBRUSH DarkMode_getHotEdgeBrush();
	HBRUSH DarkMode_getDisabledEdgeBrush();
	HPEN DarkMode_getDarkerTextPen();
	HPEN DarkMode_getEdgePen();
	HPEN DarkMode_getHotEdgePen();
	HPEN DarkMode_getDisabledEdgePen();
	COLORREF DarkMode_setViewBackgroundColor(COLORREF clrNew);
	COLORREF DarkMode_setViewTextColor(COLORREF clrNew);
	COLORREF DarkMode_setViewGridlinesColor(COLORREF clrNew);
	COLORREF DarkMode_setHeaderBackgroundColor(COLORREF clrNew);
	COLORREF DarkMode_setHeaderHotBackgroundColor(COLORREF clrNew);
	COLORREF DarkMode_setHeaderTextColor(COLORREF clrNew);
	COLORREF DarkMode_setHeaderEdgeColor(COLORREF clrNew);
	void DarkMode_setViewColors(DarkMode_ColorsView colors);
	void DarkMode_updateViewBrushesAndPens();
	COLORREF DarkMode_getViewBackgroundColor();
	COLORREF DarkMode_getViewTextColor();
	COLORREF DarkMode_getViewGridlinesColor();
	COLORREF DarkMode_getHeaderBackgroundColor();
	COLORREF DarkMode_getHeaderHotBackgroundColor();
	COLORREF DarkMode_getHeaderTextColor();
	COLORREF DarkMode_getHeaderEdgeColor();
	HBRUSH DarkMode_getViewBackgroundBrush();
	HBRUSH DarkMode_getViewGridlinesBrush();
	HBRUSH DarkMode_getHeaderBackgroundBrush();
	HBRUSH DarkMode_getHeaderHotBackgroundBrush();
	HPEN DarkMode_getHeaderEdgePen();
	void DarkMode_setDefaultColors(BOOL updateBrushesAndOther);

	void DarkMode_paintRoundRect(HDC hdc, const RECT *rect, HPEN hpen, HBRUSH hBrush, int width, int height);
	void DarkMode_paintRoundFrameRect(HDC hdc, const RECT *rect, HPEN hpen, int width, int height);
#define DarkMode_paintRoundRect_Default(hdc, rect, hpen, hBrush) DarkMode_paintRoundRect(hdc, rect, hpen, hBrush, 0, 0)
#define DarkMode_paintRoundFrameRect_Default(hdc, rect, hpen) DarkMode_paintRoundFrameRect(hdc, rect, hpen, 0, 0)

	void DarkMode_setCheckboxOrRadioBtnCtrlSubclass(HWND hWnd);
	void DarkMode_removeCheckboxOrRadioBtnCtrlSubclass(HWND hWnd);
	void DarkMode_setGroupboxCtrlSubclass(HWND hWnd);
	void DarkMode_removeGroupboxCtrlSubclass(HWND hWnd);
	void DarkMode_setUpDownCtrlSubclass(HWND hWnd);
	void DarkMode_removeUpDownCtrlSubclass(HWND hWnd);
	void DarkMode_setTabCtrlUpDownSubclass(HWND hWnd);
	void DarkMode_removeTabCtrlUpDownSubclass(HWND hWnd);
	void DarkMode_setTabCtrlSubclass(HWND hWnd);
	void DarkMode_removeTabCtrlSubclass(HWND hWnd);
	void DarkMode_setCustomBorderForListBoxOrEditCtrlSubclass(HWND hWnd);
	void DarkMode_removeCustomBorderForListBoxOrEditCtrlSubclass(HWND hWnd);
	void DarkMode_setComboBoxCtrlSubclass(HWND hWnd);
	void DarkMode_removeComboBoxCtrlSubclass(HWND hWnd);
	void DarkMode_setComboBoxExCtrlSubclass(HWND hWnd);
	void DarkMode_removeComboBoxExCtrlSubclass(HWND hWnd);
	void DarkMode_setListViewCtrlSubclass(HWND hWnd);
	void DarkMode_removeListViewCtrlSubclass(HWND hWnd);
	void DarkMode_setHeaderCtrlSubclass(HWND hWnd);
	void DarkMode_removeHeaderCtrlSubclass(HWND hWnd);
	void DarkMode_setStatusBarCtrlSubclass(HWND hWnd);
	void DarkMode_removeStatusBarCtrlSubclass(HWND hWnd);
	void DarkMode_setProgressBarCtrlSubclass(HWND hWnd);
	void DarkMode_removeProgressBarCtrlSubclass(HWND hWnd);
	void DarkMode_setStaticTextCtrlSubclass(HWND hWnd);
	void DarkMode_removeStaticTextCtrlSubclass(HWND hWnd);

	void DarkMode_setChildCtrlsSubclassAndTheme(HWND hParent, BOOL subclass, BOOL theme);
	void DarkMode_setChildCtrlsTheme(HWND hParent);
#define DarkMode_setChildCtrlsSubclassAndTheme_Default(hParent) DarkMode_setChildCtrlsSubclassAndTheme(hParent, TRUE, TRUE)

	void DarkMode_setWindowEraseBgSubclass(HWND hWnd);
	void DarkMode_removeWindowEraseBgSubclass(HWND hWnd);
	void DarkMode_setWindowCtlColorSubclass(HWND hWnd);
	void DarkMode_removeWindowCtlColorSubclass(HWND hWnd);
	void DarkMode_setWindowNotifyCustomDrawSubclass(HWND hWnd);
	void DarkMode_removeWindowNotifyCustomDrawSubclass(HWND hWnd);
	void DarkMode_setWindowMenuBarSubclass(HWND hWnd);
	void DarkMode_removeWindowMenuBarSubclass(HWND hWnd);
	void DarkMode_setWindowSettingChangeSubclass(HWND hWnd);
	void DarkMode_removeWindowSettingChangeSubclass(HWND hWnd);

	void DarkMode_enableSysLinkCtrlCtlColor(HWND hWnd);
	void DarkMode_setDarkTitleBarEx(HWND hWnd, BOOL useWin11Features);
	void DarkMode_setDarkTitleBar(HWND hWnd);
	void DarkMode_setDarkThemeExperimental(HWND hWnd, const wchar_t *themeClassName);
	void DarkMode_setDarkExplorerTheme(HWND hWnd);
	void DarkMode_setDarkScrollBar(HWND hWnd);
	void DarkMode_setDarkTooltips(HWND hWnd, DarkMode_ToolTipsType type);
	void DarkMode_setDarkLineAbovePanelToolbar(HWND hWnd);
	void DarkMode_setDarkListView(HWND hWnd);
	void DarkMode_setDarkListViewCheckboxes(HWND hWnd);
	void DarkMode_setDarkRichEdit(HWND hWnd);
	void DarkMode_setDarkWndSafe(HWND hWnd, BOOL useWin11Features);
	void DarkMode_setDarkWndNotifySafeEx(HWND hWnd, BOOL setSettingChangeSubclass, BOOL useWin11Features);
	void DarkMode_setDarkWndNotifySafe(HWND hWnd, BOOL useWin11Features);
	void DarkMode_enableThemeDialogTexture(HWND hWnd, BOOL theme);
	void DarkMode_disableVisualStyle(HWND hWnd, BOOL doDisable);
	double DarkMode_calculatePerceivedLightness(COLORREF clr);
	DarkMode_TreeViewStyle DarkMode_getTreeViewStyle();
	void DarkMode_calculateTreeViewStyle();
	void DarkMode_setTreeViewWindowTheme(HWND hWnd, BOOL force);
	DarkMode_TreeViewStyle DarkMode_getPrevTreeViewStyle();
	void DarkMode_setPrevTreeViewStyle();
	BOOL DarkMode_isThemeDark();
	BOOL DarkMode_isColorDark(COLORREF clr);
	void DarkMode_redrawWindowFrame(HWND hWnd);
	void DarkMode_setWindowStyle(HWND hWnd, BOOL setStyle, LONG_PTR styleFlag);
	void DarkMode_setWindowExStyle(HWND hWnd, BOOL setExStyle, LONG_PTR exStyleFlag);
	void DarkMode_replaceExEdgeWithBorder(HWND hWnd, BOOL replace, LONG_PTR exStyleFlag);
	void DarkMode_replaceClientEdgeWithBorderSafe(HWND hWnd);
	void DarkMode_setProgressBarClassicTheme(HWND hWnd);

	LRESULT DarkMode_onCtlColor(HDC hdc);
	LRESULT DarkMode_onCtlColorCtrl(HDC hdc);
	LRESULT DarkMode_onCtlColorDlg(HDC hdc);
	LRESULT DarkMode_onCtlColorError(HDC hdc);
	LRESULT DarkMode_onCtlColorDlgStaticText(HDC hdc, BOOL isTextEnabled);
	LRESULT DarkMode_onCtlColorDlgLinkText(HDC hdc, BOOL isTextEnabled);
	LRESULT DarkMode_onCtlColorListbox(WPARAM wParam, LPARAM lParam);
#define DarkMode_onCtlColorDlgLinkText_Default(hdc) DarkMode_onCtlColorDlgLinkText(hdc, TRUE)

	UINT_PTR CALLBACK DarkMode_HookDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#define DarkMode_setDarkThemeExperimental_Default(hWnd) DarkMode_setDarkThemeExperimental(hWnd, L"Explorer")
#define DarkMode_setDarkTooltips_Default(hWnd) DarkMode_setDarkTooltips(hWnd, DarkMode_ToolTipsType_tooltip)
#define DarkMode_setDarkWndSafe_Default(hWnd) DarkMode_setDarkWndSafe(hWnd, TRUE)
#define DarkMode_setDarkWndNotifySafe_Default(hWnd) DarkMode_setDarkWndNotifySafe((hWnd), TRUE)
#define DarkMode_setTreeViewWindowTheme_Default(hWnd) DarkMode_setTreeViewWindowTheme(hWnd, FALSE)

#ifdef __cplusplus
}
#endif

#else
#define _DARKMODELIB_NOT_USED
#endif
#endif