#ifndef WINDOW_FLITER_H
#define WINDOW_FLITER_H
#define WIN32_LEAN_AND_MEAN
#ifdef Q_OS_WIN
#include <windows.h>
#include<QWidget>
#include<QDebug>
class CSCWinInfo
{
public:
    HWND m_hWnd;
    QRect m_rtWin;    //window rect

    INT m_nLevel;    // 1 - pop up window  ;  2N - child window
};

class Window_fliter
{
public:
    static Window_fliter* instance()
    {
        if(_instance == NULL)
        {
            _instance = new Window_fliter();
        }
        return _instance;
    }
    static BOOL IsFilterWindow(HWND hWnd)
       {
           DWORD dwProcessID = GetCurrentProcessId();
           if(hWnd != NULL && IsWindow(hWnd))
           {
               DWORD dwWinProcessId(0);
               GetWindowThreadProcessId(hWnd, &dwWinProcessId);
               if(dwProcessID == dwWinProcessId)
               {
                   return TRUE;
               }
           }

           return FALSE;
       }

       static DWORD GetIncludeStyle()
       {
           return WS_VISIBLE;
       }

       static DWORD GetExcludeStyleEx()
       {
           return  WS_EX_TRANSPARENT;
       }

       static BOOL IsTargetPopupWindow()
       {
           return FALSE;
       }
       BOOL SnapshotAllWinRect()
       {
           ClearData();
           // cache current window Z order when call this function
           EnumWindows(EnumWindowsSnapshotProc, 1);

           return TRUE;
       }

       //get from current Z order of desktop
       HWND GetHWNDByPoint(QPoint pt)
       {
           m_hWndTarget = NULL;

           EnumWindows(EnumWindowsRealTimeProc, MAKELPARAM(pt.x(), pt.y()));

           return m_hWndTarget;
       }

       QRect GetWinRectByPoint(QPoint ptHit, BOOL bGetInRealTime = FALSE)
       {
           QRect rtRect(0, 0, 0, 0);
           if(bGetInRealTime) //get from current Z order
           {
               HWND hWndTarget = GetHWNDByPoint(ptHit);
               if(hWndTarget != NULL )
               {
                   RECT rect;
                   GetWindowRect(hWndTarget, &rect);
                   rtRect.setTopLeft(QPoint(rect.left, rect.top));
                   rtRect.setBottomRight(QPoint(rect.right, rect.bottom));
               }
           }
           else //get from snapshot cache
           {
               GetRectByPointFromSnapshot(ptHit, rtRect);
           }

           return rtRect;
       }

       static bool enableStyle(DWORD style, DWORD exstyle)
       {
           return (style & WS_VSCROLL) || (exstyle & WS_EX_RIGHTSCROLLBAR)
                   || (exstyle & WS_EX_LEFTSCROLLBAR);
       }

       static bool disableStyle(DWORD style, DWORD exstyle)
       {
           return (style & WS_DISABLED) || (exstyle & WS_EX_TRANSPARENT)
                   || (exstyle & WS_EX_NOACTIVATE);
       }

