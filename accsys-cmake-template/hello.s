.text
main:
    # Read Input to a1
    li a0, 6
    ecall
    mv  a1, a0
    call factorial
    # Print Result in a1
    mv a1, a0
    li a0, 1
    ecall
    # Exit with value = 0
    addi a0, x0, 10
    ecall

factorial:
    mv t0, a0 # Store loop counter in t0
    li t1, 1 # Store result in t1
factorial_loop:
    mul t1, t1, t0
    addi t0, t0, -1
    beqz t0, factorial_end
    j factorial_loop
factorial_end:
    mv a0, t1
    ret