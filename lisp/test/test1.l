;; (+ 1 2)
;; (setq b 3)
;; (setq z
;;       (+ b 5))
;; ( + b
;;     (+ b 4)
;;     (+ z 10))

;; (defun plus1 (b)
;;   (+ b 1)
;;   )

;; (defun fun (a b)
;;   (setq c  (+ a
;;               (plus1 b)))
;;   (+ c 3))

;; (fun 3 3)

;; (+)
;; ;;(-)
;; (- 3)
;; (- 3 5)

;; 'c
;; ''3

;; '( 3 5 8 '(lol a))

;; ;;(fun g h)

;; (defun make-table ()
;;   (let
;;       (( table '() ))
;;     (lambda (a)
;;       (setq table (cons a table))
;;       table)))

;; (setq table (make-table))

;; (defun funcall (a b)
;;   "funcall for lisp-like-ness but this is stupid as we have only one global namespace"
;;   (a b))

;; (table 3)
;; (table 3)
;; (table 3)

;; ;;(funcall table 3)
;; ;;(funcall table 1)

;; (setq table2 (make-table))
;; (funcall table2 1)

;; (defun test (c)
;;   (let ((x 3))
;;     (+ c x )))

;; (test 3)

;; (silent-setq ''silent-setq 'b)

;; ;silent-setq

(defun test-bug (a b c)
  (+ a . b)  c
  )


;;(test-bug 1 2 3)
;;(+ 1 . 2)
