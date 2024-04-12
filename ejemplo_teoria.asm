		MOV EFX,0       
		MOV [4],0        
OTRO:	MOV EDX,DS                                                                                                                       
		MOV CL,1                  
		MOV CH,4       
		MOV AL,%01                     
		SYS %1                                  
		CMP [0],0       
		JN SALE          
		ADD [4],[0]    
		ADD EFX,1       
		JMP OTRO         
SALE:	CMP EFX,0      
		JZ FIN             
		DIV [4],EFX   
FIN:	MOV EDX,DS
		ADD EDX,4 
		MOV CL,1
		MOV CH,4
		MOV AL,%01
		SYS %2
		STOP