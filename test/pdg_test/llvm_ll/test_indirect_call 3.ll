; ModuleID = 'test_indirect_call.c'
source_filename = "test_indirect_call.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

%struct.A = type { void (i32*)*, i32 }

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @f1(i32*) #0 {
  %2 = alloca i32*, align 8
  store i32* %0, i32** %2, align 8
  %3 = load i32*, i32** %2, align 8
  %4 = load i32, i32* %3, align 4
  %5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %4)
  ret void
}

declare i32 @printf(i8*, ...) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @f(%struct.A*) #0 {
  %2 = alloca %struct.A*, align 8
  store %struct.A* %0, %struct.A** %2, align 8
  %3 = load %struct.A*, %struct.A** %2, align 8
  %4 = getelementptr inbounds %struct.A, %struct.A* %3, i32 0, i32 0
  store void (i32*)* @f1, void (i32*)** %4, align 8
  %5 = load %struct.A*, %struct.A** %2, align 8
  %6 = getelementptr inbounds %struct.A, %struct.A* %5, i32 0, i32 0
  %7 = load void (i32*)*, void (i32*)** %6, align 8
  %8 = load %struct.A*, %struct.A** %2, align 8
  %9 = getelementptr inbounds %struct.A, %struct.A* %8, i32 0, i32 1
  call void %7(i32* %9)
  ret i32 0
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 {
  ret i32 0
}

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{!"clang version 5.0.2 (tags/RELEASE_502/final)"}
