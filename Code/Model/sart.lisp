;;
;;	Sustained Attention to Response Task (SART)
;;
;;	In each trial the participant sees a letter: "O" or "Q".
;;	They must press a key every time an O appears (90% of trials),
;;	but withhold their response when the stimulus is a Q (10% of trials).
;;
;;	Practical Cognitive Science 2018
;;


;;===================;;
;;  Experiment code  ;;
;;===================;;


;; Experiment settings
(defvar *stimulus-duration* 2) ; number of seconds the stimulus is shown
(defvar *inter-stimulus-interval* 2.5) ; number of seconds between trials
(defvar *target-trials* 240) ; number of target trials
(defvar *non-target-trials* 24) ; number of non-target trials

(defvar *output-directory* "~/output/") ; location where output files are stored
(defvar *trace-file-name* "sart-trace") ; name of file in which the trace is stored

(defvar *visible* nil) ; visibility of the experiment window

;; Global variables for data storage
(defvar *stimuli* nil)
(defvar *trial-response* nil)
(defvar *trial-start* nil)
(defvar *trial-rt* nil)
(defvar *trial-done* nil)
(defvar *all-responses* nil)
(defvar *all-rts* nil)

;; Do SART experiment n times, save trace to output file
(defun do-sart-n (n)
	(with-open-file 
		(*standard-output* 
			(ensure-directories-exist 
				(merge-pathnames 
					(make-pathname :name *trace-file-name* :type "txt")  
					*output-directory*
				)
            )
			:direction :output :if-does-not-exist :create :if-exists :supersede
		)
		
		(setf *visible* nil)
		(format t "Running ~a model participants~%" n)
		(dotimes (i n)
			(setf participant (1+ i))
			(format t "Run ~a...~%" participant)
			(do-sart)
			(write-results-to-file (concatenate 'string "dat" (write-to-string participant)) participant *stimuli* (reverse *all-responses*) (reverse *all-rts*))
		)
		(format t "Done~%")
	)
)

;; Do SART experiment 1 time
(defun do-sart ()
	(reset)
	(setf *all-responses* nil)
	(setf *all-rts* nil)
	(setf *stimuli* (permute-list (concatenate 'list (make-array *target-trials* :initial-element "O") (make-array *non-target-trials* :initial-element "Q"))))
	(setf *visible* nil)
	(loop for stim in *stimuli* do (run-trial stim))
)

;; Do a single SART trial with a target stimulus
(defun do-sart-trial-o ()
	(setf *visible* t)
	(run-trial "O")
)

;; Do a single SART trial with a non-target stimulus
(defun do-sart-trial-q ()
	(setf *visible* t)
	(run-trial "Q")
)


;; Execute a trial with a given stimulus
(defun run-trial (stim)
  (let ((window (open-exp-window "SART Experiment"
                                 :visible *visible*
                                 :width 300
                                 :height 300
                                 :x 300
                                 :y 300))
        )
    
    (add-text-to-exp-window :text stim
                              :width 30
							  :height 30
                              :x 145
                              :y 150)
    
    (setf *trial-response* nil)
    (setf *trial-start* (get-time))
    (setf *trial-rt* nil)
	(setf *trial-done* nil)
	
	(install-device window)
	(proc-display)
	(run-full-time *stimulus-duration* :real-time *visible*)
	(allow-event-manager window)
	(clear-exp-window)
	(proc-display)
	(run-full-time *inter-stimulus-interval* :real-time *visible*))
	
	(push *trial-response* *all-responses*)
	(push *trial-rt* *all-rts*)
)

;; Register the model's key presses
(defmethod rpm-window-key-event-handler ((win rpm-window) key)
  (setf *trial-rt* (/ (- (get-time) *trial-start*) 1000.0))
  (setf *trial-response* (string key))
  (setf *trial-done* t)
)

;; Write the behavioural results to a file
(defun write-results-to-file (name participant stimuli responses rts)
	(with-open-file
		(out
			(ensure-directories-exist
				(merge-pathnames
					(make-pathname :name name :type "csv")
					*output-directory*
				)
			)
			:direction :output :if-does-not-exist :create :if-exists :supersede
		)
		(format out "participant, trial, stimulus, response, rt~%")
		(loop 
			for trial from 1
			for stimulus in stimuli
			for response in responses
			for rt in rts
			do (format out "~a, ~a, ~a, ~a, ~a~%" participant trial stimulus response rt)
		)
	)	
)


	
;;===================;;
;;    Model code     ;;
;;===================;;

(clear-all)

(define-model sart

;; Model parameters
(sgp :v t ; main trace detail
	:act low ; activation trace detail
	:sact t ; include activation trace in main trace

	:show-focus t ; show where the model is looking
	:esc t ; enable sub-symbolic level
	:rt -5 ; retrieval threshold
	:bll 0.5 ; base-level learning
	:ans 0.5 ; activation noise
	:egs 0.2
)

(chunk-type beginning label)
(chunk-type goal state)
(chunk-type subgoal step)
(chunk-type srmapping stimulus hand)
(chunk-type memWander wanderNext)

(add-dm
	(start isa chunk)
	(press-on-O isa srmapping stimulus "O" hand left)
	(withhold-on-Q isa srmapping stimulus "Q" hand nil)
	(startgoal isa beginning label start)
	(attend isa goal state attend)
	(wander isa goal state wander)
	(identify isa subgoal step identify)
	(get-response isa subgoal step get-response)
	(make-response isa subgoal step make-response)
	
	;memories
	(m1 isa memWander wanderNext "yes")
	(m2 isa memWander wanderNext "yes")
	(m3 isa memWander wanderNext "yes")
	(m4 isa memWander wanderNext "no")
)

(set-base-levels
	(attend			9500	-9900)
	(wander			500		-500)
	(press-on-O		10000	-10000)
	(withhold-on-Q	10000	-10000)
	
	(m1				10000	-10000)
	(m2				10000	-10000)
	(m3				10000	-10000)
	(m4				10000	-10000)

)

(p start-task
	=goal>
		isa			beginning
		label		start
	?retrieval>
		buffer		empty
		state		free
	-	state		error
==>
	+retrieval>
		isa			goal
		state		attend
	-goal>
)

(p check-current-goal
	=retrieval>
		isa			goal
		state		attend
	?retrieval>
		state		free
	-	state		error
	?goal>
		buffer empty
	?visual>
	-	scene-change T
==>
	=retrieval>
		state		nil ; clear retrieval buffer without strengthening chunk
	-retrieval>
	+retrieval>
		isa			goal
	-	state		nil
)

;production for start of mindwandering


(p mindwander-response
	=retrieval>
		isa			goal
		state		wander
	=visual-location>
	?visual>
		state		free
	?manual>
		state		free
==>
	+visual>
		isa			clear-scene-change	;;Definitely needed
	-visual>		;;Possibly needed
	-visual-location>	;;Maybe not needed
	+manual>
		isa			punch
		hand		left
		finger		index
	=retrieval>
) 

#|
(p wander-identify-stimulus
	?goal>
		buffer		empty
	=retrieval>
		isa			goal
		state		wander
	=visual-location>
	?visual>
		state		free

==>
	+visual>
		isa			move-attention
		screen-pos	=visual-location
	+goal>
		isa			subgoal
		step		get-response
	=retrieval>
		;state		nil ; clear retrieval buffer without strengthening chunk
	;-retrieval>
)
#|
(p wander-identify-stimulus-fail
	?goal>
		buffer		empty
	=retrieval>
		isa			goal
		state		wander
	=visual-location>
	?visual>
		state		free
==>
	=retrieval>
)

(spp wander-identify-stimulus-fail :u 0.18)
|#
(p mindwander-retrieve-response
	=goal>
		isa			subgoal
		step		get-response
	=visual>
		isa			text
		value		=letter
	?visual>
		state		free
	=retrieval>
		isa			goal
		state		wander
==>
	=retrieval>
	+goal>
		isa			subgoal
		step		make-response
	+visual>
		isa			clear-scene-change
)

(p mindwander-fire-response
	=goal>
		isa			subgoal
		step		make-response
	=retrieval>
		isa			goal
		state		wander
	?manual>
		state		free
==>
	+manual>
		isa			punch
		hand		left
		finger		index
	-goal>
	-visual-location>
	-visual>
	=retrieval>
)
|#


(p mindwander-retrieve
	?goal>
		buffer empty
	=retrieval>
		isa			goal
		state		wander
	?visual>
	-	scene-change T
	?retrieval>
		state		free
	-	state		error
==>
	=retrieval>
		state		nil
	-retrieval>
	+retrieval>
		isa			memWander
	-	wanderNext	nil
)


(p mindwander-snap-out
	?goal>
		buffer empty
	=retrieval>
		isa			goal
		state		wander
	?visual>
	-	scene-change T
	?retrieval>
		state		free
	-	state		error
==>
	+retrieval>
		isa			goal
		state		attend
)
;1.2, 0.4
(spp mindwander-snap-out :u 1.5)
(spp mindwander-retrieve :u 0.4)

(p mindwander-continue
	=retrieval>
		isa			memWander
		wanderNext	"yes"
	?retrieval>
		state		free
	-	state		error
==>
	+retrieval>
		isa			goal
		state		wander
)


(p mindwander-stop
	=retrieval>
		isa			memWander
		wanderNext	"no"
	?retrieval>
		state		free
	-	state		error
==>
	+retrieval>
		isa			goal
		state		attend
)


(p identify-stimulus
	?goal>
		buffer		empty
	=retrieval>
		isa			goal
		state		attend
	=visual-location>
	?visual>
		state		free

==>
	+visual>
		isa			move-attention
		screen-pos	=visual-location
	+goal>
		isa			subgoal
		step		get-response
	=retrieval>
		state		nil ; clear retrieval buffer without strengthening chunk
	-retrieval>
)

(p retrieve-response
	=goal>
		isa			subgoal
		step		get-response
	=visual>
		isa			text
		value		=letter
	?visual>
		state		free
	?retrieval>
		buffer empty
==>
	+retrieval>
		isa			srmapping
		stimulus	=letter
	+goal>
		isa			subgoal
		step		make-response
	+visual>
		isa			clear-scene-change
)

(p respond-if-O
	=goal>
		isa			subgoal
		step		make-response
	=retrieval>
		isa			srmapping
		stimulus	=letter
		hand		=hand
	?manual>
		state		free
==>
	+manual>
		isa			punch
		hand		=hand
		finger		index
	-goal>
	-visual-location>
	-visual>
	+retrieval>
		isa			goal
	-	state		nil
)

(p do-not-respond-if-Q
	=goal>
		isa			subgoal
		step		make-response
	=retrieval>
		isa			srmapping
		stimulus	=letter
		hand		nil
	;;Don't need to check manual state because we won't be moving
	;?manual>
	;	state		free
==>
	;;Manual action not needed because we won't be moving
	;+manual>
	;	isa			punch
	;	hand		=hand
	;	finger		index
	-goal>
	-visual-location>
	-visual>
	+retrieval>
		isa			goal
	-	state		nil
)

(goal-focus startgoal)

)
