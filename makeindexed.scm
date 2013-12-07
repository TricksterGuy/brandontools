(define (script-fu-make-indexed file-in num file-out)
(let* (
	(image    (car (gimp-file-load RUN-NONINTERACTIVE file-in "")))
	(drawable (car (gimp-image-active-drawable image)))
        )
	(gimp-image-convert-indexed image 0 0 num FALSE TRUE "")
	(gimp-file-save   RUN-NONINTERACTIVE image drawable file-out "")
)
)

(script-fu-register "script-fu-make-indexed"
   ""
   "Make an image indexed color"
   "Brandon Whitehead"
   "2011, Brandon Whitehead"
   "February 26, 2011"
   ""
   SF-VALUE "In Filename" ""
   SF-VALUE "Palette Entries" "0"
   SF-VALUE "Out Filename" ""
)
