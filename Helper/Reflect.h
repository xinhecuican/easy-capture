#ifndef REFLECT_H
#define REFLECT_H
#include <QObject>
#include<QHash>
#include "debug.h"
class Reflect
{
public:
    ///使用方法：
    /// 继承QWidget为基础的类并且添加Q_OBJECT
    /// 构造函数前添加Q_INVOKABLE
    /// 调用registerClass
    template<typename T>
    static void registerClass()
    {
        metaObjects().insert( T::staticMetaObject.className(), T::staticMetaObject );
    }

    static QObject* newInstance( const QString& className,
                                 QGenericArgument val0 = QGenericArgument(nullptr),
                                 QGenericArgument val1 = QGenericArgument(),
                                 QGenericArgument val2 = QGenericArgument(),
                                 QGenericArgument val3 = QGenericArgument(),
                                 QGenericArgument val4 = QGenericArgument(),
                                 QGenericArgument val5 = QGenericArgument(),
                                 QGenericArgument val6 = QGenericArgument(),
                                 QGenericArgument val7 = QGenericArgument(),
                                 QGenericArgument val8 = QGenericArgument(),
                                 QGenericArgument val9 = QGenericArgument() )
    {
        Q_ASSERT( metaObjects().contains(className) );
        if(!metaObjects().contains(className))
        {
           qWarning() << "未将" + className + "添加入窗口管理中\n位置： Reflect::newInstance<T>()";
        }
        return metaObjects().value(className).newInstance(val0, val1, val2, val3, val4,
                                                          val5, val6, val7, val8, val9);
    }

    static QGenericReturnArgument invokeMethod(const QString& className, QObject* object, const char* method_name,
                                               QGenericReturnArgument ret = QGenericReturnArgument(),
                             QGenericArgument val0 = QGenericArgument(nullptr),
                             QGenericArgument val1 = QGenericArgument(),
                             QGenericArgument val2 = QGenericArgument(),
                             QGenericArgument val3 = QGenericArgument(),
                             QGenericArgument val4 = QGenericArgument(),
                             QGenericArgument val5 = QGenericArgument(),
                             QGenericArgument val6 = QGenericArgument(),
                             QGenericArgument val7 = QGenericArgument(),
                             QGenericArgument val8 = QGenericArgument(),
                             QGenericArgument val9 = QGenericArgument()
                             )
    {
        if(!metaObjects().contains(className))
        {
            qWarning() << className;
            qWarning(method_name);
        }
        metaObjects().value(className).invokeMethod(object, method_name, Qt::AutoConnection, ret, val0,
                                                           val1, val2, val3, val4, val5, val6, val7, val8, val9);
        return ret;
    }

private:
    static QHash<QString, QMetaObject>& metaObjects()
    {
        static QHash<QString, QMetaObject> instance;
        return instance;
    }
};
#endif // REFLECT_H
