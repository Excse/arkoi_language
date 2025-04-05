fun main(a @u32) @u32:
L0:
  %01 @u32 = alloca
  $19 @u64 = call calling_convention(1, 2, 3, 4, 5, 6, 7, 8)
  $20 @u32 = cast @u64 $19
  store @u32 $20, %01
  $21 @u32 = load %01
  ret $21

fun calling_convention(a @u32, b @u32, c @u32, d @u32, e @u32, f @u32, g @u32, h @u32) @u64:
L2:
  %01 @u64 = alloca
  store @u64 1, %01
  $12 @u64 = load %01
  ret $12

