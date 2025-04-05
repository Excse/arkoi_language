fun main(a @u32) @u32:
L0:
  %01 @u32 = alloca
  %02 @u32 = alloca
  store @u32 a, %02
  $03 @s32 = const 1
  $04 @u32 = cast @s32 $03
  $05 @s32 = const 2
  $06 @u32 = cast @s32 $05
  $07 @s32 = const 3
  $08 @u32 = cast @s32 $07
  $09 @s32 = const 4
  $10 @u32 = cast @s32 $09
  $11 @s32 = const 5
  $12 @u32 = cast @s32 $11
  $13 @s32 = const 6
  $14 @u32 = cast @s32 $13
  $15 @s32 = const 7
  $16 @u32 = cast @s32 $15
  $17 @s32 = const 8
  $18 @u32 = cast @s32 $17
  $19 @u64 = call calling_convention($04, $06, $08, $10, $12, $14, $16, $18)
  $20 @u32 = cast @u64 $19
  store @u32 $20, %01
  goto L1
L1:
  $21 @u32 = load %01
  ret $21

fun calling_convention(a @u32, b @u32, c @u32, d @u32, e @u32, f @u32, g @u32, h @u32) @u64:
L2:
  %01 @u64 = alloca
  %02 @u32 = alloca
  %03 @u32 = alloca
  %04 @u32 = alloca
  %05 @u32 = alloca
  %06 @u32 = alloca
  %07 @u32 = alloca
  %08 @u32 = alloca
  %09 @u32 = alloca
  store @u32 a, %02
  store @u32 b, %03
  store @u32 c, %04
  store @u32 d, %05
  store @u32 e, %06
  store @u32 f, %07
  store @u32 g, %08
  store @u32 h, %09
  $10 @bool = const 1
  $11 @u64 = cast @bool $10
  store @u64 $11, %01
  goto L3
L3:
  $12 @u64 = load %01
  ret $12

