#ifndef MYOS_STDARG_H
#define MYOS_STDARG_H

typedef char* va_list_;
// _INTSIZEOF 根据变量类型，取得在栈中的长度
#define _INTSIZEOF(n) ((sizeof(n) + sizeof(int)-1)&~(sizeof(int)-1))

// va_start 根据传入参数的第一个参数，获取可变参数的第一个参数的地址
#define va_start(ap,v) (ap=(va_list_)&v+_INTSIZEOF(v))

// va_arg 取出参数，并使指针跳到下一个参数的地址
#define va_arg(ap,t) (*(t*)((ap+=_INTSIZEOF(t))-_INTSIZEOF(t)))

// va_end 设指针为空
#define va_end(ap) (ap=(va_list_)0)

#endif // !MYOS_STDARG_H
