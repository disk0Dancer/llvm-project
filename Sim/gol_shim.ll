target triple = "graph"

@rng_state = internal global i32 1

declare void @llvm.graph.init(i32, i32)
declare void @llvm.graph.set.color(i32)
declare void @llvm.graph.draw.pixel(i32, i32)
declare void @llvm.graph.flush()

declare void @app()

define void @simPutPixel(i32 %x, i32 %y, i32 %c) #0 {
  call void @llvm.graph.set.color(i32 %c)
  call void @llvm.graph.draw.pixel(i32 %x, i32 %y)
  ret void
}

define void @simFlush() #0 {
  call void @llvm.graph.flush()
  ret void
}

define i32 @simRand() #0 {
  %s = load i32, ptr @rng_state
  %m = mul i32 %s, 1103515245
  %a = add i32 %m, 12345
  store i32 %a, ptr @rng_state
  %r = and i32 %a, 2147483647
  ret i32 %r
}

define void @simAssert(i32 %a, i32 %b) #0 {
  ret void
}

define i32 @main() {
  call void @llvm.graph.init(i32 512, i32 512)
  call void @app()
  ret i32 0
}

attributes #0 = { alwaysinline }
