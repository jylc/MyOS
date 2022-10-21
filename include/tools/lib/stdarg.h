#ifndef MYOS_STDARG_H
#define MYOS_STDARG_H

typedef char* va_list_;
// _INTSIZEOF ���ݱ������ͣ�ȡ����ջ�еĳ���
#define _INTSIZEOF(n) ((sizeof(n) + sizeof(int)-1)&~(sizeof(int)-1))

// va_start ���ݴ�������ĵ�һ����������ȡ�ɱ�����ĵ�һ�������ĵ�ַ
#define va_start(ap,v) (ap=(va_list_)&v+_INTSIZEOF(v))

// va_arg ȡ����������ʹָ��������һ�������ĵ�ַ
#define va_arg(ap,t) (*(t*)((ap+=_INTSIZEOF(t))-_INTSIZEOF(t)))

// va_end ��ָ��Ϊ��
#define va_end(ap) (ap=(va_list_)0)

#endif // !MYOS_STDARG_H
