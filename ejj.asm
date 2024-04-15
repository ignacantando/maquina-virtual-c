    MOV [4],1  
    MOV EDX,DS
    ADD EDX,4
	MOV AL,1
	MOV CL,1
	MOV CH,4
	SYS %2
    STOPw