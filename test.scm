(load-extension "filtopt" "init_filtopt")
(use-modules (srfi srfi-64))

(test-begin "preferred-value-test")

(define approximate-tolerance 0.001)

(test-begin "test-range-floor")
(define range-floor (floor-preferred-value 2.5))
(test-approximate 2.4 (evaluate-preferred-value range-floor) approximate-tolerance)
(test-equal 9 (get-preferred-value-index range-floor))
(test-equal 0 (get-preferred-value-order-of-magnitude range-floor))
(test-end "test-range-floor")

(test-begin "test-range-ceiling")
(define range-ceil (ceiling-preferred-value 420))
(test-approximate 430 (evaluate-preferred-value range-ceil) approximate-tolerance)
(test-equal 15 (get-preferred-value-index range-ceil))
(test-equal 2 (get-preferred-value-order-of-magnitude range-ceil))
(test-end "test-range-ceiling")

(test-begin "test-nearest-value")
(define component-value (nearest-preferred-value 69))
(test-equal 20 (get-preferred-value-index component-value))
(test-equal 1 (get-preferred-value-order-of-magnitude component-value))
(test-approximate 68 (evaluate-preferred-value component-value) approximate-tolerance)
(test-end "test-nearest-value")

(test-begin "test-increment-decrement")
(test-approximate 75 
                  (evaluate-preferred-value 
                    (increment-preferred-value component-value)) 
                  approximate-tolerance)
(test-equal 21 (get-preferred-value-index component-value))
(test-equal 1 (get-preferred-value-order-of-magnitude component-value))
(test-approximate 68 
                  (evaluate-preferred-value 
                    (decrement-preferred-value component-value)) 
                  approximate-tolerance)
(test-equal 20 (get-preferred-value-index component-value))
(test-equal 1 (get-preferred-value-order-of-magnitude component-value))
(test-end "test-increment-decrement")

(test-end "preferred-value-test")

(test-begin "component-test")

(test-begin "capacitor-test")
(define capacitor-component (make-component `capacitor component-value range-floor range-ceil))
(test-equal (get-component-value capacitor-component) component-value)
(test-equal (get-component-lower-limit capacitor-component) range-floor)
(test-equal (get-component-upper-limit capacitor-component) range-ceil)
(define randomized-capacitor (component-random-update capacitor-component))
(test-end "capacitor-test")


(test-end "component-test")





