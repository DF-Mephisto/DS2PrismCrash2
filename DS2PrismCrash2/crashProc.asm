.DATA

.CODE

crashProc PROC

push rax
mov rax,rsp
and rsp,0FFFFFFFFFFFFFFF0h
sub rsp,2A0h

fxsave qword ptr [rsp+20h]
mov [rsp+220h],rbx
mov [rsp+228h],rcx
mov [rsp+230h],rdx
mov [rsp+238h],rsi
mov [rsp+240h],rdi
mov [rsp+248h],rax
mov [rsp+250h],rbp
mov [rsp+258h],r8
mov [rsp+260h],r9
mov [rsp+268h],r10
mov [rsp+270h],r11
mov [rsp+278h],r12
mov [rsp+280h],r13
mov [rsp+288h],r14
mov [rsp+290h],r15

mov rdx, qword ptr [data]
add rdx, 1E5090h
mov qword ptr [data+16], rdx

mov rdx, qword ptr [data+8]
call qword ptr [data+16]

mov r15,[rsp+290h]
mov r14,[rsp+288h]
mov r13,[rsp+280h]
mov r12,[rsp+278h]
mov r11,[rsp+270h]
mov r10,[rsp+268h]
mov r9,[rsp+260h]
mov r8,[rsp+258h]
mov rbp,[rsp+250h]
mov rdi,[rsp+240h]
mov rsi,[rsp+238h]
mov rdx,[rsp+230h]
mov rcx,[rsp+228h]
mov rbx,[rsp+220h]
fxrstor qword ptr [rsp+20h]
mov rsp,[rsp+248h]
pop rax

comiss xmm0,dword ptr [rdx+1ACh]

nop
nop
nop
nop
nop
data:

ret
crashProc ENDP

END