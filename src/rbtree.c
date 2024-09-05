#include "rbtree.h"
#include <stdlib.h>

/**
 * new_rbtree
 * : rbtree 생성
 */
rbtree *new_rbtree(void) {
  // rbtree size: 16byte
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  // 메모리 할당 실패 시 return 처리
  if (p == NULL) {
    return NULL;
  }

  // nil 노드 생성 및 초기화 -> 이거 t->nil 에 담아서 계속 쓸거얌
  node_t *nil_node = (node_t *)calloc(1, sizeof(node_t));
  if (nil_node == NULL) {
    free(p);
    return NULL;
  }
  // nil 노드 초기화
  nil_node->color = RBTREE_BLACK;
  nil_node->left = nil_node->right = nil_node->parent = nil_node;

  p->nil = nil_node;
  p->root = p->nil;

  return p;
}


// free_subtree 노드 삭제
void free_subtree(rbtree *t, node_t *n) {
  if (n == t->nil) {
    return;
  }

  free_subtree(t, n->left);
  free_subtree(t, n->right);
  free(n);
}

/**
 * delete_rbtree
 * : rbtree 삭제
 */
void delete_rbtree(rbtree *t) {
  // null case defence
  if (t == NULL) {
    return;
  }

  // 여기서 생성한 노드를 free 처리 해 줘야 하는군... 재귀로 고
  free_subtree(t, t->root);

  // nil 노드와 트리 구조체 해제
  free(t->nil);
  free(t);
}


// left rotate
void left_rotate(rbtree *t, node_t *x) {
  node_t *y = x->right;
  // y의 왼쪽 서브트리를 x의 오른쪽 서브트리로 회전한다
  x->right = y->left;
  // y의 왼쪽 서브트리가 비어있지 않은 경우
  // x는 서브트리 루트의 부모가 된다
  if (y->left != t->nil) {
    y->left->parent = x;
  }

  // x의 부모가 y의 부모가 된다
  y->parent = x->parent;

  // x가 루트인 경우
  if (x->parent == t->nil) {
    t->root = y;
  } else if (x == x->parent->left) {
    x->parent->left = y;
  } else {
    x->parent->right = y;
  }
  y->left = x;
  x->parent = y;
}

// right rotate
void right_rotate(rbtree *t, node_t *x) {
  node_t *y = x->left;
  // y의 오른쪽 서브트리를 x의 왼쪽 서브트리로 회전한다
  x->left = y->right;
  // y의 오른쪽 서브트리가 비어있지 않은 경우
  // x는 서브트리 루트의 부모가 된다
  if (y->right != t->nil) {
    y->right->parent = x;
  }

  // x의 부모가 y의 부모가 된다
  y->parent = x->parent;

  // x가 루트인 경우
  if (x->parent == t->nil) {
    t->root = y;
  } else if (x == x->parent->left) {
    x->parent->left = y;
  } else {
    x->parent->right = y;
  }
  y->right = x;
  x->parent = y;
}

