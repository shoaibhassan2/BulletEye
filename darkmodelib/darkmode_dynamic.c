#include <DarkMode.h>
#include <windows.h>
#include <stdio.h>

HMODULE g_hDarkModeLib = NULL;

PFN_DarkMode_getLibInfo _DarkMode_getLibInfo = NULL;

PFN_DarkMode_initDarkModeConfig _DarkMode_initDarkModeConfig = NULL;
PFN_DarkMode_setRoundCornerConfig _DarkMode_setRoundCornerConfig = NULL;
PFN_DarkMode_setBorderColorConfig _DarkMode_setBorderColorConfig = NULL;
PFN_DarkMode_setMicaConfig _DarkMode_setMicaConfig = NULL;
PFN_DarkMode_setMicaExtendedConfig _DarkMode_setMicaExtendedConfig = NULL;
PFN_DarkMode_setColorizeTitleBarConfig _DarkMode_setColorizeTitleBarConfig = NULL;
PFN_DarkMode_setDarkModeConfig _DarkMode_setDarkModeConfig = NULL;
PFN_DarkMode_setDarkModeConfig_System _DarkMode_setDarkModeConfig_System = NULL;
PFN_DarkMode_initDarkMode _DarkMode_initDarkMode = NULL;
PFN_DarkMode_initDarkMode_NoIni _DarkMode_initDarkMode_NoIni = NULL;

PFN_DarkMode_isEnabled _DarkMode_isEnabled = NULL;
PFN_DarkMode_isExperimentalActive _DarkMode_isExperimentalActive = NULL;
PFN_DarkMode_isExperimentalSupported _DarkMode_isExperimentalSupported = NULL;
PFN_DarkMode_isWindowsModeEnabled _DarkMode_isWindowsModeEnabled = NULL;
PFN_DarkMode_isAtLeastWindows10 _DarkMode_isAtLeastWindows10 = NULL;
PFN_DarkMode_isAtLeastWindows11 _DarkMode_isAtLeastWindows11 = NULL;
PFN_DarkMode_getWindowsBuildNumber _DarkMode_getWindowsBuildNumber = NULL;

PFN_DarkMode_handleSettingChange _DarkMode_handleSettingChange = NULL;
PFN_DarkMode_isDarkModeReg _DarkMode_isDarkModeReg = NULL;

PFN_DarkMode_setSysColor _DarkMode_setSysColor = NULL;

PFN_DarkMode_enableDarkScrollBarForWindowAndChildren _DarkMode_enableDarkScrollBarForWindowAndChildren = NULL;

PFN_DarkMode_setColorTone _DarkMode_setColorTone = NULL;
PFN_DarkMode_getColorTone _DarkMode_getColorTone = NULL;
PFN_DarkMode_setBackgroundColor _DarkMode_setBackgroundColor = NULL;
PFN_DarkMode_setCtrlBackgroundColor _DarkMode_setCtrlBackgroundColor = NULL;
PFN_DarkMode_setHotBackgroundColor _DarkMode_setHotBackgroundColor = NULL;
PFN_DarkMode_setDlgBackgroundColor _DarkMode_setDlgBackgroundColor = NULL;
PFN_DarkMode_setErrorBackgroundColor _DarkMode_setErrorBackgroundColor = NULL;
PFN_DarkMode_setTextColor _DarkMode_setTextColor = NULL;
PFN_DarkMode_setDarkerTextColor _DarkMode_setDarkerTextColor = NULL;
PFN_DarkMode_setDisabledTextColor _DarkMode_setDisabledTextColor = NULL;
PFN_DarkMode_setLinkTextColor _DarkMode_setLinkTextColor = NULL;
PFN_DarkMode_setEdgeColor _DarkMode_setEdgeColor = NULL;
PFN_DarkMode_setHotEdgeColor _DarkMode_setHotEdgeColor = NULL;
PFN_DarkMode_setDisabledEdgeColor _DarkMode_setDisabledEdgeColor = NULL;
PFN_DarkMode_setThemeColors _DarkMode_setThemeColors = NULL;
PFN_DarkMode_updateThemeBrushesAndPens _DarkMode_updateThemeBrushesAndPens = NULL;
PFN_DarkMode_getBackgroundColor _DarkMode_getBackgroundColor = NULL;
PFN_DarkMode_getCtrlBackgroundColor _DarkMode_getCtrlBackgroundColor = NULL;
PFN_DarkMode_getHotBackgroundColor _DarkMode_getHotBackgroundColor = NULL;
PFN_DarkMode_getDlgBackgroundColor _DarkMode_getDlgBackgroundColor = NULL;
PFN_DarkMode_getErrorBackgroundColor _DarkMode_getErrorBackgroundColor = NULL;
PFN_DarkMode_getTextColor _DarkMode_getTextColor = NULL;
PFN_DarkMode_getDarkerTextColor _DarkMode_getDarkerTextColor = NULL;
PFN_DarkMode_getDisabledTextColor _DarkMode_getDisabledTextColor = NULL;
PFN_DarkMode_getLinkTextColor _DarkMode_getLinkTextColor = NULL;
PFN_DarkMode_getEdgeColor _DarkMode_getEdgeColor = NULL;
PFN_DarkMode_getHotEdgeColor _DarkMode_getHotEdgeColor = NULL;
PFN_DarkMode_getDisabledEdgeColor _DarkMode_getDisabledEdgeColor = NULL;
PFN_DarkMode_getBackgroundBrush _DarkMode_getBackgroundBrush = NULL;
PFN_DarkMode_getDlgBackgroundBrush _DarkMode_getDlgBackgroundBrush = NULL;
PFN_DarkMode_getCtrlBackgroundBrush _DarkMode_getCtrlBackgroundBrush = NULL;
PFN_DarkMode_getHotBackgroundBrush _DarkMode_getHotBackgroundBrush = NULL;
PFN_DarkMode_getErrorBackgroundBrush _DarkMode_getErrorBackgroundBrush = NULL;
PFN_DarkMode_getEdgeBrush _DarkMode_getEdgeBrush = NULL;
PFN_DarkMode_getHotEdgeBrush _DarkMode_getHotEdgeBrush = NULL;
PFN_DarkMode_getDisabledEdgeBrush _DarkMode_getDisabledEdgeBrush = NULL;
PFN_DarkMode_getDarkerTextPen _DarkMode_getDarkerTextPen = NULL;
PFN_DarkMode_getEdgePen _DarkMode_getEdgePen = NULL;
PFN_DarkMode_getHotEdgePen _DarkMode_getHotEdgePen = NULL;
PFN_DarkMode_getDisabledEdgePen _DarkMode_getDisabledEdgePen = NULL;
PFN_DarkMode_setViewBackgroundColor _DarkMode_setViewBackgroundColor = NULL;
PFN_DarkMode_setViewTextColor _DarkMode_setViewTextColor = NULL;
PFN_DarkMode_setViewGridlinesColor _DarkMode_setViewGridlinesColor = NULL;
PFN_DarkMode_setHeaderBackgroundColor _DarkMode_setHeaderBackgroundColor = NULL;
PFN_DarkMode_setHeaderHotBackgroundColor _DarkMode_setHeaderHotBackgroundColor = NULL;
PFN_DarkMode_setHeaderTextColor _DarkMode_setHeaderTextColor = NULL;
PFN_DarkMode_setHeaderEdgeColor _DarkMode_setHeaderEdgeColor = NULL;
PFN_DarkMode_setViewColors _DarkMode_setViewColors = NULL;
PFN_DarkMode_updateViewBrushesAndPens _DarkMode_updateViewBrushesAndPens = NULL;
PFN_DarkMode_getViewBackgroundColor _DarkMode_getViewBackgroundColor = NULL;
PFN_DarkMode_getViewTextColor _DarkMode_getViewTextColor = NULL;
PFN_DarkMode_getViewGridlinesColor _DarkMode_getViewGridlinesColor = NULL;
PFN_DarkMode_getHeaderBackgroundColor _DarkMode_getHeaderBackgroundColor = NULL;
PFN_DarkMode_getHeaderHotBackgroundColor _DarkMode_getHeaderHotBackgroundColor = NULL;
PFN_DarkMode_getHeaderTextColor _DarkMode_getHeaderTextColor = NULL;
PFN_DarkMode_getHeaderEdgeColor _DarkMode_getHeaderEdgeColor = NULL;
PFN_DarkMode_getViewBackgroundBrush _DarkMode_getViewBackgroundBrush = NULL;
PFN_DarkMode_getViewGridlinesBrush _DarkMode_getViewGridlinesBrush = NULL;
PFN_DarkMode_getHeaderBackgroundBrush _DarkMode_getHeaderBackgroundBrush = NULL;
PFN_DarkMode_getHeaderHotBackgroundBrush _DarkMode_getHeaderHotBackgroundBrush = NULL;
PFN_DarkMode_getHeaderEdgePen _DarkMode_getHeaderEdgePen = NULL;
PFN_DarkMode_setDefaultColors _DarkMode_setDefaultColors = NULL;

PFN_DarkMode_paintRoundRect _DarkMode_paintRoundRect = NULL;
PFN_DarkMode_paintRoundFrameRect _DarkMode_paintRoundFrameRect = NULL;

