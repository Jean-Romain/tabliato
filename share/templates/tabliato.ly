\version "2.20"

#(set-global-staff-size 20)

%=================
%   PARAMETRES
%=================

#(define cadb #<system>) %Cogeron f CADB t
#(define fingerDisplay #<fingerDisplay>)
#(define bassDisplay #<bassDisplay>)
#(define rankNumber <ranks>)

scoreIn = g
scoreOut = g

%=====================================
%   FONCTION D'ANALYSE PT et CADB
%=====================================

%% Definition d'une fonction scheme pour le calcul du rang du diato
%% nom rang
%% input x string button number i.e. a number followed by 0 1 or 2 symbol ' e.g. 3'
%% return 1, 2 or 3 refereing the rank of the button
#(define rang
    (lambda (x)
        (define sym "'")
        (define l (string-length x))	;longueur de la string
        (define rng 0)						;par défaut rang 1
        (if (> l 1) (if  (equal? (string-ref "'" 0) (string-ref x (- l 1))) (set! rng (+ rng 1 )) 0) 0)
        (if (> l 2) (if  (equal? (string-ref "'" 0) (string-ref x (- l 2))) (set! rng (+ rng 1 )) 0) 0)
        rng
     )
)

%% Définition d'une fonction pour retourner uniquement le numéro de la touche
%% sans les évetuelles apostrophes.
#(define touchnum
    (lambda (x)
        (define l (string-length x))
        (define n (rang x))
        (substring x 0 (- l (+ n)))
    )
)


make_cadb =
#(define-music-function (parser location button push) (string? number?)
(make-music
  'SequentialMusic
  'elements
  (list (make-music
  'SequentialMusic
  'elements
  (list 
       ; Reglage de la position haut/bas des spanners
       ; par rapport à la ligne centrale
       (make-music
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
            push
            'symbol
            'TextSpanner))
        ; Reglage fin de la position des spanners pour 
        ; s'aligner sur la ligne centrale
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
            (cons 0 (* -0.4 push))
            'bound-details
            (cons 0 (* -0.4 push))
            'symbol
            'TextSpanner))
         ; Reglage fin de la marge gauche des spanners. Si le bouton a 
         ; deux chiffres (10 11 12 ...) il faut laisser un peu plus de
         ; place que pour 3 4 5
        (make-music
          'ContextSpeccedMusic
          'context-type
          'Bottom
          'element
          (make-music
            'OverrideProperty
            'pop-first
            #t
            'grob-value
            1
            'grob-property-path
            (list 'bound-details
                  'left
                  'padding)
            'grob-value (if (> (string->number (touchnum button)) 9) 2.5 2)   
            'symbol
            'TextSpanner))
        )) 
        ; Ajout du markup
        (make-music
          'EventChord
          'elements
          (list (make-music
              'TextScriptEvent 'direction push
              'text ( markup #:fontsize -1.5 button))))
          ))
)

make_cogeron =
#(define-music-function (parser location button) (string?)
(if (not (eq? (rang button) 1)) ; si ce n'est pas le rang du milieux
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
            ;'pop-first #t
            'grob-property-path
            (list (quote direction))
            'grob-value (if (< (rang button) 1) -1 1)   
            'symbol
            'TextSpanner))
        (make-music
          'ContextSpeccedMusic
          'context-type
          'Bottom
          'element
          (make-music
            'OverrideProperty
            'pop-first
            #t
            'grob-value
            1
            'grob-property-path
            (list 'bound-details
                  'left
                  'padding)
            'grob-value (if (> (string->number (touchnum button)) 9) 2.5 2)   
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
            'grob-value (cons 0 0)
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
)

p = #(define-music-function (parser location button) (string?)
  (if cadb
    (make_cadb button 1)
    (make_cogeron button)
  )
)

t = #(define-music-function (parser location button) (string?)
  (if cadb
    (make_cadb button -1)
    (make_cogeron button)
  )
)

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
  title="<title>"
  subtitle="<subtitle>"
  composer="<composer>"
  instrument = "<instrument>"
  tagline = "<tagline>"
  poet="<poet>"
}


\markup { \vspace #2 }

\pointAndClickTypes #'(note-event post-event script-event text-script-event event)


%=====================
%	DATA
%=====================

diato =
{
  <tablature>
}

%=====================
%	PARTITION
%=====================

melody =
{
  \clef treble
  \tempo <tempo>
  \time <time>
  \key <key>

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
  <basses>
}

paroles = \lyricmode{ <firstVerse> }


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
  \override Script #'stencil = ##f
  \override DynamicText #'stencil = ##f
  
  \override Clef #'text = 
  #(if cadb
    #{ \markup{\raise #0.6 \column{P T}} #}
    (if (equal? rankNumber 3)
      #{ \markup{\raise #2.1 \column{3 2 1}} #}
      #{ \markup{\raise #0.6 \column{C G}} #}
    )
   )
                  
  \override TextSpanner #'style = #'solid-line
  \override TextSpanner.bound-details.left.padding = #2
  \override TextSpanner.bound-details.right.padding = #1
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
  \override TextSpanner.bound-details.left.padding = #2.5
  \override TextSpanner.bound-details.right.padding = #1
  \override TextSpanner #'to-barline = ##t
  \override TextSpanner #'staff-padding = #1.2
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
         \override LyricText #'font-name = #"Z003"
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
<leftLyrics>
        }
      }
      
       ""
      
      \column {
        \fontsize #-0.8
        \left-align {
<rightLyrics>
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
        \set Staff.midiInstrument="acoustic grand"
        \repeat unfold 1 \melody
      }
      \new Staff
      {
        \set Staff.midiInstrument="bright acoustic"
        \repeat unfold 1 \bass
      }
    >>
  }
  \midi{}
}
