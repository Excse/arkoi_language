fun main() @s32:
L0:
  %01 @s32 = alloca
  store @s32 0, %01
  $03 @s32 = load %01
  ret $03