PFN_DarkMode_setCheckboxOrRadioBtnCtrlSubclass _DarkMode_setCheckboxOrRadioBtnCtrlSubclass = NULL;
PFN_DarkMode_removeCheckboxOrRadioBtnCtrlSubclass _DarkMode_removeCheckboxOrRadioBtnCtrlSubclass = NULL;
PFN_DarkMode_setGroupboxCtrlSubclass _DarkMode_setGroupboxCtrlSubclass = NULL;
PFN_DarkMode_removeGroupboxCtrlSubclass _DarkMode_removeGroupboxCtrlSubclass = NULL;
PFN_DarkMode_setUpDownCtrlSubclass _DarkMode_setUpDownCtrlSubclass = NULL;
PFN_DarkMode_removeUpDownCtrlSubclass _DarkMode_removeUpDownCtrlSubclass = NULL;
PFN_DarkMode_setTabCtrlUpDownSubclass _DarkMode_setTabCtrlUpDownSubclass = NULL;
PFN_DarkMode_removeTabCtrlUpDownSubclass _DarkMode_removeTabCtrlUpDownSubclass = NULL;
PFN_DarkMode_setTabCtrlSubclass _DarkMode_setTabCtrlSubclass = NULL;
PFN_DarkMode_removeTabCtrlSubclass _DarkMode_removeTabCtrlSubclass = NULL;
PFN_DarkMode_setCustomBorderForListBoxOrEditCtrlSubclass _DarkMode_setCustomBorderForListBoxOrEditCtrlSubclass = NULL;
PFN_DarkMode_removeCustomBorderForListBoxOrEditCtrlSubclass _DarkMode_removeCustomBorderForListBoxOrEditCtrlSubclass = NULL;
PFN_DarkMode_setComboBoxCtrlSubclass _DarkMode_setComboBoxCtrlSubclass = NULL;
PFN_DarkMode_removeComboBoxCtrlSubclass _DarkMode_removeComboBoxCtrlSubclass = NULL;
PFN_DarkMode_setComboBoxExCtrlSubclass _DarkMode_setComboBoxExCtrlSubclass = NULL;
PFN_DarkMode_removeComboBoxExCtrlSubclass _DarkMode_removeComboBoxExCtrlSubclass = NULL;
PFN_DarkMode_setListViewCtrlSubclass _DarkMode_setListViewCtrlSubclass = NULL;
PFN_DarkMode_removeListViewCtrlSubclass _DarkMode_removeListViewCtrlSubclass = NULL;
PFN_DarkMode_setHeaderCtrlSubclass _DarkMode_setHeaderCtrlSubclass = NULL;
PFN_DarkMode_removeHeaderCtrlSubclass _DarkMode_removeHeaderCtrlSubclass = NULL;
PFN_DarkMode_setStatusBarCtrlSubclass _DarkMode_setStatusBarCtrlSubclass = NULL;
PFN_DarkMode_removeStatusBarCtrlSubclass _DarkMode_removeStatusBarCtrlSubclass = NULL;
PFN_DarkMode_setProgressBarCtrlSubclass _DarkMode_setProgressBarCtrlSubclass = NULL;
PFN_DarkMode_removeProgressBarCtrlSubclass _DarkMode_removeProgressBarCtrlSubclass = NULL;
PFN_DarkMode_setStaticTextCtrlSubclass _DarkMode_setStaticTextCtrlSubclass = NULL;
PFN_DarkMode_removeStaticTextCtrlSubclass _DarkMode_removeStaticTextCtrlSubclass = NULL;

PFN_DarkMode_setChildCtrlsSubclassAndTheme _DarkMode_setChildCtrlsSubclassAndTheme = NULL;
PFN_DarkMode_setChildCtrlsTheme _DarkMode_setChildCtrlsTheme = NULL;

PFN_DarkMode_setWindowEraseBgSubclass _DarkMode_setWindowEraseBgSubclass = NULL;
PFN_DarkMode_removeWindowEraseBgSubclass _DarkMode_removeWindowEraseBgSubclass = NULL;
PFN_DarkMode_setWindowCtlColorSubclass _DarkMode_setWindowCtlColorSubclass = NULL;
PFN_DarkMode_removeWindowCtlColorSubclass _DarkMode_removeWindowCtlColorSubclass = NULL;
PFN_DarkMode_setWindowNotifyCustomDrawSubclass _DarkMode_setWindowNotifyCustomDrawSubclass = NULL;
PFN_DarkMode_removeWindowNotifyCustomDrawSubclass _DarkMode_removeWindowNotifyCustomDrawSubclass = NULL;
PFN_DarkMode_setWindowMenuBarSubclass _DarkMode_setWindowMenuBarSubclass = NULL;
PFN_DarkMode_removeWindowMenuBarSubclass _DarkMode_removeWindowMenuBarSubclass = NULL;
PFN_DarkMode_setWindowSettingChangeSubclass _DarkMode_setWindowSettingChangeSubclass = NULL;
PFN_DarkMode_removeWindowSettingChangeSubclass _DarkMode_removeWindowSettingChangeSubclass = NULL;

PFN_DarkMode_enableSysLinkCtrlCtlColor _DarkMode_enableSysLinkCtrlCtlColor = NULL;
PFN_DarkMode_setDarkTitleBarEx _DarkMode_setDarkTitleBarEx = NULL;
PFN_DarkMode_setDarkTitleBar _DarkMode_setDarkTitleBar = NULL;
PFN_DarkMode_setDarkThemeExperimental _DarkMode_setDarkThemeExperimental = NULL;
PFN_DarkMode_setDarkExplorerTheme _DarkMode_setDarkExplorerTheme = NULL;
PFN_DarkMode_setDarkScrollBar _DarkMode_setDarkScrollBar = NULL;
PFN_DarkMode_setDarkTooltips _DarkMode_setDarkTooltips = NULL;
PFN_DarkMode_setDarkLineAbovePanelToolbar _DarkMode_setDarkLineAbovePanelToolbar = NULL;
PFN_DarkMode_setDarkListView _DarkMode_setDarkListView = NULL;
PFN_DarkMode_setDarkListViewCheckboxes _DarkMode_setDarkListViewCheckboxes = NULL;
PFN_DarkMode_setDarkRichEdit _DarkMode_setDarkRichEdit = NULL;
PFN_DarkMode_setDarkWndSafe _DarkMode_setDarkWndSafe = NULL;
PFN_DarkMode_setDarkWndNotifySafeEx _DarkMode_setDarkWndNotifySafeEx = NULL;
PFN_DarkMode_setDarkWndNotifySafe _DarkMode_setDarkWndNotifySafe = NULL;
PFN_DarkMode_enableThemeDialogTexture _DarkMode_enableThemeDialogTexture = NULL;
PFN_DarkMode_disableVisualStyle _DarkMode_disableVisualStyle = NULL;
PFN_DarkMode_calculatePerceivedLightness _DarkMode_calculatePerceivedLightness = NULL;
PFN_DarkMode_getTreeViewStyle _DarkMode_getTreeViewStyle = NULL;
PFN_DarkMode_calculateTreeViewStyle _DarkMode_calculateTreeViewStyle = NULL;
PFN_DarkMode_setTreeViewWindowTheme _DarkMode_setTreeViewWindowTheme = NULL;
PFN_DarkMode_getPrevTreeViewStyle _DarkMode_getPrevTreeViewStyle = NULL;
PFN_DarkMode_setPrevTreeViewStyle _DarkMode_setPrevTreeViewStyle = NULL;
PFN_DarkMode_isThemeDark _DarkMode_isThemeDark = NULL;
PFN_DarkMode_isColorDark _DarkMode_isColorDark = NULL;
PFN_DarkMode_redrawWindowFrame _DarkMode_redrawWindowFrame = NULL;
PFN_DarkMode_setWindowStyle _DarkMode_setWindowStyle = NULL;
PFN_DarkMode_setWindowExStyle _DarkMode_setWindowExStyle = NULL;
PFN_DarkMode_replaceExEdgeWithBorder _DarkMode_replaceExEdgeWithBorder = NULL;
PFN_DarkMode_replaceClientEdgeWithBorderSafe _DarkMode_replaceClientEdgeWithBorderSafe = NULL;
PFN_DarkMode_setProgressBarClassicTheme _DarkMode_setProgressBarClassicTheme = NULL;

PFN_DarkMode_onCtlColor _DarkMode_onCtlColor = NULL;
PFN_DarkMode_onCtlColorCtrl _DarkMode_onCtlColorCtrl = NULL;
PFN_DarkMode_onCtlColorDlg _DarkMode_onCtlColorDlg = NULL;
PFN_DarkMode_onCtlColorError _DarkMode_onCtlColorError = NULL;
PFN_DarkMode_onCtlColorDlgStaticText _DarkMode_onCtlColorDlgStaticText = NULL;
PFN_DarkMode_onCtlColorDlgLinkText _DarkMode_onCtlColorDlgLinkText = NULL;
PFN_DarkMode_onCtlColorListbox _DarkMode_onCtlColorListbox = NULL;

PFN_DarkMode_HookDlgProc _DarkMode_HookDlgProc = NULL;

