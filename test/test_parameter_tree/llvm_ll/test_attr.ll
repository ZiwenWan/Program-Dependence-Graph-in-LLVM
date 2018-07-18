; ModuleID = 'test_attr.c'
source_filename = "test_attr.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define i32 @f2(i32* %a) #0 {
entry:
  %a.addr = alloca i32*, align 8
  store i32* %a, i32** %a.addr, align 8
  %0 = load i32*, i32** %a.addr, align 8
  %1 = load i32, i32* %0, align 4
  %add = add nsw i32 %1, 16
  %2 = load i32*, i32** %a.addr, align 8
  %3 = ptrtoint i32* %2 to i32
  %sub = sub nsw i32 %add, %3
  %or = or i32 %sub, 4
  ret i32 %or
}

; Function Attrs: noinline nounwind optnone uwtable
define i32 @f1(i32* %a, i32* %b) #0 {
entry:
  %a.addr = alloca i32*, align 8
  %b.addr = alloca i32*, align 8
  %c = alloca i32, align 4
  store i32* %a, i32** %a.addr, align 8
  store i32* %b, i32** %b.addr, align 8
  %0 = load i32*, i32** %b.addr, align 8
  %1 = load i32, i32* %0, align 4
  %call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %1)
  %2 = load i32*, i32** %a.addr, align 8
  %call1 = call i32 @f2(i32* %2)
  %3 = load i32*, i32** %b.addr, align 8
  %call2 = call i32 @f2(i32* %3)
  %sub = sub nsw i32 %call1, %call2
  store i32 %sub, i32* %c, align 4
  %4 = load i32*, i32** %b.addr, align 8
  %call3 = call i32 @f2(i32* %4)
  %call4 = call i32 @f2(i32* %c)
  %mul = mul nsw i32 %call3, %call4
  ret i32 %mul
}

declare i32 @printf(i8*, ...) #1

; Function Attrs: noinline nounwind optnone uwtable
define i32 @main() #0 {
entry:
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  store i32 4, i32* %a, align 4
  store i32 5, i32* %b, align 4
  %call = call i32 @f1(i32* %a, i32* %b)
  ret i32 0
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 7.0.0 (trunk 323988) (llvm/trunk 323938)"}
