#include "stdafx.h"
#include "SChatEdit.h"

#ifndef LY_PER_INCH
#define LY_PER_INCH 1440
#endif

namespace SOUI
{

const SStringW KLabelColor = L"color";
const SStringW KLabelLink = L"link";
const SStringW KLabelFont = L"font";
const SStringW KLabelUnderline = L"underline";
const SStringW KLabelItalic = L"italic";
const SStringW KLabelBold = L"bold";
const SStringW KLabelStrike = L"strike";
const SStringW KLabelSize = L"size";
const SStringW KLabelSmiley = L"smiley";

SChatEdit::SChatEdit(void)
{
    GetEventSet()->addEvent(EVENTID(EventChatEditKeyReturn));
}

SChatEdit::~SChatEdit(void)
{
}

BOOL SChatEdit::AppendFormatText(const SStringW &strMsg, BOOL bNewLine, BOOL bCanUndo)
{
    BOOL bRet = FALSE;
    SStringW strBuf = L"<msg>" + strMsg + L"</msg>";
    LPWSTR pszBuf = strBuf.GetBuffer(strBuf.GetLength());
    {
        SXmlDoc doc;
        if (doc.load_buffer_inplace(pszBuf, strBuf.GetLength() * 2, xml_parse_default, enc_utf16))
        {
            bRet = AppendFormatText(doc.root().child(L"msg"), bNewLine, bCanUndo);
        }
    }
    strBuf.ReleaseBuffer();
    return bRet;
}

BOOL SChatEdit::AppendFormatText(const SXmlNode xmlMsg, BOOL bNewLine, BOOL bCanUndo)
{
    TCHAR szRet[] = { 0x0a, 0 };
    int nLen = (int)SSendMessage(WM_GETTEXTLENGTH);
    if (bNewLine)
    { //插入一个换行符
        SSendMessage(EM_SETSEL, nLen, nLen);
        SSendMessage(EM_REPLACESEL, bCanUndo, (LPARAM)L"\r\n");
        nLen = (int)SSendMessage(WM_GETTEXTLENGTH);
    }
    SSendMessage(EM_SETSEL, nLen, nLen);
    long iCaret = nLen;

    CHARFORMATW cf = { 0 };
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_ALL;
    SSendMessage(EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
    cf.dwEffects &= ~CFE_AUTOCOLOR;
    _InsertFormatText(iCaret, cf, xmlMsg, bCanUndo);

    SSendMessage(WM_VSCROLL, MAKEWPARAM(SB_BOTTOM, 0));
    return TRUE;
}

BOOL SChatEdit::ReplaceSelectionByFormatText(const SStringW &strMsg, BOOL bCanUndo /*=TRUE*/)
{
    BOOL bRet = FALSE;
    SStringW strBuf = L"<msg>" + strMsg + L"</msg>";
    LPWSTR pszBuf = strBuf.GetBuffer(strBuf.GetLength());
    {
        SXmlDoc doc;
        if (doc.load_buffer_inplace(pszBuf, strBuf.GetLength() * 2, xml_parse_default, enc_utf16))
        {
            SSendMessage(EM_REPLACESEL, bCanUndo, (LPARAM)L"");

            long iCaret = 0;
            SSendMessage(EM_GETSEL, (WPARAM)&iCaret);

            CHARFORMATW cf = { 0 };
            cf.cbSize = sizeof(cf);
            cf.dwMask = CFM_ALL;
            SSendMessage(EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
            cf.dwEffects &= ~CFE_AUTOCOLOR;
            _InsertFormatText(iCaret, cf, doc.root().child(L"msg"), bCanUndo);

            bRet = TRUE;
        }
    }
    strBuf.ReleaseBuffer();

    return TRUE;
}

int SChatEdit::_InsertFormatText(int iCaret, CHARFORMATW cf, SXmlNode xmlText, BOOL bCanUndo)
{
    SStringW strText = xmlText.value();
    if (xmlText.name() == KLabelSmiley)
    { // insert smiley
        return 0;
    }

    CHARFORMATW cfNew = cf;
    cfNew.dwMask = 0;
    if (xmlText.name() == KLabelColor)
    {
        cfNew.crTextColor = GETCOLOR(xmlText.attribute(L"value").value());
        if (cfNew.crTextColor != CR_INVALID)
        {
            cfNew.crTextColor &= 0x00ffffff;
            cfNew.dwMask |= CFM_COLOR;
        }
    }
    else if (xmlText.name() == KLabelFont)
    {
        wcscpy(cf.szFaceName, cfNew.szFaceName);
        wcscpy_s(cfNew.szFaceName, LF_FACESIZE - 1, xmlText.attribute(L"value").value());
        cfNew.dwMask |= CFM_FACE;
    }
    else if (xmlText.name() == KLabelUnderline)
    {
        cfNew.dwMask |= CFM_UNDERLINE;
        cfNew.dwEffects |= CFE_UNDERLINE;
    }
    else if (xmlText.name() == KLabelItalic)
    {
        cfNew.dwMask |= CFM_ITALIC;
        cfNew.dwEffects |= CFE_ITALIC;
    }
    else if (xmlText.name() == KLabelBold)
    {
        cfNew.dwMask |= CFM_BOLD;
        cfNew.dwEffects |= CFE_BOLD;
    }
    else if (xmlText.name() == KLabelStrike)
    {
        cfNew.dwMask |= CFM_STRIKEOUT;
        cfNew.dwEffects |= CFE_STRIKEOUT;
    }
    else if (xmlText.name() == KLabelLink)
    {
        cfNew.dwMask |= CFM_LINK;
        cfNew.dwEffects |= CFE_LINK;
        COLORREF cr = GETCOLOR(xmlText.attribute(L"color").value());
        if (cr != CR_INVALID)
        {
            cfNew.dwMask |= CFM_COLOR;
            cfNew.crTextColor = cr & 0x00ffffff;
        }
    }
    else if (xmlText.name() == KLabelSize)
    {
        cfNew.dwMask |= CFM_SIZE;

        HDC hdc = GetDC(NULL);
        LONG yPixPerInch = GetDeviceCaps(hdc, LOGPIXELSY);
        ReleaseDC(NULL, hdc);
        cfNew.yHeight
            = abs(MulDiv(xmlText.attribute(L"value").as_uint(12), LY_PER_INCH, yPixPerInch));
    }

    int nRet = strText.GetLength();

    SSendMessage(EM_REPLACESEL, bCanUndo, (LPARAM)(LPCWSTR)strText);
    int iEnd = iCaret + nRet;
    SSendMessage(EM_SETSEL, iCaret, iEnd);
    SSendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cfNew);
    iCaret = iEnd;
    SSendMessage(EM_SETSEL, iCaret, iCaret);

    SXmlNode xmlChild = xmlText.first_child();
    while (xmlChild)
    {
        int nSubLen = _InsertFormatText(iCaret, cfNew, xmlChild, bCanUndo);
        iCaret += nSubLen;
        nRet += nSubLen;

        xmlChild = xmlChild.next_sibling();
    }
    if (cfNew.dwMask)
    {
        cf.dwMask = CFM_ALL;
        SSendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
    }
    return nRet;
}

int SChatEdit::OnCreate(LPVOID)
{
    int nRet = SRichEdit::OnCreate(NULL);
    if (nRet == 0)
    {
        DWORD dwEvtMask = (DWORD)SSendMessage(EM_GETEVENTMASK);
        SSendMessage(EM_SETEVENTMASK, 0, dwEvtMask | ENM_LINK);
    }
    return nRet;
}

void SChatEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (nChar == VK_RETURN)
    {
        EventChatEditKeyReturn evt(this);
        if (evt.bCancel)
            return;
    }
    return SRichEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

SStringW SChatEdit::GetFormatText()
{
    SStringW strTxt;

    TEXTRANGE txtRng;
    txtRng.chrg.cpMin = 0;
    txtRng.chrg.cpMax = (LONG)SSendMessage(WM_GETTEXTLENGTH);
    txtRng.lpstrText = (LPTSTR)strTxt.GetBufferSetLength(txtRng.chrg.cpMax);

    SSendMessage(EM_GETTEXTRANGE, 0, (LPARAM)&txtRng);
    strTxt.ReleaseBuffer();

    SStringW strMsg;
    int iPlainTxtBegin = 0;
    for (int i = 0; i < strTxt.GetLength(); i++)
    {
        if (strTxt[i] == 0xfffc)
        { //找到一个OLE对象
        }
    }
    if (iPlainTxtBegin < strTxt.GetLength())
    {
        strMsg += strTxt.Right(strTxt.GetLength() - iPlainTxtBegin);
    }
    return strMsg;
}

} // namespace SOUI
