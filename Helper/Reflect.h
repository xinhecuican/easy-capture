#ifndef REFLECT_H
#define REFLECT_H
#include <QObject>
#include<QHash>
#include "debug.h"
class Reflect
{
public:
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
            Debug::show_error_message("未将" + className + "添加入窗口管理中\n位置： Reflect::newInstance<T>()");
        }
        return metaObjects().value(className).newInstance(val0, val1, val2, val3, val4,
                                                          val5, val6, val7, val8, val9);
    }

private:
    static QHash<QString, QMetaObject>& metaObjects()
    {
        static QHash<QString, QMetaObject> instance;
        return instance;
    }
};
#endif // REFLECT_H
