#pragma once
#include <control/souictrls.h>
#include "toobj.h"

template<typename T1, typename T2>
T1* QueryICtrl(IWindow* pWnd) {
	if (pWnd->IsClass(T2::GetClassName()))
	{
		IObjRef* pCtrl = NULL;
		pWnd->QueryInterface(__uuidof(T1), &pCtrl);
		if (!pCtrl) return NULL;
		return (T1*)pCtrl;
	}
	return NULL;
}

#ifndef DEF_QICTRL
#define DEF_QICTRL(module,itype,stype) module->ExportFunc("Qi"#itype,&QueryICtrl<itype,stype>)
#endif

void Exp_ICtrls(qjsbind::Module* module) {
	{
		JsClass<ICtrl> jsCls = module->ExportClass<ICtrl>("ICtrl");
		jsCls.Init(JsClass<IObjRef>::class_id());
		jsCls.AddFunc("ToIWindow", &ICtrl::ToIWindow);
	}

	{
		JsClass<IOsrPanel> jsCls = module->ExportClass<IOsrPanel>("IOsrPanel");
		jsCls.Init(JsClass<ICtrl>::class_id());
		jsCls.AddFunc("SetItemIndex", &IOsrPanel::SetItemIndex);
		jsCls.AddFunc("GetItemIndex", &IOsrPanel::GetItemIndex);
		jsCls.AddFunc("SetItemData", &IOsrPanel::SetItemData);
		jsCls.AddFunc("GetItemData", &IOsrPanel::GetItemData);
		jsCls.AddFunc("PtToHost", &IOsrPanel::PtToHost);		
		DEF_QICTRL(module, IOsrPanel, SOsrPanel);
	}
	{
		JsClass<IItemPanel> jsCls = module->ExportClass<IItemPanel>("IItemPanel");
		jsCls.Init(JsClass<IOsrPanel>::class_id());
		jsCls.AddFunc("SetSkin", &IItemPanel::SetSkin);
		jsCls.AddFunc("SetColor", &IItemPanel::SetColor);
		DEF_QICTRL(module, IItemPanel, SItemPanel);
	}

	{
		JsClass<IComboBase> jsCls = module->ExportClass<IComboBase>("IComboBase");
		jsCls.Init(JsClass<ICtrl>::class_id());
		jsCls.AddFunc("FindString", &IComboBase::FindStringA);
		jsCls.AddFunc("DropDown", &IComboBase::DropDown);
		jsCls.AddFunc("CloseUp", &IComboBase::CloseUp);
		jsCls.AddFunc("IsDropdown", &IComboBase::IsDropdown);
		jsCls.AddFunc("SetDropdown", &IComboBase::SetDropdown);
		jsCls.AddFunc("GetItemText", &IComboBase::GetItemTextA);
		DEF_QICTRL(module, IComboBase, SComboBase);
	}
	{
		JsClass<IComboBox> jsCls = module->ExportClass<IComboBox>("IComboBox");
		jsCls.Init(JsClass<IComboBase>::class_id());

		jsCls.AddFunc("SetCurSel", &IComboBox::SetCurSel);
		jsCls.AddFunc("GetCount", &IComboBox::GetCount);
		jsCls.AddFunc("GetCurSel", &IComboBox::GetCurSel);
		jsCls.AddFunc( "InsertItem", &IComboBox::InsertItemA);
		jsCls.AddFunc( "DeleteString", &IComboBox::DeleteString);
		jsCls.AddFunc( "ResetContent", &IComboBox::ResetContent);
		jsCls.AddFunc( "GetListBox", &IComboBox::GetIListBox);
		jsCls.AddFunc( "GetItemData", &IComboBox::GetItemData);
		jsCls.AddFunc( "SetItemData", &IComboBox::SetItemData);
		DEF_QICTRL(module, IComboBox, SComboBox);
	}
	{
		JsClass<IComboView> jsCls = module->ExportClass<IComboView>("IComboView");
		jsCls.Init(JsClass<IComboBase>::class_id());
		jsCls.AddFunc("SetCurSel", &IComboView::SetCurSel);
		jsCls.AddFunc("GetCount", &IComboView::GetCount);
		jsCls.AddFunc("GetCurSel", &IComboView::GetCurSel);
		jsCls.AddFunc("GetListView", &IComboView::GetIListView);
		DEF_QICTRL(module, IComboView, SComboView);
	}

	{
		JsClass<IImageWnd> jsCls = module->ExportClass<IImageWnd>("IImageWnd");
		jsCls.Init(JsClass<ICtrl>::class_id());

		jsCls.AddFunc("SetSkin", &IImageWnd::SetSkin);
		jsCls.AddFunc("GetSkin", &IImageWnd::GetSkin);
		jsCls.AddFunc("SetImage", &IImageWnd::SetImage);
		jsCls.AddFunc("GetImage", &IImageWnd::GetImage);
		jsCls.AddFunc("SetIcon", &IImageWnd::SetIcon);
		DEF_QICTRL(module, IImageWnd, SImageWnd);
	}
	{
		JsClass<IAnimateImgWnd> jsCls = module->ExportClass<IAnimateImgWnd>("IAnimateImgWnd");
		jsCls.Init(JsClass<ICtrl>::class_id());
		jsCls.AddFunc("Start", &IAnimateImgWnd::Start);
		jsCls.AddFunc("Stop", &IAnimateImgWnd::Stop);
		jsCls.AddFunc("IsPlaying", &IAnimateImgWnd::IsPlaying);
		DEF_QICTRL(module, IAnimateImgWnd, SAnimateImgWnd);
	}
	{
		JsClass<IProgress> jsCls = module->ExportClass<IProgress>("IProgress");
		jsCls.Init(JsClass<ICtrl>::class_id());

		jsCls.AddFunc("SetValue", &IProgress::SetValue);
		jsCls.AddFunc("GetValue", &IProgress::GetValue);
		jsCls.AddFunc("SetRange", &IProgress::SetRange);
		jsCls.AddFunc("GetRange", &IProgress::GetRange);
		jsCls.AddFunc("IsVertical", &IProgress::IsVertical);
		DEF_QICTRL(module, IProgress, SProgress);
	}
	{
		JsClass<IPanel> jsCls = module->ExportClass<IPanel>("IPanel");
		jsCls.Init(JsClass<ICtrl>::class_id());
		jsCls.AddFunc("ShowScrollBar", &IPanel::ShowScrollBar);
		jsCls.AddFunc("EnableScrollBar", &IPanel::EnableScrollBar);
		jsCls.AddFunc("IsScrollBarEnable", &IPanel::IsScrollBarEnable);
		jsCls.AddFunc("SetScrollInfo", &IPanel::SetScrollInfo);
		jsCls.AddFunc("SetScrollPos", &IPanel::SetScrollPos);
		jsCls.AddFunc("GetScrollPos", &IPanel::GetScrollPos);
		jsCls.AddFunc("SetScrollRange", &IPanel::SetScrollRange);
		jsCls.AddFunc("GetScrollRange", &IPanel::GetScrollRange);
		jsCls.AddFunc("HasScrollBar", &IPanel::HasScrollBar);
		DEF_QICTRL(module, IPanel, SPanel);
	}
	{
		JsClass<IScrollView> jsCls = module->ExportClass<IScrollView>("IScrollView");
		jsCls.Init(JsClass<IPanel>::class_id());
		jsCls.AddFunc("GetViewSize", &IScrollView::GetViewSize);
		jsCls.AddFunc("SetViewSize", &IScrollView::SetViewSize);
		jsCls.AddFunc("GetViewOrigin", &IScrollView::GetViewOrigin);
		jsCls.AddFunc("SetViewOrigin", &IScrollView::SetViewOrigin);
		DEF_QICTRL(module, IScrollView, SScrollView);
	}
	{
		JsClass<IHeaderCtrl> jsCls = module->ExportClass<IHeaderCtrl>("IHeaderCtrl");
		jsCls.Init(JsClass<ICtrl>::class_id());
		jsCls.AddFunc("InsertItem", &IHeaderCtrl::InsertItemA);
		jsCls.AddFunc("GetItem", &IHeaderCtrl::GetItem);
		jsCls.AddFunc("SetItem", &IHeaderCtrl::SetItem);
		jsCls.AddFunc("GetItemCount", &IHeaderCtrl::GetItemCount);
		jsCls.AddFunc("GetTotalWidth", &IHeaderCtrl::GetTotalWidth);
		jsCls.AddFunc("IsAutoResize", &IHeaderCtrl::IsAutoResize);
		jsCls.AddFunc("GetItemWidth", &IHeaderCtrl::GetItemWidth);
		jsCls.AddFunc("DeleteItem", &IHeaderCtrl::DeleteItem);
		jsCls.AddFunc("DeleteAllItems", &IHeaderCtrl::DeleteAllItems);
		jsCls.AddFunc("SetItemSort", &IHeaderCtrl::SetItemSort);
		jsCls.AddFunc("SetItemVisible", &IHeaderCtrl::SetItemVisible);
		jsCls.AddFunc("IsItemVisible", &IHeaderCtrl::IsItemVisible);
		DEF_QICTRL(module, IHeaderCtrl, SHeaderCtrl);
	}
	{
		JsClass<IListView> jsCls = module->ExportClass<IListView>("IListView");
		jsCls.Init(JsClass<IPanel>::class_id());
		jsCls.AddFunc("SetAdapter", &IListView::SetAdapter);
		jsCls.AddFunc("GetAdapter", &IListView::GetAdapter);
		jsCls.AddFunc("SetItemLocator", &IListView::SetItemLocator);
		jsCls.AddFunc("GetItemLocator", &IListView::GetItemLocator);
		jsCls.AddFunc("EnsureVisible", &IListView::EnsureVisible);
		jsCls.AddFunc("SetSel", &IListView::SetSel);
		jsCls.AddFunc("GetSel", &IListView::GetSel);
		jsCls.AddFunc("HitTest", &IListView::HitTest);
		DEF_QICTRL(module, IListView, SListView);
	}
	{
		JsClass<IMcListView> jsCls = module->ExportClass<IMcListView>("IMcListView");
		jsCls.Init(JsClass<IPanel>::class_id());
		jsCls.AddFunc("SetAdapter", &IMcListView::SetAdapter);
		jsCls.AddFunc("GetAdapter", &IMcListView::GetAdapter);
		jsCls.AddFunc("SetItemLocator", &IMcListView::SetItemLocator);
		jsCls.AddFunc("GetItemLocator", &IMcListView::GetItemLocator);
		jsCls.AddFunc("EnsureVisible", &IMcListView::EnsureVisible);
		jsCls.AddFunc("SetSel", &IMcListView::SetSel);
		jsCls.AddFunc("GetSel", &IMcListView::GetSel);
		jsCls.AddFunc("HitTest", &IMcListView::HitTest);
		jsCls.AddFunc("GetIHeaderCtrl", &IMcListView::GetIHeaderCtrl);
		jsCls.AddFunc("InsertColumn", &IMcListView::InsertColumn);
		jsCls.AddFunc("DeleteColumn", &IMcListView::DeleteColumn);
		jsCls.AddFunc("GetColumnCount", &IMcListView::GetColumnCount);
		DEF_QICTRL(module, IMcListView, SMCListView);
	}
	{
		JsClass<ITreeView> jsCls = module->ExportClass<ITreeView>("ITreeView");
		jsCls.Init(JsClass<IPanel>::class_id());
		jsCls.AddFunc("SetAdapter", &ITreeView::SetAdapter);
		jsCls.AddFunc("GetAdapter", &ITreeView::GetAdapter);
		jsCls.AddFunc("SetItemLocator", &ITreeView::SetItemLocator);
		jsCls.AddFunc("GetItemLocator", &ITreeView::GetItemLocator);
		jsCls.AddFunc("EnsureVisible", &ITreeView::EnsureVisible);
		jsCls.AddFunc("SetSel", &ITreeView::SetSel);
		jsCls.AddFunc("GetSel", &ITreeView::GetSel);
		jsCls.AddFunc("HitTest", &ITreeView::HitTest);
		DEF_QICTRL(module, ITreeView, STreeView);
	}
	{
		JsClass<ITileView> jsCls = module->ExportClass<ITileView>("ITileView");
		jsCls.Init(JsClass<IPanel>::class_id());
		jsCls.AddFunc("SetAdapter", &ITileView::SetAdapter);
		jsCls.AddFunc("GetAdapter", &ITileView::GetAdapter);
		jsCls.AddFunc("SetItemLocator", &ITileView::SetItemLocator);
		jsCls.AddFunc("GetItemLocator", &ITileView::GetItemLocator);
		jsCls.AddFunc("EnsureVisible", &ITileView::EnsureVisible);
		jsCls.AddFunc("SetSel", &ITileView::SetSel);
		jsCls.AddFunc("GetSel", &ITileView::GetSel);
		jsCls.AddFunc("HitTest", &ITileView::HitTest);
		DEF_QICTRL(module, ITileView, STileView);
	}
	{
		JsClass<IListBox> jsCls = module->ExportClass<IListBox>("IListBox");
		jsCls.Init(JsClass<IPanel>::class_id());
		jsCls.AddFunc("GetCount", &IListBox::GetCount);
		jsCls.AddFunc("GetCurSel", &IListBox::GetCurSel);
		jsCls.AddFunc("SetCurSel", &IListBox::SetCurSel);
		jsCls.AddFunc("GetTopIndex", &IListBox::GetTopIndex);
		jsCls.AddFunc("SetTopIndex", &IListBox::SetTopIndex);
		jsCls.AddFunc("GetItemHeight", &IListBox::GetItemHeight);
		jsCls.AddFunc("SetItemHeight", &IListBox::SetItemHeight);
		jsCls.AddFunc("GetItemData", &IListBox::GetItemData);
		jsCls.AddFunc("SetItemData", &IListBox::SetItemData);
		jsCls.AddFunc("GetIText", &IListBox::GetIText);
		jsCls.AddFunc("DeleteAll", &IListBox::DeleteAll);
		jsCls.AddFunc("DeleteString", &IListBox::DeleteString);
		jsCls.AddFunc("AddString", &IListBox::AddString);
		jsCls.AddFunc("InsertString", &IListBox::InsertString);
		jsCls.AddFunc("EnsureVisible", &IListBox::EnsureVisible);
		jsCls.AddFunc("FindString", &IListBox::FindString);
		DEF_QICTRL(module, IListBox, SListBox);
	}
	{
		JsClass<IDateTimePicker> jsCls = module->ExportClass<IDateTimePicker>("IDateTimePicker");
		jsCls.Init(JsClass<ICtrl>::class_id());
		jsCls.AddFunc("SetTime", &IDateTimePicker::SetTime);
		jsCls.AddFunc("GetTime", &IDateTimePicker::GetTime);
		jsCls.AddFunc("CloseUp", &IDateTimePicker::CloseUp);
		jsCls.AddFunc("DropDown", &IDateTimePicker::DropDown);
		jsCls.AddFunc("Clear", &IDateTimePicker::Clear);
		DEF_QICTRL(module, IDateTimePicker, SDateTimePicker);
	}
	{
		JsClass<ITreeCtrl> jsCls = module->ExportClass<ITreeCtrl>("ITreeCtrl");
		jsCls.Init(JsClass<IPanel>::class_id());
		jsCls.AddFunc("InsertItem", &ITreeCtrl::InsertItemA);
		jsCls.AddFunc("RemoveItem", &ITreeCtrl::RemoveItem);
		jsCls.AddFunc("RemoveAllItems", &ITreeCtrl::RemoveAllItems);
		jsCls.AddFunc("GetRootItem", &ITreeCtrl::GetRootItem);
		jsCls.AddFunc("GetNextSiblingItem", &ITreeCtrl::GetNextSiblingItem);
		jsCls.AddFunc("GetPrevSiblingItem", &ITreeCtrl::GetPrevSiblingItem);
		jsCls.AddFunc("GetChildItem", &ITreeCtrl::GetChildItem);
		jsCls.AddFunc("GetParentItem", &ITreeCtrl::GetParentItem);
		jsCls.AddFunc("GetSelectedItem", &ITreeCtrl::GetSelectedItem);
		jsCls.AddFunc("GetNextItem", &ITreeCtrl::GetNextItem);
		jsCls.AddFunc("SortChildren", &ITreeCtrl::SortChildren);
		jsCls.AddFunc("SelectItem", &ITreeCtrl::SelectItem);
		jsCls.AddFunc("GetItemText", &ITreeCtrl::GetItemTextA);
		jsCls.AddFunc("SetItemText", &ITreeCtrl::SetItemTextA);
		jsCls.AddFunc("GetItemImage", &ITreeCtrl::GetItemImage);
		jsCls.AddFunc("SetItemImage", &ITreeCtrl::SetItemImage);
		jsCls.AddFunc("GetItemData", &ITreeCtrl::GetItemData);
		jsCls.AddFunc("SetItemData", &ITreeCtrl::SetItemData);
		jsCls.AddFunc("ItemHasChildren", &ITreeCtrl::ItemHasChildren);
		jsCls.AddFunc("GetCheckState", &ITreeCtrl::GetCheckState);
		jsCls.AddFunc("SetCheckState", &ITreeCtrl::SetCheckState);
		jsCls.AddFunc("Expand", &ITreeCtrl::Expand);
		jsCls.AddFunc("EnsureVisible", &ITreeCtrl::EnsureVisible);
		DEF_QICTRL(module, ITreeCtrl, STreeCtrl);
	}
	{
		JsClass<IHotKeyCtrl> jsCls = module->ExportClass<IHotKeyCtrl>("IHotKeyCtrl");
		jsCls.Init(JsClass<ICtrl>::class_id());
		jsCls.AddFunc("SetRule", &IHotKeyCtrl::SetRule);
		jsCls.AddFunc("SetHotKey", &IHotKeyCtrl::SetHotKey);
		jsCls.AddFunc("GetHotKey", &IHotKeyCtrl::GetHotKey);
		DEF_QICTRL(module, IHotKeyCtrl, SHotKeyCtrl);
	}
	{
		JsClass<IRichEdit> jsCls = module->ExportClass<IRichEdit>("IRichEdit");
		jsCls.Init(JsClass<IPanel>::class_id());

		jsCls.AddFunc("SaveRtf", &IRichEdit::SaveRtf);
		jsCls.AddFunc("LoadRtf", &IRichEdit::LoadRtf);
		jsCls.AddFunc("SetSel", &IRichEdit::SetSel);
		jsCls.AddFunc("ReplaceSel", &IRichEdit::ReplaceSel);
		jsCls.AddFunc("GetWordWrap", &IRichEdit::GetWordWrap);
		jsCls.AddFunc("SetWordWrap", &IRichEdit::SetWordWrap);
		jsCls.AddFunc("GetReadOnly", &IRichEdit::GetReadOnly);
		jsCls.AddFunc("SetReadOnly", &IRichEdit::SetReadOnly);
		jsCls.AddFunc("GetLimitText", &IRichEdit::GetLimitText);
		jsCls.AddFunc("SetLimitText", &IRichEdit::SetLimitText);
		jsCls.AddFunc("GetDefaultAlign", &IRichEdit::GetDefaultAlign);
		jsCls.AddFunc("SetDefaultAlign", &IRichEdit::SetDefaultAlign);
		jsCls.AddFunc("GetRichTextFlag", &IRichEdit::GetRichTextFlag);
		jsCls.AddFunc("SetRichTextFlag", &IRichEdit::SetRichTextFlag);
		jsCls.AddFunc("GetDefaultLeftIndent", &IRichEdit::GetDefaultLeftIndent);
		jsCls.AddFunc("SetDefaultLeftIndent", &IRichEdit::SetDefaultLeftIndent);
		jsCls.AddFunc("SetSaveSelection", &IRichEdit::SetSaveSelection);
		jsCls.AddFunc("SetDefaultTextColor", &IRichEdit::SetDefaultTextColor);
		DEF_QICTRL(module, IRichEdit, SRichEdit);
	}
	{
		JsClass<IEdit> jsCls = module->ExportClass<IEdit>("IEdit");
		jsCls.Init(JsClass<IRichEdit>::class_id());

		jsCls.AddFunc("GetCueText", &IEdit::GetCueText);
		jsCls.AddFunc("GetCueColor", &IEdit::GetCueColor);
		DEF_QICTRL(module, IEdit, SEdit);
	}
	{
		JsClass<ITabPage> jsCls = module->ExportClass<ITabPage>("ITabPage");
		jsCls.Init(JsClass<ICtrl>::class_id());
		jsCls.AddFunc("GetTitle", &ITabPage::GetTitle);
		jsCls.AddFunc("SetTitle", &ITabPage::SetTitle);
		jsCls.AddFunc("GetIconIndex", &ITabPage::GetIconIndex);
		jsCls.AddFunc("SetIconIndex", &ITabPage::SetIconIndex);
		DEF_QICTRL(module, ITabPage, STabPage);
	}
	{
		JsClass<ITabCtrl> jsCls = module->ExportClass<ITabCtrl>("ITabCtrl");
		jsCls.Init(JsClass<ICtrl>::class_id());

		jsCls.AddFunc("GetCurSel", &ITabCtrl::GetCurSel);
		jsCls.AddFunc("SetCurSel", &ITabCtrl::SetCurSel);
		jsCls.AddFunc("SetItemTitle", &ITabCtrl::SetItemTitle);
		jsCls.AddFunc("InsertItem", &ITabCtrl::InsertItem);
		jsCls.AddFunc("GetItemCount", &ITabCtrl::GetItemCount);
		jsCls.AddFunc("GetPage", &ITabCtrl::GetPage);
		jsCls.AddFunc("RemoveItem", &ITabCtrl::RemoveItem);
		jsCls.AddFunc("RemoveAllItems", &ITabCtrl::RemoveAllItems);
		jsCls.AddFunc("GetPageIndex", &ITabCtrl::GetPageIndex);
		DEF_QICTRL(module, ITabCtrl, STabCtrl);
	}
	{
		JsClass<ISpinButtonCtrl> jsCls = module->ExportClass<ISpinButtonCtrl>("ISpinButtonCtrl");
		jsCls.Init(JsClass<ICtrl>::class_id());

		jsCls.AddFunc("SetValue", &ISpinButtonCtrl::SetValue);
		jsCls.AddFunc("SetRange", &ISpinButtonCtrl::SetRange);
		jsCls.AddFunc("SetStep", &ISpinButtonCtrl::SetStep);
		jsCls.AddFunc("GetValue", &ISpinButtonCtrl::GetValue);
		jsCls.AddFunc("GetIBuddy", &ISpinButtonCtrl::GetIBuddy);
		DEF_QICTRL(module, ISpinButtonCtrl, SSpinButtonCtrl);
	}
	{
		JsClass<IIconWnd> jsCls = module->ExportClass<IIconWnd>("IIconWnd");
		jsCls.Init(JsClass<ICtrl>::class_id());
		jsCls.AddFunc( "SetIcon", &IIconWnd::SetIcon);
		DEF_QICTRL(module, IIconWnd, SIconWnd);
	}
}