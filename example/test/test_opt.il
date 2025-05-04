fun main() @u64:
L0:
  %01 @u64 = alloca
  $06 @bool = call ok(5)
  $07 @u32 = cast @bool $06
  $08 @u32 = mul @u32 1, $07
  $11 @u32 = add @u32 $08, 1
  $12 @s32 = cast @u32 $11
  $15 @f32 = call test1($12, 10.5)
  $17 @f32 = mul @f32 $15, 2.01
  $20 @f32 = sub @f32 $17, 42
  $21 @u64 = cast @f32 $20
  store @u64 $21, %01
  $22 @u64 = load %01
  ret $22

fun ok(foo1 @f64) @bool:
L2:
  %01 @bool = alloca
  %02 @f64 = alloca
  store @f64 foo1, %02
  $03 @f64 = load %02
  $06 @bool = gth @f64 $03, 5
  if $06 then L4 else L5
L5:
  $09 @f64 = load %02
  $12 @bool = gth @f64 $09, 10
  if $12 then L7 else L8
L8:
  store @f64 21, %02
  goto L6
L7:
  store @f64 20, %02
  goto L6
L4:
  store @f64 0, %02
  goto L6
L6:
  $19 @s32 = div @s32 4, 2
  $20 @f64 = load %02
  $21 @f32 = call test2($19, $20)
  $22 @bool = cast @f32 $21
  store @bool $22, %01
  $23 @bool = load %01
  ret $23

fun test1(foo2 @s32, bar @f64) @f32:
L10:
  %01 @f32 = alloca
  %02 @s32 = alloca
  %03 @f64 = alloca
  store @s32 foo2, %02
  store @f64 bar, %03
  %04 @f32 = alloca
  store @f32 0, %04
  $06 @s32 = load %02
  $07 @f64 = cast @s32 $06
  $08 @f64 = load %03
  $09 @bool = lth @f64 $07, $08
  if $09 then L12 else L13
L13:
  $20 @s32 = load %02
  $21 @f64 = cast @s32 $20
  $22 @f64 = load %03
  $23 @f64 = mul @f64 $21, $22
  $24 @f32 = cast @f64 $23
  store @f32 $24, %04
  goto L14
L12:
  $10 @f64 = load %03
  $11 @s32 = load %02
  $12 @f64 = cast @s32 $11
  $13 @f64 = mul @f64 $10, $12
  $14 @s32 = load %02
  $15 @s32 = load %02
  $16 @bool = lth @s32 $14, $15
  $17 @f64 = cast @bool $16
  $18 @f64 = add @f64 $13, $17
  $19 @f32 = cast @f64 $18
  store @f32 $19, %04
  goto L14
L14:
  $25 @f32 = load %04
  store @f32 $25, %01
  $26 @f32 = load %01
  ret $26

fun test2(foo2 @s32, bar @f64) @f32:
L15:
  %01 @f32 = alloca
  %02 @s32 = alloca
  %03 @f64 = alloca
  store @s32 foo2, %02
  store @f64 bar, %03
  $04 @s32 = load %02
  $05 @f64 = cast @s32 $04
  $06 @f64 = load %03
  $07 @bool = lth @f64 $05, $06
  if $07 then L17 else L18
L18:
  $13 @s32 = load %02
  $14 @f64 = cast @s32 $13
  $15 @f64 = load %03
  $16 @f64 = mul @f64 $14, $15
  store @f64 $16, %03
  $17 @f64 = load %03
  $18 @f32 = cast @f64 $17
  store @f32 $18, %01
  goto L16
L17:
  $08 @f64 = load %03
  $09 @s32 = load %02
  $10 @f64 = cast @s32 $09
  $11 @f64 = mul @f64 $08, $10
  $12 @f32 = cast @f64 $11
  store @f32 $12, %01
  goto L16
L16:
  $19 @f32 = load %01
  ret $19

