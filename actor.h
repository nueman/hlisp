#ifndef ______ACTOR_____
#define ______ACTOR_____
/*
 erlang 写发大致如下
foo()
  recevie
       "aa"->
	     .....
	     foo()
	"bb"->
	     .....
	     foo()
	 _ ->
	     foo()
end

Pid =swpan(foo)
Pid ! "aa"


我承认在case上lisp写出来确实难看！
(defn foo ()
  (let [x recevie]
    (case x
      (1 (do (.....)
                 (foo)))
      (2 (do (.....)
                 (foo)))
      (all  (foo)))))

  (define pid (swpan    foo))  //创建栈！


  (send 1 foo)




*/




#endif
