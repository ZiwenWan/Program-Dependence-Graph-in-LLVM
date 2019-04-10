; ModuleID = 'test_two_source_data_dependency.c'
source_filename = "test_two_source_data_dependency.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

%struct.struct_t = type { i32, float }

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @getFlag() #0 {
  ret i32 1
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca %struct.struct_t, align 4
  %3 = alloca i32, align 4
  %4 = alloca float, align 4
  store i32 0, i32* %1, align 4
  store i32 5, i32* %3, align 4
  store float 6.000000e+00, float* %4, align 4
  %5 = call i32 @getFlag()
  %6 = icmp ne i32 %5, 0
  br i1 %6, label %7, label %10

; <label>:7:                                      ; preds = %0
  %8 = load i32, i32* %3, align 4
  %9 = getelementptr inbounds %struct.struct_t, %struct.struct_t* %2, i32 0, i32 0
  store i32 %8, i32* %9, align 4
  br label %13

; <label>:10:                                     ; preds = %0
  %11 = load float, float* %4, align 4
  %12 = getelementptr inbounds %struct.struct_t, %struct.struct_t* %2, i32 0, i32 1
  store float %11, float* %12, align 4
  br label %13

; <label>:13:                                     ; preds = %10, %7
  ret i32 0
}

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{!"clang version 5.0.2 (tags/RELEASE_502/final)"}
