extern movtact
extern swaptact


global swap
swap:
    push ebp
    mov ebp, esp

    mov edx, dword[ebp+8]
    mov ecx, dword[ebp+12]

    fld qword[edx]
    fld qword[ecx]

    fstp qword[edx]
    fstp qword[ecx]

    inc dword[swaptact]

    mov esp, ebp
    pop ebp
    ret

global isl
isl:
    push ebp
    mov ebp, esp
    fld qword [ebp+16]
    fld qword [ebp+8]

    fcomip ;st0 vs. st1
    ffreep
    setb al
    movzx ecx, al
    inc dword[movtact]

    mov eax, ecx
    mov esp, ebp
    pop ebp
    ret

global timestamp
timestamp:
    rdtsc
    ret
