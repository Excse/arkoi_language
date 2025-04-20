fun main() @s32:
L0:
  %01 @s32 = alloca
  $02 @s32 = const 0
  store @s32 $02, %01
  goto L1
L1:
  $03 @s32 = load %01
  ret $03

