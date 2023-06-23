movz w0, #0x3f00, lsl #16
movk w0, #0xb880

movz w2, #0x8000, lsl #16
movz w6, #0x4000, lsl #16
movz w5, #0x0008, lsl #16

on_wait_write:
    ldr w1, [w0, #0x38]
    tst w1, w2
    b.ne on_wait_write

on_write_buffer:
    movz w3, #0x0ac0, lsl #16

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

    movz w4, #130
    str w4, [w3, #20]

    movz w4, #0x1
    str w4, [w3, #24]

    movz w4, #0x0
    str w4, [w3, #28]

    add w3, w3, #8

    str w3, [w0, #0x20]

on_wait_read:
    ldr w1, [w0, #0x18]
    tst w1, w6
    b.ne on_wait_read

on_read_response:
    ldr w1, [w0, #0]

movz w29, #0x049, lsl #16
on_light_delay:
    sub w29, w29, #1
    cmp w29, #0
    b.ne on_light_delay

off_wait_write:
    ldr w1, [w0, #0x38]
    tst w1, w2
    b.ne off_wait_write

off_write_buffer:
    movz w3, #0x0ac0, lsl #16

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

    movz w4, #130
    str w4, [w3, #20]

    movz w4, #0x0
    str w4, [w3, #24]

    movz w4, #0x0
    str w4, [w3, #28]

    add w3, w3, #8

    str w3, [w0, #0x20]

off_wait_read:
    ldr w1, [w0, #0x18]
    tst w1, w6
    b.ne off_wait_read

ldr w1, [w0, #0]

movz w29, #0x049, lsl #16
off_light_delay:
    sub w29, w29, #1
    cmp w29, #0
    b.ne off_light_delay

b on_wait_write