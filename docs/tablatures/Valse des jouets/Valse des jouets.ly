\version "2.16.2"

#(set-global-staff-size 20)

%=================
%   PARAMETRES
%=================

#(define cadb #t) %Cogeron f CADB t
#(define fingerDisplay #f)
#(define bassDisplay #f)
#(define rankNumber <ranks>)

scoreIn = g
scoreOut = g

%=====================================
%   FONCTION D'ANALYSE PT et CADB
%=====================================

%%Definition d'une fonction scheme pour le calcul du rang du diato
#(define rang
    (lambda (touch)
        (define l (string-length touch))	;longueur de la string
             (define rng 0)						;par défaut rang 1
             (if (string=? "'" (substring touch (- l 1) l))
                 (if (string=? "'" (substring touch (- l 2) (- l 1)))
                     (+ rng (if (= rankNumber 3) 2 1))
		     (+ rng (if (= rankNumber 3) 1 2))
		 )
		 rng
	     )
    )
)

%%Définition d'une fonction pour retourner uniquement le numéro de la touche
%% sans les évetuelles apostrophes.
#(define touchnum
    (lambda (touch)
        (if (< 1 (string-length touch))
             (if (char-numeric? (string-ref touch 1))
                 (substring touch 0 2)
	         (substring touch 0 1)
	     )
	     touch
        )
    )
)

% Definition d'une fonction pour noter les notes tirées
% Pour le système corgeron il suffit de rajouter un underline au markup
%t =
%#(define-music-function (parser location button) (string?)
%(if cadb
%(make-music 'TextScriptEvent 'direction 0
%'text ( markup #:fontsize -1.5 button)
%)
%( make-music 'TextScriptEvent 'direction (rang button)
%'text ( markup #:underline #:fontsize -1.5 (touchnum button)))
%))

