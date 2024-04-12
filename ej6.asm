        XOR EDX,EDX
        XOR EBX,EBX
        XOR ECX,ECX
        XOR EAX,EAX
        XOR [8],[8]
        MOV EDX,DS
        MOV CL,1
        MOV CH,4
        MOV AL,%01
        SYS %1
        MOV BX,[0]
        MOV EDX,DS
        ADD EDX,4
        SYS %1
        MOV CX,[4]
        MOV AX,0
        MOV [8],BX
OTRO:   SUB [8],CX
        CMP [8],0
        JNP FIN
        ADD AX,1
        JMP OTRO
FIN:    CMP [8],0
        JZ FINFIN
FINFIN: MOV EFX,EAX
        ADD EDX,4
        MOV CL,1
        MOV CH,4
        MOV AL,%01
        SYS %2
        MOV [EDX],EFX
        SYS %2
        STOP