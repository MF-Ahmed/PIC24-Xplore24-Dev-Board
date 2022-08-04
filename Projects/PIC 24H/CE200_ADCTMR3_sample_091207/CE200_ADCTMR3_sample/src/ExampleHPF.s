; ..............................................................................
;    File   ExampleHPF.s
; ..............................................................................

                .equ ExampleHPFNumSections, 5

; ..............................................................................
;
; Allocate and initialize filter coefficients
;
; These coefficients have been designed for use in the Transpose filter only

                .section .xdata, xmemory, data  ;THIS line was modified
                                                ;to be compatible with C30 v1.3x

ExampleHPFCoefs:
.hword  0x2392  ; b( 1,0)/2
.hword  0xB8FF  ; b( 1,1)/2
.hword  0x3D5F  ; a( 1,1)/2
.hword  0x2392  ; b( 1,2)/2
.hword  0xEF39  ; a( 1,2)/2
.hword  0x2FBB  ; b( 2,0)/2
.hword  0xA1F2  ; b( 2,1)/2
.hword  0x579D  ; a( 2,1)/2
.hword  0x2FBB  ; b( 2,2)/2
.hword  0xDA17  ; a( 2,2)/2
.hword  0x3818  ; b( 3,0)/2
.hword  0x9344  ; b( 3,1)/2
.hword  0x68F9  ; a( 3,1)/2
.hword  0x3818  ; b( 3,2)/2
.hword  0xCC0D  ; a( 3,2)/2
.hword  0x3C51  ; b( 4,0)/2
.hword  0x8C80  ; b( 4,1)/2
.hword  0x7150  ; a( 4,1)/2
.hword  0x3C51  ; b( 4,2)/2
.hword  0xC52E  ; a( 4,2)/2
.hword  0x3E73  ; b( 5,0)/2
.hword  0x8920  ; b( 5,1)/2
.hword  0x7583  ; a( 5,1)/2
.hword  0x3E73  ; b( 5,2)/2
.hword  0xC175  ; a( 5,2)/2

; ..............................................................................
; Allocate states buffers in (uninitialized) Y data space

                .section .ybss, ymemory, bss

ExampleHPFStates1:
                .space ExampleHPFNumSections*2

ExampleHPFStates2:
                .space ExampleHPFNumSections*2

; ..............................................................................
; Allocate and intialize filter structure

                .section .data
                .global _ExampleHPFFilter

_ExampleHPFFilter:
.hword ExampleHPFNumSections-1
.hword ExampleHPFCoefs
.hword 0xFF00
.hword ExampleHPFStates1
.hword ExampleHPFStates2
.hword 0x0000

; ..............................................................................
; Sample assembly language calling program
;  The following declarations can be cut and pasted as needed into a program
;               .extern _IIRTransposeFilterInit
;               .extern _BlockIIRTransposeFilter
;               .extern _ExampleHPFFilter
;
;               .section        .bss
;
;        The input and output buffers can be made any desired size
;          the value 40 is just an example - however, one must ensure
;          that the output buffer is at least as long as the number of samples
;          to be filtered (parameter 4)
;input:         .space  40
;output:        .space  40
;               .text
;
;
;  This code can be copied and pasted as needed into a program
;
;
; Set up pointers to access input samples, filter taps, delay line and
; output samples.
;               mov     #_ExampleHPFFilter, W0  ; Initalize W0 to filter structure
;               call    _IIRTransposeFilterInit ; call this function once
;
; The next 4 instructions are required prior to each subroutine call
; to _BlockIIRTransposeFilter
;               mov     #_ExampleHPFFilter, W0  ; Initalize W0 to filter structure
;               mov     #input, W1      ; Initalize W1 to input buffer
;               mov     #output, W2     ; Initalize W2 to output buffer
;               mov     #20, W3 ; Initialize W3 with number of required output samples
;               call    _BlockIIRTransposeFilter        ; call as many times as needed
