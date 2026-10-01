#ifndef __MYLIB_TYPES_H
#define __MYLIB_TYPES_H
/* List element. */
struct list_elem
{
  struct list_elem *prev; /* Previous list element. */
  struct list_elem *next; /* Next list element. */
};

/* Hash element. */
struct hash_elem
{
  struct list_elem list_elem;
  int data;
};

//의존성 문제 때문에 두 구조체를 따로 분리해서 헤더파일로 만들었습니다.

#endif //__MYLIB_TYPES_H