// insert 시 fixup
void insert_fixup(rbtree *t, node_t *z) {
  node_t *y;
  while (z->parent->color == RBTREE_RED) {
    // z의 부모가 왼쪽 자식일 경우
    if (z->parent == z->parent->parent->left) {
      // y = z의 삼촌
      y = z->parent->parent->right;
      // 부모와 삼촌이 모두 RED 일 때 -> CASE I
      if (y != t->nil && y->color == RBTREE_RED) {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      } else {
        // CASE II
        if (z == z->parent->right) {
          z = z->parent;
          left_rotate(t, z);
        }
        // CASE III
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t, z->parent->parent);
      }
    // z의 부모가 오른쪽 자식일 경우
    } else {
      // y = z의 삼촌
      y = z->parent->parent->left;
      // 부모와 삼촌이 모두 RED 일 때 -> CASE I
      if (y != t->nil && y->color == RBTREE_RED) {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      } else {
        // CASE II
        if (z == z->parent->left) {
          z = z->parent;
          right_rotate(t, z);
        }
        // CASE III
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotate(t, z->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}

/**
 * rbtree_insert
 * : rbtree 삽입
 */
node_t *rbtree_insert(rbtree *t, const key_t key) {
  // declar new node
  node_t *new_node = (node_t *)calloc(1, sizeof(node_t));
  new_node->key = key;
  new_node->color = RBTREE_RED; // 새 노드는 반드시 red
  new_node->left = new_node->right = new_node->parent = t->nil; // 새로 삽입된 노드는 반드시 자식노드가 nil

  // 노드 탐색 시작
  node_t *x = t->root;
  node_t *y = t->nil;
  while (x != t->nil) {
    y = x;
    if (new_node->key < x->key) {
      x = x->left;
    } else {
      x = x->right;
    }
  }

  // 삽입되는 노드 x의 부모로 y를 링크, key 비교해서 노드 삽입
  new_node->parent = y;
  if (y == t->nil) {
    t->root = new_node;
  } else if (new_node->key < y->key) {
    y->left = new_node;
  } else {
    y->right = new_node;
  }

  // fix-up
  insert_fixup(t, new_node);

  return new_node;
}


// 노드 탐색하는 재귀함수
node_t *find_node(const rbtree *t, node_t *x, const key_t key) {
  if (x->key == key) {
    // 내가 찾으려던 노드 발견함
    return x;
  }
  // 노드의 끝까지 탐색
  if (x == t->nil) {
    return NULL;
  }

  // 노드 좌측 서브트리 탐색
  node_t *the_left_node = find_node(t, x->left, key);
  if (the_left_node != NULL) {
    return the_left_node;
  }
  // 탐색 결과가 없으면 우측 서브트리 탐색
  return find_node(t, x->right, key);
}

/**
 * rbtree_find
 * : 트리 내에서 key 값 찾기
 */
node_t *rbtree_find(const rbtree *t, const key_t key) {
  // 재귀로 돌려버립쉬다
  node_t *n = find_node(t, t-> root, key);
  return n;
}


// 시작노드가 주어질 때 서브트리 내 최솟값 찾기
node_t *tree_min(const rbtree *t, node_t *n) {
  if (n == NULL) {
    return NULL;
  }
  node_t *x = n;
  while (x->left != t->nil) {
    x = x->left;
  }
  return x;
}

/**
 * rbtree_min
 * : 트리 내 최솟값 찾기
 */
node_t *rbtree_min(const rbtree *t) {
  if (t == NULL) {
    return NULL;
  }
  // 최솟값 = left의 끝
  node_t *n = t->root;
  while (n->left != t->nil) {
    n = n->left;
  }
  return n;
}


// 시작노드가 주어질 때 서브트리 내 최댓값 찾기
node_t *tree_max(const rbtree *t, node_t *n) {
  if (n == NULL) {
    return NULL;
  }
  node_t *x = n;
  while (n->right != t->nil) {
    x = x->right;
  }
  return x;
}

/**
 * rbtree_max
 * : 트리 내 최댓값 찾기
 */
node_t *rbtree_max(const rbtree *t) {
  if (t == NULL) {
    return NULL;
  }
  // 최댓값 = right의 끝
  node_t *n = t->root;
  while (n->right != t->nil) {
    n = n->right;
  }
  return n;
}


// 노드 바꿔치기
void rbtree_transplant(rbtree *t, node_t *u, node_t *v) {
  if (u->parent == t->nil) {
    t->root = v;
  } else if (u == u->parent->left) {
    u->parent->left = v;
  } else {
    u->parent->right = v;
  }
  v->parent = u->parent;
}

// delete 시 fixup
void delete_fixup(rbtree *t, node_t *x) {
  while (x != t->root && x->color == RBTREE_BLACK) {
    // x가 왼쪽 자식일 때
    if (x == x->parent->left) {
      // w = x의 형제
      node_t *w = x->parent->right;
      // CASE I
      if (w->color == RBTREE_RED) {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        w = x->parent->right;
      }
      // CASE II
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) {
        w->color = RBTREE_RED;
        x = x->parent;
      } else {
        // CASE III
        if (w->right->color == RBTREE_BLACK) {
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          right_rotate(t, w);
          w = x->parent->right;
        }
        // CASE IV
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotate(t, x->parent);
        x = t->root;
      }
    // x가 오른쪽 자식일 때
    } else {
      node_t *w = x->parent->left;
      // CASE I
      if (w->color == RBTREE_RED) {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        w = x->parent->left;
      }
      // CASE II
      if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK) {
        w->color = RBTREE_RED;
        x = x->parent;
      } else {
        // CASE III
        if (w->left->color == RBTREE_BLACK) {
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotate(t, w);
          w = x->parent->left;
        }
        // CASE IV
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root;
      }
    }
  }
  x->color = RBTREE_BLACK;
}

/**
 * rbtree_erase
 * : rbtree 노드 삭제
 */
int rbtree_erase(rbtree *t, node_t *z) {
  node_t *x;
  node_t *y = z;
  color_t y_original_color = y->color;
  // 왼쪽 자식노드가 없을 때
  if (z->left == t->nil) {
    x = z->right;
    // z를 오른쪽 자식으로 바꾼다
    rbtree_transplant(t, z, z->right);
  // 오른쪽 자식 노드가 없을 때
  } else if (z->right == t->nil) {
    x = z->left;
    // z를 왼쪽 자식으로 바꾼다
    rbtree_transplant(t, z, z->left);
  } else {
    // y = z의 후손
    y = tree_min(t, z->right);
    y_original_color = y->color;
    x = y->right;
    if (y != z->right) {
      rbtree_transplant(t, y, y->right);
      y->right = z->right;
      y->right->parent = y;
    } else {
      x->parent = y;
    }
    rbtree_transplant(t, z, y);
    y->left = z->left;
    y->left->parent = y;
    y->color = z->color;
  }

  // 삭제한 노드 free 처리
  free(z);

  // red-black 위반 발생 시 fixup
  if (y_original_color == RBTREE_BLACK) {
    delete_fixup(t, x);
  }
  return 0;
}


// 트리를 배열에 담는 재귀함수
void tree_inorder(const rbtree *t, node_t *n, key_t *arr, int *idx) {
  if (n == t->nil) {
    return;
  }

  // left search
  tree_inorder(t, n->left, arr, idx);

  // 현재 키값 저장
  arr[*idx] = n->key;
  (*idx)++;

  // right search
  tree_inorder(t, n->right, arr, idx);
}

/**
 * rbtree_to_array
 * : rbtree 를 array 형태로 출력 -> 나중에 오름차순 정렬 array 와 비교함
 */
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // tree is empty
  if (t->root == t->nil) {
    return 0;
  }

  // 배열 index 추적용 변수 선언
  int idx = 0;
  tree_inorder(t, t->root, arr, &idx);
  return 0;
}
