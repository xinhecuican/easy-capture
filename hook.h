#ifndef XGLOBALHOOK_H
#define XGLOBALHOOK_H
#include <QObject>
#include <QtGlobal>
#include <QMutex>
#include <QSharedPointer>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "Hook_global.h"

#define xHook XGlobalHook::instance().data()
class HOOK_EXPORT XGlobalHook : public QObject
{
    Q_OBJECT

   /// 单例
private:
   XGlobalHook(QObject* parent = 0) : QObject(parent) {}
   XGlobalHook(const XGlobalHook& hook) = delete;
   XGlobalHook& operator==(const XGlobalHook&) = delete;

public:
   enum button_type{LBUTTON, RBUTTON, MBUTTON, MOUSE_MOVE, LBUTTON_UP, RBUTTON_UP, MOUSE_WHEEL};
   virtual ~XGlobalHook() final;
   static QSharedPointer<XGlobalHook>& instance();

private:
   static QMutex mutex;
   static QSharedPointer<XGlobalHook> hook_instance;

   /// 用户接口
public:
   // 鼠标钩子
   bool installMouseHook();          // 安装钩子
   bool uninstallMouseHook();        // 卸载钩子
   bool isMouseHookRunning() const;  // 运行状态
   // 键盘钩子
   bool installKeyHook();            // 安装钩子
   bool uninstallKeyHook();          // 卸载钩子
   bool isKeyHookRunning() const;    // 运行状态

signals:
   void mouseEvent(button_type type, PMOUSEHOOKSTRUCT pMouseHookStruct, bool* is_shield);
   void keyEvent(PKBDLLHOOKSTRUCT pKeyHookStruct);

      /// DLL 钩子接口
#if defined HOOK_LIBRARY
public:
   void onMouseEvent(int type, PMOUSEHOOKSTRUCT pMouseHookStruct, bool* is_shield);
   void onKeyEvent(PKBDLLHOOKSTRUCT pKeyHookStruct);
#endif
};

#endif // XGLOBALHOOK_H