% Définition d'une fonction pour les notes poussées
%p =
%#(define-music-function (parser location button) (string?)
%(if cadb
%  (make-music 'TextScriptEvent 'direction 1
%  'text ( markup #:fontsize -1.5 button))
%  ( make-music 'TextScriptEvent 'direction (rang button)
%  'text ( markup #:fontsize -1.5 (substring button 0 1)))
%))

p =
#(define-music-function (parser location button) (string?)
(if cadb
(make-music ; cadb
  'SequentialMusic
  'elements
  (list (make-music
  'SequentialMusic
  'elements
  (list (make-music
          'ContextSpeccedMusic
          'context-type
          'Bottom
          'element
          (make-music
            'OverrideProperty
            'pop-first
            #t
            'grob-property-path
            (list (quote direction))
            'grob-value
            1
            'symbol
            'TextSpanner))
        (make-music
          'ContextSpeccedMusic
          'context-type
          'Bottom
          'element
          (make-music
            'OverrideProperty
            'once #t
            'pop-first #t
            'grob-property-path
            (list (quote extra-offset))
            'grob-value
            (cons 0 -0.4)
            'symbol
            'TextSpanner))))

        (make-music ; add markup
            'TextScriptEvent 'direction 1
            'text ( markup #:fontsize -1.5 button))))
  
(if (not (eq? (rang button) 1)) ; si ce n'est pas le rang du milieux
(make-music ;cogeron
  'SequentialMusic
  'elements
  (list (make-music ; override textspanner position
          'ContextSpeccedMusic
          'context-type
          'Bottom
          'element
          (make-music
            'OverrideProperty
            ;'pop-first #t
            'grob-property-path
            (list (quote direction))
            'grob-value (if (< (rang button) 1) -1 1)   
            'symbol
            'TextSpanner))
        (make-music  ; add markup
            'TextScriptEvent 'direction (rang button)
            'text ( markup #:fontsize -1.5 (touchnum button)))))

  (make-music
  'SequentialMusic
  'elements
  (list (make-music
          'ContextSpeccedMusic
          'context-type
          'Bottom
          'element
          (make-music
            'OverrideProperty
            'once #t
            ;'pop-first #t
            'grob-property-path
            (list (quote extra-offset))
            'grob-value (cons 0 -2.9)
            'symbol
            'TextScript))
        (make-music ; override textspanner position
          'ContextSpeccedMusic
          'context-type
          'Bottom
          'element
          (make-music
            'OverrideProperty
            'once #t
            ;'pop-first #t
            'grob-property-path
            (list (quote direction))
            'grob-value (if (< (rang button) 1) -1 1)
            'grob-property-path
            (list (quote extra-offset))
            'grob-value (cons 0 2.9)
            'symbol
            'TextSpanner))
        (make-music
         'TextScriptEvent 
         'direction (rang button)
         'text ( markup #:fontsize -1.5 (touchnum button)))))
  )
))

t =
#(define-music-function (parser location button) (string?)
(if cadb
(make-music ; cadb
  'SequentialMusic
  'elements
  (list (make-music
  'SequentialMusic
  'elements
  (list (make-music
          'ContextSpeccedMusic
          'context-type
          'Bottom
          'element
          (make-music
            'OverrideProperty
            'pop-first
            #t
            'grob-property-path
            (list (quote direction))
            'grob-value
            -1
            'symbol
            'TextSpanner))
        (make-music
          'ContextSpeccedMusic
          'context-type
          'Bottom
          'element
          (make-music
            'OverrideProperty
            'once #t
            'pop-first #t
            'grob-property-path
            (list (quote extra-offset))
            'grob-value
            (cons 0 0.4)
            'symbol
            'TextSpanner))))
    
        (make-music ; add markup
            'TextScriptEvent 'direction 0
            'text ( markup #:fontsize -1.5 button))))
  
(if (not (eq? (rang button) 1)) ; si ce n'est pas le rang du milieux
(make-music ;cogeron
  'SequentialMusic
  'elements
  (list (make-music ; override textspanner position
          'ContextSpeccedMusic
          'context-type
          'Bottom
          'element
          (make-music
            'OverrideProperty
            'pop-first #t
            'grob-property-path
            (list (quote direction))
            'grob-value (if (< (rang button) 1) -1 1)   
            'symbol
            'TextSpanner))
        (make-music  ; add markup
            'TextScriptEvent 'direction (rang button)
            'text ( markup #:underline #:fontsize -1.5 (touchnum button)))))

  (make-music
  'SequentialMusic
  'elements
  (list (make-music
      'ContextSpeccedMusic
      'context-type
      'Bottom
      'element
      (make-music
        'OverrideProperty
        'pop-first
        #t
        'grob-property-path
        (list (quote direction))
        'grob-value
        -1
        'symbol
        'TextSpanner))
        (make-music ; override textspanner position
          'ContextSpeccedMusic
          'context-type
          'Bottom
          'element
          (make-music
            'OverrideProperty
            'once #t
            'pop-first #t
            'grob-property-path
            (list (quote direction))
            'grob-value (if (< (rang button) 1) -1 1)
            'grob-property-path
            (list (quote extra-offset))
            'grob-value (cons 0 2.9)
            'symbol
            'TextSpanner))
        (make-music
         'TextScriptEvent 
         'direction (rang button)
         'text ( markup #:underline #:fontsize -1.5 (touchnum button)))))
  )
))


up =
#(define-music-function (parser location button) (string?)
  (make-music 'TextScriptEvent 'direction 1
  'text ( markup #:fontsize -1.5 button))
)

fngr =
#(define-event-function (parser location finger) (string?)
  (make-music 'StrokeFingerEvent 'text finger)
)

%=================
%   PAGE FORMAT
%=================
\paper
{
  top-margin = 20\mm
  indent = 0\mm
  left-margin = 2\cm
  right-margin = 2\cm
}


%=============
%   HEADER
%=============
\header
{
  title="Valse des jouets"
  subtitle=""
  composer="Michel Faubert"
  instrument = ""
  tagline = ""
  poet=""
}


\markup { \vspace #2 }

%=====================
%	DATA
%=====================

diato =
{
  \set Timing.beamExceptions = #'() 
  
 
  
 \p  "7" b'8 \t  "7" a'8 \p  "7" b'8 \t  "8" c''8 \p  "7" b'8 \t  "7" a'8 \p  "6" g'8 \t  "5" e'8 \p  "5" d'8 \p  "4" b8 \p  "3" g4 
 \p  "7" b'8 \t  "7" a'8 \p  "7" b'8 \t  "8" c''8 \p  "7" b'8 \t  "7" a'8 \p  "6" g'8 \t  "5" e'8 \p  "5" d'8 \p  "4" b8 \p  "3" g4 
  
 \break 
  
 \t  "7" a'8 \p  "6" g'8 \t  "7" a'8 \p  "7" b'8 \t  "7" a'8 \p  "6" g'8  
 \t  "6" fis'4. \startTextSpan \p  "6" g'8 \stopTextSpan \t  "6" fis'8 \t  "5" e'8  
 \p  "5" d'8 \t  "3" a8 \p  "5" d'8 \t  "5" e'8 \t  "6" fis'8 \p  "5" d'8  
 \p  "6" g'8 \t  "5" e'8 \p  "5" d'8 \p  "4" b8 \p  "3" g4  
  
 \break 
  
 \p  "7" b'8 \t  "7" a'8 \p  "7" b'8 \t  "8" c''8 \p  "7" b'8 \t  "7" a'8 \p  "6" g'8 \t  "5" e'8 \p  "5" d'8 \p  "4" b8 \p  "3" g4 
 \p  "7" b'8 \t  "7" a'8 \p  "7" b'8 \t  "8" c''8 \p  "7" b'8 \t  "7" a'8 \p  "6" g'8 \t  "5" e'8 \p  "5" d'8 \p  "4" b8 \p  "3" g4 
  
 \break 
  
 \t  "7" a'8 \p  "6" g'8 \t  "7" a'8 \p  "7" b'8 \t  "7" a'8 \p  "6" g'8  
 \t  "6" fis'4. \startTextSpan \p  "6" g'8 \stopTextSpan \t  "6" fis'8 \t  "5" e'8  
 \p  "5" d'8 \t  "5" e'8 \p  "5" d'8 \t  "4" c'8 \p  "4" b8 \t  "3" a8 
 \p  "3" g2 \startTextSpan ~ g8 \p  "6" g'8 \stopTextSpan 
  
 
  
 \break 
  
 \p  "6" g'8 \t  "6" fis'8 \p  "6" g'8 \p  "5" d'8 \p  "4" b8 \p  "5" d'8 
 \t  "4" c'8 \p  "5" d'8 \t  "5" e'4. \startTextSpan \p  "6" g'8 \stopTextSpan  
 \p  "6" g'8 \t  "5" e'8 \t  "6" fis'8 \p  "5" d'8 \t  "5" e'8 \t  "6" fis'8  
 \p  "6" g'8 \t  "7" a'8 \p  "7" b'4. \startTextSpan \p  "7" b'8 \stopTextSpan 
  
 \break 
  
 \p  "6" g'8 \p  "7" b'8 \t  "9" e''8 \p  "7" b'8 \p  "6" g'8 \p  "7" b'8  
 \t  "9" e''8 \p  "7" b'8 \t  "8" c''4. \startTextSpan \t  "7" a'8 \stopTextSpan  
 \t  "7" a'8 \p  "6" g'8 \t  "6" fis'8 \p  "5" d'8 \t  "6" fis'8 \t  "7" a'8  
 \p  "8" d''8 \t  "7" a'8 \p  "7" b'4. \startTextSpan \p  "6" g'8 \stopTextSpan  
  
 \break 
  
 \p  "6" g'8 \t  "6" fis'8 \t  "5" e'8 \p  "4" b8 \p  "4" b8 \p  "4" b8  
 \t  "5" e'8 \p  "4" b8 \t  "4" c'4. \startTextSpan \t  "7" a'8 \stopTextSpan  
 \t  "7" a'8 \p  "6" g'8 \t  "6" fis'8 \t  "5" e'8 \p  "5" d'8 \t  "4" c'8  
 \p  "4" b8 \t  "3" a8 \p  "4" b2 \startTextSpan ~  
  
 \break 
  
 b4 \t  "4" c'8 \stopTextSpan \p  "4" b8 \t  "4" c'8 \p  "5" d'8 \t  "5" e'8 \t  "6" fis'8  
  
 \p  "6" g'8 \t  "7" a'8 \p  "7" b'4 \startTextSpan ~ b'8 \p  "7" b'8 \stopTextSpan 
  
 \t  "8" c''8 \t  "7" a'8 \t  "6" fis'8 \t  "7" a'8  
  
 \t  "5" e'8 \p  "5" d'8 \p  "6" g'2 \startTextSpan 
  
  
  
  \endSpanners \stopTextSpan
}

%=====================
%	PARTITION
%=====================

melody =
{
  \clef treble
  \tempo 4=120
  \time 3/4
  \key c \major

  \set strokeFingerOrientations = #'(up)
  \autoBeamOn
  \textLengthOn
  \diato
}

%=====================
%	TABLATURE
%=====================

tabulature =
{
  \autoBeamOff
  \textLengthOn
  \diato
}

% ===================
% 	BASSES
% ===================

bass =
{
  \autoBeamOff
  \textLengthOn
  
}

paroles = \lyricmode{  }


% =========================
% 	MISE EN PAGE
% =========================

graceSettings = \with
{
   graceSettings = #`(
    (Voice Stem direction ,UP)
    (Voice Stem font-size -5)
    (Voice Flag font-size -5)
    (Voice NoteHead font-size -5)
    (Voice TabNoteHead font-size -5)
    (Voice Dots font-size -3)
    (Voice Stem length-fraction 0.8)
    (Voice Stem no-stem-extend #t)
    (Voice Beam beam-thickness 0.25)
    (Voice Beam length-fraction 0.8)
    (Voice Accidental font-size -4)
    (Voice AccidentalCautionary font-size -4)
    (Voice Slur direction ,DOWN)
    (Voice Script font-size -3)
    (Voice Fingering font-size -8)
    (Voice StringNumber font-size -8)
    (Voice TextScript font-size -3)
  ) 
}

diatoStaffSetup = \with 
{
\override StringNumber #'text = #point-stencil
\override Fingering #'stencil = #point-stencil
\override StrokeFinger #'stencil = ##f
\override TextScript #'stencil = ##f
\override StringNumber #'stencil = ##f
\override TextSpanner #'stencil = #point-stencil
\override TextSpanner #'to-barline = ##t

\graceSettings
}

diatoFingeringSetup = \with 
{
  \remove "Time_signature_engraver"

  \override StaffSymbol #'line-count = #0
  \override StaffSymbol #'stencil = ##f
  \override StaffSymbol #'line-positions =  #'(0)
  \override StaffSymbol #'staff-space = #(magstep -30)
  \override Clef #'stencil = #ly:text-interface::print
  \override Clef #'text = \markup{\raise #.6 "Doigté"}
  \override Clef #'font-size = #-2
  \override Clef #'position = #8
  \override NoteHead #'stencil = ##f
  \override NoteHead #'stencil = #point-stencil
  \override KeySignature #'stencil = ##f
  \override Slur #'stencil = ##f
  \override Slur #'control-points = #'((0 . 0) (0 . 0) (0 . 0) (0 . 0))
  \override Stem #'stencil = ##f
  \override Stem #'length = #0
  \override Tie #'stencil = ##f
  \override Flag #'stencil = ##f
  \override Beam #'stencil = ##f
  \override Dots #'stencil = ##f
  \override TextScript #'stencil = ##f
  \override TupletBracket #'stencil = ##f
  \override TupletNumber #'stencil = ##f
  \override KeyChangeEvent #'stencil = #point-stencil
  \override StringNumber #'self-alignment-X = #-.7
  \override LedgerLineSpanner #'stencil = #point-stencil
  \override Accidental #'stencil = #point-stencil
  \override TextSpanner #'stencil = #point-stencil
  \override StrokeFinger #'font-size = #-3
  \override StrokeFinger #'padding = #10
  \override StrokeFinger #'self-alignment-X = #-.5
}

diatoButtonSetup = \with
{
  \remove "Time_signature_engraver"			% Retire la signature du temps
  
  \override StaffSymbol #'line-positions =
  #(if cadb
    #{ #'(0) #}
    (if (equal? rankNumber 3)
      #{ #'(-3 3) #}
      #{ #'(0) #}
    )
   )
  
  \override NoteHead #'stencil = #point-stencil 	% Supprime les notes
  \override Tie #'stencil = ##f
  \override Slur #'stencil = ##f
  \override Slur #'control-points = #'((0 . 0) (0 . 0) (0 . 0) (0 . 0))
  \override Stem #'length = #0				% Supprime les hampes des notes
  \override Stem #'flag = #point-stencil
  \override Stem #'stencil = #point-stencil
  \override Beam #'stencil = #point-stencil
  \override Beam #'positions = #'(0 . 0)  
  \override Staff #'stencil = #point-stencil
  \override StaffSymbol #'staff-space = #1
  \override LedgerLineSpanner #'stencil = #point-stencil
  \override Dots #'stencil = #ly:text-interface::print
  \override Dots #'text = ""
  \override Dots #'staff-position = #-2
  \override Flag #'stencil = #point-stencil  		% suppression des queues de croches
  \override Clef #'stencil = #ly:text-interface::print
  
  \override Clef #'text = 
  #(if cadb
    #{ \markup{\raise #0.6 \column{P T}} #}
    (if (equal? rankNumber 3)
      #{ \markup{\raise #2.1 \column{3 2 1}} #}
      #{ \markup{\raise #0.6 \column{C G}} #}
    )
   )
                  
  \override TextSpanner #'style = #'solid-line
  \override TextSpanner #'bound-details #'left #'padding = #2
  \override TextSpanner #'bound-details #'right #'padding = #1
  \override TextSpanner #'to-barline = ##t
  \override TextSpanner #'staff-padding = #1.4
  
  \override TupletBracket.bracket-visibility = ##f
  \override TupletNumber.text = ""
  
  \graceSettings
}

diatoBassSetup = \with
{
  \remove "Time_signature_engraver"
  \remove "Clef_engraver"
  
  \override StaffSymbol #'line-count = #1
  \override NoteHead #'font-size = #-50 	% Retire les notes de la tablature
  \override Stem #'length = #0
  \override Dots #'stencil = #ly:text-interface::print
  \override Dots #'text = ""
  \override Flag #'stencil = #point-stencil 		% suppression des queues de croches
  \override Rest #'stencil = ##f 		% Suppression de silence ajouté artificellement pour la sortie midi
  \override Rest #'font-size = #-20
  \override Tie #'stencil = ##f
  \override Slur #'stencil = ##f
  \override Slur #'control-points = #'((0 . 0) (0 . 0) (0 . 0) (0 . 0))
  \override TextSpanner #'style = #'solid-line
  \override TextSpanner #'bound-details #'left #'padding = #2
  \override TextSpanner #'bound-details #'right #'padding = #1.5
  \override TextSpanner #'to-barline = ##t
  \override TextSpanner #'staff-padding = #1.1
   \override VerticalAxisGroup #'staff-staff-spacing =  #'(('basic-distance  . 0) (minimum-distance . 0) (padding . .5) (stretchability . 0))
}

% ===================
% 	GRAVURE
% ===================

\score
{

  \new StaffGroup 
  <<
    %\override StaffGroup.SystemStartBracket #'transparent = ##t
    
    %\new Staff \with { \diatoStaffSetup } << \key c \major \transpose \scoreIn \scoreOut \melody >>  
    \new Staff \with { \diatoStaffSetup } << \melody \addlyrics{\paroles} >>  
    
    \new DrumStaff \with { \diatoButtonSetup } << \tabulature >> 
    
    #(if bassDisplay 
       #{ \new DrumStaff \with { \diatoBassSetup } << \bass >> #})
     
    #(if fingerDisplay 
       #{\new Staff \with { \diatoFingeringSetup } << \melody >> #}) 
   
  >>
  
  \layout 
  {
    \context 
    { %propriété des Lyrics
         \Lyrics
         \override LyricText #'font-name = #"URW Chancery L"
         \override LyricText #'font-size =  #-1.0
    }
   } 
}

\markup {
  \fill-line {
    {
      \column {
        \fontsize #-0.8
        \left-align {

        }
      }
      
       ""
      
      \column {
        \fontsize #-0.8
        \left-align {

        }
      }
    }
  }
}

% ===================
%    FICHIER MIDI
% ===================

\score
{
  \unfoldRepeats
  {
    \new StaffGroup
    <<     
      \new Staff
      {
        \set Staff.midiInstrument="accordion"
        \repeat unfold 1 \melody
      }
      \new Staff
      {
        \set Staff.midiInstrument="accordion"
        \repeat unfold 1 \bass
      }
    >>
  }
  \midi{}
}
