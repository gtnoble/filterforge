(load-extension "filtopt" "init_filtopt")
(use-modules (srfi srfi-64))

(test-begin "component-test")

(test-begin "capacitor-test")

(define range-floor (floor-preferred-value 2.5))
(define range-ceil (ceiling-preferred-value 420))
(define component-value (nearest-preferred-value 69))

(define capacitor-component (make-component `capacitor component-value range-floor range-ceil))
(test-equal (get-component-value capacitor-component) component-value)
(test-equal (get-component-lower-limit capacitor-component) range-floor)
(test-equal (get-component-upper-limit capacitor-component) range-ceil)
(define randomized-capacitor (component-random-update capacitor-component))
(test-end "capacitor-test")


(test-end "component-test")

