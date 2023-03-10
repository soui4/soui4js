#pragma once

#include <qjsbind.h>
#include <event/SEvents.h>
#include <core/SDefine.h>
using namespace SOUI;

#define SEnum(x) module->ExportEnumInt32(#x, x)
#define SEnum2(x) module->ExportEnumUint32(#x, x)

void Exp_SEnum(qjsbind::Module* module){
//soui events
SEnum(EVT_INIT);
SEnum(EVT_EXIT);
SEnum(EVT_TIMER);
SEnum(EVT_SETFOCUS);
SEnum(EVT_KILLFOCUS);
SEnum(EVT_CREATE);
SEnum(EVT_INIT_FINISH);
SEnum(EVT_DESTROY);
SEnum(EVT_SIZE);
SEnum(EVT_VISIBLECHANGED);
SEnum(EVT_STATECHANGED);
SEnum(EVT_LBUTTONDOWN);
SEnum(EVT_LBUTTONUP);
SEnum(EVT_UPDATE_TOOLTIP);
SEnum(EVT_ANIMATION_START);
SEnum(EVT_ANIMATION_STOP);
SEnum(EVT_ANIMATION_REPEAT);
SEnum(EVT_POS);
SEnum(EVT_KEYDOWN);
SEnum(EVT_MOUSE_HOVER);
SEnum(EVT_MOUSE_LEAVE);
SEnum(EVT_CMD);
SEnum(EVT_CTXMENU);
SEnum(EVT_SCROLLVIEW_ORIGINCHANGED);
SEnum(EVT_SCROLLVIEW_SIZECHANGED);
SEnum(EVT_SCROLL);
SEnum(EVT_OFEVENT);
SEnum(EVT_OFPANEL);
SEnum(EVT_ITEMPANEL_CLICK);
SEnum(EVT_ITEMPANEL_RCLICK);
SEnum(EVT_ITEMPANEL_CLICK_UP);
SEnum(EVT_ITEMPANEL_RCLICK_UP);
SEnum(EVT_ITEMPANEL_DBCLICK);
SEnum(EVT_ITEMPANEL_HOVER);
SEnum(EVT_ITEMPANEL_LEAVE);
SEnum(EVT_RADIOGROUP_CHECK_CHANGED);
SEnum(EVT_TAB_SELCHANGING);
SEnum(EVT_TAB_SELCHANGED);
SEnum(EVT_TAB_ITEMHOVER);
SEnum(EVT_TAB_ITEMLEAVE);
SEnum(EVT_LB_SELCHANGING);
SEnum(EVT_LB_SELCHANGED);
SEnum(EVT_LB_DBCLICK);
SEnum(EVT_LC_SELCHANGING);
SEnum(EVT_LC_SELCHANGED);
SEnum(EVT_LC_ITEMDELETED);
SEnum(EVT_LC_DBCLICK);
SEnum(EVT_TV_SELCHANGING);
SEnum(EVT_TV_SELCHANGED);
SEnum(EVT_TC_SELCHANGING);
SEnum(EVT_TC_SELCHANGED);
SEnum(EVT_TC_EXPAND);
SEnum(EVT_TC_CHECKSTATE);
SEnum(EVT_TC_DBCLICK);
SEnum(EVT_CB_BEFORE_CLOSEUP);
SEnum(EVT_LV_SELCHANGING);
SEnum(EVT_LV_SELCHANGED);
SEnum(EVT_LV_ITEMCLICK);
SEnum(EVT_RE_NOTIFY);
SEnum(EVT_RE_MENU);
SEnum(EVT_SLIDER_POS);
SEnum(EVT_HEADER_CLICK);
SEnum(EVT_HEADER_ITEMCHANGING);
SEnum(EVT_HEADER_ITEMCHANGED);
SEnum(EVT_HEADER_ITEMSWAP);
SEnum(EVT_HEADER_RELAYOUT);
SEnum(EVT_CB_SELCHANGE);
SEnum(EVT_CB_DROPDOWN);
SEnum(EVT_CALENDAR_SELDAY);
SEnum(EVT_CALENDAR_SETDATE);
SEnum(EVT_CALENDAREX_CHANGED);
SEnum(EVT_DATETIME_CHANGED);
SEnum(EVT_SPIN_VALUE2STRING);
SEnum(EVT_SPLIT_PANE_MOVED);
SEnum(EVT_HOT_KEY_SET);
SEnum(EVT_IMAGE_ANI_START);
SEnum(EVT_IMAGE_ANI_STOP);
SEnum(EVT_SELECTMENU);
SEnum(EVT_POPMENU);
SEnum(EVT_EXTERNAL_BEGIN);

SEnum2(STVI_ROOT);
SEnum2(STVI_FIRST);
SEnum2(STVI_LAST);

SEnum2(TVC_COLLAPSE);
SEnum2(TVC_EXPAND);
SEnum2(TVC_TOGGLE);

SEnum2(WndState_Normal);
SEnum2(WndState_Hover);
SEnum2(WndState_PushDown);
SEnum2(WndState_Check);
SEnum2(WndState_Invisible);
SEnum2(WndState_Disable);
}