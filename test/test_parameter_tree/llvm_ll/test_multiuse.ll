; ModuleID = 'test_multiuse.c'
source_filename = "test_multiuse.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@.str.1 = private unnamed_addr constant [8 x i8] c"%d + %f\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define void @use1(i32* %addr) #0 {
entry:
  %addr.addr = alloca i32*, align 8
  store i32* %addr, i32** %addr.addr, align 8
  %0 = load i32*, i32** %addr.addr, align 8
  %1 = load i32, i32* %0, align 4
  %call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %1)
  ret void
}

declare i32 @printf(i8*, ...) #1

; Function Attrs: noinline nounwind optnone uwtable
define void @use2(i32* %addr) #0 {
entry:
  %addr.addr = alloca i32*, align 8
  store i32* %addr, i32** %addr.addr, align 8
  %0 = load i32*, i32** %addr.addr, align 8
  %arrayidx = getelementptr inbounds i32, i32* %0, i64 1
  %1 = load i32, i32* %arrayidx, align 4
  %call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %1)
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define void @multiuse(i32* %a1, i32* %a2) #0 {
entry:
  %a1.addr = alloca i32*, align 8
  %a2.addr = alloca i32*, align 8
  store i32* %a1, i32** %a1.addr, align 8
  store i32* %a2, i32** %a2.addr, align 8
  %0 = load i32*, i32** %a1.addr, align 8
  call void @use1(i32* %0)
  %1 = load i32*, i32** %a2.addr, align 8
  call void @use2(i32* %1)
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define void @f(i32 %a, float %b) #0 {
entry:
  %a.addr = alloca i32, align 4
  %b.addr = alloca float, align 4
  store i32 %a, i32* %a.addr, align 4
  store float %b, float* %b.addr, align 4
  %0 = load i32, i32* %a.addr, align 4
  %1 = load float, float* %b.addr, align 4
  %conv = fpext float %1 to double
  %call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([8 x i8], [8 x i8]* @.str.1, i32 0, i32 0), i32 %0, double %conv)
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define i32 @main() #0 {
entry:
  %heap = alloca i32*, align 8
  %local1 = alloca i32, align 4
  %local2 = alloca i32, align 4
  %flt = alloca float, align 4
  store i32 7, i32* %local1, align 4
  store i32 8, i32* %local2, align 4
  store float 0x401154FE00000000, float* %flt, align 4
  %0 = load i32, i32* %local1, align 4
  %1 = load float, float* %flt, align 4
  call void @f(i32 %0, float %1)
  %call = call noalias i8* @malloc(i64 16) #3
  %2 = bitcast i8* %call to i32*
  store i32* %2, i32** %heap, align 8
  call void @multiuse(i32* %local2, i32* %local1)
  %3 = load i32*, i32** %heap, align 8
  call void @multiuse(i32* %3, i32* %local1)
  ret i32 0
}

; Function Attrs: nounwind
declare noalias i8* @malloc(i64) #2

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 7.0.0 (trunk 323988) (llvm/trunk 323938)"}
