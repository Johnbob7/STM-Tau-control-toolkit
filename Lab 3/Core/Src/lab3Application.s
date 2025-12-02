.syntax unified
.thumb

.global start_app

.type start_app, %function

.section .text
.align 2
start_app:
    /* Force PA8, PA9, PA10, PA11 ON */
    LDR r0, =0x40020014        /* GPIOA_ODR address */
    LDR r1, [r0]               /* read current ODR */
    ORR r1, r1, #(0xF << 8)    /* set bits 8,9,10,11 */
    STR r1, [r0]               /* write back */

    /* Force PB10, PB11 ON (if you use them instead of PA9/PA10) */
    LDR r0, =0x40020414        /* GPIOB_ODR address */
    LDR r1, [r0]
    ORR r1, r1, #(0x3 << 10)   /* set bits 10,11 */
    STR r1, [r0]

forever:
    B forever
