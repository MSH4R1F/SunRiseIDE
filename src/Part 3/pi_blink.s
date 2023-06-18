movz w0, #0x3f00, lsl #16
movk w0, #0xb880

movz w2, #0x8000, lsl #16
movz w6, #0x4000, lsl #16
movz w5, #0x0008, lsl #16

movz w3, #0x0ac0, lsl #16

wait_write:
    ldr w1, [w0, #0x38]
    tst w1, w2
    b.ne wait_write

write_buffer:
    movz w4, #0x20
    str w4, [w3, #0]

    movz w4, #0x0
    str w4, [w3, #4]

    movz w4, #0x0003, lsl #16
    movk w4, #0x8041
    str w4, [w3, #8]

    movz w4, #0x8
    str w4, [w3, #12]

    movz w4, #0x0
    str w4, [w3, #16]

    movz w4, #0x82
    str w4, [w3, #20]

    movz w4, #0x1
    str w4, [w3, #24]

    movz w4, #0x0
    str w4, [w3, #28]

    add w3, w3, #8

    str w3, [w0, #0x20]

