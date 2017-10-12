//(define (singleton_tree x)
// (cons x '(EmptyTree EmptyTree)))
//
// (define (insert x tree)
// (if (eq? tree 'EmptyTree)
//    (singleton_tree x)
//    (let ( (var (car tree)) )
//      (cond
//        ((eq? var x)
//         tree)
//        ((< var x)
//         `(,var ,(insert x (cadr tree)) ,(caddr tree)))
//        ((> var x)
//         `(,var  ,(cadr tree)  ,(insert x (caddr tree))))
//        (else 'caonima)))))
//
//
// (define (find  x tree)
//   (if (eq? tree 'EmptyTree)
//     'false
//     (let ( (var (car tree)) )
//        (cond
//          ((eq? var x) 'true)
//          ((< var x)
//           (find x (cadr tree)))
//          ((> var x)
//           (find x (caddr tree)))))))
//
// (define (reduce func x lot)
//   (if (null? lot)
//      x
//      (reduce func (func  (car lot) x) (cdr lot))))
//
//
//(define aa (reduce insert 'EmptyTree `(31 432 5 321 5  6 8 2 5 7 321  765 2  6 8)))

//data Tree a = EmptyTree | Node a (Tree a) (Tree a) deriving (Show, Read, Eq)
//
//singleton :: a -> Tree a
//singleton x = Node x EmptyTree EmptyTree
//
//treeInsert :: (Ord a) => a -> Tree a -> Tree a
//treeInsert x EmptyTree = singleton x
//treeInsert x (Node a left right)
//      | x == a = Node x left right
//      | x < a  = Node a (treeInsert x left) right
//      | x > a  = Node a left (treeInsert x right)


//treeElem :: (Ord a) => a -> Tree a -> Bool
//treeElem x EmptyTree = False
//treeElem x (Node a left right)
//    | x == a = True
//    | x < a  = treeElem x left
//    | x > a  = treeElem x right


//为了让代码不那么蛋疼。。。。特别加入模式匹配和adt的语法糖