BOOL DarkMode_LoadLibrary(const wchar_t *dllPath)
{
    if (g_hDarkModeLib)
    {
        return TRUE;
    }

    g_hDarkModeLib = LoadLibraryW(dllPath);
    if (!g_hDarkModeLib)
    {

        return FALSE;
    }

#define GET_PROC_ADDRESS(name)                                   \
    _##name = (PFN_##name)GetProcAddress(g_hDarkModeLib, #name); \
    if (!_##name)                                                \
    {                                                            \
                                                                 \
        DarkMode_FreeLibrary();                                  \
        return FALSE;                                            \
    }

    GET_PROC_ADDRESS(DarkMode_getLibInfo);
    GET_PROC_ADDRESS(DarkMode_initDarkModeConfig);
    GET_PROC_ADDRESS(DarkMode_setRoundCornerConfig);
    GET_PROC_ADDRESS(DarkMode_setMicaConfig);
    GET_PROC_ADDRESS(DarkMode_setMicaExtendedConfig);
    GET_PROC_ADDRESS(DarkMode_setColorizeTitleBarConfig);
    GET_PROC_ADDRESS(DarkMode_setDarkModeConfig);
    GET_PROC_ADDRESS(DarkMode_setDarkModeConfig_System);
    GET_PROC_ADDRESS(DarkMode_initDarkMode);
    GET_PROC_ADDRESS(DarkMode_initDarkMode_NoIni);
    GET_PROC_ADDRESS(DarkMode_isEnabled);
    GET_PROC_ADDRESS(DarkMode_isExperimentalActive);
    GET_PROC_ADDRESS(DarkMode_isExperimentalSupported);
    GET_PROC_ADDRESS(DarkMode_isWindowsModeEnabled);
    GET_PROC_ADDRESS(DarkMode_isAtLeastWindows10);
    GET_PROC_ADDRESS(DarkMode_isAtLeastWindows11);
    GET_PROC_ADDRESS(DarkMode_getWindowsBuildNumber);
    GET_PROC_ADDRESS(DarkMode_handleSettingChange);
    GET_PROC_ADDRESS(DarkMode_isDarkModeReg);
    GET_PROC_ADDRESS(DarkMode_setSysColor);
    GET_PROC_ADDRESS(DarkMode_enableDarkScrollBarForWindowAndChildren);
    GET_PROC_ADDRESS(DarkMode_setColorTone);
    GET_PROC_ADDRESS(DarkMode_getColorTone);
    GET_PROC_ADDRESS(DarkMode_setBackgroundColor);
    GET_PROC_ADDRESS(DarkMode_setCtrlBackgroundColor);
    GET_PROC_ADDRESS(DarkMode_setHotBackgroundColor);
    GET_PROC_ADDRESS(DarkMode_setDlgBackgroundColor);
    GET_PROC_ADDRESS(DarkMode_setErrorBackgroundColor);
    GET_PROC_ADDRESS(DarkMode_setTextColor);
    GET_PROC_ADDRESS(DarkMode_setDarkerTextColor);
    GET_PROC_ADDRESS(DarkMode_setDisabledTextColor);
    GET_PROC_ADDRESS(DarkMode_setLinkTextColor);
    GET_PROC_ADDRESS(DarkMode_setEdgeColor);
    GET_PROC_ADDRESS(DarkMode_setHotEdgeColor);
    GET_PROC_ADDRESS(DarkMode_setDisabledEdgeColor);
    GET_PROC_ADDRESS(DarkMode_setThemeColors);
    GET_PROC_ADDRESS(DarkMode_updateThemeBrushesAndPens);
    GET_PROC_ADDRESS(DarkMode_getBackgroundColor);
    GET_PROC_ADDRESS(DarkMode_getCtrlBackgroundColor);
    GET_PROC_ADDRESS(DarkMode_getHotBackgroundColor);
    GET_PROC_ADDRESS(DarkMode_getDlgBackgroundColor);
    GET_PROC_ADDRESS(DarkMode_getErrorBackgroundColor);
    GET_PROC_ADDRESS(DarkMode_getTextColor);
    GET_PROC_ADDRESS(DarkMode_getDarkerTextColor);
    GET_PROC_ADDRESS(DarkMode_getDisabledTextColor);
    GET_PROC_ADDRESS(DarkMode_getLinkTextColor);
    GET_PROC_ADDRESS(DarkMode_getEdgeColor);
    GET_PROC_ADDRESS(DarkMode_getHotEdgeColor);
    GET_PROC_ADDRESS(DarkMode_getDisabledEdgeColor);
    GET_PROC_ADDRESS(DarkMode_getBackgroundBrush);
    GET_PROC_ADDRESS(DarkMode_getDlgBackgroundBrush);
    GET_PROC_ADDRESS(DarkMode_getCtrlBackgroundBrush);
    GET_PROC_ADDRESS(DarkMode_getHotBackgroundBrush);
    GET_PROC_ADDRESS(DarkMode_getErrorBackgroundBrush);
    GET_PROC_ADDRESS(DarkMode_getEdgeBrush);
    GET_PROC_ADDRESS(DarkMode_getHotEdgeBrush);
    GET_PROC_ADDRESS(DarkMode_getDisabledEdgeBrush);
    GET_PROC_ADDRESS(DarkMode_getDarkerTextPen);
    GET_PROC_ADDRESS(DarkMode_getEdgePen);
    GET_PROC_ADDRESS(DarkMode_getHotEdgePen);
    GET_PROC_ADDRESS(DarkMode_getDisabledEdgePen);
    GET_PROC_ADDRESS(DarkMode_setViewBackgroundColor);
    GET_PROC_ADDRESS(DarkMode_setViewTextColor);
    GET_PROC_ADDRESS(DarkMode_setViewGridlinesColor);
    GET_PROC_ADDRESS(DarkMode_setHeaderBackgroundColor);
    GET_PROC_ADDRESS(DarkMode_setHeaderHotBackgroundColor);
    GET_PROC_ADDRESS(DarkMode_setHeaderTextColor);
    GET_PROC_ADDRESS(DarkMode_setHeaderEdgeColor);
    GET_PROC_ADDRESS(DarkMode_setViewColors);
    GET_PROC_ADDRESS(DarkMode_updateViewBrushesAndPens);
    GET_PROC_ADDRESS(DarkMode_getViewBackgroundColor);
    GET_PROC_ADDRESS(DarkMode_getViewTextColor);
    GET_PROC_ADDRESS(DarkMode_getViewGridlinesColor);
    GET_PROC_ADDRESS(DarkMode_getHeaderBackgroundColor);
    GET_PROC_ADDRESS(DarkMode_getHeaderHotBackgroundColor);
    GET_PROC_ADDRESS(DarkMode_getHeaderTextColor);
    GET_PROC_ADDRESS(DarkMode_getHeaderEdgeColor);
    GET_PROC_ADDRESS(DarkMode_getViewBackgroundBrush);
    GET_PROC_ADDRESS(DarkMode_getViewGridlinesBrush);
    GET_PROC_ADDRESS(DarkMode_getHeaderBackgroundBrush);
    GET_PROC_ADDRESS(DarkMode_getHeaderHotBackgroundBrush);
    GET_PROC_ADDRESS(DarkMode_getHeaderEdgePen);
    GET_PROC_ADDRESS(DarkMode_setDefaultColors);
    GET_PROC_ADDRESS(DarkMode_paintRoundRect);
    GET_PROC_ADDRESS(DarkMode_paintRoundFrameRect);
    GET_PROC_ADDRESS(DarkMode_setCheckboxOrRadioBtnCtrlSubclass);
    GET_PROC_ADDRESS(DarkMode_removeCheckboxOrRadioBtnCtrlSubclass);
    GET_PROC_ADDRESS(DarkMode_setGroupboxCtrlSubclass);
    GET_PROC_ADDRESS(DarkMode_removeGroupboxCtrlSubclass);
    GET_PROC_ADDRESS(DarkMode_setUpDownCtrlSubclass);
    GET_PROC_ADDRESS(DarkMode_removeUpDownCtrlSubclass);
    GET_PROC_ADDRESS(DarkMode_setTabCtrlUpDownSubclass);
    GET_PROC_ADDRESS(DarkMode_removeTabCtrlUpDownSubclass);
    GET_PROC_ADDRESS(DarkMode_setTabCtrlSubclass);
    GET_PROC_ADDRESS(DarkMode_removeTabCtrlSubclass);
    GET_PROC_ADDRESS(DarkMode_setCustomBorderForListBoxOrEditCtrlSubclass);
    GET_PROC_ADDRESS(DarkMode_removeCustomBorderForListBoxOrEditCtrlSubclass);
    GET_PROC_ADDRESS(DarkMode_setComboBoxCtrlSubclass);
    GET_PROC_ADDRESS(DarkMode_removeComboBoxCtrlSubclass);
    GET_PROC_ADDRESS(DarkMode_setComboBoxExCtrlSubclass);
    GET_PROC_ADDRESS(DarkMode_removeComboBoxExCtrlSubclass);
    GET_PROC_ADDRESS(DarkMode_setListViewCtrlSubclass);
    GET_PROC_ADDRESS(DarkMode_removeListViewCtrlSubclass);
    GET_PROC_ADDRESS(DarkMode_setHeaderCtrlSubclass);
    GET_PROC_ADDRESS(DarkMode_removeHeaderCtrlSubclass);
    GET_PROC_ADDRESS(DarkMode_setStatusBarCtrlSubclass);
    GET_PROC_ADDRESS(DarkMode_removeStatusBarCtrlSubclass);
    GET_PROC_ADDRESS(DarkMode_setProgressBarCtrlSubclass);
    GET_PROC_ADDRESS(DarkMode_removeProgressBarCtrlSubclass);
    GET_PROC_ADDRESS(DarkMode_setStaticTextCtrlSubclass);
    GET_PROC_ADDRESS(DarkMode_removeStaticTextCtrlSubclass);
    GET_PROC_ADDRESS(DarkMode_setChildCtrlsSubclassAndTheme);
    GET_PROC_ADDRESS(DarkMode_setChildCtrlsTheme);
    GET_PROC_ADDRESS(DarkMode_setWindowEraseBgSubclass);
    GET_PROC_ADDRESS(DarkMode_removeWindowEraseBgSubclass);
    GET_PROC_ADDRESS(DarkMode_setWindowCtlColorSubclass);
    GET_PROC_ADDRESS(DarkMode_removeWindowCtlColorSubclass);
    GET_PROC_ADDRESS(DarkMode_setWindowNotifyCustomDrawSubclass);
    GET_PROC_ADDRESS(DarkMode_removeWindowNotifyCustomDrawSubclass);
    GET_PROC_ADDRESS(DarkMode_setWindowMenuBarSubclass);
    GET_PROC_ADDRESS(DarkMode_removeWindowMenuBarSubclass);
    GET_PROC_ADDRESS(DarkMode_setWindowSettingChangeSubclass);
    GET_PROC_ADDRESS(DarkMode_removeWindowSettingChangeSubclass);
    GET_PROC_ADDRESS(DarkMode_enableSysLinkCtrlCtlColor);
    GET_PROC_ADDRESS(DarkMode_setDarkTitleBarEx);
    GET_PROC_ADDRESS(DarkMode_setDarkTitleBar);
    GET_PROC_ADDRESS(DarkMode_setDarkThemeExperimental);
    GET_PROC_ADDRESS(DarkMode_setDarkExplorerTheme);
    GET_PROC_ADDRESS(DarkMode_setDarkScrollBar);
    GET_PROC_ADDRESS(DarkMode_setDarkTooltips);
    GET_PROC_ADDRESS(DarkMode_setDarkLineAbovePanelToolbar);
    GET_PROC_ADDRESS(DarkMode_setDarkListView);
    GET_PROC_ADDRESS(DarkMode_setDarkListViewCheckboxes);
    GET_PROC_ADDRESS(DarkMode_setDarkRichEdit);
    GET_PROC_ADDRESS(DarkMode_setDarkWndSafe);
    GET_PROC_ADDRESS(DarkMode_setDarkWndNotifySafeEx);
    GET_PROC_ADDRESS(DarkMode_setDarkWndNotifySafe);
    GET_PROC_ADDRESS(DarkMode_enableThemeDialogTexture);
    GET_PROC_ADDRESS(DarkMode_disableVisualStyle);
    GET_PROC_ADDRESS(DarkMode_calculatePerceivedLightness);
    GET_PROC_ADDRESS(DarkMode_getTreeViewStyle);
    GET_PROC_ADDRESS(DarkMode_calculateTreeViewStyle);
    GET_PROC_ADDRESS(DarkMode_setTreeViewWindowTheme);
    GET_PROC_ADDRESS(DarkMode_getPrevTreeViewStyle);
    GET_PROC_ADDRESS(DarkMode_setPrevTreeViewStyle);
    GET_PROC_ADDRESS(DarkMode_isThemeDark);
    GET_PROC_ADDRESS(DarkMode_isColorDark);
    GET_PROC_ADDRESS(DarkMode_redrawWindowFrame);
    GET_PROC_ADDRESS(DarkMode_setWindowStyle);
    GET_PROC_ADDRESS(DarkMode_setWindowExStyle);
    GET_PROC_ADDRESS(DarkMode_replaceExEdgeWithBorder);
    GET_PROC_ADDRESS(DarkMode_replaceClientEdgeWithBorderSafe);
    GET_PROC_ADDRESS(DarkMode_setProgressBarClassicTheme);
    GET_PROC_ADDRESS(DarkMode_onCtlColor);
    GET_PROC_ADDRESS(DarkMode_onCtlColorCtrl);
    GET_PROC_ADDRESS(DarkMode_onCtlColorDlg);
    GET_PROC_ADDRESS(DarkMode_onCtlColorError);
    GET_PROC_ADDRESS(DarkMode_onCtlColorDlgStaticText);
    GET_PROC_ADDRESS(DarkMode_onCtlColorDlgLinkText);
    GET_PROC_ADDRESS(DarkMode_onCtlColorListbox);
    _DarkMode_HookDlgProc = (PFN_DarkMode_HookDlgProc)GetProcAddress(g_hDarkModeLib, "DarkMode_HookDlgProc");
    if (!_DarkMode_HookDlgProc)
    {
        DarkMode_FreeLibrary();
        return FALSE;
    }

#undef GET_PROC_ADDRESS

    return TRUE;
}

void DarkMode_FreeLibrary()
{
    if (g_hDarkModeLib)
    {
        FreeLibrary(g_hDarkModeLib);
        g_hDarkModeLib = NULL;
    }

    _DarkMode_getLibInfo = NULL;

    _DarkMode_initDarkModeConfig = NULL;
    _DarkMode_setRoundCornerConfig = NULL;
    _DarkMode_setBorderColorConfig = NULL;
    _DarkMode_setMicaConfig = NULL;
    _DarkMode_setMicaExtendedConfig = NULL;
    _DarkMode_setColorizeTitleBarConfig = NULL;
    _DarkMode_setDarkModeConfig = NULL;
    _DarkMode_setDarkModeConfig_System = NULL;
    _DarkMode_initDarkMode = NULL;
    _DarkMode_initDarkMode_NoIni = NULL;

    _DarkMode_isEnabled = NULL;
    _DarkMode_isExperimentalActive = NULL;
    _DarkMode_isExperimentalSupported = NULL;
    _DarkMode_isWindowsModeEnabled = NULL;
    _DarkMode_isAtLeastWindows10 = NULL;
    _DarkMode_isAtLeastWindows11 = NULL;
    _DarkMode_getWindowsBuildNumber = NULL;

    _DarkMode_handleSettingChange = NULL;
    _DarkMode_isDarkModeReg = NULL;

    _DarkMode_setSysColor = NULL;

    _DarkMode_enableDarkScrollBarForWindowAndChildren = NULL;

    _DarkMode_setColorTone = NULL;
    _DarkMode_getColorTone = NULL;
    _DarkMode_setBackgroundColor = NULL;
    _DarkMode_setCtrlBackgroundColor = NULL;
    _DarkMode_setHotBackgroundColor = NULL;
    _DarkMode_setDlgBackgroundColor = NULL;
    _DarkMode_setErrorBackgroundColor = NULL;
    _DarkMode_setTextColor = NULL;
    _DarkMode_setDarkerTextColor = NULL;
    _DarkMode_setDisabledTextColor = NULL;
    _DarkMode_setLinkTextColor = NULL;
    _DarkMode_setEdgeColor = NULL;
    _DarkMode_setHotEdgeColor = NULL;
    _DarkMode_setDisabledEdgeColor = NULL;
    _DarkMode_setThemeColors = NULL;
    _DarkMode_updateThemeBrushesAndPens = NULL;
    _DarkMode_getBackgroundColor = NULL;
    _DarkMode_getCtrlBackgroundColor = NULL;
    _DarkMode_getHotBackgroundColor = NULL;
    _DarkMode_getDlgBackgroundColor = NULL;
    _DarkMode_getErrorBackgroundColor = NULL;
    _DarkMode_getTextColor = NULL;
    _DarkMode_getDarkerTextColor = NULL;
    _DarkMode_getDisabledTextColor = NULL;
    _DarkMode_getLinkTextColor = NULL;
    _DarkMode_getEdgeColor = NULL;
    _DarkMode_getHotEdgeColor = NULL;
    _DarkMode_getDisabledEdgeColor = NULL;
    _DarkMode_getBackgroundBrush = NULL;
    _DarkMode_getDlgBackgroundBrush = NULL;
    _DarkMode_getCtrlBackgroundBrush = NULL;
    _DarkMode_getHotBackgroundBrush = NULL;
    _DarkMode_getErrorBackgroundBrush = NULL;
    _DarkMode_getEdgeBrush = NULL;
    _DarkMode_getHotEdgeBrush = NULL;
    _DarkMode_getDisabledEdgeBrush = NULL;
    _DarkMode_getDarkerTextPen = NULL;
    _DarkMode_getEdgePen = NULL;
    _DarkMode_getHotEdgePen = NULL;
    _DarkMode_getDisabledEdgePen = NULL;
    _DarkMode_setViewBackgroundColor = NULL;
    _DarkMode_setViewTextColor = NULL;
    _DarkMode_setViewGridlinesColor = NULL;
    _DarkMode_setHeaderBackgroundColor = NULL;
    _DarkMode_setHeaderHotBackgroundColor = NULL;
    _DarkMode_setHeaderTextColor = NULL;
    _DarkMode_setHeaderEdgeColor = NULL;
    _DarkMode_setViewColors = NULL;
    _DarkMode_updateViewBrushesAndPens = NULL;
    _DarkMode_getViewBackgroundColor = NULL;
    _DarkMode_getViewTextColor = NULL;
    _DarkMode_getViewGridlinesColor = NULL;
    _DarkMode_getHeaderBackgroundColor = NULL;
    _DarkMode_getHeaderHotBackgroundColor = NULL;
    _DarkMode_getHeaderTextColor = NULL;
    _DarkMode_getHeaderEdgeColor = NULL;
    _DarkMode_getViewBackgroundBrush = NULL;
    _DarkMode_getViewGridlinesBrush = NULL;
    _DarkMode_getHeaderBackgroundBrush = NULL;
    _DarkMode_getHeaderHotBackgroundBrush = NULL;
    _DarkMode_getHeaderEdgePen = NULL;
    _DarkMode_setDefaultColors = NULL;

    _DarkMode_paintRoundRect = NULL;
    _DarkMode_paintRoundFrameRect = NULL;

    _DarkMode_setCheckboxOrRadioBtnCtrlSubclass = NULL;
    _DarkMode_removeCheckboxOrRadioBtnCtrlSubclass = NULL;
    _DarkMode_setGroupboxCtrlSubclass = NULL;
    _DarkMode_removeGroupboxCtrlSubclass = NULL;
    _DarkMode_setUpDownCtrlSubclass = NULL;
    _DarkMode_removeUpDownCtrlSubclass = NULL;
    _DarkMode_setTabCtrlUpDownSubclass = NULL;
    _DarkMode_removeTabCtrlUpDownSubclass = NULL;
    _DarkMode_setTabCtrlSubclass = NULL;
    _DarkMode_removeTabCtrlSubclass = NULL;
    _DarkMode_setCustomBorderForListBoxOrEditCtrlSubclass = NULL;
    _DarkMode_removeCustomBorderForListBoxOrEditCtrlSubclass = NULL;
    _DarkMode_setComboBoxCtrlSubclass = NULL;
    _DarkMode_removeComboBoxCtrlSubclass = NULL;
    _DarkMode_setComboBoxExCtrlSubclass = NULL;
    _DarkMode_removeComboBoxExCtrlSubclass = NULL;
    _DarkMode_setListViewCtrlSubclass = NULL;
    _DarkMode_removeListViewCtrlSubclass = NULL;
    _DarkMode_setHeaderCtrlSubclass = NULL;
    _DarkMode_removeHeaderCtrlSubclass = NULL;
    _DarkMode_setStatusBarCtrlSubclass = NULL;
    _DarkMode_removeStatusBarCtrlSubclass = NULL;
    _DarkMode_setProgressBarCtrlSubclass = NULL;
    _DarkMode_removeProgressBarCtrlSubclass = NULL;
    _DarkMode_setStaticTextCtrlSubclass = NULL;
    _DarkMode_removeStaticTextCtrlSubclass = NULL;

    _DarkMode_setChildCtrlsSubclassAndTheme = NULL;
    _DarkMode_setChildCtrlsTheme = NULL;

    _DarkMode_setWindowEraseBgSubclass = NULL;
    _DarkMode_removeWindowEraseBgSubclass = NULL;
    _DarkMode_setWindowCtlColorSubclass = NULL;
    _DarkMode_removeWindowCtlColorSubclass = NULL;
    _DarkMode_setWindowNotifyCustomDrawSubclass = NULL;
    _DarkMode_removeWindowNotifyCustomDrawSubclass = NULL;
    _DarkMode_setWindowMenuBarSubclass = NULL;
    _DarkMode_removeWindowMenuBarSubclass = NULL;
    _DarkMode_setWindowSettingChangeSubclass = NULL;
    _DarkMode_removeWindowSettingChangeSubclass = NULL;

    _DarkMode_enableSysLinkCtrlCtlColor = NULL;
    _DarkMode_setDarkTitleBarEx = NULL;
    _DarkMode_setDarkTitleBar = NULL;
    _DarkMode_setDarkThemeExperimental = NULL;
    _DarkMode_setDarkExplorerTheme = NULL;
    _DarkMode_setDarkScrollBar = NULL;
    _DarkMode_setDarkTooltips = NULL;
    _DarkMode_setDarkLineAbovePanelToolbar = NULL;
    _DarkMode_setDarkListView = NULL;
    _DarkMode_setDarkListViewCheckboxes = NULL;
    _DarkMode_setDarkRichEdit = NULL;
    _DarkMode_setDarkWndSafe = NULL;
    _DarkMode_setDarkWndNotifySafeEx = NULL;
    _DarkMode_setDarkWndNotifySafe = NULL;
    _DarkMode_enableThemeDialogTexture = NULL;
    _DarkMode_disableVisualStyle = NULL;
    _DarkMode_calculatePerceivedLightness = NULL;
    _DarkMode_getTreeViewStyle = NULL;
    _DarkMode_calculateTreeViewStyle = NULL;
    _DarkMode_setTreeViewWindowTheme = NULL;
    _DarkMode_getPrevTreeViewStyle = NULL;
    _DarkMode_setPrevTreeViewStyle = NULL;
    _DarkMode_isThemeDark = NULL;
    _DarkMode_isColorDark = NULL;
    _DarkMode_redrawWindowFrame = NULL;
    _DarkMode_setWindowStyle = NULL;
    _DarkMode_setWindowExStyle = NULL;
    _DarkMode_replaceExEdgeWithBorder = NULL;
    _DarkMode_replaceClientEdgeWithBorderSafe = NULL;
    _DarkMode_setProgressBarClassicTheme = NULL;

    _DarkMode_onCtlColor = NULL;
    _DarkMode_onCtlColorCtrl = NULL;
    _DarkMode_onCtlColorDlg = NULL;
    _DarkMode_onCtlColorError = NULL;
    _DarkMode_onCtlColorDlgStaticText = NULL;
    _DarkMode_onCtlColorDlgLinkText = NULL;
    _DarkMode_onCtlColorListbox = NULL;
    _DarkMode_HookDlgProc = NULL;
}

int DarkMode_getLibInfo(DarkMode_LibInfo libInfoType)
{
    if (_DarkMode_getLibInfo)
    {
        return _DarkMode_getLibInfo(libInfoType);
    }
    return 0;
}

void DarkMode_initDarkModeConfig(UINT dmType)
{
    if (_DarkMode_initDarkModeConfig)
    {
        _DarkMode_initDarkModeConfig(dmType);
    }
}
void DarkMode_setRoundCornerConfig(UINT roundCornerStyle)
{
    if (_DarkMode_setRoundCornerConfig)
    {
        _DarkMode_setRoundCornerConfig(roundCornerStyle);
    }
}
void DarkMode_setBorderColorConfig(COLORREF clr)
{
    if (_DarkMode_setBorderColorConfig)
    {
        _DarkMode_setBorderColorConfig(clr);
    }
}
void DarkMode_setMicaConfig(UINT mica)
{
    if (_DarkMode_setMicaConfig)
    {
        _DarkMode_setMicaConfig(mica);
    }
}
void DarkMode_setMicaExtendedConfig(BOOL extendMica)
{
    if (_DarkMode_setMicaExtendedConfig)
    {
        _DarkMode_setMicaExtendedConfig(extendMica);
    }
}
void DarkMode_setColorizeTitleBarConfig(BOOL colorize)
{
    if (_DarkMode_setColorizeTitleBarConfig)
    {
        _DarkMode_setColorizeTitleBarConfig(colorize);
    }
}
void DarkMode_setDarkModeConfig(UINT dmType)
{
    if (_DarkMode_setDarkModeConfig)
    {
        _DarkMode_setDarkModeConfig(dmType);
    }
}
void DarkMode_setDarkModeConfig_System()
{
    if (_DarkMode_setDarkModeConfig_System)
    {
        _DarkMode_setDarkModeConfig_System();
    }
}
void DarkMode_initDarkMode(const wchar_t *iniName)
{
    if (_DarkMode_initDarkMode)
    {
        _DarkMode_initDarkMode(iniName);
    }
}
void DarkMode_initDarkMode_NoIni()
{
    if (_DarkMode_initDarkMode_NoIni)
    {
        _DarkMode_initDarkMode_NoIni();
    }
}

BOOL DarkMode_isEnabled()
{
    if (_DarkMode_isEnabled)
    {
        return _DarkMode_isEnabled();
    }
    return FALSE;
}
BOOL DarkMode_isExperimentalActive()
{
    if (_DarkMode_isExperimentalActive)
    {
        return _DarkMode_isExperimentalActive();
    }
    return FALSE;
}
BOOL DarkMode_isExperimentalSupported()
{
    if (_DarkMode_isExperimentalSupported)
    {
        return _DarkMode_isExperimentalSupported();
    }
    return FALSE;
}
BOOL DarkMode_isWindowsModeEnabled()
{
    if (_DarkMode_isWindowsModeEnabled)
    {
        return _DarkMode_isWindowsModeEnabled();
    }
    return FALSE;
}
BOOL DarkMode_isAtLeastWindows10()
{
    if (_DarkMode_isAtLeastWindows10)
    {
        return _DarkMode_isAtLeastWindows10();
    }
    return FALSE;
}
BOOL DarkMode_isAtLeastWindows11()
{
    if (_DarkMode_isAtLeastWindows11)
    {
        return _DarkMode_isAtLeastWindows11();
    }
    return FALSE;
}
DWORD DarkMode_getWindowsBuildNumber()
{
    if (_DarkMode_getWindowsBuildNumber)
    {
        return _DarkMode_getWindowsBuildNumber();
    }
    return 0;
}

BOOL DarkMode_handleSettingChange(LPARAM lParam)
{
    if (_DarkMode_handleSettingChange)
    {
        return _DarkMode_handleSettingChange(lParam);
    }
    return FALSE;
}
BOOL DarkMode_isDarkModeReg()
{
    if (_DarkMode_isDarkModeReg)
    {
        return _DarkMode_isDarkModeReg();
    }
    return FALSE;
}

void DarkMode_setSysColor(int nIndex, COLORREF color)
{
    if (_DarkMode_setSysColor)
    {
        _DarkMode_setSysColor(nIndex, color);
    }
}

void DarkMode_enableDarkScrollBarForWindowAndChildren(HWND hWnd)
{
    if (_DarkMode_enableDarkScrollBarForWindowAndChildren)
    {
        _DarkMode_enableDarkScrollBarForWindowAndChildren(hWnd);
    }
}

void DarkMode_setColorTone(DarkMode_ColorTone colorTone)
{
    if (_DarkMode_setColorTone)
    {
        _DarkMode_setColorTone(colorTone);
    }
}
DarkMode_ColorTone DarkMode_getColorTone()
{
    if (_DarkMode_getColorTone)
    {
        return _DarkMode_getColorTone();
    }
    return DarkMode_ColorTone_black;
}
COLORREF DarkMode_setBackgroundColor(COLORREF clrNew)
{
    if (_DarkMode_setBackgroundColor)
    {
        return _DarkMode_setBackgroundColor(clrNew);
    }
    return 0;
}
COLORREF DarkMode_setCtrlBackgroundColor(COLORREF clrNew)
{
    if (_DarkMode_setCtrlBackgroundColor)
    {
        return _DarkMode_setCtrlBackgroundColor(clrNew);
    }
    return 0;
}
COLORREF DarkMode_setHotBackgroundColor(COLORREF clrNew)
{
    if (_DarkMode_setHotBackgroundColor)
    {
        return _DarkMode_setHotBackgroundColor(clrNew);
    }
    return 0;
}
COLORREF DarkMode_setDlgBackgroundColor(COLORREF clrNew)
{
    if (_DarkMode_setDlgBackgroundColor)
    {
        return _DarkMode_setDlgBackgroundColor(clrNew);
    }
    return 0;
}
COLORREF DarkMode_setErrorBackgroundColor(COLORREF clrNew)
{
    if (_DarkMode_setErrorBackgroundColor)
    {
        return _DarkMode_setErrorBackgroundColor(clrNew);
    }
    return 0;
}
COLORREF DarkMode_setTextColor(COLORREF clrNew)
{
    if (_DarkMode_setTextColor)
    {
        return _DarkMode_setTextColor(clrNew);
    }
    return 0;
}
COLORREF DarkMode_setDarkerTextColor(COLORREF clrNew)
{
    if (_DarkMode_setDarkerTextColor)
    {
        return _DarkMode_setDarkerTextColor(clrNew);
    }
    return 0;
}
COLORREF DarkMode_setDisabledTextColor(COLORREF clrNew)
{
    if (_DarkMode_setDisabledTextColor)
    {
        return _DarkMode_setDisabledTextColor(clrNew);
    }
    return 0;
}
COLORREF DarkMode_setLinkTextColor(COLORREF clrNew)
{
    if (_DarkMode_setLinkTextColor)
    {
        return _DarkMode_setLinkTextColor(clrNew);
    }
    return 0;
}
COLORREF DarkMode_setEdgeColor(COLORREF clrNew)
{
    if (_DarkMode_setEdgeColor)
    {
        return _DarkMode_setEdgeColor(clrNew);
    }
    return 0;
}
COLORREF DarkMode_setHotEdgeColor(COLORREF clrNew)
{
    if (_DarkMode_setHotEdgeColor)
    {
        return _DarkMode_setHotEdgeColor(clrNew);
    }
    return 0;
}
COLORREF DarkMode_setDisabledEdgeColor(COLORREF clrNew)
{
    if (_DarkMode_setDisabledEdgeColor)
    {
        return _DarkMode_setDisabledEdgeColor(clrNew);
    }
    return 0;
}
void DarkMode_setThemeColors(DarkMode_Colors colors)
{
    if (_DarkMode_setThemeColors)
    {
        _DarkMode_setThemeColors(colors);
    }
}
void DarkMode_updateThemeBrushesAndPens()
{
    if (_DarkMode_updateThemeBrushesAndPens)
    {
        _DarkMode_updateThemeBrushesAndPens();
    }
}
COLORREF DarkMode_getBackgroundColor()
{
    if (_DarkMode_getBackgroundColor)
    {
        return _DarkMode_getBackgroundColor();
    }
    return 0;
}
COLORREF DarkMode_getCtrlBackgroundColor()
{
    if (_DarkMode_getCtrlBackgroundColor)
    {
        return _DarkMode_getCtrlBackgroundColor();
    }
    return 0;
}
COLORREF DarkMode_getHotBackgroundColor()
{
    if (_DarkMode_getHotBackgroundColor)
    {
        return _DarkMode_getHotBackgroundColor();
    }
    return 0;
}
COLORREF DarkMode_getDlgBackgroundColor()
{
    if (_DarkMode_getDlgBackgroundColor)
    {
        return _DarkMode_getDlgBackgroundColor();
    }
    return 0;
}
COLORREF DarkMode_getErrorBackgroundColor()
{
    if (_DarkMode_getErrorBackgroundColor)
    {
        return _DarkMode_getErrorBackgroundColor();
    }
    return 0;
}
COLORREF DarkMode_getTextColor()
{
    if (_DarkMode_getTextColor)
    {
        return _DarkMode_getTextColor();
    }
    return 0;
}
COLORREF DarkMode_getDarkerTextColor()
{
    if (_DarkMode_getDarkerTextColor)
    {
        return _DarkMode_getDarkerTextColor();
    }
    return 0;
}
COLORREF DarkMode_getDisabledTextColor()
{
    if (_DarkMode_getDisabledTextColor)
    {
        return _DarkMode_getDisabledTextColor();
    }
    return 0;
}
COLORREF DarkMode_getLinkTextColor()
{
    if (_DarkMode_getLinkTextColor)
    {
        return _DarkMode_getLinkTextColor();
    }
    return 0;
}
COLORREF DarkMode_getEdgeColor()
{
    if (_DarkMode_getEdgeColor)
    {
        return _DarkMode_getEdgeColor();
    }
    return 0;
}
COLORREF DarkMode_getHotEdgeColor()
{
    if (_DarkMode_getHotEdgeColor)
    {
        return _DarkMode_getHotEdgeColor();
    }
    return 0;
}
COLORREF DarkMode_getDisabledEdgeColor()
{
    if (_DarkMode_getDisabledEdgeColor)
    {
        return _DarkMode_getDisabledEdgeColor();
    }
    return 0;
}
HBRUSH DarkMode_getBackgroundBrush()
{
    if (_DarkMode_getBackgroundBrush)
    {
        return _DarkMode_getBackgroundBrush();
    }
    return NULL;
}
HBRUSH DarkMode_getDlgBackgroundBrush()
{
    if (_DarkMode_getDlgBackgroundBrush)
    {
        return _DarkMode_getDlgBackgroundBrush();
    }
    return NULL;
}
HBRUSH DarkMode_getCtrlBackgroundBrush()
{
    if (_DarkMode_getCtrlBackgroundBrush)
    {
        return _DarkMode_getCtrlBackgroundBrush();
    }
    return NULL;
}
HBRUSH DarkMode_getHotBackgroundBrush()
{
    if (_DarkMode_getHotBackgroundBrush)
    {
        return _DarkMode_getHotBackgroundBrush();
    }
    return NULL;
}
HBRUSH DarkMode_getErrorBackgroundBrush()
{
    if (_DarkMode_getErrorBackgroundBrush)
    {
        return _DarkMode_getErrorBackgroundBrush();
    }
    return NULL;
}
HBRUSH DarkMode_getEdgeBrush()
{
    if (_DarkMode_getEdgeBrush)
    {
        return _DarkMode_getEdgeBrush();
    }
    return NULL;
}
HBRUSH DarkMode_getHotEdgeBrush()
{
    if (_DarkMode_getHotEdgeBrush)
    {
        return _DarkMode_getHotEdgeBrush();
    }
    return NULL;
}
HBRUSH DarkMode_getDisabledEdgeBrush()
{
    if (_DarkMode_getDisabledEdgeBrush)
    {
        return _DarkMode_getDisabledEdgeBrush();
    }
    return NULL;
}
HPEN DarkMode_getDarkerTextPen()
{
    if (_DarkMode_getDarkerTextPen)
    {
        return _DarkMode_getDarkerTextPen();
    }
    return NULL;
}
HPEN DarkMode_getEdgePen()
{
    if (_DarkMode_getEdgePen)
    {
        return _DarkMode_getEdgePen();
    }
    return NULL;
}
HPEN DarkMode_getHotEdgePen()
{
    if (_DarkMode_getHotEdgePen)
    {
        return _DarkMode_getHotEdgePen();
    }
    return NULL;
}
HPEN DarkMode_getDisabledEdgePen()
{
    if (_DarkMode_getDisabledEdgePen)
    {
        return _DarkMode_getDisabledEdgePen();
    }
    return NULL;
}
COLORREF DarkMode_setViewBackgroundColor(COLORREF clrNew)
{
    if (_DarkMode_setViewBackgroundColor)
    {
        return _DarkMode_setViewBackgroundColor(clrNew);
    }
    return 0;
}
COLORREF DarkMode_setViewTextColor(COLORREF clrNew)
{
    if (_DarkMode_setViewTextColor)
    {
        return _DarkMode_setViewTextColor(clrNew);
    }
    return 0;
}
COLORREF DarkMode_setViewGridlinesColor(COLORREF clrNew)
{
    if (_DarkMode_setViewGridlinesColor)
    {
        return _DarkMode_setViewGridlinesColor(clrNew);
    }
    return 0;
}
COLORREF DarkMode_setHeaderBackgroundColor(COLORREF clrNew)
{
    if (_DarkMode_setHeaderBackgroundColor)
    {
        return _DarkMode_setHeaderBackgroundColor(clrNew);
    }
    return 0;
}
COLORREF DarkMode_setHeaderHotBackgroundColor(COLORREF clrNew)
{
    if (_DarkMode_setHeaderHotBackgroundColor)
    {
        return _DarkMode_setHeaderHotBackgroundColor(clrNew);
    }
    return 0;
}
COLORREF DarkMode_setHeaderTextColor(COLORREF clrNew)
{
    if (_DarkMode_setHeaderTextColor)
    {
        return _DarkMode_setHeaderTextColor(clrNew);
    }
    return 0;
}
COLORREF DarkMode_setHeaderEdgeColor(COLORREF clrNew)
{
    if (_DarkMode_setHeaderEdgeColor)
    {
        return _DarkMode_setHeaderEdgeColor(clrNew);
    }
    return 0;
}
void DarkMode_setViewColors(DarkMode_ColorsView colors)
{
    if (_DarkMode_setViewColors)
    {
        _DarkMode_setViewColors(colors);
    }
}
void DarkMode_updateViewBrushesAndPens()
{
    if (_DarkMode_updateViewBrushesAndPens)
    {
        _DarkMode_updateViewBrushesAndPens();
    }
}
COLORREF DarkMode_getViewBackgroundColor()
{
    if (_DarkMode_getViewBackgroundColor)
    {
        return _DarkMode_getViewBackgroundColor();
    }
    return 0;
}
COLORREF DarkMode_getViewTextColor()
{
    if (_DarkMode_getViewTextColor)
    {
        return _DarkMode_getViewTextColor();
    }
    return 0;
}
COLORREF DarkMode_getViewGridlinesColor()
{
    if (_DarkMode_getViewGridlinesColor)
    {
        return _DarkMode_getViewGridlinesColor();
    }
    return 0;
}
COLORREF DarkMode_getHeaderBackgroundColor()
{
    if (_DarkMode_getHeaderBackgroundColor)
    {
        return _DarkMode_getHeaderBackgroundColor();
    }
    return 0;
}
COLORREF DarkMode_getHeaderHotBackgroundColor()
{
    if (_DarkMode_getHeaderHotBackgroundColor)
    {
        return _DarkMode_getHeaderHotBackgroundColor();
    }
    return 0;
}
COLORREF DarkMode_getHeaderTextColor()
{
    if (_DarkMode_getHeaderTextColor)
    {
        return _DarkMode_getHeaderTextColor();
    }
    return 0;
}
COLORREF DarkMode_getHeaderEdgeColor()
{
    if (_DarkMode_getHeaderEdgeColor)
    {
        return _DarkMode_getHeaderEdgeColor();
    }
    return 0;
}
HBRUSH DarkMode_getViewBackgroundBrush()
{
    if (_DarkMode_getViewBackgroundBrush)
    {
        return _DarkMode_getViewBackgroundBrush();
    }
    return NULL;
}
HBRUSH DarkMode_getViewGridlinesBrush()
{
    if (_DarkMode_getViewGridlinesBrush)
    {
        return _DarkMode_getViewGridlinesBrush();
    }
    return NULL;
}
HBRUSH DarkMode_getHeaderBackgroundBrush()
{
    if (_DarkMode_getHeaderBackgroundBrush)
    {
        return _DarkMode_getHeaderBackgroundBrush();
    }
    return NULL;
}
HBRUSH DarkMode_getHeaderHotBackgroundBrush()
{
    if (_DarkMode_getHeaderHotBackgroundBrush)
    {
        return _DarkMode_getHeaderHotBackgroundBrush();
    }
    return NULL;
}
HPEN DarkMode_getHeaderEdgePen()
{
    if (_DarkMode_getHeaderEdgePen)
    {
        return _DarkMode_getHeaderEdgePen();
    }
    return NULL;
}
void DarkMode_setDefaultColors(BOOL updateBrushesAndOther)
{
    if (_DarkMode_setDefaultColors)
    {
        _DarkMode_setDefaultColors(updateBrushesAndOther);
    }
}

void DarkMode_paintRoundRect(HDC hdc, const RECT *rect, HPEN hpen, HBRUSH hBrush, int width, int height)
{
    if (_DarkMode_paintRoundRect)
    {
        _DarkMode_paintRoundRect(hdc, rect, hpen, hBrush, width, height);
    }
}
void DarkMode_paintRoundFrameRect(HDC hdc, const RECT *rect, HPEN hpen, int width, int height)
{
    if (_DarkMode_paintRoundFrameRect)
    {
        _DarkMode_paintRoundFrameRect(hdc, rect, hpen, width, height);
    }
}

void DarkMode_setCheckboxOrRadioBtnCtrlSubclass(HWND hWnd)
{
    if (_DarkMode_setCheckboxOrRadioBtnCtrlSubclass)
    {
        _DarkMode_setCheckboxOrRadioBtnCtrlSubclass(hWnd);
    }
}
void DarkMode_removeCheckboxOrRadioBtnCtrlSubclass(HWND hWnd)
{
    if (_DarkMode_removeCheckboxOrRadioBtnCtrlSubclass)
    {
        _DarkMode_removeCheckboxOrRadioBtnCtrlSubclass(hWnd);
    }
}
void DarkMode_setGroupboxCtrlSubclass(HWND hWnd)
{
    if (_DarkMode_setGroupboxCtrlSubclass)
    {
        _DarkMode_setGroupboxCtrlSubclass(hWnd);
    }
}
void DarkMode_removeGroupboxCtrlSubclass(HWND hWnd)
{
    if (_DarkMode_removeGroupboxCtrlSubclass)
    {
        _DarkMode_removeGroupboxCtrlSubclass(hWnd);
    }
}
void DarkMode_setUpDownCtrlSubclass(HWND hWnd)
{
    if (_DarkMode_setUpDownCtrlSubclass)
    {
        _DarkMode_setUpDownCtrlSubclass(hWnd);
    }
}
void DarkMode_removeUpDownCtrlSubclass(HWND hWnd)
{
    if (_DarkMode_removeUpDownCtrlSubclass)
    {
        _DarkMode_removeUpDownCtrlSubclass(hWnd);
    }
}
void DarkMode_setTabCtrlUpDownSubclass(HWND hWnd)
{
    if (_DarkMode_setTabCtrlUpDownSubclass)
    {
        _DarkMode_setTabCtrlUpDownSubclass(hWnd);
    }
}
void DarkMode_removeTabCtrlUpDownSubclass(HWND hWnd)
{
    if (_DarkMode_removeTabCtrlUpDownSubclass)
    {
        _DarkMode_removeTabCtrlUpDownSubclass(hWnd);
    }
}
void DarkMode_setTabCtrlSubclass(HWND hWnd)
{
    if (_DarkMode_setTabCtrlSubclass)
    {
        _DarkMode_setTabCtrlSubclass(hWnd);
    }
}
void DarkMode_removeTabCtrlSubclass(HWND hWnd)
{
    if (_DarkMode_removeTabCtrlSubclass)
    {
        _DarkMode_removeTabCtrlSubclass(hWnd);
    }
}
void DarkMode_setCustomBorderForListBoxOrEditCtrlSubclass(HWND hWnd)
{
    if (_DarkMode_setCustomBorderForListBoxOrEditCtrlSubclass)
    {
        _DarkMode_setCustomBorderForListBoxOrEditCtrlSubclass(hWnd);
    }
}
void DarkMode_removeCustomBorderForListBoxOrEditCtrlSubclass(HWND hWnd)
{
    if (_DarkMode_removeCustomBorderForListBoxOrEditCtrlSubclass)
    {
        _DarkMode_removeCustomBorderForListBoxOrEditCtrlSubclass(hWnd);
    }
}
void DarkMode_setComboBoxCtrlSubclass(HWND hWnd)
{
    if (_DarkMode_setComboBoxCtrlSubclass)
    {
        _DarkMode_setComboBoxCtrlSubclass(hWnd);
    }
}
void DarkMode_removeComboBoxCtrlSubclass(HWND hWnd)
{
    if (_DarkMode_removeComboBoxCtrlSubclass)
    {
        _DarkMode_removeComboBoxCtrlSubclass(hWnd);
    }
}
void DarkMode_setComboBoxExCtrlSubclass(HWND hWnd)
{
    if (_DarkMode_setComboBoxExCtrlSubclass)
    {
        _DarkMode_setComboBoxExCtrlSubclass(hWnd);
    }
}
void DarkMode_removeComboBoxExCtrlSubclass(HWND hWnd)
{
    if (_DarkMode_removeComboBoxExCtrlSubclass)
    {
        _DarkMode_removeComboBoxExCtrlSubclass(hWnd);
    }
}
void DarkMode_setListViewCtrlSubclass(HWND hWnd)
{
    if (_DarkMode_setListViewCtrlSubclass)
    {
        _DarkMode_setListViewCtrlSubclass(hWnd);
    }
}
void DarkMode_removeListViewCtrlSubclass(HWND hWnd)
{
    if (_DarkMode_removeListViewCtrlSubclass)
    {
        _DarkMode_removeListViewCtrlSubclass(hWnd);
    }
}
void DarkMode_setHeaderCtrlSubclass(HWND hWnd)
{
    if (_DarkMode_setHeaderCtrlSubclass)
    {
        _DarkMode_setHeaderCtrlSubclass(hWnd);
    }
}
void DarkMode_removeHeaderCtrlSubclass(HWND hWnd)
{
    if (_DarkMode_removeHeaderCtrlSubclass)
    {
        _DarkMode_removeHeaderCtrlSubclass(hWnd);
    }
}
void DarkMode_setStatusBarCtrlSubclass(HWND hWnd)
{
    if (_DarkMode_setStatusBarCtrlSubclass)
    {
        _DarkMode_setStatusBarCtrlSubclass(hWnd);
    }
}
void DarkMode_removeStatusBarCtrlSubclass(HWND hWnd)
{
    if (_DarkMode_removeStatusBarCtrlSubclass)
    {
        _DarkMode_removeStatusBarCtrlSubclass(hWnd);
    }
}
void DarkMode_setProgressBarCtrlSubclass(HWND hWnd)
{
    if (_DarkMode_setProgressBarCtrlSubclass)
    {
        _DarkMode_setProgressBarCtrlSubclass(hWnd);
    }
}
void DarkMode_removeProgressBarCtrlSubclass(HWND hWnd)
{
    if (_DarkMode_removeProgressBarCtrlSubclass)
    {
        _DarkMode_removeProgressBarCtrlSubclass(hWnd);
    }
}
void DarkMode_setStaticTextCtrlSubclass(HWND hWnd)
{
    if (_DarkMode_setStaticTextCtrlSubclass)
    {
        _DarkMode_setStaticTextCtrlSubclass(hWnd);
    }
}
void DarkMode_removeStaticTextCtrlSubclass(HWND hWnd)
{
    if (_DarkMode_removeStaticTextCtrlSubclass)
    {
        _DarkMode_removeStaticTextCtrlSubclass(hWnd);
    }
}

void DarkMode_setChildCtrlsSubclassAndTheme(HWND hParent, BOOL subclass, BOOL theme)
{
    if (_DarkMode_setChildCtrlsSubclassAndTheme)
    {
        _DarkMode_setChildCtrlsSubclassAndTheme(hParent, subclass, theme);
    }
}
void DarkMode_setChildCtrlsTheme(HWND hParent)
{
    if (_DarkMode_setChildCtrlsTheme)
    {
        _DarkMode_setChildCtrlsTheme(hParent);
    }
}

void DarkMode_setWindowEraseBgSubclass(HWND hWnd)
{
    if (_DarkMode_setWindowEraseBgSubclass)
    {
        _DarkMode_setWindowEraseBgSubclass(hWnd);
    }
}
void DarkMode_removeWindowEraseBgSubclass(HWND hWnd)
{
    if (_DarkMode_removeWindowEraseBgSubclass)
    {
        _DarkMode_removeWindowEraseBgSubclass(hWnd);
    }
}
void DarkMode_setWindowCtlColorSubclass(HWND hWnd)
{
    if (_DarkMode_setWindowCtlColorSubclass)
    {
        _DarkMode_setWindowCtlColorSubclass(hWnd);
    }
}
void DarkMode_removeWindowCtlColorSubclass(HWND hWnd)
{
    if (_DarkMode_removeWindowCtlColorSubclass)
    {
        _DarkMode_removeWindowCtlColorSubclass(hWnd);
    }
}
void DarkMode_setWindowNotifyCustomDrawSubclass(HWND hWnd)
{
    if (_DarkMode_setWindowNotifyCustomDrawSubclass)
    {
        _DarkMode_setWindowNotifyCustomDrawSubclass(hWnd);
    }
}
void DarkMode_removeWindowNotifyCustomDrawSubclass(HWND hWnd)
{
    if (_DarkMode_removeWindowNotifyCustomDrawSubclass)
    {
        _DarkMode_removeWindowNotifyCustomDrawSubclass(hWnd);
    }
}
void DarkMode_setWindowMenuBarSubclass(HWND hWnd)
{
    if (_DarkMode_setWindowMenuBarSubclass)
    {
        _DarkMode_setWindowMenuBarSubclass(hWnd);
    }
}
void DarkMode_removeWindowMenuBarSubclass(HWND hWnd)
{
    if (_DarkMode_removeWindowMenuBarSubclass)
    {
        _DarkMode_removeWindowMenuBarSubclass(hWnd);
    }
}
void DarkMode_setWindowSettingChangeSubclass(HWND hWnd)
{
    if (_DarkMode_setWindowSettingChangeSubclass)
    {
        _DarkMode_setWindowSettingChangeSubclass(hWnd);
    }
}
void DarkMode_removeWindowSettingChangeSubclass(HWND hWnd)
{
    if (_DarkMode_removeWindowSettingChangeSubclass)
    {
        _DarkMode_removeWindowSettingChangeSubclass(hWnd);
    }
}

void DarkMode_enableSysLinkCtrlCtlColor(HWND hWnd)
{
    if (_DarkMode_enableSysLinkCtrlCtlColor)
    {
        _DarkMode_enableSysLinkCtrlCtlColor(hWnd);
    }
}
void DarkMode_setDarkTitleBarEx(HWND hWnd, BOOL useWin11Features)
{
    if (_DarkMode_setDarkTitleBarEx)
    {
        _DarkMode_setDarkTitleBarEx(hWnd, useWin11Features);
    }
}
void DarkMode_setDarkTitleBar(HWND hWnd)
{
    if (_DarkMode_setDarkTitleBar)
    {
        _DarkMode_setDarkTitleBar(hWnd);
    }
}
void DarkMode_setDarkThemeExperimental(HWND hWnd, const wchar_t *themeClassName)
{
    if (_DarkMode_setDarkThemeExperimental)
    {
        _DarkMode_setDarkThemeExperimental(hWnd, themeClassName);
    }
}
void DarkMode_setDarkExplorerTheme(HWND hWnd)
{
    if (_DarkMode_setDarkExplorerTheme)
    {
        _DarkMode_setDarkExplorerTheme(hWnd);
    }
}
void DarkMode_setDarkScrollBar(HWND hWnd)
{
    if (_DarkMode_setDarkScrollBar)
    {
        _DarkMode_setDarkScrollBar(hWnd);
    }
}
void DarkMode_setDarkTooltips(HWND hWnd, DarkMode_ToolTipsType type)
{
    if (_DarkMode_setDarkTooltips)
    {
        _DarkMode_setDarkTooltips(hWnd, type);
    }
}
void DarkMode_setDarkLineAbovePanelToolbar(HWND hWnd)
{
    if (_DarkMode_setDarkLineAbovePanelToolbar)
    {
        _DarkMode_setDarkLineAbovePanelToolbar(hWnd);
    }
}
void DarkMode_setDarkListView(HWND hWnd)
{
    if (_DarkMode_setDarkListView)
    {
        _DarkMode_setDarkListView(hWnd);
    }
}
void DarkMode_setDarkListViewCheckboxes(HWND hWnd)
{
    if (_DarkMode_setDarkListViewCheckboxes)
    {
        _DarkMode_setDarkListViewCheckboxes(hWnd);
    }
}
void DarkMode_setDarkRichEdit(HWND hWnd)
{
    if (_DarkMode_setDarkRichEdit)
    {
        _DarkMode_setDarkRichEdit(hWnd);
    }
}
void DarkMode_setDarkWndSafe(HWND hWnd, BOOL useWin11Features)
{
    if (_DarkMode_setDarkWndSafe)
    {
        _DarkMode_setDarkWndSafe(hWnd, useWin11Features);
    }
}
void DarkMode_setDarkWndNotifySafeEx(HWND hWnd, BOOL setSettingChangeSubclass, BOOL useWin11Features)
{
    if (_DarkMode_setDarkWndNotifySafeEx)
    {
        _DarkMode_setDarkWndNotifySafeEx(hWnd, setSettingChangeSubclass, useWin11Features);
    }
}
void DarkMode_setDarkWndNotifySafe(HWND hWnd, BOOL useWin11Features)
{
    if (_DarkMode_setDarkWndNotifySafe)
    {
        _DarkMode_setDarkWndNotifySafe(hWnd, useWin11Features);
    }
}
void DarkMode_enableThemeDialogTexture(HWND hWnd, BOOL theme)
{
    if (_DarkMode_enableThemeDialogTexture)
    {
        _DarkMode_enableThemeDialogTexture(hWnd, theme);
    }
}
void DarkMode_disableVisualStyle(HWND hWnd, BOOL doDisable)
{
    if (_DarkMode_disableVisualStyle)
    {
        _DarkMode_disableVisualStyle(hWnd, doDisable);
    }
}
double DarkMode_calculatePerceivedLightness(COLORREF clr)
{
    if (_DarkMode_calculatePerceivedLightness)
    {
        return _DarkMode_calculatePerceivedLightness(clr);
    }
    return 0.0;
}
DarkMode_TreeViewStyle DarkMode_getTreeViewStyle()
{
    if (_DarkMode_getTreeViewStyle)
    {
        return _DarkMode_getTreeViewStyle();
    }
    return DarkMode_TreeViewStyle_classic;
}
void DarkMode_calculateTreeViewStyle()
{
    if (_DarkMode_calculateTreeViewStyle)
    {
        _DarkMode_calculateTreeViewStyle();
    }
}
void DarkMode_setTreeViewWindowTheme(HWND hWnd, BOOL force)
{
    if (_DarkMode_setTreeViewWindowTheme)
    {
        _DarkMode_setTreeViewWindowTheme(hWnd, force);
    }
}
DarkMode_TreeViewStyle DarkMode_getPrevTreeViewStyle()
{
    if (_DarkMode_getPrevTreeViewStyle)
    {
        return _DarkMode_getPrevTreeViewStyle();
    }
    return DarkMode_TreeViewStyle_classic;
}
void DarkMode_setPrevTreeViewStyle()
{
    if (_DarkMode_setPrevTreeViewStyle)
    {
        _DarkMode_setPrevTreeViewStyle();
    }
}
BOOL DarkMode_isThemeDark()
{
    if (_DarkMode_isThemeDark)
    {
        return _DarkMode_isThemeDark();
    }
    return FALSE;
}
BOOL DarkMode_isColorDark(COLORREF clr)
{
    if (_DarkMode_isColorDark)
    {
        return _DarkMode_isColorDark(clr);
    }
    return FALSE;
}
void DarkMode_redrawWindowFrame(HWND hWnd)
{
    if (_DarkMode_redrawWindowFrame)
    {
        _DarkMode_redrawWindowFrame(hWnd);
    }
}
void DarkMode_setWindowStyle(HWND hWnd, BOOL setStyle, LONG_PTR styleFlag)
{
    if (_DarkMode_setWindowStyle)
    {
        _DarkMode_setWindowStyle(hWnd, setStyle, styleFlag);
    }
}
void DarkMode_setWindowExStyle(HWND hWnd, BOOL setExStyle, LONG_PTR exStyleFlag)
{
    if (_DarkMode_setWindowExStyle)
    {
        _DarkMode_setWindowExStyle(hWnd, setExStyle, exStyleFlag);
    }
}
void DarkMode_replaceExEdgeWithBorder(HWND hWnd, BOOL replace, LONG_PTR exStyleFlag)
{
    if (_DarkMode_replaceExEdgeWithBorder)
    {
        _DarkMode_replaceExEdgeWithBorder(hWnd, replace, exStyleFlag);
    }
}
void DarkMode_replaceClientEdgeWithBorderSafe(HWND hWnd)
{
    if (_DarkMode_replaceClientEdgeWithBorderSafe)
    {
        _DarkMode_replaceClientEdgeWithBorderSafe(hWnd);
    }
}
void DarkMode_setProgressBarClassicTheme(HWND hWnd)
{
    if (_DarkMode_setProgressBarClassicTheme)
    {
        _DarkMode_setProgressBarClassicTheme(hWnd);
    }
}

LRESULT DarkMode_onCtlColor(HDC hdc)
{
    if (_DarkMode_onCtlColor)
    {
        return _DarkMode_onCtlColor(hdc);
    }
    return 0;
}
LRESULT DarkMode_onCtlColorCtrl(HDC hdc)
{
    if (_DarkMode_onCtlColorCtrl)
    {
        return _DarkMode_onCtlColorCtrl(hdc);
    }
    return 0;
}
LRESULT DarkMode_onCtlColorDlg(HDC hdc)
{
    if (_DarkMode_onCtlColorDlg)
    {
        return _DarkMode_onCtlColorDlg(hdc);
    }
    return 0;
}
LRESULT DarkMode_onCtlColorError(HDC hdc)
{
    if (_DarkMode_onCtlColorError)
    {
        return _DarkMode_onCtlColorError(hdc);
    }
    return 0;
}
LRESULT DarkMode_onCtlColorDlgStaticText(HDC hdc, BOOL isTextEnabled)
{
    if (_DarkMode_onCtlColorDlgStaticText)
    {
        return _DarkMode_onCtlColorDlgStaticText(hdc, isTextEnabled);
    }
    return 0;
}
LRESULT DarkMode_onCtlColorDlgLinkText(HDC hdc, BOOL isTextEnabled)
{
    if (_DarkMode_onCtlColorDlgLinkText)
    {
        return _DarkMode_onCtlColorDlgLinkText(hdc, isTextEnabled);
    }
    return 0;
}
LRESULT DarkMode_onCtlColorListbox(WPARAM wParam, LPARAM lParam)
{
    if (_DarkMode_onCtlColorListbox)
    {
        return _DarkMode_onCtlColorListbox(wParam, lParam);
    }
    return 0;
}

UINT_PTR CALLBACK DarkMode_HookDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (_DarkMode_HookDlgProc)
    {
        return _DarkMode_HookDlgProc(hWnd, uMsg, wParam, lParam);
    }
    return (UINT_PTR)0;
}