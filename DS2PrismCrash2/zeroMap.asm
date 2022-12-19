.DATA

.CODE

zeroMap PROC

mov rsi, 0999h
mov r9, 0999h
mov [rsp+28h], rcx

nop
nop
nop
nop
nop
data:

ret
zeroMap ENDP

END