       static HWND find_scroll_window(HWND hwnd, QPoint point)
       {
           scroll_hwnd = NULL;
           DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);
           DWORD ex_style = GetWindowLong(hwnd, GWL_EXSTYLE);
           qDebug() << hwnd << dwStyle << ex_style;
           if(enableStyle(dwStyle, ex_style) && !disableStyle(dwStyle, ex_style))
           {
               return hwnd;
           }
           EnumChildWindows(hwnd, EnumScrollWindowProc, MAKELPARAM(point.x(), point.y()));
           return scroll_hwnd;
       }

   protected:
       static HWND scroll_hwnd;
       static BOOL CALLBACK EnumScrollWindowProc(HWND hwnd, LPARAM lParam)
       {
           DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);
           DWORD ex_style = GetWindowLong(hwnd, GWL_EXSTYLE);
           if(!(enableStyle(dwStyle, ex_style) && !disableStyle(dwStyle, ex_style)))
           {
               EnumChildWindows(hwnd, EnumScrollWindowProc, lParam);
           }
           else
           {
               POINT point;
               point.x = lParam & 0xffff;
               point.y = lParam >> 16;
               RECT rect;
               GetClientRect(hwnd, &rect);
               if(PtInRect(&rect, point))
               {
                   scroll_hwnd = hwnd;
               }
           }
           return TRUE;
       }

       static BOOL CALLBACK EnumWindowsRealTimeProc(HWND hwnd, LPARAM lParam)
       {
           if(!PtInWinRect(hwnd, QPoint((lParam & 0xffff), (lParam >> 16)))) return TRUE;

           if(ShouldWinBeFiltered(hwnd))  return TRUE;

           m_hWndTarget = hwnd;

           if(IsTargetPopupWindow()) return FALSE; //this is the target window, exit search

           EnumChildWindows(hwnd, EnumChildRealTimeProc, lParam);

           return FALSE;
       }

       static BOOL CALLBACK EnumChildRealTimeProc(HWND hwnd, LPARAM lParam)
       {
           if(!PtInWinRect(hwnd, QPoint((lParam & 0xffff), (lParam >> 16)))) return TRUE;

           if(ShouldWinBeFiltered(hwnd)) return TRUE;

           m_hWndTarget = hwnd;
           EnumChildWindows(hwnd, EnumChildRealTimeProc, lParam);

           return FALSE;
       }

   protected:
       static BOOL CALLBACK EnumWindowsSnapshotProc(HWND hwnd, LPARAM lParam)
       {
           INT nLevel = lParam;
           if(ShouldWinBeFiltered(hwnd))  return TRUE;

           SaveSnapshotWindow(hwnd, nLevel);

           if(!IsTargetPopupWindow())
           {
               ++nLevel;
               EnumChildWindows(hwnd, EnumChildSnapshotProc, nLevel);
           }

           return TRUE;
       }

       static BOOL CALLBACK EnumChildSnapshotProc(HWND hwnd, LPARAM lParam)
       {
           INT nLevel = lParam;
           if(ShouldWinBeFiltered(hwnd)) return TRUE;
           SaveSnapshotWindow(hwnd, nLevel);
           ++nLevel;
           EnumChildWindows(hwnd, EnumChildSnapshotProc, nLevel);

           return TRUE;
       }

   protected:
       static BOOL PtInWinRect(HWND hWnd, QPoint pt)
       {
           RECT rect1;
           GetWindowRect(hWnd, &rect1);
           POINT point;
           point.x = pt.x();
           point.y = pt.y();
           return PtInRect(&rect1, point);
       }

       static BOOL ShouldWinBeFiltered(HWND hWnd)
       {
           if(IsFilterWindow(hWnd)) return TRUE;

           DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);
           DWORD dwStyleMust = GetIncludeStyle();
           if((dwStyle & dwStyleMust) != dwStyleMust) return TRUE;

           DWORD dwStyleEx = GetWindowLong(hWnd, GWL_EXSTYLE);
           DWORD dwStyleMustNot = GetExcludeStyleEx();
           if((dwStyleMustNot & dwStyleEx) != 0) return TRUE;

           return FALSE;
       }

       //find the first window that level is biggest
       static BOOL  GetRectByPointFromSnapshot(QPoint ptHit, QRect& rtRet)
       {
           int nCount = m_arSnapshot.size();
           CSCWinInfo* pInfo = NULL;
           CSCWinInfo* pTarget = NULL;
           for(int i=0; i<nCount; ++i)
           {
               pInfo = m_arSnapshot[i];
               Q_ASSERT(pInfo != NULL);

               //target window is found
               //and level is not increasing,
               //that is checking its sibling or parent window, exit search
               if(pTarget != NULL
                   && pInfo->m_nLevel <= pTarget->m_nLevel)
               {
                   break;
               }
               POINT point;
               point.x = ptHit.x();
               point.y = ptHit.y();
               RECT rect;
               rect.top = pInfo->m_rtWin.top();rect.left = pInfo->m_rtWin.left();
               rect.right = pInfo->m_rtWin.right(); rect.bottom = pInfo->m_rtWin.bottom();
               if(PtInRect(&rect, point))
               {
                   if(pTarget == NULL)
                   {
                       pTarget = pInfo;
                   }
                   else
                   {
                       if( pInfo->m_nLevel > pTarget->m_nLevel)
                       {
                           pTarget = pInfo;
                       }
                   }
               }
           }

           if(pTarget != NULL)
           {
               rtRet = pTarget->m_rtWin;
               return TRUE;
           }

           return FALSE;
       }

       static VOID SaveSnapshotWindow(HWND hwnd, INT nLevel)
       {
           RECT rect2;
           POINT point2;
           point2.x = 0;point2.y = 0;
           GetClientRect(hwnd, &rect2);
           ClientToScreen(hwnd, &point2);
           QRect rtWin = QRect(QPoint(point2.x, point2.y), QSize(rect2.right, rect2.bottom));
           if(rtWin.isEmpty()) return;

           CSCWinInfo* pInfo = new CSCWinInfo();
           if(pInfo == NULL) return;

           pInfo->m_hWnd = hwnd;
           pInfo->m_nLevel = nLevel;
           pInfo->m_rtWin = rtWin;
           m_arSnapshot.push_back(pInfo);
       }

       static VOID ClearData()
       {
           int nCount = m_arSnapshot.size();
           for(int i=0; i<nCount; ++i)
           {
               delete m_arSnapshot[i];
           }

           m_arSnapshot.clear();
       }



   protected:
       static Window_fliter* _instance;
       static HWND m_hWndTarget;
       static QVector<CSCWinInfo*> m_arSnapshot;
};

#endif

#endif // WINDOW_FLITER_H
