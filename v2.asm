TEXTO EQU "hola pt"
      SUMA: PUSH BP
            MOV BP, SP
            PUSH EBX
            MOV EAX, 0
            MOV EBX, [BP+8]
SIGUE_SUMA: CMP EBX, 0
            JNP SALE_SUMA
            ADD EAX, EBX
            SUB EBX, 1
            JMP SIGUE_SUMA
SALE_SUMA:  POP EBX
            MOV SP, BP
            POP BP
            RET
      


_START: MOV AL, %01
        MOV EDX, DS
        MOV CL, 1
        MOV CH, 4
        SYS %1
        PUSH [EDX]
        CALL SUMA
        ADD SP, 4
        MOV [4], eax
        SYS %F
        MOV EDX, DS
        ADD EDX, 4
        MOV CL, 1
        MOV CH, 4
        MOV AL, 1
        SYS %2
        MOV EDX, KS
        ADD EDX, TEXTO
        SYS %4
        STOP