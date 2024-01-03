(load-extension "filtopt" "init_filtopt")
(use-modules (srfi srfi-64))

(test-begin "load-test")

(test-begin "resistor-load")
(define range-floor (floor-preferred-value 2.5))
(define range-ceil (ceiling-preferred-value 420))
(define component-value (nearest-preferred-value 100))

(define resistor (make-component `resistor component-value range-floor range-ceil))
(define load (make-component-load resistor))

(test-equal 100.0+0.0i (impedance 22 load))
(test-end "resistor-load")

(test-begin "series-load")
(define series-load (make-series-load (vector load load)))
(test-equal "series-impedance" 200.0+0.0i (impedance 22 series-load))
(test-end "series-load")

(test-begin "parallel-load")
(define parallel-load (make-parallel-load (vector load load)))
(test-equal "parallel-impedance" 50.0+0.0i (impedance 22 parallel-load))
(test-end "parallel-load")

(test-end "load-test")