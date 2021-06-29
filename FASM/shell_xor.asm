use32
xor ebx,ebx
mov ecx, 0xfffffda7
xor ecx, 0xffffffff
metka:  
  mov eax,[esp+26+ebx*4]
  xor eax,0x11111111
  mov [esp+26+ebx*4], eax
  inc ebx
  loop metka