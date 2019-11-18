(defpackage #:shape
  (:use #:cl)
  (:export
   #:circle
   #:square
   #:draw
   #:intersec))

(in-package :shape)

(defclass circle ()
  (radius))

(defclass square ()
  (length))

(defgeneric draw (shape)
  (:documentation "draw a shape"))

(defmethod draw ((shape circle))
  (format t "Draw a Circle"))

(defmethod draw ((shape square))
  (format t "Draw a Square"))

(defgeneric intersec (shape1 shape2)
  (:documentation "Compute shape intersection"))

(defmethod intersec ((shape1 circle) (shape2 circle))
  (format t "Compute Circle - Circle intersection"))

(defmethod intersec ((shape1 circle) (shape2 square))
  (format t "Compute Circle - Square intersection"))

(defmethod intersec ((shape1 square) (shape2 circle))
  (format t "Compute Square - Circle intersection"))

(defmethod intersec ((shape1 square) (shape2 square))
  (format t "Compute Square - Square intersection"))
